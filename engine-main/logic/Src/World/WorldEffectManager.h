#ifndef __WORLD_EFFECT_MANAGER_HEADER__
#define __WORLD_EFFECT_MANAGER_HEADER__

#include "Core.h"
#include "Scene/SceneNode.h"
#include "Effect/EffectSystem.h"

using namespace LORD;

namespace BLOCKMAN
{
	class SimpleEffect
	{
	public:
		String mEffectName = "";
		String mTemplateName = "";
		Vector3 mPosition;
		Quaternion mQuaternion;
		int mDuration = -1;
		SceneNode* mNode = NULL;
		EffectSystem* mEffect = NULL;

	public:
		SimpleEffect(String effectName, String templateName, Vector3 position, Quaternion quaternion, int duration, SceneNode* node, EffectSystem* effect)
			: mEffectName(effectName)
			, mTemplateName(templateName)
			, mPosition(position)
			, mQuaternion(quaternion)
			, mDuration(duration)
			, mNode(node)
			, mEffect(effect)
		{}
		
		void setExtenalParam(const Vector3& beginPos, const Vector3& endPos);
		void setSpecialParam(float yaw_1, float yaw_2);
		void setParabolicParam(const Vector3& point_1, const Vector3& point_2, const Vector3& point_3);
		void setViewSpace(bool isViewSpace);
	};

	class WorldEffectManager : public Singleton<WorldEffectManager>, public ObjectAlloc
	{
	private:
		SceneNode* mEffectNode = NULL;
		std::map<String, SimpleEffect*> mSimpleEffectMap;

	public:
		WorldEffectManager();
		~WorldEffectManager();

		SceneNode* getEffectNode() { return mEffectNode; }
		SimpleEffect* addSimpleEffect(String templateName, Vector3 position, float yaw, int duration, float scaleX = 1.0f, float scaleY = 1.0f, float scaleZ = 1.0f);
		void renderSimpleEffects(ui32 frameTime, Vector3 pos);

	private:
		void destorySimpleEffect(String effectName);
		void destoryAllSimpleEffect();
		void removeExpiredSimpleEffect();
		String getNextEffectName();
	};

}

#endif 
