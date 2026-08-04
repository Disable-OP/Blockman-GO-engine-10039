/********************************************************************
filename: 	ImageCodecMgr.h
file path:	dev\engine\Src\Core\Image\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_IMAGECODECMGR_H__
#define __LORD_IMAGECODECMGR_H__

#include "Core.h"

namespace LORD
{
	class LORD_CORE_API ImageCodecMgr: public ObjectAlloc, public Singleton<ImageCodecMgr>
	{
	public:
		inline static ImageCodecMgr* Instance()
		{
			return ms_pSingleton;
		}

	public:
		ImageCodecMgr();
		~ImageCodecMgr();

	public:
		void			registerCodec(ImageCodec *pImgCodec);
		void			unregisterCodec(ImageFormat imgFmt);
		ImageCodec*		getCodec(ImageFormat imgFmt) const;

	private:
		typedef map<int, ImageCodec*>::type CodecMap;
		CodecMap m_codecMap; 
	};
}

#endif
