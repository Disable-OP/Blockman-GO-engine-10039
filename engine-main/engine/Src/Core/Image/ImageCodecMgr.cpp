#include "ImageCodec.h"
#include "ImageCodecMgr.h"
#include <FreeImage.h>
#ifdef _WINDOWS_
#undef _WINDOWS_
#endif

namespace LORD
{
	//---------------------------------------------------------------------
	/*ImageCodecMgr* ImageCodecMgr::Instance()
	{
		return ms_pSingleton;
	}*/

	ImageCodecMgr::ImageCodecMgr()
	{
		// lazy initialization.
		FreeImage_Initialise();

		ImageCodec *pDDSCodec = LordNew ImageCodec(IF_DDS);
		ImageCodec *pJPGCodec = LordNew ImageCodec(IF_JPG);
		ImageCodec *pPNGCodec = LordNew ImageCodec(IF_PNG);
		ImageCodec *pPVRCodec = LordNew ImageCodec(IF_PVR);
		ImageCodec *pTGACodec = LordNew ImageCodec(IF_TGA);
		ImageCodec *pBMPCodec = LordNew ImageCodec(IF_BMP);

		registerCodec(pDDSCodec);
		registerCodec(pJPGCodec);
		registerCodec(pPNGCodec);
		registerCodec(pPVRCodec);
		registerCodec(pTGACodec);
		registerCodec(pBMPCodec);
	}

	ImageCodecMgr::~ImageCodecMgr()
	{
		CodecMap::iterator it = m_codecMap.begin();
		for(; it != m_codecMap.end(); ++it)
		{
			LordSafeDelete(it->second);
		}
		m_codecMap.clear();

		FreeImage_DeInitialise();
	}

	void ImageCodecMgr::registerCodec(ImageCodec* pImgCodec)
	{
		LordAssertX(pImgCodec, "The Image Codec can't be null!");

		if(pImgCodec)
		{
			int idx = (int)pImgCodec->getImageFormat();
			m_codecMap[idx] = pImgCodec;
		}
	}

	void ImageCodecMgr::unregisterCodec(ImageFormat imgFmt)
	{
		CodecMap::iterator it = m_codecMap.find((int)imgFmt);
		if (it == m_codecMap.end())
		{
			LordAssertX(false, "Couldn't found the codec.");
			return;
		}

		ImageCodec *pImgCodec= it->second;
		m_codecMap.erase(it);

		LordSafeDelete(pImgCodec);
	}

	ImageCodec* ImageCodecMgr::getCodec(ImageFormat imgFmt) const
	{
		CodecMap::const_iterator it = m_codecMap.find(imgFmt);
		if (it == m_codecMap.end())
			return NULL;
		else
			return it->second;
	}
}