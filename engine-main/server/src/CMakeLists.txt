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

else()
    set(LOGIC_BIN_PATH "../../logic/Bin")
    set(CORE_BIN_PATH "../../engine/Bin")
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

set(GAMESERVER_NAME GameServer)


add_executable(${GAMESERVER_NAME} ${HEADER_FILES} ${SOURCE_FILES})


if(${LORD_PLATFORM_LINUX})
set(EXECUTABLE_LINK_FLAGS "-pthread ${EXECUTABLE_LINK_FLAGS} -rdynamic")
set_target_properties(${GAMESERVER_NAME} PROPERTIES LINK_FLAGS
  "${EXECUTABLE_LINK_FLAGS}"
)
target_link_libraries(${GAMESERVER_NAME} -ldl)

endif()
set_target_properties(${GAMESERVER_NAME} PROPERTIES DEBUG_OUTPUT_NAME "${GAMESERVER_NAME}_d")
set_target_properties(${GAMESERVER_NAME} PROPERTIES MINSIZEREL_OUTPUT_NAME ${GAMESERVER_NAME})
set_target_properties(${GAMESERVER_NAME} PROPERTIES RELEASE_OUTPUT_NAME ${GAMESERVER_NAME})

if(${LORD_PLATFORM_WIN32})
target_link_libraries(${GAMESERVER_NAME} 
debug ${CORE_BIN_PATH}/Debug/LordCore_d optimized ${CORE_BIN_PATH}/release/LordCore
debug ${LOGIC_BIN_PATH}/Debug/Logic_d optimized ${LOGIC_BIN_PATH}/release/Logic
debug ${LORD_DEP_PATH}/Zlib/Win32/Lib/zlib_d optimized ${LORD_DEP_PATH}/Zlib/Win32/Lib/zlib
debug ${LOGIC_DEP_PATH}/cpr/win32/lib/cpr_d optimized ${LOGIC_DEP_PATH}/cpr/win32/lib/cpr
debug ${LOGIC_DEP_PATH}/curl/win32/lib/libcurl_d_imp optimized ${LOGIC_DEP_PATH}/curl/win32/lib/libcurl_imp)


endif()

message(STATUS "Configure success!")
