#include "Core.h"
#include "EffectLayerRope.h"
#include "EffectSystemManager.h"
#include "Image/Image.h"
#include "Geom/SimpleSpline.h"
#include "Scene/SceneNode.h"
#include "Resource/DataStream.h"

namespace LORD
{
	EffectLayerRope::EffectLayerRope()
		: EffectLayer()
		, mStartPos(0, 2.0f, 0)
		, mEndPos(0, 0, 0)
		, mNormal(0, 1, 0)
		, mSplineIntDir(Vector3::ZERO)
		, mWidthStart(0.1f)
		, mWidthEnd(0.1f)
		, mAlphaStart(0.1f)
		, mAlphaEnd(0.1f)
		, mAlphaMid(1.0f)
		, mAmplitude(0.2f)
		, mUVscale(0.0f)
		, mCurDeltaUV(0.0f)
		, mFlyRadio(0.0f)
		, mSegment(10)
		, mFlyTime(0)
		, mEscapTime(0)
		, mVertPairNum(0)
		, mExtenalParam(false)
		, mUseNormal(false)
		, mSplineIntPos(NULL)
	{
		originalDirection = Vector3::UNIT_Z;
		direction = Vector3::UNIT_Z;

		mType = ELT_Rope;
	}

	EffectLayerRope::~EffectLayerRope()
	{

	}

	void EffectLayerRope::prepare()
	{
		EffectLayer::prepare();

		if (mSegment < 2) mSegment = 2;
		mVertPairNum = mSegment + 1;

		mSplineIntPos = (Vector3*)LordMalloc(sizeof(Vector3)*(mSegment + 1));
	}

	void EffectLayerRope::unprepare()
	{
		EffectLayer::unprepare();

		LordSafeFree(mSplineIntPos);
	}

	int EffectLayerRope::_update(i32 time)
	{
		int ret = EffectLayer::_update(time);

		if (mbAffected)
		{
			mEscapTime = Math::Min(mEscapTime += time, mFlyTime);
			mFlyRadio = (mFlyTime > 0) ? Math::Min(1.0f, (float)mEscapTime / (float)mFlyTime) : 1.0f;
			updatePath();
		}
		mCurDeltaUV -= ((mFlyRadio == 1.0f) ? (mUVscale * (float)time*0.001f) : 0.0f);
		return ret;
	}

	void EffectLayerRope::updatePath()
	{
		int nCount = mSegment;
		SimpleSpline* pSpline = LordNew(SimpleSpline);
		if (!mIsWiggly)
		{
			Vector3 dir = mStartPos - mEndPos;
			float dist = dir.normalizeLen();
			Vector3 up;
			if (mUseNormal)
			{
				Vector3 vNormal = mParentSystem->getEffectSceneNode()->getWorldOrientation() * mNormal;
				up = dir.cross(vNormal);
			}
			else
			{
				up = dir.cross((mParentSystem->getCurrentCamera()->getPosition() - mStartPos).normalizedCopy().abs());
				up.abs();
			}
			Vector3 mid = (mStartPos + mEndPos) * 0.5f + up * Math::Abs(mAmplitude - dist) / dist * mAmplitude;
			pSpline->addPoint(mStartPos);
			pSpline->addPoint(mid);
			pSpline->addPoint(mEndPos);
		}
		else
		{
			Vector3 point_1, point_2, point_3, point_4, point_5;
			float extensionValue = 0.2f;
			float x_1 = -Math::Sin((mYaw_1 - 90.0f) * Math::DEG2RAD) * extensionValue;
			float z_1 = Math::Cos((mYaw_1 - 90.0f) * Math::DEG2RAD) * extensionValue;
			float x_5 = -Math::Sin((mYaw_2 - 90.0f) * Math::DEG2RAD) * extensionValue;
			float z_5 = Math::Cos((mYaw_2 - 90.0f) * Math::DEG2RAD) * extensionValue;

			float scaleValue = 0.8f;
			float deltaScale = 1.0f - Math::Cos((mYaw_1 - mYaw_2) * Math::DEG2RAD);
			float angle_1 = Math::ATan2(mEndPos.z - mStartPos.z, mEndPos.x - mStartPos.x) * Math::RAD2DEG - 180.0f;
			float x_2 = -Math::Sin(angle_1 * Math::DEG2RAD) * mAmplitude * scaleValue * (1.0f - deltaScale * 0.25f);
			float z_2 = Math::Cos(angle_1 * Math::DEG2RAD) * mAmplitude *scaleValue * (1.0f - deltaScale * 0.25f);
			float x_3 = -Math::Sin(angle_1 * Math::DEG2RAD) * mAmplitude * (1.0f - deltaScale * 0.5f);
			float z_3 = Math::Cos(angle_1 * Math::DEG2RAD) * mAmplitude * (1.0f - deltaScale * 0.5f);
			float x_4 = -Math::Sin(angle_1 * Math::DEG2RAD) * mAmplitude * scaleValue * (1.0f - deltaScale * 0.75f);
			float z_4 = Math::Cos(angle_1 * Math::DEG2RAD) * mAmplitude * scaleValue * (1.0f - deltaScale * 0.75f);

			point_1 = mStartPos + Vector3(x_1, 0.0f, z_1);
			point_2 = (0.75f * mStartPos + 0.25f * mEndPos) + Vector3(x_2, 0.0f, z_2);
			point_3 = (0.50f * mStartPos + 0.50f * mEndPos) + Vector3(x_3, 0.0f, z_3);
			point_4 = (0.25f * mStartPos + 0.75f * mEndPos) + Vector3(x_4, 0.0f, z_4);
			point_5 = mEndPos + Vector3(x_5, 0.0f, z_5);

			pSpline->addPoint(mStartPos);
			pSpline->addPoint(point_1);
			pSpline->addPoint(point_2);
			pSpline->addPoint(point_3);
			pSpline->addPoint(point_4);
			pSpline->addPoint(point_5);
			pSpline->addPoint(mEndPos);
		}

		for (int i = 0; i <= nCount; i++)
		{
			mSplineIntPos[i] = pSpline->interpolate(float(i) / nCount);
		}

		mSplineIntDir = (mSplineIntPos[1] - mSplineIntPos[0]).normalizedCopy();
		LordDelete pSpline;
	}
	
	void EffectLayerRope::_updateRenderQueue()
	{
		if ((!mbAffected) || (!mbVisible))
			return;

		ui32 curSegment = Math::Max((ui32)(mSegment * mFlyRadio), (ui32)2);

		EffectRenderable* pRenderable = EffectSystemManager::Instance()->getRenderable(mMaterial->getTexturePtr(), mRenderGroup, mMaterial->getBlendMode(), mMaterial, (curSegment + 1) * 2, curSegment * 6, mViewSpace);
		if (!pRenderable)
			return;

		pRenderable->setSpecialRender(mParentSystem->IsSpecialRender());

		EffectVertexFmt* pVertexPtr = NULL;
		ui16* pIdxLockPtr = NULL;
		ui16 vertBegin;
		if (pRenderable->getLockPtr((curSegment + 1) * 2, curSegment * 6, vertBegin, pVertexPtr, pIdxLockPtr))
		{
			Matrix4 matTran = mParentSystem->getEffectSceneNode()->getWorldMatrix();
			Real nodeScale = mParentSystem->getEffectSceneNode()->getWorldScaling().x;

			Vector3 vStart, vEnd, vDir, vUp;
			Vector3 vViewUp, vLastViewUp, vLast(0.f, 0.f, 0.f);
			Vector3 vCamPos = mParentSystem->getCurrentCamera()->getPosition();

			float fWidthStart = mWidthStart * scale;
			float fWidthEnd = mWidthEnd * scale;

			Color lightningColor = color;

			Vector3 vNormal;

			if (mUseNormal)
			{
				vNormal = mParentSystem->getEffectSceneNode()->getWorldOrientation() * mNormal;
			}

			int nRenderCount = curSegment + 1; //mVertPairNum;
			float fInvRenderCountMinus1 = 1.0f / float(nRenderCount - 1);
			float tl = mMaterial->getTexAddrLeft();
			float tr = mMaterial->getTexAddrRight();
			float tt = mMaterial->getTexAddrTop() + mCurDeltaUV;
			float tb = mMaterial->getTexAddrButton() + mCurDeltaUV;
			bool swapUV = mMaterial->getSwapUV();

			for (int i = 0; i < nRenderCount; i++)
			{
				int nBase = i * 2;
				float fRatio = i * fInvRenderCountMinus1;
				Vector3 vVert[2];
				float fAlpha = 1.0f;
				float tv = i*fInvRenderCountMinus1;

				calcSplineVertex(
					i,
					nRenderCount,
					nBase,
					fRatio,
					fWidthStart,
					fWidthEnd,
					fAlpha,
					matTran,
					vNormal,
					vCamPos,
					vLast,
					vLastViewUp,
					vVert[0],
					vVert[1],
					nodeScale);
				
				ui32 dcolor = (lightningColor*fAlpha).getABGR();
				pVertexPtr[0].pos = vVert[0];
				pVertexPtr[0].color = dcolor;
				pVertexPtr[1].pos = vVert[1];
				pVertexPtr[1].color = dcolor;
				if (swapUV)
				{
					tv = tl + tv*(tr - tl);
					pVertexPtr[0].tx = tv;
					pVertexPtr[0].ty = tb;
					pVertexPtr[1].tx = tv;
					pVertexPtr[1].ty = tt;
				}
				else
				{
					tv = tt + tv*(tb - tt);
					pVertexPtr[0].tx = tl;
					pVertexPtr[0].ty = tv;
					pVertexPtr[1].tx = tr;
					pVertexPtr[1].ty = tv;
				}
				
				pVertexPtr += 2;
			}

			for (i32 i = 0; i < mVertPairNum - 1; ++i)
			{
				*pIdxLockPtr++ = vertBegin + 2;
				*pIdxLockPtr++ = vertBegin;
				*pIdxLockPtr++ = vertBegin + 1;

				*pIdxLockPtr++ = vertBegin + 3;
				*pIdxLockPtr++ = vertBegin + 2;
				*pIdxLockPtr++ = vertBegin + 1;

				vertBegin += 2;
			}
		}
	}


	void EffectLayerRope::calcSplineVertex(const int i,
		const int nRenderCount,
		const int nBase,
		float fRatio,
		const float fWidthStart,
		const float fWidthEnd,
		float& fAlpha,
		const Matrix4& matTran,
		const Vector3& vNormal,
		const Vector3& vCamPos,
		Vector3& vLast,
		Vector3& vLastViewUp,
		Vector3& vOutVert1,
		Vector3& vOutVert2,
		Real nodeScale)
	{
		if (i == 0)
		{
			Vector3 vIntStart(mSplineIntPos[0] * matTran);
			Vector3 vDir = oritation * mSplineIntDir;
			vLast = vIntStart;

			Vector3 vViewUp = vDir.cross(mUseNormal ? vNormal : vIntStart - vCamPos);
			vViewUp.normalize();
			vLastViewUp = vViewUp;
			vViewUp *= fWidthStart;

			vOutVert1 = vIntStart - vViewUp*nodeScale;
			vOutVert2 = vIntStart + vViewUp*nodeScale;
			fAlpha = mAlphaStart;
		}
		else if (i + 1 == nRenderCount)
		{
			Vector3 vIntEnd(mSplineIntPos[i] * matTran);
			Vector3 vDir(vIntEnd - vLast);
			Vector3 vViewUp = vDir.cross(mUseNormal ? vNormal : vIntEnd - vCamPos);
			vViewUp.normalize();

			if (vViewUp.dot(vLastViewUp) < 0)
				vViewUp *= -1;

			vViewUp *= fWidthEnd;
			vOutVert1 = vIntEnd - vViewUp*nodeScale;
			vOutVert2 = vIntEnd + vViewUp*nodeScale;
			fAlpha = mAlphaEnd;
		}
		else
		{
			Vector3 vPos(mSplineIntPos[i] * matTran);
			Vector3 vDir(vPos - vLast);
			vLast = vPos;

			Vector3 vViewUp = vDir.cross(mUseNormal ? vNormal : vPos - vCamPos);
			vViewUp.normalize();

			if (vViewUp.dot(vLastViewUp) < 0)
				vViewUp *= -1;
			vLastViewUp = vViewUp;

			vViewUp *= fWidthStart * (1.f - fRatio) + fWidthEnd * fRatio;
			if (fRatio < .5f)
			{
				fAlpha = mAlphaStart * (1.f - fRatio) + mAlphaMid * fRatio;
			}
			else
			{
				fAlpha = mAlphaMid * (1.f - fRatio) + mAlphaEnd * fRatio;
			}

			vOutVert1 = vPos - vViewUp*nodeScale;
			vOutVert2 = vPos + vViewUp*nodeScale;
		}
	}

	void EffectLayerRope::importData(DataStream* pStream, int version)
	{
		EffectLayer::importData(pStream, version);
		pStream->read(&mStartPos, sizeof(Vector3));
		pStream->read(&mEndPos, sizeof(Vector3));
		pStream->read(&mSegment, sizeof(ui32));
		pStream->read(&mWidthStart, sizeof(Real));
		pStream->read(&mWidthEnd, sizeof(Real));
		pStream->read(&mAlphaStart, sizeof(Real));
		pStream->read(&mAlphaEnd, sizeof(Real));
		pStream->read(&mAlphaMid, sizeof(Real));
		pStream->read(&mUVscale, sizeof(Real));
		pStream->read(&mAmplitude, sizeof(Real));
		pStream->read(&mUseNormal, sizeof(bool));
		pStream->read(&mNormal, sizeof(Vector3));
		pStream->read(&mExtenalParam, sizeof(bool));
		pStream->read(&mFlyTime, sizeof(i32));
	}

	void EffectLayerRope::exportData(DataStream* pStream, int version)
	{
		EffectLayer::exportData(pStream, version);
		pStream->write(&mStartPos, sizeof(Vector3));
		pStream->write(&mEndPos, sizeof(Vector3));
		pStream->write(&mSegment, sizeof(ui32));
		pStream->write(&mWidthStart, sizeof(Real));
		pStream->write(&mWidthEnd, sizeof(Real));
		pStream->write(&mAlphaStart, sizeof(Real));
		pStream->write(&mAlphaEnd, sizeof(Real));
		pStream->write(&mAlphaMid, sizeof(Real));
		pStream->write(&mUVscale, sizeof(Real));
		pStream->write(&mAmplitude, sizeof(Real));
		pStream->write(&mUseNormal, sizeof(bool));
		pStream->write(&mNormal, sizeof(Vector3));
		pStream->write(&mExtenalParam, sizeof(bool));
		pStream->write(&mFlyTime, sizeof(i32));
	}

	void EffectLayerRope::mergeAligenBox(Box& aabb)
	{
		if (!mbAffected)
			return;

		Vector3 startNoise = Vector3(mWidthStart, mWidthStart, mWidthStart);
		Vector3 endNoise = Vector3(mWidthEnd, mWidthEnd, mWidthEnd);

		for (ui32 i = 0; i < mSegment + 1; ++i)
		{
			if (i == 0)
			{
				aabb.addPoint(mSplineIntPos[i] + startNoise);
				aabb.addPoint(mSplineIntPos[i] - startNoise);
			}
			else if (i == mSegment)
			{
				aabb.addPoint(mSplineIntPos[i] + endNoise);
				aabb.addPoint(mSplineIntPos[i] - endNoise);
			}
			else
			{
				aabb.addPoint(mSplineIntPos[i]);
			}
		}
	}

	void EffectLayerRope::copyAttributesTo(EffectParticle* layer)
	{
		EffectLayer::copyAttributesTo(layer);
		EffectLayerRope* lightning = static_cast<EffectLayerRope*>(layer);
		lightning->mStartPos = mStartPos;
		lightning->mEndPos = mEndPos;
		lightning->mSegment = mSegment;
		lightning->mWidthStart = mWidthStart;
		lightning->mWidthEnd = mWidthEnd;
		lightning->mAlphaStart = mAlphaStart;
		lightning->mAlphaEnd = mAlphaEnd;
		lightning->mAlphaMid = mAlphaMid;
		lightning->mUseNormal = mUseNormal;
		lightning->mNormal = mNormal;
		lightning->mExtenalParam = mExtenalParam;
		lightning->mAmplitude = mAmplitude;
		lightning->mUVscale = mUVscale;
		lightning->mFlyTime = mFlyTime;
	}

	void EffectLayerRope::_notifyStart()
	{
		EffectLayer::_notifyStart();
		mEscapTime = 0;
		mCurDeltaUV = 0;
	}

	void EffectLayerRope::_notifyExtenalParam(const Vector3& p1, const Vector3& p2)
	{
		if (mExtenalParam)
		{
			mStartPos = p1;
			mEndPos = p2;
		}
	}

	void EffectLayerRope::_notifySpecialParam(float p1, float p2)
	{
		mYaw_1 = p1;
		mYaw_2 = p2;
	}

	void EffectLayerRope::getPropertyList(PropertyList& list)
	{
		EffectLayer::getPropertyList(list);

		list.push_back(token[TOKEN_LY_RP_BEGINPOS]);
		list.push_back(token[TOKEN_LY_RP_ENDPOS]);
		list.push_back(token[TOKEN_LY_RP_SEGMENT]);
		list.push_back(token[TOKEN_LY_RP_STARTWIDTH]);
		list.push_back(token[TOKEN_LY_RP_ENDWIDTH]);
		list.push_back(token[TOKEN_LY_RP_STARTALPHA]);
		list.push_back(token[TOKEN_LY_RP_ENDALPHA]);
		list.push_back(token[TOKEN_LY_RP_MIDDERALPHA]);
		list.push_back(token[TOKEN_LY_LN_USENORMAL]);
		list.push_back(token[TOKEN_LY_LN_NORMAL]);
		list.push_back(token[TOKEN_LY_RP_CURVEDLEN]);
		list.push_back(token[TOKEN_LY_LN_EXTENALPARAM]);
		list.push_back(token[TOKEN_LY_RP_FLYTIME]);
		list.push_back(token[TOKEN_LY_RP_UVSCALE]);
	}

	bool EffectLayerRope::getPropertyType(const String& name, PropertyType& type)
	{
		if (EffectLayer::getPropertyType(name, type))
			return true;
		if (name == token[TOKEN_LY_RP_BEGINPOS])
		{
			type = IElement::PT_VECTOR3; return true;
		}
		else if (name == token[TOKEN_LY_RP_ENDPOS])
		{
			type = IElement::PT_VECTOR3; return true;
		}
		else if (name == token[TOKEN_LY_RP_SEGMENT])
		{
			type = IElement::PT_INT; return true;
		}
		else if (name == token[TOKEN_LY_RP_STARTWIDTH])
		{
			type = IElement::PT_REAL; return true;
		}
		else if (name == token[TOKEN_LY_RP_ENDWIDTH])
		{
			type = IElement::PT_REAL; return true;
		}
		else if (name == token[TOKEN_LY_RP_STARTALPHA])
		{
			type = IElement::PT_REAL; return true;
		}
		else if (name == token[TOKEN_LY_RP_ENDALPHA])
		{
			type = IElement::PT_REAL; return true;
		}
		else if (name == token[TOKEN_LY_RP_MIDDERALPHA])
		{
			type = IElement::PT_REAL; return true;
		}
		else if (name == token[TOKEN_LY_LN_USENORMAL])
		{
			type = IElement::PT_BOOL; return true;
		}
		else if (name == token[TOKEN_LY_LN_NORMAL])
		{
			type = IElement::PT_VECTOR3; return true;
		}
		else if (name == token[TOKEN_LY_RP_CURVEDLEN])
		{
			type = IElement::PT_REAL; return true;
		}
		else if (name == token[TOKEN_LY_LN_EXTENALPARAM])
		{
			type = IElement::PT_BOOL; return true;
		}
		else if (name == token[TOKEN_LY_RP_FLYTIME])
		{
			type = IElement::PT_INT; return true;
		}
		else if (name == token[TOKEN_LY_RP_UVSCALE])
		{
			type = IElement::PT_REAL; return true;
		}
		return false;
	}

	bool EffectLayerRope::getPropertyValue(const String& name, String&value)
	{
		if (EffectLayer::getPropertyValue(name, value))
			return true;

		if (name == token[TOKEN_LY_RP_BEGINPOS])
		{
			value = StringUtil::ToString(mStartPos); return true;
		}
		else if (name == token[TOKEN_LY_RP_ENDPOS])
		{
			value = StringUtil::ToString(mEndPos); return true;
		}
		else if (name == token[TOKEN_LY_RP_SEGMENT])
		{
			value = StringUtil::ToString(mSegment); return true;
		}
		else if (name == token[TOKEN_LY_RP_STARTWIDTH])
		{
			value = StringUtil::ToString(mWidthStart); return true;
		}
		else if (name == token[TOKEN_LY_RP_ENDWIDTH])
		{
			value = StringUtil::ToString(mWidthEnd); return true;
		}
		else if (name == token[TOKEN_LY_RP_STARTALPHA])
		{
			value = StringUtil::ToString(mAlphaStart); return true;
		}
		else if (name == token[TOKEN_LY_RP_ENDALPHA])
		{
			value = StringUtil::ToString(mAlphaEnd); return true;
		}
		else if (name == token[TOKEN_LY_RP_MIDDERALPHA])
		{
			value = StringUtil::ToString(mAlphaMid); return true;
		}
		else if (name == token[TOKEN_LY_LN_USENORMAL])
		{
			value = StringUtil::ToString(mUseNormal); return true;
		}
		else if (name == token[TOKEN_LY_LN_NORMAL])
		{
			value = StringUtil::ToString(mNormal); return true;
		}
		else if (name == token[TOKEN_LY_RP_CURVEDLEN])
		{
			value = StringUtil::ToString(mAmplitude); return true;
		}
		else if (name == token[TOKEN_LY_LN_EXTENALPARAM])
		{
			value = StringUtil::ToString(mExtenalParam); return true;
		}
		else if (name == token[TOKEN_LY_RP_FLYTIME])
		{
			value = StringUtil::ToString(mFlyTime); return true;
		}
		else if (name == token[TOKEN_LY_RP_UVSCALE])
		{
			value = StringUtil::ToString(mUVscale); return true;
		}
		return false;
	}

	bool EffectLayerRope::setPropertyValue(const String& name, const String& value)
	{
		if (EffectLayer::setPropertyValue(name, value))
			return true;

		if (name == token[TOKEN_LY_RP_BEGINPOS])
		{
			mStartPos = StringUtil::ParseVec3(value); return true;
		}
		else if (name == token[TOKEN_LY_RP_ENDPOS])
		{
			mEndPos = StringUtil::ParseVec3(value); return true;
		}
		else if (name == token[TOKEN_LY_RP_SEGMENT])
		{
			mSegment = StringUtil::ParseI32(value);
			if (mSegment < 2) mSegment = 2;
			return true;
		}
		else if (name == token[TOKEN_LY_RP_STARTWIDTH])
		{
			mWidthStart = StringUtil::ParseFloat(value); return true;
		}
		else if (name == token[TOKEN_LY_RP_ENDWIDTH])
		{
			mWidthEnd = StringUtil::ParseFloat(value); return true;
		}
		else if (name == token[TOKEN_LY_RP_STARTALPHA])
		{
			mAlphaStart = StringUtil::ParseFloat(value); return true;
		}
		else if (name == token[TOKEN_LY_RP_ENDALPHA])
		{
			mAlphaEnd = StringUtil::ParseFloat(value); return true;
		}
		else if (name == token[TOKEN_LY_RP_MIDDERALPHA])
		{
			mAlphaMid = StringUtil::ParseFloat(value); return true;
		}
		else if (name == token[TOKEN_LY_LN_USENORMAL])
		{
			mUseNormal = StringUtil::ParseBool(value); return true;
		}
		else if (name == token[TOKEN_LY_LN_NORMAL])
		{
			mNormal = StringUtil::ParseVec3(value); return true;
		}
		else if (name == token[TOKEN_LY_RP_CURVEDLEN])
		{
			mAmplitude = StringUtil::ParseFloat(value); return true;
		}
		else if (name == token[TOKEN_LY_LN_EXTENALPARAM])
		{
			mExtenalParam = StringUtil::ParseBool(value); return true;
		}
		else if (name == token[TOKEN_LY_RP_FLYTIME])
		{
			mFlyTime = StringUtil::ParseI32(value); return true;
		}
		else if (name == token[TOKEN_LY_RP_UVSCALE])
		{
			mUVscale = StringUtil::ParseReal(value); return true;
		}

		return false;
	}
}