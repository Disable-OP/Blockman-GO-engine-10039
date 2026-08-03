message(STATUS "Configuring module: GLES2Render...")

# ---------------------------------------------------------------------
# set include files

set(INCLUDE_FILES 
  ${LORD_SRC_PATH}/Core
)

if(${LORD_PLATFORM_WIN32})
  list(APPEND INCLUDE_FILES ${LORD_DEP_PATH}/opengles_v2/Include)
  list(APPEND INCLUDE_FILES ${LORD_DEP_PATH}/g3log/win32/Include)
endif()

include_directories( ${INCLUDE_FILES} )

if(${LORD_PLATFORM_ANDROID})
  list(APPEND INCLUDE_FILES ${LORD_DEP_PATH}/GL/Android/Include)
endif()

# ---------------------------------------------------------------------

file(GLOB_RECURSE HEADER_FILES *.h *.inl)
file(GLOB_RECURSE SOURCE_FILES *.cpp)

set(ALL_FILES ${HEADER_FILES} ${SOURCE_FILES})

add_definitions("-DLORD_GLES2RENDER_EXPORTS")
add_definitions("-DTOLUA_STATIC")
if(${LORD_PLATFORM_WIN32})
  add_definitions("-D_CRT_SECURE_NO_WARNINGS")
endif()

if(LORD_UNICODE)
	add_definitions("-DUNICODE -D_UNICODE")
endif()

if (LORD_STATIC)
	set(GLES2RENDER_NAME "LordGLES2Render")
    add_definitions("-DLORD_STATIC")
else()
	set(GLES2RENDER_NAME "LordGLES2Render")
endif()

if(${LORD_PLATFORM_WIN32})

link_directories(
  ${LORD_DEP_PATH}/FreeImage/Win32/Lib
  ${LORD_DEP_PATH}/FreeType/Win32/Lib
  ${LORD_DEP_PATH}/Zlib/Win32/Lib
  ${LORD_DEP_PATH}/opengles_v2/Lib
)

link_libraries(
  FreeImage.lib
  FreeType.lib
  Zlib.lib
  ws2_32.lib
  libEGL.lib
  libGLESv2.lib
)
endif()

if (LORD_STATIC)
	add_library(GLES2Render STATIC ${HEADER_FILES} ${SOURCE_FILES})
else()
	add_library(GLES2Render SHARED ${HEADER_FILES} ${SOURCE_FILES})
endif()


target_link_libraries(GLES2Render Core)

set_target_properties(GLES2Render PROPERTIES DEBUG_OUTPUT_NAME "${GLES2RENDER_NAME}_d")
set_target_properties(GLES2Render PROPERTIES MINSIZEREL_OUTPUT_NAME ${GLES2RENDER_NAME})
set_target_properties(GLES2Render PROPERTIES RELEASE_OUTPUT_NAME ${GLES2RENDER_NAME})
set_target_properties(GLES2Render PROPERTIES RELWITHDEBINFO_OUTPUT_NAME ${GLES2RENDER_NAME})


set_target_properties(GLES2Render PROPERTIES FOLDER "Engine")

message(STATUS "Configure success!")