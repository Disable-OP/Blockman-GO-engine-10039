# The ARMv7 is significanly faster due to the use of the hardware FPU
APP_MODULES := libLordCore libLordGLES2Render
APP_ABI := armeabi-v7a
APP_PLATFORM := android-15
APP_STL := gnustl_static
LOCAL_CXXFLAGS := -gstabs+
APP_CPPFLAGS := -std=c++11 -pthread -fexceptions -frtti
APP_CPPFLAGS += -Wno-logical-op-parentheses -Wno-bitwise-op-parentheses -Wno-deprecated-register -Wno-shift-count-overflow -Wno-string-plus-int -Wno-tautological-compare -Wno-return-type -Wno-switch -Wno-null-conversion -Wno-delete-non-virtual-dtor -Wno-self-assign-field -Wno-empty-body -Wno-pointer-bool-conversion -Wno-unused-value -Wno-inconsistent-missing-override -Wno-constant-conversion -Wno-writable-strings -Wno-parentheses -Wno-format-security
APP_SHORT_COMMANDS := true
ifeq ($(NDK_DEBUG), 1)
APP_OPTIM := debug
else
APP_OPTIM := release
endif
