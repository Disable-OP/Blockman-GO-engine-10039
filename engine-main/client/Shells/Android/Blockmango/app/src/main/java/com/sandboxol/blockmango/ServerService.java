package com.sandboxol.blockmango;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;

import java.io.File;

/**
 * ServerService — runs the in-process game server on Android.
 *
 * The server is compiled into libGameServer.so (built from
 * engine-main/server/ when LORD_PLATFORM_ANDROID is set). It listens on
 * UDP port 19130 (default) on 127.0.0.1, runs the actual world
 * generation pipeline (logic/Src/WorldGenerator/ — Perlin noise + biome
 * layers + structure pieces), and is the single source of truth for
 * every chunk the client renders.
 *
 * Lifecycle:
 *   startService(ServerService)  →  nativeServerStart(port, filesDir, seed)
 *   stopService(ServerService)   →  nativeServerStop()
 *
 * The client (in libBlockMan.so, started by EchoesActivity) connects to
 * 127.0.0.1:19130 — no external matchmaking, no HTTP services, no PC.
 *
 * See docs/WORLDGEN.md for the design contract and docs/ARCHITECTURE.md
 * for the topology.
 */
public class ServerService extends Service {

    private static final String TAG = "BlockmanServerService";

    /** Default UDP port the server binds on 0.0.0.0 (client connects to 127.0.0.1). */
    public static final int DEFAULT_PORT = 19130;

    /** Default world seed (0 → use the server's built-in test seed). */
    public static final long DEFAULT_SEED = 0L;

    // ---------------------------------------------------------------------
    // JNI declarations — implemented in engine-main/server/src/ServerJni.cpp
    // ---------------------------------------------------------------------

    /**
     * Start the server in a background thread. Returns immediately; the
     * server boots asynchronously. Use {@link #nativeServerIsRunning()}
     * to poll for readiness.
     *
     * @param port    UDP port to bind (0 → default 19130)
     * @param workDir Android filesDir; world data lives under &lt;workDir&gt;/world
     * @param seed    world seed (0 → use built-in test seed)
     */
    public static native void nativeServerStart(int port, String workDir, long seed);

    /** Stop the server. Shutdown completes within one server tick (~50ms). */
    public static native void nativeServerStop();

    /** @return true if the server is currently running. */
    public static native boolean nativeServerIsRunning();

    // ---------------------------------------------------------------------
    // Service lifecycle
    // ---------------------------------------------------------------------

    /** Convenience wrapper used by EchoesActivity to start the server in-process. */
    public static boolean startInProcess(int port, String workDir, long seed) {
        try {
            // Ensure the world directory exists.
            File worldDir = new File(workDir, "world");
            if (!worldDir.exists()) {
                worldDir.mkdirs();
            }
            File gameDir = new File(workDir, "game");
            if (!gameDir.exists()) {
                gameDir.mkdirs();
            }

            // Load the server native lib (may already be loaded by the client).
            try {
                System.loadLibrary("GameServer");
            } catch (UnsatisfiedLinkError e) {
                // Already loaded — that's fine.
                Log.i(TAG, "libGameServer.so already loaded: " + e.getMessage());
            }

            nativeServerStart(port, workDir, seed);
            Log.i(TAG, "Server start requested on port " + port + ", workDir=" + workDir);
            return true;
        } catch (Throwable t) {
            Log.e(TAG, "Failed to start server", t);
            return false;
        }
    }

    public static void stopInProcess() {
        try {
            nativeServerStop();
            Log.i(TAG, "Server stop requested");
        } catch (Throwable t) {
            Log.e(TAG, "Failed to stop server", t);
        }
    }

    public static boolean isRunning() {
        try {
            return nativeServerIsRunning();
        } catch (UnsatisfiedLinkError e) {
            return false;
        }
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        int port = intent != null ? intent.getIntExtra("port", DEFAULT_PORT) : DEFAULT_PORT;
        long seed = intent != null ? intent.getLongExtra("seed", DEFAULT_SEED) : DEFAULT_SEED;
        String workDir = getFilesDir().getAbsolutePath();
        startInProcess(port, workDir, seed);
        return START_STICKY;
    }

    @Override
    public void onDestroy() {
        stopInProcess();
        super.onDestroy();
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;  // not bindable, started only
    }
}
