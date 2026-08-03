#include "Core.h"
#include "EffectLayer3DBillBoard.h"
#include "EffectSystemManager.h"
#include "Scene/SceneNode.h"
#include "Resource/DataStream.h"

namespace LORD
{

	EffectLayer3DBillBoard::EffectLayer3DBillBoard()
		: EffectLayer()
		, mWidth(DEFAULT_LY_WIDTH)
		, mHeight(DEFAULT_LY_HEIGHT)
		, mCenterOffestW(DEFAULT_LY_CENTEROFFSETW)
		, mCenterOffestH(DEFAULT_LY_CENTEROFFSETH)
	{
		originalDirection = Vector3::UNIT_Z;
		direction = Vector3::UNIT_Z;
		mType = ELT_3DBillboard;
	}

	EffectLayer3DBillBoard::~EffectLayer3DBillBoard()
	{

	}

	void EffectLayer3DBillBoard::prepare()
	{
		EffectLayer::prepare();

		float leftOff, rightOff, topOff, bottomOff; 

		leftOff = -mCenterOffestW;
		rightOff = 1.0f - mCenterOffestW;
		bottomOff = -mCenterOffestH;
		topOff = 1.0f - mCenterOffestH;

		leftOff = leftOff*mWidth;
		rightOff = rightOff*mWidth;
		topOff = topOff*mHeight;
		bottomOff = bottomOff*mHeight;

		mPosCached[0].x = leftOff;	mPosCached[0].y = bottomOff; mPosCached[0].z = 0;
		mPosCached[1].x = leftOff;	mPosCached[1].y = topOff;	 mPosCached[1].z = 0;
		mPosCached[2].x = rightOff; mPosCached[2].y = bottomOff; mPosCached[2].z = 0;
		mPosCached[3].x = rightOff; mPosCached[3].y = topOff;    mPosCached[3].z = 0;
	}

	void EffectLayer3DBillBoard::_updateRenderQueue()
	{
		if ((!mbAffected) || (!mbVisible))
			return;

		EffectRenderable* pRenderable = EffectSystemManager::Instance()->getRenderable(mMaterial->getTexturePtr(), mRenderGroup, mMaterial->getBlendMode(), mMaterial, 4, 6, mViewSpace);
		if(!pRenderable)
			return;

		pRenderable->setSpecialRender(mParentSystem->IsSpecialRender());

		EffectVertexFmt* pVetLockPtr = NULL;
		ui16* pIdxLockPtr = NULL;
		ui16 vertBegin;
		Vector3 tempPos = position;
		if(pRenderable->getLockPtr(4, 6, vertBegin, pVetLockPtr, pIdxLockPtr))
		{
			Quaternion rotSelf;
			if(Math::Abs(selfRotation) > 1.f)
			{
				Vector3 axis(0, 0, 1);
				rotSelf.rotateAxis(axis, selfRotation*Math::DEG2RAD);
			}
			rotSelf = rotSelf * oritation;

			Matrix4 mtxLocal;
			mtxLocal.makeScaling(scale, scale, scale);
			Matrix4 mtxRotate;
			mtxRotate.fromQuan(rotSelf);
			mtxLocal = mtxLocal * mtxRotate;
			mtxLocal.translate(tempPos);
			Matrix4 mtxWolrd = mParentSystem->getEffectSceneNode()->getWorldMatrix();
			mtxWolrd = mtxLocal*mtxWolrd;

			ui32 colour = color.getABGR();
			float tl,tt,tr,tb;
			tl = mMaterial->getTexAddrLeft();
			tr = mMaterial->getTexAddrRight();
			tt = mMaterial->getTexAddrTop();
			tb = mMaterial->getTexAddrButton();

			pVetLockPtr[0].pos = mPosCached[0]*mtxWolrd;
			pVetLockPtr[0].color = colour;
			pVetLockPtr[1].pos = mPosCached[1]*mtxWolrd;
			pVetLockPtr[1].color = colour;
			pVetLockPtr[2].pos = mPosCached[2]*mtxWolrd;
			pVetLockPtr[2].color = colour;
			pVetLockPtr[3].pos = mPosCached[3]*mtxWolrd;
			pVetLockPtr[3].color = colour;
			
			if(mMaterial->getSwapUV())
			{
				pVetLockPtr[0].tx = tr; pVetLockPtr[0].ty = tb;  //lb
				pVetLockPtr[1].tx = tl; pVetLockPtr[1].ty = tb;  //lt
				pVetLockPtr[2].tx = tr; pVetLockPtr[2].ty = tt;  //rb
				pVetLockPtr[3].tx = tl; pVetLockPtr[3].ty = tt;  //rt
			}
			else
			{
				pVetLockPtr[0].tx = tl; pVetLockPtr[0].ty = tb;
				pVetLockPtr[1].tx = tl; pVetLockPtr[1].ty = tt;
				pVetLockPtr[2].tx = tr; pVetLockPtr[2].ty = tb;
				pVetLockPtr[3].tx = tr; pVetLockPtr[3].ty = tt;
			}

			pIdxLockPtr[0] = vertBegin;
			pIdxLockPtr[1] = vertBegin+1;
			pIdxLockPtr[2] = vertBegin+2;

			pIdxLockPtr[3] = vertBegin+2;
			pIdxLockPtr[4] = vertBegin+1;
			pIdxLockPtr[5] = vertBegin+3;
		}
	}

	//-----------------------------------------------------------------------

	void EffectLayer3DBillBoard::copyAttributesTo(EffectParticle* particle)
	{
		EffectLayer::copyAttributesTo(particle);
		EffectLayer3DBillBoard* layerBillboard = static_cast<EffectLayer3DBillBoard*>(particle);
		layerBillboard->mWidth = mWidth;
		layerBillboard->mHeight = mHeight;
		layerBillboard->mCenterOffestW = mCenterOffestW;
		layerBillboard->mCenterOffestH = mCenterOffestH;
	}

	void EffectLayer3DBillBoard::exportData(DataStream* pStream, int version)
	{
		EffectLayer::exportData(pStream, version);
		pStream->write(&mWidth, sizeof(float));
		pStream->write(&mHeight, sizeof(float));
		pStream->write(&mCenterOffestW, sizeof(float));
		pStream->write(&mCenterOffestH, sizeof(float));
	}

	void EffectLayer3DBillBoard::importData(DataStream* pStream, int version)
	{
		EffectLayer::importData(pStream, version);
		pStream->read(&mWidth, sizeof(float));
		pStream->read(&mHeight, sizeof(float));
		pStream->read(&mCenterOffestW, sizeof(float));
		pStream->read(&mCenterOffestH, sizeof(float));
	}

	void EffectLayer3DBillBoard::mergeAligenBox(Box& aabb)
	{
		if (!mbAffected)
			return;

		float fwidth = mWidth*scale;
		float fheight = mHeight*scale;
		float adjust  = Math::Max(fwidth/2.f, fheight/2.f);
		Vector3  vecAdjust(adjust, adjust, adjust);
		aabb.addPoint(position + vecAdjust);
		aabb.addPoint(position - vecAdjust);
	}

	void EffectLayer3DBillBoard::getPropertyList(PropertyList & list)
	{
		EffectLayer::getPropertyList(list);
		list.push_back(token[TOKEN_LY_BB_WIDTH]);
		list.push_back(token[TOKEN_LY_BB_HEIGHT]);
		list.push_back(token[TOKEN_LY_BB_CENTEROFFSETW]);
		list.push_back(token[TOKEN_LY_BB_CENTEROFFSETH]);
	}

	bool EffectLayer3DBillBoard::getPropertyType(const String & name, PropertyType &type)
	{
		if(EffectLayer::getPropertyType(name, type))
			return true;
		if (name == token[TOKEN_LY_BB_WIDTH])
		{
			type = IElement::PT_REAL; return true;
		}
		else if(name == token[TOKEN_LY_BB_HEIGHT])
		{
			type = IElement::PT_REAL; return true;
		}
		else if(name == token[TOKEN_LY_BB_CENTEROFFSETW])
		{
			type = IElement::PT_REAL; return true;
		}
		else if(name == token[TOKEN_LY_BB_CENTEROFFSETH])
		{
			type = IElement::PT_REAL; return true;
		}
		return false;
	}

	bool EffectLayer3DBillBoard::getPropertyValue(const String & name, String &value)
	{
		if(EffectLayer::getPropertyValue(name, value))
			return true;

		if (name == token[TOKEN_LY_BB_WIDTH])
		{
			value = StringUtil::ToString(mWidth); return true;
		}
		else if (name == token[TOKEN_LY_BB_HEIGHT])
		{
			value = StringUtil::ToString(mHeight); return true;
		}
		else if (name == token[TOKEN_LY_BB_CENTEROFFSETW])
		{
			value = StringUtil::ToString(mCenterOffestW); return true;
		}
		else if (name == token[TOKEN_LY_BB_CENTEROFFSETH])
		{
			value = StringUtil::ToString(mCenterOffestH); return true;
		}
		return false;
	}

	bool EffectLayer3DBillBoard::setPropertyValue(const String & name,const String & value)
	{
		if(EffectLayer::setPropertyValue(name, value))
			return true;

		if (name == token[TOKEN_LY_BB_WIDTH])
		{
			mWidth = StringUtil::ParseReal(value); return true;
		}
		else if (name == token[TOKEN_LY_BB_HEIGHT])
		{
			mHeight = StringUtil::ParseReal(value); return true;
		}
		else if (name == token[TOKEN_LY_BB_CENTEROFFSETW])
		{
			mCenterOffestW = StringUtil::ParseReal(value); return true;
		}
		else if (name == token[TOKEN_LY_BB_CENTEROFFSETH])
		{
			mCenterOffestH = StringUtil::ParseReal(value); return true;
		}
		return false;
	}
}