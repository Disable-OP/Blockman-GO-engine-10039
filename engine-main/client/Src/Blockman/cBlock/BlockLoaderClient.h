#ifndef __BLOCK_LOADER_CLIENT_HEADER__
#define __BLOCK_LOADER_CLIENT_HEADER__

#include "Block/BlockLoader.h"

namespace BLOCKMAN
{
	class BlockLoaderClient : public BlockLoader
	{
	public:
		virtual void loadBlockResources(const LORD::String& blockDir, const BlockConfig &config) override;
		virtual void prepareBlock(CustomBlock* block, const BlockConfig &config) override;

	private:
		virtual CustomBlock* createBlock(int blockId, const LORD::String &pluginName, const LORD::String &blockName, const BlockConfig &config) override;
		virtual ItemBlock* createItemBlock(int itemId) override;
	};
}

#endif // !__BLOCK_LOADER_CLIENT_HEADER__
