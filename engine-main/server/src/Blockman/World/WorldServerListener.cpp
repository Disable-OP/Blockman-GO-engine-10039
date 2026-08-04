#include "WorldServerListener.h"
#include "ServerWorld.h"
#include "Blockman/Entity/EntityTracker.h"
#include "Network/ServerNetwork.h"
#include "Server.h"
#include "Blockman/Entity/EntityPlayerMP.h"

namespace BLOCKMAN
{

WorldServerListener::WorldServerListener(ServerWorld* pWorld)
{
	theWorldServer = pWorld;
}

void WorldServerListener::onEntityCreate(Entity* pEntity)
{
	EntityTracker::Instance()->addEntityToTracker(pEntity);
}

void WorldServerListener::onEntityDestroy(Entity* pEntity)
{
	EntityTracker::Instance()->removeEntityFromAllTrackingPlayers(pEntity);
}

void WorldServerListener::playSound(const Vector3& pos, SoundType soundType)
{
	// todo.
	// this.mcServer.getConfigurationManager().sendToAllNear(par2, par4, par6, par8 > 1.0F ? (double)(16.0F * par8) : 16.0D, this.theWorldServer.provider.dimensionId, new Packet62LevelSound(name, par2, par4, par6, par8, par9));
}

void WorldServerListener::playSoundToNearExcept(EntityPlayer* exceptPlayer, const Vector3& pos, SoundType soundType)
{
	// todo.
	// this.mcServer.getConfigurationManager().sendToAllNearExcept(par1EntityPlayer, par3, par5, par7, par9 > 1.0F ? (double)(16.0F * par9) : 16.0D, this.theWorldServer.provider.dimensionId, new Packet62LevelSound(par2Str, par3, par5, par7, par9, par10));
}

void WorldServerListener::markBlockForUpdate(const BlockPos& pos)
{
	// todo.
	// this.theWorldServer.getPlayerManager().markBlockForUpdate(par1, par2, par3);
}

void WorldServerListener::playAuxSFX(EntityPlayer* player, int type, const BlockPos& pos, int data)
{
	// todo.
	// this.mcServer.getConfigurationManager().sendToAllNearExcept(par1EntityPlayer, (double)par3, (double)par4, (double)par5, 64.0D, this.theWorldServer.provider.dimensionId, new Packet61DoorChange(par2, par3, par4, par5, par6, false));
}

void WorldServerListener::broadcastSound(int soundID, const BlockPos& pos, int data)
{
	// todo.
	// this.mcServer.getConfigurationManager().sendPacketToAllPlayers(new Packet61DoorChange(par1, par2, par3, par4, par5, true));
}

void WorldServerListener::destroyBlockPartially(int breakerID, const BlockPos& pos, int progress)
{
	// todo.
	/*Iterator var6 = this.mcServer.getConfigurationManager().playerEntityList.iterator();

	while (var6.hasNext())
	{
		EntityPlayerMP var7 = (EntityPlayerMP)var6.next();

		if (var7 != null && var7.worldObj == this.theWorldServer && var7.entityId != par1)
		{
			double var8 = (double)par2 - var7.posX;
			double var10 = (double)par3 - var7.posY;
			double var12 = (double)par4 - var7.posZ;

			if (var8 * var8 + var10 * var10 + var12 * var12 < 1024.0D)
			{
				var7.playerNetServerHandler.sendPacketToPlayer(new Packet55BlockDestroy(par1, par2, par3, par4, par5));
			}
		}
	}*/
}

void WorldServerListener::onPlayerChangeActor(EntityPlayer* pPlayer)
{
	ServerNetwork::Instance()->getSender()->broadCastPlayerChangeActor(pPlayer);
}

void WorldServerListener::onEntityChangeActor(Entity * pEntity, const String & actorName)
{
}

void WorldServerListener::onPlayerRestoreActor(Entity * pEntity)
{
	ServerNetwork::Instance()->getSender()->broadCastPlayerRestoreActor(pEntity->entityId);
}

}
