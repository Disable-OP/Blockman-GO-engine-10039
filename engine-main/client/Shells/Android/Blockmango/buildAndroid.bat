@echo off

setlocal enabledelayedexpansion

set PROJECT_DIR="%cd%"
echo %PROJECT_DIR%

set BUILD_TYP=0
set APP_APP_ABI="armeabi-v7a"

SET PATH=%PATH%;D:\2023-N3XT\Ultra-Go-2024\Android\Tools\NDK

echo info : [BUILD_TYP = %BUILD_TYP%] [APP_APP_ABI = %APP_APP_ABI%]



cd %PROJECT_DIR%/../../../ApkBuild/
call ndk-build NDK_DEBUG=%BUILD_TYP% APP_ABI=%APP_APP_ABI% -j4

cd %PROJECT_DIR%/app/
call ndk-build NDK_DEBUG=%BUILD_TYP% APP_ABI=%APP_APP_ABI% -j4

cd %PROJECT_DIR%