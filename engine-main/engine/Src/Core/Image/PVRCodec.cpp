#include "Core.h"
#include "PVRCodec.h"
#include "Util/StringUtil.h"
#include "Util/PathUtil.h"

namespace LORD
{
	PVRCodec::PVRCodec()
		:ImageCodec(IF_PVR)
	{
	}

	PVRCodec::~PVRCodec()
	{
	}
	bool PVRCodec::doEncode(ImageFormat imgFmt, const Buffer &inBuff, Buffer &outBuff, const Image::ImageInfo &imgInfo) const
	{

		return true;
	}

	DataStream* PVRCodec::doDecode(ImageFormat imgFmt, DataStream* inStream, Image::ImageInfo& imgInfo)
	{

		return NULL;
	}

	bool PVRCodec::doDecode(ImageFormat imgFmt, const Buffer &inBuff, Buffer &outBuff, Image::ImageInfo &imgInfo)
	{

		return true;
	}
}