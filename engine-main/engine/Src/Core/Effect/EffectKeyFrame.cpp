#include "Core.h"
#include "EffectSystemManager.h"
#include "EffectKeyFrame.h"
#include "EffectLayer.h" 
#include "Geom/SimpleSpline.h"
#include "Resource/DataStream.h"

namespace LORD
{
	EffectKeyFrame::EffectKeyFrame()
		:mPosition(0, 0, 0)
		,mOritation(1, 0, 0, 0)
		,mSelfRotate(0)
		,mColor(1, 1, 1, 1)
		,mScale(1)
		,mBeginTime(0)
		,mAffectTime(2000)
		,mInterpolationType(IT_CONTROLLER)
		,mAutoOritationForSpline(false)
		,mSpline(NULL)
		,mSplineOldPos(Vector3::ZERO)
		,mPitch(0)
		,mYaw(0)
		,mRoll(0)
		,mParentLayer(0)
		,mNextKeyFrame(NULL)
		,mCurrentTime(0)
	{

	}

	EffectKeyFrame::~EffectKeyFrame()
	{
		destroyAllControllers();

		LordSafeDelete(mSpline);
	}

	void EffectKeyFrame::_notifyStart(EffectKeyFrame* nextFrame)
	{
		mCurrentTime = 0;

		mParentLayer->originalPosition = mParentLayer->position = mPosition;
		mParentLayer->originalScale = mParentLayer->scale = 1.0f;
		mParentLayer->originalColor = mParentLayer->color = mColor;
		mParentLayer->originalScale = mParentLayer->scale = mScale;
		mParentLayer->originalOritation = mParentLayer->oritation = mOritation;
		mParentLayer->direction = mOritation * mParentLayer->originalDirection; 
		mParentLayer->originalSelfRotation = mParentLayer->selfRotation = mSelfRotate;

		mParentLayer->mTime = 0;
		if(mSpline && mInterpolationType == IT_SPLINE)
		{
			mSplineOldPos = mSpline->getPoint(0);
			Vector3 prePos = mSpline->interpolate(0.01f);
			prePos -= mSplineOldPos;
			prePos.normalize();
			mParentLayer->originalDirection = mParentLayer->direction = prePos;
		}

		for(size_t i=0; i<mControllers.size(); ++i)
		{
			mControllers[i]->_notifyStart(mParentLayer);
		}

		mNextKeyFrame = nextFrame;
	}

	void EffectKeyFrame::_notifyContinue(EffectKeyFrame* nextFrame)
	{
		mCurrentTime = 0;

		mParentLayer->_notifyKeyframeEnd();

		if(mInterpolationType == IT_CONTROLLER)
		{
			mParentLayer->forwardParticleState();
			mOritation = mParentLayer->oritation;
			mColor = mParentLayer->color;
			mPosition = mParentLayer->position;
			mScale = mParentLayer->scale;
			mSelfRotate = mParentLayer->selfRotation;
			for(size_t i=0; i<mControllers.size(); ++i)
			{
				mControllers[i]->_notifyStart(mParentLayer);
			}
		}

		mNextKeyFrame = nextFrame;
	}

	i32 EffectKeyFrame::_update_sys(i32 time)
	{
		mCurrentTime += time;
		i32 remainedTime = 0;
		if(mAffectTime>0 && mCurrentTime>mAffectTime)
		{
			remainedTime = mCurrentTime - mAffectTime;
			mCurrentTime = mAffectTime;
		}

		mParentLayer->reParticleState();

		if( mInterpolationType == IT_CONTROLLER )
		{
			for(size_t i =0; i<mControllers.size(); ++i)
			{
				if(mParentLayer->getFallowLayer() != NULL)
				{
					if(!mControllers[i]->isControllerMovement())
						mControllers[i]->controll(mCurrentTime, mParentLayer);
				}
				else
					mControllers[i]->controll(mCurrentTime, mParentLayer);
			}

			if(mParentLayer->getFallowLayer() != NULL)
				mParentLayer->fallowMovement();
		}
		else if( mInterpolationType == IT_SPLINE )
		{
			for(size_t i =0; i<mControllers.size(); ++i)
			{
				if(!mControllers[i]->isControllerMovement())
					mControllers[i]->controll(mCurrentTime, mParentLayer);
			}
			if(mSpline)
			{
				float interpolateRate = float(mCurrentTime)/float(mAffectTime);
				if(interpolateRate < 0.0f)
					interpolateRate = 0.0f;

				Vector3 pos = mSpline->interpolate(interpolateRate);
				mParentLayer->position = pos;
				Vector3 dir = (pos - mSplineOldPos).normalizedCopy();
				mParentLayer->direction = dir;
				mSplineOldPos = pos;

				if(mAutoOritationForSpline)
				{
					Vector3 dirXZ = Vector3(dir.x, 0 ,dir.z);

					Quaternion ori,ori2,oriInverse;
					ori = Vector3::UNIT_Z.getRotationTo(dirXZ);

					oriInverse = ori;
					oriInverse.inverse();
					dirXZ = oriInverse*dirXZ;
					dir = oriInverse*dir;
					ori2 = Vector3::UNIT_Z.getRotationTo(dir);

					mParentLayer->oritation = ori*ori2;
				}
			}
		}
		else
		{
			for(size_t i =0; i<mControllers.size(); ++i)
			{
				if(!mControllers[i]->isControllerMovement())
					mControllers[i]->controll(mCurrentTime, mParentLayer);
			}

			if(mNextKeyFrame)
			{
				Vector3 pos1 = mPosition;
				float rot = mSelfRotate;
				Quaternion ori1 = mOritation;
				Color color1 = mColor;
				float sc1 = mScale;
				Vector3 pos2 = mNextKeyFrame->getPosition();
				Quaternion ori2 = mNextKeyFrame->getOritation();
				float rot2 = mNextKeyFrame->getSelfRotate();
				Color color2 = mNextKeyFrame->getColor();
				float sc2 = mNextKeyFrame->getScale();

				float interpolateRate = float(mCurrentTime)/float(mAffectTime);
				mParentLayer->position = pos1 + (pos2 - pos1) * interpolateRate;
				Quaternion::Slerp(mParentLayer->oritation, ori1, ori2, interpolateRate);
				mParentLayer->selfRotation = rot + (rot2-rot)*interpolateRate;
				mParentLayer->color = color1 + (color2-color1)*interpolateRate;
				mParentLayer->scale = sc1 + (sc2 - sc1)*interpolateRate;
			}
		}

		return remainedTime;
	}

	String EffectKeyFrame::getInterpolationTypeString() const
	{
		String typeString = "Controller";
		if(mInterpolationType == IT_SPLINE)
			typeString = "Spline";
		else if(mInterpolationType == IT_SLERP)
			typeString = "Slerp";

		return typeString;
	}

	void EffectKeyFrame::changeSpline(vector<Vector3>::type& points)
	{
		if (points.size() != mSpline->getNumPoints())
		{
			return;
		}
		for (int i = 0; i < points.size(); i++)
		{
			mSpline->updatePoint(i, points[i]);
		}
	}

	void EffectKeyFrame::copyAttributesTo(EffectKeyFrame* keyFrame)
	{
		keyFrame->destroyAllControllers();

		keyFrame->mPosition = mPosition;
		keyFrame->mSelfRotate = mSelfRotate;
		keyFrame->mScale = mScale;
		keyFrame->mOritation = mOritation;
		keyFrame->mPitch = mPitch;
		keyFrame->mYaw = mYaw;
		keyFrame->mRoll = mRoll;
		keyFrame->mColor = mColor;
		keyFrame->mBeginTime = mBeginTime;
		keyFrame->mAffectTime = mAffectTime;
		keyFrame->mInterpolationType = mInterpolationType;
		keyFrame->mAutoOritationForSpline = mAutoOritationForSpline;

		for(size_t i=0; i<mControllers.size(); ++i)
		{
			EffectController* cloneController = EffectSystemManager::Instance()->cloneController(getController(i));
			keyFrame->addController(cloneController);
		}

		if(mSpline != NULL)
		{
			keyFrame->mSpline = LordNew SimpleSpline;
			for(ui32 i=0; i<mSpline->getNumPoints(); ++i)
			{
				keyFrame->mSpline->addPoint(mSpline->getPoint(i));
			}
		}
	}

	EffectController* EffectKeyFrame::createController(EFFECT_CONTROL_TYPE controlType)
	{
		EffectController* controller = EffectSystemManager::Instance()->createController(controlType);
		addController(controller);
		return controller;
	}

	void EffectKeyFrame::addController(EffectController* controller)
	{
		assert(controller && "Controller is null!");
		mControllers.push_back(controller);
		EffectController* swapTemp;
		for(size_t i=mControllers.size()-1; i>0; --i)
		{
			if(mControllers[i]->getPriority() < mControllers[i-1]->getPriority())
			{
				swapTemp = mControllers[i];
				mControllers[i] = mControllers[i-1];
				mControllers[i-1] = swapTemp;
			}
			else
			{
				break;
			}
		}
	}

	void EffectKeyFrame::removeController(EffectController* controller)
	{
		LordAssert(controller && "Controller is null!");
		EffectControllerItor it;
		EffectControllerItor itEnd = mControllers.end();
		for (it = mControllers.begin(); it != itEnd; ++it)
		{
			if (*it == controller)
			{
				// Remove it
				mControllers.erase(it);
				break;
			}
		}
	}

	EffectController* EffectKeyFrame::getController(size_t index) const
	{
		assert(index < mControllers.size() && "Controller index out of bounds!");
		return mControllers[index];
	}

	size_t EffectKeyFrame::getNumControllers() const
	{
		return mControllers.size();
	}

	void EffectKeyFrame::destroyController(size_t index)
	{
		destroyController(getController(index));
	}

	void EffectKeyFrame::destroyController(EffectController* controller)
	{
		LordAssert(controller && "Controller is null!");
		EffectControllerItor it;
		EffectControllerItor itEnd = mControllers.end();
		for (it = mControllers.begin(); it != itEnd; ++it)
		{
			if (*it == controller)
			{
				EffectSystemManager::Instance()->destroyController(*it);
				mControllers.erase(it);
				break;
			}
		}
	}

	void EffectKeyFrame::destroyAllControllers()
	{
		EffectControllerItor it;
		EffectControllerItor itEnd = mControllers.end();
		for (it = mControllers.begin(); it != itEnd; ++it)
		{
			EffectSystemManager::Instance()->destroyController(*it);
		}
		mControllers.clear();
	}

	void EffectKeyFrame::mirrorXy()
	{
		mPosition.z = -mPosition.z;
	}

	void EffectKeyFrame::mirrorYz()
	{
		mPosition.x = -mPosition.x;
	}

	void EffectKeyFrame::mirrorXz()
	{
		mPosition.y = -mPosition.y;
	}

	void EffectKeyFrame::reCaculateOritation()
	{
		mOritation.fromEulerAngle(mPitch, mYaw, mRoll);
	}

	void EffectKeyFrame::reCaculateAngle()
	{
		mOritation.toEulerAngle(mPitch, mYaw, mRoll);
	}


	void EffectKeyFrame::createSpline()
	{
		assert(mSpline == NULL);
		mSpline = LordNew SimpleSpline;
	}

	SimpleSpline* EffectKeyFrame::getSplinePtr() const
	{
		return mSpline;
	}

	void EffectKeyFrame::destroySpline()
	{
		LordSafeDelete(mSpline);
	}

	void EffectKeyFrame::setSpline(const String& str)
	{
		String trackPoints;
		trackPoints = StringUtil::Replace(str, "_", " ");
		StringArray points = StringUtil::Split(trackPoints, "|");

		if (points.size())
		{
			LordSafeDelete(mSpline);
			mSpline = LordNew SimpleSpline;

			for (int indexPoint = 0; indexPoint < (int)points.size(); indexPoint++)
			{
				mSpline->addPoint(StringUtil::ParseVec3(points[indexPoint]));
			}
		}
	}

	String EffectKeyFrame::getSpline() const
	{
		if(mSpline == NULL)
			return StringUtil::BLANK;

		String result = StringUtil::BLANK;

		for (int index = 0; index < mSpline->getNumPoints(); index++)
		{
			String strPoint = StringUtil::BLANK;
			if(index != 0)
				strPoint = "|";

			Vector3 point = mSpline->getPoint(index);

			strPoint += 
				StringUtil::ToString(point.x) + "_" +
				StringUtil::ToString(point.y) + "_" +
				StringUtil::ToString(point.z);

			result += strPoint;
		}

		return result;
	}

	void EffectKeyFrame::exportData(DataStream* pStream, int version)
	{
		/// base property
		pStream->write(&mPosition, sizeof(Vector3));
		pStream->write(&mSelfRotate, sizeof(float));
		pStream->write(&mScale, sizeof(float));
		pStream->write(&mOritation, sizeof(Quaternion));
		pStream->write(&mColor, sizeof(Color));
		pStream->write(&mBeginTime, sizeof(i32));
		pStream->write(&mAffectTime, sizeof(i32));
		pStream->write(&mInterpolationType, sizeof(INTERPOLATION_TYPE));
		pStream->write(&mAutoOritationForSpline, sizeof(bool));

		// spline
		ui32 splinePointNum = 0;
		if(mSpline)
		{
			splinePointNum = mSpline->getNumPoints();
			pStream->write(&splinePointNum, sizeof(ui32));
			for(ui32 i=0; i<splinePointNum; ++i)
			{
				Vector3 splinePoint = mSpline->getPoint(i);
				pStream->write(&splinePoint, sizeof(Vector3));
			}
		}
		else
		{
			pStream->write(&splinePointNum, sizeof(ui32));
		}

		// controllers
		ui32 controllerNum = (ui32)mControllers.size();
		pStream->write(&controllerNum, sizeof(ui32));
		EFFECT_CONTROL_TYPE type;
		EffectController* pController = NULL;
		for(size_t j=0; j<controllerNum; ++j)
		{
			pController = mControllers[j];
			type = pController->getControllerType();
			pStream->write(&type, sizeof(EFFECT_CONTROL_TYPE));
			pController->exportData(pStream, version);
		}
	}

	void EffectKeyFrame::importData(DataStream* pStream, int version)
	{
		/// base property
		pStream->read(&mPosition, sizeof(Vector3));
		pStream->read(&mSelfRotate, sizeof(float));
		pStream->read(&mScale, sizeof(float));
		pStream->read(&mOritation, sizeof(Quaternion));
		pStream->read(&mColor, sizeof(Color));
		pStream->read(&mBeginTime, sizeof(i32));
		pStream->read(&mAffectTime, sizeof(i32));
		pStream->read(&mInterpolationType, sizeof(INTERPOLATION_TYPE));
		pStream->read(&mAutoOritationForSpline, sizeof(bool));

		// spline
		LordAssertX(mSpline==NULL, "Error: EffectKeyFrame::importData error occurse in spline.");
		ui32 splinePointNum = 0;
		pStream->read(&splinePointNum, sizeof(ui32));
		if(splinePointNum > 0)
		{
			createSpline();
			Vector3 splinePoint;
			for(size_t i=0; i<splinePointNum; ++i)
			{
				pStream->read(&splinePoint, sizeof(Vector3));
				mSpline->addPoint(splinePoint);
			}
		}

		// controllers
		LordAssertX(mControllers.empty(), "Error: EffectKeyFrame::importData error occurse in controllers.");
		ui32 controllerNum;
		pStream->read(&controllerNum, sizeof(ui32));
		EFFECT_CONTROL_TYPE type;
		for(size_t j=0; j<controllerNum; ++j)
		{
			pStream->read(&type, sizeof(EFFECT_CONTROL_TYPE));
			EffectController* pController = createController(type);
			pController->importData(pStream, version);
		}
	}

	void EffectKeyFrame::getPropertyList(PropertyList & list)
	{
		list.push_back(token[TOKEN_KF_POSITION]);
		list.push_back(token[TOKEN_KF_YAW]);
		list.push_back(token[TOKEN_KF_PITCH]);
		list.push_back(token[TOKEN_KF_ROLL]);
		list.push_back(token[TOKEN_KF_SELFROTATE]);
		list.push_back(token[TOKEN_KF_SCALE]);
		list.push_back(token[TOKEN_KF_COLOR]);
		list.push_back(token[TOKEN_KF_AFFECTTIME]);
		list.push_back(token[TOKEN_KF_INTERPOLATION]);
	}

	bool EffectKeyFrame::getPropertyType(const String & name, PropertyType &type)
	{
		if (name == token[TOKEN_KF_POSITION])
		{
			type = IElement::PT_VECTOR3; return true;
		}
		else if (name == token[TOKEN_KF_PITCH])
		{
			type = IElement::PT_REAL; return true;
		}
		else if (name == token[TOKEN_KF_YAW])
		{
			type = IElement::PT_REAL; return true;
		}
		else if (name == token[TOKEN_KF_ROLL])
		{
			type = IElement::PT_REAL; return true;
		}
		else if(name == token[TOKEN_KF_SELFROTATE])
		{
			type = IElement::PT_REAL; return true;
		}
		else if(name == token[TOKEN_KF_SCALE])
		{
			type = IElement::PT_REAL; return true;
		}
		else if (name == token[TOKEN_KF_COLOR])
		{
			type = IElement::PT_COLOR; return true;
		}
		/*else if (name == token[TOKEN_KF_BEGINTIME])
		{
		type = IElement::PT_INT; return true;
		}*/
		else if (name == token[TOKEN_KF_AFFECTTIME])
		{
			type = IElement::PT_INT; return true;
		}
		else if (name == token[TOKEN_KF_INTERPOLATION])
		{
			type = IElement::PT_KEYFRAME_SLERP_MODE; return true;
		}
		return false;
	}

	bool EffectKeyFrame::getPropertyValue(const String & name, String &value)
	{
		if (name == token[TOKEN_KF_POSITION])
		{
			value = StringUtil::ToString(mPosition); return true;
		}
		else if (name == token[TOKEN_KF_PITCH])
		{
			value = StringUtil::ToString(mPitch); return true;
		}
		else if (name == token[TOKEN_KF_YAW])
		{
			value = StringUtil::ToString(mYaw); return true;
		}
		else if (name == token[TOKEN_KF_ROLL])
		{
			value = StringUtil::ToString(mRoll); return true;
		}
		else if(name == token[TOKEN_KF_SELFROTATE])
		{
			value = StringUtil::ToString(mSelfRotate); return true;
		}
		else if(name == token[TOKEN_KF_SCALE])
		{
			value = StringUtil::ToString(mScale); return true;
		}
		else if (name == token[TOKEN_KF_COLOR])
		{
			value = StringUtil::ToString(mColor); return true;
		}
		/*else if (name == token[TOKEN_KF_BEGINTIME])
		{
		value = StringUtil::ToString(mBeginTime); return true;
		}*/
		else if (name == token[TOKEN_KF_AFFECTTIME])
		{
			value = StringUtil::ToString(mAffectTime); return true;
		}
		else if (name == token[TOKEN_KF_INTERPOLATION])
		{
			value = getInterpolationTypeString(); return true;
		}
		return false;
	}

	bool EffectKeyFrame::setPropertyValue(const String & name,const String & value)
	{
		if (name == token[TOKEN_KF_POSITION])
		{
			mPosition = StringUtil::ParseVec3(value); return true;
		}
		else if (name == token[TOKEN_KF_PITCH])
		{
			mPitch = StringUtil::ParseReal(value); return true;
		}
		else if (name == token[TOKEN_KF_YAW])
		{
			mYaw = StringUtil::ParseReal(value); return true;
		}
		else if (name == token[TOKEN_KF_ROLL])
		{
			mRoll = StringUtil::ParseReal(value); return true;
		}
		else if(name == token[TOKEN_KF_SELFROTATE])
		{
			mSelfRotate = StringUtil::ParseReal(value); return true;
		}
		else if(name == token[TOKEN_KF_SCALE])
		{
			mScale = StringUtil::ParseReal(value); return true;
		}
		else if (name == token[TOKEN_KF_COLOR])
		{
			mColor = StringUtil::ParseColor(value); return true;
		}
		/*else if (name == token[TOKEN_KF_BEGINTIME])
		{
		mBeginTime = Ogre::StringConverter::parseInt(value); return true;
		}*/
		else if (name == token[TOKEN_KF_AFFECTTIME])
		{
			mAffectTime = StringUtil::ParseInt(value); return true;
		}
		else if (name == token[TOKEN_KF_INTERPOLATION])
		{
			mInterpolationType = IT_CONTROLLER;
			if(value == "Spline")
				mInterpolationType = IT_SPLINE;
			else if(value == "Slerp")
				mInterpolationType = IT_SLERP;
			return true;
		}
		return false;
	}
}
