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
include $(CLEAR_VARS)
LOCAL_MODULE := raknet

TARGET_OUT := $(LOCAL_PATH)/../../Android/raknet/lib/$(APP_OPTIM)/$(TARGET_ARCH_ABI)

LOCAL_SRC_FILES := $(call rwildcard, $(LOCAL_PATH)/../../Android/raknet/include/, *.cpp)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)

LOCAL_SRC_FILES := $(LOCAL_SRC_FILES)
include $(BUILD_STATIC_LIBRARY)

#-------------------------------------------------------------------------------------
