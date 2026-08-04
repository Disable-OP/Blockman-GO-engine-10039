/********************************************************************
filename: 	TessMaterial.h
file path:	dev\engine\Src\Core\Tessellator\

version:	1
author:		ajohn
company:	supernano
datatime:   2016-11-29
*********************************************************************/

#ifndef __LORD_TESS_MATERIAL_H__
#define __LORD_TESS_MATERIAL_H__

#include "Core.h"

namespace LORD
{
	class LORD_CORE_API TessMaterial : public ObjectAlloc
	{
	public:
		static const TESS_RENDER_PASS	DEFAULT_TR_RENDER_GROUP;
		static const String				DEFAULT_MT_TEXTURENAME;

	public:
		TessMaterial();
		~TessMaterial();

		void prepare_res();
		void prepare_io();
		void prepare();
		void unprepare();

		void importData(DataStream* pStream, int version);
		void exportData(DataStream* pStream, int version);

		void _notifyStart();
		void _update(i32 time);

		virtual void copyAttributesTo(TessMaterial* material);

		inline TESS_RENDER_PASS getRenderGroup() const { return mRenderGroup; }
		inline void setRenderGroup(TESS_RENDER_PASS mode) { mRenderGroup = mode; }
		BlendState::BlendFactor getSrcBlendFactor() const;
		BlendState::BlendFactor getDestBlendFactor() const;
		inline const String& getTextureName() const { return mTextureName; }
		inline void setTextureName(const String& tn) { mTextureName = tn; }
		inline Texture* getTexturePtr() const { return mTexturePtr; }
		inline float getTexAddrLeft() const { return mTexAddrLeftRealtime; }
		inline float getTexAddrTop() const { return mTexAddrTopRealtime; }
		inline float getTexAddrRight() const { return mTexAddrRightRealtime; }
		inline float getTexAddrButton() const { return mTexAddrBottomRealtime; }
		inline bool getSwapUV() const { return mbSwapUV; }
		
		inline bool operator==(const TessMaterial& rhs) const
		{
			return mTextureName == rhs.mTextureName &&
				mRenderGroup == rhs.mRenderGroup;
		}

	protected:
		TESS_RENDER_PASS	mRenderGroup;
		String				mTextureName;
		Texture*		    mTexturePtr;
		float				mTexAddrLeft;
		float				mTexAddrRight;
		float				mTexAddrTop;
		float				mTexAddrBottom;
		float				mTexAddrLeftRealtime;
		float				mTexAddrRightRealtime;
		float				mTexAddrTopRealtime;
		float				mTexAddrBottomRealtime;
		bool				mbFlipU;
		bool				mbFlipV;
		bool				mbSwapUV;
		float				mUSpped;
		float				mVSpeed;
		int					mTimer;
	};
}

#endif