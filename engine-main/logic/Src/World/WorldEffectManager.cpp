#include "WorldEffectManager.h"
#include "Scene/SceneManager.h"
#include "Effect/EffectSystemManager.h"

namespace BLOCKMAN
{
	void SimpleEffect::setExtenalParam(const Vector3& beginPos, const Vector3& endPos)
	{
		mEffect->setExtenalParam(beginPos, endPos);
	}

	void SimpleEffect::setSpecialParam(float yaw_1, float yaw_2)
	{
		mEffect->setSpecialParam(yaw_1, yaw_2);
	}

	void SimpleEffect::setParabolicParam(const Vector3& point_1, const Vector3& point_2, const Vector3& point_3)
	{
		mEffect->setParabolicParam(point_1, point_2, point_3);
	}

	void SimpleEffect::setViewSpace(bool isViewSpace)
	{
		mEffect->setLayersViewSpace(isViewSpace);
	}

	WorldEffectManager::WorldEffectManager()
	{
		mEffectNode = LordNew SceneNode;
	}

	WorldEffectManager::~WorldEffectManager()
	{
		if (mEffectNode != NULL)
		{
			mEffectNode->destroyAllChildren();
			delete mEffectNode;
			mEffectNode = NULL;
		}
		destoryAllSimpleEffect();
	}

	SimpleEffect* WorldEffectManager::addSimpleEffect(String templateName, Vector3 position, float yaw, int duration, float scaleX, float scaleY, float scaleZ)
	{
		String effectName = getNextEffectName();
		EffectSystem* pEffect = EffectSystemManager::Instance()->createEffectSystem(effectName, templateName);
		if (!pEffect)
		{
			return nullptr;
		}

		SceneNode* pNode = NULL;
		pNode = getEffectNode()->createChild();
		pEffect->setParentSceneNode(pNode);
		pNode->setWorldPosition(position);
		pNode->setLocalScaling(Vector3(scaleX, scaleY, scaleZ));
		Quaternion quaternion(Vector3::UNIT_Y, -yaw * Math::DEG2RAD);
		pNode->setLocalOrientation(quaternion);
		pNode->update();
		if (!pEffect->isPlaying())
		{
			pEffect->prepare_sys(true);
			pEffect->setCurrentCamera(SceneManager::Instance()->getMainCamera());
			pEffect->start();
		}
		SimpleEffect* pSimpleEffect = new SimpleEffect(effectName, templateName, position, quaternion, duration, pNode, pEffect);
		mSimpleEffectMap[effectName] = pSimpleEffect;
		return pSimpleEffect;
	}

	void WorldEffectManager::renderSimpleEffects(ui32 frameTime, Vector3 pos)
	{
		for (auto iter = mSimpleEffectMap.begin(); iter != mSimpleEffectMap.end(); ++iter)
		{
			auto pEffect = iter->second;
			if (pEffect->mDuration == 0)
				continue;
			if (pos.distanceTo(pEffect->mPosition) > 80)
				continue;
			pEffect->mNode->setWorldPosition(pEffect->mPosition);
			pEffect->mNode->setLocalOrientation(pEffect->mQuaternion);
			pEffect->mNode->update();
			pEffect->mEffect->_update_sys(frameTime);
			pEffect->mEffect->_updateRenderQueue();
			if (pEffect->mDuration != -1)
			{
				pEffect->mDuration -= frameTime;
				pEffect->mDuration = pEffect->mDuration > 0 ? pEffect->mDuration : 0;
			}
		}
		removeExpiredSimpleEffect();
	}

	void WorldEffectManager::destorySimpleEffect(String effectName)
	{
		auto iter = mSimpleEffectMap.find(effectName);
		if (iter != mSimpleEffectMap.end())
		{
			delete iter->second;
			mSimpleEffectMap.erase(iter);
		}
	}

	void WorldEffectManager::destoryAllSimpleEffect()
	{
		for (auto iter = mSimpleEffectMap.begin(); iter != mSimpleEffectMap.end(); ++iter)
		{
			delete iter->second;
		}
		mSimpleEffectMap.clear();
	}

	void WorldEffectManager::removeExpiredSimpleEffect()
	{
		for (auto iter = mSimpleEffectMap.begin(); iter != mSimpleEffectMap.end(); )
		{
			if (iter->second->mDuration == 0)
			{
				delete iter->second;
				EffectSystem* pEffect = EffectSystemManager::Instance()->getEffectSystem(iter->first);
				if (pEffect)
				{
					SceneNode* effectNode = pEffect->getEffectSceneNode();
					EffectSystemManager::Instance()->destroyEffectSystem(pEffect);
					getEffectNode()->destroyChild(effectNode);
				}
				iter = mSimpleEffectMap.erase(iter);
			}
			else
			{
				iter++;
			}
		}
	}

	String WorldEffectManager::getNextEffectName()
	{
		static int worldEffectCount = 0;
		return "WorldEffect" + StringUtil::Format("%d", worldEffectCount++);
	}

}