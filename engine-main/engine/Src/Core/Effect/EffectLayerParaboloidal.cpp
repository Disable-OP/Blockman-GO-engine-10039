#include "Core.h"
#include "EffectLayerParaboloidal.h"
#include "EffectSystemManager.h"
#include "Scene/SceneNode.h"
#include "Resource/DataStream.h"

namespace LORD
{

	const i32 EffectLayerParaboloidal::sParaboloidalLatitude = 8;
	const i32 EffectLayerParaboloidal::sParaboloidalLongitude = 16;

	EffectLayerParaboloidal::EffectLayerParaboloidal()
		: EffectLayer()
		, mParam(1)
		, mHeight(1)
		, mVertices(NULL)
	{
		originalDirection = Vector3::UNIT_Z;
		direction = Vector3::UNIT_Z;
		mType = ELT_Paraboloidal;
	}

	EffectLayerParaboloidal::~EffectLayerParaboloidal()
	{
	}

	void EffectLayerParaboloidal::prepare()
	{
		EffectLayer::prepare();

		mVertices = (EffectVertexFmt2*)LordMalloc(sizeof(EffectVertexFmt2)*(sParaboloidalLatitude+1)*(sParaboloidalLongitude));

		float tl = mMaterial->getTexAddrLeft();
		float tr = mMaterial->getTexAddrRight();
		float tt = mMaterial->getTexAddrTop();
		float tb = mMaterial->getTexAddrButton();
		bool swapUV = mMaterial->getSwapUV();

		Vector3 baseAxis,currentAxis;
		float delta = Math::PI_2/sParaboloidalLongitude;
		Matrix4 mtxRotate;
		for(i32 i=0; i<=sParaboloidalLatitude; ++i)
		{
			baseAxis.z = mHeight / sParaboloidalLatitude * i;
			baseAxis.x = 0.0f;
			baseAxis.y = Math::Sqrt(baseAxis.z*mParam);
			
			float ulength = (tr-tl)/sParaboloidalLongitude*i;
			float vlength = (tb-tt)/sParaboloidalLongitude*i;

			for(i32 j=0; j<sParaboloidalLongitude; ++j)
			{
				mtxRotate.rotateAxisReplace(Vector3::UNIT_Z, delta*j);
				currentAxis = baseAxis * mtxRotate;

				mVertices[i*sParaboloidalLongitude+j].pos = currentAxis;
				if(swapUV)
				{
					mVertices[i*sParaboloidalLongitude+j].tx = tl+(tr-tl)*0.5f + Math::Cos(delta*j)*ulength;
					mVertices[i*sParaboloidalLongitude+j].ty = tt+(tb-tt)*0.5f + Math::Sin(delta*j)*vlength;
				}
				else
				{
					mVertices[i*sParaboloidalLongitude+j].tx = tl+(tr-tl)*0.5f + Math::Sin(delta*j)*ulength;
					mVertices[i*sParaboloidalLongitude+j].ty = tt+(tb-tt)*0.5f - Math::Cos(delta*j)*vlength;
				}
			}
		}
	}

	void EffectLayerParaboloidal::unprepare()
	{
		LordSafeFree(mVertices);
		EffectLayer::unprepare();
	}

	void EffectLayerParaboloidal::_updateRenderQueue()
	{
		if ((!mbAffected) || (!mbVisible) || (!mVertices))
			return;

		EffectRenderable* pRenderable = EffectSystemManager::Instance()->getRenderable(mMaterial->getTexturePtr(), mRenderGroup, mMaterial->getBlendMode(), mMaterial, (sParaboloidalLatitude + 1)*sParaboloidalLongitude, sParaboloidalLongitude*sParaboloidalLatitude * 6, mViewSpace);
		if(!pRenderable)
			return;

		pRenderable->setSpecialRender(mParentSystem->IsSpecialRender());

		EffectVertexFmt* pVertexPtr = NULL;
		ui16* pIdxLockPtr = NULL;
		ui16 vertBegin;
		if(pRenderable->getLockPtr((sParaboloidalLatitude+1)*sParaboloidalLongitude, sParaboloidalLongitude*sParaboloidalLatitude*6, vertBegin, pVertexPtr, pIdxLockPtr))
		{
			ui32 colour = color.getABGR();
			Matrix4 mtxWorld = mParentSystem->getEffectSceneNode()->getWorldMatrix();
			Matrix4 mtxLocal;
			mtxLocal.makeScaling(scale, scale, scale);
			Matrix4 mtxRot;
			Quaternion quanRot;
			quanRot.rotateAxis(Vector3::UNIT_Z, selfRotation*Math::DEG2RAD);
			quanRot = quanRot*oritation;
			quanRot.toMat4(mtxRot);
			mtxLocal = mtxLocal * mtxRot;
			mtxLocal.translate(position);
			mtxWorld = mtxLocal * mtxWorld;

			i32 i;
			for(i=0; i<(sParaboloidalLatitude+1)*sParaboloidalLongitude; ++i)
			{
				pVertexPtr[i].pos = mVertices[i].pos * mtxWorld;
				pVertexPtr[i].color = colour;
				pVertexPtr[i].tx = mVertices[i].tx; pVertexPtr[i].ty = mVertices[i].ty;
			}

			for(i=0; i<sParaboloidalLatitude; ++i)
			{
				for(i32 j=0; j<sParaboloidalLongitude; ++j)
				{
					if(j == sParaboloidalLongitude-1)
						*pIdxLockPtr++ = vertBegin+1-sParaboloidalLongitude;
					else
						*pIdxLockPtr++ = vertBegin+1;
					*pIdxLockPtr++ = vertBegin+sParaboloidalLongitude;
					*pIdxLockPtr++ = vertBegin;

					*pIdxLockPtr++ = vertBegin+sParaboloidalLongitude;
					if(j == sParaboloidalLongitude-1)
					{
						*pIdxLockPtr++ = vertBegin+1-sParaboloidalLongitude;
						*pIdxLockPtr++ = vertBegin+1;
					}
					else
					{
						*pIdxLockPtr++ = vertBegin+1;
						*pIdxLockPtr++ = vertBegin+1+sParaboloidalLongitude;
					}
					vertBegin += 1;
				}
			}
		}
	}

	void EffectLayerParaboloidal::copyAttributesTo(EffectParticle* particle)
	{
		EffectLayer::copyAttributesTo(particle);
		EffectLayerParaboloidal* paraboloidal = static_cast<EffectLayerParaboloidal*>(particle);
		paraboloidal->mParam = mParam;
		paraboloidal->mHeight = mHeight;
	}

	void EffectLayerParaboloidal::exportData(DataStream* pStream, int version)
	{
		EffectLayer::exportData(pStream, version);
		pStream->write(&mParam, sizeof(float));
		pStream->write(&mHeight, sizeof(float));
	}

	void EffectLayerParaboloidal::importData(DataStream* pStream, int version)
	{
		EffectLayer::importData(pStream, version);
		pStream->read(&mParam, sizeof(float));
		pStream->read(&mHeight, sizeof(float));
	}

	void EffectLayerParaboloidal::mergeAligenBox(Box& aabb)
	{
		if (!mbAffected)
			return;

		Matrix4 mtxLocal;
		mtxLocal.makeScaling(scale, scale, scale);
		Matrix4 mtxRot;
		Quaternion quanRot;
		quanRot.rotateAxis(Vector3::UNIT_Z, selfRotation*Math::DEG2RAD);
		quanRot = quanRot*oritation;
		quanRot.toMat4(mtxRot);
		mtxLocal = mtxLocal * mtxRot;
		mtxLocal.translate(position);

		for(size_t i=0; i<(sParaboloidalLatitude+1)*(sParaboloidalLongitude); ++i)
		{
			aabb.addPoint(mVertices[i].pos * mtxLocal);
		}
	}

	void EffectLayerParaboloidal::getPropertyList(PropertyList& list)
	{
		EffectLayer::getPropertyList(list);
		list.push_back(token[TOKEN_LY_PB_PARAM]);
		list.push_back(token[TOKEN_LY_PB_HEIGHT]);
	}

	bool EffectLayerParaboloidal::getPropertyType(const String& name, PropertyType& type)
	{
		if(EffectLayer::getPropertyType(name, type))
			return true;
		if (name == token[TOKEN_LY_PB_PARAM])
		{
			type = IElement::PT_REAL; return true;
		}
		else if(name == token[TOKEN_LY_PB_HEIGHT])
		{
			type = IElement::PT_REAL; return true;
		}
		return false;
	}

	bool EffectLayerParaboloidal::getPropertyValue(const String& name, String& value)
	{
		if(EffectLayer::getPropertyValue(name, value))
			return true;

		if (name == token[TOKEN_LY_PB_PARAM])
		{
			value = StringUtil::ToString(mParam); return true;
		}
		else if(name == token[TOKEN_LY_PB_HEIGHT])
		{
			value = StringUtil::ToString(mHeight); return true;
		}

		return false;
	}

	bool EffectLayerParaboloidal::setPropertyValue(const String& name,const String& value)
	{
		if(EffectLayer::setPropertyValue(name, value))
			return true;

		if (name == token[TOKEN_LY_PB_PARAM])
		{
			mParam = StringUtil::ParseReal(value); return true;
		}
		else if(name == token[TOKEN_LY_PB_HEIGHT])
		{
			mHeight = StringUtil::ParseReal(value); return true;
		}

		return false;
	}
}