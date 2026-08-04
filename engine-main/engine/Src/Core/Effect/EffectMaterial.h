/********************************************************************
filename: 	EffectMaterial.h
file path:	dev\engine\Src\Core\Effect\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_EFFECT_MATERIAL_H__
#define __LORD_EFFECT_MATERIAL_H__

#include "Core.h"
#include "EffectIElement.h"
#include "Scene/Camera.h"

namespace LORD
{
	class LORD_CORE_API EffectMaterial : public IElement
	{
	public:
		String getBlendModeStr() const;
		void setBlendModeStr(const String& blendStr);
		EFFECT_BLEND_MODE getBlendMode() const;
	public:
		//interface
		virtual void getPropertyList(PropertyList& list);
		virtual bool getPropertyType(const String& name, PropertyType& type);
		virtual bool getPropertyValue(const String& name, String& value);
		virtual bool setPropertyValue(const String& name, const String& value);

	public:
		static const EFFECT_BLEND_MODE	DEFAULT_MT_BLEND_MODE;
		static const String				DEFAULT_MT_TEXTURENAME;
		static const String				DEFAULT_DS_TEXTURENAME;

	public:
		EffectMaterial();
		~EffectMaterial();

		void prepare_res();
		void prepare_io();
		void prepare();
		void unprepare();

		void importData(DataStream* pStream, int version);
		void exportData(DataStream* pStream, int version);

		void _notifyStart();
		void _update(i32 time);

		virtual void copyAttributesTo (EffectMaterial* material);

		inline EFFECT_BLEND_MODE getSceneBlend() const { return mBlendMode; }
		inline void setSceneBlend(EFFECT_BLEND_MODE mode) { mBlendMode = mode; }
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
		String getUVString() const;
		void setUVString(const String& str);
		String getDistortionUV()const;
		void setDistortionUV(const String& str);
		inline const String& getDistortionTextureName() const { return mNoiseTexName; }

		inline void setDistortionEnable( bool _val ) { mDistortionEnable = _val; }
		inline bool getDistortionEnable() const { return mDistortionEnable; }

		inline void getDistortionUVScaleOffset( Vector4& outVec ) const
		{
			outVec.x = mDistortionTcScaleU;
			outVec.y = mDistortionTcScaleV;
			outVec.z = mDistortionTcOffsetU;
			outVec.w = mDistortionTcOffsetV;
		}

		inline float getDistortionAlphaThreshold() const
		{
			return mDistortionAlphaThreshold;
		}


		inline Texture* getDistortionTexturePtr() const
		{
			return mNoiseTextPtr;
		}

		inline void setDistortionTexturePtr( Texture* pTex )
		{
			mNoiseTextPtr = pTex;
		}

		inline bool getOnlyDistortionLayer() const
		{
			return mDistortionOnly;
		}

		inline bool operator==(const EffectMaterial& anotherMaterial) const
		{
			if( !mDistortionEnable )
			{
				return mTextureName == anotherMaterial.mTextureName &&
					mBlendMode == anotherMaterial.mBlendMode;
			}
			else
			{
				return mTextureName == anotherMaterial.mTextureName &&
					mBlendMode == anotherMaterial.mBlendMode && 
					mNoiseTexName == anotherMaterial.mNoiseTexName &&
					mDistortionOnly == mDistortionOnly;
			}
		}
	protected:
		EFFECT_BLEND_MODE	mBlendMode;			
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

		bool                mDistortionEnable;
		String				mNoiseTexName;					
		Texture*		    mNoiseTextPtr;	
		float				mDistorTexAddrLeft;
		float				mDistorTexAddrRight;
		float				mDistorTexAddrTop;
		float				mDistorTexAddrBottom;
		float				mDistortionAlphaThreshold;
		float				mDistortionTcOffsetU;
		float				mDistortionTcOffsetV;
		float               mDistortionTcScaleU;
		float               mDistortionTcScaleV;
		float               mDistortionBlendWeight;
		bool				mDistortionOnly;
	};
}

#endif