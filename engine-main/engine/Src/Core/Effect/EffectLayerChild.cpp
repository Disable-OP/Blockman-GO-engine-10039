#include "Core.h"
#include "EffectLayerChild.h"
#include "EffectSystem.h"
#include "EffectSystemManager.h"
#include "Scene/SceneManager.h"
#include "Scene/SceneNode.h"
#include "Resource/DataStream.h"
#include "Util/StringUtil.h"
#include "Util/AssertX.h"

namespace LORD
{

	EffectLayerChild::EffectLayerChild()
		: EffectLayer()
		, mChildEffectName(StringUtil::BLANK)
		, mChildEffectLoop(true)
		, mChildEffectSpeed(1.f)
		, mChildEffect(NULL)
		, mChildEffectNode(NULL)
	{
		originalDirection = Vector3::UNIT_Y;
		direction = Vector3::UNIT_Y;
		mType = ELT_Child;
	}

	EffectLayerChild::~EffectLayerChild()
	{

	}

	void EffectLayerChild::prepare_res()
	{
		if(mChildEffectName != StringUtil::BLANK && mParentSystem->getEffectSceneNode())
		{
			LordAssert(mChildEffect == NULL);
			LordAssert(mChildEffectNode == NULL);

			bool isUIEffect = mParentSystem->getIsUIEffect();
			SceneNode* pParentNode = mParentSystem->getEffectSceneNode();
			mChildEffectNode = pParentNode->createChild();

			static ui32 childCounter = 0;
			childCounter++;
			mChildEffect = LORD::EffectSystemManager::Instance()->createEffectSystem(LORD::String("child") + LORD::StringUtil::ToString(childCounter++), mChildEffectName, isUIEffect);
			mChildEffect->setIsChild();
			mChildEffect->setParentSceneNode(mChildEffectNode);

			if (isUIEffect)
			{
				mChildEffect->setCurrentCamera(SceneManager::Instance()->getGUICamera());	
			} 
			else
			{
				mChildEffect->setCurrentCamera(SceneManager::Instance()->getMainCamera());	
			}

			mChildEffect->prepare_res();
		}
	}

	void EffectLayerChild::prepare_io()
	{
		if(mChildEffect)
		{
			mChildEffect->prepare_io_sys();
		}
	}

	void EffectLayerChild::prepare()
	{
		if(mChildEffect)
		{
			mChildEffect->prepareLayer();
		}
	}

	void EffectLayerChild::unprepare()
	{
		if(mChildEffect)
		{
			mChildEffect->unprepareLayer();
			LORD::EffectSystemManager::Instance()->destroyEffectSystem(mChildEffect);

			// add by ajohn to fix a bug.Unconventional means
			SceneNode* paraentNode = mParentSystem->getEffectSceneNode();
			if(paraentNode)
				paraentNode->destroyChild(mChildEffectNode);
			else 
				LordDelete mChildEffectNode;
		
			mChildEffectNode = NULL;
			mChildEffect = NULL;
		}
	}

	void EffectLayerChild::_notifyStart(void)
	{
		EffectLayer::_notifyStart();
		if(mChildEffect)
		{
			mChildEffect->start();
		}
	}

	void EffectLayerChild::_notifyReStart(void)
	{
		EffectLayer::_notifyReStart();
		if(mChildEffect)
		{
			mChildEffect->start();
		}
	}

	void EffectLayerChild::_notifyStop()
	{
		if(mChildEffect)
		{
			mChildEffect->stop();
		}
	}

	void EffectLayerChild::_notifyPause()
	{
		if(mChildEffect)
		{
			mChildEffect->pause();
		}
	}

	void EffectLayerChild::_notifyResume()
	{
		if(mChildEffect)
		{
			mChildEffect->resume();
		}
	}

	int EffectLayerChild::_update(i32 time)
	{
		int ret = EffectLayer::_update(time);
		if(mChildEffect && mbAffected)
		{
			mChildEffect->_update(i32(time*mChildEffectSpeed));
			if(mChildEffect->isStoped() && mChildEffectLoop)
			{
				mChildEffect->start();
				mChildEffect->_update(1);
			}

			Quaternion quanRot;
			quanRot.rotateAxis(originalDirection, selfRotation*Math::DEG2RAD);
			quanRot = quanRot*oritation;

			mChildEffectNode->setLocalScaling(Vector3(scale, scale, scale));
			mChildEffectNode->setLocalPosition(position);
			mChildEffectNode->setLocalOrientation(quanRot);
			mChildEffectNode->update();
		}
		return ret;
	}

	void EffectLayerChild::_updateRenderQueue()
	{
		if ((!mbAffected) || (!mbVisible) || (!mChildEffect))
			return;

		mChildEffect->_updateRenderQueue();
	}

	void EffectLayerChild::copyAttributesTo(EffectParticle* particle)
	{
		EffectLayer::copyAttributesTo(particle);
		EffectLayerChild* child = static_cast<EffectLayerChild*>(particle);
		child->mChildEffectName = mChildEffectName;
		child->mChildEffectLoop = mChildEffectLoop;
		child->mChildEffectSpeed = mChildEffectSpeed;
	}

	void EffectLayerChild::_notifyParabolicParam(const Vector3& p1, const Vector3& p2, const Vector3& p3)
	{
		if (mChildEffect)
		{
			vector<EffectLayer*>::type layerList = mChildEffect->getLayers();
			for (auto iter : layerList)
			{
				iter->_notifyParabolicParam(p1, p2, p3);
			}
		}
	}

	void EffectLayerChild::exportData(DataStream* pStream, int version)
	{
		EffectLayer::exportData(pStream, version);
		ui32 strlength = (ui32)mChildEffectName.length();
		pStream->write(&strlength, sizeof(ui32));
		pStream->write(mChildEffectName.c_str(), strlength);
		pStream->write(&mChildEffectLoop, sizeof(bool));
		pStream->write(&mChildEffectSpeed, sizeof(float));
	}

	void EffectLayerChild::importData(DataStream* pStream, int version)
	{
		EffectLayer::importData(pStream, version);
		ui32 strLength;
		pStream->read(&strLength, sizeof(strLength));
		char str[128];
		pStream->read(str, strLength);
		str[strLength] = 0;
		mChildEffectName = str;
		pStream->read(&mChildEffectLoop, sizeof(bool));
		pStream->read(&mChildEffectSpeed, sizeof(float));
	}

	void EffectLayerChild::mergeAligenBox(Box& aabb)
	{
		if (!mbAffected)
			return;

		if(mChildEffect)
		{
			mChildEffect->caculateAligenBox();
			aabb.unionBox(mChildEffect->getBoundingBox());
		}
	}

	void EffectLayerChild::getPropertyList(PropertyList & list)
	{
		EffectLayer::getPropertyList(list);
		list.push_back(token[TOKEN_LY_CE_NAME]);
		list.push_back(token[TOKEN_LY_CE_LOOP]);
		list.push_back(token[TOKEN_LY_CE_SPEED]);
	}

	bool EffectLayerChild::getPropertyType(const String & name, PropertyType &type)
	{
		if(EffectLayer::getPropertyType(name, type))
			return true;
		if (name == token[TOKEN_LY_CE_NAME])
		{
			type = IElement::PT_CHILDEFFECT_NAME_TYPE; return true;
		}
		else if(name == token[TOKEN_LY_CE_LOOP])
		{
			type = IElement::PT_BOOL; return true;
		}
		else if(name == token[TOKEN_LY_CE_SPEED])
		{
			type = IElement::PT_REAL; return true;
		}
		return false;
	}

	bool EffectLayerChild::getPropertyValue(const String & name, String &value)
	{
		if(EffectLayer::getPropertyValue(name, value))
			return true;

		if (name == token[TOKEN_LY_CE_NAME])
		{
			value = mChildEffectName; return true;
		}
		else if(name == token[TOKEN_LY_CE_LOOP])
		{
			value = StringUtil::ToString(mChildEffectLoop); return true;
		}
		else if(name == token[TOKEN_LY_CE_SPEED])
		{
			value = StringUtil::ToString(mChildEffectSpeed); return true;
		}
		
		return false;
	}

	bool EffectLayerChild::setPropertyValue(const String & name,const String & value)
	{
		if(EffectLayer::setPropertyValue(name, value))
			return true;

		if (name == token[TOKEN_LY_CE_NAME])
		{
			mChildEffectName = value; return true;
		}
		else if(name == token[TOKEN_LY_CE_LOOP])
		{
			mChildEffectLoop = StringUtil::ParseBool(value); return true;
		}
		else if(name == token[TOKEN_LY_CE_SPEED])
		{
			mChildEffectSpeed = StringUtil::ParseFloat(value); return true;
		}

		return false;
	}
}