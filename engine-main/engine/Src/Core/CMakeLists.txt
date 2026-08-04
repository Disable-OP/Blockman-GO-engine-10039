message(STATUS "Configuring module: Core...")

# ---------------------------------------------------------------------
# set include files

set(INCLUDE_FILES 
  ${LORD_SRC_PATH}/Core
  ${LORD_DEP_PATH}/nedmalloc/Include
  ${LORD_DEP_PATH}/RapidXml/Include
  ${LORD_DEP_PATH}/rapidjson/Include
)

message(STATUS "INCLUDE_FILES: ${INCLUDE_FILES}")

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${LORD_SRC_PATH}/Core/LordConfig.h.in"
  "${LORD_SRC_PATH}/Core/LordConfig.h"
  )

message(STATUS "LORD_PLATFORM_OSX: ${LORD_PLATFORM_OSX}")
message(STATUS "LORD_PLATFORM_IOS: ${LORD_PLATFORM_IOS}")
message(STATUS "LORD_PLATFORM_WIN32: ${LORD_PLATFORM_WIN32}")
message(STATUS "LORD_PLATFORM_ANDROID: ${LORD_PLATFORM_ANDROID}")
message(STATUS "LORD_PLATFORM_LINUX: ${LORD_PLATFORM_LINUX}")

if(${LORD_PLATFORM_WIN32})
  list(APPEND INCLUDE_FILES ${LORD_DEP_PATH}/dirent/Include)
  list(APPEND INCLUDE_FILES ${LORD_DEP_PATH}/FreeImage/Win32/Include)
  list(APPEND INCLUDE_FILES ${LORD_DEP_PATH}/FreeType/Win32/Include)
  list(APPEND INCLUDE_FILES ${LORD_DEP_PATH}/Zlib/Win32/Include)
  list(APPEND INCLUDE_FILES ${LORD_DEP_PATH}/g3log/win32/include)
elseif(${LORD_PLATFORM_ANDROID})
  list(APPEND INCLUDE_FILES ${LORD_DEP_PATH}/FreeImage/Android/Include)
  list(APPEND INCLUDE_FILES ${LORD_DEP_PATH}/FreeType/Android/Include)
  list(APPEND INCLUDE_FILES ${LORD_DEP_PATH}/Zlib/Android/Include)
elseif(${LORD_PLATFORM_LINUX})
  list(APPEND INCLUDE_FILES ${LORD_DEP_PATH}/FreeImage/Android/Include)
  list(APPEND INCLUDE_FILES ${LORD_DEP_PATH}/FreeType/Android/Include)
  list(APPEND INCLUDE_FILES ${LORD_DEP_PATH}/Zlib/Android/Include)
  list(APPEND INCLUDE_FILES ${LORD_DEP_PATH}/g3log/linux/include)
else()
  message(STATUS "Unknown Platform")
endif()
message(STATUS "INCLUDE_FILES: ${INCLUDE_FILES}")
include_directories( ${INCLUDE_FILES} )

# ---------------------------------------------------------------------
# set link libraries

if(${LORD_PLATFORM_WIN32})

link_directories(
  ${LORD_DEP_PATH}/FreeImage/Win32/Lib
  ${LORD_DEP_PATH}/FreeType/Win32/Lib
  ${LORD_DEP_PATH}/Zlib/Win32/Lib
)

link_libraries(
  FreeImage.lib
  FreeType.lib
  Zlib.lib
  ws2_32.lib
)

elseif(${LORD_PLATFORM_ANDROID})

link_directories(
  ${LORD_DEP_PATH}/FreeImage/Android/Lib
  ${LORD_DEP_PATH}/FreeType/Android/Lib
  ${LORD_DEP_PATH}/Zlib/Android/Lib
)

link_libraries(
  libFreeImage.a
  libFreeType.a
  libzlib.a
)
elseif(${LORD_PLATFORM_LINUX})

link_directories(
  ${LORD_DEP_PATH}/FreeImage/Linux/Lib
  ${LORD_DEP_PATH}/FreeType/Linux/Lib
  ${LORD_DEP_PATH}/Zlib/Linux/Lib
  ${LORD_DEP_PATH}/g3log/linux/Lib
)

link_libraries(
  libFreeImage.a
  libFreeType.a
  libz.a
  libg3logger.a
)
endif()

# ---------------------------------------------------------------------

file(GLOB_RECURSE HEADER_FILES  *.h *.inl)
file(GLOB_RECURSE SOURCE_FILES *.cpp *.c)

if(NOT LORD_INCLUDE_GUI)
	file(GLOB_RECURSE ALL_UI_FILES UI/*.h UI/*.inl UI/*.c UI/*.cpp)
	list(REMOVE_ITEM HEADER_FILES ${ALL_UI_FILES})
	list(REMOVE_ITEM SOURCE_FILES ${ALL_UI_FILES})
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
                set(sgbd_group_name "Common")
            endif(sgbd_nogroup)
                  
            source_group(${sgbd_group_name} FILES ${group_files})
            
        endforeach(group_files)
endmacro(GROUP_FILES)

GROUP_FILES(ALL_FILES)

add_definitions("-DLORD_CORE_EXPORTS")
add_definitions("-DTOLUA_STATIC")
if(${LORD_PLATFORM_WIN32})
  add_definitions("-D_CRT_SECURE_NO_WARNINGS")
endif()

if(LORD_UNICODE)
	add_definitions("-DUNICODE -D_UNICODE")
endif()
            	
if (LORD_STATIC)
	set(CORE_NAME "LordCore")
    add_definitions("-DLORD_STATIC")
else()
	set(CORE_NAME "LordCore")
endif()

if (LORD_STATIC)
	add_library(Core STATIC ${HEADER_FILES} ${SOURCE_FILES})
else()
	add_library(Core SHARED ${HEADER_FILES} ${SOURCE_FILES})
endif()

set_target_properties(Core PROPERTIES DEBUG_OUTPUT_NAME "${CORE_NAME}_d")
set_target_properties(Core PROPERTIES MINSIZEREL_OUTPUT_NAME ${CORE_NAME})
set_target_properties(Core PROPERTIES RELEASE_OUTPUT_NAME ${CORE_NAME})
set_target_properties(Core PROPERTIES RELWITHDEBINFO_OUTPUT_NAME ${CORE_NAME})

if(LORD_PLATFORM_ANDROID)
	set_target_properties(Core PROPERTIES LINK_FLAGS_DEBUG "${LORD_DEP_PATH}/FreeImage/Android/Lib/libFreeImage.a")
	set_target_properties(Core PROPERTIES LINK_FLAGS_MINSIZEREL "${LORD_DEP_PATH}/FreeImage/Android/Lib/libFreeImage.a")
	set_target_properties(Core PROPERTIES LINK_FLAGS_RELEASE "${LORD_DEP_PATH}/FreeImage/Android/Lib/libFreeImage.a")
	set_target_properties(Core PROPERTIES LINK_FLAGS_RELWITHDEBINFO "${LORD_DEP_PATH}/FreeImage/Android/Lib/libFreeImage.a")
	
	set_target_properties(Core PROPERTIES LINK_FLAGS_DEBUG "${LORD_DEP_PATH}/FreeType/Android/Lib/libFreeType.a")
	set_target_properties(Core PROPERTIES LINK_FLAGS_MINSIZEREL "${LORD_DEP_PATH}/FreeType/Android/Lib/libFreeType.a")
	set_target_properties(Core PROPERTIES LINK_FLAGS_RELEASE "${LORD_DEP_PATH}/FreeType/Android/Lib/libFreeType.a")
	set_target_properties(Core PROPERTIES LINK_FLAGS_RELWITHDEBINFO "${LORD_DEP_PATH}/FreeType/Android/Lib/libFreeType.a")
elseif(LORD_PLATFORM_LINUX)
	set_target_properties(Core PROPERTIES LINK_FLAGS_DEBUG "${LORD_DEP_PATH}/FreeImage/Linux/Lib/libFreeImage.a")
	set_target_properties(Core PROPERTIES LINK_FLAGS_MINSIZEREL "${LORD_DEP_PATH}/FreeImage/Linux/Lib/libFreeImage.a")
	set_target_properties(Core PROPERTIES LINK_FLAGS_RELEASE "${LORD_DEP_PATH}/FreeImage/Linux/Lib/libFreeImage.a")
	set_target_properties(Core PROPERTIES LINK_FLAGS_RELWITHDEBINFO "${LORD_DEP_PATH}/FreeImage/Linux/Lib/libFreeImage.a")
	
	set_target_properties(Core PROPERTIES LINK_FLAGS_DEBUG "${LORD_DEP_PATH}/FreeType/Linux/Lib/libFreeType.a")
	set_target_properties(Core PROPERTIES LINK_FLAGS_MINSIZEREL "${LORD_DEP_PATH}/FreeType/Linux/Lib/libFreeType.a")
	set_target_properties(Core PROPERTIES LINK_FLAGS_RELEASE "${LORD_DEP_PATH}/FreeType/Linux/Lib/libFreeType.a")
	set_target_properties(Core PROPERTIES LINK_FLAGS_RELWITHDEBINFO "${LORD_DEP_PATH}/FreeType/Linux/Lib/libFreeType.a")
endif()

set_target_properties(Core PROPERTIES LOCATION ${LORD_LIB_PATH})
set_target_properties(Core PROPERTIES FOLDER "Engine")

message(STATUS "Configure success!")