#include "BlockLoader.h"
#include <regex>
#include "Block/BlockConfig.h"
#include "Block/CustomBlock.h"
#include "Block/BlockManager.h"
#include "Item/Items.h"
#include "Misc/IdMapping.h"

#define RETURN_ON_FAIL(expr, msg, ...) if(!(expr)) { LordLogError(msg, ##__VA_ARGS__); return false; }

using namespace LORD;

namespace BLOCKMAN
{
	BlockLoader::BlockLoader()
	{
		m_blockArchiveFactory = LORD::make_unique<BlockArchiveFactory>();
		ArchiveManager::Instance()->addArchiveFactory(m_blockArchiveFactory.get());
	}

	void BlockLoader::loadCustomBlock(int blockId, const String & blockDir)
	{
		BlockConfig config;
		if (!readConfig(blockDir, config))
		{
			return;
		}
		loadBlockResources(blockDir, config);
		auto block = createBlock(blockId, getPluginName(blockDir), getBlockName(blockDir), config);
		prepareBlock(block, config);
	}

	void BlockLoader::loadCustomBlocksInMap(const String & mapPath)
	{
		StringArray pluginNameList;
		PathUtil::EnumSubDirs(pluginNameList, PathUtil::ConcatPath(mapPath, "plugins"));
		for (const auto& pluginDir : pluginNameList)
		{
			StringArray blockDirList;
			PathUtil::EnumSubDirs(blockDirList, PathUtil::ConcatPath(pluginDir, "blocks"));
			for (const auto& blockDir : blockDirList)
			{
				auto components = StringUtil::Split(blockDir, "/\\");
				String blockFullname = components[components.size() - 3] + ":" + components[components.size() - 1];
				int blockId = IdMapping::getBlockId(blockFullname.c_str());
				if (blockId > 0)
				{
					loadCustomBlock(blockId, blockDir);
				}
			}
		}

		for (auto block : CustomBlock::getCustomBlockList())
		{
			block->initializeBlock();
		}
	}

	String BlockLoader::getPluginName(const String & blockDir) const
	{
		const auto& components = StringUtil::Split(PathUtil::GetAbsolutePath(blockDir), "/\\");
		return components[components.size() - 3];
	}

	String BlockLoader::getBlockName(const String & blockDir) const
	{
		const auto& components = StringUtil::Split(PathUtil::GetAbsolutePath(blockDir), "/\\");
		return components[components.size() - 1];
	}

	void BlockLoader::prepareBlock(CustomBlock * block, const BlockConfig & config)
	{
		int blockId = block->getBlockID();
		int itemId = block->getItemBlockId();
		if (itemId < 0 || itemId >= Item::MAX_ITEM_COUNT)
			return;

		if (Item::itemsList[itemId] == NULL)
		{
			Item::itemsList[itemId] = createItemBlock(itemId);
		}

		bool useNeighborBrightness = false;

		if (BlockManager::sCanBlockGrass[blockId])
		{
			useNeighborBrightness = true;
		}

		if (BlockManager::sLightOpacity[blockId] == 0)
		{
			useNeighborBrightness = true;
		}

		BlockManager::sUseNeighborBrightness[blockId] = useNeighborBrightness;

		Item::itemNameToIdMap[block->getFullName()] = block->getItemBlockId();
	}

	CustomBlock * BlockLoader::createBlock(int blockId, const LORD::String & pluginName, const LORD::String & blockName, const BlockConfig & config)
	{
		return LordNew CustomBlock(blockId, pluginName, blockName, config);
	}

	ItemBlock * BlockLoader::createItemBlock(int itemId)
	{
		return LordNew ItemBlock(itemId - 256);
	}

	bool BlockLoader::readConfig(const String & blockDir, BlockConfig & config)
	{
		String blockName = getBlockName(blockDir);
		String pluginName = getPluginName(blockDir);

		static std::regex blockNameRegex("[a-z0-9_]+");
		RETURN_ON_FAIL(std::regex_match(blockName, blockNameRegex), 
			"Failed to load custom block [%s:%s]: block name must contains number, lowercase letters and underscore only: %s",
			pluginName.c_str(), blockName.c_str(), blockName.c_str());
		RETURN_ON_FAIL(std::regex_match(pluginName, blockNameRegex),
			"Failed to load custom block [%s:%s]: block namespace must contains number, lowercase letters and underscore only: %s",
			pluginName.c_str(), blockName.c_str(), pluginName.c_str());
		RETURN_ON_FAIL(BlockConfig::readJson(config, PathUtil::ConcatPath(blockDir, "block.json")),
			"Failed to load custom block [%s:%s]: failed to read block.json",
			pluginName.c_str(), blockName.c_str());
		return true;
	}
}
