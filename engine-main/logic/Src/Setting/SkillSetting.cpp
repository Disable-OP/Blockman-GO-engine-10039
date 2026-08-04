#include "SkillSetting.h"

namespace BLOCKMAN
{
	std::map<int, SkillEffect*> SkillSetting::m_skillEffectMap = {};
	std::map<int, SkillItem*> SkillItemSetting::m_skillItemMap = {};

	void SkillSetting::unloadSetting()
	{
		for (auto iter = m_skillEffectMap.begin(); iter != m_skillEffectMap.end(); iter++)
		{
			SkillEffect* pSkillEffect = iter->second;
			delete pSkillEffect;
		}

		m_skillEffectMap.clear();
	}

	SkillEffect * SkillSetting::getSkillEffect(int id)
	{
		auto iter = m_skillEffectMap.find(id);
		if (iter != m_skillEffectMap.end())
		{
			return iter->second;
		}
		return nullptr;
	}

	void SkillSetting::addSkillEffect(SkillEffect skillEffect)
	{
		SkillEffect * pSkillEffect = getSkillEffect(skillEffect.id);
		if (!pSkillEffect)
		{
			pSkillEffect = new SkillEffect();
			pSkillEffect->id = skillEffect.id;
			m_skillEffectMap.insert(std::pair<int, SkillEffect*>(pSkillEffect->id, pSkillEffect));
		}
		pSkillEffect->name = skillEffect.name;
		pSkillEffect->duration = skillEffect.duration;
		pSkillEffect->width = skillEffect.width;
		pSkillEffect->height = skillEffect.height;
		pSkillEffect->density = skillEffect.density;
		pSkillEffect->color = skillEffect.color;
	}

	void SkillItemSetting::unloadSetting()
	{
		for (auto iter = m_skillItemMap.begin(); iter != m_skillItemMap.end(); iter++)
		{
			SkillItem* pSkillItem = iter->second;
			delete pSkillItem;
		}

		m_skillItemMap.clear();
	}

	bool SkillItemSetting::isSkillItem(int itemId)
	{
		for (auto iter = m_skillItemMap.begin(); iter != m_skillItemMap.end(); iter++)
		{
			if (iter->second->ItemId == itemId)
			{
				return true;
			}
		}
		return false;
	}

	bool SkillItemSetting::isAutoCastSkill(int itemId)
	{
		for (auto iter = m_skillItemMap.begin(); iter != m_skillItemMap.end(); iter++)
		{
			if (iter->second->ItemId == itemId)
			{
				return iter->second->AutoCast;
			}
		}
		return false;
	}

	bool SkillItemSetting::isShowCrossHair(int itemId)
	{
		for (auto iter = m_skillItemMap.begin(); iter != m_skillItemMap.end(); iter++)
		{
			if (iter->second->ItemId == itemId)
			{
				return iter->second->IsCrossHair;
			}
		}
		return true;
	}

	bool SkillItemSetting::isThrowSkillItem(int itemId)
	{
		for (auto iter = m_skillItemMap.begin(); iter != m_skillItemMap.end(); iter++)
		{
			if (iter->second->ItemId == itemId)
			{
				if (iter->second->MoveSpeed > 0)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}
		return false;
	}

	SkillItem * SkillItemSetting::getSkillItem(int itemId, int Occupation)
	{
		for (auto iter = m_skillItemMap.begin(); iter != m_skillItemMap.end(); iter++)
		{
			if (iter->second->ItemId == itemId && iter->second->Occupation == Occupation)
			{
				return iter->second;
			}
		}
		return nullptr;
	}

	SkillItem * SkillItemSetting::getSkillItem(int skillId)
	{
		for (auto iter = m_skillItemMap.begin(); iter != m_skillItemMap.end(); iter++)
		{
			if (iter->second->SkillId == skillId)
			{
				return iter->second;
			}
		}
		return nullptr;
	}

	void SkillItemSetting::addSkillItem(SkillItem skillItem)
	{
		SkillItem * pSkillItem = getSkillItem(skillItem.ItemId,skillItem.Occupation);
		if (!pSkillItem)
		{
			pSkillItem = new SkillItem();
			pSkillItem->SkillId = skillItem.SkillId;
			m_skillItemMap.insert(std::pair<int, SkillItem*>(pSkillItem->SkillId, pSkillItem));
		}
		pSkillItem->Occupation = skillItem.Occupation;
		pSkillItem->ItemId = skillItem.ItemId;
		pSkillItem->ItemSkillCd = skillItem.ItemSkillCd;
		pSkillItem->ItemSkillDistace = skillItem.ItemSkillDistace;
		pSkillItem->MoveSpeed = skillItem.MoveSpeed;
		pSkillItem->DropSpeed = skillItem.DropSpeed;
		pSkillItem->AtlasName = skillItem.AtlasName;
		pSkillItem->ItemIconName = skillItem.ItemIconName;
		pSkillItem->Parabola = skillItem.Parabola;
		pSkillItem->AutoCast = skillItem.AutoCast;
		pSkillItem->IsCrossHair = skillItem.IsCrossHair;
		pSkillItem->EffectName = skillItem.EffectName;
	}
}