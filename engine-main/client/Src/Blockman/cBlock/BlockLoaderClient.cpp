#include "BlockLoaderClient.h"
#include "cBlock/CustomBlockClient.h"
#include "cBlock/cBlockManager.h"
#include "cItem/cItem.h"
#include "Render/TextureAtlas.h"

using namespace LORD;

namespace BLOCKMAN
{
	void BlockLoaderClient::loadBlockResources(const String & blockDir, const BlockConfig & config)
	{
		ResourceGroupManager::Instance()->addResourceLocation(blockDir, "Block");
		if (PathUtil::IsFileExist(PathUtil::ConcatPath(blockDir, "texture.json")))
		{
			auto pluginName = getPluginName(blockDir);
			auto blockName = getBlockName(blockDir);
			TextureAtlasRegister::Instance()->addTextureAtlas(
				"plugin/" + pluginName + "/block/" + blockName + "/texture.json",
				"plugin:" + pluginName + ":block:" + blockName);
		}
	}

	void BlockLoaderClient::prepareBlock(CustomBlock * block, const BlockConfig & config)
	{
		BlockLoader::prepareBlock(block, config);
		auto cblock = dynamic_cast<CustomBlockClient*>(block);
		int blockId = cblock->getBlockID();
		cBlockManager::scBlocks[blockId] = cblock;
		cblock->setStepSound(&cBlock::soundStoneFootstep);
		int itemId = cblock->getItemBlockId();
		auto itemBlock = dynamic_cast<cItem*>(Item::itemsList[itemId]);
		ItemClient::citemsList[itemId] = itemBlock;
		itemBlock->registerIcons(TextureAtlasRegister::Instance());
	}

	CustomBlock * BlockLoaderClient::createBlock(int blockId, const String & pluginName, const String & blockName, const BlockConfig & config)
	{
		return LordNew CustomBlockClient(blockId, pluginName, blockName, config);
	}

	ItemBlock * BlockLoaderClient::createItemBlock(int itemId)
	{
		return LordNew cItemBlockImp(itemId - 256);
	}
}