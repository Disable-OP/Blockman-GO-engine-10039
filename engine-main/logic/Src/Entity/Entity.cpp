#include "Entity.h"
#include "DamageSource.h"
#include "Enchantment.h"
#include "EntityPlayer.h"

#include "Block/BM_Material.h"
#include "Util/Random.h"
#include "World/World.h"
#include "World/WorldProvider.h"
#include "Block/Block.h"
#include "Block/Blocks.h"
#include "Block/BlockManager.h"
#include "Item/ItemStack.h"
#include "Entity/EntityItem.h"
#include "Common.h"
#include "Network/protocol/S2CPackets.h"
#include "Script/Event/LogicScriptEvents.h"
namespace BLOCKMAN
{

int Entity::nextEntityID = 1;

int Entity::getSuperClassID(int classID)
{
	deque<const RuntimeClass*>::type searchQueue;
	searchQueue.push_back(&rttiEntity);
	while (!searchQueue.empty())
	{
		const RuntimeClass* rtti = searchQueue.front();
		searchQueue.pop_front();
		if (rtti->m_classID == classID)
		{
			if (rtti->m_parent)
				return rtti->m_parent->m_classID;
			return 0;
		}

		for (int i = 0; i < int(rtti->m_children.size()); ++i)
			searchQueue.push_back(rtti->m_children[i]);
	}
	return 0;
}

Entity::Entity(World* pWorld)
{
	world = pWorld;

	if (nextEntityID <= 0) {
		nextEntityID = 1;
	}

	entityId = nextEntityID++;
	entityUniqueID = UUID::randomUUID();

	doConstruct();
}

Entity::~Entity()
{
	doDestruct();
}

void Entity::doDestruct()
{
	LordSafeDelete(rand);
}

void Entity::doConstruct()
{
	init();

	rand = LordNew Random();
	boundingBox = Box(0, 0, 0, 0, 0, 0);
	width = 0.6F;
	length = 0.6F;
	height = 1.8F;

	fire = 0;
	firstUpdate = true;
	setPosition(Vector3::ZERO);

	if (world != NULL) {
		dimension = world->m_provider->dimensionId;
	}

	entityInit();
}

void Entity::init()
{
	entityType = 1;

	isInWeb = false;
	nextStepDistance = 1;
	inWater = false;
	m_isImmuneToFire = false;
	entityRiderPitchDelta = 0.f;
	entityRiderYawDelta = 0.f;
	invulnerable = false;

	renderDistanceWeight = 1.f;
	preventEntitySpawning = false;
	riddenByEntity = NULL;
	ridingEntity = NULL;
	forceSpawn = false;
	prevPos = Vector3::ZERO;
	position = Vector3::ZERO;
	motion = Vector3::ZERO;
	rotationYaw = 0.f;
	rotationPitch = 0.f;
	prevRotationYaw = 0.f;
	prevRotationPitch = 0.f;
	onGround = false;
	isCollidedHorizontally = false;
	isCollidedVertically = false;
	isCollided = false;
	beenAttacked = false;
	isDead = false;
	yOffset = 0.f;
	width = 0.f;
	length = 0.f;
	height = 0.f;
	prevDistanceWalkedModified = 0.f;
	distanceWalkedModified = 0.f;
	distanceWalkedOnStepModified = 0.f;
	fallDistance = 0.f;
	lastTickPos = Vector3::ZERO;
	yPositionDecrement = 0.f;
	stepHeight = 0.f;
	noClip = false;
	entityCollisionReduction = 0.f;
	ticksExisted = 0;
	fireResistance = 0;
	hurtResistantTime = 0;
	addedToChunk = false;
	chunkCoord = Vector3i::ZERO;
	serverPos = Vector3i::ZERO;
	ignoreFrustumCheck = false;
	isAirBorne = false;
	timeUntilPortal = 0;
	dimension = 0;
	myEntitySize = 0;
	m_flags.clear();
	m_air = 0;
}

void Entity::setSize(float w, float h, Vector3 scale, bool force)
{
	setSize(w, w, h, scale, force);
}

void Entity::setSize(float w, float l, float h, Vector3 scale, bool force)
{
	float w1;
	if (w != width || l != length || h != height || force)
	{
		w1 = width;
		width = w * scale.x;
		length = l * scale.z;
		height = h * scale.y;
		boundingBox.setExtents(boundingBox.vMin, boundingBox.vMin + Vector3(width, height, width));
		if (width > w1 && !firstUpdate && !world->m_isClient)
		{
			moveEntity(Vector3(w1 - width, 0.0f, w1 - width));
		}
	}
	w1 = Math::Mod(w, 2.0f);
	if (w1 < 0.375f)
		myEntitySize = ENTITY_SIZE_1;
	else if (w1 < 0.75f)
		myEntitySize = ENTITY_SIZE_2;
	else if (w1 < 1.0f)
		myEntitySize = ENTITY_SIZE_3;
	else if (w1 < 1.375f)
		myEntitySize = ENTITY_SIZE_4;
	else if (w1 < 1.75f)
		myEntitySize = ENTITY_SIZE_5;
	else
		myEntitySize = ENTITY_SIZE_6;
}

void Entity::preparePlayerToSpawn()
{
	if (world == NULL)
		return;

	while (position.y > 0.f)
	{
		setPosition(position);

		if (world->getCollidingBoundingBoxes(this, boundingBox).empty())
		{
			break;
		}

		position.y += 1.f;
	}

	motion = Vector3::ZERO;
	rotationPitch = 0.0F;
}

void Entity::setRotation(float yaw, float pitch)
{
	// rotationYaw = Math::Mod(yaw, 360.f);
	// rotationPitch = Math::Mod(pitch, 360.f);
	rotationYaw = yaw;
	rotationPitch = pitch;
}

void Entity::setPosition(const Vector3& pos)
{
	position = pos;
	float wh = width / 2.0F;
	boundingBox.setExtents(pos.x - wh, pos.y - yOffset + yPositionDecrement, pos.z - wh, pos.x + wh, pos.y - yOffset + yPositionDecrement + height, pos.z + wh);
}

void Entity::setAngles(float yaw, float pitch)
{
	float p = rotationPitch;
	float y = rotationYaw;
	rotationYaw = rotationYaw + yaw * 0.15f;
	rotationPitch = rotationPitch - pitch * 0.15f;

	if (rotationPitch < -90.0F)
		rotationPitch = -90.0F;

	if (rotationPitch > 90.0F)
		rotationPitch = 90.0F;

	prevRotationPitch += rotationPitch - p;
	prevRotationYaw += rotationYaw - y;
}

void Entity::onEntityUpdate()
{
	if (ridingEntity != NULL && ridingEntity->isDead)
	{
		ridingEntity = NULL;
	}

	prevDistanceWalkedModified = distanceWalkedModified;
	prevPos = position;
	prevRotationPitch = rotationPitch;
	prevRotationYaw = rotationYaw;
	bool isCameraLocked = false;
	if (this->isClass(ENTITY_CLASS_PLAYER))
	{
		isCameraLocked = dynamic_cast<EntityPlayer*>(this)->m_isCameraLocked;
	}

	if (isSprinting() && !isInWater() && !isCameraLocked)
	{
		Vector3 pos = position + Vector3(0.f, -0.2f - yOffset, 0.f);
		BlockPos ipos = pos.getFloor();
		int blockID = world->getBlockId(ipos);

		if (blockID > 0)
		{
			String name = StringUtil::Format("tilecrack_%d_%d", blockID, world->getBlockMeta(ipos));
			world->spawnParticle(name,
				Vector3(position.x + (rand->nextFloat() - 0.5f) * width, boundingBox.vMin.y + 0.1f, pos.z + (rand->nextFloat() - 0.5f) * width),
				Vector3(-motion.x * 4.0f, 1.5f, -motion.z * 4.0f));
		}
	}

	handleWaterMovement();

	if (world->m_isClient)
	{
		fire = 0;
	}
	else if (fire > 0)
	{
		if (m_isImmuneToFire)
		{
			fire -= 4;

			if (fire < 0)
				fire = 0;
		}
		else
		{
			if (fire % 20 == 0)
			{
				attackEntityFrom(DamageSource::onFire, 1.0F);
			}
			--fire;
		}
	}

	if (handleLavaMovement())
	{
		setOnFireFromLava();
		fallDistance *= 0.5F;
	}

	if (position.y < -64.f)
	{
		kill();
	}

	if (!world->m_isClient)
	{
		bool oldBurning = getFlag(ENTITY_STATUS_BURNING);
		setFlag(ENTITY_STATUS_BURNING, fire > 0);
		if (oldBurning != getFlag(ENTITY_STATUS_BURNING) && this->isClass(ENTITY_CLASS_PLAYERMP)) {
			NETWORK_DEFINE::PacketPlayerActionType action = NETWORK_DEFINE::PacketPlayerActionType::STOP_ON_FIRE;
			if (getFlag(ENTITY_STATUS_BURNING)) {
				action = NETWORK_DEFINE::PacketPlayerActionType::START_ON_FIRE;
			}
			BLOCKMAN::GameCommon::Instance()->getCommonPacketSender()
				->sendPlayerActionToTrackingPlayers(action, entityId, true);
		}
	}

	firstUpdate = false;
}

void Entity::setOnFireFromLava()
{
	if (!m_isImmuneToFire)
	{
		attackEntityFrom(DamageSource::lava, 4.0F);
		setFire(15);
	}
}

void Entity::setFire(int second)
{
	int i = second * 20;
	i = EnchantmentProtection::getFireTimeForEntity(this, i);

	if (fire < i)
	{
		fire = i;
	}
}

bool Entity::isOffsetPositionInLiquid(const Vector3& pos)
{
	Box bb = boundingBox.getOffset(pos);
	AABBList& aabbs = world->getCollidingBoundingBoxes(this, bb);
	return !aabbs.empty() ? false : !world->isAnyLiquid(bb);
	// return !var8.isEmpty() ? false : !worldObj.isAnyLiquid(var7);
	return false;
}

void Entity::moveEntity(const Vector3& vel)
{
	Vector3 prePos = position;

	if (noClip || isFlyEntity())
	{
		
		boundingBox.move(vel);
		position.x = (boundingBox.vMin.x + boundingBox.vMax.x) / 2.0f;
		position.y = boundingBox.vMin.y + yOffset - yPositionDecrement;
		position.z = (boundingBox.vMin.z + boundingBox.vMax.z) / 2.0f;
	}
	else
	{
		yPositionDecrement *= 0.4F;
		Vector3 pos = position;
		float x = vel.x;
		float y = vel.y;
		float z = vel.z;

		bool canIgnoreWeb = false;
		if (auto entityPlayer = dynamic_cast<EntityPlayer*>(this))
		{
			canIgnoreWeb = entityPlayer->capabilities.isWatchMode;
		}

		if (isInWeb && !canIgnoreWeb)
		{
			x *= 0.25f;
			y *= 0.05f;
			z *= 0.25f;
			motion = Vector3::ZERO;
		}
		isInWeb = false;

		float tx = x;
		float ty = y;
		float tz = z;
		Box bb = boundingBox;
		EntityPlayer* pplayer = dynamic_cast<EntityPlayer*>(this);
		bool playerOnGroundAndSneaking = onGround && isSneaking() && pplayer;

		if (playerOnGroundAndSneaking)
		{
			float f;

			for (f = 0.05f; x != 0.0f && world->getCollidingBoundingBoxes(this, boundingBox.getOffset(x, -1.0f, 0.0f)).empty(); tx = x)
			{
				if (x < f && x >= -f)
					x = 0.0f;
				else if (x > 0.0f)
					x -= f;
				else
					x += f;
			}

			for (; z != 0.0f && world->getCollidingBoundingBoxes(this, boundingBox.getOffset(0.0f, -1.0f, z)).empty(); tz = z)
			{
				if (z < f && z >= -f)
					z = 0.0f;
				else if (z > 0.0f)
					z -= f;
				else
					z += f;
			}

			while (x != 0.0f && z != 0.0f && world->getCollidingBoundingBoxes(this, boundingBox.getOffset(x, -1.0f, z)).empty())
			{
				if (x < f && x >= -f)
					x = 0.0f;
				else if (x > 0.0f)
					x -= f;
				else
					x += f;

				if (z < f && z >= -f)
					z = 0.0f;
				else if (z > 0.0f)
					z -= f;
				else
					z += f;

				tx = x;
				tz = z;
			}
		}

		AABBList& aabbs = world->getCollidingBoundingBoxes(this, boundingBox.addCoord(x, y, z));
		if (y != 0.0f) {
			for (int i = 0; i < int(aabbs.size()); ++i)
			{
				y = aabbs[i].getYOffset(boundingBox, y);
			}
			if (y != 0.0f) {
				boundingBox.move(0.0f, y, 0.0f);
			}
		}
		

		bool canTouchGround = onGround || ty != y && ty < 0.0f;
		int i;

		if (x != 0.0f) {
			for (i = 0; i < int(aabbs.size()); ++i)
			{
				x = aabbs[i].getXOffset(boundingBox, x);
			}
			if (x != 0.0f) {
				boundingBox.move(x, 0.0f, 0.0f);
			}
		}

		if (z != 0.0f) {
			for (i = 0; i < int(aabbs.size()); ++i)
			{
				z = aabbs[i].getZOffset(boundingBox, z);
			}

			if (z != 0.0f) {
				boundingBox.move(0.0f, 0.0f, z);
			}
		}

		float dy;
		float dz;
		float dx;
		if (stepHeight > 0.0F && canTouchGround && (playerOnGroundAndSneaking || yPositionDecrement < 0.05F) && (tx != x || tz != z))
		{
			dx = x;
			dy = y;
			dz = z;
			x = tx;
			y = stepHeight;
			z = tz;
			Box bb1 = boundingBox;
			boundingBox = bb;
			aabbs = world->getCollidingBoundingBoxes(this, boundingBox.addCoord(tx, y, tz));

			for (i = 0; i < int(aabbs.size()); ++i)
			{
				y = aabbs[i].getYOffset(boundingBox, y);
			}

			boundingBox.move(0.0f, y, 0.0f);

			for (i = 0; i < int(aabbs.size()); ++i)
			{
				x = aabbs[i].getXOffset(boundingBox, x);
			}

			boundingBox.move(x, 0.0f, 0.0f);

			for (i = 0; i < int(aabbs.size()); ++i)
			{
				z = aabbs[i].getZOffset(boundingBox, z);
			}
			boundingBox.move(0.0f, 0.0f, z);

			y = -stepHeight;
			for (i = 0; i < int(aabbs.size()); ++i)
			{
				y = aabbs[i].getYOffset(boundingBox, y);
			}
			boundingBox.move(0.0f, y, 0.0f);

			if (dx * dx + dz * dz >= x * x + z * z)
			{
				x = dx;
				y = dy;
				z = dz;
				boundingBox = bb1;
			}
		}

		position.x = (boundingBox.vMin.x + boundingBox.vMax.x) / 2.0f;
		position.y = boundingBox.vMin.y + yOffset - yPositionDecrement;
		position.z = (boundingBox.vMin.z + boundingBox.vMax.z) / 2.0f;

		isCollidedHorizontally = (Math::Abs(tx - x) > 0.0001f) || Math::Abs(tz - z) > 0.0001f;
		isCollidedVertically = (Math::Abs(ty - y) > 0.0001f);
		if (world->m_isClient || !dynamic_cast<EntityPlayer*>(this)) {
			onGround = isCollidedVertically && ty < 0.0f;
		}
		isCollided = isCollidedHorizontally || isCollidedVertically;

		if (tx != x)
			motion.x = 0.0f;

		if (ty != y)
		{
			motion.y = 0.0f;
		}

		if (tz != z)
			motion.z = 0.0f;

		updateFallState(y, onGround);
		dx = position.x - pos.x;
		dy = position.y - pos.y;
		dz = position.z - pos.z;

		if (canTriggerWalking() && !playerOnGroundAndSneaking && ridingEntity == NULL)
		{
			BlockPos pos = (position + Vector3(0.f, -0.2f - yOffset, 0.f)).getFloor();
			int blockID = world->getBlockId(pos);

			if (blockID == 0)
			{
				pos = pos.getNegY();
				int blockID1 = world->getBlockId(pos.getNegY());

				if (blockID1 == BLOCK_ID_FENCE ||
					blockID1 == BLOCK_ID_COBBLE_STONE_WALL||
					blockID1 == BLOCK_ID_FENCE_GATE)
				{
					blockID = blockID1;
				}
			}

			if (blockID != BLOCK_ID_LADDER)
				dy = 0.0f;

			distanceWalkedModified = distanceWalkedModified + Math::Sqrt(dx * dx + dz * dz) * 0.6f;
			distanceWalkedOnStepModified = distanceWalkedOnStepModified + Math::Sqrt(dx * dx + dy * dy + dz * dz) * 0.6f;

			if (distanceWalkedOnStepModified > nextStepDistance && blockID > 0)
			{
				nextStepDistance = (int)distanceWalkedOnStepModified + 1;

				if (isInWater())
				{
					float var42 = Math::Sqrt(motion.x * motion.x * 0.2f + motion.y * motion.y + motion.z * motion.z * 0.2f) * 0.35F;

					if (var42 > 1.0F)
						var42 = 1.0F;

					//playSound("liquid.swim", var42, 1.0F + (rand->nextFloat() - rand->nextFloat()) * 0.4F);
					if (world->canPlaySpecialSound(2))
					{
						playSoundByType(ST_Swim);
					}
				}

				playStepSound(pos, blockID);
				BlockManager::sBlocks[blockID]->onEntityWalking(world, pos, this);
			}
		}

		doBlockCollisions();
		
		bool wet = isWet();

		if (world->isBoundingBoxBurning(boundingBox.contract(0.001f, 0.001f, 0.001f)))
		{
			dealFireDamage(1);

			if (!wet)
			{
				++fire;

				if (fire == 0)
				{
					setFire(8);
				}
			}
		}
		else if (fire <= 0)
		{
			fire = -fireResistance;
		}

		if (wet && fire > 0)
		{
			//playSound("random.fizz", 0.7F, 1.6F + (rand->nextFloat() - rand->nextFloat()) * 0.4F);
			playSoundByType(ST_Fizz);
			fire = -fireResistance;
		}
	}
}

void Entity::doBlockCollisions()
{
	BlockPos minv = (boundingBox.vMin + 0.001f).getFloor();
	BlockPos maxv = (boundingBox.vMax - 0.001f).getFloor();

	if (!world->checkChunksExist(minv, maxv))
		return;

	for (int ix = minv.x; ix <= maxv.x; ++ix)
	{
		for (int iy = minv.y; iy <= maxv.y; ++iy)
		{
			for (int iz = minv.z; iz <= maxv.z; ++iz)
			{
				BlockPos pos(ix, iy, iz);
				int blockID = world->getBlockId(pos);

				if (blockID > 0)
				{
					BlockManager::sBlocks[blockID]->onEntityCollidedWithBlock(world, pos, this);
				}
			}
		}
	}
}

void Entity::playSoundByType(SoundType soundType)
{
	world->playSoundAtEntityByType(this, soundType);
}

void Entity::updateFallState(float y, bool onGroundIn)
{
	if (onGroundIn)
	{
		if (fallDistance > 0.0F)
		{
			fall(fallDistance);
		}
		fallDistance = 0.0F;
	}
	else if (y < 0.0f)
	{
		fallDistance = fallDistance - y;
	}
}

void Entity::dealFireDamage(int amountDamage)
{
	if (!m_isImmuneToFire)
	{
		attackEntityFrom(DamageSource::inFire, (float)amountDamage);
	}
}

void Entity::fall(float y)
{
	if (riddenByEntity != NULL)
	{
		riddenByEntity->fall(y);
	}
}

bool Entity::isWet()
{
	if (inWater)
		return inWater;

	BlockPos pos = position.getFloor();
	BlockPos pos1 = (position + Vector3(0.f, height, 0.f)).getFloor();
	return world->canLightningStrikeAt(pos) || world->canLightningStrikeAt(pos1);
}

bool Entity::handleWaterMovement()
{
	if (world->handleMaterialAcceleration(boundingBox.expland(0.f, -0.4f, 0.f).contract(0.001f, 0.001f, 0.001f), BM_Material::BM_MAT_water, this))
	{
		if (!inWater && !firstUpdate)
		{
			float s = Math::Sqrt(motion.x * motion.x * 0.2f + motion.y * motion.y + motion.z * motion.z * 0.2f) * 0.2F;

			if (s > 1.0F)
				s = 1.0F;

			//playSound("liquid.splash", s, 1.0F + (rand->nextFloat() - rand->nextFloat()) * 0.4F);
			if (world->canPlaySpecialSound(3))
			{
				playSoundByType(ST_LiquidSplash);
			}
			float y = Math::Floor(boundingBox.vMin.y);
			int i;
			float x;
			float z;

			for (i = 0; (float)i < 1.0F + width * 20.0F; ++i)
			{
				x = (rand->nextFloat() * 2.0F - 1.0F) * width;
				z = (rand->nextFloat() * 2.0F - 1.0F) * width;
				world->spawnParticle("bubble", { position.x + x, position.y + 1.0f, position.z + z }, { motion.x, motion.y - rand->nextFloat() * 0.2F, motion.z });
			}

			for (i = 0; (float)i < 1.0F + width * 20.0F; ++i)
			{
				x = (rand->nextFloat() * 2.0F - 1.0F) * width;
				z = (rand->nextFloat() * 2.0F - 1.0F) * width;
				world->spawnParticle("splash", { position.x + x, position.y + 1.0f, position.z + z }, motion);
			}
		}
		fallDistance = 0.0F;
		inWater = true;
		fire = 0;
	}
	else
		inWater = false;

	return inWater;
}

bool Entity::isInsideOfMaterial(const BM_Material& mat)
{
	float eye =/* position.y +*/ getEyeHeight();
	BlockPos pos = (position + Vector3(0.f, eye, 0.f)).getFloor();
	int blockID = world->getBlockId(pos);

	if (blockID != 0 && BlockManager::sBlocks[blockID]->getMaterial() == mat)
	{
		float percent = BlockFluid::getFluidHeightPercent(world->getBlockMeta(pos)) - 0.11111111F;
		float y = (float)(pos.y + 1) - percent;
		return eye < y;
	}
	return false;
}

bool Entity::handleLavaMovement()
{
	return world->isMaterialInBB(boundingBox.expland(-0.1f, -0.4f, -0.1f), BM_Material::BM_MAT_lava);
}

void Entity::moveFlying(float strafe, float forward, float speed)
{
	float distance = strafe * strafe + forward * forward;
	float dt = world->getFrameDeltaTime();

	if (distance >= 1e-4f)
	{
		distance = Math::Sqrt(distance);

		if (distance < 1.0F)
		{
			distance = 1.0F;
		}

		speed = speed / distance;
		strafe *= speed;
		forward *= speed;
		float f1 = Math::Sin(rotationYaw * Math::PI / 180.0F);
		float f2 = Math::Cos(rotationYaw * Math::PI / 180.0F);
		float dx = strafe * f2 - forward * f1;
		float dz = forward * f2 + strafe * f1;
		motion.x += dx;
		motion.z += dz;
	}
}

int Entity::getBrightnessForRender(float rdt)
{
	int x = int(Math::Floor(position.x));
	int z = int(Math::Floor(position.z));

	if (world->blockExists(BlockPos(x, 0, z)))
	{
		float height = (boundingBox.vMax.y - boundingBox.vMin.y) * 0.66f;
		int y = int(Math::Floor(position.y - yOffset + height));
		return world->getLightBrightnessForSkyBlocks(BlockPos(x, y, z), 0);
	}
	return 0;
}

float Entity::getBrightness(float rdt)
{
	int x = int(Math::Floor(position.x));
	int z = int(Math::Floor(position.z));

	if (world->blockExists(BlockPos(x, 0, z)))
	{
		double height = (boundingBox.vMax.y - boundingBox.vMin.y) * 0.66f;
		int y = int(Math::Floor(position.y - yOffset + height));
		return world->getLightBrightness(BlockPos(x, y, z));
	}
	return 0.0F;
}

void Entity::setPositionAndRotation(const Vector3& pos, float yaw, float pitch)
{
	prevPos = position = pos;
	prevRotationYaw = rotationYaw = yaw;
	prevRotationPitch = rotationPitch = pitch;
	yPositionDecrement = 0.0F;
	float dyaw = prevRotationYaw - yaw;

	if (dyaw < -180.0f)
		prevRotationYaw += 360.0F;

	if (dyaw >= 180.0f)
		prevRotationYaw -= 360.0F;

	setPosition(position);
	setRotation(yaw, pitch);
}

void Entity::setLocationAndAngles(const Vector3& pos, float yaw, float pitch)
{
	lastTickPos = prevPos = position = pos + Vector3(0.f, yOffset, 0.f);
	rotationYaw = yaw;
	rotationPitch = pitch;
	setPosition(position);
}

float Entity::getDistanceToEntity(Entity* pEntity)
{
	LordAssert(pEntity);
	Vector3 distance = position - pEntity->position;
	return distance.len();
}

float Entity::getDistanceSqToEntity(Entity* pEntity)
{
	LordAssert(pEntity);
	Vector3 distance = position - pEntity->position;
	return distance.lenSqr();
}

float Entity::getDistanceSq(const Vector3& pos)
{
	Vector3 distance = position - pos;
	return distance.lenSqr();
}

float Entity::getDistance(const Vector3& pos)
{
	Vector3 distance = position - pos;
	return distance.len();
}

void Entity::applyEntityCollision(Entity* pEntity)
{
	if (pEntity == NULL ||
		pEntity->riddenByEntity == this ||
		pEntity->ridingEntity == this)
		return;

	float dx = pEntity->position.x - position.x;
	float dz = pEntity->position.z - position.z;
	float dmax = Math::Max(Math::Abs(dx), Math::Abs(dz));

	if (dmax >= 0.01f)
	{
		dmax = Math::Sqrt(dmax);
		dx /= dmax;
		dz /= dmax;
		float var8 = 1.0f / dmax;

		if (var8 > 1.0f)
			var8 = 1.0f;

		dx *= var8;
		dz *= var8;
		dx *= 0.05f;
		dz *= 0.05f;
		dx *= 1.0F - entityCollisionReduction;
		dz *= 1.0F - entityCollisionReduction;
		addVelocity(Vector3(-dx, 0.0f, -dz));
		pEntity->addVelocity(Vector3(dx, 0.0f, dz));
	}
}

void Entity::addVelocity(const Vector3& vel)
{
	motion += vel;
	isAirBorne = true;
}

bool Entity::attackEntityFrom(DamageSource* pSource, float amount)
{
	if (isEntityInvulnerable())
	{
		return false;
	}
	else
	{
		setBeenAttacked();
		return false;
	}
}

bool Entity::isInRangeToRenderVec3D(const Vector3& vec) const
{
	Vector3 d = position - vec;
	return isInRangeToRenderDist(d.lenSqr());
}

bool Entity::isInRangeToRenderDist(float distanceSqr) const
{
	float edgeLen = boundingBox.getAverageEdgeLength();
	edgeLen *= 64.0f * renderDistanceWeight;
	return distanceSqr < edgeLen * edgeLen;
}

bool Entity::isInRangeToRenderDist(const Vector3& campos) const
{
	Vector3 d = campos - position;
	return isInRangeToRenderDist(d.lenSqr());
}

bool Entity::addNotRiddenEntityID(NBTTagCompound* pNBT)
{
	String name = getEntityString();

	if (!isDead && name != StringUtil::BLANK)
	{
		pNBT->setString("id", name);
		writeToNBT(pNBT);
		return true;
	}
	else
	{
		return false;
	}
}

bool Entity::addEntityID(NBTTagCompound* pNBT)
{
	String name = getEntityString();

	if (!isDead && name != StringUtil::BLANK && riddenByEntity == NULL)
	{
		pNBT->setString("id", name);
		writeToNBT(pNBT);
		return true;
	}
	else
	{
		return false;
	}
}

void Entity::writeToNBT(NBTTagCompound* pNBT)
{
	pNBT->setTag("Pos", newFloatNBTList(Vector3(position.x, position.y + yPositionDecrement, position.z)));
	pNBT->setTag("Motion", newFloatNBTList(motion));
	pNBT->setTag("Rotation", newFloatNBTList(Vector2(rotationYaw, rotationPitch)));
	pNBT->setFloat("FallDistance", fallDistance);
	pNBT->setShort("Fire", (short)fire);
	pNBT->setShort("Air", (short)getAir());
	pNBT->setBool("OnGround", onGround);
	pNBT->setInteger("Dimension", dimension);
	pNBT->setBool("Invulnerable", invulnerable);
	pNBT->setInteger("PortalCooldown", timeUntilPortal);
	pNBT->setLong("UUIDMost", entityUniqueID.getMostSignificantBits());
	pNBT->setLong("UUIDLeast", entityUniqueID.getLeastSignificantBits());
	writeEntityToNBT(pNBT);

	if (ridingEntity != NULL)
	{
		NBTTagCompound* pRidingTag = LordNew NBTTagCompound("Riding");

		if (ridingEntity->addNotRiddenEntityID(pRidingTag))
		{
			pNBT->setTag("Riding", pRidingTag);
		}
	}
}

void Entity::readFromNBT(NBTTagCompound* pNBT)
{
	NBTTagList* tag_position = pNBT->getTagList("Pos");
	NBTTagList* tag_motion = pNBT->getTagList("Motion");
	NBTTagList* tag_rotation = pNBT->getTagList("Rotation");
	motion.x = ((NBTTagFloat*)tag_motion->tagAt(0))->data;
	motion.y = ((NBTTagFloat*)tag_motion->tagAt(1))->data;
	motion.z = ((NBTTagFloat*)tag_motion->tagAt(2))->data;

	if (Math::Abs(motion.x) > 10.0f)
		motion.x = 0.0f;

	if (Math::Abs(motion.y) > 10.0f)
		motion.y = 0.0f;

	if (Math::Abs(motion.z) > 10.0f)
		motion.z = 0.0f;

	prevPos.x = lastTickPos.x = position.x = ((NBTTagFloat*)tag_position->tagAt(0))->data;
	prevPos.y = lastTickPos.y = position.y = ((NBTTagFloat*)tag_position->tagAt(1))->data;
	prevPos.z = lastTickPos.z = position.z = ((NBTTagFloat*)tag_position->tagAt(2))->data;
	prevRotationYaw = rotationYaw = ((NBTTagFloat*)tag_rotation->tagAt(0))->data;
	prevRotationPitch = rotationPitch = ((NBTTagFloat*)tag_rotation->tagAt(1))->data;
	fallDistance = pNBT->getFloat("FallDistance");
	fire = pNBT->getShort("Fire");
	setAir(pNBT->getShort("Air"));
	onGround = pNBT->getBool("OnGround");
	dimension = pNBT->getInteger("Dimension");
	invulnerable = pNBT->getBool("Invulnerable");
	timeUntilPortal = pNBT->getInteger("PortalCooldown");

	if (pNBT->hasKey("UUIDMost") && pNBT->hasKey("UUIDLeast"))
	
		entityUniqueID = UUID(pNBT->getLong("UUIDMost"), pNBT->getLong("UUIDLeast"));

	setPosition(position);
	setRotation(rotationYaw, rotationPitch);
	readEntityFromNBT(pNBT);
}

NBTTagList* Entity::newFloatNBTList(const Vector3& vec)
{
	NBTTagList* pResult = LordNew NBTTagList();
	pResult->appendTag(LordNew NBTTagFloat(StringUtil::BLANK, vec.x));
	pResult->appendTag(LordNew NBTTagFloat(StringUtil::BLANK, vec.y));
	pResult->appendTag(LordNew NBTTagFloat(StringUtil::BLANK, vec.z));
	return pResult;
}

NBTTagList* Entity::newFloatNBTList(const Vector2& vec)
{
	NBTTagList* pResult = LordNew NBTTagList();
	pResult->appendTag(LordNew NBTTagFloat(StringUtil::BLANK, vec.x));
	pResult->appendTag(LordNew NBTTagFloat(StringUtil::BLANK, vec.y));
	return pResult;
}

String Entity::getEntityString()
{
	// todo.
	// return EntityManager::Instance()->getEntityString(this);
	return StringUtil::BLANK;
}

EntityItem* Entity::dropItemWithOffset(int itemID, int count, float yOffset)
{
	return entityDropItem(LORD::make_shared<ItemStack>(itemID, count, 0), yOffset);
}

/** Drops an item at the position of the entity. */
EntityItem* Entity::entityDropItem(ItemStackPtr pStack, float yOffset)
{
	if (pStack->stackSize == 0)
	{
		return NULL;
	}
	else
	{
		EntityItem* pResult = LordNew EntityItem(world, position + Vector3(0.f, yOffset, 0.f), pStack);
		pResult->delayBeforeCanPickup = 10;
		world->spawnEntityInWorld(pResult);
		return pResult;
	}
}

bool Entity::isEntityInsideOpaqueBlock()
{
	for (int i = 0; i < 8; ++i)
	{
		Vector3 vpos = Vector3(width, 0.1f, width) * Vector3(0.8f, 1.f, 0.8f) * ((float)((i >> 0) % 2) - 0.5F);
		BlockPos pos = (vpos + position + Vector3(0.f, getEyeHeight(), 0.f)).getFloor();

		if (world->isBlockNormalCube(pos))
		{
			return true;
		}
	}

	return false;
}

void Entity::updateRidden()
{
	if (ridingEntity->isDead)
	{
		ridingEntity = NULL;
	}
	else
	{
		motion = Vector3::ZERO;
		onUpdate();

		if (ridingEntity != NULL)
		{
			ridingEntity->updateRiderPosition();
			entityRiderYawDelta += ridingEntity->rotationYaw - ridingEntity->prevRotationYaw;

			for (entityRiderPitchDelta += ridingEntity->rotationPitch - ridingEntity->prevRotationPitch; entityRiderYawDelta >= 180.0f; entityRiderYawDelta -= 360.0f)
				;

			while (entityRiderYawDelta < -180.0f)
				entityRiderYawDelta += 360.0f;

			while (entityRiderPitchDelta >= 180.0f)
				entityRiderPitchDelta -= 360.0f;

			while (entityRiderPitchDelta < -180.0f)
				entityRiderPitchDelta += 360.0f;

			float dyaw = entityRiderYawDelta * 0.5f;
			float dpitch = entityRiderPitchDelta * 0.5f;
			float dmax = 10.0F;

			if (dyaw > dmax)
				dyaw = dmax;

			if (dyaw < -dmax)
				dyaw = -dmax;

			if (dpitch > dmax)
				dpitch = dmax;

			if (dpitch < -dmax)
				dpitch = -dmax;

			entityRiderYawDelta -= dyaw;
			entityRiderPitchDelta -= dpitch;
		}
	}
}

void Entity::updateRiderPosition()
{
	if (riddenByEntity != NULL)
	{
		riddenByEntity->setPosition(position + Vector3(0.f, getMountedYOffset() + riddenByEntity->getYOffset(), 0.f));
	}
}

void Entity::mountEntity(Entity* pEntity)
{
	entityRiderPitchDelta = 0.0f;
	entityRiderYawDelta = 0.0f;

	if (pEntity == NULL)
	{
		if (ridingEntity != NULL)
		{
			setLocationAndAngles(Vector3(ridingEntity->position.x, ridingEntity->boundingBox.vMin.y + ridingEntity->height, ridingEntity->position.z), rotationYaw, rotationPitch);
			ridingEntity->riddenByEntity = NULL;
		}

		ridingEntity = NULL;
	}
	else
	{
		if (ridingEntity != NULL)
		{
			ridingEntity->riddenByEntity = NULL;
		}

		ridingEntity = pEntity;
		pEntity->riddenByEntity = this;
	}
}

void Entity::setPositionAndRotation2(const Vector3& pos, float yaw, float pitch, int increment)
{
	setPosition(pos);
	setRotation(yaw, pitch);
	AABBList& aabbs = world->getCollidingBoundingBoxes(this, boundingBox.contract(0.03125f, 0.0f, 0.03125f));
	float y = 0.f;

	if (!aabbs.empty())
	{
		float maxy = 0.0f;

		for (int i = 0; i < int(aabbs.size()); ++i)
		{
			Box& box = aabbs[i];

			if (box.vMax.y > maxy)
			{
				maxy = box.vMax.y;
			}
		}

		y += maxy - boundingBox.vMin.y;
		setPosition(pos + Vector3(0.f, y, 0.f));
	}
}

void Entity::setFlag(int flag, bool val) 
{
	LordAssert(flag > ENTITY_STATUS_INVALID && flag < ENTITY_STATUS_COUNT);
	if (val) 
	{
		m_flags.set(flag);
	}
	else
	{
		m_flags.reset(flag);
	}
}

void Entity::onStruckByLightning(EntityLightningBolt* pLight)
{
	dealFireDamage(5);
	++fire;

	if (fire == 0)
	{
		setFire(8);
	}
}

bool Entity::pushOutOfBlocks(const Vector3& pos)
{
	BlockPos pos1 = pos.getFloor();
	Vector3 offset = pos - Vector3(pos1);
	AABBList& aabbs = world->getCollidingBlockBounds(boundingBox);

	if (aabbs.empty() && !world->isBlockFullCube(pos1))
	{
		return false;
	}
	else
	{
		bool west = !world->isBlockFullCube(pos1.getNegX());
		bool east = !world->isBlockFullCube(pos1.getPosX());
		bool bottom = !world->isBlockFullCube(pos1.getNegY());
		bool top = !world->isBlockFullCube(pos1.getPosY());
		bool north = !world->isBlockFullCube(pos1.getNegZ());
		bool south = !world->isBlockFullCube(pos1.getPosZ());
		int face = 3;
		float minOffset = 9999.0f;

		if (west && offset.x < minOffset)
		{
			minOffset = offset.x;
			face = 0;
		}

		if (east && 1.0f - offset.x < minOffset)
		{
			minOffset = 1.0f - offset.x;
			face = 1;
		}

		if (top && 1.0f - offset.y < minOffset)
		{
			minOffset = 1.0f - offset.y;
			face = 3;
		}

		if (north && offset.z < minOffset)
		{
			minOffset = offset.z;
			face = 4;
		}

		if (south && 1.0f - offset.z < minOffset)
		{
			minOffset = 1.0f - offset.z;
			face = 5;
		}

		float f = rand->nextFloat() * 0.2F + 0.1F;

		if (face == 0)
			motion.x = -f;
		else if (face == 1)
			motion.x = f;
		else if (face == 2)
			motion.y = -f;
		else if (face == 3)
			motion.y = f;
		else if (face == 4)
			motion.z = -f;
		else if (face == 5)
			motion.z = f;

		return true;
	}
}

String Entity::getEntityName()
{
	// todo.
	// String name = EntityList::getEntityString(this);
	String name;

	if (name == StringUtil::BLANK)
	{
		name = "generic";
	}

	// return StatCollector.translateToLocal("entity." + name + ".name");
	return name;
}

String Entity::toString()
{
	// return StringUtil::Format("%s[\'%s\'/%d, l=\'%s\', x=%.2f, y=%.2f, z=%.2f]", new Object[]{ getClass().getSimpleName(), getEntityName(), Integer.valueOf(entityId), worldObj == null ? "~NULL~" : worldObj.getWorldInfo().getWorldName(), Double.valueOf(posX), Double.valueOf(posY), Double.valueOf(posZ) });
	return StringUtil::BLANK;
}

void Entity::copyLocationAndAnglesFrom(Entity* pEntity)
{
	setLocationAndAngles(pEntity->position, pEntity->rotationYaw, pEntity->rotationPitch);
}

float Entity::getExplosionResistance(Explosion* pExplosion, World* pWorld, const BlockPos& pos, Block* pBlock)
{
	return pBlock->getExplosionResistance(this); 
}

void Entity::copyDataFrom(Entity* pEntity, bool par2)
{
	NBTTagCompound* pNBT = LordNew NBTTagCompound();
	pEntity->writeToNBT(pNBT);
	readFromNBT(pNBT);
	timeUntilPortal = pEntity->timeUntilPortal;
}


Vector3 Entity::getPosition(float rdt, bool bForce)
{
	if (!bForce && this->isClass(ENTITY_CLASS_PLAYER_SPMP))
	{
		auto player = dynamic_cast<EntityPlayer*>(this);
		if (NULL != player && player->m_isCameraLocked)
		{
			return player->m_prevCameraLockPos + (player->m_cameraLockPos - player->m_prevCameraLockPos) * rdt;
		}
	}

	return prevPos + (position - prevPos) * rdt;
}

Vector3 Entity::getLastTickPosition(float rdt)
{
	if (this->isClass(ENTITY_CLASS_PLAYER_SPMP))
	{
		auto player = dynamic_cast<EntityPlayer*>(this);
		if (NULL != player && player->m_isCameraLocked)
		{
			return player->m_prevCameraLockPos + (player->m_cameraLockPos - player->m_prevCameraLockPos) * rdt;
		}
	}

	return lastTickPos + (position - lastTickPos) * rdt;
}

int Entity::multiplyBy32AndRound(int s, float v)
{
	float decimals = v - (Math::Floor(v) + 0.5f);
	float SCALE = 32.f;
	switch (s)
	{
	case 1:
		if (decimals < 0.0f)
		{
			if (decimals < -0.3125f)
			{
				return int(Math::Ceil(v * SCALE));
			}
		}
		else if (decimals < 0.3125f)
		{
			return int(Math::Ceil(v * SCALE));
		}

		return int(Math::Floor(v * SCALE));

	case 2:
		if (decimals < 0.0f)
		{
			if (decimals < -0.3125f)
			{
				return int(Math::Floor(v * SCALE));
			}
		}
		else if (decimals < 0.3125f)
		{
			return int(Math::Floor(v * SCALE));
		}

		return int(Math::Ceil(v * SCALE));

	case 3:
		if (decimals > 0.0f)
		{
			return int(Math::Floor(v * SCALE));
		}

		return int(Math::Ceil(v * SCALE));

	case 4:
		if (decimals < 0.0f)
		{
			if (decimals < -0.1875f)
			{
				return int(Math::Ceil(v * SCALE));
			}
		}
		else if (decimals < 0.1875f)
		{
			return int(Math::Ceil(v * SCALE));
		}

		return int(Math::Floor(v *SCALE));

	case 5:
		if (decimals < 0.0f)
		{
			if (decimals < -0.1875f)
			{
				return int(Math::Floor(v * SCALE));
			}
		}
		else if (decimals < 0.1875f)
		{
			return int(Math::Floor(v * SCALE));
		}

		return int(Math::Ceil(v * SCALE));

	case 6:
	default:
		if (decimals > 0.0f)
		{
			return int(Math::Ceil(v * SCALE));
		}
		return int(Math::Floor(v * SCALE));
	}
}

bool Entity::canFall()
{
	Vector3i posFloor = position.getFloor();
	int block_id = world->getBlockId(posFloor.getNegY());
	bool cannotFall = Math::Abs(posFloor.y - position.y) < 0.2
		&& block_id > 0
		&& block_id != 8	// Fluid
		&& block_id != 9
		&& block_id != 10
		&& block_id != 11
		&& block_id != 916;//transparency block

	return !cannotFall;
}

void Entity::setOnSelected(int type)
{
	if (type > 2)
		return;
	SelectNodeType nodeType = (SelectNodeType)type;
	onSelectedChange = onSelectedChange || (selectNodeType != nodeType);
	selectNodeType = nodeType;
}

}
