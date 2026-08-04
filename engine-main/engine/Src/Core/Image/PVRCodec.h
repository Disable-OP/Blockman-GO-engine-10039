/********************************************************************
filename: 	PVRCodec.h
file path:	dev\engine\Src\Core\Image\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_PVRCODEC_H__
#define __LORD_PVRCODEC_H__

#include "ImageCodec.h"

namespace LORD
{
	class LORD_CORE_API PVRCodec: public ImageCodec
	{
	public:
		PVRCodec();
		virtual ~PVRCodec();

	protected:
		bool						doEncode(ImageFormat imgFmt, const Buffer &inBuff, Buffer &outBuff, const Image::ImageInfo &imgInfo) const;
		bool						doDecode(ImageFormat imgFmt, const Buffer &inBuff, Buffer &outBuff, Image::ImageInfo &imgInfo);
		DataStream*					doDecode(ImageFormat imgFmt, DataStream* inStream, Image::ImageInfo& imgInfo);
	};

}

#endif