#include "LogicSetting.h"
#include "Block/Block.h"
#include "Block/BlockManager.h"
#include "Item/Items.h"

namespace BLOCKMAN
{
	LogicSetting::LogicSetting()
	{

	}

	LogicSetting::~LogicSetting()
	{

	}

	void LogicSetting::setGunPluginSetting(vector<GunPluginSetting>::type gunPluginSetting)
	{
		m_gunPluginSetting.clear();
		for (auto & setting : gunPluginSetting)
		{
			addGunPluginSetting(setting);
		}
	}

	void LogicSetting::addGunPluginSetting(GunPluginSetting setting)
	{
		GunSetting::updateGunSetting(setting);
		m_gunPluginSetting.push_back(std::move(setting));
	}

	void LogicSetting::setBulletClipPluginSetting(vector<BulletClipPluginSetting>::type bulletClipPluginSetting)
	{
		m_bulletClipPluginSetting.clear();
		for (auto & setting : bulletClipPluginSetting)
		{
			addBulletClipPluginSetting(setting.bulletId, setting.maxStack);
		}
	}

	void LogicSetting::addBulletClipPluginSetting(int bulletId, int maxStack)
	{
		BulletClipPluginSetting setting = BulletClipPluginSetting();
		setting.bulletId = bulletId;
		setting.maxStack = maxStack;
		BulletClipSetting::updateBulletClipSetting(setting);
		m_bulletClipPluginSetting.push_back(std::move(setting));
	}

	void LogicSetting::setBlockDynamicAttr(vector<BlockDynamicAttr>::type blockDynamicAttr)
	{
		m_blockDynamicAttr.clear();
		for (auto & attr : blockDynamicAttr)
		{
			addBlockDynamicAttr(attr.blockId, attr.hardness);
		}
	}

	void LogicSetting::addBlockDynamicAttr(int blockId, float hardness)
	{
		Block* block = BlockManager::sBlocks[blockId];
		if (!block)
			return;
		block->setHardness(hardness);
		BlockDynamicAttr attr = BlockDynamicAttr();
		attr.blockId = blockId;
		attr.hardness = hardness;
		m_blockDynamicAttr.push_back(std::move(attr));
	}

	void LogicSetting::setToolItemDynamicAttr(vector<ToolItemDynamicAttr>::type toolItemDynamicAttr)
	{
		m_toolItemDynamicAttr.clear();
		for (auto & attr : toolItemDynamicAttr)
		{
			addToolItemDynamicAttr(attr.itemId, attr.distance, attr.efficiency);
		}
	}

	void LogicSetting::addToolItemDynamicAttr(int itemId, float distance, float efficiency)
	{
		if (Item::isBlockItem(itemId))
			return;
		Item* item = Item::getItemById(itemId);
		if (!item)
			return;
		ItemTool* tool = dynamic_cast<ItemTool*>(item);
		if (!tool)
			return;
		tool->setEfficiency(efficiency);
		tool->setEffectiveDistance(distance);
		ToolItemDynamicAttr attr = ToolItemDynamicAttr();
		attr.itemId = itemId;
		attr.distance = distance;
		attr.efficiency = efficiency;
		m_toolItemDynamicAttr.push_back(std::move(attr));
	}

	void LogicSetting::addMonsterSetting(MonsterSetting setting)
	{
		MonsterSetting::addMonsterSetting(setting);
	}

	void LogicSetting::addSkillEffect(SkillEffect effect)
	{
		SkillSetting::addSkillEffect(effect);
	}

	void LogicSetting::addSkillItem(SkillItem skillItem)
	{
		SkillItemSetting::addSkillItem(skillItem);
		m_skillItemSetting.push_back(std::move(skillItem));
	}

	void LogicSetting::setSkillItemSetting(vector<SkillItem>::type skillItemSetting)
	{
		m_skillItemSetting.clear();
		for (auto & setting : skillItemSetting)
		{
			SkillItemSetting::addSkillItem(setting);
		}
	}

	SkillItem * LogicSetting::getSkillItem(int itemId, int Occupation)
	{
		return SkillItemSetting::getSkillItem(itemId, Occupation);
	}

	SkillItem * LogicSetting::getSkillItem(int skillId)
	{
		return SkillItemSetting::getSkillItem(skillId);
	}

}