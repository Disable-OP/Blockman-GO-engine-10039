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

define rwildcard
	$(wildcard $1$2) $(foreach d, $(wildcard $1*), $(call rwildcard, $d/, $2))
endef

ifeq ($(TARGET_ARCH_ABI), arm64-v8a)
	INCLUDE_DIR:= Include64
else ifeq ($(TARGET_ARCH_ABI), mips64)
	INCLUDE_DIR:= Include64
else ifeq ($(TARGET_ARCH_ABI), x86_64)
	INCLUDE_DIR:= Include64
else
	INCLUDE_DIR:= Include
endif

$(warning  $(INCLUDE_DIR))

#lib: libLordClient
include $(CLEAR_VARS)

LOCAL_MODULE := libGameClient

LOCAL_CXXFLAGS := -DHAVE_PTHREAD

TARGET_OUT := $(LOCAL_PATH)/../../Bin/$(APP_OPTIM)/$(TARGET_ARCH_ABI)
LOCAL_CFLAGS += -O3

CORE_DIR := $(LOCAL_PATH)/../../../engine/Src/Core
GLES2RENDER_DIR := $(LOCAL_PATH)/../../../engine/Src/GLES2Render
LOGIC_DIR := $(LOCAL_PATH)/../../../logic/Src

GAMECLIENT_DIR := $(LOCAL_PATH)/../../Src

LOCAL_C_INCLUDES += $(CORE_DIR)
LOCAL_C_INCLUDES += $(GLES2RENDER_DIR)
LOCAL_C_INCLUDES += $(GAMECLIENT_DIR)
LOCAL_C_INCLUDES += $(LOGIC_DIR)
LOCAL_C_INCLUDES += $(GAMECLIENT_DIR)/Blockman
LOCAL_C_INCLUDES += $(GAMECLIENT_DIR)/External
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../engine/Dependencies/RapidXml/Include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../engine/Dependencies/rapidjson/Include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../engine/Dependencies/Lua/Android/Include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../curl/Android/$(INCLUDE_DIR)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../engine/Dependencies/Lua/Android/Include
LOCAL_C_INCLUDES += $(LOGIC_DIR)/../dependencies/linux
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../libraries/include/android/cpr/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../libraries/src/android/cpr/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../libraries/src/android/zippp/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../libraries/include/android/g3log/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../logic/dependencies/sparsehash/linux/include

#LOCAL_LDLIBS := -landroid -lc -lm -ldl -llog -lEGL -lGLESv1_CM -lGLESv2

GAMECLIENT_SRC_FILES := $(call rwildcard, $(LOCAL_PATH)/../../Src/, *.cpp)
GAMECLIENT_SRC_FILES := $(GAMECLIENT_SRC_FILES:$(LOCAL_PATH)/%=%)

LOCAL_SRC_FILES := $(GAMECLIENT_SRC_FILES)

include $(BUILD_STATIC_LIBRARY)
#---------------------------------------------------------------------------------------------
