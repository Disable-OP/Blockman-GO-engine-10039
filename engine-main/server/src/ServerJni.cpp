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
std::atomic<bool>             g_serverStopFlag{false};  // passed by ref to Server::tick()
std::string                   g_serverWorkDir;
int                           g_serverPort = 19130;

// The server thread boots Server with a synthesised standalone config:
//   - DISABLE_ROOM=1  (no external room server)
//   - All HTTP endpoints stubbed to localhost:1 (server runs without any
//     external HTTP services — connection refused, requests fail fast)
//   - World seed: fixed default (override via nativeServerStart's seed arg)
//   - Map dir: <workDir>/world
void serverThreadMain()
{
        LOGI("Server thread starting; port=%d workDir=%s", g_serverPort, g_serverWorkDir.c_str());

        try {
                RoomGameConfig cfg;
                cfg.gameId = "g_local";
                cfg.gameName = "blockman-go-local";
                cfg.gameIp = "0.0.0.0";
                cfg.serverPort = g_serverPort;
                cfg.monitorAddr = "";                 // empty → RoomClient is skipped (DISABLE_ROOM)
                cfg.gameType = "";
                cfg.testGameDataDir = g_serverWorkDir + "/game";
                cfg.testScriptDir = "";                // no Lua scripts in standalone mode
                cfg.testScriptCommonDir = "";
                cfg.maxPlayers = 8;
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

                Server server;
                server.init(cfg);
                server.start();
                g_serverRunning = true;
                LOGI("Server running on 0.0.0.0:%d", g_serverPort);

                // tick() blocks until g_serverStopFlag becomes true.
                g_serverStopFlag = false;
                server.tick(g_serverStopFlag);

                LOGI("Server tick loop exited; shutting down...");
                server.stopThread();
                server.unInit();
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

        // seed is currently ignored — ServerWorld uses its own test_rand array.
        // Wiring a custom seed through to WorldSettings is a future task.
        (void)seed;

        g_serverThread = std::make_unique<std::thread>(serverThreadMain);
        g_serverThread->detach();
}

// Stop the server. The tick loop checks the stop flag once per tick (~50ms),
// so shutdown completes within one tick.
JNIEXPORT void JNICALL
Java_com_sandboxol_blockmango_ServerService_nativeServerStop(JNIEnv*, jclass)
{
        if (!g_serverRunning.load()) {
                return;
        }
        g_serverStopFlag.store(true, std::memory_order_release);
}

// Check whether the server is currently running.
JNIEXPORT jboolean JNICALL
Java_com_sandboxol_blockmango_ServerService_nativeServerIsRunning(JNIEnv*, jclass)
{
        return g_serverRunning.load() ? JNI_TRUE : JNI_FALSE;
}

} // extern "C"

#endif  // LORD_PLATFORM_ANDROID
