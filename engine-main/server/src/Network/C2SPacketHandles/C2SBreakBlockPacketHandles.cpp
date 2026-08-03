#include "C2SBreakBlockPacketHandles.h"
#include "Network/ClientPeer.h"
#include "Server.h"
#include "Block/BlockManager.h"
#include "Block/Block.h"
#include "Item/ItemStack.h"
#include "Script/GameServerEvents.h"
#include "Block/Blocks.h"
#include "Network/protocol/S2CPackets.h"
#include "Network/ServerNetwork.h"
#include "TileEntity/TileEntitys.h"

using std::chrono::steady_clock;

map<int, C2SBreakBlockPacketHandles::BreakBlockRecord>::type C2SBreakBlockPacketHandles::breakBlockRecords;

void C2SBreakBlockPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketStartBreakBlock>& packet)
{
	auto startTime = steady_clock::now();
	int id = clientPeer->getEntityRuntimeID();
	auto iter = breakBlockRecords.find(id);
	clientPeer->getEntityPlayer()->world->extinguishFire(clientPeer->getEntityPlayer(), BlockPos((int)packet->x, (int)packet->y, (int)packet->z), BM_FACE_UP);
	
	if (iter != breakBlockRecords.end())
	{
		// TODO: return failure packet
		LordLogWarning("C2SPacketStartBreakBlock is received more than once");
		return;
	}
	// TODO: check distance is valid or not
	auto world = Server::Instance()->getWorld();
	Vector3i position = Vector3i((int)packet->x, (int)packet->y, (int)packet->z);
	auto blockId = world->getBlockId(position);
	SCRIPT_EVENT::BlockStartBreakEvent::invoke(clientPeer->getRakssid(), blockId, position);
	breakBlockRecords.insert({ id, {startTime, {(int)packet->x, (int)packet->y, (int)packet->z}} });
}

void C2SBreakBlockPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketAbortBreakBlock>& packet)
{
	int id = clientPeer->getEntityRuntimeID();
	auto iter = breakBlockRecords.find(id);
	if (iter == breakBlockRecords.end())
	{
		ServerNetwork::Instance()->getSender()->sendBlockDestroyeFailure(clientPeer->getRakssid(), false, NOT_BREAKING);
		return;
	}
	auto world = Server::Instance()->getWorld();
	auto blockId = world->getBlockId(iter->second.position);
	SCRIPT_EVENT::BlockAbortBreakEvent::invoke(clientPeer->getRakssid(), blockId, iter->second.position);
	breakBlockRecords.erase(iter);
}

bool destroyBedBlock(EntityPlayerMP* player, const Vector3i& position, ServerWorld* world, int blockId, int meta, Block* block)
{
	if (blockId == BLOCK_ID_BED)
	{
		auto blockbed = (dynamic_cast<BlockBed*>(block));
		if (blockbed != nullptr)
		{
			auto pos = blockbed->getOtherSidePos(position, meta);
			auto nextid = world->getBlockId(pos);

			int itemId = 0;
			int currentStackIndex = 0;
			if (player && player->inventory && player->inventory->getCurrentItem())
			{
				itemId = player->inventory->getCurrentItem()->itemID;
				currentStackIndex = player->inventory->currentItemIndex;
			}

			if (!SCRIPT_EVENT::BlockBreakEvent::invoke(player->getRaknetID(), blockId, pos) || !SCRIPT_EVENT::BlockBreakWithMetaEvent::invoke(player->getRaknetID(), blockId, meta, position, itemId, currentStackIndex))
			{
				// ½ûÖ¹ÆÆ»µ·½¿é
				auto sendPk = LORD::make_shared<S2CPacketUpdateBlocks>();
				sendPk->updates.reserve(1);

				sendPk->updates.emplace_back(position, blockId, meta);


				Server::Instance()->getNetwork()->sendPacket(sendPk, player->getRaknetID(), true);
			}
		}
	}

	return true;
}

bool canDestroyBlock(EntityPlayerMP* player, const Vector3i& position, ServerWorld* world, int blockId, int meta, Block* block)
{
	if (!block)
	{
		// TODO: error
		LordLogError("block does not exists");
		return false;
	}

	int itemId = 0;
	int currentStackIndex = 0;
	if (player && player->inventory && player->inventory->getCurrentItem())
	{
		itemId = player->inventory->getCurrentItem()->itemID;
		currentStackIndex = player->inventory->currentItemIndex;
	}

	if (!SCRIPT_EVENT::BlockBreakEvent::invoke(player->getRaknetID(), blockId, position) || !SCRIPT_EVENT::BlockBreakWithMetaEvent::invoke(player->getRaknetID(), blockId, meta, position, itemId, currentStackIndex))
	{
		// ½ûÖ¹ÆÆ»µ·½¿é
		auto sendPk = LORD::make_shared<S2CPacketUpdateBlocks>();
		sendPk->updates.reserve(1);
		sendPk->updates.emplace_back(position, blockId, meta);
		Server::Instance()->getNetwork()->sendPacket(sendPk, player->getRaknetID(), true);
		if (TileEntitySign* entity = dynamic_cast<TileEntitySign*>(world->getBlockTileEntity(position)))
		{
			Server::Instance()->getNetwork()->getSender()->sendSetSignTexts(entity, player->getRaknetID());
		}
		return false;
	}

	return true;
}

bool destroyBlockCrops(EntityPlayerMP* player, const Vector3i& position, ServerWorld* world, int blockId, int meta, Block* block)
{
	if (dynamic_cast<BlockNewCrops*>(block))
	{
		BlockPos tempPos = position;
		bool bRet = Server::Instance()->getBlockCropsManager()->pickCrops(player->getPlatformUserId(), tempPos);
		if (!bRet)
		{
			auto sendPk = LORD::make_shared<S2CPacketUpdateBlocks>();
			sendPk->updates.reserve(1);
			sendPk->updates.emplace_back(position, blockId, meta);
			Server::Instance()->getNetwork()->sendPacket(sendPk, player->getRaknetID(), true);
		}

		return false;
	}

	return true;
}

bool destroyBlockFruits(EntityPlayerMP* player, const Vector3i& position, ServerWorld* world, int blockId, int meta, Block* block)
{
	if (dynamic_cast<BlockFruits*>(block))
	{
		BlockPos tempPos = position;
		bool bRet = Server::Instance()->getBlockFruitsManager()->onGather(player->getPlatformUserId(), 0, tempPos);
		if (!bRet)
		{
			auto sendPk = LORD::make_shared<S2CPacketUpdateBlocks>();
			sendPk->updates.reserve(1);
			sendPk->updates.emplace_back(position, blockId, meta);
			Server::Instance()->getNetwork()->sendPacket(sendPk, player->getRaknetID(), true);
		}
		return false;
	}

	return true;
}

void harvestDestroyBlock(EntityPlayerMP* player, const Vector3i& position, ServerWorld* world, int blockId, int meta, Block* block)
{
	block->onBlockHarvested(world, position, meta, player);
	block->onBlockDestroyedByPlayer(world, position, meta);
	ItemStackPtr pStack = player->getCurrentEquippedItem();
	bool canHarvest = player->canHarvestBlock(BlockManager::sBlocks[blockId]);
	bool removeResult = Server::Instance()->getWorld()->setBlockToAir(position);
	if (pStack)
	{
		pStack->onBlockDestroyed(world, blockId, position, player);

		if (pStack->stackSize == 0)
		{
			player->destroyCurrentEquippedItem();
		}
	}

	if (removeResult && canHarvest && blockId != BLOCK_ID_BED)
	{
		BlockManager::sBlocks[blockId]->harvestBlock(world, player, position, meta);
	}
}

void destoryBlock(EntityPlayerMP* player, const Vector3i& position)
{
	auto world = Server::Instance()->getWorld();
	auto blockId = world->getBlockId(position);
	auto meta = world->getBlockMeta(position);
	auto block = BlockManager::getBlockById(blockId);

	if (!canDestroyBlock(player, position, world, blockId, meta, block))
		return;

	if (!destroyBedBlock(player, position, world, blockId, meta, block))
		return;

	if (!destroyBlockCrops(player, position, world, blockId, meta, block))
		return;

	if (!destroyBlockFruits(player, position, world, blockId, meta, block))
		return;

	harvestDestroyBlock(player, position, world, blockId, meta, block);
}

void C2SBreakBlockPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketFinishBreakBlock>& packet)
{
	using namespace std::chrono_literals;
	auto endTime = steady_clock::now();
	int id = clientPeer->getEntityRuntimeID();
	auto iter = breakBlockRecords.find(id);
	if (iter == breakBlockRecords.end())
	{
		LordLogWarning("C2SPacketFinishBreakBlock is received but the player is not breaking a block");
		ServerNetwork::Instance()->getSender()->sendBlockDestroyeFailure(clientPeer->getRakssid(), false, NOT_BREAKING);
		return;
	}
	// TODO: check the duration of breaking time more accurately
	auto duration = endTime - iter->second.startTime;
	if (duration < 10ms)
	{
		LordLogWarning("breaking time is too short: %d milliseconds", std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());
		ServerNetwork::Instance()->getSender()->sendBlockDestroyeFailure(clientPeer->getRakssid(),false, TIME_SHORT);
		//return;
	}
	// TODO: check player position. 
	// Actually, server should track the player's position and cancel the breaking immediately when the player go too far away from the target block
	destoryBlock(clientPeer->getEntityPlayer(), iter->second.position);
	breakBlockRecords.erase(iter);
}
