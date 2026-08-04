# The ARMv7 is significanly faster due to the use of the hardware FPU
APP_MODULES := libClothes
#APP_ABI := all
APP_ABI := armeabi-v7a
#armeabi-v7a x86s
#APP_ABI := arm64-v8a
APP_PLATFORM := android-15
APP_STL := gnustl_static

#APP_CPPFLAGS := -std=c++11 -pthread -fexceptions -frtti -D__cplusplus=201403
APP_CPPFLAGS := -std=c++11 -pthread -fexceptions -frtti -ftemplate-depth=8000
APP_CPPFLAGS += -Wno-logical-op-parentheses -Wno-bitwise-op-parentheses -Wno-deprecated-register -Wno-shift-count-overflow -Wno-string-plus-int -Wno-tautological-compare -Wno-return-type -Wno-switch -Wno-null-conversion -Wno-delete-non-virtual-dtor -Wno-self-assign-field -Wno-empty-body -Wno-pointer-bool-conversion -Wno-unused-value -Wno-inconsistent-missing-override -Wno-constant-conversion -Wno-writable-strings -Wno-parentheses -Wno-format-security

ifeq ($(NDK_DEBUG), 1)
APP_OPTIM := debug
else
APP_OPTIM := release
endif
