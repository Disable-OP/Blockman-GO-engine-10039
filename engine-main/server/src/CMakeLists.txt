# ---------------------------------------------------------------------
# set include files

if(${LORD_PLATFORM_WIN32})
set(INCLUDE_FILES 
        ${PROJECT_SOURCE_DIR}
        ${LORD_SRC_PATH}/Core
        ${LOGIC_SRC_PATH}
        ${LOGIC_DEP_PATH}/win32
        ${LOGIC_DEP_PATH}/cpr/win32/include
        ${LOGIC_DEP_PATH}/curl/win32/include
        ${LORD_DEP_PATH}/rapidjson/Include
        ${LORD_DEP_PATH}/Zlib/Win32/Include
        ${LORD_DEP_PATH}/g3log/win32/include
        ${LORD_DEP_PATH}/Lua/Win32/Include
  ${SERVER_DEP_PATH}/win32/mysql/include
  ${SERVER_DEP_PATH}/win32/redis/include
  ${SERVER_DEP_PATH}/win32/behaviac/include
        ${LOGIC_DEP_PATH}/sparsehash/win32/include
)

elseif(${LORD_PLATFORM_LINUX})
set(INCLUDE_FILES 
        ${PROJECT_SOURCE_DIR}
        ${LORD_SRC_PATH}/Core
        ${LOGIC_SRC_PATH}
        ${LOGIC_DEP_PATH}/linux
        ${LOGIC_DEP_PATH}/cpr/linux/include
        ${LOGIC_DEP_PATH}/curl/linux/include
        ${LORD_DEP_PATH}/Zlib/Linux/Include
        ${LORD_DEP_PATH}/rapidjson/Include
        ${LORD_DEP_PATH}/Lua/Linux/Include
        ${LORD_DEP_PATH}/g3log/linux/include
  ${SERVER_DEP_PATH}/linux/mysql/include
  ${SERVER_DEP_PATH}/linux/redis/include
  ${SERVER_DEP_PATH}/linux/behaviac/include
        ${LOGIC_DEP_PATH}/sparsehash/linux/include
)

elseif(${LORD_PLATFORM_ANDROID})
# Android: server is built as a shared library (libGameServer.so) loaded
# via JNI. The deps come from the same NDK-built static libs that the
# client uses (built by engine/NDKBuild/DepBuild + libraries/ApkBuild/jni).
#
# For server-only deps (behaviac, mysql, redis headers), we reuse the linux
# headers — they are platform-independent C/C++ headers. The prebuilt .a
# libs for these don't exist for Android, so we DON'T link them (the server
# code paths that use MySQL/Redis are already dead code — commented out in
# Server.cpp; behaviac's initBehaviac() fails silently on TCP connect).
set(INCLUDE_FILES
        ${PROJECT_SOURCE_DIR}
        ${LORD_SRC_PATH}/Core
        ${LOGIC_SRC_PATH}
        ${LORD_DEP_PATH}/rapidjson/Include
        ${LORD_DEP_PATH}/Zlib/Android/Include
        ${LORD_DEP_PATH}/Lua/Android/Include
        ${LORD_DEP_PATH}/g3log/linux/include
        ${LORD_DEP_PATH}/FreeType/Android/Include
        ${LORD_DEP_PATH}/FreeImage/Android/Include
        ${LOGIC_DEP_PATH}/curl/android/include
        ${LOGIC_DEP_PATH}/cpr/android/include
        ${LOGIC_DEP_PATH}/Android/raknet/include
  # Server-only deps: reuse the linux headers (platform-independent).
  ${SERVER_DEP_PATH}/linux/behaviac/include
  ${SERVER_DEP_PATH}/linux/mysql/include
  ${SERVER_DEP_PATH}/linux/redis/include
        ${LOGIC_DEP_PATH}/sparsehash/android/include
)

endif()

include_directories( ${INCLUDE_FILES} )

# ---------------------------------------------------------------------
# set link libraries

#set(TARGET_LINK_FLAGS "")

if(${LORD_PLATFORM_WIN32})

link_directories(
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        ${LOGIC_DEP_PATH}/win32/raknet/lib
        ${LORD_DEP_PATH}/FreeImage/Win32/Lib
        ${LORD_DEP_PATH}/FreeType/Win32/Lib
        ${LORD_DEP_PATH}/Lua/Win32/Lib
        ${LORD_DEP_PATH}/g3log/win32/lib
  ${LORD_DEP_PATH}/luajit/prebuilt/Win32
  ${SERVER_DEP_PATH}/win32/mysql/lib
  ${SERVER_DEP_PATH}/win32/redis/lib
  ${SERVER_DEP_PATH}/win32/behaviac/lib
)

link_libraries(
        RakNet.lib
        ws2_32.lib
        FreeType.lib
        FreeImage.lib
        lua.lib
        debug g3logger_d.lib
        optimized g3logger.lib
  mysqlcppconn-static.lib
  mysqlcppconn.lib
  hiredis.lib
  debug Win32_Interop_debug.lib
  optimized Win32_Interop_release.lib
  debug libbehaviac_msvc_debug.lib
  optimized libbehaviac_msvc_release.lib
)

elseif(${LORD_PLATFORM_LINUX})

link_directories(
  ${CLIENT_URL_PATH}/Lib
  ${LORD_DEP_PATH}/Lua/Linux/Lib
  ${LORD_DEP_PATH}/FreeImage/Linux/Lib
  ${LORD_DEP_PATH}/FreeType/Linux/Lib
  ${LORD_DEP_PATH}/Zlib/Linux/Lib
  ${LORD_DEP_PATH}/g3log/linux/lib
  ${LOGIC_DEP_PATH}/lib
  ${LOGIC_BIN_PATH}
  ${CORE_BIN_PATH}
  ${LOGIC_DEP_PATH}/linux/raknet/lib
  ${LOGIC_DEP_PATH}/cpr/linux/lib
  ${LOGIC_DEP_PATH}/curl/linux/lib
  ${SERVER_DEP_PATH}/linux/mysql/lib
  ${SERVER_DEP_PATH}/linux/redis/lib
  ${SERVER_DEP_PATH}/linux/behaviac/lib
)

IF (CMAKE_BUILD_TYPE MATCHES "Debug" OR CMAKE_BUILD_TYPE MATCHES "None")  
    link_libraries(
        libLogic_d.a
        libraknet.a
        libLordCore_d.a
        libFreeType.a
        libFreeImage.a
        libcpr.a
        libcurl.a
        liblua.a
        libg3logger.a
        libmysqlcppconn-static.a
        libhiredis.a
        libbehaviac_gcc_release.a
)
ELSEIF (CMAKE_BUILD_TYPE MATCHES "Release")  
    link_libraries(
        libLogic.a
        libraknet.a
        libLordCore.a
        libFreeType.a
        libFreeImage.a
        libcpr.a
        libcurl.a
        liblua.a
        libg3logger.a
        libmysqlcppconn-static.a
        libhiredis.a
        libbehaviac_gcc_release.a
    )
ENDIF()

elseif(${LORD_PLATFORM_ANDROID})

# Android: link against the same NDK-built static libs the client uses
# (produced by engine/NDKBuild/DepBuild + libraries/ApkBuild/jni).
# The HTTP-based MysqlHttpRequest/RedisHttpRequest mean we don't need
# libmysqlclient or libhiredis on Android — those code paths are dead
# (commented out in Server.cpp). We only need RakNet + the engine deps.
link_directories(
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
  ${LOGIC_BIN_PATH}
  ${CORE_BIN_PATH}
  ${LORD_DEP_PATH}/FreeType/Android/Lib/${CMAKE_BUILD_TYPE}/${ANDROID_ABI}
  ${LORD_DEP_PATH}/FreeImage/Android/Lib/${CMAKE_BUILD_TYPE}/${ANDROID_ABI}
  ${LORD_DEP_PATH}/Zlib/Android/Lib/${CMAKE_BUILD_TYPE}/${ANDROID_ABI}
  ${LORD_DEP_PATH}/Lua/Android/Lib/${CMAKE_BUILD_TYPE}/${ANDROID_ABI}
  ${LORD_DEP_PATH}/g3log/android/lib/${ANDROID_ABI}
  ${LOGIC_DEP_PATH}/lib/${ANDROID_ABI}
)

link_libraries(
  Logic
  LordCore
  raknet
  FreeType
  FreeImage
  zlib
  lua
  g3logger
  curl
  cpr
  # Android system libs:
  log       # __android_log_print (used by ServerJni.cpp)
  android   # ANativeActivity, JNI helpers
)

else()
    set(LOGIC_BIN_PATH "../../logic/Bin")
    set(CORE_BIN_PATH "../../engine/Bin")
endif()

# ---------------------------------------------------------------------

file(GLOB_RECURSE HEADER_FILES *.h *.inl)
file(GLOB_RECURSE SOURCE_FILES *.cpp *.c)

# On Android, the server is a shared library (libGameServer.so) with a JNI
# entry point (ServerJni.cpp) — it does NOT have a main(). We exclude
# main.cpp from the Android build to avoid a duplicate-symbol link error.
if(LORD_PLATFORM_ANDROID)
        list(FILTER SOURCE_FILES EXCLUDE REGEX "/main[.]cpp$")

        # Build behaviac from source (vendored at server/dependencies/behaviac-src/).
        # The prebuilt libbehaviac_gcc_release.a only exists for Linux x86_64; for
        # Android we compile the behaviac source directly into libGameServer.so.
        # The headers at server/dependencies/linux/behaviac/include/ are reused
        # (they are platform-independent C++ headers).
        set(BEHAVIAC_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/behaviac-src")
        file(GLOB_RECURSE BEHAVIAC_SOURCES "${BEHAVIAC_SRC_DIR}/*.cpp")
        # Exclude the MSVC-only (_vcc) variants — use the _gcc variants for clang.
        list(FILTER BEHAVIAC_SOURCES EXCLUDE REGEX "_vcc[.]cpp$")
        # Exclude the test/example files if any.
        list(FILTER BEHAVIAC_SOURCES EXCLUDE REGEX "/test/")
        list(APPEND SOURCE_FILES ${BEHAVIAC_SOURCES})
        list(LENGTH BEHAVIAC_SOURCES BEHAVIAC_SOURCES_COUNT)
        message(STATUS "Added ${BEHAVIAC_SOURCES_COUNT} behaviac source files from ${BEHAVIAC_SRC_DIR}")
endif()

set(ALL_FILES ${HEADER_FILES} ${SOURCE_FILES})

macro(GROUP_FILES src_files)
        set(cur_dir ${CMAKE_CURRENT_SOURCE_DIR})
        foreach(group_files ${${src_files}})
            string(REGEX REPLACE ${cur_dir}/\(.*\) \\1 sgbd_fpath ${group_files})
            string(REGEX REPLACE "\(.*\)/.*" \\1 sgbd_group_name ${sgbd_fpath})
            string(COMPARE EQUAL ${sgbd_fpath} ${sgbd_group_name} sgbd_nogroup)
            string(REPLACE "/" "\\" sgbd_group_name ${sgbd_group_name})
            if(sgbd_nogroup)
                set(sgbd_group_name "Logic")
            endif(sgbd_nogroup)
                        
            source_group(${sgbd_group_name} FILES ${group_files})
        endforeach(group_files)
endmacro(GROUP_FILES)

GROUP_FILES(ALL_FILES)

add_definitions("-DLORD_GAMECLIENT_EXPORTS")
if(LORD_STATIC)
    add_definitions("-DLORD_STATIC")
endif()
add_definitions("-DTOLUA_STATIC")
if(${LORD_PLATFORM_WIN32})
  add_definitions("-D_CRT_SECURE_NO_WARNINGS")
endif()

if(LORD_UNICODE)
        add_definitions("-DUNICODE -D_UNICODE")
endif()

set(GAMESERVER_NAME GameServer)

if(LORD_PLATFORM_ANDROID)
        # Android: shared library loaded via JNI by ServerService.java.
        # Produces libGameServer.so, packaged into the APK's jniLibs/<abi>/.
        add_library(${GAMESERVER_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES})
else()
        add_executable(${GAMESERVER_NAME} ${HEADER_FILES} ${SOURCE_FILES})
endif()


if(${LORD_PLATFORM_LINUX})
set(EXECUTABLE_LINK_FLAGS "-pthread ${EXECUTABLE_LINK_FLAGS} -rdynamic")
set_target_properties(${GAMESERVER_NAME} PROPERTIES LINK_FLAGS
  "${EXECUTABLE_LINK_FLAGS}"
)
target_link_libraries(${GAMESERVER_NAME} -ldl)

endif()
if(NOT LORD_PLATFORM_ANDROID)
        # On Android, the .so is always just "libGameServer.so" (no _d suffix).
        set_target_properties(${GAMESERVER_NAME} PROPERTIES DEBUG_OUTPUT_NAME "${GAMESERVER_NAME}_d")
        set_target_properties(${GAMESERVER_NAME} PROPERTIES MINSIZEREL_OUTPUT_NAME ${GAMESERVER_NAME})
        set_target_properties(${GAMESERVER_NAME} PROPERTIES RELEASE_OUTPUT_NAME ${GAMESERVER_NAME})
endif()

if(${LORD_PLATFORM_WIN32})
target_link_libraries(${GAMESERVER_NAME} 
debug ${CORE_BIN_PATH}/Debug/LordCore_d optimized ${CORE_BIN_PATH}/release/LordCore
debug ${LOGIC_BIN_PATH}/Debug/Logic_d optimized ${LOGIC_BIN_PATH}/release/Logic
debug ${LORD_DEP_PATH}/Zlib/Win32/Lib/zlib_d optimized ${LORD_DEP_PATH}/Zlib/Win32/Lib/zlib
debug ${LOGIC_DEP_PATH}/cpr/win32/lib/cpr_d optimized ${LOGIC_DEP_PATH}/cpr/win32/lib/cpr
debug ${LOGIC_DEP_PATH}/curl/win32/lib/libcurl_d_imp optimized ${LOGIC_DEP_PATH}/curl/win32/lib/libcurl_imp)


endif()

message(STATUS "Configure success!")
