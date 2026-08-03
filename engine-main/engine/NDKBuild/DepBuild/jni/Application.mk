# The ARMv7 is significanly faster due to the use of the hardware FPU
APP_MODULES := libFreeImage libFreeType libzlib libLua
#APP_ABI := all
#APP_ABI := armeabi
APP_ABI := armeabi-v7a
#armeabi-v7a x86
#APP_ABI := arm64-v8a
APP_PLATFORM := android-15
APP_STL := gnustl_static
APP_CPPFLAGS := -std=c++11 -frtti -fexceptions
ifeq ($(NDK_DEBUG), 1)
APP_OPTIM := debug
else
APP_OPTIM := release
endif
