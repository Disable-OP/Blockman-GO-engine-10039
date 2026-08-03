#ifndef __CUSTOM_BLOCK_CLIENT_HEADER__
#define __CUSTOM_BLOCK_CLIENT_HEADER__

#include "cBlock/cBlock.h"
#include "Block/CustomBlock.h"

namespace BLOCKMAN
{
	class CustomBlockClient : public CustomBlock, public cBlock
	{
	public:
		CustomBlockClient(int id, const String &nameSpace, const String &name, const BlockConfig& config);
		void setTextures(const BlockConfig& config);
		virtual AtlasSprite* getIcon(int side, int meta) override;
		virtual int getRenderBlockPass() override;
		virtual bool shouldSideBeRendered(IBlockAccess* pBlockAccess, const BlockPos& pos, int side) override;
		virtual ui32 colorMultiplier(IBlockAccess* piBlockAccess, const BlockPos& pos) override;

	private:
		struct
		{
			AtlasSprite* top;
			AtlasSprite* bottom;
			AtlasSprite* left;
			AtlasSprite* right;
			AtlasSprite* front;
			AtlasSprite* back;
		} m_texture;
		bool m_transparent = false;
		LORD::Color m_color = LORD::Color::WHITE;

		AtlasSprite* getSprite(const LORD::String& textureString);
	};
}

#endif // !__CUSTOM_BLOCK_CLIENT_HEADER__
