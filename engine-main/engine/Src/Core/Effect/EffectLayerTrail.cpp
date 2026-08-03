#include "Core.h"
#include "EffectLayerTrail.h"
#include "EffectSystemManager.h"
#include "Scene/SceneNode.h"
#include "Resource/DataStream.h"

namespace LORD
{
	const float EffectLayerTrail::minDistance = 0.005f;

	EffectLayerTrail::EffectLayerTrail()
		: EffectLayer()
		, mOriginalPosition1(0, 0.2f, 0)
		, mOriginalPosition2(0, -0.2f, 0)
		, mDurationTime(500)
		, mFallowParent(false)
		, mElementNum(0)
	{
		originalDirection = Vector3::UNIT_Z;
		direction = Vector3::UNIT_Z;
		mType = ELT_TRAIL;
	}

	EffectLayerTrail::~EffectLayerTrail()
	{
		mTrailElementList.clear();
	}

	void EffectLayerTrail::unprepare()
	{
		EffectLayer::unprepare();
		mTrailElementList.clear();
		mElementNum = 0;
		mLastPosition = Vector3::ZERO;
	}

	int EffectLayerTrail::_update(i32 time)
	{
		int ret = EffectLayer::_update(time);

		if(mbAffected)
		{
			// test need add vertex.
			Vector3 transed, currentPos;
			if(mFallowParent)
			{
				currentPos = position;
				transed = position - mLastPosition;
			}
			else
			{
				const Matrix4 &mtxWorld = mParentSystem->getEffectSceneNode()->getWorldMatrix();
				currentPos = position * mtxWorld;
				transed = currentPos - mLastPosition;
			}

			TrailElementIter it = mTrailElementList.begin();
			mElementNum = 0;
			for(; it!=mTrailElementList.end(); ++it)
			{
				TrailElement& element = *it;
				element.mcurrentTime -= time;
				if(element.mcurrentTime <= 0)
				{
					break;
				}
				mElementNum ++;
			}
			mTrailElementList.erase(it, mTrailElementList.end());

			if(transed.lenSqr() > minDistance)
			{
				mLastPosition = currentPos;
				// scale
				Vector3 pos1,pos2;
				pos1 = mOriginalPosition1*scale;
				pos2 = mOriginalPosition2*scale;

				// rotate self
				Quaternion rotSelf(1,0,0,0);
				if(Math::Abs(selfRotation)>1.f)
				{
					rotSelf.rotateAxis(direction, selfRotation*Math::DEG2RAD);
				}

				// oritation
				rotSelf = rotSelf * oritation;
				pos1 = rotSelf * pos1;
				pos2 = rotSelf * pos2;
				pos1 += position;
				pos2 += position;

				if(!mFallowParent)
				{
					const Matrix4 &mtxWorld = mParentSystem->getEffectSceneNode()->getWorldMatrix();
					pos1 = pos1 * mtxWorld;
					pos2 = pos2 * mtxWorld;
				}
				TrailElement element(pos1, pos2, mDurationTime);
				mTrailElementList.push_front(element);
				mElementNum ++;		
			}
		}
		return ret;
	}

	void EffectLayerTrail::_updateRenderQueue()
	{
		if ((!mbAffected) || (!mbVisible) || mElementNum<2)
			return;

		LordAssertX(mElementNum == mTrailElementList.size(), "Error: EffectLayerTrail update error");

		EffectRenderable* pRenderable = EffectSystemManager::Instance()->getRenderable(mMaterial->getTexturePtr(), mRenderGroup, mMaterial->getBlendMode(), mMaterial, mElementNum * 2, 6 * (mElementNum - 1), mViewSpace);
		if(!pRenderable)
			return;

		pRenderable->setSpecialRender(mParentSystem->IsSpecialRender());

		EffectVertexFmt* pVertexPtr = NULL;
		ui16* pIdxLockPtr = NULL;
		ui16 vertBegin;
		if(pRenderable->getLockPtr(mElementNum*2, 6*(mElementNum-1), vertBegin, pVertexPtr, pIdxLockPtr))
		{
			ui32 colour;
			Color tcolor = color;
			size_t elementCount = 0;
			Matrix4 mtxWorld;
			float tl,tt,tr,tb;
			float texslerp;
			Vector3 lastbottom, lasttop;
			TrailElementIter it = mTrailElementList.begin();

			if(!mFallowParent)
			{
				mtxWorld = mParentSystem->getEffectSceneNode()->getWorldMatrix();
			}
			else
			{
				mtxWorld.identity();
			}

			tl = mMaterial->getTexAddrLeft();
			tr = mMaterial->getTexAddrRight();
			tt = mMaterial->getTexAddrTop();
			tb = mMaterial->getTexAddrButton();
			bool swapUV = mMaterial->getSwapUV();

			for(; it!=mTrailElementList.end(); ++it, ++elementCount) 
			{
				TrailElement& nextElement = *it;
				float timeslerp = (float(nextElement.mcurrentTime)/float(mDurationTime));
				if(swapUV)
					texslerp = tb - (tb-tt)*timeslerp;
				else
					texslerp = tr - (tr-tl)*timeslerp;
				tcolor = color;
				tcolor.a = tcolor.a * timeslerp;
				colour = tcolor.getABGR();
				lasttop = nextElement.position1;
				lastbottom = nextElement.position2;
				if(mFallowParent)
				{
					lastbottom = lastbottom * mtxWorld;
					lasttop = lasttop * mtxWorld;
				}
				size_t idx = elementCount*2;
				pVertexPtr[idx].pos = lastbottom;
				pVertexPtr[idx].color = colour;
				if(swapUV)
				{
					pVertexPtr[idx].tx = tl;
					pVertexPtr[idx].ty = texslerp;
				}
				else
				{
					pVertexPtr[idx].tx = texslerp;
					pVertexPtr[idx].ty = tb;
				}
				idx = elementCount*2+1;
				pVertexPtr[idx].pos = lasttop;
				pVertexPtr[idx].color = colour;
				if(swapUV)
				{
					pVertexPtr[idx].tx = tr;
					pVertexPtr[idx].ty = texslerp;
				}
				else
				{
					pVertexPtr[idx].tx = texslerp;
					pVertexPtr[idx].ty = tt;
				}
			}

			ui16 baseInx, baseVtx;
			for(ui16 i=0; i<mElementNum-1; ++i)
			{
				baseInx = i*6;
				baseVtx = vertBegin+i*2;
				pIdxLockPtr[baseInx]   = baseVtx;
				pIdxLockPtr[baseInx+1] = baseVtx+1;
				pIdxLockPtr[baseInx+2] = baseVtx+2;

				pIdxLockPtr[baseInx+3] = baseVtx+2;
				pIdxLockPtr[baseInx+4] = baseVtx+1;
				pIdxLockPtr[baseInx+5] = baseVtx+3;
			}
		}
	}

	void EffectLayerTrail::exportData(DataStream* pStream, int version)
	{
		EffectLayer::exportData(pStream, version);
		pStream->write(&mOriginalPosition1, sizeof(float)*3);
		pStream->write(&mOriginalPosition2, sizeof(float)*3);
		pStream->write(&mDurationTime, sizeof(int));
		pStream->write(&mFallowParent, sizeof(bool));
	}

	void EffectLayerTrail::importData(DataStream* pStream, int version)
	{
		EffectLayer::importData(pStream, version);
		pStream->read(&mOriginalPosition1, sizeof(float)*3);
		pStream->read(&mOriginalPosition2, sizeof(float)*3);
		pStream->read(&mDurationTime, sizeof(int));
		pStream->read(&mFallowParent, sizeof(bool));
	}

	void EffectLayerTrail::mergeAligenBox(Box& aabb)
	{
		if (!mbAffected)
			return;

		TrailElementIter it = mTrailElementList.begin();
		for(; it!=mTrailElementList.end(); ++it)
		{
			TrailElement& element = *it;
			// in editor, it is this. dont thank about the fllow parent property.
			aabb.addPoint(element.position1);
			aabb.addPoint(element.position2);
		}
	}

	void EffectLayerTrail::copyAttributesTo(EffectParticle* particle)
	{
		EffectLayer::copyAttributesTo(particle);
		EffectLayerTrail* trail = static_cast<EffectLayerTrail*>(particle);
		trail->mOriginalPosition1 = mOriginalPosition1;
		trail->mOriginalPosition2 = mOriginalPosition2;
		trail->mDurationTime = mDurationTime;
		trail->mFallowParent = mFallowParent;
	}

	void EffectLayerTrail::getPropertyList(PropertyList& list)
	{
		EffectLayer::getPropertyList(list);
		list.push_back(token[TOKEN_LY_TR_POSITION1]);
		list.push_back(token[TOKEN_LY_TR_POSITION2]);
		list.push_back(token[TOKEN_LY_TR_DURATIONTIME]);
		list.push_back(token[TOKEN_LY_TR_FALLOWPARENT]);
	}

	bool EffectLayerTrail::getPropertyType(const String& name, PropertyType& type)
	{
		if(EffectLayer::getPropertyType(name, type))
			return true;

		if (name == token[TOKEN_LY_TR_POSITION1])
		{
			type = IElement::PT_VECTOR3; return true;
		}
		else if(name == token[TOKEN_LY_TR_POSITION2])
		{
			type = IElement::PT_VECTOR3; return true;
		}
		else if(name == token[TOKEN_LY_TR_DURATIONTIME])
		{
			type = IElement::PT_INT; return true;
		}
		else if(name == token[TOKEN_LY_TR_FALLOWPARENT])
		{
			type = IElement::PT_BOOL; return true;
		}

		return false;
	}

	bool EffectLayerTrail::getPropertyValue(const String& name, String& value)
	{
		if(EffectLayer::getPropertyValue(name, value))
			return true;

		if (name == token[TOKEN_LY_TR_POSITION1])
		{
			value = StringUtil::ToString(mOriginalPosition1); return true;
		}
		else if(name == token[TOKEN_LY_TR_POSITION2])
		{
			value = StringUtil::ToString(mOriginalPosition2); return true;
		}
		else if(name == token[TOKEN_LY_TR_DURATIONTIME])
		{
			value = StringUtil::ToString(mDurationTime); return true;
		}
		else if(name == token[TOKEN_LY_TR_FALLOWPARENT])
		{
			value = StringUtil::ToString(mFallowParent); return true;
		}

		return false;
	}

	bool EffectLayerTrail::setPropertyValue(const String& name,const String& value)
	{
		if(EffectLayer::setPropertyValue(name, value))
			return true;

		if (name == token[TOKEN_LY_TR_POSITION1])
		{
			mOriginalPosition1 = StringUtil::ParseVec3(value); return true;
		}
		else if(name == token[TOKEN_LY_TR_POSITION2])
		{
			mOriginalPosition2 = StringUtil::ParseVec3(value); return true;
		}
		else if(name == token[TOKEN_LY_TR_DURATIONTIME])
		{
			mDurationTime = StringUtil::ParseInt(value); return true;
		}
		else if(name == token[TOKEN_LY_TR_FALLOWPARENT])
		{
			mFallowParent = StringUtil::ParseBool(value); return true;
		}

		return false;
	}
}