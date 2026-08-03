cmake_minimum_required(VERSION 2.8)

if(COMMAND cmake_policy)  
        cmake_policy(SET CMP0015 NEW)  
endif(COMMAND cmake_policy)  

IF (CMAKE_BUILD_TYPE MATCHES "Debug" OR CMAKE_BUILD_TYPE MATCHES "None")  
    MESSAGE(STATUS "CMAKE_BUILD_TYPE is Debug")  
ELSEIF (CMAKE_BUILD_TYPE MATCHES "Release")  
    MESSAGE(STATUS "CMAKE_BUILD_TYPE is Release")
ENDIF()

SET(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_ALLOW_LOSSE_LOOP_CONSTRUCTS ON)

project(Server)

message(STATUS "#####################################################################")
message(STATUS "# Server Build System")
message(STATUS "#####################################################################")
message(STATUS "Configuring build system...")
message(STATUS "CMake version: ${CMAKE_VERSION}")
message(STATUS "Platform: ${CMAKE_SYSTEM_NAME}")

# CMake 2.8.2 has a bug that creates unusable Xcode projects when using ARCHS_STANDARD_32_BIT
# to specify both armv6 and armv7.
#if(CMAKE_VERSION VERSION_EQUAL 2.8.2)
#       message(FATAL_ERROR "CMake 2.8.2 cannot create compatible Xcode projects for iOS")
#endif()

option(LORD_STATIC "Static build" TRUE)

# Android target: the server compiles into a shared library (libGameServer.so)
# that the Java side loads via System.loadLibrary and starts via JNI. This is
# what makes "no PC, no other server" possible — the phone runs the server
# in-process. See docs/ARCHITECTURE.md and ServerJni.cpp.
option(LORD_BUILD_PLATFORM_ANDROID "Build server as Android shared lib (libGameServer.so)" FALSE)

if(ANDROID)
        # ANDROID is set by CMake's Android toolchain file — auto-enable.
        set(LORD_BUILD_PLATFORM_ANDROID TRUE CACHE BOOL "Android detected" FORCE)
endif()

if(LORD_BUILD_PLATFORM_ANDROID)
        set(LORD_PLATFORM_ANDROID TRUE)
        set(LORD_STATIC TRUE CACHE BOOL "Forcing static build for android" FORCE)
        message(STATUS "Platform: Android (server → libGameServer.so)")

        # Android uses clang from the NDK. C++11 + GNU STL ABI (matches the
        # legacy gnustl_static used by the client ndk-build setup; until the
        # client is migrated to c++_shared, both sides must agree on STL).
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -D_FORCE_INLINES -D_GLIBCXX_USE_CXX11_ABI=0 -fexceptions -frtti -D__GXX_EXPERIMENTAL_CXX0X__ -include gnustl_compat.h")
        set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -D_FORCE_INLINES")
        # Position-independent code (required for shared lib).
        set(CMAKE_POSITION_INDEPENDENT_CODE ON)
elseif(WIN32)

                set(LORD_PLATFORM_WIN32 TRUE)
                message(STATUS "Platform: Windows")
        
                # compiler building setting
                # Flags used by the compiler during all build types.
                set(CMAKE_CXX_FLAGS "/DWIN32 /D_WINDOWS /W3 /Zm1000 /EHsc /GR /MP /bigobj")
                # Flags used by the compiler during debug builds.
                set(CMAKE_CXX_FLAGS_DEBUG "/D_DEBUG /MDd /Zi /Ob0 /Od /RTC1")
                # Flags used by the compiler during release minsize builds.
                set(CMAKE_CXX_FLAGS_MINSIZEREL "/MD /O1 /Ob1 /D NDEBUG")
                # Flags used by the compiler during release builds (/MD /Ob1 /Oi /Ot /Oy /Gs will produce slightly less optimized but smaller files).
                set(CMAKE_CXX_FLAGS_RELEASE "/MD /O2 /Ob2 /D NDEBUG")
                # Flags used by the compiler during Release with Debug Info builds.
                set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/MD /Zi /O2 /Ob1 /D NDEBUG")
                # Flags used by the compiler during all build types.
                set(CMAKE_C_FLAGS "/DWIN32 /D_WINDOWS /W3 /Zm1000")
                # Flags used by the compiler during debug builds.
                set(CMAKE_C_FLAGS_DEBUG "/D_DEBUG /MDd /Zi  /Ob0 /Od /RTC1")
                # Flags used by the compiler during release minsize builds.
                set(CMAKE_C_FLAGS_MINSIZEREL "/MD /O1 /Ob1 /D NDEBUG")
                # Flags used by the compiler during release builds (/MD /Ob1 /Oi /Ot /Oy /Gs will produce slightly less optimized but smaller files).
                set(CMAKE_C_FLAGS_RELEASE "/MD /O2 /Ob2 /D NDEBUG")
                # Flags used by the compiler during Release with Debug Info builds.
                set(CMAKE_C_FLAGS_RELWITHDEBINFO "/MD /Zi /O2 /Ob1 /D NDEBUG")
                
                if(MSVC)
                        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fp:fast")
                        
                        # Enable intrinsics on MSVC in debug mode
                        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Oi")
                        if (CMAKE_CL_64)
                                # Visual Studio bails out on debug builds in 64bit mode unless
                                # this flag is set...
                                set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /bigobj")
                                set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /bigobj")
                        endif ()
                        
                endif()
                
                # This seems to break Xcode projects so definitely don't enable on Apple builds
                set(CMAKE_USE_RELATIVE_PATHS true)
                set(CMAKE_SUPPRESS_REGENERATION true)

elseif(UNIX)
                message(STATUS "Platform: Unix/Linux")
                set(LORD_PLATFORM_LINUX TRUE)
                set(LORD_STATIC TRUE CACHE BOOL "Forcing static build for linux" FORCE)
                
                set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -std=c++11 --no-warnings -D_FORCE_INLINES")
            set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++11 --no-warnings -D_FORCE_INLINES")
            set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -std=c++11 --no-warnings  -Wfatal-errors -D_FORCE_INLINES")
                set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O2 -std=c++11 --no-warnings -Wfatal-errors -D_FORCE_INLINES")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_RELEASE} -fPIE -D_GLIBCXX_USE_CXX11_ABI=0 -std=c++11 --no-warnings -D_FORCE_INLINES")
endif()

message(STATUS "Set Environment Variables...")

if(CMAKE_BUILD_TYPE STREQUAL "")
  # CMake defaults to leaving CMAKE_BUILD_TYPE empty. This screws up
  # differentiation between debug and release builds.
#  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()

set(SERVER_ROOT_PATH "${PROJECT_SOURCE_DIR}")
set(PROJECT_SOURCE_DIR "${SERVER_ROOT_PATH}/src")


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${SERVER_ROOT_PATH}/Bin/${CMAKE_BUILD_TYPE}")
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${SERVER_ROOT_PATH}/../../res/client")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${SERVER_ROOT_PATH}/Bin/${CMAKE_BUILD_TYPE}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${SERVER_ROOT_PATH}/Bin/${CMAKE_BUILD_TYPE}")
set(LORD_DEP_PATH "../../engine/Dependencies")
set(LOGIC_DEP_PATH "../../logic/dependencies")
set(SERVER_DEP_PATH "../../server/dependencies")
if(LORD_PLATFORM_LINUX)
    IF (CMAKE_BUILD_TYPE MATCHES "Debug" OR CMAKE_BUILD_TYPE MATCHES "None")  
        MESSAGE(STATUS "CMAKE_BUILD_TYPE is Debug")  
        set(LOGIC_BIN_PATH "../../logic/Bin/Debug")
        set(CORE_BIN_PATH "../../engine/Bin/Debug")
    ELSEIF (CMAKE_BUILD_TYPE MATCHES "Release")  
        MESSAGE(STATUS "CMAKE_BUILD_TYPE is Release")
        set(LOGIC_BIN_PATH "../../logic/Bin/Release")
        set(CORE_BIN_PATH "../../engine/Bin/Release")
    ENDIF()
else()
    set(LOGIC_BIN_PATH "../../logic/Bin")
    set(CORE_BIN_PATH "../../engine/Bin")
endif()

set(LORD_SRC_PATH "../../engine/Src")
set(LOGIC_SRC_PATH "../../logic/Src")

if(LORD_PLATFORM_WIN32)
        set(CLIENT_URL_PATH "../curl/Windows")
elseif(LORD_PLATFORM_ANDROID)
        set(CLIENT_URL_PATH "../curl/Android")
elseif(LORD_PLATFORM_LINUX)
        set(CLIENT_URL_PATH "../curl/Android")
endif()

message(STATUS "\${CMAKE_BUILD_TYPE}: ${CMAKE_BUILD_TYPE}")
message(STATUS "\${PROJECT_SOURCE_DIR}: ${PROJECT_SOURCE_DIR}")
message(STATUS "\${PROJECT_BINARY_DIR}: ${PROJECT_BINARY_DIR}")
message(STATUS "\${SERVER_ROOT_PATH}: ${SERVER_ROOT_PATH}")
message(STATUS "\${LOGIC_DEP_PATH}: ${LOGIC_DEP_PATH}")
message(STATUS "\${LORD_DEP_PATH}: ${LORD_DEP_PATH}")
message(STATUS "\${SERVER_DEP_PATH}: ${SERVER_DEP_PATH}")

# Add sub directories
add_subdirectory("src")