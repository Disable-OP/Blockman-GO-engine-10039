#include "EntityBulletClient.h"
#include "Script/Event/LogicScriptEvents.h"
#include "Block/Block.h"
#include "cWorld/Blockman.h"
#include "World/GameSettings.h"
#include "cEffects/EntityFx.h"
#include "Network/ClientNetwork.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "World/World.h"
#include "World/WorldEffectManager.h"
#include "Render/TextureAtlas.h"
#include "Tessolator/SkinedRenderable.h"
#include "Tessolator/TessRenderable.h"
#include "Tessolator/TessManager.h"
#include "Scene/ModelEntity.h"
#include "Model/MeshManager.h"
#include "Model/Mesh.h"
#include "Actor/ActorManager.h"
#include "Setting/LogicSetting.h"
#include "Object/Root.h"

namespace BLOCKMAN
{
	EntityBulletClient::EntityBulletClient(World* pWorld, EntityLivingBase* pShooting, GunSetting* pSetting, const Vector3& beginPos, const Vector3& endPos, float yawOffset, float pitchOffset)
		: EntityBullet(pWorld)
	{
		if (!pSetting || !pShooting || pShooting->isDead)
		{
			setDead();
			return;
		}
		m_shootingEntity = pShooting;
		m_gunSetting = pSetting;
		m_shootingEntityId = pShooting->entityId;
		m_bounceTimes = pSetting->bounceTimes;
		m_isShootingByMe = m_shootingEntityId == Blockman::Instance()->m_pPlayer->entityId;

		Vector3 beginPosTemp = calcPosition(pShooting, pSetting);

		float dx = endPos.x - beginPosTemp.x;
		float dy = endPos.y - beginPosTemp.y;
		float dz = endPos.z - beginPosTemp.z;
		float dxz = Math::Sqrt(dx * dx + dz * dz);
		float beginYaw = (Math::ATan2(dz, dx) * Math::RAD2DEG) - 90.0F;
		float beginPitch = -(Math::ATan2(dy, dxz) * Math::RAD2DEG);
		if (Vector3(dx, dy, dz).normalizedCopy().len() < 0.00001f)
		{
			beginYaw = pShooting->rotationYaw;
			beginPitch = pShooting->rotationPitch;
		}

		setShootingRange(pSetting->shootRange);
		setLocationAndAngles(beginPosTemp, beginYaw + yawOffset, beginPitch + pitchOffset);
		setGunID(pSetting->gunId);

		Vector3 dir;
		dir.x = -float(Math::Sin(double(rotationYaw) * Math::DEG2RAD) * Math::Cos(double(rotationPitch) * Math::DEG2RAD));
		dir.z = float(Math::Cos(double(rotationYaw) * Math::DEG2RAD) * Math::Cos(double(rotationPitch) * Math::DEG2RAD));
		dir.y = -float(Math::Sin(double(rotationPitch) * Math::DEG2RAD));
		float velocity = pSetting->bulletSpeed;
		setThrowableHeading(dir, velocity * 1.5F, 1.0F);
		
		m_bulletEffectName = pSetting->bulletEffect;

		if (pShooting == Blockman::Instance()->m_pPlayer && Blockman::Instance()->m_gameSettings->getPersonView() == GameSettings::SPVT_FIRST)
		{
			m_isFirstPerspective = true;
		}
	}

	EntityBulletClient::~EntityBulletClient()
	{
		if (m_needDeleteEffect && m_bulletEffect != nullptr)
		{
			m_bulletEffect->mDuration = 0;
			m_bulletEffect = nullptr;
		}
	}

	void EntityBulletClient::onUpdate()
	{
		Entity::onUpdate();

		if (m_shootingEntity && m_shootingEntity->isDead)
		{
			m_shootingEntity = nullptr;
		}

		Vector3 begin = position;
		Vector3 entityEnd = position + motion;
		Vector3 blockEnd = position + motion;
		RayTraceResult trace = world->rayTraceBlocks(begin, blockEnd, false, true);
		float min_dis = m_shootingRange;
		float ratio = 1.0f;
		if (trace.result)
		{
			blockEnd = trace.hitVec;
			min_dis = begin.distanceTo(blockEnd);
			if (min_dis <= motion.len() && motion.len() > 0.00001f)
			{
				ratio = min_dis / motion.len();
			}
			else
			{
				min_dis = m_shootingRange;
			}
		}

		Entity* tracyEntity = nullptr;
		Vector3 hitPos = Vector3::ZERO;
		EntityArr entities = world->getEntitiesWithinAABBExcludingEntity(this, boundingBox.addCoord(motion.x * ratio, motion.y * ratio, motion.z * ratio));
		for (auto entity : entities)
		{
			if (entity && entity->canBePushed() && begin.distanceTo(entity->position) <= m_shootingRange && entity->entityId != m_shootingEntityId)
			{
				Box entity_box = entity->boundingBox;
				RayTraceResult tracy_entity = RayTraceResult::calculateIntercept(entity_box, begin, entityEnd);
				if (tracy_entity.result)
				{
					float dis = begin.distanceTo(tracy_entity.hitVec);
					if (dis < min_dis)
					{
						tracyEntity = entity;
						hitPos = tracy_entity.hitVec;
						min_dis = dis;
					}
				}
			}
		}
		bool isHeadshot = false;
		if (tracyEntity)
		{
			trace.set(tracyEntity, hitPos);
			if (m_isShootingByMe && LogicSetting::Instance()->getAllowHeadshotStatus() && tracyEntity->isClass(ENTITY_CLASS_PLAYER))
			{
				EntityPlayer* pTarget = dynamic_cast<EntityPlayer*>(tracyEntity);
				EntityPlayer* pShooting = dynamic_cast<EntityPlayer*>(m_shootingEntity);
				if (pTarget && pShooting && (LogicSetting::Instance()->getShowGunEffectWithSingleStatus() || pTarget->getTeamId() != pShooting->getTeamId()))
				{
					isHeadshot = world->isEntityHeadWithinRayTracy(Box(Vector3(tracyEntity->boundingBox.vMin.x, tracyEntity->boundingBox.vMin.y + tracyEntity->getHeight() * 5 / 7, tracyEntity->boundingBox.vMin.z), tracyEntity->boundingBox.vMax), begin, motion.normalizedCopy(), begin.distanceTo(blockEnd));// head:body = 1:2.5
				}
			}
		}

		if (trace.result && trace.entityHit)
		{
			EntityPlayer* tracyPlayer = dynamic_cast<EntityPlayer*>(trace.entityHit);
			if (tracyPlayer && tracyPlayer->capabilities.disableDamage)
			{
				trace.reset();
			}
		}

		if (trace.result)
		{
			if (trace.entityHit)
			{
				if (m_isShootingByMe)
				{
					int traceType = NONE_TRACE_TYPE;
					if (trace.entityHit->isClass(ENTITY_CLASS_CREATURE))
					{
						traceType = ENTITY_CREATUREAI_TRACE_TYPE;
					}
					else if (trace.entityHit->isClass(ENTITY_CLASS_PLAYER))
					{
						traceType = ENTITY_PLAYER_TRACE_TYPE;
					}
					ClientNetwork::Instance()->getSender()->sendEffectGunFireResult(ClientNetwork::Instance()->getDataCache()->getServerId(trace.entityHit->entityId), trace.hitVec, motion.normalizedCopy(), getGunID(), traceType, trace.getBlockPos(), isHeadshot);
				}
				motion = trace.hitVec - position;
				position = trace.hitVec;
				setDead();
			}
			else
			{
				if (m_isShootingByMe && world->getBlockId(trace.getBlockPos()) > 0)
				{
					ClientNetwork::Instance()->getSender()->sendEffectGunFireResult(0, trace.hitVec, motion.normalizedCopy(), getGunID(), BLOCK_TRACE_TYPE, trace.getBlockPos());
				}

				if (m_bounceTimes > 0 && trace.sideHit)
				{
					prevPos = trace.hitVec - motion.normalizedCopy() * 0.05f;
					motion = motion.Reflect(trace.sideHit->getDirectionVec());
					position += motion;
					m_bounceTimes--;
				}
				else
				{
					motion = trace.hitVec - position;
					position = trace.hitVec - motion.normalizedCopy() * 0.05f;
					setDead();
				}
			}
		}
		else
		{
			position += motion;
			m_shootingRange -= motion.len();
			if (m_shootingRange <= 0.0f)
			{
				setDead();
			}
		}

		setPosition(position);

		if (m_bulletEffect)
		{
			m_bulletEffect->mPosition = position;
		}
	}

	Vector3 EntityBulletClient::calcPosition(EntityLivingBase* shooting, GunSetting* gunSetting, float rdt)
	{
		if (!shooting || !gunSetting)
		{
			return Vector3::ZERO;
		}

		bool isFirstView = shooting == Blockman::Instance()->m_pPlayer && Blockman::Instance()->m_gameSettings->getPersonView() == GameSettings::SPVT_FIRST;
		Camera* mainCamera = SceneManager::Instance()->getMainCamera();
		Vector3 pos, dir, right, down;
		float frontOff, rightOff, downOff;
		if (isFirstView)
		{
			pos = mainCamera->getPosition();

			dir = mainCamera->getDirection();
			right = mainCamera->getRight();
			down = -mainCamera->getUp();

			frontOff = gunSetting->flame_1_frontOff;
			rightOff = gunSetting->flame_1_rightOff;
			downOff = gunSetting->flame_1_downOff;
		}
		else
		{
			pos = shooting->getPosition(rdt);
			pos.y += shooting->getEyeHeight();

			if (shooting == (EntityPlayer*)(Blockman::Instance()->m_pPlayer))
			{
				pos.y += 0.08f;
			}

			float yaw = shooting->prevRenderYawOffset + (shooting->renderYawOffset - shooting->prevRenderYawOffset) * rdt;
			float pitch = 0.f;

			dir.x = -float(Math::Sin(double(yaw) * Math::DEG2RAD) * Math::Cos(double(pitch) * Math::DEG2RAD));
			dir.z = float(Math::Cos(double(yaw) * Math::DEG2RAD) * Math::Cos(double(pitch) * Math::DEG2RAD));
			dir.y = -float(Math::Sin(double(pitch) * Math::DEG2RAD));
			right = Vector3::NEG_UNIT_Y.cross(dir);
			down = Vector3::NEG_UNIT_Y;

			frontOff = gunSetting->flame_3_frontOff;
			rightOff = gunSetting->flame_3_rightOff;
			downOff = gunSetting->flame_3_downOff;
		}

		pos += dir * frontOff;
		pos += right * rightOff;
		pos += down * downOff;

		return pos;
	}

	//------------------------------------------------------------------------

	EntityMortarBulletClient::EntityMortarBulletClient(World * pWorld, EntityLivingBase * pShooting, GunSetting * pSetting, const Vector3 & beginPos, const Vector3 & endPos)
		: EntityBulletClient(pWorld, pShooting, pSetting, beginPos, endPos)
	{
		Mesh* pMesh = MeshManager::Instance()->createMesh("bullet_mortar.mesh");
		pMesh->load();
		ModelEntity* entity = LordNew ModelEntity;
		SceneNode* node = SceneManager::Instance()->getRootNode()->createChild();

		Quaternion quatYaw = Quaternion(Vector3::UNIT_Y, -(pShooting->rotationYaw + 90.0f) * Math::DEG2RAD);
		Quaternion quatPitch = Quaternion(Vector3::UNIT_Z, -pShooting->rotationPitch * Math::DEG2RAD);
		node->setLocalOrientation(quatYaw * quatPitch);
		node->setLocalScaling(Vector3(0.7f, 0.7f, 0.7f));

		entity->setMesh(pMesh);
		entity->attachTo(node);
		entity->setRenderType(ModelEntity::RT_FOR_ACTOR_TRANSPARENT);
		entity->setMaterial(ActorManager::Instance()->GetStaticMaterial_transparent());
		entity->createRenderable(true);
		entity->prepareTexture(true);
		m_modelEntity = entity;

		m_bulletType = BT_MORTAR;
	}

	EntityMortarBulletClient::~EntityMortarBulletClient()
	{
		if (m_modelEntity != nullptr)
		{
			SceneNode* node = m_modelEntity->getSceneNode();
			Mesh* mesh = m_modelEntity->getMesh();
			SceneManager::Instance()->getRootNode()->destroyChild(node);
			MeshManager::Instance()->releaseResource(mesh);
			LordDelete(m_modelEntity);
			m_modelEntity = nullptr;
		}
	}

	void EntityMortarBulletClient::onUpdate()
	{
		Entity::onUpdate();

		if (m_shootingEntity && m_shootingEntity->isDead)
		{
			m_shootingEntity = nullptr;
		}

		motion.y -= m_gunSetting->gravityVelocity;
		float motion_xz = Math::Sqrt(motion.x * motion.x + motion.z * motion.z);
		rotationYaw = Math::ATan2(motion.x, motion.z) * Math::RAD2DEG;
		rotationPitch = Math::ATan2(motion.y, motion_xz) * Math::RAD2DEG;

		Vector3 begin = position;
		Vector3 entityEnd = position + motion;
		Vector3 blockEnd = position + motion;
		RayTraceResult trace = world->rayTraceBlocks(begin, blockEnd, false, true);
		float min_dis = m_shootingRange;
		float ratio = 1.0f;
		if (trace.result)
		{
			blockEnd = trace.hitVec;
			min_dis = begin.distanceTo(blockEnd);
			if (min_dis <= motion.len() && motion.len() > 0.00001f)
			{
				ratio = min_dis / motion.len();
			}
			else
			{
				min_dis = m_shootingRange;
			}
		}

		Entity* tracyEntity = nullptr;
		Vector3 hitPos = Vector3::ZERO;
		EntityArr entities = world->getEntitiesWithinAABBExcludingEntity(this, boundingBox.addCoord(motion.x * ratio, motion.y * ratio, motion.z * ratio));
		for (auto entity : entities)
		{
			if (entity && entity->canBePushed() && begin.distanceTo(entity->position) <= m_shootingRange && entity->entityId != m_shootingEntityId)
			{
				Box entity_box = entity->boundingBox;
				RayTraceResult tracy_entity = RayTraceResult::calculateIntercept(entity_box, begin, entityEnd);
				if (tracy_entity.result)
				{
					float dis = begin.distanceTo(tracy_entity.hitVec);
					if (dis < min_dis)
					{
						tracyEntity = entity;
						hitPos = tracy_entity.hitVec;
						min_dis = dis;
					}
				}
			}
		}
		bool isHeadshot = false;
		if (tracyEntity)
		{
			trace.set(tracyEntity, hitPos);
			if (m_isShootingByMe && LogicSetting::Instance()->getAllowHeadshotStatus() && tracyEntity->isClass(ENTITY_CLASS_PLAYER))
			{
				EntityPlayer* pTarget = dynamic_cast<EntityPlayer*>(tracyEntity);
				EntityPlayer* pShooting = dynamic_cast<EntityPlayer*>(m_shootingEntity);
				if (pTarget && pShooting && (LogicSetting::Instance()->getShowGunEffectWithSingleStatus() || pTarget->getTeamId() != pShooting->getTeamId()))
				{
					isHeadshot = world->isEntityHeadWithinRayTracy(Box(Vector3(tracyEntity->boundingBox.vMin.x, tracyEntity->boundingBox.vMin.y + tracyEntity->getHeight() * 5 / 7, tracyEntity->boundingBox.vMin.z), tracyEntity->boundingBox.vMax), begin, motion.normalizedCopy(), begin.distanceTo(blockEnd));// head:body = 1:2.5
				}
			}
		}

		if (trace.result)
		{
			if (m_isShootingByMe)
			{
				Box box = Box(trace.hitVec, trace.hitVec);
				int hitEntityId = -1;
				if (trace.entityHit)
				{
					int traceType = NONE_TRACE_TYPE;
					if (trace.entityHit->isClass(ENTITY_CLASS_CREATURE))
					{
						traceType = ENTITY_CREATUREAI_TRACE_TYPE;
					}
					else if (trace.entityHit->isClass(ENTITY_CLASS_PLAYER))
					{
						traceType = ENTITY_PLAYER_TRACE_TYPE;
					}
					ClientNetwork::Instance()->getSender()->sendEffectGunFireResult(ClientNetwork::Instance()->getDataCache()->getServerId(trace.entityHit->entityId), trace.hitVec, motion.normalizedCopy(), getGunID(), traceType, trace.getBlockPos(), isHeadshot);
					
					box = trace.entityHit->boundingBox;
					hitEntityId = trace.entityHit->entityId;
				}
				else
				{
					if (world->getBlockId(trace.getBlockPos()) > 0)
					{
						ClientNetwork::Instance()->getSender()->sendEffectGunFireResult(0, trace.hitVec, motion.normalizedCopy(), getGunID(), BLOCK_TRACE_TYPE, trace.getBlockPos());
					}
				}

				float explosionRange = m_gunSetting->explosionRange;
				ClientNetwork::Instance()->getSender()->sendEffectGunFireExplosionResult(trace.hitVec, explosionRange);
				EntityArr explosionEntities = world->getEntitiesWithinAABBExcludingEntity(this, box.contract(-explosionRange, -explosionRange, -explosionRange));
				for (auto entity : explosionEntities)
				{
					if (entity && entity->canBePushed() && entity->entityId != m_shootingEntityId && entity->entityId != hitEntityId)
					{
						if (entity->isClass(ENTITY_CLASS_PLAYER))
						{
							EntityPlayer* tracyPlayer = dynamic_cast<EntityPlayer*>(entity);
							if (tracyPlayer && tracyPlayer->capabilities.disableDamage)
							{
								continue;
							}
							ClientNetwork::Instance()->getSender()->sendEffectGunFireResult(ClientNetwork::Instance()->getDataCache()->getServerId(entity->entityId), entity->position, motion.normalizedCopy(), getGunID(), ENTITY_PLAYER_TRACE_TYPE, trace.getBlockPos());
						}
						else if (entity->isClass(ENTITY_CLASS_CREATURE))
						{
							ClientNetwork::Instance()->getSender()->sendEffectGunFireResult(ClientNetwork::Instance()->getDataCache()->getServerId(entity->entityId), entity->position, motion.normalizedCopy(), getGunID(), ENTITY_CREATUREAI_TRACE_TYPE, trace.getBlockPos());
						}
					}
				}
			}

			motion = trace.hitVec - position;
			position = trace.hitVec - motion.normalizedCopy() * 0.05f;
			setDead();
		}
		else
		{
			position += motion;
			m_shootingRange -= motion.len();
			if (m_shootingRange <= 0.f)
			{
				setDead();
			}
		}

		setPosition(position);

		if (m_bulletEffect)
		{
			m_bulletEffect->mPosition = position;
		}
	}

	//------------------------------------------------------------------------

	EntityLaserBulletClient::EntityLaserBulletClient(World* pWorld, EntityLivingBase* pShooting, GunSetting* pSetting, const Vector3& beginPos, const Vector3& endPos, float contractiveValue)
		: EntityBulletClient(pWorld, pShooting, pSetting, beginPos, endPos)
	{
		setSize(1.0f, 1.0f);

		m_laserBeginPos = position;
		motion = (endPos - position).normalizedCopy() * m_shootingRange;
		m_laserEndPos = m_laserBeginPos + motion;
		m_lastLaserEndPos = m_laserEndPos;

		m_bulletEffectName = pSetting->bulletEffect != "effect_gun_default_bullet.effect" ? pSetting->bulletEffect : "effect_gun_laser_blue.effect";
		m_startEffectName = m_bulletEffectName.length() > 7 ? m_bulletEffectName.substr(0, m_bulletEffectName.length() - 7) + "_flame" + m_bulletEffectName.substr(m_bulletEffectName.length() - 7, m_bulletEffectName.length()) : "effect_gun_laser_blue_flame.effect";
		m_contractiveValue = contractiveValue;
		m_bulletType = BT_LASER;

		m_currTime = Root::Instance()->getCurrentTime();
		m_sound_type = pSetting->soundType;
		m_lastConsumeBulletTime = m_currTime;
	}

	EntityLaserBulletClient::~EntityLaserBulletClient()
	{
		if (m_needDeleteEffect && m_startEffect != nullptr)
		{
			m_startEffect->mDuration = 0;
			m_startEffect = nullptr;
		}
	}

	void EntityLaserBulletClient::onUpdate()
	{
		if (m_shootingEntity && m_shootingEntity->isDead)
		{
			m_shootingEntity = nullptr;
		}

		if (!m_shootingEntity)
		{
			ClientNetwork::Instance()->getSender()->sendStopLaserGun();
			setDead();
			return;
		}

		m_lastLaserEndPos = m_laserEndPos;

		m_record_time += (Root::Instance()->getCurrentTime() - m_currTime);

		if (m_record_time > 1130)
		{
			m_record_time = 0;
		}

		if (m_record_time == 0)
		{
			playSoundByType((SoundType)m_sound_type);
		}

		m_currTime = Root::Instance()->getCurrentTime();
		if (m_isShootingByMe)
		{
			if (m_currTime - m_lastConsumeBulletTime >= m_gunSetting->cdTime * 1000)
			{
				auto pPlayer = Blockman::Instance()->m_pPlayer;
				ItemStackPtr stack = pPlayer->inventory->getCurrentItem();

				if (!stack || stack->stackSize < 1)
				{
					ClientNetwork::Instance()->getSender()->sendStopLaserGun();
					setDead();
					return;
				}

				ItemGun* pGun = dynamic_cast<ItemGun*>(stack->getItem());
				if (!pGun)
				{
					ClientNetwork::Instance()->getSender()->sendStopLaserGun();
					setDead();
					return;
				}

				const GunSetting* pGunSetting = pGun->getGunSetting();
				if (!pGunSetting)
				{
					ClientNetwork::Instance()->getSender()->sendStopLaserGun();
					setDead();
					return;
				}

				int bulletNum = stack->getCurrentClipBulletNum();
				if (bulletNum < 1)
				{
					ClientNetwork::Instance()->getSender()->sendStopLaserGun();
					setDead();
					return;
				}
				else
				{
					stack->consumeBullet();
					ClientNetwork::Instance()->getSender()->sendConsumeBullet();
				}

				m_lastConsumeBulletTime = m_currTime;
				m_canHurt = true;
			}
		}

		Entity::onUpdate();

		rotationYaw = m_shootingEntity->rotationYaw;
		rotationPitch = m_shootingEntity->rotationPitch;

		Vector3 dir;
		dir.x = -float(Math::Sin(double(rotationYaw) * Math::DEG2RAD) * Math::Cos(double(rotationPitch) * Math::DEG2RAD));
		dir.y = -float(Math::Sin(double(rotationPitch) * Math::DEG2RAD));
		dir.z = float(Math::Cos(double(rotationYaw) * Math::DEG2RAD) * Math::Cos(double(rotationPitch) * Math::DEG2RAD));

		Vector3 beginPos;
		if (m_isShootingByMe)
		{
			beginPos = m_shootingEntity->position + Blockman::Instance()->m_pPlayer->m_thirdPersonCameraOffset;// position of crossHair
		}
		else
		{
			beginPos = calcPosition(m_shootingEntity, m_gunSetting, 0.0f);
		}
		Vector3 endPos = beginPos + dir * m_shootingRange;

		RayTraceResult trace = world->rayTraceBlocks(beginPos, endPos, false, true);
		float min_dis = m_shootingRange;
		float ratio = 1.0f;
		if (trace.result)
		{
			endPos = trace.hitVec;
			min_dis = beginPos.distanceTo(endPos);
			if (min_dis <= m_shootingRange && m_shootingRange > 0.00001f)
			{
				ratio = min_dis / m_shootingRange;
			}
			else
			{
				min_dis = m_shootingRange;
			}
		}

		Entity* tracyEntity = nullptr;
		EntityArr playerEntities = world->getEntitiesWithinRayTracy(m_shootingEntity, ENTITY_CLASS_ENTITY, beginPos, dir, m_shootingRange * ratio);
		for (int i = 0; i < int(playerEntities.size()); ++i)
		{
			Entity* pEntity = playerEntities[i];
			float distance = beginPos.distanceTo(pEntity->position);
			if (pEntity->canBePushed() && distance <= min_dis && pEntity != m_shootingEntity)
			{
				tracyEntity = pEntity;
				min_dis = distance;
			}
		}

		if (tracyEntity)
		{
			LORD::Ray ray(beginPos, endPos - beginPos);
			LORD::HitInfo hitinfo;
			float trim;
			const Box& aabb = tracyEntity->boundingBox;
			bool hit = ray.hitBox(aabb, trim, hitinfo);
			if (hit)
			{
				endPos = hitinfo.hitPos;
			}
		}

		m_laserBeginPos = calcPosition(m_shootingEntity, m_gunSetting, 0.0f);
		motion = (endPos - m_laserBeginPos).normalizedCopy() * m_shootingRange;
		m_laserEndPos = m_laserBeginPos + motion;

		trace = world->rayTraceBlocks(m_laserBeginPos, m_laserEndPos, false, true);
		min_dis = m_shootingRange;
		ratio = 1.0f;
		if (trace.result)
		{
			m_laserEndPos = trace.hitVec;
			min_dis = m_laserBeginPos.distanceTo(m_laserEndPos);
			if (min_dis <= motion.len() && motion.len() > 0.00001f)
			{
				ratio = min_dis / motion.len();
			}
			else
			{
				min_dis = m_shootingRange;
			}
		}

		if (m_isShootingByMe)
		{
			EntityArr entities = world->getEntitiesWithinAABBExcludingEntity(this, boundingBox.addCoord(motion.x * ratio, motion.y * ratio, motion.z * ratio));
			for (auto entity : entities)
			{
				if (entity && entity->canBePushed() && entity->entityId != m_shootingEntityId)
				{
					Box entity_box = entity->boundingBox;
					RayTraceResult tracy_entity = RayTraceResult::calculateIntercept(entity_box.contract(-m_contractiveValue), m_laserBeginPos, m_laserBeginPos + motion);
					if (tracy_entity.result && m_canHurt)
					{
						if (entity->isClass(ENTITY_CLASS_PLAYER))
						{
							EntityPlayer* tracyPlayer = dynamic_cast<EntityPlayer*>(entity);
							if (tracyPlayer && tracyPlayer->capabilities.disableDamage)
							{
								continue;
							}
							ClientNetwork::Instance()->getSender()->sendEffectGunFireResult(ClientNetwork::Instance()->getDataCache()->getServerId(entity->entityId), tracy_entity.hitVec, motion.normalizedCopy(), getGunID(), ENTITY_PLAYER_TRACE_TYPE, trace.getBlockPos());
						}
						else if (entity->isClass(ENTITY_CLASS_CREATURE))
						{
							ClientNetwork::Instance()->getSender()->sendEffectGunFireResult(ClientNetwork::Instance()->getDataCache()->getServerId(entity->entityId), tracy_entity.hitVec, motion.normalizedCopy(), getGunID(), ENTITY_CREATUREAI_TRACE_TYPE, trace.getBlockPos());
						}
						m_canHurt = false;
					}
				}
			}
		}

		setPosition(m_laserBeginPos);

		if (m_bulletEffect)
		{
			m_bulletEffect->mPosition = position;
			m_bulletEffect->setExtenalParam(position, m_laserEndPos);
			if (m_startEffect)
			{
				m_startEffect->mPosition = position;
			}
		}
	}

	//------------------------------------------------------------------------

	void EntityArmorPiercingBulletClient::onUpdate()
	{
		Entity::onUpdate();

		if (m_shootingEntity && m_shootingEntity->isDead)
		{
			m_shootingEntity = nullptr;
		}

		Vector3 begin = position;
		Vector3 entityEnd = position + motion;
		Vector3 blockEnd = position + motion;
		RayTraceResult trace = world->rayTraceBlocks(begin, blockEnd, false, true);
		float min_dis = m_shootingRange;
		float ratio = 1.0f;
		if (trace.result)
		{
			blockEnd = trace.hitVec;
			min_dis = begin.distanceTo(blockEnd);
			if (min_dis <= motion.len() && motion.len() > 0.00001f)
			{
				ratio = min_dis / motion.len();
			}
			else
			{
				min_dis = m_shootingRange;
			}
		}

		if (m_isShootingByMe)
		{
			EntityArr entities = world->getEntitiesWithinAABBExcludingEntity(this, boundingBox.addCoord(motion.x * ratio, motion.y * ratio, motion.z * ratio));
			for (auto entity : entities)
			{
				if (entity && entity->canBePushed() && begin.distanceTo(entity->position) <= m_shootingRange && entity->entityId != m_shootingEntityId)
				{
					auto iter = std::find(hitEntities.begin(), hitEntities.end(), entity->entityId);
					if (iter == hitEntities.end())
					{
						Box entity_box = entity->boundingBox;
						RayTraceResult tracy_entity = RayTraceResult::calculateIntercept(entity_box, begin, entityEnd);
						if (tracy_entity.result)
						{
							if (entity->isClass(ENTITY_CLASS_PLAYER))
							{
								EntityPlayer* tracyPlayer = dynamic_cast<EntityPlayer*>(entity);
								if (tracyPlayer && tracyPlayer->capabilities.disableDamage)
								{
									continue;
								}

								bool isHeadshot = false;
								if (LogicSetting::Instance()->getAllowHeadshotStatus())
								{
									EntityPlayer* pShooting = dynamic_cast<EntityPlayer*>(m_shootingEntity);
									if (tracyPlayer && pShooting && (LogicSetting::Instance()->getShowGunEffectWithSingleStatus() || tracyPlayer->getTeamId() != pShooting->getTeamId()))
									{
										isHeadshot = world->isEntityHeadWithinRayTracy(Box(Vector3(entity->boundingBox.vMin.x, entity->boundingBox.vMin.y + entity->getHeight() * 5 / 7, entity->boundingBox.vMin.z), entity->boundingBox.vMax), begin, motion.normalizedCopy(), begin.distanceTo(blockEnd));// head:body = 1:2.5
									}
								}

								ClientNetwork::Instance()->getSender()->sendEffectGunFireResult(ClientNetwork::Instance()->getDataCache()->getServerId(entity->entityId), tracy_entity.hitVec, motion.normalizedCopy(), getGunID(), ENTITY_PLAYER_TRACE_TYPE, trace.getBlockPos(), isHeadshot);
							}
							else if (entity->isClass(ENTITY_CLASS_CREATURE))
							{
								ClientNetwork::Instance()->getSender()->sendEffectGunFireResult(ClientNetwork::Instance()->getDataCache()->getServerId(entity->entityId), tracy_entity.hitVec, motion.normalizedCopy(), getGunID(), ENTITY_CREATUREAI_TRACE_TYPE, trace.getBlockPos());
							}
							hitEntities.push_back(entity->entityId);
						}
					}
				}
			}
		}

		if (trace.result && trace.typeOfHit == RAYTRACE_TYPE_BLOCK)
		{
			if (m_isShootingByMe && world->getBlockId(trace.getBlockPos()) > 0)
			{
				ClientNetwork::Instance()->getSender()->sendEffectGunFireResult(0, trace.hitVec, motion.normalizedCopy(), getGunID(), BLOCK_TRACE_TYPE, trace.getBlockPos());
			}

			if (m_bounceTimes > 0 && trace.sideHit)
			{
				prevPos = trace.hitVec - motion.normalizedCopy() * 0.05f;
				motion = motion.Reflect(trace.sideHit->getDirectionVec());
				position += motion;
				m_bounceTimes--;
			}
			else
			{
				motion = trace.hitVec - position;
				position = trace.hitVec - motion.normalizedCopy() * 0.05f;
				setDead();
			}
		}
		else
		{
			position += motion;
			m_shootingRange -= motion.len();
			if (m_shootingRange <= 0.0f)
			{
				setDead();
			}
		}

		setPosition(position);

		if (m_bulletEffect)
		{
			m_bulletEffect->mPosition = position;
		}
	}

	//------------------------------------------------------------------------

	EntityMomentaryLaserBulletClient::EntityMomentaryLaserBulletClient(World* pWorld, EntityLivingBase* pShooting, GunSetting* pSetting, const Vector3& beginPos, const Vector3& endPos)
		: EntityLaserBulletClient(pWorld, pShooting, pSetting, beginPos, endPos)
	{
		m_bulletType = BT_MOMENTARYLASER;
		m_needDeleteEffect = false;
	}

	void EntityMomentaryLaserBulletClient::onUpdate()
	{
		if (!m_hasUsed)
		{
			m_hasUsed = true;
			Entity::onUpdate();

			RayTraceResult trace = world->rayTraceBlocks(m_laserBeginPos, m_laserEndPos, false, true);
			float min_dis = m_shootingRange;
			float ratio = 1.0f;
			if (trace.result)
			{
				m_laserEndPos = trace.hitVec;
				min_dis = m_laserBeginPos.distanceTo(m_laserEndPos);
				if (min_dis <= motion.len() && motion.len() > 0.00001f)
				{
					ratio = min_dis / motion.len();
				}
				else
				{
					min_dis = m_shootingRange;
				}
			}

			Entity* tracyEntity = nullptr;
			Vector3 hitPos = Vector3::ZERO;
			EntityArr entities = world->getEntitiesWithinAABBExcludingEntity(this, boundingBox.addCoord(motion.x * ratio, motion.y * ratio, motion.z * ratio));
			for (auto entity : entities)
			{
				if (entity && entity->canBePushed() && entity->entityId != m_shootingEntityId)
				{
					Box entity_box = entity->boundingBox;
					RayTraceResult tracy_entity = RayTraceResult::calculateIntercept(entity_box, m_laserBeginPos, m_laserBeginPos + motion);
					if (tracy_entity.result)
					{
						float dis = m_laserBeginPos.distanceTo(tracy_entity.hitVec);
						if (dis < min_dis)
						{
							tracyEntity = entity;
							hitPos = tracy_entity.hitVec;
							min_dis = dis;
						}
					}
				}
			}
			if (tracyEntity)
			{
				trace.set(tracyEntity, hitPos);
				m_laserEndPos = hitPos;
			}

			if (trace.result)
			{
				if (m_isShootingByMe && trace.entityHit)
				{
					int traceType = NONE_TRACE_TYPE;
					if (trace.entityHit->isClass(ENTITY_CLASS_CREATURE))
					{
						traceType = ENTITY_CREATUREAI_TRACE_TYPE;
					}
					else if (trace.entityHit->isClass(ENTITY_CLASS_PLAYER))
					{
						traceType = ENTITY_PLAYER_TRACE_TYPE;
					}
					ClientNetwork::Instance()->getSender()->sendEffectGunFireResult(ClientNetwork::Instance()->getDataCache()->getServerId(trace.entityHit->entityId), trace.hitVec, motion.normalizedCopy(), getGunID(), traceType, trace.getBlockPos());
				}
			}
		}
		else if (m_existenceCnt >= 20)
		{
			setDead();
		}
		else
		{
			m_existenceCnt++;
		}
	}

	//------------------------------------------------------------------------

	void EntityMomentaryPiercingLaserBulletClient::onUpdate()
	{
		if (!m_hasUsed)
		{
			m_hasUsed = true;
			Entity::onUpdate();

			RayTraceResult trace = world->rayTraceBlocks(m_laserBeginPos, m_laserEndPos, false, true);
			float min_dis = m_shootingRange;
			float ratio = 1.0f;
			if (trace.result)
			{
				m_laserEndPos = trace.hitVec;
				min_dis = m_laserBeginPos.distanceTo(m_laserEndPos);
				if (min_dis <= motion.len() && motion.len() > 0.00001f)
				{
					ratio = min_dis / motion.len();
				}
				else
				{
					min_dis = m_shootingRange;
				}
			}

			if (m_isShootingByMe)
			{
				EntityArr entities = world->getEntitiesWithinAABBExcludingEntity(this, boundingBox.addCoord(motion.x * ratio, motion.y * ratio, motion.z * ratio));
				for (auto entity : entities)
				{
					if (entity && entity->canBePushed() && entity->entityId != m_shootingEntityId)
					{
						Box entity_box = entity->boundingBox;
						RayTraceResult tracy_entity = RayTraceResult::calculateIntercept(entity_box, m_laserBeginPos, m_laserBeginPos + motion);
						if (tracy_entity.result)
						{
							if (entity->isClass(ENTITY_CLASS_PLAYER))
							{
								EntityPlayer* tracyPlayer = dynamic_cast<EntityPlayer*>(entity);
								if (tracyPlayer && tracyPlayer->capabilities.disableDamage)
								{
									continue;
								}
								ClientNetwork::Instance()->getSender()->sendEffectGunFireResult(ClientNetwork::Instance()->getDataCache()->getServerId(entity->entityId), tracy_entity.hitVec, motion.normalizedCopy(), getGunID(), ENTITY_PLAYER_TRACE_TYPE, trace.getBlockPos());
							}
							else if (entity->isClass(ENTITY_CLASS_CREATURE))
							{
								ClientNetwork::Instance()->getSender()->sendEffectGunFireResult(ClientNetwork::Instance()->getDataCache()->getServerId(entity->entityId), tracy_entity.hitVec, motion.normalizedCopy(), getGunID(), ENTITY_CREATUREAI_TRACE_TYPE, trace.getBlockPos());
							}
						}
					}
				}
			}
		}
		else if (m_existenceCnt >= 20)
		{
			setDead();
		}
		else
		{
			m_existenceCnt++;
		}
	}

}