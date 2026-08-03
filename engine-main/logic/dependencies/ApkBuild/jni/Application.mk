# The ARMv7 is significanly faster due to the use of the hardware FPU
APP_MODULES := raknet
APP_ABI := armeabi-v7a
APP_PLATFORM := android-15
APP_STL := gnustl_static
APP_CPPFLAGS := -std=c++11 -pthread -fexceptions -frtti
ifeq ($(NDK_DEBUG), 1)
APP_OPTIM := debug
else
APP_OPTIM := release
endif
