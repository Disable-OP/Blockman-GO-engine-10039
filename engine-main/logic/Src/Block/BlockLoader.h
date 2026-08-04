#ifndef __BLOCK_LOADER_HEADER__
#define __BLOCK_LOADER_HEADER__

#include "Core.h"
#include "Block/BlockArchive.h"

namespace BLOCKMAN
{
	class CustomBlock;
	struct BlockConfig;
	class ItemBlock;

	class BlockLoader : public LORD::ObjectAlloc, public LORD::Singleton<BlockLoader>
	{
	public:
		BlockLoader();
		virtual ~BlockLoader() = default;
		void loadCustomBlock(int blockId, const LORD::String& blockDir);
		void loadCustomBlocksInMap(const LORD::String& mapPath);

	protected:
		LORD::String getPluginName(const LORD::String& blockDir) const;
		LORD::String getBlockName(const LORD::String& blockDir) const;
		virtual void prepareBlock(CustomBlock* block, const BlockConfig& config);
		virtual void loadBlockResources(const LORD::String& blockDir, const BlockConfig& config) {}

	private:
		std::unique_ptr<BlockArchiveFactory> m_blockArchiveFactory;

		virtual CustomBlock* createBlock(int blockId, const LORD::String &pluginName, const LORD::String &blockName, const BlockConfig &config);
		virtual ItemBlock* createItemBlock(int itemId);
		bool readConfig(const LORD::String & blockDir, BlockConfig& config);
	};
}

#endif // !__BLOCK_LOADER_HEADER__
