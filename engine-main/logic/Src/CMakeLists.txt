# ---------------------------------------------------------------------
# set include files

if(${LORD_PLATFORM_WIN32})
set(INCLUDE_FILES 
	${PROJECT_SOURCE_DIR}
  	${LORD_SRC_PATH}/Core
	${LOGIC_DEP_PATH}/win32
	${LORD_DEP_PATH}/Zlib/Win32/Include
	${LORD_DEP_PATH}/rapidjson/Include
	${LORD_DEP_PATH}/g3log/win32/include
	${LOGIC_DEP_PATH}/sparsehash/win32/include
	${LOGIC_DEP_PATH}/cpr/win32/include
)

elseif(${LORD_PLATFORM_ANDROID})
set(INCLUDE_FILES 
	${PROJECT_SOURCE_DIR}
  	${LORD_SRC_PATH}/Core
	${LOGIC_DEP_PATH}/Android
	${LORD_DEP_PATH}/Zlib/Linux/Include
	${LORD_DEP_PATH}/rapidjson/Include
	${LOGIC_DEP_PATH}/sparsehash/linux/include
	${LOGIC_DEP_PATH}/cpr/linux/lib
)

else()
set(INCLUDE_FILES 
	${PROJECT_SOURCE_DIR}
  	${LORD_SRC_PATH}/Core
	${LOGIC_DEP_PATH}/linux
	${LORD_DEP_PATH}/Zlib/Linux/Include
	${LORD_DEP_PATH}/rapidjson/Include
	${LORD_DEP_PATH}/g3log/linux/include
	${LOGIC_DEP_PATH}/sparsehash/linux/include
	${LOGIC_DEP_PATH}/cpr/linux/include
)
endif()

include_directories( ${INCLUDE_FILES} )

# ---------------------------------------------------------------------
# set link libraries

#set(TARGET_LINK_FLAGS "")

if(${LORD_PLATFORM_WIN32})

link_directories(
	${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
	${LOGIC_DEP_PATH}/win32/raknet/lib/debug
)

link_libraries(
	LordCore.lib
	raknet.lib
)

elseif(${LORD_PLATFORM_ANDROID})
# add your link dir and libiary for android
elseif(${LORD_PLATFORM_LINUX})

link_directories(
	${LORD_BIN_PATH}
	${LOGIC_DEP_PATH}/raknet/lib
)

link_libraries(
	libCore.a
	libraknet.a
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

add_definitions("-DLORD_LOGIC_EXPORTS")
if(LORD_STATIC)
    add_definitions("-DLORD_STATIC")
endif()

if(${LORD_PLATFORM_WIN32})
  add_definitions("-D_CRT_SECURE_NO_WARNINGS")
endif()

if(LORD_UNICODE)
	add_definitions("-DUNICODE -D_UNICODE")
endif()

set(LOGIC_NAME "Logic")

add_library(Logic STATIC ${HEADER_FILES} ${SOURCE_FILES})


set_target_properties(Logic PROPERTIES DEBUG_OUTPUT_NAME "${LOGIC_NAME}_d")
set_target_properties(Logic PROPERTIES MINSIZEREL_OUTPUT_NAME ${LOGIC_NAME})
set_target_properties(Logic PROPERTIES RELEASE_OUTPUT_NAME ${LOGIC_NAME})
set_target_properties(Logic PROPERTIES RELWITHDEBINFO_OUTPUT_NAME ${LOGIC_NAME})

message(STATUS "Configure success!")