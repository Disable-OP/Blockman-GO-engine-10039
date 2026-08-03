# ---------------------------------------------------------------------
# set include files

if(${LORD_PLATFORM_WIN32})
set(INCLUDE_FILES 
	${PROJECT_SOURCE_DIR}
	${PROJECT_SOURCE_DIR}/Blockman
	${PROJECT_SOURCE_DIR}/External
  	${LORD_SRC_PATH}/Core
  	${LORD_SRC_PATH}/GLES2Render
  	${LORD_DEP_PATH}/opengles_v2/Include
  	${LORD_DEP_PATH}/RapidXml/Include
  	${LORD_DEP_PATH}/Lua/Win32/Include
  	${LORD_DEP_PATH}/Zlib/Win32/Include
  	${LORD_DEP_PATH}/rapidjson/Include
  	${LORD_DEP_PATH}/g3log/win32/include
	${LORD_DEP_PATH}/FreeType/IOS/Include
	${LOGIC_SRC_PATH}
	${LOGIC_DEP_PATH}/win32
  	${CLIENT_URL_PATH}/Include
	${LOGIC_DEP_PATH}/cpr/win32/include
	${LOGIC_DEP_PATH}/curl/win32/include
	${LOGIC_DEP_PATH}/libzippp/win32/include
	${LOGIC_DEP_PATH}/sparsehash/win32/include
)

elseif(${LORD_PLATFORM_ANDROID})
set(INCLUDE_FILES 
	${PROJECT_SOURCE_DIR}
	${PROJECT_SOURCE_DIR}/Blockman
	${PROJECT_SOURCE_DIR}/External
  	${LORD_SRC_PATH}/Core
  	${LORD_SRC_PATH}/GLES2Render
  	${LORD_DEP_PATH}/RapidXml/Include
  	${LORD_DEP_PATH}/Lua/Android/Include
  	${LORD_DEP_PATH}/Zlib/Linux/Include
    ${LORD_DEP_PATH}/rapidjson/Include
	${LORD_DEP_PATH}/FreeType/IOS/Include
	${LOGIC_SRC_PATH}
	${LOGIC_DEP_PATH}/Android
  	${CLIENT_URL_PATH}/Include
	${LOGIC_DEP_PATH}/sparsehash/linux/include
)

elseif(${LORD_PLATFORM_IOS})
set(INCLUDE_FILES 
	${PROJECT_SOURCE_DIR}
	${PROJECT_SOURCE_DIR}/Blockman
	${PROJECT_SOURCE_DIR}/External
  	${LORD_SRC_PATH}/Core
  	${LORD_SRC_PATH}/GLES2Render
    ${LOGIC_DEP_PATH}/src
    ${LOGIC_SRC_PATH}
  	${LORD_DEP_PATH}/RapidXml/Include
    ${LORD_DEP_PATH}/cpr/iOS/Include
  	${LORD_DEP_PATH}/Lua/IOS/Include
  	${LORD_DEP_PATH}/Zlib/Linux/Include
    ${LORD_DEP_PATH}/zippp/iOS/Include
    ${LORD_DEP_PATH}/zip/iOS/Include
    ${LORD_DEP_PATH}/rapidjson/Include
    ${LORD_DEP_PATH}/FreeType/IOS/Include
    ${LORD_DEP_PATH}/g3log/ios/include
    ${LORD_DEP_PATH}/cpr/iOS/Include
    ${LORD_DEP_PATH}/cpr/iOS/Include/curl/include/curl
	${LOGIC_SRC_PATH}
	${LOGIC_DEP_PATH}/Ios
  	# ${CLIENT_URL_PATH}/Include
	${LOGIC_DEP_PATH}/sparsehash/linux/include
)

else()

endif()

include_directories( ${INCLUDE_FILES} )

# ---------------------------------------------------------------------
# set link libraries

#set(TARGET_LINK_FLAGS "")

if(${LORD_PLATFORM_WIN32})

link_directories(
	${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
	${CLIENT_URL_PATH}/Lib
	${LORD_DEP_PATH}/Lua/Win32/Lib
	${LOGIC_SRC_PATH}/../dependencies/raknet/win32/debug
	${LOGIC_DEP_PATH}/cpr/win32/lib
	${LOGIC_DEP_PATH}/curl/win32/lib
)

link_libraries(
	LordGLES2Render.lib
	LordCore.lib
	libcurl_imp.lib
	Lua.lib
	logic.lib
	raknet.lib
	zlib.lib
	cpr.lib
	libcurl.lib
)

elseif(${LORD_PLATFORM_ANDROID})

link_directories(
  ${CLIENT_URL_PATH}/Lib
  ${LORD_DEP_PATH}/Lua/Android/Lib
)

link_libraries(
  libcurl.a
  libLua.a
)

elseif(${LORD_PLATFORM_OSX})

elseif(${LORD_PLATFORM_IOS})

link_directories(
	${CLIENT_URL_PATH}/Lib
	${LORD_DEP_PATH}/Lua/Android/Lib
)
link_libraries(
	libcurl.a
	libLua.a
)

endif()
# ---------------------------------------------------------------------

file(GLOB_RECURSE HEADER_FILES *.h *.inl)
file(GLOB_RECURSE SOURCE_FILES *.cpp *.c)

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

set(GAMECLIENT_NAME "GameClient")

add_library(GameClient STATIC ${HEADER_FILES} ${SOURCE_FILES})


set_target_properties(GameClient PROPERTIES DEBUG_OUTPUT_NAME "${GAMECLIENT_NAME}_d")
set_target_properties(GameClient PROPERTIES MINSIZEREL_OUTPUT_NAME ${GAMECLIENT_NAME})
set_target_properties(GameClient PROPERTIES RELEASE_OUTPUT_NAME ${GAMECLIENT_NAME})
set_target_properties(GameClient PROPERTIES RELWITHDEBINFO_OUTPUT_NAME ${GAMECLIENT_NAME})

message(STATUS "Configure success!")
