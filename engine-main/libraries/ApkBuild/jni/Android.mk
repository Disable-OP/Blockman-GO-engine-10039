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

$(warning  "LOCAL_PATH==================== $(LOCAL_PATH)") 

##----------------------------------------------------------------------------------------

include $(CLEAR_VARS)
LOCAL_MODULE := Raknet

TARGET_OUT := $(LOCAL_PATH)/../../build/lib/$(APP_OPTIM)/$(TARGET_ARCH_ABI)

LOCAL_SRC_FILES := $(call rwildcard, $(LOCAL_PATH)/../../src/android/raknet/, *.cpp)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)

LOCAL_SRC_FILES := $(LOCAL_SRC_FILES)
include $(BUILD_STATIC_LIBRARY)

#----------------------------------------------------------------------------------------

include $(CLEAR_VARS)

LOCAL_MODULE := FreeType
TARGET_OUT := $(LOCAL_PATH)/../../build/lib/$(APP_OPTIM)/$(TARGET_ARCH_ABI)


LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../include/android/freeType/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../include/android/freeType/freetype/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../src/android/freeType/

LOCAL_CFLAGS += -DFT2_BUILD_LIBRARY -O3

LOCAL_SRC_FILES :=\
	../../src/android/freeType/autofit/autofit.c\
	../../src/android/freeType/bdf/bdf.c\
	../../src/android/freeType/cff/cff.c\
	../../src/android/freeType/base/ftbase.c\
	../../src/android/freeType/base/ftbitmap.c\
	../../src/android/freeType/cache/ftcache.c\
	../../src/android/freeType/base/ftfstype.c\
	../../src/android/freeType/base/ftgasp.c\
	../../src/android/freeType/base/ftglyph.c\
	../../src/android/freeType/gzip/ftgzip.c\
	../../src/android/freeType/base/ftinit.c\
	../../src/android/freeType/lzw/ftlzw.c\
	../../src/android/freeType/base/ftstroke.c\
	../../src/android/freeType/base/ftsystem.c\
	../../src/android/freeType/smooth/smooth.c\
	../../src/android/freeType/base/ftbbox.c\
	../../src/android/freeType/base/ftgxval.c\
	../../src/android/freeType/base/ftlcdfil.c\
	../../src/android/freeType/base/ftmm.c\
	../../src/android/freeType/base/ftotval.c\
	../../src/android/freeType/base/ftpatent.c\
	../../src/android/freeType/base/ftpfr.c\
	../../src/android/freeType/base/ftsynth.c\
	../../src/android/freeType/base/fttype1.c\
	../../src/android/freeType/base/ftwinfnt.c\
	../../src/android/freeType/base/ftxf86.c\
	../../src/android/freeType/pcf/pcf.c\
	../../src/android/freeType/pfr/pfr.c\
	../../src/android/freeType/psaux/psaux.c\
	../../src/android/freeType/pshinter/pshinter.c\
	../../src/android/freeType/psnames/psmodule.c\
	../../src/android/freeType/raster/raster.c\
	../../src/android/freeType/sfnt/sfnt.c\
	../../src/android/freeType/truetype/truetype.c\
	../../src/android/freeType/type1/type1.c\
	../../src/android/freeType/cid/type1cid.c\
	../../src/android/freeType/type42/type42.c\
	../../src/android/freeType/winfonts/winfnt.c\

include $(BUILD_STATIC_LIBRARY)

#----------------------------------------------------------------------------------------

include $(CLEAR_VARS)

LOCAL_MODULE := FreeImage
TARGET_OUT := $(LOCAL_PATH)/../../build/lib/$(APP_OPTIM)/$(TARGET_ARCH_ABI)
LOCAL_LDLIBS := -ldl -lz
LOCAL_CFLAGS := -DFREEIMAGE_LIB=1 -DPNG_STATIC=1 -O3

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../include/android/freeImage
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../src/android/freeImage/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../src/android/freeImage/Metadata
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../src/android/freeImage/DeprecationManager

LOCAL_SRC_FILES :=\
    ../../src/android/freeImage/DeprecationManager/Deprecated.cpp\
    ../../src/android/freeImage/DeprecationManager/DeprecationMgr.cpp\
    ../../src/android/freeImage/FreeImage/BitmapAccess.cpp\
    ../../src/android/freeImage/FreeImage/CacheFile.cpp\
    ../../src/android/freeImage/FreeImage/ColorLookup.cpp\
    ../../src/android/freeImage/FreeImage/Conversion.cpp\
    ../../src/android/freeImage/FreeImage/ConversionFloat.cpp\
    ../../src/android/freeImage/FreeImage/Conversion4.cpp\
    ../../src/android/freeImage/FreeImage/Conversion8.cpp\
    ../../src/android/freeImage/FreeImage/Conversion16_555.cpp\
    ../../src/android/freeImage/FreeImage/Conversion16_565.cpp\
    ../../src/android/freeImage/FreeImage/Conversion24.cpp\
    ../../src/android/freeImage/FreeImage/Conversion32.cpp\
    ../../src/android/freeImage/FreeImage/ConversionRGB16.cpp\
    ../../src/android/freeImage/FreeImage/ConversionRGBF.cpp\
    ../../src/android/freeImage/FreeImage/ConversionUINT16.cpp\
    ../../src/android/freeImage/FreeImage/ConversionType.cpp\
    ../../src/android/freeImage/FreeImage/FreeImage.cpp\
    ../../src/android/freeImage/FreeImage/FreeImageIO.cpp\
    ../../src/android/freeImage/FreeImage/GetType.cpp\
    ../../src/android/freeImage/FreeImage/Halftoning.cpp\
    ../../src/android/freeImage/FreeImage/J2KHelper.cpp\
    ../../src/android/freeImage/FreeImage/MemoryIO.cpp\
    ../../src/android/freeImage/FreeImage/MultiPage.cpp\
    ../../src/android/freeImage/FreeImage/NNQuantizer.cpp\
    ../../src/android/freeImage/FreeImage/PixelAccess.cpp\
    ../../src/android/freeImage/FreeImage/Plugin.cpp\
    ../../src/android/freeImage/FreeImage/PluginBMP.cpp\
    ../../src/android/freeImage/FreeImage/PluginDDS.cpp\
    ../../src/android/freeImage/FreeImage/PluginGIF.cpp\
    ../../src/android/freeImage/FreeImage/PluginHDR.cpp\
    ../../src/android/freeImage/FreeImage/PluginICO.cpp\
    ../../src/android/freeImage/FreeImage/PluginJPEG.cpp\
    ../../src/android/freeImage/FreeImage/PluginPNG.cpp\
    ../../src/android/freeImage/FreeImage/PluginTARGA.cpp\
    ../../src/android/freeImage/FreeImage/ToneMapping.cpp\
    ../../src/android/freeImage/FreeImage/tmoColorConvert.cpp\
    ../../src/android/freeImage/FreeImage/tmoDrago03.cpp\
    ../../src/android/freeImage/FreeImage/tmoFattal02.cpp\
    ../../src/android/freeImage/FreeImage/tmoReinhard05.cpp\
    ../../src/android/freeImage/FreeImage/WuQuantizer.cpp\
    ../../src/android/freeImage/FreeImage/ZLibInterface.cpp\
    ../../src/android/freeImage/FreeImageToolkit/Background.cpp\
    ../../src/android/freeImage/FreeImageToolkit/Colors.cpp\
    ../../src/android/freeImage/FreeImageToolkit/Flip.cpp\
    ../../src/android/freeImage/FreeImageToolkit/Resize.cpp\
    ../../src/android/freeImage/FreeImageToolkit/BSplineRotate.cpp\
    ../../src/android/freeImage/FreeImageToolkit/CopyPaste.cpp\
    ../../src/android/freeImage/FreeImageToolkit/JPEGTransform.cpp\
    ../../src/android/freeImage/FreeImageToolkit/Channels.cpp\
    ../../src/android/freeImage/FreeImageToolkit/Display.cpp\
    ../../src/android/freeImage/FreeImageToolkit/MultigridPoissonSolver.cpp\
    ../../src/android/freeImage/FreeImageToolkit/ClassicRotate.cpp\
    ../../src/android/freeImage/FreeImageToolkit/Rescale.cpp\
    ../../src/android/freeImage/LibJPEG/ckconfig.c\
    ../../src/android/freeImage/LibJPEG/jaricom.c\
    ../../src/android/freeImage/LibJPEG/jcapimin.c\
    ../../src/android/freeImage/LibJPEG/jcapistd.c\
    ../../src/android/freeImage/LibJPEG/jcarith.c\
    ../../src/android/freeImage/LibJPEG/jccoefct.c\
    ../../src/android/freeImage/LibJPEG/jccolor.c\
    ../../src/android/freeImage/LibJPEG/jcdctmgr.c\
    ../../src/android/freeImage/LibJPEG/jchuff.c\
    ../../src/android/freeImage/LibJPEG/jcinit.c\
    ../../src/android/freeImage/LibJPEG/jcmainct.c\
    ../../src/android/freeImage/LibJPEG/jcmarker.c\
    ../../src/android/freeImage/LibJPEG/jcmaster.c\
    ../../src/android/freeImage/LibJPEG/jcomapi.c\
    ../../src/android/freeImage/LibJPEG/jconfig.txt\
    ../../src/android/freeImage/LibJPEG/jcparam.c\
    ../../src/android/freeImage/LibJPEG/jcprepct.c\
    ../../src/android/freeImage/LibJPEG/jcsample.c\
    ../../src/android/freeImage/LibJPEG/jctrans.c\
    ../../src/android/freeImage/LibJPEG/jdapimin.c\
    ../../src/android/freeImage/LibJPEG/jdapistd.c\
    ../../src/android/freeImage/LibJPEG/jdarith.c\
    ../../src/android/freeImage/LibJPEG/jdatadst.c\
    ../../src/android/freeImage/LibJPEG/jdatasrc.c\
    ../../src/android/freeImage/LibJPEG/jdcoefct.c\
    ../../src/android/freeImage/LibJPEG/jdcolor.c\
    ../../src/android/freeImage/LibJPEG/jddctmgr.c\
    ../../src/android/freeImage/LibJPEG/jdhuff.c\
    ../../src/android/freeImage/LibJPEG/jdinput.c\
    ../../src/android/freeImage/LibJPEG/jdmainct.c\
    ../../src/android/freeImage/LibJPEG/jdmarker.c\
    ../../src/android/freeImage/LibJPEG/jdmaster.c\
    ../../src/android/freeImage/LibJPEG/jdmerge.c\
    ../../src/android/freeImage/LibJPEG/jdpostct.c\
    ../../src/android/freeImage/LibJPEG/jdsample.c\
    ../../src/android/freeImage/LibJPEG/jdtrans.c\
    ../../src/android/freeImage/LibJPEG/jerror.c\
    ../../src/android/freeImage/LibJPEG/jfdctflt.c\
    ../../src/android/freeImage/LibJPEG/jfdctfst.c\
    ../../src/android/freeImage/LibJPEG/jfdctint.c\
    ../../src/android/freeImage/LibJPEG/jidctflt.c\
    ../../src/android/freeImage/LibJPEG/jidctfst.c\
    ../../src/android/freeImage/LibJPEG/jidctint.c\
    ../../src/android/freeImage/LibJPEG/jmemmgr.c\
    ../../src/android/freeImage/LibJPEG/jmemnobs.c\
    ../../src/android/freeImage/LibJPEG/jquant1.c\
    ../../src/android/freeImage/LibJPEG/jquant2.c\
    ../../src/android/freeImage/LibJPEG/jutils.c\
    ../../src/android/freeImage/LibJPEG/rdbmp.c\
    ../../src/android/freeImage/LibJPEG/rdcolmap.c\
    ../../src/android/freeImage/LibJPEG/rdgif.c\
    ../../src/android/freeImage/LibJPEG/rdppm.c\
    ../../src/android/freeImage/LibJPEG/rdrle.c\
    ../../src/android/freeImage/LibJPEG/rdswitch.c\
    ../../src/android/freeImage/LibJPEG/rdtarga.c\
    ../../src/android/freeImage/LibJPEG/transupp.c\
    ../../src/android/freeImage/LibJPEG/wrbmp.c\
    ../../src/android/freeImage/LibJPEG/wrgif.c\
    ../../src/android/freeImage/LibJPEG/wrppm.c\
    ../../src/android/freeImage/LibJPEG/wrrle.c\
    ../../src/android/freeImage/LibJPEG/wrtarga.c\
    ../../src/android/freeImage/LibPNG/png.c\
    ../../src/android/freeImage/LibPNG/pngerror.c\
    ../../src/android/freeImage/LibPNG/pngget.c\
    ../../src/android/freeImage/LibPNG/pngmem.c\
    ../../src/android/freeImage/LibPNG/pngpread.c\
    ../../src/android/freeImage/LibPNG/pngread.c\
    ../../src/android/freeImage/LibPNG/pngrio.c\
    ../../src/android/freeImage/LibPNG/pngrtran.c\
    ../../src/android/freeImage/LibPNG/pngrutil.c\
    ../../src/android/freeImage/LibPNG/pngset.c\
    ../../src/android/freeImage/LibPNG/pngtrans.c\
    ../../src/android/freeImage/LibPNG/pngwio.c\
    ../../src/android/freeImage/LibPNG/pngwrite.c\
    ../../src/android/freeImage/LibPNG/pngwtran.c\
    ../../src/android/freeImage/LibPNG/pngwutil.c\
    ../../src/android/freeImage/Metadata/Exif.cpp\
    ../../src/android/freeImage/Metadata/FIRational.cpp\
    ../../src/android/freeImage/Metadata/FreeImageTag.cpp\
    ../../src/android/freeImage/Metadata/IPTC.cpp\
    ../../src/android/freeImage/Metadata/TagConversion.cpp\
    ../../src/android/freeImage/Metadata/TagLib.cpp\
    ../../src/android/freeImage/LibOpenJPEG/bio.c\
    ../../src/android/freeImage/LibOpenJPEG/cidx_manager.c\
    ../../src/android/freeImage/LibOpenJPEG/cio.c\
    ../../src/android/freeImage/LibOpenJPEG/dwt.c\
    ../../src/android/freeImage/LibOpenJPEG/event.c\
    ../../src/android/freeImage/LibOpenJPEG/image.c\
    ../../src/android/freeImage/LibOpenJPEG/j2k.c\
    ../../src/android/freeImage/LibOpenJPEG/j2k_lib.c\
    ../../src/android/freeImage/LibOpenJPEG/jp2.c\
    ../../src/android/freeImage/LibOpenJPEG/jpt.c\
    ../../src/android/freeImage/LibOpenJPEG/mct.c\
    ../../src/android/freeImage/LibOpenJPEG/mct.h\
    ../../src/android/freeImage/LibOpenJPEG/mqc.c\
    ../../src/android/freeImage/LibOpenJPEG/mqc.h\
    ../../src/android/freeImage/LibOpenJPEG/openjpeg.c\
    ../../src/android/freeImage/LibOpenJPEG/phix_manager.c\
    ../../src/android/freeImage/LibOpenJPEG/pi.c\
    ../../src/android/freeImage/LibOpenJPEG/ppix_manager.c\
    ../../src/android/freeImage/LibOpenJPEG/raw.c\
    ../../src/android/freeImage/LibOpenJPEG/t1.c\
    ../../src/android/freeImage/LibOpenJPEG/t2.c\
    ../../src/android/freeImage/LibOpenJPEG/tcd.c\
    ../../src/android/freeImage/LibOpenJPEG/tgt.c\
    ../../src/android/freeImage/LibOpenJPEG/thix_manager.c\
    ../../src/android/freeImage/LibOpenJPEG/tpix_manager.c\
	
include $(BUILD_STATIC_LIBRARY)

##----------------------------------------------------------------------------------------

include $(CLEAR_VARS)
LOCAL_MODULE := Zlib

TARGET_OUT := $(LOCAL_PATH)/../../build/lib/$(APP_OPTIM)/$(TARGET_ARCH_ABI)
LOCAL_CFLAGS += -O3

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../include/android/zlib/
LOCAL_SRC_FILES := $(call rwildcard, $(LOCAL_PATH)/../../src/android/zlib/, *.cpp)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)

LOCAL_SRC_FILES := $(LOCAL_SRC_FILES)
include $(BUILD_STATIC_LIBRARY)

#----------------------------------------------------------------------------------------

include $(CLEAR_VARS)
LOCAL_MODULE := Lua

TARGET_OUT := $(LOCAL_PATH)/../../build/lib/$(APP_OPTIM)/$(TARGET_ARCH_ABI)

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../include/android/lua/
LOCAL_SRC_FILES := $(call rwildcard, $(LOCAL_PATH)/../../src/android/lua/, *.cpp)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)

LOCAL_SRC_FILES := $(LOCAL_SRC_FILES)
include $(BUILD_STATIC_LIBRARY)

##----------------------------------------------------------------------------------------

include $(CLEAR_VARS)
LOCAL_MODULE := Zip
LOCAL_CFLAGS += -DHAVE_CONFIG_H -Dzip_EXPORTS -DINTERNAL=static -DSIZEOF_SIZE_T=8
TARGET_OUT := $(LOCAL_PATH)/../../build/lib/$(APP_OPTIM)/$(TARGET_ARCH_ABI)
LOCAL_CFLAGS += -O3

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../include/android/zip/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../src/android/zip/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../src/android/zip/gladman-fcrypt/

LOCAL_SRC_FILES := $(call rwildcard, $(LOCAL_PATH)/../../src/android/zip/, *.cpp)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)

LOCAL_SRC_FILES := $(LOCAL_SRC_FILES)
LOCAL_STATIC_LIBRARIES := Zlib
include $(BUILD_STATIC_LIBRARY)

##----------------------------------------------------------------------------------------

include $(CLEAR_VARS)
LOCAL_MODULE := Zippp

TARGET_OUT := $(LOCAL_PATH)/../../build/lib/$(APP_OPTIM)/$(TARGET_ARCH_ABI)
LOCAL_CFLAGS += -O3

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../include/android/zip/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../src/android/zippp/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../src/android/zip/

LOCAL_SRC_FILES := $(call rwildcard, $(LOCAL_PATH)/../../src/android/zippp/, *.cpp)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)

LOCAL_SRC_FILES := $(LOCAL_SRC_FILES)
LOCAL_STATIC_LIBRARIES := Zip
include $(BUILD_STATIC_LIBRARY)

##----------------------------------------------------------------------------------------
# include $(CLEAR_VARS)
# include $(LOCAL_PATH)/../../src/android/curl/Makefile.inc
# LOCAL_MODULE := curl

# ifeq ($(TARGET_ARCH_ABI), arm64-v8a)
	# CURL_DIR:= 64
# else ifeq ($(TARGET_ARCH_ABI), mips64)
	# CURL_DIR:= 64
# else ifeq ($(TARGET_ARCH_ABI), x86_64)
	# CURL_DIR:= 64
# else
	# CURL_DIR:= 32
# endif

# TARGET_OUT := $(LOCAL_PATH)/../../build/lib/$(APP_OPTIM)/$(TARGET_ARCH_ABI)

# include $(LOCAL_PATH)/../../src/android/curl/Makefile.inc 
# LOCAL_CFLAGS += -Wpointer-arith -Wwrite-strings -Wunused -Winline -Wnested-externs -Wmissing-declarations -Wmissing-prototypes -Wno-long-long -Wfloat-equal -Wno-multichar -Wsign-compare -Wno-format-nonliteral -Wendif-labels -Wstrict-prototypes -Wdeclaration-after-statement -Wno-system-headers -DHAVE_CONFIG_H  
# LOCAL_CFLAGS += -Dsread -Dswrite -DHAVE_STRUCT_TIMEVAL

# LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../include/android/curl/$(CURL_DIR)/
# LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../include/android/curl/
# LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../include/android/zlib/
# LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../src/android/curl/

# LOCAL_SRC_FILES := $(call rwildcard, $(LOCAL_PATH)/../../src/android/curl/, *.cpp)
# LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)

# LOCAL_SRC_FILES := $(LOCAL_SRC_FILES)
# LOCAL_STATIC_LIBRARIES := zlib
# include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := curl
#LOCAL_SRC_FILES := ../../build/lib/$(APP_OPTIM)/$(TARGET_ARCH_ABI)/libcurl.a
LOCAL_SRC_FILES := ../../../client/curl/Android/Lib/$(APP_OPTIM)/$(TARGET_ARCH_ABI)/libcurl.a
include $(PREBUILT_STATIC_LIBRARY)

##----------------------------------------------------------------------------------------

include $(CLEAR_VARS)
LOCAL_MODULE := Cpr
ifeq ($(TARGET_ARCH_ABI), arm64-v8a)
	CURL_DIR:= 64
else ifeq ($(TARGET_ARCH_ABI), mips64)
	CURL_DIR:= 64
else ifeq ($(TARGET_ARCH_ABI), x86_64)
	CURL_DIR:= 64
else
	CURL_DIR:= 32
endif

TARGET_OUT := $(LOCAL_PATH)/../../build/lib/$(APP_OPTIM)/$(TARGET_ARCH_ABI)
LOCAL_CFLAGS += -O3
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../include/android/curl/$(CURL_DIR)/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../include/android/cpr/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../src/android/cpr/

LOCAL_SRC_FILES := $(call rwildcard, $(LOCAL_PATH)/../../src/android/cpr/, *.cpp)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)

LOCAL_SRC_FILES := $(LOCAL_SRC_FILES)
LOCAL_STATIC_LIBRARIES := curl
include $(BUILD_STATIC_LIBRARY)

##----------------------------------------------------------------------------------------

include $(CLEAR_VARS)
LOCAL_MODULE := G3log

TARGET_OUT := $(LOCAL_PATH)/../../build/lib/$(APP_OPTIM)/$(TARGET_ARCH_ABI)
LOCAL_CFLAGS += -O3

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../include/android/g3log
LOCAL_SRC_FILES := $(call rwildcard, $(LOCAL_PATH)/../../src/android/g3log/, *.cpp)
LOCAL_SRC_FILES := $(call rwildcard, $(LOCAL_PATH)/../../src/android/g3log/, *.c)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)

LOCAL_SRC_FILES := $(LOCAL_SRC_FILES)
include $(BUILD_STATIC_LIBRARY)
