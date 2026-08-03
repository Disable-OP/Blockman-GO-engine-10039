#include "EffectMaterial.h"
#include "EffectSystemManager.h"
#include "Render/TextureManager.h"
#include "Object/Exception.h"
#include "Render/Material.h"
#include "Scene/SceneManager.h"
#include "Resource/DataStream.h"

namespace LORD
{
	const EFFECT_BLEND_MODE	EffectMaterial::DEFAULT_MT_BLEND_MODE = AlphaLightMode;
	const String			EffectMaterial::DEFAULT_MT_TEXTURENAME = "blank.png";
	const String			EffectMaterial::DEFAULT_DS_TEXTURENAME = "noise.jpg";


	EffectMaterial::EffectMaterial()
		: mBlendMode(DEFAULT_MT_BLEND_MODE)
		, mTextureName(DEFAULT_MT_TEXTURENAME)
		, mTexturePtr(NULL)
		, mTexAddrLeft(0)
		, mTexAddrRight(1)
		, mTexAddrTop(0)
		, mTexAddrBottom(1)
		, mbFlipU(false)
		, mbFlipV(false)
		, mbSwapUV(false)
		, mUSpped(0)
		, mVSpeed(0)
		, mTimer(0)
		, mDistortionEnable(false)
		, mNoiseTexName(DEFAULT_DS_TEXTURENAME)
		, mNoiseTextPtr(NULL)
		, mDistorTexAddrLeft(0)
		, mDistorTexAddrRight(1)
		, mDistorTexAddrTop(0)
		, mDistorTexAddrBottom(1)
		, mDistortionAlphaThreshold(0.0f)
		, mDistortionTcOffsetU(0.0f)
		, mDistortionTcOffsetV(0.0f)
		, mDistortionTcScaleU(0.0f)
		, mDistortionTcScaleV(0.0f)
		, mDistortionBlendWeight(0.0f)
		, mDistortionOnly(false)
	{

	}

	EffectMaterial::~EffectMaterial()
	{
		unprepare();
	}

	void EffectMaterial::prepare_res()
	{
		if(mTexturePtr==NULL)
		{
			mTexturePtr = TextureManager::Instance()->createTexture(mTextureName);
		}

		if( mDistortionEnable && mNoiseTextPtr == NULL &&  mNoiseTexName != "")
		{
			mNoiseTextPtr = TextureManager::Instance()->createTexture(mNoiseTexName);
		}
	}

	void EffectMaterial::prepare_io()
	{
		mTexturePtr->prepare();

		if( mDistortionEnable && mNoiseTextPtr )
		{
			mNoiseTextPtr->prepare();
		}
	}

	void EffectMaterial::prepare()
	{
		mTexturePtr->load();

		if( mDistortionEnable )
		{
			mNoiseTextPtr->load();
		}

		SamplerState* pState = EffectSystemManager::Instance()->getSamplerState();
		mTexturePtr->setSamplerState(pState);

		mTexAddrLeftRealtime = mTexAddrLeft;
		mTexAddrRightRealtime = mTexAddrRight;
		mTexAddrTopRealtime = mTexAddrTop;
		mTexAddrBottomRealtime = mTexAddrBottom;
		if(mbFlipU)
		{
			Math::Swap(mTexAddrLeftRealtime, mTexAddrRightRealtime);
		}
		if(mbFlipV)
		{
			Math::Swap(mTexAddrTopRealtime, mTexAddrBottomRealtime);
		}
		_update(0);
	}

	void EffectMaterial::unprepare()
	{
		if(mTexturePtr)
		{
			TextureManager::Instance()->releaseResource(mTexturePtr);
			mTexturePtr = NULL;
		}

		if( mNoiseTextPtr )
		{
			TextureManager::Instance()->releaseResource(mNoiseTextPtr);
			mNoiseTextPtr = NULL;
		}
	}

	/*bool EffectMaterial::operator ==(const EffectMaterial& anotherMaterial) const
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
	}*/

	void EffectMaterial::_notifyStart()
	{
	}

	void EffectMaterial::_update(i32 time)
	{
		mTimer += time;
		if(mUSpped != 0)
		{
			float transu = mUSpped*(float)(mTimer)/1000.f;
			mTexAddrLeftRealtime = mTexAddrLeft+transu;
			mTexAddrRightRealtime = mTexAddrRight+transu;
		}
		if(mVSpeed != 0)
		{
			float transv = mVSpeed*(float)(mTimer)/1000.f;
			mTexAddrTopRealtime = mTexAddrTop + transv;
			mTexAddrBottomRealtime = mTexAddrBottom + transv;
		}
	}

	BlendState::BlendFactor EffectMaterial::getSrcBlendFactor()const
	{
		switch(mBlendMode)
		{
		case AlphaLightMode:
			return BlendState::BF_SRC_ALPHA;
		case HighLightMode:
			return BlendState::BF_SRC_ALPHA;
		}
		return BlendState::BF_SRC_ALPHA;
	}

	BlendState::BlendFactor EffectMaterial::getDestBlendFactor()const
	{
		switch(mBlendMode)
		{
		case AlphaLightMode:
			return BlendState::BF_INV_SRC_ALPHA;
		case HighLightMode:
			return BlendState::BF_ONE;
		}
		return BlendState::BF_DST_ALPHA;
	}

	String EffectMaterial::getUVString() const
	{
		String ret = StringUtil::ToString(Vector4(mTexAddrLeft, mTexAddrTop, mTexAddrRight, mTexAddrBottom));
		return ret;
	}

	void EffectMaterial::setUVString(const String& str)
	{
		Vector4 vec = StringUtil::ParseVec4(str);
		mTexAddrLeft = vec.x;
		mTexAddrTop = vec.y;
		mTexAddrRight = vec.z;
		mTexAddrBottom = vec.w;
	}

	String EffectMaterial::getDistortionUV() const
	{
		String ret = StringUtil::ToString(Vector4(mDistorTexAddrLeft, mDistorTexAddrTop, mDistorTexAddrRight, mDistorTexAddrBottom));
		return ret;
	}

	void EffectMaterial::setDistortionUV(const String& str)
	{
		Vector4 vec = StringUtil::ParseVec4(str);
		mDistorTexAddrLeft = vec.x;
		mDistorTexAddrTop = vec.y;
		mDistorTexAddrRight = vec.z;
		mDistorTexAddrBottom = vec.w;
	}

	void EffectMaterial::copyAttributesTo(EffectMaterial* material)
	{
		material->mBlendMode = mBlendMode;
		material->mTextureName = mTextureName;
		material->mTexAddrLeft = mTexAddrLeft;
		material->mTexAddrRight = mTexAddrRight;
		material->mTexAddrTop = mTexAddrTop;
		material->mTexAddrBottom = mTexAddrBottom;
		material->mbFlipU = mbFlipU;
		material->mbFlipV = mbFlipV;
		material->mbSwapUV = mbSwapUV;
		material->mUSpped = mUSpped;
		material->mVSpeed = mVSpeed;
		material->mDistortionEnable = mDistortionEnable;
		if(mDistortionEnable)
		{
			material->mNoiseTexName = mNoiseTexName;
			material->mDistorTexAddrLeft = mDistorTexAddrLeft;
			material->mDistorTexAddrRight = mDistorTexAddrRight;
			material->mDistorTexAddrTop = mDistorTexAddrTop;
			material->mDistorTexAddrBottom = mDistorTexAddrBottom;
			material->mDistortionAlphaThreshold = mDistortionAlphaThreshold;
			material->mDistortionTcOffsetU = mDistortionTcOffsetU;
			material->mDistortionTcOffsetV = mDistortionTcOffsetV;
			material->mDistortionTcScaleU = mDistortionTcScaleU;
			material->mDistortionTcScaleV = mDistortionTcScaleV;
			material->mDistortionBlendWeight = mDistortionBlendWeight;
			material->mDistortionOnly = mDistortionOnly;
		}
	}

	String EffectMaterial::getBlendModeStr() const
	{
		String result;
		switch(mBlendMode)
		{
		case AlphaLightMode:
			result = "alpha_lightting"; break;
		case HighLightMode:
			result = "high_lightting"; break;
		}
		return result;
	}

	void EffectMaterial::setBlendModeStr(const String &blendStr)
	{
		EFFECT_BLEND_MODE blendMode = AlphaLightMode;
		if(blendStr == "high_lightting")
			blendMode = HighLightMode;

		mBlendMode = blendMode;
	}

	EFFECT_BLEND_MODE EffectMaterial::getBlendMode() const
	{
		return mBlendMode;
	}

	void EffectMaterial::exportData(DataStream* pStream, int version)
	{
		pStream->write(&mBlendMode, sizeof(EFFECT_BLEND_MODE));
		ui32 strlength = (ui32)mTextureName.length();
		pStream->write(&strlength, sizeof(ui32));
		pStream->write(mTextureName.c_str(), strlength);
		pStream->write(&mTexAddrLeft, sizeof(float));
		pStream->write(&mTexAddrTop, sizeof(float));
		pStream->write(&mTexAddrRight, sizeof(float));
		pStream->write(&mTexAddrBottom, sizeof(float));
		pStream->write(&mbFlipU, sizeof(bool));
		pStream->write(&mbFlipV, sizeof(bool));
		pStream->write(&mbSwapUV, sizeof(bool));
		pStream->write(&mUSpped, sizeof(float));
		pStream->write(&mVSpeed, sizeof(float));

		pStream->write(&mDistortionEnable,sizeof(bool));
		if(mDistortionEnable)
		{
			ui32 strlength = (ui32)mNoiseTexName.length();
			pStream->write(&strlength, sizeof(ui32));
			pStream->write(mNoiseTexName.c_str(), strlength);
			pStream->write(&mDistorTexAddrLeft, sizeof(float));
			pStream->write(&mDistorTexAddrTop, sizeof(float));
			pStream->write(&mDistorTexAddrRight, sizeof(float));
			pStream->write(&mDistorTexAddrBottom, sizeof(float));
			pStream->write(&mDistortionAlphaThreshold,sizeof(float));
			pStream->write(&mDistortionTcOffsetU,sizeof(float));
			pStream->write(&mDistortionTcOffsetV,sizeof(float));
			pStream->write(&mDistortionTcScaleU,sizeof(float));
			pStream->write(&mDistortionTcScaleV,sizeof(float));
			pStream->write(&mDistortionBlendWeight,sizeof(float));
			pStream->write(&mDistortionOnly,sizeof(bool));
		}
	}

	void EffectMaterial::importData(DataStream* pStream, int version)
	{
		pStream->read(&mBlendMode, sizeof(EFFECT_BLEND_MODE));
		ui32 strLength;
		pStream->read(&strLength, sizeof(ui32));
		char str[128];
		pStream->read(str, strLength);
		str[strLength] = 0;
		mTextureName = str;
		pStream->read(&mTexAddrLeft, sizeof(float));
		pStream->read(&mTexAddrTop, sizeof(float));
		pStream->read(&mTexAddrRight, sizeof(float));
		pStream->read(&mTexAddrBottom, sizeof(float));
		pStream->read(&mbFlipU, sizeof(bool));
		pStream->read(&mbFlipV, sizeof(bool));
		pStream->read(&mbSwapUV, sizeof(bool));
		pStream->read(&mUSpped, sizeof(float));
		pStream->read(&mVSpeed, sizeof(float));
		pStream->read(&mDistortionEnable,sizeof(bool));
		if(mDistortionEnable)
		{
			ui32 strLength;
			pStream->read(&strLength, sizeof(ui32));
			char str[128];
			pStream->read(str, strLength);
			str[strLength] = 0;
			mNoiseTexName = str;
			pStream->read(&mDistorTexAddrLeft, sizeof(float));
			pStream->read(&mDistorTexAddrTop, sizeof(float));
			pStream->read(&mDistorTexAddrRight, sizeof(float));
			pStream->read(&mDistorTexAddrBottom, sizeof(float));
			pStream->read(&mDistortionAlphaThreshold,sizeof(float));
			pStream->read(&mDistortionTcOffsetU,sizeof(float));
			pStream->read(&mDistortionTcOffsetV,sizeof(float));
			pStream->read(&mDistortionTcScaleU,sizeof(float));
			pStream->read(&mDistortionTcScaleV,sizeof(float));
			pStream->read(&mDistortionBlendWeight,sizeof(float));
			pStream->read(&mDistortionOnly,sizeof(bool));
		}
	}

	void EffectMaterial::getPropertyList(PropertyList& list)
	{
		list.push_back(token[TOKEN_MT_BLENDMODE]);
		list.push_back(token[TOKEN_MT_TEXTURE]);
		list.push_back(token[TOKEN_MT_UVVALUE]);
		list.push_back(token[TOKEN_MT_FLIPU]);
		list.push_back(token[TOKEN_MT_FLIPV]);
		list.push_back(token[TOKEN_MT_SWAPUV]);
		list.push_back(token[TOKEN_MT_TASPEEDU]);
		list.push_back(token[TOKEN_MT_TASPEEDV]);
		list.push_back(token[TOKEN_DS_ENABEL]);
		list.push_back(token[TOKEN_DS_TEXTURE]);
		list.push_back(token[TOKEN_DS_UVVALUE]);
		list.push_back(token[TOKEN_DS_ALPHA_THRESHOLD]);
		list.push_back(token[TOKEN_DS_TC_OFFSETU]);
		list.push_back(token[TOKEN_DS_TC_OFFSETV]);
		list.push_back(token[TOKEN_DS_TC_SCALEU]);
		list.push_back(token[TOKEN_DS_TC_SCALEV]);
		list.push_back(token[TOKEN_DS_BLEND_WEIGHT]);
		list.push_back(token[TOKEN_DS_ONLY]);
	}

	bool EffectMaterial::getPropertyType(const String& name, PropertyType& type)
	{
		if (name == token[TOKEN_MT_BLENDMODE])
		{
			type = IElement::PT_MATERIAL_BLEND_MODE; return true;
		}
		else if (name == token[TOKEN_MT_TEXTURE])
		{
			type = IElement::PT_MATERIAL_TEXTURE; return true;
		}
		else if(name == token[TOKEN_MT_UVVALUE])
		{
			type = IElement::PT_VECTOR4; return true;
		}
		else if(name == token[TOKEN_MT_FLIPU])
		{
			type = IElement::PT_BOOL; return true;
		}
		else if(name == token[TOKEN_MT_FLIPV])
		{
			type = IElement::PT_BOOL; return true;
		}
		else if(name == token[TOKEN_MT_SWAPUV])
		{
			type = IElement::PT_BOOL; return true;
		}
		else if(name == token[TOKEN_MT_TASPEEDU])
		{
			type = IElement::PT_REAL; return true;
		}
		else if(name == token[TOKEN_MT_TASPEEDV])
		{
			type = IElement::PT_REAL; return true;	
		}
		else if(name == token[TOKEN_DS_ENABEL])
		{
			type = IElement::PT_BOOL; return true;	
		}
		else if (name == token[TOKEN_DS_TEXTURE])
		{
			type = IElement::PT_MATERIAL_TEXTURE; return true;
		}
		else if(name == token[TOKEN_DS_UVVALUE])
		{
			type = IElement::PT_VECTOR4; return true;
		}
		else if(name == token[TOKEN_DS_ALPHA_THRESHOLD])
		{
			type = IElement::PT_REAL; return true;	
		}
		else if(name == token[TOKEN_DS_TC_OFFSETU])
		{
			type = IElement::PT_REAL; return true;	
		}
		else if(name == token[TOKEN_DS_TC_OFFSETV])
		{
			type = IElement::PT_REAL; return true;	
		}
		else if(name == token[TOKEN_DS_TC_SCALEU])
		{
			type = IElement::PT_REAL; return true;	
		}
		else if(name == token[TOKEN_DS_TC_SCALEV])
		{
			type = IElement::PT_REAL; return true;	
		}
		else if (name == token[TOKEN_DS_BLEND_WEIGHT])
		{
			type = IElement::PT_REAL; return true;
		}
		else if(name == token[TOKEN_DS_ONLY])
		{
			type = IElement::PT_BOOL; return true;	
		}
		return false;
	}

	bool EffectMaterial::getPropertyValue(const String& name, String& value)
	{
		if (name == token[TOKEN_MT_BLENDMODE])
		{
			value = getBlendModeStr(); return true;
		}
		else if (name == token[TOKEN_MT_TEXTURE])
		{
			value = mTextureName; return true;
		}
		else if(name == token[TOKEN_MT_UVVALUE])
		{
			value = getUVString(); return true;
		}
		else if(name == token[TOKEN_MT_FLIPU])
		{
			value = StringUtil::ToString(mbFlipU); return true;
		}
		else if(name == token[TOKEN_MT_FLIPV])
		{
			value = StringUtil::ToString(mbFlipV); return true;
		}
		else if(name == token[TOKEN_MT_SWAPUV])
		{
			value = StringUtil::ToString(mbSwapUV); return true;
		}
		else if(name == token[TOKEN_MT_TASPEEDU])
		{
			value = StringUtil::ToString(mUSpped); return true;
		}
		else if(name == token[TOKEN_MT_TASPEEDV])
		{
			value = StringUtil::ToString(mVSpeed); return true;
		}
		else if(name == token[TOKEN_DS_ENABEL])
		{
			value = StringUtil::ToString(mDistortionEnable); return true;
		}
		else if (name == token[TOKEN_DS_TEXTURE])
		{
			value = mNoiseTexName; return true;
		}
		else if(name == token[TOKEN_DS_UVVALUE])
		{
			value = getDistortionUV(); return true;
		}
		else if(name == token[TOKEN_DS_ALPHA_THRESHOLD])
		{
			value = StringUtil::ToString(mDistortionAlphaThreshold); return true;
		}
		else if(name == token[TOKEN_DS_TC_OFFSETU])
		{
			value = StringUtil::ToString(mDistortionTcOffsetU); return true;
		}
		else if(name == token[TOKEN_DS_TC_OFFSETV])
		{
			value = StringUtil::ToString(mDistortionTcOffsetV); return true;
		}
		else if(name == token[TOKEN_DS_TC_SCALEU])
		{
			value = StringUtil::ToString(mDistortionTcScaleU); return true;
		}
		else if(name == token[TOKEN_DS_TC_SCALEV])
		{
			value = StringUtil::ToString(mDistortionTcScaleV); return true;
		}
		else if(name == token[TOKEN_DS_BLEND_WEIGHT])
		{
			value = StringUtil::ToString(mDistortionBlendWeight); return true;
		}
		else if(name == token[TOKEN_DS_ONLY])
		{
			value = StringUtil::ToString(mDistortionOnly); return true;
		}
		return false;
	}

	bool EffectMaterial::setPropertyValue(const String& name,const String& value)
	{
		if (name == token[TOKEN_MT_BLENDMODE])
		{
			setBlendModeStr(value); return true;
		}
		else if (name == token[TOKEN_MT_TEXTURE])
		{
			mTextureName = value; return true;
		}
		else if(name == token[TOKEN_MT_UVVALUE])
		{
			setUVString(value); return true;
		}
		else if(name == token[TOKEN_MT_FLIPU])
		{
			mbFlipU = StringUtil::ParseBool(value); return true;
		}
		else if(name == token[TOKEN_MT_FLIPV])
		{
			mbFlipV = StringUtil::ParseBool(value); return true;
		}
		else if(name == token[TOKEN_MT_SWAPUV])
		{
			mbSwapUV = StringUtil::ParseBool(value); return true;
		}
		else if(name == token[TOKEN_MT_TASPEEDU])
		{
			mUSpped = StringUtil::ParseReal(value); return true;
		}
		else if(name == token[TOKEN_MT_TASPEEDV])
		{
			mVSpeed = StringUtil::ParseReal(value); return true;
		}
		else if(name == token[TOKEN_DS_ENABEL])
		{
			mDistortionEnable = StringUtil::ParseBool(value); return true;
		}
		else if (name == token[TOKEN_DS_TEXTURE])
		{
			mNoiseTexName = value; return true;
		}
		else if(name == token[TOKEN_DS_UVVALUE])
		{
			setDistortionUV(value); return true;
		}
		else if(name == token[TOKEN_DS_ALPHA_THRESHOLD])
		{
			mDistortionAlphaThreshold = StringUtil::ParseReal(value); return true;
		}
		else if(name == token[TOKEN_DS_TC_OFFSETU])
		{
			mDistortionTcOffsetU = StringUtil::ParseReal(value); return true;
		}
		else if(name == token[TOKEN_DS_TC_OFFSETV])
		{
			mDistortionTcOffsetV = StringUtil::ParseReal(value); return true;
		}
		else if(name == token[TOKEN_DS_TC_SCALEU])
		{
			mDistortionTcScaleU = StringUtil::ParseReal(value); return true;
		}
		else if(name == token[TOKEN_DS_TC_SCALEV])
		{
			mDistortionTcScaleV = StringUtil::ParseReal(value); return true;
		}
		else if(name == token[TOKEN_DS_BLEND_WEIGHT])
		{
			mDistortionBlendWeight = StringUtil::ParseReal(value); return true;
		}
		else if(name == token[TOKEN_DS_ONLY])
		{
			mDistortionOnly = StringUtil::ParseBool(value); return true;
		}
		return false;
	}
}