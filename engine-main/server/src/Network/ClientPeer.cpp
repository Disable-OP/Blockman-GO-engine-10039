#include "ClientPeer.h"
#include "Blockman/Entity/EntityPlayerMP.h"

#include "World/World.h"
#include "Server.h"
#include "ServerPacketSender.h"
#include "ServerNetwork.h"
#include "HttpRequest.h"
#include "ManorRequest.h"
#include "Script/GameServerEvents.h"
#include "Network/RoomClient.h"

#include "Util/LanguageKey.h"
#include "LuaRegister/Template/LuaEngine.h"
#include "Blockman/Entity/EntityTracker.h"
#include "rapidjson/document.h"
#include "Setting/MultiLanTipSetting.h"
#include "BM_Container_def.h"
#include "Entity/EntityVehicle.h"
#include "Entity/EntityActorNpc.h"
#include "World/Manor.h"
#include "Blockman/Entity/EntityCreatureAI.h"
#include "Blockman/World/BlockCropsManager.h"
#include "Network/protocol/S2CPackets.h"

#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
#define I64FMT "%I64d"
#define U64FMT "%I64u"
#else
#define I64FMT "%lld"
#define U64FMT "%llu"
#endif

using namespace rapidjson;

ClientPeer::ClientPeer(ui64 rakssid)
	: m_rakSsid(rakssid)
	, m_logon(false)
	, m_entityPlayer(NULL)
	, currentTicks(0)
	, ticksForFloatKick(0)
	, keepAliveRandomID(0)
	, keepAliveTimeSent(0)
	, ticksOfLastKeepAlive(0)
	, chatSpamThresholdCount(0)
	, creativeItemCreationSpamThresholdTally(0)
	, lastPos(Vector3::ZERO)
	, hasMoved(true)
{
	respawnPos = Server::Instance()->getInitPos();
}

ClientPeer::~ClientPeer()
{

}

void ClientPeer::removePlayerEntity()
{
	if (NULL != m_entityPlayer)
	{
		auto pWorld = Server::Instance()->getWorld();
		pWorld->removePlayerEntityDangerously(m_entityPlayer);
		LordLogInfo("player %s has destroyed", m_name.c_str());
	}
}

int ClientPeer::getEntityRuntimeID()
{
	if (NULL != m_entityPlayer)
		return m_entityPlayer->entityId;
	else
		return 0;
}

void ClientPeer::setTeamInfo(int teamId, String teamName)
{
	setTeamId(teamId);
	setTeamName(teamName);
	if (m_entityPlayer)
	{
		m_entityPlayer->setTeamId(teamId);
		m_entityPlayer->setTeamName(teamName);
	}
}

bool ClientPeer::isVisitor()
{
	return isPioneer() ? (m_platformUserId & 15) == 1 : ((m_platformUserId >> 15) & 1) == 1;
}

void ClientPeer::doAttack(int targetEngityRuntimeId, float targetX, float targetY, float targetZ)
{
	LordLogInfo("handlePacket_AttackPacket from %s, targetId=%d", m_name.c_str(), targetEngityRuntimeId);

	EntityPlayerMP*  targetPlayerEntity = nullptr;
	for (auto p : Server::Instance()->getWorld()->getPlayers())
	{
		if (p.second->entityId == targetEngityRuntimeId)
		{
			targetPlayerEntity = dynamic_cast<EntityPlayerMP*>(p.second);
			break;
		}
	}

	if (!targetPlayerEntity)
	{
		LordLogError("handlePacket AttackPacket from %s, target entity not found", m_name.c_str());
		//auto enti = Server::Instance()->getWorld()->getEntity(targetEngityRuntimeId);
		//if (enti)
		//{
		//	m_entityPlayer->attackTargetEntityWithCurrentItem(enti);
		//}
		return;
	}

	if (targetPlayerEntity->isOnVehicle())
		return;

	Vector3 clientTargetPos(targetX, targetY, targetZ);
	if (clientTargetPos.distanceTo(targetPlayerEntity->position) > 3)
	{
		LordLogError("handlePacket_AttackPacket from %s, target entity pos changed", m_name.c_str());
		ServerNetwork::Instance()->getSender()->syncPlayerMovement(this->getRakssid(), targetEngityRuntimeId);
		return;
	}


	bool attacked = m_entityPlayer->attackTargetEntityWithCurrentItem(targetPlayerEntity);
	if (!attacked)
	{
		LordLogInfo("handlePacket_AttackPacket from %s, attack fail", m_name.c_str());
	}
}

void ClientPeer::sendLoginResult(bool suc, int teamId, String teamName, int curPlayer, int maxPlayer)
{
	if (suc)
	{
		LordLogInfo("ClientPeer::sendLoginResult succ and sendUserIn to room, name[%s] platform uid[" U64FMT "] entityID[%d] rankID[" U64FMT "] teamId[%d] teamName[%s]",
			m_name.c_str(), m_platformUserId, m_entityPlayer->entityId, getRakssid(), teamId, teamName.c_str());
		setTeamInfo(teamId, teamName);
		ServerNetwork::Instance()->getSender()->sendLoginResult(getRakssid(), (int32_t)NETWORK_DEFINE::PacketLoginResult::emSucc);
		if (Server::Instance()->getEnableRoom())
		{
			Server::Instance()->getRoomManager()->getRoomClient()->sendUserIn(this->m_platformUserId, this->m_teamId);
		}
		ServerNetwork::Instance()->getSender()->sendUserIn(0, getPlatformUserId(), getName(), teamId, teamName, curPlayer, maxPlayer);
		for (auto player : ServerNetwork::Instance()->getPlayers())
		{
			if (player.second->getRakssid() != this->getRakssid() && player.second->m_logon)
			{
				ServerNetwork::Instance()->getSender()->sendUserIn(getRakssid(), player.second->getPlatformUserId(), 
					player.second->getName(), player.second->getTeamId(), player.second->getTeamName(), curPlayer, maxPlayer);
			}
		}
		
	}
	else
	{
		LordLogError("ClientPeer::sendLoginResult fail and do not sendUserIn to room, name[%s] platform uid[" U64FMT "] entityID[%d] rankID[" U64FMT "] teamId[%d] teamName[%s]",
			m_name.c_str(), m_platformUserId, m_entityPlayer->entityId, getRakssid(), teamId, teamName.c_str());
		ServerNetwork::Instance()->getSender()->sendLoginResult(getRakssid(), (int32_t)NETWORK_DEFINE::PacketLoginResult::emErrGetUserAttrFail);
	}
}

void ClientPeer::movePlayer(float toX, float toY, float toZ, float toYaw, float toPitch, bool onGround, bool toMoving, bool rotating, bool fromServer/* = false*/)
{
	if (!m_entityPlayer || m_entityPlayer->playerConqueredTheEnd)
		return;

	if (!fromServer && (m_entityPlayer->isOnVehicle() || m_entityPlayer->isOnAircraft()))
		return;

	/*LordLogInfo("movePlayer, from client, pos=(%f, %f, %f), onGround=%s, toMoving=%s", 
		toX, toY, toZ, onGround ? "true" : "false", toMoving ? "true" : "false");*/

	float dy = toY - lastPos.y;
	float dx = toX - lastPos.x;
	float dz = toZ - lastPos.z;
	float moveLenSqr = dx * dx + dy * dy + dz * dz;
	if (m_entityPlayer->isForceTeleport() && !fromServer)
	{
		if (moveLenSqr > 27)
		{
			LordLogInfo("Player move when the force teleport.");
			EntityTracker::Instance()->forceTeleport(m_entityPlayer->entityId);
			return;
		}
	}
	m_entityPlayer->forceTeleportEnd();

	bool moving = toMoving;
	float maxMoveDis = 3.0f; // incase of knockback distance: 3
	if (m_entityPlayer->hurtTime > 0 || !m_entityPlayer->onGround || !onGround)
	{
		maxMoveDis = 3000.0f;
	}

	if (!hasMoved)
	{
		if (moveLenSqr > 0.1 || m_entityPlayer->onGround != onGround) // 0.316 * 0.316 = 0.1
		{
			hasMoved = true;
		}
	}

	if (hasMoved)
	{
		lastPos = m_entityPlayer->position;
		Vector3 playerPos = m_entityPlayer->position;
		float yaw = m_entityPlayer->rotationYaw;
		float pitch = m_entityPlayer->rotationPitch;

		if (moving && toY == -999.0f)
		{
			moving = false;
		}


		if (moving)
		{
			playerPos.x = toX;
			playerPos.y = toY;
			playerPos.z = toZ;

			if (Math::Abs(toX) > 3.2E7f || Math::Abs(toZ) > 3.2E7f)
			{
				// kickPlayerFromServer("Illegal position");
				return;
			}
		}

		if (rotating)
		{
			yaw = toYaw;
			pitch = toPitch;
		}

		//m_entityPlayer->triggerEntityUpdate();
		m_entityPlayer->yPositionDecrement = 0.0F;
		m_entityPlayer->setPositionAndRotation(lastPos, yaw, pitch);

		Vector3 entityMotion = playerPos - m_entityPlayer->position;
		/*LordLogInfo("movePlayer, current server, pos=(%f, %f, %f), onGround=%s, entityMotion=(%f,%f,%f)",
			m_entityPlayer->position.x, m_entityPlayer->position.y, m_entityPlayer->position.z, m_entityPlayer->onGround ? "true" : "false",
			entityMotion.x, entityMotion.y, entityMotion.z);*/

		float motionLenSqr = entityMotion.x * entityMotion.x + entityMotion.z * entityMotion.z;
		if (!fromServer && motionLenSqr > maxMoveDis && !m_entityPlayer->isOnVehicle() && !m_entityPlayer->isOnAircraft())
		{
			setPlayerLocation(lastPos, m_entityPlayer->rotationYaw, m_entityPlayer->rotationPitch);
			EntityTracker::Instance()->forceTeleport(m_entityPlayer->entityId);
			LordLogError("movePlayer failed, move too fast, want to go(%f,%f,%f), set it to last pos(%f,%f,%f), motionLenSqr=%f", 
				playerPos.x, playerPos.y, playerPos.z, lastPos.x, lastPos.y, lastPos.z, motionLenSqr);
			return;
		}

		float extend = 0.0625F;
		World* pWorld = m_entityPlayer->world;
		bool noCollisionBeforeMove = pWorld->getCollidingBoundingBoxes
				(m_entityPlayer, m_entityPlayer->boundingBox.contract(extend)).empty();

		if (m_entityPlayer->onGround && !onGround && entityMotion.y > 0.0f)
		{
			m_entityPlayer->jump();
			entityMotion.y = 0;
		} 
		
		if (!m_entityPlayer->onGround) {
			entityMotion.y = 0;
		}
		
		m_entityPlayer->onGround = onGround;
		m_entityPlayer->moveEntity(entityMotion);
		m_entityPlayer->addMovementStat(entityMotion);
		m_entityPlayer->movementTimes();
		if (m_entityPlayer->canMovement() == false)
			m_entityPlayer->enableMovement();

		Vector3 serverToGoPos = m_entityPlayer->position;
		Vector3 detalPos = playerPos - serverToGoPos;
		detalPos.y = 0.0f;

		motionLenSqr = detalPos.lenSqr();
		bool wrongMove = false;

		if (motionLenSqr > maxMoveDis)
		{
			wrongMove = true;
			// mcServer.getLogAgent().logWarning(playerEntity.getCommandSenderName() + " moved wrongly!");
		}
		
		m_entityPlayer->setPositionAndRotation(playerPos, yaw, pitch);
		if (fromServer)
			lastPos = playerPos;

		bool collidedAfterMoved = !(pWorld->getCollidingBoundingBoxes(m_entityPlayer, m_entityPlayer->boundingBox.contract(extend)).empty());

		 if (!fromServer && noCollisionBeforeMove && (wrongMove || collidedAfterMoved) && !m_entityPlayer->isOnAircraft())
		{
			setPlayerLocation(lastPos, yaw, pitch); 
			EntityTracker::Instance()->forceTeleport(m_entityPlayer->entityId);
			LordLogError("playerId=%d, playerName=%s, movePlayer failed, something wrong happened, set it to last pos, last pos(%f,%f,%f),"
				"client want to go to (%f,%f,%f),server can only go to(%f,%f,%f), detalPos=(%f,%f,%f),"
				"diffLenSqr=%f, moving:%s, wrongMove:%s,collidedAfterMoved=%s", m_entityPlayer->entityId, m_entityPlayer->getShowName().c_str(),
				lastPos.x, lastPos.y, lastPos.z, playerPos.x, playerPos.y, playerPos.z, serverToGoPos.x, serverToGoPos.y, serverToGoPos.z,
				detalPos.x, detalPos.y, detalPos.z, motionLenSqr, (moving ? "true" : "false"),
				(wrongMove ? "true" : "false"), (collidedAfterMoved ? "true" : "false"));
			return;
		}

		Box aabb = m_entityPlayer->boundingBox.expand(extend).addCoord(0.0f, -0.55f, 0.0f);

		if (!pWorld->checkBlockCollision(aabb))
		{
			if (dy >= -0.03125f)
			{
				++ticksForFloatKick;

				if (ticksForFloatKick > 80)
				{
					// mcServer.getLogAgent().logWarning(playerEntity.getCommandSenderName() + " was kicked for floating too long!");
					// kickPlayerFromServer("Flying is not enabled on this server");
					return;
				}
			}
		}
		else
		{
			ticksForFloatKick = 0;
		}

		m_entityPlayer->onGround = onGround;
		m_entityPlayer->updateFlyingState(m_entityPlayer->position.y - dy, onGround);
	}
	else if (currentTicks % 20 == 0)
	{
		setPlayerLocation(lastPos, m_entityPlayer->rotationYaw, m_entityPlayer->rotationPitch);
	}
}

void ClientPeer::onSpawn(String name, EntityPlayerMP *mpPlayer)
{
	m_name = name;
	m_entityPlayer = mpPlayer;
	m_entityPlayer->setShowName(m_showName);
	lastPos = m_entityPlayer->position;
}

void ClientPeer::login()
{
	m_logon = true;
	SCRIPT_EVENT::PlayerLoginEvent::invoke(this);
	loadUserMoney();
	if (!LuaEngine::getInstance()->isLoadSuc()) {
		this->sendLoginResult(true, 0, "", 1, 8);
		ServerNetwork::Instance()->getSender()->sendSystemsChat(LanguageKey::SYSTEM_MESSAGE_PLAYER_ENTER_SERVER, getName(), 0);
	}
}

void ClientPeer::logout()
{
	if (!m_logon) {
		LordLogWarning("player unlogon, no need to logout");
		return;
	}

	if (!LuaEngine::getInstance()->isLoadSuc()) {
		ServerNetwork::Instance()->getSender()->sendSystemsChat(LanguageKey::SYSTEM_MESSAGE_PLAYER_EXIT_SERVER, getName(), 0);
	}

	ServerNetwork::Instance()->getSender()->sendUserOut(getPlatformUserId());
	String name = getName();
	LordLogInfo("ClientPeer::logout, sessionId_" U64FMT "_name_%s, platform id=" U64FMT, getRakssid(), name.c_str(), getPlatformUserId());

	SCRIPT_EVENT::PlayerLogoutEvent::invoke(this->getPlatformUserId());
	removePlayerEntity();
	
	if (Server::Instance()->getEnableRoom())
	{
		Server::Instance()->getRoomManager()->getRoomClient()->sendUserOut(this->m_platformUserId, this->m_teamId);
	}
	if (this->m_platformUserId == 0) {
		LordLogWarning("sending user out to room, platformId=" U64FMT ",m_logon=%s", 
			this->m_platformUserId, (this->m_logon ? "true" : "false"));
	}
	m_logon = false;

	if (m_entityPlayer) {
		m_entityPlayer->setLogout(true);
		m_entityPlayer->setDelayTicksToFree(60);
		m_entityPlayer = nullptr;
	}
}

void ClientPeer::resetPos(float x, float y, float z, float yaw)
{
	auto player = getEntityPlayer();
	player->motion = Vector3(0.0f, 0.0f, 0.0f);
	if (player != nullptr) 
	{
		if (yaw != 361)
		{
			player->rotationYaw = yaw;
		}
		//setPlayerLocation({ x, y, z }, player->rotationYaw, player->rotationPitch);
		movePlayer(x, y, z, player->rotationYaw, player->rotationPitch, player->onGround, true, true, true);
		EntityTracker::Instance()->forceTeleport(player->entityId);
		player->forceTeleportStart();
	}
}

void ClientPeer::setShowName(String name, ui64 targetId)
{
	m_showName = name;
	if (m_entityPlayer != NULL) {
		m_entityPlayer->setShowName(m_showName);
		if (targetId == 0)
		{
			Server::Instance()->getNetwork()->getSender()->broadCastChangePlayerInfo(m_entityPlayer);
		}
		else
		{
			Server::Instance()->getNetwork()->getSender()->sendChangePlayerInfo(targetId, m_entityPlayer);
		}
	}
}

void ClientPeer::setPlayerLocation(const Vector3& pos, float yaw, float pitch)
{
	hasMoved = false;
	lastPos = pos;
	m_entityPlayer->setPositionAndRotation(pos, yaw, pitch);
	m_entityPlayer->recordOldPos();
}

void ClientPeer::setPlatformUserId(ui64 platformUserId)
{
	this->m_platformUserId = platformUserId;
	if (m_entityPlayer)
	{
		m_entityPlayer->setPlatformUserId(platformUserId);
	}
}

void ClientPeer::onTick()
{
	// todo  send init blocks
	++currentTicks;

	if (currentTicks - ticksOfLastKeepAlive > 20L)
	{
		ticksOfLastKeepAlive = currentTicks;
		// keepAliveTimeSent = Root::Instance()->getCurrentTime() / 1000000L;
		// keepAliveRandomID = randomGenerator.nextInt();
		// sendPacketToPlayer(new Packet0KeepAlive(keepAliveRandomID));
	}

	if (chatSpamThresholdCount > 0)
	{
		--chatSpamThresholdCount;
	}

	if (creativeItemCreationSpamThresholdTally > 0)
	{
		--creativeItemCreationSpamThresholdTally;
	}
}

void ClientPeer::doGunShootTarget(ui32 tracyType, ui32 hitEntityID, const C2SPACKET_DETAIL::Vector3& hitpos,
	bool isHeadshot, const GunSetting* pGunSetting)
{
	EntityPlayer* targetPlayerEntity = nullptr;
	EntityVehicle* targetVehicleEntity = nullptr;
	EntityCreature* targetCreatureEntity = nullptr;
	EntityActorNpc* targetActorNpcEntity = nullptr;

	switch (tracyType)
	{
	case ENTITY_PLAYER_TRACE_TYPE:
		targetPlayerEntity = dynamic_cast<EntityPlayer*>(Server::Instance()->getWorld()->getPlayerEntity(hitEntityID));
		if (targetPlayerEntity) {
			m_entityPlayer->shootTargetByGun(targetPlayerEntity, hitpos, isHeadshot, pGunSetting);
		}
		break;
	case ENTITY_VEHICLE_TRACE_TYPE:
		targetVehicleEntity = dynamic_cast<EntityVehicle*>(Server::Instance()->getWorld()->getEntity(hitEntityID));
		if (targetVehicleEntity) {
			targetVehicleEntity->shootTargetByGun(m_entityPlayer, hitpos, pGunSetting);
		}
		break;
	case ENTITY_CREATUREAI_TRACE_TYPE:
		targetCreatureEntity = dynamic_cast<EntityCreatureAI*>(Server::Instance()->getWorld()->getEntity(hitEntityID));
		if (targetCreatureEntity) {
			m_entityPlayer->attackCreatureEntity(targetCreatureEntity, (int)PLAYER_ATTACK_CREATURE_GUN);
		}
		break;
	case ENTITY_ACTOR_NPC_TRACE_TYPE:
		targetActorNpcEntity = dynamic_cast<EntityActorNpc*>(Server::Instance()->getWorld()->getEntity(hitEntityID));
		if (targetActorNpcEntity) {
			m_entityPlayer->attackActorNpc(targetActorNpcEntity, PLAYER_ATTACK_GUN);
		}
		break;
	default:
		break;
	}

}

void ClientPeer::reloadBullet()
{
	m_entityPlayer->reloadBullet();
}


void ClientPeer::loadUserMoney()
{
	String url = getHttpUrl();
	if (url.length() == 0)
	{
		LordLogError("load use money error: url is null");
		return;
	}
	HttpRequest::loadUserMoney(url, m_platformUserId,
		[](i64 platformUserId, String result) {
			auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
			if (!peer || !peer->getEntityPlayer())
			{
				return;
			}
			Document* doc = new Document;
			doc->Parse(result.c_str());
			auto player = peer->getEntityPlayer();
			if (doc->HasParseError())
			{	
				delete doc;
				doc = nullptr;
				return;
			}
			if (doc->HasMember("code"))
			{
				int code = doc->FindMember("code")->value.GetInt();
				if (code != 1)
				{
					delete doc;
					doc = nullptr;
					return;
				}
				if (doc->HasMember("data"))
				{
					auto data = doc->FindMember("data")->value.GetObject();
					i64 diamonds = 0;
					i64 golds = 0;
					if (data.HasMember("diamonds"))
					{
						diamonds = data.FindMember("diamonds")->value.GetInt64();
						player->m_wallet->setDiamonds(diamonds);
					}
					if (data.HasMember("golds"))
					{
						golds = data.FindMember("golds")->value.GetInt64();
						player->m_wallet->setGolds(golds);
					}
					
					ServerNetwork::Instance()->getSender()->sendInitMoney(peer->getRakssid(), diamonds, golds);
				}
			}
			delete doc;
			doc = nullptr;
	});
}

void ClientPeer::buyGoods(i32 groupIndex, i32 goodsIndex, i32 goodsId)
{
	String url = getHttpUrl();
	if (url.length() == 0)
	{
		LordLogError("buy goods error: url is null");
		return;
	}
	Shop* shop = Server::Instance()->getWorld()->getShop();
	if (!shop)
		return;
	auto group = shop->getGoods()[groupIndex];
	auto goods = group[goodsIndex];
	if (goods.uniqueId != goodsId)
	{
		LordLogError("goods uniqueId is invalid, goodsId:", goodsId);
		return;
	}
	if (!goods.canBuy())
	{
		ServerNetwork::Instance()->getSender()->
			sendShoppingResult(getRakssid(), 2, "inventory shortage");
		return;
	}
	Wallet* wallet = getEntityPlayer()->m_wallet;
	if (!wallet)
	{
		LordLogError("wallet is null");
		return;
	}
	ui64 money = wallet->getMoneyByCoinId(goods.coinId);
	bool pioneer = isPioneer();
	i32 price = pioneer ? goods.blockymodsPrice : goods.blockmanPrice;
	if (money < price)
	{
		ServerNetwork::Instance()->getSender()->
			sendShoppingResult(getRakssid(), 3, "not enough money");
		return;
	}
	if (!getEntityPlayer()->getInventory())
	{
		return;
	}
	if (!getEntityPlayer()->getInventory()->isCanAddItem(goods.itemId, goods.itemMeta, goods.itemNum))
	{
		ServerNetwork::Instance()->getSender()->sendShoppingResult(this->getRakssid(), 4, "player inventory full");
		return;
	}

	if (!getEntityPlayer()->canAddBulletItem(goods.itemId))
	{
		ServerNetwork::Instance()->getSender()->sendShoppingResult(this->getRakssid(), 4, "has no gun");
		return;
	}
	IntProxy msgType;
	msgType.value = 0;
	BoolProxy isAddItem;
	isAddItem.value = true;
	if (!SCRIPT_EVENT::PlayerBuyGoodsEvent::invoke(getRakssid(), group.getCategory(), goods.itemId, &msgType, &isAddItem))
	{
		if (msgType.value != 0)
		{
			auto lang = getLanguage();
			std::string msg = MultiLanTipSetting::getMessage(lang, msgType.value, "");
			ServerNetwork::Instance()->getSender()->sendShoppingResult(this->getRakssid(), 5, msg.c_str());
		}
		return;
	}
	shop->buyGoods(groupIndex, goodsIndex);
	if (goods.coinId <= 2)
	{
		HttpRequest::buyGoods(url, price, m_platformUserId, groupIndex, goodsIndex, isAddItem.value,
			[](i64 platformUserId, String result, i32 groupIndex, i32 goodsIndex, bool isAddItem, bool isSuccess) {
			Shop* shop = Server::Instance()->getWorld()->getShop();
			if (!shop)
			{
				return;
			}

			auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
			if (!peer || !peer->getEntityPlayer())
			{
				shop->resumeBuyGoods(groupIndex, goodsIndex);
				return;
			}
			
			if (!isSuccess)
			{
				shop->resumeBuyGoods(groupIndex, goodsIndex);
				ServerNetwork::Instance()->getSender()->sendShoppingResult(peer->getRakssid(), 6, "buy failed");
				return;
			}
			Document* doc = new Document;
			doc->Parse(result.c_str());
			if (doc->HasParseError())
			{
				delete doc;
				doc = nullptr;
				shop->resumeBuyGoods(groupIndex, goodsIndex);
				ServerNetwork::Instance()->getSender()->sendShoppingResult(peer->getRakssid(), 6, "buy failed");
				return;
			}
			if (doc->HasMember("code"))
			{
				int code = doc->FindMember("code")->value.GetInt();
				if (code != 1)
				{
					delete doc;
					doc = nullptr;
					shop->resumeBuyGoods(groupIndex, goodsIndex);
					ServerNetwork::Instance()->getSender()->sendShoppingResult(peer->getRakssid(), 6, "buy failed");
					return;
				}
				if (doc->HasMember("data"))
				{
					auto player = peer->getEntityPlayer();
					auto group = shop->getGoods()[groupIndex];
					auto goods = group[goodsIndex];
					if (goods.itemId >= 10000)
					{
						player->addCurrency(goods.itemNum);
						ManorRequest::trade(player->getPlatformUserId(), goods.itemNum);
					}
					else
					{
						if (isAddItem)
						{
							if (SCRIPT_EVENT::PlayerBuyGoodsAddItemEvent::invoke(peer->getRakssid(), goods.itemId, goods.itemNum, goods.itemMeta))
							{
								player->addItem(goods.itemId, goods.itemNum, goods.itemMeta);
							}
							ServerNetwork::Instance()->getSender()->notifyGetItem(peer->getRakssid(), goods.itemId, goods.itemMeta, goods.itemNum);
						}
					}
					SCRIPT_EVENT::PlayerBuyGoodsSuccessEvent::invoke(peer->getRakssid(), group.getCategory(), goods.itemId);
					auto data = doc->FindMember("data")->value.GetObject();
					i64 diamonds = 0;
					i64 golds = 0;
					if (data.HasMember("diamonds"))
					{
						diamonds = data.FindMember("diamonds")->value.GetInt64();
						player->m_wallet->setDiamonds(diamonds);
					}
					if (data.HasMember("golds"))
					{
						golds = data.FindMember("golds")->value.GetInt64();
						player->m_wallet->setGolds(golds);
					}
					ServerNetwork::Instance()->getSender()->sendInitMoney(peer->getRakssid(), diamonds, golds);
					if (data.HasMember("orderId"))
					{
						auto orderId = data.FindMember("orderId")->value.GetString();
						ServerNetwork::Instance()->getSender()->sendShoppingResult(peer->getRakssid(), code, String(orderId));
					}
				}
			}
			delete doc;
			doc = nullptr;
		});
	}
	else
	{
		auto player = getEntityPlayer();
		player->subCurrency(price);
		player->addItem(goods.itemId, goods.itemNum, goods.itemMeta);
		ManorRequest::trade(player->getPlatformUserId(), -price);
		ServerNetwork::Instance()->getSender()->sendShoppingResult(this->getRakssid(), 6, "success");
		ServerNetwork::Instance()->getSender()->notifyGetItem(this->getRakssid(), goods.itemId, goods.itemMeta, goods.itemNum);
	}
}

void ClientPeer::buyRespawn(bool isRespawn, i32 uniqueId, i32 index)
{
	Shop* shop = Server::Instance()->getWorld()->getShop();
	if (!shop)
	{
		SCRIPT_EVENT::PlayerBuyRespawnResultEvent::invoke(getRakssid(), 0);
		return;
	}
	shop->removeRespawnRecord(getRakssid());
	if (index == -1)
	{
		SCRIPT_EVENT::PlayerBuyRespawnResultEvent::invoke(getRakssid(), 0);
		return;
	}
	bool canBuy = true;
	if (!isRespawn)
	{
		canBuy = false;
	}
	String url = getHttpUrl();
	if (url.length() == 0)
	{
		LordLogError("buy respawn error: url is null");
		canBuy = false;
	}
	RespawnGoods goods = shop->getRespawnGoods()[index];
	if (goods.uniqueId == -1 || goods.uniqueId != uniqueId)
	{
		canBuy = false;
	}
	ui64 money = getEntityPlayer()->m_wallet->getMoneyByCoinId(goods.coinId);
	bool pioneer = isPioneer();
	i32 price = pioneer ? goods.blockymodsPrice : goods.blockmanPrice;
	if (money < price)
	{
		canBuy = false;
	}
	if (!canBuy)
	{
		SCRIPT_EVENT::PlayerBuyRespawnResultEvent::invoke(getRakssid(), 0);
		return;
	}
	HttpRequest::buyRespawn(url, goods.uniqueId, price, m_platformUserId,
		[](i64 platformUserId, String result, bool isSuccess) {
		
		auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
		if (!peer || !peer->getEntityPlayer())
		{
			return;
		}

		bool buySuccess = true;
		if (!isSuccess)
		{
			buySuccess = false;
		}
		else
		{
			Document* doc = new Document;
			doc->Parse(result.c_str());
			if (doc->HasParseError())
			{
				buySuccess = false;
			}
			if (doc->HasMember("code"))
			{
				int code = doc->FindMember("code")->value.GetInt();
				if (code != 1)
				{
					buySuccess = false;
				}
				if (code == 1 && doc->HasMember("data"))
				{
					auto player = peer->getEntityPlayer();
					auto data = doc->FindMember("data")->value.GetObject();
					i64 diamonds = 0;
					i64 golds = 0;
					if (data.HasMember("diamonds"))
					{
						diamonds = data.FindMember("diamonds")->value.GetInt64();
						player->m_wallet->setDiamonds(diamonds);
					}
					if (data.HasMember("golds"))
					{
						golds = data.FindMember("golds")->value.GetInt64();
						player->m_wallet->setGolds(golds);
					}
					ServerNetwork::Instance()->getSender()->sendInitMoney(peer->getRakssid(), diamonds, golds);
					if (data.HasMember("orderId"))
					{
						auto orderId = data.FindMember("orderId")->value.GetString();
						ServerNetwork::Instance()->getSender()->sendBuyRespawnResult(peer->getRakssid(), code, String(orderId));
					}
				}
			}
			delete doc;
			doc = nullptr;
		}
		SCRIPT_EVENT::PlayerBuyRespawnResultEvent::invoke(peer->getRakssid(), buySuccess ? 1 : 0);
	});
}

void ClientPeer::buyChangeActor(bool isCanChangeActor, int changeActorCount, int needMoneyCount)
{
	if (changeActorCount == 0 || needMoneyCount == 0 || isCanChangeActor)
	{
		this->getEntityPlayer()->setChangePlayerActor(isCanChangeActor, changeActorCount, needMoneyCount);
	}
	else 
	{
		String url = getHttpUrl();
		if (url.length() == 0)
		{
			LordLogError("buy change actor error: url is null");
			SCRIPT_EVENT::PlayerBuyActorResultEvent::invoke(this->getRakssid(), false);
		}
		HttpRequest::buyChangeActor(url, changeActorCount, needMoneyCount, m_platformUserId,
			[](i64 platformUserId, String result, bool isSuccess) {

			auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
			if (!peer || !peer->getEntityPlayer())
			{
				return;
			}

			if (!isSuccess)
			{
				return;
			}

			Document* doc = new Document;
			doc->Parse(result.c_str());
			if (!doc->HasParseError() && doc->HasMember("code"))
			{
				int code = doc->FindMember("code")->value.GetInt();
				if (code == 1 && doc->HasMember("data"))
				{
					auto player = peer->getEntityPlayer();
					auto data = doc->FindMember("data")->value.GetObject();
					i64 diamonds = 0;
					i64 golds = 0;
					if (data.HasMember("diamonds"))
					{
						diamonds = data.FindMember("diamonds")->value.GetInt64();
						player->m_wallet->setDiamonds(diamonds);
					}
					if (data.HasMember("golds"))
					{
						golds = data.FindMember("golds")->value.GetInt64();
						player->m_wallet->setGolds(golds);
					}
					ServerNetwork::Instance()->getSender()->sendInitMoney(peer->getRakssid(), diamonds, golds);
					if (data.HasMember("orderId"))
					{
						auto orderId = data.FindMember("orderId")->value.GetString();
						String url = peer->getHttpUrl();
						if (url.length() == 0)
						{
							LordLogError("resume order error: url is null");
						}
						HttpRequest::resumeOrder(url, orderId);
					}
					player->setChangePlayerActor(true, 1, 0);
				}
			}
			delete doc;
			doc = nullptr;
		});
	}
}

void ClientPeer::pickupItemPay(i32 itemId, i32 itemMeta, i32 addNum, i32 price, i32 entityItemId, i32 currencyType)
{
	String url = getHttpUrl();
	if (url.length() == 0)
	{
		LordLogError("pickup Item pay error: url is null");
		ServerNetwork::Instance()->getSender()->sendRequestResult(getRakssid(), itemId, ResultCode::PAY_FAILURE, "pay failure");
		return;
	}

	Wallet* wallet = getEntityPlayer()->m_wallet;
	if (!wallet)
	{
		LordLogError("wallet is null");
		ServerNetwork::Instance()->getSender()->sendRequestResult(getRakssid(), itemId, ResultCode::PAY_FAILURE, "pay failure");
		return;
	}
	ui64 money = wallet->getMoneyByCoinId(currencyType);
	if (money < price)
	{
		ServerNetwork::Instance()->getSender()->sendRequestResult(getRakssid(), itemId, ResultCode::LACK_MONEY, "not enough money");
		return;
	}

	if (!getEntityPlayer()->getInventory() || !getEntityPlayer()->getInventory()->isCanAddItem(itemId, itemMeta, addNum))
	{
		ServerNetwork::Instance()->getSender()->sendRequestResult(getRakssid(), itemId, ResultCode::INVENTORY_FULL, "inventory is full");
		return;
	}

	HttpRequest::pay(url, currencyType, itemId, price, m_platformUserId, 
		[itemId, itemMeta, addNum, price, entityItemId](i64 platformUserId, String result, bool isSuccess) 
	{
		auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
		if (!peer || !peer->getEntityPlayer())
		{
			return;
		}

		bool paySuccess = peer->payResult(platformUserId, result, isSuccess);
		SCRIPT_EVENT::PlayerBuyPickupItemResultEvent::invoke(
			peer->getRakssid(), paySuccess, itemId, itemMeta, addNum, price, entityItemId);

		if (paySuccess)
		{
			ServerNetwork::Instance()->getSender()->sendRequestResult(peer->getRakssid(), itemId, ResultCode::PAY_SUCCESS, "pay success");
			ServerNetwork::Instance()->getSender()->notifyGetItem(peer->getRakssid(), itemId, itemMeta, addNum);
		}
		else
		{
			ServerNetwork::Instance()->getSender()->sendRequestResult(peer->getRakssid(), itemId, ResultCode::PAY_FAILURE, "pay failure");
			LordLogInfo("pickup Item [%d] pay failure", itemId);
		}
	});
}

bool ClientPeer::checkParam(String url, i32 currencyType, i32 price)
{
	if (url.length() == 0)
	{
		LordLogError("check param: url is null");
		ServerNetwork::Instance()->getSender()->sendRequestResult(getRakssid(), 0, ResultCode::PAY_FAILURE, "pay failure");
		return false;
	}

	Wallet* wallet = getEntityPlayer()->m_wallet;
	if (!wallet)
	{
		LordLogError("wallet is null");
		ServerNetwork::Instance()->getSender()->sendRequestResult(getRakssid(), 0, ResultCode::PAY_FAILURE, "pay failure");
		return false;
	}
	ui64 money = wallet->getMoneyByCoinId(currencyType);
	if (money < price)
	{
		ServerNetwork::Instance()->getSender()->sendRequestResult(getRakssid(), 0, ResultCode::LACK_MONEY, "not enough money");
		return false;
	}
	return true;
}

void ClientPeer::upgradeManor()
{
	String url = getHttpUrl();
	String dataServerUrl = Server::Instance()->getDataServerUrl();

	if (dataServerUrl.length() == 0)
	{
		LordLogError("upgrade manor  pay error: dataServerUrl is null");
		ServerNetwork::Instance()->getSender()->sendRequestResult(getRakssid(), 0, ResultCode::PAY_FAILURE, "pay failure");
		return;
	}

	ManorInfo* info = getEntityPlayer()->m_manor->getNextInfo();
	if (info && info->level > info->maxLevel)
	{
		return;
	}
	if (info && checkParam(url, (i32)info->currencyType, info->price))
	{
		i32 currencyType = (i32)info->currencyType;
		i32 price = info->price;
		i32 level = info->level;

		i64 platformUserId = m_platformUserId;
		ManorRequest::pay(url, dataServerUrl, currencyType, level, price, m_platformUserId, 
			[price, currencyType, platformUserId, dataServerUrl](String result, bool isSuccess)
		{
			bool paySuccess = false;
			i32  exchangePrice = price;
			if ((i32)currencyType <=2 )
			{
				IntProxy rate;
				rate.value = price;
				SCRIPT_EVENT::GetMoneyExchangeRateEvent::invoke(&rate);
				auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
				if (peer && peer->getEntityPlayer())
				{
					paySuccess = peer->payResult(platformUserId, result, isSuccess);
				}
				
				exchangePrice = rate.value;
			}
			else
			{
				paySuccess = ManorRequest::manorPayResult(platformUserId, result, isSuccess);
			}
			
			if (paySuccess)
			{
				ManorRequest::upgradeManor(dataServerUrl, platformUserId, exchangePrice, 
					[platformUserId, exchangePrice](String result, bool isSuccess) 
				{
					String logTag = StringUtil::Format("upgradeManor(UserId=%lld)", platformUserId);
					HttpResponse  response = ManorRequest::parseResponse(isSuccess, logTag, result);

					auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
					if (!peer || !peer->getEntityPlayer())
					{
						if (response.code != 1) {
							ManorRequest::trade(platformUserId, exchangePrice);
						}
						return;
					}

					SCRIPT_EVENT::PlayerUpdateManorResultEvent::invoke(peer->getRakssid(), response.code == 1, response.data);
					if (response.code == 1)
					{
						ServerNetwork::Instance()->getSender()->sendRequestResult(peer->getRakssid(), 0, ResultCode::PAY_SUCCESS, "pay success");
					}
					else
					{
						ServerNetwork::Instance()->getSender()->sendRequestResult(peer->getRakssid(), 0, ResultCode::COMMON, LanguageKey::GUI_MANOR_UPGRADE_FAILURE);
						//Refund for payment failure
						ManorRequest::trade(platformUserId, exchangePrice);
					}
				});
			}
			else
			{
				auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
				if (peer) {
					ServerNetwork::Instance()->getSender()->sendRequestResult(peer->getRakssid(), 0, ResultCode::PAY_FAILURE, "pay failure");
				}
			}
		});
	}
}

void ClientPeer::buyHouse(i32 houseId)
{
	String url = getHttpUrl();
	String dataServerUrl = Server::Instance()->getDataServerUrl();
	String manorId = getEntityPlayer()->m_manor->getManorId();
	if (dataServerUrl.length() == 0 || manorId.length() == 0)
	{
		LordLogError("buy house  pay error: dataServerUrl is null or manorId is null");
		ServerNetwork::Instance()->getSender()->sendRequestResult(getRakssid(), 0, ResultCode::PAY_FAILURE, "pay failure");
		return;
	}
	//TODO
	ManorHouse* info = getEntityPlayer()->m_manor->findHouseById(houseId);
	if (info && checkParam(url, (i32)info->currencyType, info->price) )
	{
		i32 currencyType = (i32)info->currencyType;
		i32 price = info->price;
		i32 itemId = info->id;
		i32 charm = info->charm;
		i64 platformUserId = m_platformUserId;

		ManorRequest::pay(url, dataServerUrl, currencyType, itemId, price, m_platformUserId, 
			[price, currencyType, platformUserId, dataServerUrl, manorId, itemId, charm](String result, bool isSuccess)
		{
			bool paySuccess = false;
			i32  exchangePrice = price;
			if (currencyType <= 2)
			{
				IntProxy rate;
				rate.value = price;
				SCRIPT_EVENT::GetMoneyExchangeRateEvent::invoke(&rate);
				auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
				if (peer && peer->getEntityPlayer())
				{
					paySuccess = peer->payResult(platformUserId, result, isSuccess);
				}

				exchangePrice = rate.value;
			}
			else
			{
				paySuccess = ManorRequest::manorPayResult(platformUserId, result, isSuccess);
			}

			if (paySuccess)
			{
				
				ManorRequest::buyHouse(dataServerUrl, manorId, itemId, exchangePrice, charm, 
					[platformUserId, manorId, exchangePrice, paySuccess, itemId, price, charm](String result, bool isSuccess)
				{
					String logTag = StringUtil::Format("buyHouse(UserId=%lld , manorId[%s])", platformUserId, manorId.c_str());
					HttpResponse  response = ManorRequest::parseResponse(isSuccess, logTag, result);
					auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
					if (!peer || !peer->getEntityPlayer())
					{
						if (response.code != 1) {
							ManorRequest::trade(platformUserId, exchangePrice);
						}
						return;
					}
					if (response.code == 1)
					{
						ServerNetwork::Instance()->getSender()->sendRequestResult(peer->getRakssid(), 0, ResultCode::PAY_SUCCESS, "pay success");
						SCRIPT_EVENT::PlayerBuyHouseResultEvent::invoke(peer->getRakssid(), paySuccess, itemId, price, charm);
					}
					else
					{
						ServerNetwork::Instance()->getSender()->sendRequestResult(peer->getRakssid(), 0, ResultCode::PAY_FAILURE, "pay failure");
						//Refund for payment failure
						ManorRequest::trade(platformUserId, exchangePrice);
					}
				});
			}
			else
			{
				auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
				if (peer) {
					ServerNetwork::Instance()->getSender()->sendRequestResult(peer->getRakssid(), 0, ResultCode::PAY_FAILURE, "pay failure");
				}
			}
		});
	}
}

void ClientPeer::buyFurniture(i32 furnitureId)
{
	String url = getHttpUrl();
	String dataServerUrl = Server::Instance()->getDataServerUrl();
	String manorId = getEntityPlayer()->m_manor->getManorId();
	if (dataServerUrl.length() == 0 || manorId.length() == 0)
	{
		LordLogError("buy furniture  pay error: dataServerUrl is null or manorId is null");
		ServerNetwork::Instance()->getSender()->sendRequestResult(getRakssid(), 0, ResultCode::PAY_FAILURE, "pay failure");
		return;
	}

	ManorFurniture* info = getEntityPlayer()->m_manor->findFurnitureById(furnitureId);
	
	if (info && checkParam(url, (i32)info->currencyType, info->price))
	{
		i32 currencyType = (i32)info->currencyType;
		i32 price = info->price;
		i32 itemId = info->id;
		i32 charm = info->charm;
		i64 platformUserId = m_platformUserId;

		ManorRequest::pay(url, dataServerUrl, currencyType, itemId, price, m_platformUserId, 
			[platformUserId, price, currencyType, dataServerUrl, manorId, itemId, charm](String result, bool isSuccess)
		{
			bool paySuccess = false;
			i32  exchangePrice = price;
			if (currencyType <= 2)
			{
				IntProxy rate;
				rate.value = price;
				SCRIPT_EVENT::GetMoneyExchangeRateEvent::invoke(&rate);
				auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
				if (peer && peer->getEntityPlayer()) {
					paySuccess = peer->payResult(platformUserId, result, isSuccess);
				}
				exchangePrice = rate.value;
			}
			else
			{
				paySuccess = ManorRequest::manorPayResult(platformUserId, result, isSuccess);
			}

			if (paySuccess)
			{
				ManorRequest::buyFurniture(dataServerUrl, manorId, itemId, exchangePrice, charm, 
					[platformUserId, manorId, paySuccess, itemId, price, exchangePrice](String result, bool isSuccess)
				{
					String logTag = StringUtil::Format("buyFurniture(UserId=%lld , manorId[%s])", platformUserId, manorId.c_str());
					HttpResponse  response = ManorRequest::parseResponse(isSuccess, logTag, result);
					auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
					if (!peer || !peer->getEntityPlayer()) {
						if (response.code != 1) {
							ManorRequest::trade(platformUserId, exchangePrice);
						}
						return;
					}
					if (response.code == 1)
					{
						SCRIPT_EVENT::PlayerBuyFurnitureResultEvent::invoke(peer->getRakssid(), paySuccess, itemId, price);
						ServerNetwork::Instance()->getSender()->sendRequestResult(peer->getRakssid(), 0, ResultCode::PAY_SUCCESS, "pay success");
					}
					else
					{
						ServerNetwork::Instance()->getSender()->sendRequestResult(peer->getRakssid(), 0, ResultCode::PAY_FAILURE, "pay failure");
						//Refund for payment failure
						ManorRequest::trade(platformUserId, exchangePrice);
					}
				});
			}
			else
			{
				auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
				if (peer) {
					ServerNetwork::Instance()->getSender()->sendRequestResult(peer->getRakssid(), 0, ResultCode::PAY_FAILURE, "pay failure");
				}
			}
		});
	}
}

void ClientPeer::consumeDiamonds(i32 uniqueId, i32 diamonds, String remark, bool isConsume)
{
	String url = getHttpUrl();
	if (url.length() == 0)
	{
		SCRIPT_EVENT::ConsumeDiamondsEvent::invoke(getRakssid(), false, "url.nil", remark, "");
		return;
	}
	Wallet* wallet = getEntityPlayer()->m_wallet;
	if (!wallet)
	{
		SCRIPT_EVENT::ConsumeDiamondsEvent::invoke(getRakssid(), false, "wallet.nil", remark, "");
		return;
	}
	ui64 money = wallet->getMoneyByCoinId(1);
	if (money < diamonds)
	{
		SCRIPT_EVENT::ConsumeDiamondsEvent::invoke(getRakssid(), false, "diamonds.not.enough", remark, "");
		return;
	}
	HttpRequest::pay(url, 1, uniqueId, diamonds, m_platformUserId, 
		[isConsume, remark](i64 platformUserId, String result, bool isSuccess) 
	{
		auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
		if (!peer || !peer->getEntityPlayer())
		{
			return;
		}
		//bool paySuccess = payResult(result, isSuccess);
		OrderPayResult payResult = peer->payResultByConsume(platformUserId, isSuccess, isConsume, result);
		if (payResult.code == 1)
		{
			SCRIPT_EVENT::ConsumeDiamondsEvent::invoke(peer->getRakssid(), true, "pay.success", remark, payResult.orderId);
		}
		else
		{
			SCRIPT_EVENT::ConsumeDiamondsEvent::invoke(peer->getRakssid(), false, "pay.failed", remark, "");
		}
	});
}

void ClientPeer::buyRanchItem(i32 itemId, i32 addNum, i32 price, i32 currencyType)
{
	String url = getHttpUrl();
	

	BoolProxy canSave;
	canSave.value = false;

	IntProxy totalPrice;
	totalPrice.value = 0;

	std::vector<RanchCommon> items = std::vector<RanchCommon>();
	items.push_back(RanchCommon(itemId, addNum, 0));

	SCRIPT_EVENT::CanRanchStorageSaveItemsEvent::invoke(getRakssid(), items, currencyType, &totalPrice, &canSave);

	if (canSave.value == false)
	{
		ServerNetwork::Instance()->getSender()->sendRequestResult(getRakssid(), itemId, ResultCode::RANCH_STORAGE_FULL, "gui_ranch_storage_full_operation_failure");
		return;
	}

	bool check = checkParam(url, currencyType, totalPrice.value);
	if (!check)
	{
		return;
	}

	if (currencyType < 3)
	{
		HttpRequest::pay(url, currencyType, 1031010, totalPrice.value, m_platformUserId,
			[items, itemId](i64 platformUserId, String result, bool isSuccess) 
		{
			auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
			if (!peer || !peer->getEntityPlayer())
			{
				return;
			}
			bool paySuccess = peer->payResult(platformUserId, result, isSuccess);
			SCRIPT_EVENT::PlayerBuyRanchItemResultEvent::invoke(peer->getRakssid(), paySuccess, items);
			if (!paySuccess)
			{
				ServerNetwork::Instance()->getSender()->sendRequestResult(peer->getRakssid(), itemId, ResultCode::PAY_FAILURE, "pay failure");
				LordLogInfo("buy ranch Item [%d] pay failure", itemId);
			}
		});
	}
	else 
	{
		getEntityPlayer()->subCurrency(totalPrice.value);
		SCRIPT_EVENT::PlayerBuyRanchItemResultEvent::invoke(getRakssid(), true, items);
	}
}

void ClientPeer::buyRanchBuildItem(i32 itemId, i32 addNum, i32 type, i32 currencyType)
{
	String url = getHttpUrl();

	IntProxy totalPrice;
	totalPrice.value = 0;

	IntProxy moneyType;
	moneyType.value = 3;

	BoolProxy isSuccess;
	isSuccess.value = false;

	SCRIPT_EVENT::GetRanchBuildItemCostEvent::invoke(getRakssid(), type, itemId, addNum, &isSuccess, &moneyType, &totalPrice);

	if (isSuccess.value == false)
	{
		return;
	}

	bool check = checkParam(url, moneyType.value, totalPrice.value);
	if (!check)
	{
		return;
	}

	if (moneyType.value < 3)
	{
		HttpRequest::pay(url, moneyType.value, 1031012, totalPrice.value, m_platformUserId, 
			[type, itemId, addNum](i64 platformUserId, String result, bool isSuccess)
		{
			auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
			if (!peer || !peer->getEntityPlayer())
			{
				return;
			}

			bool paySuccess = peer->payResult(platformUserId, result, isSuccess);
			SCRIPT_EVENT::PlayerBuyRanchBuildItemResultEvent::invoke(peer->getRakssid(), type, paySuccess, itemId, addNum);
			if (!paySuccess)
			{
				ServerNetwork::Instance()->getSender()->sendRequestResult(peer->getRakssid(), itemId, ResultCode::PAY_FAILURE, "pay failure");
				LordLogInfo("buy ranch Item [%d] pay failure", itemId);
			}
		});
	}
	else
	{
		getEntityPlayer()->subCurrency(totalPrice.value);
		SCRIPT_EVENT::PlayerBuyRanchBuildItemResultEvent::invoke(getRakssid(), type, true, itemId, addNum);
	}
	
}

void ClientPeer::ranchShortcut(i32 landCode , i32 currencyType, i32 totalPrice)
{
	String url = getHttpUrl();
	bool check = checkParam(url, currencyType, totalPrice);
	if (!check)
	{
		return;
	}
	if (currencyType < 3)
	{
		HttpRequest::pay(url, currencyType, 1031009, totalPrice, m_platformUserId, [landCode](i64 platformUserId, String result, bool isSuccess) {
			auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
			if (!peer || !peer->getEntityPlayer())
			{
				return;
			}

			bool paySuccess = peer->payResult(platformUserId, result, isSuccess);
			SCRIPT_EVENT::PlayerRanchShortcutEvent::invoke(peer->getRakssid(), paySuccess, landCode);
			if (!paySuccess)
			{
				ServerNetwork::Instance()->getSender()->sendRequestResult(peer->getRakssid(), landCode, ResultCode::PAY_FAILURE, "pay failure");
				LordLogInfo("ranch short cut landCode[%d] pay failure", landCode);
			}
		});
	}
	else
	{
		getEntityPlayer()->subCurrency(totalPrice);
		SCRIPT_EVENT::PlayerRanchShortcutEvent::invoke(getRakssid(), true, landCode);
	}
}

void ClientPeer::buyRanchItems(vector<RanchCommon>::type items)
{
	i32 currencyType = 1;
	
	String url = getHttpUrl();


	BoolProxy canSave;
	canSave.value = false;

	IntProxy totalPrice;
	totalPrice.value = 0;

	std::vector<RanchCommon> ranchCommonItems = std::vector<RanchCommon>();
	for (auto item : items)
	{
		ranchCommonItems.push_back(item);
	}

	SCRIPT_EVENT::CanRanchStorageSaveItemsEvent::invoke(getRakssid(), ranchCommonItems, currencyType, &totalPrice, &canSave);

	if (canSave.value == false)
	{
		ServerNetwork::Instance()->getSender()->sendRequestResult(getRakssid(), 1, ResultCode::RANCH_STORAGE_FULL, "gui_ranch_storage_full_operation_failure");
		return;
	}

	bool check = checkParam(url, currencyType, totalPrice.value);
	if (!check)
	{
		return;
	}

	if (currencyType < 3)
	{
		HttpRequest::pay(url, currencyType, 1031006, totalPrice.value, m_platformUserId, [ranchCommonItems](i64 platformUserId, String result, bool isSuccess) {
			auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
			if (!peer || !peer->getEntityPlayer())
			{
				return;
			}

			bool paySuccess = peer->payResult(platformUserId, result, isSuccess);
			SCRIPT_EVENT::PlayerBuyRanchItemResultEvent::invoke(peer->getRakssid(), paySuccess, ranchCommonItems);
			if (!paySuccess)
			{
				ServerNetwork::Instance()->getSender()->sendRequestResult(peer->getRakssid(), 1, ResultCode::PAY_FAILURE, "pay failure");
				LordLogInfo("buy ranch Items pay failure");
			}
		});
	}
	else
	{
		getEntityPlayer()->subCurrency(totalPrice.value);
		SCRIPT_EVENT::PlayerBuyRanchItemResultEvent::invoke(getRakssid(), true, ranchCommonItems);
	}

}

void ClientPeer::ranchBuildQueueOperation(i32 entityId, i32 type, i32 queueId, i32 productId)
{	
	String url = getHttpUrl();

	if (type == 1)
	{
		// production
		SCRIPT_EVENT::PlayerRanchBuildingQueueProductEvent::invoke(getRakssid(), entityId, queueId, productId);
		return;
	}

	if (type == 2)
	{
		//speed up
		IntProxy money;
		money.value = 0;

		BoolProxy isSuccess;
		isSuccess.value = false;

		SCRIPT_EVENT::GetRanchBuildingSpeedUpCostEvent::invoke(getRakssid(), entityId, queueId, productId, &isSuccess, &money);

		if (isSuccess.value == false)
		{
			return;
		}

		if (money.value == 0)
		{
			SCRIPT_EVENT::PlayerRanchBuildingQueueSpeedUpEvent::invoke(getRakssid(), entityId, queueId, productId, true);
			return;
		}

		bool check = checkParam(url, 1, money.value);
		if (!check)
		{
			return;
		}

		HttpRequest::pay(url, 1, 1031011, money.value, m_platformUserId, 
			[entityId, queueId, productId](i64 platformUserId, String result, bool isSuccess) 
		{
			auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
			if (!peer || !peer->getEntityPlayer())
			{
				return;
			}

			bool paySuccess = peer->payResult(platformUserId, result, isSuccess);
			SCRIPT_EVENT::PlayerRanchBuildingQueueSpeedUpEvent::invoke(peer->getRakssid(), entityId, queueId, productId, paySuccess);
			if (!paySuccess)
			{
				ServerNetwork::Instance()->getSender()->sendRequestResult(peer->getRakssid(), 1, ResultCode::PAY_FAILURE, "pay failure");
				LordLogInfo("ranch building speed up failure");
			}
		});
		return;
	}

	if (type == 3)
	{
		// unlockQueue

		IntProxy totalPrice;
		totalPrice.value = 0;

		IntProxy moneyType;
		moneyType.value = 3;

		BoolProxy isSuccess;
		isSuccess.value = false;

		SCRIPT_EVENT::GetRanchBuildQueueUnlockCostEvent::invoke(getRakssid(), entityId, queueId, &isSuccess, &moneyType, &totalPrice);

		if (isSuccess.value == false)
		{
			return;
		}

		bool check = checkParam(url, moneyType.value, totalPrice.value);
		if (!check)
		{
			return;
		}

		if (moneyType.value < 3)
		{
			HttpRequest::pay(url, moneyType.value, 1031008, totalPrice.value, m_platformUserId, 
				[entityId, queueId, productId](i64 platformUserId, String result, bool isSuccess)
			{
				auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
				if (!peer || !peer->getEntityPlayer())
				{
					return;
				}

				bool paySuccess = peer->payResult(platformUserId, result, isSuccess);
				SCRIPT_EVENT::PlayerRanchBuildingQueueUnlockEvent::invoke(peer->getRakssid(), entityId, queueId, productId, paySuccess);
				if (!paySuccess)
				{
					ServerNetwork::Instance()->getSender()->sendRequestResult(peer->getRakssid(), 1, ResultCode::PAY_FAILURE, "pay failure");
					LordLogInfo("ranch building queue unlock cost failure");
				}
			});
		}
		else
		{
			getEntityPlayer()->subCurrency(totalPrice.value);
			SCRIPT_EVENT::PlayerRanchBuildingQueueUnlockEvent::invoke(getRakssid(), entityId, queueId, productId, true);
		}
	}

	if (type == 4)
	{
		//building product speed up 
		SCRIPT_EVENT::PlayerRanchBuildingProductionSpeedUpEvent::invoke(getRakssid(), entityId, queueId, productId);
	}
}

void ClientPeer::speedUpCrop(Vector3i blockPos, i32 blockId, i32 totalPrice)
{
	String url = getHttpUrl();
	bool check = checkParam(url, 1, totalPrice);
	if (!check)
	{
		return;
	}

	HttpRequest::pay(url, 1, 1031005, totalPrice, m_platformUserId, 
		[blockId, blockPos](i64 platformUserId, String result, bool isSuccess) 
	{
		auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
		if (!peer || !peer->getEntityPlayer())
		{
			return;
		}

		bool paySuccess = peer->payResult(platformUserId, result, isSuccess);
		
		if (!paySuccess)
		{
			ServerNetwork::Instance()->getSender()->sendRequestResult(peer->getRakssid(), blockId, ResultCode::PAY_FAILURE, "pay failure");
			LordLogInfo("ranch speedUpCrop [%d] pay failure", blockId);
		}
		else 
		{
			BlockPos bos = BlockPos(blockPos.x, blockPos.y, blockPos.z);
			Server::Instance()->getBlockCropsManager()->speedUpCrop(platformUserId, bos);
		}
	});
}

void ClientPeer::useCubeFinshOrder(i32 orderId, i32 index)
{
	IntProxy money;
	money.value = 0;

	BoolProxy isSuccess;
	isSuccess.value = false;

	SCRIPT_EVENT::GetRanchTaskItemsCostEvent::invoke(getRakssid(), orderId, index, &isSuccess, &money);

	if (isSuccess.value == false)
	{
		return;
	}

	String url = getHttpUrl();
	bool check = checkParam(url, 1, money.value);
	if (!check)
	{
		return;
	}

	HttpRequest::pay(url, 1, 1031007, money.value, m_platformUserId, 
		[orderId, index](i64 platformUserId, String result, bool isSuccess) 
	{
		auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
		if (!peer || !peer->getEntityPlayer())
		{
			return;
		}

		OrderPayResult payResult = peer->payResultByConsume(platformUserId, isSuccess, false, result);

		if (payResult.code != 1)
		{
			ServerNetwork::Instance()->getSender()->sendRequestResult(peer->getRakssid(), orderId, ResultCode::PAY_FAILURE, "pay failure");
			LordLogInfo("useCubeFinshOrder [%d] pay failure", orderId);
		}
		else
		{
			SCRIPT_EVENT::PlayerBuyRanchTaskItemsResultEvent::invoke(peer->getRakssid(), orderId, index, payResult.orderId);
		}
	});
}

bool ClientPeer::payResult(i64 platformUserId, String result, bool isSuccess)
{
	OrderPayResult r = payResultByConsume(platformUserId, isSuccess, true, result);
	return r.code == 1;
}

ClientPeer::OrderPayResult ClientPeer::payResultByConsume(i64 platformUserId, bool isSuccess, bool isConsume, String result)
{
	OrderPayResult resultInfo = OrderPayResult();
	resultInfo.isConsume = isConsume;
	if (isSuccess)
	{
		Document* doc = new Document;
		doc->Parse(result.c_str());
		if (doc->HasParseError())
		{
			resultInfo.code = 0;
		}
		if (doc->HasMember("code"))
		{
			resultInfo.code = doc->FindMember("code")->value.GetInt();
			if (resultInfo.code == 1 && doc->HasMember("data"))
			{
				// called in aysnc callback, need to check peer and player
				auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
				auto data = doc->FindMember("data")->value.GetObject();
				if (peer && peer->getEntityPlayer())
				{
					auto player = this->getEntityPlayer();
					i64 diamonds = 0;
					i64 golds = 0;
					if (data.HasMember("diamonds"))
					{
						diamonds = data.FindMember("diamonds")->value.GetInt64();
						player->m_wallet->setDiamonds(diamonds);
					}
					if (data.HasMember("golds"))
					{
						golds = data.FindMember("golds")->value.GetInt64();
						player->m_wallet->setGolds(golds);
					}
					ServerNetwork::Instance()->getSender()->sendInitMoney(getRakssid(), diamonds, golds);
				}
				if (data.HasMember("orderId"))
				{
					resultInfo.orderId = data.FindMember("orderId")->value.GetString();
					String url = getHttpUrl();
					if (url.length() == 0)
					{
						LordLogError("resume order error: url is null");
					}

					if (isConsume)
					{
						HttpRequest::resumeOrder(url, resultInfo.orderId);
					}
				}
			}
		}
		delete doc;
		doc = nullptr;
	}
	return resultInfo;
}

String ClientPeer::getHttpUrl()
{
	if (m_platformUserId == 0) {
		LordLogError("client peer get http url : platformUserId = 0");
		return "";
	}
		
	if (!m_entityPlayer)
	{
		LordLogError("client peer get entity player is null");
		return "";
	}

	if (Server::Instance()->getEnableRoom())
	{
		UserAttrInfo attrInfo;
		bool hasAttr = Server::Instance()->getRoomManager()->getUserAttrInfo(m_platformUserId, attrInfo);
		if (!hasAttr)
		{
			LordLogError("client peer UserAttrInfo not exist");
			return "";
		}
			
		RoomGameConfig config = Server::Instance()->getConfig();
		return String(attrInfo.pioneer ? config.blockymodsUrl.c_str() : config.blockmanUrl.c_str());
	}
	return "";
}

bool ClientPeer::isPioneer()
{
	if (m_platformUserId == 0)
		return false;
	if (!m_entityPlayer)
		return false;

	if (Server::Instance()->getEnableRoom())
	{
		UserAttrInfo attrInfo;
		bool hasAttr = Server::Instance()->getRoomManager()->getUserAttrInfo(m_platformUserId, attrInfo);
		if (!hasAttr)
			return false;
		return attrInfo.pioneer;
	}
	return false;
}

void ClientPeer::stopLaserGun()
{
	Server::Instance()->getNetwork()->getSender()->broadCastStopLaserGun(m_entityPlayer->entityId);
}
