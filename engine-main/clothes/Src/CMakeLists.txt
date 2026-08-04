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
)

elseif(${LORD_PLATFORM_IOS})
set(INCLUDE_FILES 
	${PROJECT_SOURCE_DIR}
	${PROJECT_SOURCE_DIR}/Blockman
	${PROJECT_SOURCE_DIR}/External
  	${LORD_SRC_PATH}/Core
  	${LORD_SRC_PATH}/GLES2Render
  	${LORD_DEP_PATH}/RapidXml/Include
  	${LORD_DEP_PATH}/Lua/IOS/Include
  	${LORD_DEP_PATH}/Zlib/Linux/Include
    ${LORD_DEP_PATH}/rapidjson/Include
    ${LORD_DEP_PATH}/g3log/ios/include
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
	${LORD_DEP_PATH}/Lua/Win32/Lib
)

link_libraries(
	LordGLES2Render.lib
	LordCore.lib
	Lua.lib
	zlib.lib
)

elseif(${LORD_PLATFORM_ANDROID})

link_directories(
  ${LORD_DEP_PATH}/Lua/Android/Lib
)

link_libraries(
  libLua.a
)

elseif(${LORD_PLATFORM_OSX})

elseif(${LORD_PLATFORM_IOS})

link_directories(
	${LORD_DEP_PATH}/Lua/Android/Lib
)
link_libraries(
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

add_definitions("-DLORD_CLOTHES_EXPORTS")
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

set(CLOTHES_NAME "Clothes")

add_library(Clothes STATIC ${HEADER_FILES} ${SOURCE_FILES})


set_target_properties(Clothes PROPERTIES DEBUG_OUTPUT_NAME "${CLOTHES_NAME}_d")
set_target_properties(Clothes PROPERTIES MINSIZEREL_OUTPUT_NAME ${CLOTHES_NAME})
set_target_properties(Clothes PROPERTIES RELEASE_OUTPUT_NAME ${CLOTHES_NAME})
set_target_properties(Clothes PROPERTIES RELWITHDEBINFO_OUTPUT_NAME ${CLOTHES_NAME})

message(STATUS "Configure success!")
