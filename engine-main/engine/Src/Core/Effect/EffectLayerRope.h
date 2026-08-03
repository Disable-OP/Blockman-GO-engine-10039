/********************************************************************
filename: 	EffectLayerRope.h
file path:	dev\engine\Src\Core\Effect\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_EFFECT_LAYER_ROPE_H__
#define __LORD_EFFECT_LAYER_ROPE_H__

#include "EffectLayer.h"

namespace LORD
{

	class LORD_CORE_API EffectLayerRope : public EffectLayer
	{
	public:
		enum FilterType
		{
			FT_TRIANGLE = 0,
			FT_RECTANGLE,
			FT_SIN,
			FT_TRAPEZIA,
		};

	public:
		virtual void getPropertyList(PropertyList& list);
		virtual bool getPropertyType(const String& name, PropertyType& type);
		virtual bool getPropertyValue(const String& name, String& value);
		virtual bool setPropertyValue(const String& name, const String& value);

	public:
		EffectLayerRope();
		virtual ~EffectLayerRope();

		virtual int _update(i32 time);
		virtual void _updateRenderQueue();

		virtual void prepare();
		virtual void unprepare();

		virtual void _notifyStart();
		virtual void _notifyExtenalParam(const Vector3& p1, const Vector3& p2);
		virtual void _notifySpecialParam(float p1, float p2);

		virtual void importData(DataStream* pStream, int version);
		virtual void exportData(DataStream* pStream, int version);
		virtual void mergeAligenBox(Box& aabb);

		virtual void copyAttributesTo(EffectParticle* particle);

	protected:
		Vector3 calcVertVec(const Vector3& vDir)
		{
			Vector3 v = fabs(vDir.y) > .99f ? vDir.cross(Vector3::UNIT_X)
				: Vector3::UNIT_Y.cross(vDir);
			v.normalize();
			return v;
		}
		void updatePath();
		void calcSplineVertex(const int i,
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
			Real nodeScale);

	protected:
		Vector3	mStartPos;
		Vector3	mEndPos;
		Vector3	mNormal;
		Vector3	mSplineIntDir;
		Real	mWidthStart;
		Real	mWidthEnd;
		Real	mAlphaStart;
		Real	mAlphaEnd;
		Real	mAlphaMid;
		Real	mAmplitude;
		Real    mUVscale;
		Real    mCurDeltaUV;
		Real	mFlyRadio;
		ui32	mSegment;
		i32		mFlyTime;
		i32     mEscapTime;
		i32		mVertPairNum;
		bool	mUseNormal;
		bool	mExtenalParam;

		bool	mIsWiggly = false;
		Real	mYaw_1 = 0.0f;
		Real	mYaw_2 = 0.0f;

		Vector3*	mSplineIntPos;
	};

}

#endif