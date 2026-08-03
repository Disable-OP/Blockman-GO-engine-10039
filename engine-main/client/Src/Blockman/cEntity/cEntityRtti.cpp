#include "Entity/Entity.h"
#include "Entity/EntityArrow.h"
#include "Entity/EntityBullet.h"
#include "Entity/EntityThrowable.h"
#include "Entity/EntityPotion.h"
#include "Entity/EntityXPOrb.h"
#include "Entity/EntityLivingBase.h"
#include "Entity/EntityItem.h"
#include "Entity/EntityLiving.h"
#include "Entity/EntityCreature.h"
#include "Entity/EntityPlayer.h"
#include "Entity/EntityCreature.h"
#include "EntityOtherPlayerMP.h"
#include "EntityPlayerSP.h"
#include "EntityPlayerSPMP.h"
#include "Entity/EntityThrowable.h"
#include "Entity/EntitySnowball.h"
#include "Entity/EntityTNTPrimed.h"
#include "Entity/EntityTNTThrowable.h"
#include "Entity/EntityMerchant.h"
#include "Entity/EntityGrenade.h"
#include "Entity/EntityVehicle.h"
#include "Entity/EntityRankNpc.h"
#include "Entity/EntityAircraft.h"
#include "Entity/EntityActorNpc.h"
#include "Entity/EntitySessionNpc.h"
#include "Entity/EntityCreatureBullet.h"
#include "Entity/EntitySkillThrowable.h"
#include "Entity/EntityFireball.h"
#include "Entity/EntityBlockman.h"
#include "Entity/EntityBlockmanEmpty.h"
#include "Entity/EntityFishHook.h"
#include "EntityFishHookClient.h"
#include "Entity/EntityActorCannon.h"
#include "EntityActorCannonClient.h"
#include "Entity/EntityBuildNpc.h"
#include "Entity/EntityLandNpc.h"
#include "EntityGunFlame.h"
#include "Entity/EntityBulletin.h"
#include "Entity/EntityBird.h"


namespace BLOCKMAN
{

RTTI_IMPLEMENT_BASE(Entity, ENTITY_CLASS_ENTITY)
RTTI_IMPLEMENT(EntityArrow, ENTITY_CLASS_ARROW, Entity)
RTTI_IMPLEMENT(EntityBullet, ENTITY_CLASS_BULLET, Entity)
RTTI_IMPLEMENT(EntityItem, ENTITY_CLASS_ITEM, Entity)
RTTI_IMPLEMENT(EntityXPOrb, ENTITY_CLASS_XPORB, Entity)
RTTI_IMPLEMENT(EntityLivingBase, ENTITY_CLASS_LIVING_BASE, Entity)
RTTI_IMPLEMENT(EntityLiving, ENTITY_CLASS_LIVING, EntityLivingBase)
RTTI_IMPLEMENT(EntityPlayer, ENTITY_CLASS_PLAYER, EntityLivingBase)
RTTI_IMPLEMENT(EntityOtherPlayerMP, ENTITY_CLASS_PLAYER_OTHERMP, EntityPlayer)
RTTI_IMPLEMENT(EntityPlayerSP, ENTITY_CLASS_PLAYER_SP, EntityPlayer)
RTTI_IMPLEMENT(EntityCreature, ENTITY_CLASS_CREATURE, EntityLiving)
RTTI_IMPLEMENT(EntityPlayerSPMP, ENTITY_CLASS_PLAYER_SPMP, EntityPlayerSP)
RTTI_IMPLEMENT(EntityThrowable, ENTITY_CLASS_THROWABLE, Entity)
RTTI_IMPLEMENT(EntityPotion, ENTITY_CLASS_POTION, EntityThrowable)
RTTI_IMPLEMENT(EntitySnowball, ENTITY_CLASS_SNOWBALL, EntityThrowable)
RTTI_IMPLEMENT(EntityTNTPrimed, ENTITY_CLASS_TNT_PRIMED, Entity)
RTTI_IMPLEMENT(EntityTNTThrowable, ENTITY_CLASS_TNT_THROWABLE, EntityThrowable)
RTTI_IMPLEMENT(EntityMerchant, ENTITY_CLASS_MERCHANT, Entity)
RTTI_IMPLEMENT(EntityGrenade, ENTITY_CLASS_GRENADE, EntityThrowable)
RTTI_IMPLEMENT(EntityVehicle, ENTITY_CLASS_VEHICLE, Entity)
RTTI_IMPLEMENT(EntityRankNpc, ENTITY_CLASS_RANK_NPC, Entity)
RTTI_IMPLEMENT(EntityAircraft, ENTITY_CLASS_AIRCRAFT, Entity)
RTTI_IMPLEMENT(EntityActorNpc, ENTITY_CLASS_ACTOR_NPC, Entity)
RTTI_IMPLEMENT(EntitySessionNpc, ENTITY_CLASS_SESSION_NPC, Entity)
RTTI_IMPLEMENT(EntityCreatureBullet, ENTITY_CLASS_CREATURE_BULLET, Entity)
RTTI_IMPLEMENT(EntitySkillThrowable, ENTITY_CLASS_ITEM_SKILL, Entity)
RTTI_IMPLEMENT(EntityFireball, ENTITY_CLASS_FIRE_BALL, EntityThrowable)
RTTI_IMPLEMENT(EntityBlockman, ENTITY_CLASS_BLOCKMAN, Entity)
RTTI_IMPLEMENT(EntityBlockmanEmpty, ENTITY_CLASS_BLOCKMAN_EMPTY, EntityBlockman)
RTTI_IMPLEMENT(EntityFishHook, ENTITY_CLASS_FISHHOOK, Entity)
RTTI_IMPLEMENT(EntityFishHookClient, ENTITY_CLASS_FISHHOOK_CLIENT, EntityFishHook)
RTTI_IMPLEMENT(EntityActorCannon, ENTITY_CLASS_ACTOR_CANNON, Entity)
RTTI_IMPLEMENT(EntityActorCannonClient, ENTITY_CLASS_ACTOR_CANNON_CLIENT, EntityActorCannon)
RTTI_IMPLEMENT(EntityBuildNpc, ENTITY_CLASS_BUILD_NPC, Entity)
RTTI_IMPLEMENT(EntityLandNpc, ENTITY_CLASS_LAND_NPC, Entity)
RTTI_IMPLEMENT(EntityGunFlame, ENTITY_CLASS_GUN_FLAME, Entity)
RTTI_IMPLEMENT(EntityBulletin, ENTITY_CLASS_BULLETIN, Entity)
RTTI_IMPLEMENT(EntityBird, ENTITY_CLASS_BIRD, EntityLiving)

}
