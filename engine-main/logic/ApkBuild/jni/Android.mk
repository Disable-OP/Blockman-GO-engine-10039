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

#----------------------------------------------------------------------------------------
#lib: libLordClient
include $(CLEAR_VARS)
LOCAL_MODULE := Logic
LOCAL_CXXFLAGS := -DHAVE_PTHREAD
TARGET_OUT := $(LOCAL_PATH)/../../Bin/$(APP_OPTIM)/$(TARGET_ARCH_ABI)

CORE_DIR := $(LOCAL_PATH)/../../../engine/Src/Core
LOGIC_DIR := $(LOCAL_PATH)/../../Src
LOCAL_C_INCLUDES += $(CORE_DIR) \
LOCAL_C_INCLUDES += $(LOGIC_DIR) \
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../engine/Dependencies/rapidjson/Include
LOCAL_C_INCLUDES += $(LOGIC_DIR)/../dependencies/boost_1_63_0
LOCAL_C_INCLUDES += $(LOGIC_DIR)/../dependencies/Android
LOCAL_C_INCLUDES += $(LOGIC_DIR)/../dependencies/sparsehash/linux/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../libraries/include/android/g3log
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../libraries/include/android/cpr/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../libraries/src/android/cpr/

#LOCAL_LDLIBS := -landroid -lc -lm -ldl -llog -lEGL -lGLESv1_CM -lGLESv2
LOGIC_SRC_FILES := $(call rwildcard, $(LOCAL_PATH)/../../Src/, *.cpp)
LOGIC_SRC_FILES := $(LOGIC_SRC_FILES:$(LOCAL_PATH)/%=%)
LOCAL_SRC_FILES := $(LOGIC_SRC_FILES)

include $(BUILD_STATIC_LIBRARY)
#---------------------------------------------------------------------------------------------
