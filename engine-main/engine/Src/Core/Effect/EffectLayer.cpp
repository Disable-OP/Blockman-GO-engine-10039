#include "Core.h"
#include "EffectSystem.h"
#include "EffectLayer.h"
#include "EffectSystemManager.h"
#include "Resource/DataStream.h"

namespace LORD
{
	const int	EffectLayer::DEFAULT_REPLAY_COUNT = 1;
	const int	EffectLayer::DEFAULT_REPLAY_TIME = 0;
	const bool	EffectLayer::DEFAULT_RESTATE_LOOPEND = false;
	const float EffectLayer::DEFAULT_LY_WIDTH = 1.0f;
	const float EffectLayer::DEFAULT_LY_HEIGHT = 1.0f;
	const bool	EffectLayer::DEFAULT_LY_PERPENDICULAR = false;
	const bool	EffectLayer::DEFAULT_LY_NOSCALEHEIGHT = false;
	const bool	EffectLayer::DEFAULT_LY_NOSCALEWIDTH = false;
	const float EffectLayer::DEFAULT_LY_CENTEROFFSETW = 0.5f;
	const float EffectLayer::DEFAULT_LY_CENTEROFFSETH = 0.5f;
	const float EffectLayer::DEFAULT_LY_OFFSETVIEWPORT = 0.0f;
	const float EffectLayer::DEFAULT_LY_BOUNDALPHA = 0.05f;

	EffectLayer::EffectLayer() 
		:mRenderGroup(ERG_GROUPUP)
		,mReplayCount(DEFAULT_REPLAY_COUNT)
		,mReplayTime(DEFAULT_REPLAY_TIME)
		,mbRestateLoopEnd(DEFAULT_RESTATE_LOOPEND)
		,mName(StringUtil::BLANK)
		,mType(ELT_Unknow)
		,mParentSystem(NULL)
		,mVisualbleParticleNum(0)
		,mbVisible(true)
		,mCurrentTime(0)
		,mReplayIdelTime(0)
		,mbReplayIdel(false)
		,mFrameIndex(0)
		,mbPlayEnd(false)
		,mbAffected(false)
		,mpFallowLayer(NULL)
		,mMaterial(NULL)
		,mbParticleSystem(false)
		,mbShowVisualHelp(false)
	{

	}

	EffectLayer::~EffectLayer()
	{
		destroyAllKeyFrames();
		mMaterial->unprepare();
		destroyMaterial();
	}

	void EffectLayer::prepare_res()
	{
		if(mMaterial)
			mMaterial->prepare_res();
	}

	void EffectLayer::prepare_io()
	{
		if(!mMaterial)
			assert(0);

		mMaterial->prepare_io();
	}

	void EffectLayer::prepare()
	{
		if(!mMaterial)
			assert(0);

		mMaterial->prepare();

	}

	void EffectLayer::unprepare()
	{
		mMaterial->unprepare();
	}

	String EffectLayer::getName() const
	{
		return mName;
	}

	void EffectLayer::setName(const String& name)
	{
		mName = name;
	}

	void EffectLayer::_notifyReStart()
	{
		mCurrentTime = 0;
		mReplayIdelTime = 0;
		mbReplayIdel = false;
		mFrameIndex = 0;
		mbPlayEnd = false;
		mbAffected = false;

		if(mMaterial)
			mMaterial->_notifyStart();

		if(mbRestateLoopEnd)
		{
			reParticleState();
		}

		EffectKeyFrame* secondKeyframe = NULL;
		if(mKeyFrames.size() > 1)
			secondKeyframe = mKeyFrames[1];
		mKeyFrames[0]->_notifyStart(secondKeyframe);
	}

	void EffectLayer::_notifyStart()
	{
		mCurrentTime = 0;
		mReplayIdelTime = 0;
		mbReplayIdel = false;
		mbPlayEnd = false;
		mbAffected = false;
		mFrameIndex = 0;
		mInerReplayCount = mReplayCount;

		if(mMaterial)
			mMaterial->_notifyStart();

		EffectKeyFrame* secondKeyframe = NULL;
		if(mKeyFrames.size() > 1)
			secondKeyframe = mKeyFrames[1];
		mKeyFrames[0]->_notifyStart(secondKeyframe);
	}

	void EffectLayer::_notifyStop()
	{

	}

	void EffectLayer::_notifyPause()
	{

	}

	void EffectLayer::_notifyResume()
	{

	}

	void EffectLayer::_notifyAffected()
	{
		_update(1);
	}

	void EffectLayer::_notifyUnAffected()
	{

	}

	void EffectLayer::_notifyExtenalParam(const Vector3& p1, const Vector3& p2)
	{
		// nothing to do.
	}

	void EffectLayer::_notifyShowVisualHelp(bool show)
	{
		mbShowVisualHelp = show;
	}

	void EffectLayer::copyAttributesTo(EffectParticle* particle)
	{
		EffectLayer* pLayer = (EffectLayer*)particle;
		pLayer->destroyAllKeyFrames();
		pLayer->destroyMaterial();

		EffectParticle::copyAttributesTo(pLayer);

		// copy property
		pLayer->mbRestateLoopEnd = pLayer->mbRestateLoopEnd;
		pLayer->mName = mName;
		pLayer->mbVisible = mbVisible;
		pLayer->mRenderGroup = mRenderGroup;
		pLayer->mReplayCount = mReplayCount;
		pLayer->mReplayTime = mReplayTime;

		pLayer->mFrameIndex = mFrameIndex;
		pLayer->mReplayIdelTime = mReplayIdelTime;
		pLayer->mbReplayIdel = mbReplayIdel;
		pLayer->mCurrentTime = mCurrentTime;
		pLayer->mbPlayEnd = mbPlayEnd;
		pLayer->mInerReplayCount = mInerReplayCount;

		// copy componment
		for(size_t i=0; i<getNumKeyFrames(); ++i)
		{
			EffectKeyFrame* cloneKeyFrame = EffectSystemManager::Instance()->cloneKeyFrame(getKeyFrame(i));
			pLayer->addKeyFrame(cloneKeyFrame);
		}

		pLayer->setEffectMaterial(EffectSystemManager::Instance()->cloneMaterial(mMaterial));
	}

	void EffectLayer::setRenderQueueGroup(EFFECT_RENDER_GROUP rg)
	{
		mRenderGroup = rg;
	}

	EFFECT_RENDER_GROUP EffectLayer::getRenderQueueGroup() const 
	{
		return mRenderGroup;
	}

	void EffectLayer::_updateRenderQueue()
	{

	}

	bool EffectLayer::getVisible()const
	{
		return mbVisible;
	}

	void EffectLayer::setVisible(bool visible)
	{
		mbVisible = visible;
	}

	EffectMaterial* EffectLayer::createMaterial()
	{
		EffectMaterial* material = EffectSystemManager::Instance()->createMaterial();
		setEffectMaterial(material);
		return material;
	}

	void EffectLayer::setEffectMaterial(EffectMaterial* material)
	{
		mMaterial = material;
	}

	void EffectLayer::destroyMaterial()
	{
		EffectSystemManager::Instance()->destroyMaterial(mMaterial);
		mMaterial = NULL;
	}

	EffectKeyFrame* EffectLayer::createkeyFrame(size_t index /* = -1 */)
	{
		EffectKeyFrame* keyFrame = EffectSystemManager::Instance()->createKeyFrame();
		addKeyFrame(keyFrame, index);
		return keyFrame;
	}

	void EffectLayer::addKeyFrame(EffectKeyFrame* keyFrame, size_t index)
	{
		if(index == -1)
			mKeyFrames.push_back(keyFrame);
		else
		{
			mKeyFrames.push_back(keyFrame);
			EffectKeyFrame* temp;
			for(size_t tindex = mKeyFrames.size()-1; tindex>index+1; tindex--)
			{
				temp = mKeyFrames[tindex];
				mKeyFrames[tindex] = mKeyFrames[tindex-1];
				mKeyFrames[tindex-1] = temp;
			}
		}
		keyFrame->setParentLayer(this);
	}

	void EffectLayer::removeKeyFrame(EffectKeyFrame* keyFrame)
	{
		assert(keyFrame && "KeyFrame is null!");
		EffectKeyFrameIterator it;
		for (it = mKeyFrames.begin(); it != mKeyFrames.end(); ++it)
		{
			if ((*it) == keyFrame)
			{
				mKeyFrames.erase(it);
				break;
			}
		}
		keyFrame->setParentLayer(0);
	}

	EffectKeyFrame* EffectLayer::getKeyFrame(size_t index) const
	{
		assert(index < mKeyFrames.size() && "KeyFrame index out of bounds!");
		return mKeyFrames[index];
	}

	void EffectLayer::destroyKeyFrame(size_t index)
	{
		destroyKeyFrame(getKeyFrame(index));
	}

	void EffectLayer::destroyKeyFrame(EffectKeyFrame* keyFrame)
	{
		assert(keyFrame && "KeyFrame is null!");
		EffectKeyFrameIterator it;
		for (it = mKeyFrames.begin(); it != mKeyFrames.end(); ++it)
		{
			if ((*it) == keyFrame)
			{
				EffectSystemManager::Instance()->destroyKeyFrame(*it);
				mKeyFrames.erase(it);
				break;
			}
		}
	}

	void EffectLayer::destroyAllKeyFrames()
	{
		EffectKeyFrameIterator it;
		for (it = mKeyFrames.begin(); it != mKeyFrames.end(); ++it)
		{
			EffectSystemManager::Instance()->destroyKeyFrame(*it);
		}
		mKeyFrames.clear();
	}

	int EffectLayer::_update(i32 time)
	{
		if(mbPlayEnd)
			return 0;

		mCurrentTime += time;

		if(mbReplayIdel)
		{
			mReplayIdelTime += time;
			if(mReplayIdelTime >= mReplayTime)
			{
				mInerReplayCount--;
				mbReplayIdel = false;
				if(mInerReplayCount == 0)
				{
					mbPlayEnd = true;
				}
				else
				{
					_notifyReStart();
					return 1;
				}
			}
			else
			{
				return 1;
			}
		}
		else if(!mbAffected)
		{
			i32 frameTime = mCurrentTime - mKeyFrames[mFrameIndex]->getBeginTime();
			if(frameTime <= 0)
			{
				return 1;
			}
			else  
			{
				mbAffected = true;
				_notifyAffected();
				_updateKeyFrame(frameTime);
			}
		}
		else 
		{
			_updateKeyFrame(time);
		}

		if(mbPlayEnd)
			return 0;
		return 1;
	}

	void EffectLayer::_updateKeyFrame(i32 time)
	{
		assert(mbAffected);

		if(mMaterial)
		{
			mMaterial->_update(time);

		}

		i32 remainderTime = mKeyFrames[mFrameIndex]->_update_sys(time);
		if(remainderTime > 0)
		{
			size_t frameCount = mKeyFrames.size();
			mFrameIndex++;
			if(mFrameIndex >= frameCount)
			{
				mbAffected = false;
				_notifyUnAffected();
				if(mReplayTime > 0)
				{
					mReplayIdelTime = remainderTime;
					mbReplayIdel = true;
				}
				else
				{
					mInerReplayCount--;
					mbReplayIdel = false; 
					if(mInerReplayCount == 0)
					{  
						mbPlayEnd = true; 
					}
					else
					{
						_notifyReStart();
						_update(remainderTime);
					}
				}
			}
			else
			{
				EffectKeyFrame* secondKeyframe = NULL;
				if(mKeyFrames.size() > mFrameIndex+1)
					secondKeyframe = mKeyFrames[mFrameIndex+1];
				mKeyFrames[mFrameIndex]->_notifyContinue(secondKeyframe);
				_updateKeyFrame(remainderTime);
			}
		}
	}

	Matrix4 EffectLayer::getTransMatrix()
	{
		Quaternion selfOritation;
		selfOritation.rotateAxis(originalDirection, selfRotation);
		Matrix4 selfRotateMatrix(selfOritation);
		Matrix4 rotateMatrix;
		rotateMatrix.fromQuan(oritation);
		Matrix4 transMatrix = Matrix4::IDENTITY;
		transMatrix.translate(position);
		Matrix4 sclaeMatrix = Matrix4::IDENTITY;
		sclaeMatrix.scale(scale, scale, scale);
		transMatrix = transMatrix * rotateMatrix * sclaeMatrix * selfRotateMatrix;
		return transMatrix;
	}

	float EffectLayer::getLayerTimeLength() const 
	{
		unsigned int keyFrameTime = mKeyFrames[0]->getBeginTime();
		EffectKeyFrameConstIterator it;

		for( it=mKeyFrames.begin(); it!=mKeyFrames.end(); ++it)
		{
			if((*it)->getAffectTime() < 0)
				return -1.0f;
			keyFrameTime += (*it)->getAffectTime();
		}

		keyFrameTime += mReplayTime;
		keyFrameTime *= mReplayCount;

		return float(keyFrameTime)/1000.0f;
	}

	void EffectLayer::setFallowLayer(EffectLayer* layer)
	{
		mpFallowLayer = layer;
	}

	EffectLayer* EffectLayer::getFallowLayer() const
	{
		return mpFallowLayer;
	}

	void EffectLayer::fallowMovement()
	{
		if(mpFallowLayer)
		{
			position = mpFallowLayer->position;
			oritation = mpFallowLayer->oritation;
			direction = mpFallowLayer->direction;
			selfRotation = mpFallowLayer->selfRotation;
		}
	}

	void EffectLayer::exportData(DataStream* pStream, int version)
	{
		// property base
		i32 rg = (ui32)mRenderGroup;
		pStream->write(&rg, sizeof(i32));
		pStream->write(&mReplayCount, sizeof(i32));
		pStream->write(&mReplayTime, sizeof(i32));
		pStream->write(&mbRestateLoopEnd, sizeof(bool));
		ui32 strlength = (ui32)mName.length();
		pStream->write(&strlength, sizeof(ui32));
		pStream->write(mName.c_str(), strlength);

		// material
		mMaterial->exportData(pStream, version);

		// keyframe
		ui32 keyFrameNum = (ui32)mKeyFrames.size();
		pStream->write(&keyFrameNum, sizeof(ui32));

		for(size_t i=0; i<keyFrameNum; ++i)
		{
			EffectKeyFrame* pKeyFrame = mKeyFrames[i];
			pKeyFrame->exportData(pStream, version);
		}
	}

	void EffectLayer::importData(DataStream* pStream, int version)
	{
		// property base
		i32 rg;
		pStream->read(&rg, sizeof(i32)); 
		mRenderGroup = (EFFECT_RENDER_GROUP)rg;
		pStream->read(&mReplayCount, sizeof(i32));
		pStream->read(&mReplayTime, sizeof(i32));
		pStream->read(&mbRestateLoopEnd, sizeof(bool));
		ui32 strlength;
		pStream->read(&strlength, sizeof(ui32));
		char str[128];
		pStream->read(str, strlength);
		str[strlength] = 0;
		mName = str;

		// material
		LordAssertX(mMaterial == NULL, "Error: EffectLayer::importData error occurse in material");
		mMaterial = EffectSystemManager::Instance()->createMaterial();
		mMaterial->importData(pStream, version);

		// keyframe
		LordAssertX(mKeyFrames.empty(), "Error: EffectLayer::importData error occurse in Keyframes");
		ui32 keyFrameNum;
		pStream->read(&keyFrameNum, sizeof(ui32));
	
		for(size_t i=0; i<keyFrameNum; ++i)
		{
			EffectKeyFrame* pKeyFrame = createkeyFrame();
			pKeyFrame->importData(pStream, version);
		}
	}

	void EffectLayer::getPropertyList(PropertyList & list)
	{
		list.push_back(token[TOKEN_LY_RENDERGROUP]);
		list.push_back(token[TOKEN_LY_REPLAYCOUNT]);
		list.push_back(token[TOKEN_LY_REPLAYTIME]);
		list.push_back(token[TOKEN_LY_RESTATELOOP]);
	}

	bool EffectLayer::getPropertyType(const String & name, PropertyType &type)
	{
		if(name == token[TOKEN_LY_RENDERGROUP])
		{
			type = IElement::PT_RENDERQUEUE; return true;
		}
		else if (name == token[TOKEN_LY_REPLAYCOUNT])
		{
			type = IElement::PT_INT; return true;
		}
		else if (name == token[TOKEN_LY_REPLAYTIME])
		{
			type = IElement::PT_INT; return true;
		}
		else if (name == token[TOKEN_LY_RESTATELOOP])
		{
			type = IElement::PT_BOOL; return true;
		}

		return false;
	}

	bool EffectLayer::getPropertyValue(const String & name, String &value)
	{
		if (name == token[TOKEN_LY_RENDERGROUP])
		{
			value = StringUtil::ToString(mRenderGroup); return true;
		}
		else if (name == token[TOKEN_LY_REPLAYCOUNT])
		{
			value = StringUtil::ToString(mReplayCount); return true;
		}
		else if (name == token[TOKEN_LY_REPLAYTIME])
		{
			value = StringUtil::ToString(mReplayTime); return true;
		}
		else if (name == token[TOKEN_LY_RESTATELOOP])
		{
			value = StringUtil::ToString(mbRestateLoopEnd); return true;
		}

		return false;
	}

	bool EffectLayer::setPropertyValue(const String & name,const String & value)
	{
		if (name == token[TOKEN_LY_RENDERGROUP])
		{
			mRenderGroup = (EFFECT_RENDER_GROUP)StringUtil::ParseInt(value);
			return true;
		}
		else if (name == token[TOKEN_LY_REPLAYCOUNT])
		{
			mReplayCount = StringUtil::ParseInt(value); return true;
		}
		else if (name == token[TOKEN_LY_REPLAYTIME])
		{
			mReplayTime = StringUtil::ParseInt(value); return true;
		}
		else if (name == token[TOKEN_LY_RESTATELOOP])
		{
			mbRestateLoopEnd = StringUtil::ParseBool(value); return true;
		}

		return false;
	}
}