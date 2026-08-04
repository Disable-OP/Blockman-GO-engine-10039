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

project(Logic)

message(STATUS "#####################################################################")
message(STATUS "# GameLogic Build System")
message(STATUS "#####################################################################")
message(STATUS "Configuring build system...")
message(STATUS "CMake version: ${CMAKE_VERSION}")
message(STATUS "Platform: ${CMAKE_SYSTEM_NAME}")

# CMake 2.8.2 has a bug that creates unusable Xcode projects when using ARCHS_STANDARD_32_BIT
# to specify both armv6 and armv7.
#if(CMAKE_VERSION VERSION_EQUAL 2.8.2)
#	message(FATAL_ERROR "CMake 2.8.2 cannot create compatible Xcode projects for iOS")
#endif()

option(LORD_BUILD_PLATFORM_ANDROID "Android platform." FALSE)
	
if(UNIX)
	if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	    option(LORD_BUILD_PLATFORM_LINUX "Linux platform." TRUE)
	else()
		option(LORD_BUILD_PLATFORM_ANDROID "Android platform." TRUE)
	endif()
endif()

option(LORD_STATIC "Static build" TRUE)
option(LORD_UNICODE "Using Unicode charset" FALSE)

if(WIN32)
	if(LORD_BUILD_PLATFORM_ANDROID)
		set(LORD_PLATFORM_ANDROID TRUE)
		message(STATUS "Platform: Android")

		set(LORD_STATIC TRUE CACHE BOOL "Forcing static build for android" FORCE)
		
		#notice the "recycling" of CMAKE_C_FLAGS
		#this is necessary to pick up android flags
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic -fPIC")
	else()
	
		set(LORD_PLATFORM_WIN32 TRUE)
		message(STATUS "Platform: Windows")
	
		# compiler building setting
		# Flags used by the compiler during all build types.
		set(CMAKE_CXX_FLAGS "/DWIN32 /D_WINDOWS /W3 /Zm1000 /EHsc /GR /MP")
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
		
	endif()
elseif(UNIX)
	if(LORD_BUILD_PLATFORM_ANDROID)
		set(LORD_PLATFORM_ANDROID TRUE)
		message(STATUS "Platform: Android")

		set(LORD_STATIC TRUE CACHE BOOL "Forcing static build for android" FORCE)

		set(CMAKE_OSX_SYSROOT "" CACHE STRING "" FORCE)
		set(CMAKE_OSX_ARCHITECTURES "" CACHE STRING "" FORCE)
		set(XCODE_ATTRIBUTE_SDKROOT "" CACHE STRING "" FORCE)
		
		#notice the "recycling" of CMAKE_C_FLAGS
		#this is necessary to pick up android flags
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic -fPIC")
	elseif(LORD_BUILD_PLATFORM_LINUX)
		set(LORD_PLATFORM_LINUX TRUE)
		message(STATUS "Platform: Unix/Linux")
		
		set(LORD_STATIC TRUE CACHE BOOL "Forcing static build for linux" FORCE)
		
		set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -std=c++11 --no-warnings -D_FORCE_INLINES")
	    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++11 --no-warnings -D_FORCE_INLINES")
	    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -std=c++11 -Wall -Wfatal-errors -Wextra -D_FORCE_INLINES")
		set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++11 -Wall -Wfatal-errors -Wextra -D_FORCE_INLINES")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIE -D_GLIBCXX_USE_CXX11_ABI=0 -std=c++11 --no-warnings -D_FORCE_INLINES")
	endif()
else()
	message(STATUS "Platform: Unknown")
endif()

message(STATUS "Set Environment Variables...")

if(CMAKE_BUILD_TYPE STREQUAL "")
  # CMake defaults to leaving CMAKE_BUILD_TYPE empty. This screws up
  # differentiation between debug and release builds.
#  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()

set(LOGIC_ROOT_PATH "${PROJECT_SOURCE_DIR}")
set(LOGIC_DEP_PATH "${LOGIC_ROOT_PATH}/dependencies")

set(PROJECT_SOURCE_DIR "${LOGIC_ROOT_PATH}/Src")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${LOGIC_ROOT_PATH}/Bin/${CMAKE_BUILD_TYPE}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${LOGIC_ROOT_PATH}/Bin/${CMAKE_BUILD_TYPE}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${LOGIC_ROOT_PATH}/Bin/${CMAKE_BUILD_TYPE}")

set(LORD_SRC_PATH "../../engine/Src")
set(LORD_DEP_PATH "../../engine/Dependencies")
set(LORD_BIN_PATH "../../engine/Bin")

message(STATUS "\${CMAKE_BUILD_TYPE}: ${CMAKE_BUILD_TYPE}")
message(STATUS "\${PROJECT_SOURCE_DIR}: ${PROJECT_SOURCE_DIR}")
message(STATUS "\${PROJECT_BINARY_DIR}: ${PROJECT_BINARY_DIR}")
message(STATUS "\${LOGIC_ROOT_PATH}: ${LOGIC_ROOT_PATH}")
message(STATUS "\${LORD_SRC_PATH}: ${LORD_SRC_PATH}")
message(STATUS "\${LORD_DEP_PATH}: ${LORD_DEP_PATH}")
message(STATUS "\${LOGIC_DEP_PATH}: ${LOGIC_DEP_PATH}")

# Add sub directories
add_subdirectory("Src")

# include jni
if(LORD_PLATFORM_ANDROID)
	include_directories("jni")
endif()