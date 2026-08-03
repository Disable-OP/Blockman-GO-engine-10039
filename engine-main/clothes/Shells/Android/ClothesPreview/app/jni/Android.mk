LOCAL_PATH := $(call my-dir)

ifeq ($(TARGET_ARCH_ABI), arm64-v8a)
	INCLUDE_DIR:= Include64
else ifeq ($(TARGET_ARCH_ABI), mips64)
	INCLUDE_DIR:= Include64
else ifeq ($(TARGET_ARCH_ABI), x86_64)
	INCLUDE_DIR:= Include64
else
	INCLUDE_DIR:= Include
endif

#$(shell echo $(INCLUDE_DIR)>>$(LOCAL_PATH)/echo_file)

#-----------------------------------------------------------------------------------------

include $(CLEAR_VARS)
LOCAL_MODULE := FreeType
LOCAL_SRC_FILES := ../../../../../../libraries/build/lib/$(APP_OPTIM)/$(TARGET_ARCH_ABI)/libFreeType.a
include $(PREBUILT_STATIC_LIBRARY)

#-----------------------------------------------------------------------------------------

include $(CLEAR_VARS)
LOCAL_MODULE := FreeImage
LOCAL_SRC_FILES := ../../../../../../libraries/build/lib/$(APP_OPTIM)/$(TARGET_ARCH_ABI)/libFreeImage.a
include $(PREBUILT_STATIC_LIBRARY)

#-----------------------------------------------------------------------------------------

include $(CLEAR_VARS)
LOCAL_MODULE := Zlib
LOCAL_SRC_FILES := ../../../../../../libraries/build/lib/$(APP_OPTIM)/$(TARGET_ARCH_ABI)/libZlib.a
include $(PREBUILT_STATIC_LIBRARY)

#-----------------------------------------------------------------------------------------

include $(CLEAR_VARS)
LOCAL_MODULE := Lua
LOCAL_SRC_FILES := ../../../../../../libraries/build/lib/$(APP_OPTIM)/$(TARGET_ARCH_ABI)/libLua.a
include $(PREBUILT_STATIC_LIBRARY)

#-----------------------------------------------------------------------------------------

include $(CLEAR_VARS)
LOCAL_MODULE := Zip
LOCAL_SRC_FILES := ../../../../../../libraries/build/lib/$(APP_OPTIM)/$(TARGET_ARCH_ABI)/libZip.a
include $(PREBUILT_STATIC_LIBRARY)

#-----------------------------------------------------------------------------------------

include $(CLEAR_VARS)
LOCAL_MODULE := Zippp
LOCAL_SRC_FILES := ../../../../../../libraries/build/lib/$(APP_OPTIM)/$(TARGET_ARCH_ABI)/libZippp.a
LOCAL_STATIC_LIBRARIES := Zip
include $(PREBUILT_STATIC_LIBRARY)

#-----------------------------------------------------------------------------------------

include $(CLEAR_VARS)
LOCAL_MODULE := G3log
LOCAL_SRC_FILES := ../../../../../../libraries/build/lib/$(APP_OPTIM)/$(TARGET_ARCH_ABI)/libG3log.a
include $(PREBUILT_STATIC_LIBRARY)

#-----------------------------------------------------------------------------------------

include $(CLEAR_VARS)

LOCAL_MODULE := libLordCore

LOCAL_SRC_FILES := ../../../../../../engine/Bin/$(APP_OPTIM)/$(TARGET_ARCH_ABI)/libLordCore.a

LOCAL_STATIC_LIBRARIES := FreeType FreeImage Zlib

include $(PREBUILT_STATIC_LIBRARY)

#-----------------------------------------------------------------------------------------

include $(CLEAR_VARS)

LOCAL_MODULE := libLordGLES2Render

LOCAL_SRC_FILES := ../../../../../../engine/Bin/$(APP_OPTIM)/$(TARGET_ARCH_ABI)/libLordGLES2Render.a

LOCAL_STATIC_LIBRARIES := libLordCore

include $(PREBUILT_STATIC_LIBRARY)

#-----------------------------------------------------------------------------------------

include $(CLEAR_VARS)

LOCAL_MODULE := libClothes

LOCAL_SRC_FILES := ../../../../../Bin/$(APP_OPTIM)/$(TARGET_ARCH_ABI)/libClothes.a

LOCAL_STATIC_LIBRARIES := libLordCore libLordGLES2Render Lua Zippp Zip G3log

include $(PREBUILT_STATIC_LIBRARY)

#-----------------------------------------------------------------------------------------


include $(CLEAR_VARS)

LOCAL_MODULE    := ClothesPreview

LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../../Src
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../../../engine/Src/Core
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../../../engine/Dependencies/RapidXml/Include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../../../engine/Dependencies/rapidjson/Include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../../../libraries/include/android/g3log

LOCAL_LDLIBS := -landroid -lc -lm -ldl -llog -lz -lEGL -lGLESv1_CM -lGLESv2 -latomic

LOCAL_SRC_FILES := EchoHelper.cpp ClothesPlug.cpp JniHelper.cpp ShellInterfaceAndroid.cpp EditBoxImplAndroid.cpp

LOCAL_STATIC_LIBRARIES := libClothes

LOCAL_LDFLAGS := -Wl,--hash-style=both

include $(BUILD_SHARED_LIBRARY)
