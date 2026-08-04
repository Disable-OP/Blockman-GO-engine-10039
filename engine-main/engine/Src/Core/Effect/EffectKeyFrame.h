/********************************************************************
filename: 	EffectKeyFrame.h
file path:	dev\engine\Src\Core\Effect\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_EFFECT_KEYFRAME_H__
#define __LORD_EFFECT_KEYFRAME_H__

#include "EffectIElement.h"

namespace LORD
{
	class SimpleSpline;
	class LORD_CORE_API EffectKeyFrame : public IElement
	{
	public:
		virtual void getPropertyList(PropertyList& list);
		virtual bool getPropertyType(const String& name, PropertyType& type);
		virtual bool getPropertyValue(const String& name, String& value);
		virtual bool setPropertyValue(const String& name, const String& value);

	public:
		enum INTERPOLATION_TYPE
		{
			IT_CONTROLLER, 
			IT_SPLINE,
			IT_SLERP,
			IT_UNKNOW,
		};

		typedef vector<EffectController*>::type EffectControllerList;
		typedef EffectControllerList::iterator EffectControllerItor;

		EffectKeyFrame();
		~EffectKeyFrame();

		EffectController* createController(EFFECT_CONTROL_TYPE controlType);
		void addController(EffectController* controller);
		void removeController(EffectController* controller);
		EffectController* getController(size_t index) const;
		size_t getNumControllers() const;
		void destroyController(size_t index);
		void destroyController(EffectController* controller);
		void destroyAllControllers();

		void mirrorXy();
		void mirrorYz();
		void mirrorXz();

		void reCaculateOritation();
		void reCaculateAngle();

		i32 _update_sys(i32 time);

		void _notifyStart(EffectKeyFrame* nextFreame);
		void _notifyContinue(EffectKeyFrame* nextFrame);

		String getName() const { return "KeyFrame"; }
		void copyAttributesTo(EffectKeyFrame* keyframe);
		void setParentLayer(EffectLayer* parent) { mParentLayer = parent; }

		virtual void importData(DataStream* pStream, int version);
		virtual void exportData(DataStream* pStream, int version);

		/// time property
		void setBeginTime(i32 time) { mBeginTime = time; }
		void setAffectTime(i32 time) { mAffectTime = time; }
		i32 getBeginTime() const { return mBeginTime; }
		i32 getAffectTime() const { return mAffectTime; }
		i32 getCurrentTime() const { return mCurrentTime; }

		//// particle property
		void setPosition(const Vector3& pos) { mPosition = pos; }
		Vector3 getPosition() const { return mPosition; }
		void setOritation(const Quaternion& ori) { mOritation = ori; }
		Quaternion getOritation() const { return mOritation; }
		void setSelfRotate(float degree) { mSelfRotate = degree; }
		float getSelfRotate() const { return mSelfRotate; }
		void setColor(const Color& color) { mColor = color; }
		Color getColor() const { return mColor; }
		void setScale(float scale) { mScale = scale; }
		float getScale() const { return mScale; }

		//// spline property
		void createSpline();
		SimpleSpline* getSplinePtr() const;
		void setSpline(const String& str);
		String getSpline() const;
		void destroySpline();

		bool getAutoOritationForSpline() const { return mAutoOritationForSpline;}
		void setAutoOritationForSpline(bool param) { mAutoOritationForSpline = param; }

		void setInterpolationType(INTERPOLATION_TYPE type) { mInterpolationType = type; }
		String getInterpolationTypeString() const;
		INTERPOLATION_TYPE getInterpolationType() const { return mInterpolationType; }

		void changeSpline(vector<Vector3>::type& points);

	protected:
		Vector3				mPosition;
		Quaternion				mOritation;
		float				mSelfRotate;
		Color				mColor;
		float				mScale;
		i32					mBeginTime;
		i32					mAffectTime;
		INTERPOLATION_TYPE	mInterpolationType;
		bool				mAutoOritationForSpline;
		SimpleSpline*		mSpline;
		Vector3				mSplineOldPos;

		float				mPitch;
		float				mYaw;
		float				mRoll;

		EffectLayer*		mParentLayer;	
		EffectKeyFrame*		mNextKeyFrame;
		EffectControllerList mControllers;
		i32					mCurrentTime;
	}; 
}

#endif