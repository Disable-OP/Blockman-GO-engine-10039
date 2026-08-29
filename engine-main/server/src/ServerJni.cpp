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

            Startup state machine (fixes the double-start crash):
              IDLE -> STARTING -> RUNNING -> (stop) -> IDLE
              nativeServerStart atomically flips IDLE->STARTING; any call
              while STARTING or RUNNING is a no-op. This matters because
              Server::init() takes seconds (engine init + block registry +
              world spawn generation), and `new Server()` twice would trip
              LordAssert(!ms_pSingleton) → SIGABRT → app crash. The Reset
              button / activity re-init used to hit exactly that path.
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
#include <mutex>
#include <memory>
#include <string>
#include <cstring>
#include <chrono>

#define LOG_TAG "BlockmanServer"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,  LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

namespace {

enum class ServerState
{
        IDLE,      // no server thread alive
        STARTING,  // thread spawned, Server::init() in progress (socket not bound yet)
        RUNNING,   // init + start() done — RakNet socket bound, accepting connections
};

std::mutex              g_stateMutex;
ServerState             g_serverState = ServerState::IDLE;
std::string             g_serverWorkDir;
int                     g_serverPort = 19130;

// Guarded by g_stateMutex. The Server instance is owned by the server
// thread; the pointer is only read under the mutex for stop requests.
Server*                 g_serverInstance = nullptr;

// Anonymous-namespace holder for the seed the Java side handed us.
// Set by nativeServerStart, read by serverThreadMain.
std::atomic<int64_t> g_serverWorldSeed{0};
int                  g_serverWorldType = 0;  // 0 = TERRAIN_TYPE_DEFAULT (vanilla overworld)

// The server thread boots Server with a synthesised standalone config:
//   - DISABLE_ROOM=1  (no external room server)
//   - All HTTP endpoints stubbed to localhost:1 (server runs without any
//     external HTTP services — connection refused, requests fail fast)
//   - World seed: from nativeServerStart's `seed` arg (or default if 0)
//   - World type: TERRAIN_TYPE_DEFAULT — the real vanilla overworld
//     generator (ChunkProviderGenerate: biomes, caves, ores, trees via
//     BiomeDecorator populate, water)
//   - Map dir: <workDir>/world
void serverThreadMain()
{
        LOGI("Server thread starting; port=%d workDir=%s seed=%lld",
                        g_serverPort, g_serverWorkDir.c_str(), (long long)g_serverWorldSeed.load());

        Server* server = nullptr;
        {
                std::lock_guard<std::mutex> lk(g_stateMutex);
                g_serverInstance = nullptr; // set below only after successful init
        }

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

                // World generation: pass the Java-supplied seed through to
                // WorldSettings. Type = vanilla overworld (ChunkProviderGenerate +
                // BiomeDecorator populate), the "actual world generation" pipeline.
                cfg.worldSeed = g_serverWorldSeed.load();
                cfg.worldType = g_serverWorldType;

                server = new Server();
                server->init(cfg);
                server->start();
                // start() has bound the RakNet socket — mark RUNNING so the Java
                // readiness poll (ServerService.isRunning()) unblocks and the client
                // connects only now. This removes the old blind-500ms-sleep race
                // that caused intermittent connect failures.
                {
                        std::lock_guard<std::mutex> lk(g_stateMutex);
                        g_serverInstance = server;
                        g_serverState = ServerState::RUNNING;
                }
                LOGI("Server running on 0.0.0.0:%d", g_serverPort);
                server->waitForStopEvent();
                LOGI("Server stop event received; shutting down...");
                {
                        std::lock_guard<std::mutex> lk(g_stateMutex);
                        g_serverState = ServerState::STARTING; // shutting down — reject new starts until done
                        g_serverInstance = nullptr;
                }
                server->stopThread();
                server->unInit();
                delete server;
                server = nullptr;
                LOGI("Server shut down cleanly");
        } catch (const std::exception& e) {
                LOGE("Server thread crashed: %s", e.what());
        } catch (...) {
                LOGE("Server thread crashed: unknown exception");
        }
        // Single delete point: cleans up when init/start threw (server was
        // allocated but never deleted in the try block).
        delete server;
        {
                std::lock_guard<std::mutex> lk(g_stateMutex);
                g_serverInstance = nullptr;
                g_serverState = ServerState::IDLE;
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
//
// Idempotent: if a server thread is already STARTING or RUNNING, this is a
// logged no-op (prevents the double-`new Server()` crash).
JNIEXPORT void JNICALL
Java_com_sandboxol_blockmango_ServerService_nativeServerStart(
                JNIEnv* env, jclass, jint port, jstring workDir, jlong seed)
{
        {
                std::lock_guard<std::mutex> lk(g_stateMutex);
                if (g_serverState != ServerState::IDLE)
                {
                        LOGW("nativeServerStart: server already starting/running (state=%d) — ignoring",
                                        (int)g_serverState);
                        return;
                }
                g_serverState = ServerState::STARTING;
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
                LOGE("pthread_create failed (rc=%d), falling back to std::thread", rc);
                // NOTE: state stays STARTING — the fallback thread owns it now and
                // will move it to RUNNING (or back to IDLE on failure). Resetting it
                // here could let a concurrent nativeServerStart double-start.
                std::thread(serverThreadMain).detach();
        }
        pthread_detach(pthread_handle);
}

// Stop the server. Calls Server::setStopEvent() which unblocks
// waitForStopEvent(), allowing the server thread to exit cleanly.
JNIEXPORT void JNICALL
Java_com_sandboxol_blockmango_ServerService_nativeServerStop(JNIEnv*, jclass)
{
        Server* target = nullptr;
        {
                std::lock_guard<std::mutex> lk(g_stateMutex);
                if (g_serverState != ServerState::RUNNING)
                {
                        LOGW("nativeServerStop: server not running (state=%d) — ignoring", (int)g_serverState);
                        return;
                }
                target = g_serverInstance;
        }
        if (target)
        {
                target->setStopEvent();
        }
}

// Check whether the server is currently running (socket bound, accepting
// connections). Returns FALSE while the server is still STARTING — the Java
// side polls this until true before letting the client connect.
JNIEXPORT jboolean JNICALL
Java_com_sandboxol_blockmango_ServerService_nativeServerIsRunning(JNIEnv*, jclass)
{
        std::lock_guard<std::mutex> lk(g_stateMutex);
        return (g_serverState == ServerState::RUNNING) ? JNI_TRUE : JNI_FALSE;
}

} // extern "C"

#endif  // LORD_PLATFORM_ANDROID
