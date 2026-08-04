#include "Core.h"
#include "EffectLayer2DBillBoard.h"
#include "EffectSystemManager.h"
#include "Scene/SceneNode.h"
#include "Resource/DataStream.h"

namespace LORD
{
	EffectLayer2DBillBoard::EffectLayer2DBillBoard()
		: EffectLayer()
		, mWidth(DEFAULT_LY_WIDTH)
		, mHeight(DEFAULT_LY_HEIGHT)
		, mCenterOffestW(DEFAULT_LY_CENTEROFFSETW)
		, mCenterOffestH(DEFAULT_LY_CENTEROFFSETH)
		, mOffsetViewport(DEFAULT_LY_OFFSETVIEWPORT)
		, mbPerpendicular(false)
	{
		originalDirection = Vector3::UNIT_Z;
		direction = Vector3::UNIT_Z;
		mType = ELT_2DBillboard;
	}

	EffectLayer2DBillBoard::~EffectLayer2DBillBoard()
	{

	}

	void EffectLayer2DBillBoard::_updateRenderQueue()
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
		if(pRenderable->getLockPtr(4, 6, vertBegin, pVetLockPtr, pIdxLockPtr))
		{
			float leftOff, rightOff, topOff, bottomOff; 
			Vector3 camX, camY;				
			Vector3 camDir;					

			leftOff = -mCenterOffestW;
			rightOff = 1.0f - mCenterOffestW;
			topOff = -mCenterOffestH;
			bottomOff = 1.0f - mCenterOffestH;

			Vector3 tmpBbPos = position;
			Vector3 vOwnOffset[4];
			Vector3 vLeffectOff, vRightOff, vTopOff, vBottomOff;

			if (mParentSystem->getCurrentCamera()->getProjectionMode() == Camera::PM_ORTHO)
			{
				camDir = -mParentSystem->getCameraOriInEffect().zAxis();
			}
			else
			{
				camDir = (tmpBbPos - mParentSystem->getCameraPosInEffect()).normalizedCopy();
			}
			if(mbPerpendicular)
			{
				camY = direction;
				camX = camDir.cross(camY);
				camX.normalize();
			}
			else
			{
				camX = mParentSystem->getCameraOriInEffect() * Vector3::UNIT_X;
				camY = mParentSystem->getCameraOriInEffect() * Vector3::UNIT_Y;
			}

			if(mOffsetViewport > 0.0f)
				tmpBbPos -= camDir*(mOffsetViewport);

			Quaternion rotSelf(1,0,0,0);
			if(Math::Abs(selfRotation) > 1.f)
			{
				Vector3 axis = -camDir;
				rotSelf.rotateAxis(axis, selfRotation*Math::DEG2RAD);
			}
			camX = rotSelf * camX;
			camY = rotSelf * camY;

			vLeffectOff = camX * (leftOff*mWidth*scale);
			vRightOff = camX * (rightOff*mWidth*scale);
			vTopOff = camY * (bottomOff*mHeight*scale);
			vBottomOff = camY * (topOff*mHeight*scale);

			vOwnOffset[0] = (vLeffectOff  + vBottomOff) + tmpBbPos;
			vOwnOffset[1] = (vLeffectOff  + vTopOff) + tmpBbPos;
			vOwnOffset[2] = (vRightOff + vBottomOff) + tmpBbPos;
			vOwnOffset[3] = (vRightOff + vTopOff) + tmpBbPos;

			Matrix4 mtxWorld = mParentSystem->getEffectSceneNode()->getWorldMatrix();

			ui32 colour = color.getABGR();
			float tl,tt,tr,tb;
			tl = mMaterial->getTexAddrLeft();
			tr = mMaterial->getTexAddrRight();
			tt = mMaterial->getTexAddrTop();
			tb = mMaterial->getTexAddrButton();

			pVetLockPtr[0].pos = vOwnOffset[0] * mtxWorld;
			pVetLockPtr[0].color = colour;
			pVetLockPtr[1].pos = vOwnOffset[1] * mtxWorld;
			pVetLockPtr[1].color = colour;
			pVetLockPtr[2].pos = vOwnOffset[2] * mtxWorld;
			pVetLockPtr[2].color = colour;
			pVetLockPtr[3].pos = vOwnOffset[3] * mtxWorld;
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

	void EffectLayer2DBillBoard::_notifyParabolicParam(const Vector3& p1, const Vector3& p2, const Vector3& p3)
	{
		vector<Vector3>::type points = {p1, p2, p3};
		for (auto iter : mKeyFrames)
		{
			iter->changeSpline(points);
		}
	}

	//-----------------------------------------------------------------------

	void EffectLayer2DBillBoard::copyAttributesTo(EffectParticle* particle)
	{
		EffectLayer::copyAttributesTo(particle);
		EffectLayer2DBillBoard* layerBillboard = static_cast<EffectLayer2DBillBoard*>(particle);
		layerBillboard->mWidth = mWidth;
		layerBillboard->mHeight = mHeight;
		layerBillboard->mCenterOffestW = mCenterOffestW;
		layerBillboard->mCenterOffestH = mCenterOffestH;
		layerBillboard->mOffsetViewport = mOffsetViewport;
		layerBillboard->mbPerpendicular = mbPerpendicular;
	}

	void EffectLayer2DBillBoard::exportData(DataStream* pStream, int version)
	{
		EffectLayer::exportData(pStream, version);
		pStream->write(&mWidth, sizeof(float));
		pStream->write(&mHeight, sizeof(float));
		pStream->write(&mCenterOffestW, sizeof(float));
		pStream->write(&mCenterOffestH, sizeof(float));
		pStream->write(&mOffsetViewport, sizeof(float));
		pStream->write(&mbPerpendicular, sizeof(bool));
	}

	void EffectLayer2DBillBoard::importData(DataStream* pStream, int version)
	{
		EffectLayer::importData(pStream, version);
		pStream->read(&mWidth, sizeof(float));
		pStream->read(&mHeight, sizeof(float));
		pStream->read(&mCenterOffestW, sizeof(float));
		pStream->read(&mCenterOffestH, sizeof(float));
		pStream->read(&mOffsetViewport, sizeof(float));
		pStream->read(&mbPerpendicular, sizeof(bool));
	}

	void EffectLayer2DBillBoard::mergeAligenBox(Box& aabb)
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

	void EffectLayer2DBillBoard::getPropertyList(PropertyList & list)
	{
		EffectLayer::getPropertyList(list);
		list.push_back(token[TOKEN_LY_BB_WIDTH]);
		list.push_back(token[TOKEN_LY_BB_HEIGHT]);
		list.push_back(token[TOKEN_LY_BB_CENTEROFFSETW]);
		list.push_back(token[TOKEN_LY_BB_CENTEROFFSETH]);
		list.push_back(token[TOKEN_LY_BB_OFFSETVIEWPORT]);
		list.push_back(token[TOKEN_LY_BB_PERPENDICULAR]);
	}

	bool EffectLayer2DBillBoard::getPropertyType(const String & name, PropertyType &type)
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
		else if(name == token[TOKEN_LY_BB_OFFSETVIEWPORT])
		{
			type = IElement::PT_REAL; return true;
		}
		else if(name == token[TOKEN_LY_BB_PERPENDICULAR])
		{
			type = IElement::PT_BOOL; return true;
		}
		return false;
	}

	bool EffectLayer2DBillBoard::getPropertyValue(const String & name, String &value)
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
		else if (name == token[TOKEN_LY_BB_OFFSETVIEWPORT])
		{
			value = StringUtil::ToString(mOffsetViewport); return true;
		}
		else if(name == token[TOKEN_LY_BB_PERPENDICULAR])
		{
			value = StringUtil::ToString(mbPerpendicular); return true;
		}
		return false;
	}

	bool EffectLayer2DBillBoard::setPropertyValue(const String & name,const String & value)
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
		else if (name == token[TOKEN_LY_BB_OFFSETVIEWPORT])
		{
			mOffsetViewport = StringUtil::ParseReal(value); return true;
		}
		else if(name == token[TOKEN_LY_BB_PERPENDICULAR])
		{
			mbPerpendicular = StringUtil::ParseBool(value); return true;
		}
		return false;
	}
}