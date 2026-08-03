#include "Core.h"
#include "EffectLayerColumn.h"
#include "EffectSystemManager.h"
#include "Scene/SceneNode.h"
#include "Resource/DataStream.h"

namespace LORD
{

	EffectLayerColumn::EffectLayerColumn()
		: EffectLayer()
		, mSegment(12)
		, mRadiusBottom(1)
		, mRadiusTop(1)
		, mHeight(1)
		, mNoScaleRadius(false)
		, mNoScaleHeight(false)
		, mKeepCenter(false)
		, mTextureClamp(false)
		, mVertices(NULL)
	{
		originalDirection = Vector3::UNIT_Y;
		direction = Vector3::UNIT_Y;
		mType = ELT_Column;
	}

	EffectLayerColumn::~EffectLayerColumn()
	{

	}

	void EffectLayerColumn::prepare()
	{
		EffectLayer::prepare();
		mVertices = (EffectVertexFmt2*)LordMalloc(sizeof(EffectVertexFmt2)*mSegment*4);
		Vector3 initVertexTop(mRadiusTop, mHeight, 0);
		Vector3 initVertexBottom(mRadiusBottom, 0, 0);
		if(mKeepCenter)
		{
			initVertexTop.y /= 2.0f;
			initVertexBottom.y = -initVertexTop.y;
		}

		float tl = mMaterial->getTexAddrLeft();
		float tr = mMaterial->getTexAddrRight();
		float tt = mMaterial->getTexAddrTop();
		float tb = mMaterial->getTexAddrButton();

		Vector3 vertexTop, vertexBottom;
		float iangle = 0;
		float idelta =360.0f/mSegment*Math::DEG2RAD;
		Matrix4 mtxRotate;
		bool swapUV = mMaterial->getSwapUV();
		mtxRotate.rotateAxisReplace(Vector3::UNIT_Y, iangle);
		for(i32 i=0; i<mSegment; ++i)
		{
			vertexTop = initVertexTop * mtxRotate;
			vertexBottom = initVertexBottom * mtxRotate;

			mVertices[i*4].pos = vertexBottom;
			mVertices[i*4+1].pos = vertexTop;
			if(swapUV)
			{
				mVertices[i*4].ty = mTextureClamp? tb : (tb-(tb-tt)/mSegment*i);
				mVertices[i*4].tx = tr;
				mVertices[i*4+1].ty = mTextureClamp? tb : (tb-(tb-tt)/mSegment*i);
				mVertices[i*4+1].tx = tl;
			}
			else
			{
				mVertices[i*4].ty = tb;
				mVertices[i*4].tx = mTextureClamp? tl : (tl+(tr-tl)/mSegment*i);
				mVertices[i*4+1].ty = tt;
				mVertices[i*4+1].tx = mTextureClamp? tl : (tl+(tr-tl)/mSegment*i);
			}

			iangle += idelta;
			mtxRotate.rotateAxisReplace(Vector3::UNIT_Y, iangle);

			vertexTop = initVertexTop * mtxRotate;
			vertexBottom = initVertexBottom * mtxRotate;

			mVertices[i*4+2].pos = vertexBottom;
			mVertices[i*4+3].pos = vertexTop;

			if(swapUV)
			{
				mVertices[i*4+2].ty = mTextureClamp? tt : (tb-(tb-tt)/mSegment*i);
				mVertices[i*4+2].tx = tr;
				mVertices[i*4+3].ty = mTextureClamp? tt : (tb-(tb-tt)/mSegment*i);
				mVertices[i*4+3].tx = tl;
			}
			else
			{
				mVertices[i*4+2].ty = tb;
				mVertices[i*4+2].tx = mTextureClamp? tr : (tl+(tr-tl)/mSegment*(i+1));
				mVertices[i*4+3].ty = tt;
				mVertices[i*4+3].tx = mTextureClamp? tr : (tl+(tr-tl)/mSegment*(i+1));
			}
		}
	}

	void EffectLayerColumn::unprepare()
	{
		LordSafeFree(mVertices);
		EffectLayer::unprepare();
	}

	void EffectLayerColumn::_updateRenderQueue()
	{
		if ((!mbAffected) || (!mbVisible) || (!mVertices))
			return;

		i32 vertNeed = 2 * (mSegment + 1);
		if (mTextureClamp)
			vertNeed = 4 * mSegment;

		EffectRenderable* pRenderable = EffectSystemManager::Instance()->getRenderable(mMaterial->getTexturePtr(), mRenderGroup, mMaterial->getBlendMode(), mMaterial, vertNeed, 6 * mSegment, mViewSpace);
		if(!pRenderable)
			return;

		pRenderable->setSpecialRender(mParentSystem->IsSpecialRender());

		EffectVertexFmt* pVertexPtr = NULL;
		ui16* pIdxLockPtr = NULL;
		ui16 vertBegin;
		if(pRenderable->getLockPtr(vertNeed, 6*mSegment, vertBegin, pVertexPtr, pIdxLockPtr))
		{
			ui32 colour = color.getABGR();
			Matrix4 mtxWorld = mParentSystem->getEffectSceneNode()->getWorldMatrix();

			Matrix4 mtxLocal;
			Vector3 vScale(scale, scale, scale);
			if(mNoScaleHeight)
				vScale.y = 1;
			if(mNoScaleRadius)
				vScale.x = vScale.z = 1;
			mtxLocal.makeScaling(vScale);

			Matrix4 mtxRot;
			Quaternion quanRot;
			quanRot.rotateAxis(direction, selfRotation*Math::DEG2RAD);
			quanRot = quanRot*oritation;
			quanRot.toMat4(mtxRot);
			mtxLocal = mtxLocal * mtxRot;
			mtxLocal.translate(position);

			mtxWorld = mtxLocal * mtxWorld;
			i32 i = 0;
			ui16 baseInx,baseVtx;
			if(mTextureClamp)
			{
				// to reduce twice matrix mutipy coast.
				Vector3 lastbottomVec = mVertices[0].pos * mtxWorld;
				Vector3 lasttopVec = mVertices[1].pos * mtxWorld;

				for(; i<mSegment; ++i)
				{
					pVertexPtr[i*4].pos = lastbottomVec;
					pVertexPtr[i*4].color = colour;
					pVertexPtr[i*4].tx = mVertices[i*4].tx;
					pVertexPtr[i*4].ty = mVertices[i*4].ty;

					pVertexPtr[i*4+1].pos = lasttopVec;
					pVertexPtr[i*4+1].color = colour;
					pVertexPtr[i*4+1].tx = mVertices[i*4+1].tx;
					pVertexPtr[i*4+1].ty = mVertices[i*4+1].ty;

					// remenber the vec pos.
					pVertexPtr[i*4+2].pos = lastbottomVec = mVertices[i*4+2].pos * mtxWorld;
					pVertexPtr[i*4+2].color = colour;
					pVertexPtr[i*4+2].tx = mVertices[i*4+2].tx;
					pVertexPtr[i*4+2].ty = mVertices[i*4+2].ty;

					pVertexPtr[i*4+3].pos = lasttopVec = mVertices[i*4+3].pos * mtxWorld;
					pVertexPtr[i*4+3].color = colour;
					pVertexPtr[i*4+3].tx = mVertices[i*4+3].tx;
					pVertexPtr[i*4+3].ty = mVertices[i*4+3].ty;
				}
			}
			else
			{
				for(i=0; i<mSegment; ++i)
				{
					pVertexPtr[i*2].pos = mVertices[i*4].pos * mtxWorld;
					pVertexPtr[i*2].color = colour;
					pVertexPtr[i*2].tx = mVertices[i*4].tx;
					pVertexPtr[i*2].ty = mVertices[i*4].ty;

					pVertexPtr[i*2+1].pos = mVertices[i*4+1].pos * mtxWorld;
					pVertexPtr[i*2+1].color = colour;
					pVertexPtr[i*2+1].tx = mVertices[i*4+1].tx;
					pVertexPtr[i*2+1].ty = mVertices[i*4+1].ty;
				}
				pVertexPtr[mSegment*2].pos = pVertexPtr[0].pos;
				pVertexPtr[mSegment*2].color = colour;
				pVertexPtr[mSegment*2].tx = mVertices[mSegment*4-2].tx;
				pVertexPtr[mSegment*2].ty = mVertices[mSegment*4-2].ty;

				pVertexPtr[mSegment*2+1].pos = pVertexPtr[1].pos;
				pVertexPtr[mSegment*2+1].color = colour;
				pVertexPtr[mSegment*2+1].tx = mVertices[mSegment*4-1].tx;
				pVertexPtr[mSegment*2+1].ty = mVertices[mSegment*4-1].ty;
			}
			for(i=0; i<mSegment; ++i)
			{
				baseInx = i*6;
				baseVtx = vertBegin+i*(mTextureClamp?4:2);
				pIdxLockPtr[baseInx]   = baseVtx;
				pIdxLockPtr[baseInx+1] = baseVtx+1;
				pIdxLockPtr[baseInx+2] = baseVtx+2;

				pIdxLockPtr[baseInx+3] = baseVtx+2;
				pIdxLockPtr[baseInx+4] = baseVtx+1;
				pIdxLockPtr[baseInx+5] = baseVtx+3;
			}
		}
	}

	void EffectLayerColumn::copyAttributesTo(EffectParticle* particle)
	{
		EffectLayer::copyAttributesTo(particle);
		EffectLayerColumn* column = static_cast<EffectLayerColumn*>(particle);
		column->mSegment = mSegment;
		column->mRadiusBottom = mRadiusBottom;
		column->mRadiusTop = mRadiusTop;
		column->mHeight = mHeight;
		column->mNoScaleRadius = mNoScaleRadius;
		column->mNoScaleHeight = mNoScaleHeight;
		column->mKeepCenter = mKeepCenter;
		column->mTextureClamp = mTextureClamp;
	}

	void EffectLayerColumn::exportData(DataStream* pStream, int version)
	{
		EffectLayer::exportData(pStream, version);
		pStream->write(&mSegment, sizeof(i32));
		pStream->write(&mRadiusBottom, sizeof(float));
		pStream->write(&mRadiusTop, sizeof(float));
		pStream->write(&mHeight, sizeof(float));
		pStream->write(&mNoScaleRadius, sizeof(bool));
		pStream->write(&mNoScaleHeight, sizeof(bool));
		pStream->write(&mKeepCenter, sizeof(bool));
		pStream->write(&mTextureClamp, sizeof(bool));
	}

	void EffectLayerColumn::importData(DataStream* pStream, int version)
	{
		EffectLayer::importData(pStream, version);
		pStream->read(&mSegment, sizeof(i32));
		pStream->read(&mRadiusBottom, sizeof(float));
		pStream->read(&mRadiusTop, sizeof(float));
		pStream->read(&mHeight, sizeof(float));
		pStream->read(&mNoScaleRadius, sizeof(bool));
		pStream->read(&mNoScaleHeight, sizeof(bool));
		pStream->read(&mKeepCenter, sizeof(bool));
		pStream->read(&mTextureClamp, sizeof(bool));
	}

	void EffectLayerColumn::mergeAligenBox(Box& aabb)
	{
		if (!mbAffected)
			return;

		Matrix4 mtxLocal;
		Vector3 vScale(scale, scale, scale);
		if(mNoScaleHeight)
			vScale.y = 1;
		if(mNoScaleRadius)
			vScale.x = vScale.z = 1;
		mtxLocal.makeScaling(vScale);

		Matrix4 mtxRot;
		Quaternion quanRot;
		quanRot.rotateAxis(direction, selfRotation*Math::DEG2RAD);
		quanRot = quanRot*oritation;
		quanRot.toMat4(mtxRot);
		mtxLocal = mtxLocal * mtxRot;
		mtxLocal.translate(position);

		for(i32 i=0; i<mSegment; ++i)
		{
			aabb.addPoint(mVertices[i].pos * mtxLocal);
		}
	}

	void EffectLayerColumn::getPropertyList(PropertyList & list)
	{
		EffectLayer::getPropertyList(list);
		list.push_back(token[TOKEN_LY_CO_SEGMENT]);
		list.push_back(token[TOKEN_LY_CO_RADIUSBOTTOM]);
		list.push_back(token[TOKEN_LY_CO_RADIUSTOP]);
		list.push_back(token[TOKEN_LY_CO_HEIGHT]);
		list.push_back(token[TOKEN_LY_CO_NOSCALERADIUS]);
		list.push_back(token[TOKEN_LY_CO_NOSCALEHEIGHT]);
		list.push_back(token[TOKEN_LY_CO_KEEPCENTER]);
		list.push_back(token[TOKEN_LY_CO_TEXCLAMP]);
	}

	bool EffectLayerColumn::getPropertyType(const String & name, PropertyType &type)
	{
		if(EffectLayer::getPropertyType(name, type))
			return true;
		if (name == token[TOKEN_LY_CO_SEGMENT])
		{
			type = IElement::PT_INT; return true;
		}
		else if(name == token[TOKEN_LY_CO_RADIUSBOTTOM])
		{
			type = IElement::PT_REAL; return true;
		}
		else if(name == token[TOKEN_LY_CO_RADIUSTOP])
		{
			type = IElement::PT_REAL; return true;
		}
		else if(name == token[TOKEN_LY_CO_HEIGHT])
		{
			type = IElement::PT_REAL; return true;
		}
		else if(name == token[TOKEN_LY_CO_NOSCALERADIUS])
		{
			type = IElement::PT_BOOL; return true;
		}
		else if(name == token[TOKEN_LY_CO_NOSCALEHEIGHT])
		{
			type = IElement::PT_BOOL; return true;
		}
		else if(name == token[TOKEN_LY_CO_KEEPCENTER])
		{
			type = IElement::PT_BOOL; return true;
		}
		else if(name == token[TOKEN_LY_CO_TEXCLAMP])
		{
			type = IElement::PT_BOOL; return true;
		}
		return false;
	}

	bool EffectLayerColumn::getPropertyValue(const String & name, String &value)
	{
		if(EffectLayer::getPropertyValue(name, value))
			return true;

		if (name == token[TOKEN_LY_CO_SEGMENT])
		{
			value = StringUtil::ToString(mSegment); return true;
		}
		else if(name == token[TOKEN_LY_CO_RADIUSBOTTOM])
		{
			value = StringUtil::ToString(mRadiusBottom); return true;
		}
		else if(name == token[TOKEN_LY_CO_RADIUSTOP])
		{
			value = StringUtil::ToString(mRadiusTop); return true;
		}
		else if(name == token[TOKEN_LY_CO_HEIGHT])
		{
			value = StringUtil::ToString(mHeight); return true;
		}
		else if(name == token[TOKEN_LY_CO_NOSCALERADIUS])
		{
			value = StringUtil::ToString(mNoScaleRadius); return true;
		}
		else if(name == token[TOKEN_LY_CO_NOSCALEHEIGHT])
		{
			value = StringUtil::ToString(mNoScaleHeight); return true;
		}
		else if(name == token[TOKEN_LY_CO_KEEPCENTER])
		{
			value = StringUtil::ToString(mKeepCenter); return true;
		}
		else if(name == token[TOKEN_LY_CO_TEXCLAMP])
		{
			value = StringUtil::ToString(mTextureClamp); return true;
		}

		return false;
	}

	bool EffectLayerColumn::setPropertyValue(const String & name,const String & value)
	{
		if(EffectLayer::setPropertyValue(name, value))
			return true;

		if (name == token[TOKEN_LY_CO_SEGMENT])
		{
			mSegment = StringUtil::ParseInt(value); return true;
		}
		else if(name == token[TOKEN_LY_CO_RADIUSBOTTOM])
		{
			mRadiusBottom = StringUtil::ParseReal(value); return true;
		}
		else if(name == token[TOKEN_LY_CO_RADIUSTOP])
		{
			mRadiusTop = StringUtil::ParseReal(value); return true;
		}
		else if(name == token[TOKEN_LY_CO_HEIGHT])
		{
			mHeight = StringUtil::ParseReal(value); return true;
		}
		else if(name == token[TOKEN_LY_CO_NOSCALERADIUS])
		{
			mNoScaleRadius = StringUtil::ParseBool(value); return true;
		}
		else if(name == token[TOKEN_LY_CO_NOSCALEHEIGHT])
		{
			mNoScaleHeight = StringUtil::ParseBool(value); return true;
		}
		else if(name == token[TOKEN_LY_CO_KEEPCENTER])
		{
			mKeepCenter = StringUtil::ParseBool(value); return true;
		}
		else if(name == token[TOKEN_LY_CO_TEXCLAMP])
		{
			mTextureClamp = StringUtil::ParseBool(value); return true;
		}

		return false;
	}
}