#include "TessMaterial.h"
#include "TessManager.h"
#include "Render/TextureManager.h"
#include "Object/Exception.h"
#include "Render/Material.h"
#include "Scene/SceneManager.h"
#include "Resource/DataStream.h"

namespace LORD
{

const TESS_RENDER_PASS	TessMaterial::DEFAULT_TR_RENDER_GROUP = TRP_SOLID;
const String			TessMaterial::DEFAULT_MT_TEXTURENAME = "Blocks.png";


TessMaterial::TessMaterial()
	: mRenderGroup(DEFAULT_TR_RENDER_GROUP)
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
{

}

TessMaterial::~TessMaterial()
{
	unprepare();
}

void TessMaterial::prepare_res()
{
	if (mTexturePtr == NULL)
	{
		mTexturePtr = TextureManager::Instance()->createTexture(mTextureName);
	}
}

void TessMaterial::prepare_io()
{
	mTexturePtr->prepare();
}

void TessMaterial::prepare()
{
	mTexturePtr->load();

	SamplerState* pState = TessManager::Instance()->getSamplerState();
	mTexturePtr->setSamplerState(pState);

	mTexAddrLeftRealtime = mTexAddrLeft;
	mTexAddrRightRealtime = mTexAddrRight;
	mTexAddrTopRealtime = mTexAddrTop;
	mTexAddrBottomRealtime = mTexAddrBottom;
	if (mbFlipU)
	{
		Math::Swap(mTexAddrLeftRealtime, mTexAddrRightRealtime);
	}
	if (mbFlipV)
	{
		Math::Swap(mTexAddrTopRealtime, mTexAddrBottomRealtime);
	}
	_update(0);
}

void TessMaterial::unprepare()
{
	if (mTexturePtr)
	{
		TextureManager::Instance()->releaseResource(mTexturePtr);
		mTexturePtr = NULL;
	}
}

void TessMaterial::_notifyStart()
{

}

void TessMaterial::_update(i32 time)
{
	mTimer += time;
	if (mUSpped != 0)
	{
		float transu = mUSpped*(float)(mTimer) / 1000.f;
		mTexAddrLeftRealtime = mTexAddrLeft + transu;
		mTexAddrRightRealtime = mTexAddrRight + transu;
	}
	if (mVSpeed != 0)
	{
		float transv = mVSpeed*(float)(mTimer) / 1000.f;
		mTexAddrTopRealtime = mTexAddrTop + transv;
		mTexAddrBottomRealtime = mTexAddrBottom + transv;
	}
}

BlendState::BlendFactor TessMaterial::getSrcBlendFactor()const
{
	switch (mRenderGroup)
	{
	case TRP_SOLID:
		return BlendState::BF_ONE;
	case TRP_TRANSPARENT_2D:
		return BlendState::BF_SRC_ALPHA;
	}
	return BlendState::BF_SRC_ALPHA;
}

BlendState::BlendFactor TessMaterial::getDestBlendFactor()const
{
	switch (mRenderGroup)
	{
	case TRP_SOLID:
		return BlendState::BF_ZERO;
	case TRP_TRANSPARENT_2D:
		return BlendState::BF_INV_SRC_ALPHA;
	}
	return BlendState::BF_DST_ALPHA;
}

void TessMaterial::copyAttributesTo(TessMaterial* material)
{
	material->mRenderGroup = mRenderGroup;
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
}

}