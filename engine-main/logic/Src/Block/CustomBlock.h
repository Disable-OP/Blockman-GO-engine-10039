#ifndef __CUSTOM_BLOCK_HEADER__
#define __CUSTOM_BLOCK_HEADER__

#include "Block.h"
#include "BlockConfig.h"
#include "Item/Item.h"

namespace BLOCKMAN
{
	class CustomBlock : public Block
	{
	public:
		static const vector<CustomBlock*>::type& getCustomBlockList()
		{
			return customBlockList;
		}

		CustomBlock(int id, const String &nameSpace, const String &name, const BlockConfig& config);
		~CustomBlock();
		int idDropped(int meta, Random& rand, int fortune) override;

		String getNamespace() const { return m_namespace; }
		String getFullName() const { return m_namespace + ":" + m_blockName; }
		virtual bool renderAsNormalBlock() const override;
		virtual bool isCollidable() const override;
		virtual int quantityDropped(Random& rand) override;
		virtual Box getCollisionBoundingBox(World* pWorld, const BlockPos& pos) override;
		virtual bool onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit);

	private:
		static vector<CustomBlock*>::type customBlockList;

	protected:
		String m_namespace; // namespace
		BlockState m_state = BlockState::SOLID;
		LORD::Box m_collisionBox = LORD::Box({ 0,0,0 }, { 1,1,1 });
		bool m_clickable = false;
	};
}

#endif // !__CUSTOM_BLOCK_HEADER__
