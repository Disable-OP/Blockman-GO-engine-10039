/********************************************************************
filename: 	ImageCodec.h
file path:	dev\engine\Src\Core\Image\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_IMAGECODEC_H__
#define __LORD_IMAGECODEC_H__

#include "Core.h"
#include "Object/Buffer.h"

namespace LORD
{
	class LORD_CORE_API ImageCodec: public ObjectAlloc
	{
	public:
		ImageCodec(ImageFormat imgFmt);
		virtual ~ImageCodec();

	public:

		virtual ImageFormat			getImageFormat() const;
		virtual bool				codeToFile(ImageFormat imgFmt, const Buffer &buff, const Image::ImageInfo &imgInfo, const String &filename) const;
		virtual bool				encode(const Buffer &inBuff, Buffer &outBuff, const Image::ImageInfo &imgInfo) const;
		virtual bool				decode(const Buffer &inBuff, Buffer &outBuff, Image::ImageInfo &imgInfo);
		virtual DataStream*			decode(DataStream* inStream, Image::ImageInfo& imgInfo);

		static ImageFormat			GetImageFormat(const String &filename);

	protected:
		bool						doEncode(ImageFormat imgFmt, const Buffer &inBuff, Buffer &outBuff, const Image::ImageInfo &imgInfo) const;
		bool						doDecode(ImageFormat imgFmt, const Buffer &inBuff, Buffer &outBuff, Image::ImageInfo &imgInfo);
		DataStream*					doDecode(ImageFormat imgFmt, DataStream* inStream, Image::ImageInfo& imgInfo);

	protected:
		ImageFormat			m_imgFmt;
	};
}

#endif