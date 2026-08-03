#ifndef __SKILL_EFFECT_MANAGER_HEADER__
#define __SKILL_EFFECT_MANAGER_HEADER__

#include "Core.h"

using namespace LORD;

namespace BLOCKMAN
{
	class EntitySkillEffect
	{
	public:
		String name;
		float duration;
		int width;
		int height;
		int density;
		Vector3 position;
		Vector3 color;
	public:
		EntitySkillEffect() :
			name(""),
			duration(0.f),
			width(0),
			height(0),
			density(1),
			position(Vector3(0.f, 0.f, 0.f)),
			color(Vector3(0.f, 0.f, 0.f))
		{

		}

		EntitySkillEffect(String name, float duration, int width, int height, int density, Vector3 position, Vector3 color) :
			name(name),
			duration(duration),
			width(width),
			height(height),
			density(density),
			position(position),
			color(color)
		{

		}
	};

	class SkillEffectManager : public Singleton<SkillEffectManager>, public ObjectAlloc
	{
	public:
		SkillEffectManager();
		~SkillEffectManager();
		void onUpdate();
		void addSkillEffect(EntitySkillEffect* skillEffect);
	private:
		list<EntitySkillEffect*>::type m_skillEffects;
		void removeSkillEffects();
	};

}




#endif // !__SKILL_EFFECT_MANAGER_HEADER__