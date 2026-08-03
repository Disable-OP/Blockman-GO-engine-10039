# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH:= $(call my-dir)

#define SearchCoreSrcFiles
#$(patsubst ./%, %, \
#	$(shell cd "../Src/Core"; \
#	find "../../Src/Core" -name "*.cpp" -or -name "*.c" -and -not -name ".*") \
#)
#endef

#define SearchGLES2RenderSrcFiles
#$(patsubst ./%, %, \
#	$(shell cd "../Src/GLES2Render" ; \
#	find "../../Src/GLES2Render" -name "*.cpp" -or -name "*.c" -and -not -name ".*") \
#)
#endef

define rwildcard
	$(wildcard $1$2) $(foreach d, $(wildcard $1*), $(call rwildcard, $d/, $2))
endef

#----------------------------------------------------------------------------------------
#lib: LordCore
include $(CLEAR_VARS)
LOCAL_SHORT_COMMANDS := true
LOCAL_MODULE := libLordCore

TARGET_OUT := $(LOCAL_PATH)/../../Bin/$(APP_OPTIM)/$(TARGET_ARCH_ABI)

LOCAL_CFLAGS += -DFREEIMAGE_LIB=1 -O3

CORE_DIR := $(LOCAL_PATH)/../../Src/Core

LOCAL_C_INCLUDES += $(CORE_DIR)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../Dependencies/nedmalloc/Include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../Dependencies/RapidXml/Include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../Dependencies/rapidjson/Include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../Dependencies/FreeImage/Android/Include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../Dependencies/FreeType/Android/Include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../libraries/include/android/g3log

#LOCAL_SRC_FILES := $(call SearchCoreSrcFiles)

CORE_SRC_FILES := $(call rwildcard, $(LOCAL_PATH)/../../Src/Core/, *.cpp)
CORE_SRC_FILES := $(CORE_SRC_FILES:$(LOCAL_PATH)/%=%)

LOCAL_SRC_FILES := $(CORE_SRC_FILES)

LOCAL_STATIC_LIBRARIES := -landroid -lc -lm -ldl -llog -lgnustl_static

include $(BUILD_STATIC_LIBRARY)

#----------------------------------------------------------------------------------------
#lib: LordGLES2Render
include $(CLEAR_VARS)

LOCAL_MODULE := libLordGLES2Render

TARGET_OUT := $(LOCAL_PATH)/../../Bin/$(APP_OPTIM)/$(TARGET_ARCH_ABI)
LOCAL_CFLAGS += -O3

GLES2RENDER_DIR := $(LOCAL_PATH)/../../Src/GLES2Render

LOCAL_C_INCLUDES += $(CORE_DIR)
LOCAL_C_INCLUDES += $(GLES2RENDER_DIR)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../libraries/include/android/g3log

#LOCAL_CFLAGS := -DGL_GLEXT_PROTOTYPES=1

LOCAL_STATIC_LIBRARIES := -landroid -lc -lm -ldl -llog -lEGL -lGLESv1_CM -lGLESv2

#LOCAL_SRC_FILES := $(call SearchGLES2RenderSrcFiles)

GLES2RENDER_SRC_FILES := $(call rwildcard, $(LOCAL_PATH)/../../Src/GLES2Render/, *.cpp)
GLES2RENDER_SRC_FILES := $(GLES2RENDER_SRC_FILES:$(LOCAL_PATH)/%=%)

LOCAL_SRC_FILES := $(GLES2RENDER_SRC_FILES)

include $(BUILD_STATIC_LIBRARY)
#----------------------------------------------------------------------------------------
