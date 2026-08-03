# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
   set(BUILD_TYPE "debug")
else()
   set(BUILD_TYPE "release")
endif()

if(${ANDROID_ABI} MATCHES "64")
    set(CURL_INCLUDE_DIR "Include64")
    set(CURL_DIR "64")
else()
    set(CURL_INCLUDE_DIR "Include")
    set(CURL_DIR "32")
endif()

add_library(BlockMan SHARED
            ${CMAKE_SOURCE_DIR}/jni/Blockman.cpp
            ${CMAKE_SOURCE_DIR}/jni/EchoHelper.cpp
            ${CMAKE_SOURCE_DIR}/jni/Blockman.cpp
            ${CMAKE_SOURCE_DIR}/jni/JniHelper.cpp
            ${CMAKE_SOURCE_DIR}/jni/EditBoxImplAndroid.cpp
            ${CMAKE_SOURCE_DIR}/jni/SimpleAudioEngineJni.cpp
            ${CMAKE_SOURCE_DIR}/jni/AndroidSoundEngine.cpp
            ${CMAKE_SOURCE_DIR}/jni/ShellInterfaceAndroid.cpp
            ${CMAKE_SOURCE_DIR}/../../../../Src/game.cpp
            ${CMAKE_SOURCE_DIR}/../../../../../engine/Src/Core/UI/GUIFreeTypeFont.cpp
            ${CMAKE_SOURCE_DIR}/../../../../../engine/Src/Core/Math/IntMath.cpp
            ${CMAKE_SOURCE_DIR}/../../../../../engine/Src/Core/Image/ImageCodec.cpp
            ${CMAKE_SOURCE_DIR}/../../../../../engine/Src/Core/Image/ImageCodecMgr.cpp
            ${CMAKE_SOURCE_DIR}/../../../../../engine/Src/GLES2Render/GLES2RenderState.cpp
            ${CMAKE_SOURCE_DIR}/../../../../../engine/Src/GLES2Render/GLES2Texture.cpp
            ${CMAKE_SOURCE_DIR}/../../../../../engine/Src/GLES2Render/GLES2TextureManager.cpp
            ${CMAKE_SOURCE_DIR}/../../../../../engine/Src/GLES2Render/GLES2TextureLoaderHelper.cpp
            ${CMAKE_SOURCE_DIR}/../../../../../engine/Src/GLES2Render/GLES2RenderTargetView.cpp
            ${CMAKE_SOURCE_DIR}/../../../../../engine/Src/GLES2Render/GLES2DepthStencilView.cpp
            ${CMAKE_SOURCE_DIR}/../../../../../engine/Src/GLES2Render/GLES2RenderInput.cpp
            ${CMAKE_SOURCE_DIR}/../../../../../engine/Src/GLES2Render/GLES2FrameBuffer.cpp
            ${CMAKE_SOURCE_DIR}/../../../../../engine/Src/GLES2Render/GLES2GPUBuffer.cpp
            ${CMAKE_SOURCE_DIR}/../../../../../logic/Src/World/World.cpp
            ${CMAKE_SOURCE_DIR}/../../../../../logic/Src/World/WorldProvider.cpp
            ${CMAKE_SOURCE_DIR}/../../../../../logic/Src/World/WorldSettings.cpp
            ${CMAKE_SOURCE_DIR}/../../../../../logic/Src/World/SchematicModel.cpp
            ${CMAKE_SOURCE_DIR}/../../../../../logic/Src/Network/RaknetInstance.cpp
            ${CMAKE_SOURCE_DIR}/../../../../../logic/Src/Item/ItemCrafter.cpp
            ${CMAKE_SOURCE_DIR}/../../../../../logic/Src/Entity/EntityBullet.cpp
            ${CMAKE_SOURCE_DIR}/../../../../../logic/Src/Entity/EntityArrow.cpp
            ${CMAKE_SOURCE_DIR}/../../../../../logic/Src/Entity/EntityPotion.cpp
            ${CMAKE_SOURCE_DIR}/../../../../../logic/Src/Entity/EntityVehicle.cpp
            ${CMAKE_SOURCE_DIR}/../../../../../logic/Src/Entity/EntityLandNpc.cpp
            ${CMAKE_SOURCE_DIR}/../../../../../logic/Src/Entity/EntityBuildNpc.cpp
            ${CMAKE_SOURCE_DIR}/../../../../../logic/Src/Entity/EntityCreatureBullet.cpp
            ${CMAKE_SOURCE_DIR}/../../../../../logic/Src/Entity/EntitySkillThrowable.cpp
            ${CMAKE_SOURCE_DIR}/../../../../../logic/Src/Util/LanguageKey.cpp
            ${CMAKE_SOURCE_DIR}/../../../../../logic/Src/Util/ChecksumUtil.cpp
            ${CMAKE_SOURCE_DIR}/../../../../../logic/Src/Util/UThreadLinux.cpp
            ${CMAKE_SOURCE_DIR}/../../../../../logic/Src/Chunk/ChunkReadableStorageFile.cpp
            ${CMAKE_SOURCE_DIR}/../../../../../libraries/src/android/zippp/libzippp.cpp
            ${CMAKE_SOURCE_DIR}/../../../../../libraries/src/android/cpr/session.cpp
            ${CMAKE_SOURCE_DIR}/../../../../../Client/Src/Util/LanguageManager.cpp
            ${CMAKE_SOURCE_DIR}/../../../../../Client/Src/Network/ClientHttpRequest.cpp
            ${CMAKE_SOURCE_DIR}/../../../../../Client/Src/Blockman/Render/VisualTessThread.cpp
            ${CMAKE_SOURCE_DIR}/../../../../../Client/Src/Blockman/cWorld/ImageManager.cpp
            ${CMAKE_SOURCE_DIR}/../../../../../Client/Src/Blockman/GUI/gui_outputConfig.cpp
           )

include_directories(
                    ${CMAKE_SOURCE_DIR}/jni/
                    ${CMAKE_SOURCE_DIR}/../../../../Src/
                    ${CMAKE_SOURCE_DIR}/../../../../Src/External/
                    ${CMAKE_SOURCE_DIR}/../../../../Src/Blockman/
                    ${CMAKE_SOURCE_DIR}/../../../../Src/cItem/
                    ${CMAKE_SOURCE_DIR}/../../../../curl/Android/${CURL_INCLUDE_DIR}/
                    ${CMAKE_SOURCE_DIR}/../../../../../logic/Src/
                    ${CMAKE_SOURCE_DIR}/../../../../../logic/dependencies/Android/
                    ${CMAKE_SOURCE_DIR}/../../../../../logic/dependencies/sparsehash/linux/include/
                    ${CMAKE_SOURCE_DIR}/../../../../../engine/Src/Core/
                    ${CMAKE_SOURCE_DIR}/../../../../../engine/Src/Core/Math/
                    ${CMAKE_SOURCE_DIR}/../../../../../engine/Src/Core/Image/
                    ${CMAKE_SOURCE_DIR}/../../../../../engine/Src/GLES2Render/
                    ${CMAKE_SOURCE_DIR}/../../../../../engine/Dependencies/FreeImage/Android/Include/
                    ${CMAKE_SOURCE_DIR}/../../../../../engine/Dependencies/FreeType/Android/Include/
                    ${CMAKE_SOURCE_DIR}/../../../../../engine/Dependencies/Zlib/Android/Include/
                    ${CMAKE_SOURCE_DIR}/../../../../../engine/Dependencies/RapidXml/Include/
                    ${CMAKE_SOURCE_DIR}/../../../../../engine/Dependencies/rapidjson/Include/
                    ${CMAKE_SOURCE_DIR}/../../../../../libraries/include/android/curl/${CURL_DIR}
                    ${CMAKE_SOURCE_DIR}/../../../../../libraries/include/android/cpr
                    ${CMAKE_SOURCE_DIR}/../../../../../libraries/include/android/zip
                    ${CMAKE_SOURCE_DIR}/../../../../../libraries/include/android/zippp
                    ${CMAKE_SOURCE_DIR}/../../../../../libraries/include/android/g3log
                    ${CMAKE_SOURCE_DIR}/../../../../../libraries/src/android/zip
)


add_library(raknet STATIC IMPORTED)
set_target_properties(raknet PROPERTIES IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/../../../../../libraries/build/lib/${BUILD_TYPE}/${ANDROID_ABI}/libRaknet.a")

add_library(Logic STATIC IMPORTED)
set_target_properties(Logic PROPERTIES IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/../../../../../logic/Bin/${BUILD_TYPE}/${ANDROID_ABI}/libLogic.a")

add_library(FreeType STATIC IMPORTED)
set_target_properties(FreeType PROPERTIES IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/../../../../../libraries/build/lib/${BUILD_TYPE}/${ANDROID_ABI}/libFreeType.a")

add_library(FreeImage STATIC IMPORTED)
set_target_properties(FreeImage PROPERTIES IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/../../../../../libraries/build/lib/${BUILD_TYPE}/${ANDROID_ABI}/libFreeImage.a")

add_library(zlib STATIC IMPORTED)
set_target_properties(zlib PROPERTIES IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/../../../../../libraries/build/lib/${BUILD_TYPE}/${ANDROID_ABI}/libZlib.a")

add_library(Lua STATIC IMPORTED)
set_target_properties(Lua PROPERTIES IMPORTED_LOCATION  "${CMAKE_SOURCE_DIR}/../../../../../libraries/build/lib/${BUILD_TYPE}/${ANDROID_ABI}/libLua.a")

add_library(curl STATIC IMPORTED)
set_target_properties(curl PROPERTIES IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/../../../../curl/Android/Lib/${BUILD_TYPE}/${ANDROID_ABI}/libcurl.a")


add_library(Zip STATIC IMPORTED)
set_target_properties(Zip PROPERTIES IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/../../../../../libraries/build/lib/${BUILD_TYPE}/${ANDROID_ABI}/libZip.a")

add_library(Zippp STATIC IMPORTED)
set_target_properties(Zippp PROPERTIES IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/../../../../../libraries/build/lib/${BUILD_TYPE}/${ANDROID_ABI}/libZippp.a")

add_library(Cpr STATIC IMPORTED)
set_target_properties(Cpr PROPERTIES IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/../../../../../libraries/build/lib/${BUILD_TYPE}/${ANDROID_ABI}/libCpr.a")

add_library(G3log STATIC IMPORTED)
set_target_properties(G3log PROPERTIES IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/../../../../../libraries/build/lib/${BUILD_TYPE}/${ANDROID_ABI}/libG3log.a")

add_library(LordCore STATIC IMPORTED)
set_target_properties(LordCore PROPERTIES IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/../../../../../engine/Bin/${BUILD_TYPE}/${ANDROID_ABI}/libLordCore.a")
#add_dependencies(LordCore FreeType FreeImage )

add_library(LordGLES2Render STATIC IMPORTED)
set_target_properties(LordGLES2Render PROPERTIES IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/../../../../../engine/Bin/${BUILD_TYPE}/${ANDROID_ABI}/libLordGLES2Render.a")

add_library(GameClient STATIC IMPORTED)
set_target_properties(GameClient PROPERTIES IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/../../../../Bin/${BUILD_TYPE}/${ANDROID_ABI}/libGameClient.a")
#add_dependencies(Blockman GameClient)


# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       BlockMan
                       c
                       m
                       dl
                       z
                       EGL
                       GLESv1_CM
                       GLESv2
                       atomic
                       android
                       raknet
                       Logic
                       FreeType
                       FreeImage
                       zlib
                       Zip
                       Zippp
                       Cpr
                       G3log
                       Lua
                       curl
                       LordCore
                       LordGLES2Render
                       GameClient

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib}
                       )