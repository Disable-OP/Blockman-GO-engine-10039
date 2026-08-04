/********************************************************************
filename:       ServerJni.cpp
file path:      engine-main/server/src/ServerJni.cpp

version:        1
author:         Blockman-GO-engine-10039 contributors
date:           2026-08-03

purpose:        Android JNI entry point for the in-process game server.

            On Android, the server is NOT a standalone executable —
            Android does not host `main()` binaries. Instead, the
            server is compiled into a shared library (libGameServer.so)
            that the Java side loads and starts via JNI. The server
            runs in a background thread; the client (in the same APK,
            in libBlockMan.so) connects to 127.0.0.1:19130 over RakNet
            UDP, exactly as it would to a remote server.

            This file is only compiled when LORD_PLATFORM_ANDROID is
            defined. On Linux/Win32, the server's main.cpp is the
            entry point instead.

            Lifecycle:
              Java: ServerService.nativeServerStart(port)  →  C++: startServerThread(port)
              Java: ServerService.nativeServerStop()        →  C++: stopServerThread()
              Java: ServerService.nativeServerIsRunning()   →  C++: isServerRunning()
*********************************************************************/
#if LORD_PLATFORM == LORD_PLATFORM_ANDROID

#include "Server.h"
#include "Global.h"
#include "Util/UThreadBase.h"   // StopFlag = const std::atomic_bool&

#include <jni.h>
#include <android/log.h>
#include <thread>
#include <pthread.h>
#include <atomic>
#include <memory>
#include <string>
#include <cstring>
#include <chrono>

#define LOG_TAG "BlockmanServer"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,  LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

namespace {

std::unique_ptr<std::thread> g_serverThread;
std::atomic<bool>             g_serverRunning{false};
std::string                   g_serverWorkDir;
int                           g_serverPort = 19130;

// Anonymous-namespace holder for the seed the Java side handed us.
// Set by nativeServerStart, read by serverThreadMain.
std::atomic<int64_t> g_serverWorldSeed{0};
int                  g_serverWorldType = 100;  // TERRAIN_TYPE_CUSTOM = sky islands

// The server thread boots Server with a synthesised standalone config:
//   - DISABLE_ROOM=1  (no external room server)
//   - All HTTP endpoints stubbed to localhost:1 (server runs without any
//     external HTTP services — connection refused, requests fail fast)
//   - World seed: from nativeServerStart's `seed` arg (or default if 0)
//   - World type: TERRAIN_TYPE_CUSTOM (sky islands) by default
//   - Map dir: <workDir>/world
void serverThreadMain()
{
        LOGI("Server thread starting; port=%d workDir=%s seed=%lld",
                g_serverPort, g_serverWorkDir.c_str(), (long long)g_serverWorldSeed.load());

        try {
                RoomGameConfig cfg;
                cfg.gameId = "g_local";
                cfg.gameName = "blockman-go-local";
                cfg.gameIp = "0.0.0.0";
                cfg.serverPort = g_serverPort;
                cfg.monitorAddr = "";                 // empty → RoomClient is skipped (DISABLE_ROOM)
                cfg.gameType = "";
                cfg.testGameDataDir = g_serverWorkDir;
                cfg.mapDir = g_serverWorkDir;
                cfg.logDir = g_serverWorkDir;
                cfg.testScriptDir = "";                // no Lua scripts in standalone mode
                cfg.testScriptCommonDir = "";
                cfg.maxPlayers = 1;
                cfg.mapID = "";
                cfg.userConfig = "users.json";
                cfg.propAddr = "http://127.0.0.1:1";   // stub: connection refused
                cfg.rankAddr = "http://127.0.0.1:1";
                cfg.rewardAddr = "http://127.0.0.1:1";
                cfg.secret = "blockman-go-local-secret";  // login token salt (must match client)
                cfg.isDebug = true;
                cfg.isChina = false;
                cfg.blockymodsUrl = "http://127.0.0.1:1";
                cfg.blockmanUrl = "http://127.0.0.1:1";
                cfg.blockymodsRewardAddr = "http://127.0.0.1:1";
                cfg.dbIp = "";
                cfg.dbUsername = "";
                cfg.dbPassword = "";
                cfg.dbName = "";
                cfg.dbHttpServiceUrl = "http://127.0.0.1:1";  // stub HTTP DB proxy
                cfg.redisDbIp = "";
                cfg.redisPort = 0;
                cfg.redisDbPassword = "";

                // Custom world generation: pass the Java-supplied seed through
                // to WorldSettings. Default type = sky islands (TERRAIN_TYPE_CUSTOM=100).
                cfg.worldSeed = g_serverWorldSeed.load();
                cfg.worldType = g_serverWorldType;

                Server* server = new Server();
                server->init(cfg);
                server->start();
                g_serverRunning = true;
                LOGI("Server running on 0.0.0.0:%d", g_serverPort);
                server->waitForStopEvent();
                LOGI("Server stop event received; shutting down...");
                server->stopThread();
                server->unInit();
                delete server;
                g_serverRunning = false;
                LOGI("Server shut down cleanly");
        } catch (const std::exception& e) {
                LOGE("Server thread crashed: %s", e.what());
                g_serverRunning = false;
        } catch (...) {
                LOGE("Server thread crashed: unknown exception");
                g_serverRunning = false;
        }
}

} // anonymous namespace


// -----------------------------------------------------------------------------
// JNI exports — called from Java via ServerService (see
// client/Shells/Android/Blockmango/app/src/main/java/com/sandboxol/blockmango/ServerService.java)
// -----------------------------------------------------------------------------

extern "C" {

// Start the server in a background thread.
//   port:     UDP port to bind (0 → default 19130)
//   workDir:  Android filesDir; world data lives under <workDir>/world
//   seed:     world seed (0 → use default test seed inside ServerWorld)
JNIEXPORT void JNICALL
Java_com_sandboxol_blockmango_ServerService_nativeServerStart(
        JNIEnv* env, jclass, jint port, jstring workDir, jlong seed)
{
        if (g_serverRunning.load()) {
                LOGW("nativeServerStart: server already running");
                return;
        }

        g_serverPort = port ? port : 19130;
        const char* wd = env->GetStringUTFChars(workDir, nullptr);
        g_serverWorkDir = wd ? wd : "/data/data/com.sandboxol.blockmango/files";
        env->ReleaseStringUTFChars(workDir, wd);

        // Stash the seed for the server thread. 0 → use default test seed
        // inside ServerWorld (preserves the old behaviour for callers that
        // don't supply one).
        g_serverWorldSeed.store((int64_t)seed);

        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setstacksize(&attr, 8 * 1024 * 1024);
        pthread_t pthread_handle;
        int rc = pthread_create(&pthread_handle, &attr,
            [](void*) -> void* { serverThreadMain(); return nullptr; }, nullptr);
        pthread_attr_destroy(&attr);
        if (rc != 0) {
            LOGE("pthread_create failed (rc=%d)", rc);
            g_serverThread = std::unique_ptr<std::thread>(new std::thread(serverThreadMain));
            g_serverThread->detach();
        } else {
            pthread_detach(pthread_handle);
        }
}

// Stop the server. Calls Server::setStopEvent() which unblocks
// waitForStopEvent(), allowing the server thread to exit cleanly.
JNIEXPORT void JNICALL
Java_com_sandboxol_blockmango_ServerService_nativeServerStop(JNIEnv*, jclass)
{
        if (!g_serverRunning.load()) {
                return;
        }
        // Signal the server's stop event. The server thread's waitForStopEvent()
        // will return, then the thread cleans up and exits.
        // Note: we don't have a direct pointer to the Server instance here, but
        // Server is a Singleton — we can get it via Server::Instance().
        if (Server::Instance()) {
                Server::Instance()->setStopEvent();
        }
}

// Check whether the server is currently running.
JNIEXPORT jboolean JNICALL
Java_com_sandboxol_blockmango_ServerService_nativeServerIsRunning(JNIEnv*, jclass)
{
        return g_serverRunning.load() ? JNI_TRUE : JNI_FALSE;
}

} // extern "C"

#endif  // LORD_PLATFORM_ANDROID
