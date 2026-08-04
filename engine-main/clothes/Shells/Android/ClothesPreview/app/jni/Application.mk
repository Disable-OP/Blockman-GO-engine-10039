# The ARMv7 is significanly faster due to the use of the hardware FPU
APP_ABI := armeabi-v7a
APP_PLATFORM := android-15
APP_STL := gnustl_shared
#APP_CFLAG := -g -ggdb -O0 -gstabs+
APP_CPPFLAGS := -fexceptions -frtti -std=c++11 -D__ANDROID__
APP_CPPFLAGS += -Wno-logical-op-parentheses -Wno-bitwise-op-parentheses -Wno-deprecated-register -Wno-shift-count-overflow -Wno-string-plus-int -Wno-tautological-compare -Wno-return-type -Wno-switch -Wno-null-conversion -Wno-delete-non-virtual-dtor -Wno-self-assign-field -Wno-empty-body -Wno-pointer-bool-conversion -Wno-unused-value -Wno-inconsistent-missing-override -Wno-constant-conversion -Wno-writable-strings -Wno-parentheses

APP_ALLOW_MISSING_DEPS=true

ifeq ($(NDK_DEBUG), 1)
APP_OPTIM := debug
else
APP_OPTIM := release
endif

