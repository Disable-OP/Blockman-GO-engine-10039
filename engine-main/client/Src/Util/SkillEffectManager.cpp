#include "SkillEffectManager.h"
#include "game.h"

namespace BLOCKMAN
{
	SkillEffectManager::SkillEffectManager()
	{
	}

	SkillEffectManager::~SkillEffectManager()
	{
		for (auto skillEffect : m_skillEffects)
		{
			LordSafeDelete(skillEffect);
		}
		m_skillEffects.clear();
	}

	void SkillEffectManager::onUpdate()
	{
		World* world = Blockman::Instance()->getWorld();
		float delta = world->getFrameDeltaTime() > 0.05f ? world->getFrameDeltaTime() : 0.05f;
		for (auto skillEffect : m_skillEffects)
		{
			skillEffect->duration -= delta;
			int startX = skillEffect->width % skillEffect->density;
			int startZ = skillEffect->height % skillEffect->density;
			for (int x = startX; x <= skillEffect->width; x += skillEffect->density)
			{
				for (int z = startZ; z <= skillEffect->height; z += skillEffect->density)
				{
					world->spawnParticle(skillEffect->name.c_str(), Vector3(skillEffect->position.x - x, skillEffect->position.y, skillEffect->position.z + z), skillEffect->color);
					world->spawnParticle(skillEffect->name.c_str(), Vector3(skillEffect->position.x + x, skillEffect->position.y, skillEffect->position.z + z), skillEffect->color);
					world->spawnParticle(skillEffect->name.c_str(), Vector3(skillEffect->position.x - x, skillEffect->position.y, skillEffect->position.z - z), skillEffect->color);
					world->spawnParticle(skillEffect->name.c_str(), Vector3(skillEffect->position.x + x, skillEffect->position.y, skillEffect->position.z - z), skillEffect->color);
				}
			}
		}
		removeSkillEffects();
	}

	void SkillEffectManager::addSkillEffect(EntitySkillEffect* skillEffect)
	{
		m_skillEffects.push_back(skillEffect);
	}

	void SkillEffectManager::removeSkillEffects()
	{
		for (auto it = m_skillEffects.begin(); it != m_skillEffects.end(); )
		{
			if ((*it)->duration <= 0)
			{
				LordSafeDelete(*it);
				it = m_skillEffects.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
}