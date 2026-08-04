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

#lib----libFreeType
include $(CLEAR_VARS)

LOCAL_MODULE := libFreeType
TARGET_OUT := $(LOCAL_PATH)/../../../Dependencies/FreeType/Android/Lib/$(APP_OPTIM)/$(TARGET_ARCH_ABI)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/freetype/include \
LOCAL_C_INCLUDES +=	$(LOCAL_PATH)/freetype/include/freetype \
LOCAL_C_INCLUDES +=	$(LOCAL_PATH)/freetype/src

LOCAL_CFLAGS := -DFT2_BUILD_LIBRARY

LOCAL_SRC_FILES :=\
			freetype/src/autofit/autofit.c \
			freetype/src/bdf/bdf.c \
			freetype/src/cff/cff.c \
			freetype/src/base/ftbase.c \
			freetype/src/base/ftbitmap.c \
			freetype/src/cache/ftcache.c \
			freetype/src/base/ftfstype.c \
			freetype/src/base/ftgasp.c \
			freetype/src/base/ftglyph.c \
			freetype/src/gzip/ftgzip.c \
			freetype/src/base/ftinit.c \
			freetype/src/lzw/ftlzw.c \
			freetype/src/base/ftstroke.c \
			freetype/src/base/ftsystem.c \
			freetype/src/smooth/smooth.c \
			\
			freetype/src/base/ftbbox.c \
			freetype/src/base/ftgxval.c \
			freetype/src/base/ftlcdfil.c \
			freetype/src/base/ftmm.c \
			freetype/src/base/ftotval.c \
			freetype/src/base/ftpatent.c \
			freetype/src/base/ftpfr.c \
			freetype/src/base/ftsynth.c \
			freetype/src/base/fttype1.c \
			freetype/src/base/ftwinfnt.c \
			freetype/src/base/ftxf86.c \
			freetype/src/pcf/pcf.c \
			freetype/src/pfr/pfr.c \
			freetype/src/psaux/psaux.c \
			freetype/src/pshinter/pshinter.c \
			freetype/src/psnames/psmodule.c \
			freetype/src/raster/raster.c \
			freetype/src/sfnt/sfnt.c \
			freetype/src/truetype/truetype.c \
			freetype/src/type1/type1.c \
			freetype/src/cid/type1cid.c \
			freetype/src/type42/type42.c \
			freetype/src/winfonts/winfnt.c \
			
include $(BUILD_STATIC_LIBRARY)

#lib-----FreeImage
include $(CLEAR_VARS)

LOCAL_MODULE := libFreeImage
TARGET_OUT := $(LOCAL_PATH)/../../../Dependencies/FreeImage/Android/Lib/$(APP_OPTIM)/$(TARGET_ARCH_ABI)
LOCAL_LDLIBS := -ldl -lz
LOCAL_CFLAGS := -DFREEIMAGE_LIB=1 -DPNG_STATIC=1

LOCAL_C_INCLUDES:= \
$(LOCAL_PATH)/freeimage \
$(LOCAL_PATH)/freeimage/Metadata \
$(LOCAL_PATH)/freeimage/DeprecationManager

LOCAL_SRC_FILES :=\
    freeimage/DeprecationManager/Deprecated.cpp\
    freeimage/DeprecationManager/DeprecationMgr.cpp\
    freeimage/FreeImage/BitmapAccess.cpp\
    freeimage/FreeImage/CacheFile.cpp\
    freeimage/FreeImage/ColorLookup.cpp\
    freeimage/FreeImage/Conversion.cpp\
    freeimage/FreeImage/ConversionFloat.cpp\
    freeimage/FreeImage/Conversion4.cpp\
    freeimage/FreeImage/Conversion8.cpp\
    freeimage/FreeImage/Conversion16_555.cpp\
    freeimage/FreeImage/Conversion16_565.cpp\
    freeimage/FreeImage/Conversion24.cpp\
    freeimage/FreeImage/Conversion32.cpp\
    freeimage/FreeImage/ConversionRGB16.cpp\
    freeimage/FreeImage/ConversionRGBF.cpp\
    freeimage/FreeImage/ConversionUINT16.cpp\
    freeimage/FreeImage/ConversionType.cpp\
    freeimage/FreeImage/FreeImage.cpp\
    freeimage/FreeImage/FreeImageIO.cpp\
    freeimage/FreeImage/GetType.cpp\
    freeimage/FreeImage/Halftoning.cpp\
    freeimage/FreeImage/J2KHelper.cpp\
    freeimage/FreeImage/MemoryIO.cpp\
    freeimage/FreeImage/MultiPage.cpp\
    freeimage/FreeImage/NNQuantizer.cpp\
    freeimage/FreeImage/PixelAccess.cpp\
    freeimage/FreeImage/Plugin.cpp\
    freeimage/FreeImage/PluginBMP.cpp\
    freeimage/FreeImage/PluginDDS.cpp\
    freeimage/FreeImage/PluginGIF.cpp\
    freeimage/FreeImage/PluginHDR.cpp\
    freeimage/FreeImage/PluginICO.cpp\
    freeimage/FreeImage/PluginJPEG.cpp\
    freeimage/FreeImage/PluginPNG.cpp\
    freeimage/FreeImage/PluginTARGA.cpp\
    freeimage/FreeImage/ToneMapping.cpp\
    freeimage/FreeImage/tmoColorConvert.cpp\
    freeimage/FreeImage/tmoDrago03.cpp\
    freeimage/FreeImage/tmoFattal02.cpp\
    freeimage/FreeImage/tmoReinhard05.cpp\
    freeimage/FreeImage/WuQuantizer.cpp\
    freeimage/FreeImage/ZLibInterface.cpp\
    freeimage/FreeImageToolkit/Background.cpp\
    freeimage/FreeImageToolkit/Colors.cpp\
    freeimage/FreeImageToolkit/Flip.cpp\
    freeimage/FreeImageToolkit/Resize.cpp\
    freeimage/FreeImageToolkit/BSplineRotate.cpp\
    freeimage/FreeImageToolkit/CopyPaste.cpp\
    freeimage/FreeImageToolkit/JPEGTransform.cpp\
    freeimage/FreeImageToolkit/Channels.cpp\
    freeimage/FreeImageToolkit/Display.cpp\
    freeimage/FreeImageToolkit/MultigridPoissonSolver.cpp\
    freeimage/FreeImageToolkit/ClassicRotate.cpp\
    freeimage/FreeImageToolkit/Rescale.cpp\
    freeimage/LibJPEG/ckconfig.c\
    freeimage/LibJPEG/jaricom.c\
    freeimage/LibJPEG/jcapimin.c\
    freeimage/LibJPEG/jcapistd.c\
    freeimage/LibJPEG/jcarith.c\
    freeimage/LibJPEG/jccoefct.c\
    freeimage/LibJPEG/jccolor.c\
    freeimage/LibJPEG/jcdctmgr.c\
    freeimage/LibJPEG/jchuff.c\
    freeimage/LibJPEG/jcinit.c\
    freeimage/LibJPEG/jcmainct.c\
    freeimage/LibJPEG/jcmarker.c\
    freeimage/LibJPEG/jcmaster.c\
    freeimage/LibJPEG/jcomapi.c\
    freeimage/LibJPEG/jconfig.txt\
    freeimage/LibJPEG/jcparam.c\
    freeimage/LibJPEG/jcprepct.c\
    freeimage/LibJPEG/jcsample.c\
    freeimage/LibJPEG/jctrans.c\
    freeimage/LibJPEG/jdapimin.c\
    freeimage/LibJPEG/jdapistd.c\
    freeimage/LibJPEG/jdarith.c\
    freeimage/LibJPEG/jdatadst.c\
    freeimage/LibJPEG/jdatasrc.c\
    freeimage/LibJPEG/jdcoefct.c\
    freeimage/LibJPEG/jdcolor.c\
    freeimage/LibJPEG/jddctmgr.c\
    freeimage/LibJPEG/jdhuff.c\
    freeimage/LibJPEG/jdinput.c\
    freeimage/LibJPEG/jdmainct.c\
    freeimage/LibJPEG/jdmarker.c\
    freeimage/LibJPEG/jdmaster.c\
    freeimage/LibJPEG/jdmerge.c\
    freeimage/LibJPEG/jdpostct.c\
    freeimage/LibJPEG/jdsample.c\
    freeimage/LibJPEG/jdtrans.c\
    freeimage/LibJPEG/jerror.c\
    freeimage/LibJPEG/jfdctflt.c\
    freeimage/LibJPEG/jfdctfst.c\
    freeimage/LibJPEG/jfdctint.c\
    freeimage/LibJPEG/jidctflt.c\
    freeimage/LibJPEG/jidctfst.c\
    freeimage/LibJPEG/jidctint.c\
    freeimage/LibJPEG/jmemmgr.c\
    freeimage/LibJPEG/jmemnobs.c\
    freeimage/LibJPEG/jquant1.c\
    freeimage/LibJPEG/jquant2.c\
    freeimage/LibJPEG/jutils.c\
    freeimage/LibJPEG/rdbmp.c\
    freeimage/LibJPEG/rdcolmap.c\
    freeimage/LibJPEG/rdgif.c\
    freeimage/LibJPEG/rdppm.c\
    freeimage/LibJPEG/rdrle.c\
    freeimage/LibJPEG/rdswitch.c\
    freeimage/LibJPEG/rdtarga.c\
    freeimage/LibJPEG/transupp.c\
    freeimage/LibJPEG/wrbmp.c\
    freeimage/LibJPEG/wrgif.c\
    freeimage/LibJPEG/wrppm.c\
    freeimage/LibJPEG/wrrle.c\
    freeimage/LibJPEG/wrtarga.c\
    freeimage/LibPNG/png.c\
    freeimage/LibPNG/pngerror.c\
    freeimage/LibPNG/pngget.c\
    freeimage/LibPNG/pngmem.c\
    freeimage/LibPNG/pngpread.c\
    freeimage/LibPNG/pngread.c\
    freeimage/LibPNG/pngrio.c\
    freeimage/LibPNG/pngrtran.c\
    freeimage/LibPNG/pngrutil.c\
    freeimage/LibPNG/pngset.c\
    freeimage/LibPNG/pngtrans.c\
    freeimage/LibPNG/pngwio.c\
    freeimage/LibPNG/pngwrite.c\
    freeimage/LibPNG/pngwtran.c\
    freeimage/LibPNG/pngwutil.c\
    freeimage/Metadata/Exif.cpp\
    freeimage/Metadata/FIRational.cpp\
    freeimage/Metadata/FreeImageTag.cpp\
    freeimage/Metadata/IPTC.cpp\
    freeimage/Metadata/TagConversion.cpp\
    freeimage/Metadata/TagLib.cpp\
    freeimage/LibOpenJPEG/bio.c\
    freeimage/LibOpenJPEG/cidx_manager.c\
    freeimage/LibOpenJPEG/cio.c\
    freeimage/LibOpenJPEG/dwt.c\
    freeimage/LibOpenJPEG/event.c\
    freeimage/LibOpenJPEG/image.c\
    freeimage/LibOpenJPEG/j2k.c\
    freeimage/LibOpenJPEG/j2k_lib.c\
    freeimage/LibOpenJPEG/jp2.c\
    freeimage/LibOpenJPEG/jpt.c\
    freeimage/LibOpenJPEG/mct.c\
    freeimage/LibOpenJPEG/mct.h\
    freeimage/LibOpenJPEG/mqc.c\
    freeimage/LibOpenJPEG/mqc.h\
    freeimage/LibOpenJPEG/openjpeg.c\
    freeimage/LibOpenJPEG/phix_manager.c\
    freeimage/LibOpenJPEG/pi.c\
    freeimage/LibOpenJPEG/ppix_manager.c\
    freeimage/LibOpenJPEG/raw.c\
    freeimage/LibOpenJPEG/t1.c\
    freeimage/LibOpenJPEG/t2.c\
    freeimage/LibOpenJPEG/tcd.c\
    freeimage/LibOpenJPEG/tgt.c\
    freeimage/LibOpenJPEG/thix_manager.c\
    freeimage/LibOpenJPEG/tpix_manager.c\
    
include $(BUILD_STATIC_LIBRARY)
																	
#lib---libzlib
include $(CLEAR_VARS)

LOCAL_MODULE :=libzlib
TARGET_OUT := $(LOCAL_PATH)/../../../Dependencies/Zlib/Android/Lib/$(APP_OPTIM)/$(TARGET_ARCH_ABI)
LOCAL_SRC_FILES := \
	zlib/adler32.c \
	zlib/compress.c \
	zlib/crc32.c \
	zlib/deflate.c \
	zlib/gzio.c \
	zlib/infback.c \
	zlib/inffast.c \
	zlib/inflate.c \
	zlib/inftrees.c \
	zlib/trees.c \
	zlib/uncompr.c \
	zlib/zutil.c \

LOCAL_C_INCLUDES := \
	../lordlib/zlib

include $(BUILD_STATIC_LIBRARY)

#lib---libLua
include $(CLEAR_VARS)

LOCAL_MODULE :=libLua
TARGET_OUT := $(LOCAL_PATH)/../../../Dependencies/Lua/Android/Lib/$(APP_OPTIM)/$(TARGET_ARCH_ABI)

LOCAL_SRC_FILES := \
	lua/lapi.c \
	lua/lauxlib.c \
	lua/lbaselib.c \
	lua/lcode.c \
	lua/ldblib.c \
	lua/ldebug.c \
	lua/ldo.c \
	lua/ldump.c \
	lua/lfunc.c \
	lua/lgc.c \
	lua/linit.c \
	lua/liolib.c \
	lua/llex.c \
	lua/lmathlib.c \
	lua/lmem.c \
	lua/loadlib.c \
	lua/lobject.c \
	lua/lopcodes.c \
	lua/loslib.c \
	lua/lparser.c \
	lua/lstate.c \
	lua/lstring.c \
	lua/lstrlib.c \
	lua/ltable.c \
	lua/ltablib.c \
	lua/ltm.c \
	lua/lundump.c \
	lua/lvm.c \
	lua/lzio.c \
	lua/print.c \
	
	

LOCAL_C_INCLUDES := \
	lua/

include $(BUILD_STATIC_LIBRARY)