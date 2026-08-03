#include "C2SInitPacketHandles.h"
#include "Network/ClientPeer.h"
#include "Server.h"
#include "Util/LanguageKey.h"
#include "Network/ServerNetwork.h"
#include "Util/sha1.h"
#include "cpr/cpr.h"
#include "Object/Root.h"
#include "Network/HttpRequest.h"
#include "Inventory/CraftingManager.h"
#include "Script/GameServerEvents.h"
#include "Blockman/Entity/EntityTracker.h"
#include "GameVersionDefine.h"
#include "Network/protocol/S2CPackets.h"

#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
#define I64FMT "%I64d"
#define U64FMT "%I64u"
#else
#define I64FMT "%lld"
#define U64FMT "%llu"
#endif

void C2SInitPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketLogin>& packet)
{
	if (!clientPeer) {
		LordLogError("C2SPacketLogin clientPeer is nullptr, platform uid " U64FMT ", name %s",
			packet->m_platformUserId, packet->m_playerName.c_str());

		return;
	}

	// handle the situation that receive login packet twice
	// don't know why receive login packet twice yet
	if (clientPeer->getEntityPlayer() && clientPeer->hasLogon() && clientPeer->getPlatformUserId() == packet->m_platformUserId) {
		LordLogError("C2SPacketLogin , recv same login packet twice, session id " U64FMT ", platform uid " U64FMT ", name %s",
			 clientPeer->getRakssid(), packet->m_platformUserId, packet->m_playerName.c_str());

		return;
	}

	String rawString = HttpRequest::WEB_HTTP_SECRET + StringUtil::ToString((i64)packet->m_platformUserId) + StringUtil::ToString((i64)packet->m_gameTimestamp).c_str();
	SHA1 sha1;
	sha1.update(rawString.c_str());
	const std::string hash = sha1.final();
	clientPeer->setLanguage(packet->m_language);
	clientPeer->setShowName(packet->m_playerName);
	clientPeer->setPlatformUserId(packet->m_platformUserId);

	LordLogInfo("m_gameToken____[%s] ====str____[%s]", packet->m_gameToken.c_str(), hash.c_str());

	if (packet->m_gameToken != (String)hash.c_str())
	{
		ServerNetwork::Instance()->getSender()->sendLoginResult(clientPeer->getRakssid(), (int32_t)NETWORK_DEFINE::PacketLoginResult::emErrTokenWrong);
		LordLogError("C2SPacketLogin token wrong %s!=%s, session id " U64FMT ", platform uid " U64FMT ", name %s",
			packet->m_gameToken.c_str(), hash.c_str(), clientPeer->getRakssid(), packet->m_platformUserId, packet->m_playerName.c_str());

		return;
	}

	if (Server::Instance()->getEnableRoom())
	{
		bool isAttrExisted = Server::Instance()->getRoomManager()->UserAttrExisted(packet->m_platformUserId);
		if (!isAttrExisted)
		{
			if (ServerNetwork::Instance()->hasBeenDeferredLogin(packet->m_platformUserId)) {
				ServerNetwork::Instance()->getSender()->sendLoginResult(clientPeer->getRakssid(), (int32_t)NETWORK_DEFINE::PacketLoginResult::emErrWaitRoleAttrFail);
				LordLogError("C2SPacketLogin, no user attr from room, session id " U64FMT ", platform uid " U64FMT ", name %s",
					clientPeer->getRakssid(), packet->m_platformUserId, packet->m_playerName.c_str());

				return;
			} else {
				ServerNetwork::Instance()->deferLoginProcess(packet->m_platformUserId, packet);

				LordLogError("C2SPacketLogin, recv login packet but can not find user attr,deferLoginProcess, session id " U64FMT ", platform uid " U64FMT ", name %s",
					clientPeer->getRakssid(), packet->m_platformUserId, packet->m_playerName.c_str());

				return;
			}
		}
	}

	{
		LordLogInfo("C2SPacketLogin token correct, session id " U64FMT ", platform uid " U64FMT ", name %s",
			clientPeer->getRakssid(), packet->m_platformUserId, packet->m_playerName.c_str());
		auto pworld = Server::Instance()->getWorld();
		static size_t playerCount = 0;
		auto &spawnPos = Server::Instance()->getInitPos();

		EntityPlayerMP *pPlayer = LordNew EntityPlayerMP(pworld, packet->m_playerName, spawnPos);
		pPlayer->setRaknetID(clientPeer->getRakssid());
		UserAttrInfo attrInfo;
		attrInfo.sex = 1; // default, 1:boy, 2:girl
		attrInfo.defaultIdle = 0;

		bool attrExisted = false;
		if (Server::Instance()->getEnableRoom())
		{
			attrExisted = Server::Instance()->getRoomManager()->getUserAttrInfo(packet->m_platformUserId, attrInfo);
			if (attrExisted)
			{
				pPlayer->m_sex = attrInfo.sex;
				pPlayer->m_faceID = attrInfo.faceId;
				pPlayer->m_hairID = attrInfo.hairId;
				pPlayer->m_topsID = attrInfo.topsId;
				pPlayer->m_pantsID = attrInfo.pantsId;
				pPlayer->m_shoesID = attrInfo.shoesId;
				pPlayer->m_glassesId = attrInfo.glassesId;
				pPlayer->m_scarfId = attrInfo.scarfId;
				pPlayer->m_wingId = attrInfo.wingId;
				pPlayer->m_hatId = attrInfo.hatId;
				pPlayer->m_decorate_hatId = attrInfo.decoratehatId;
				pPlayer->m_armId = attrInfo.armId;
				pPlayer->m_tailId = attrInfo.tailId;
				pPlayer->m_extra_wingId = attrInfo.extrawingId;
				pPlayer->m_footHaloId = attrInfo.footHaloId;
				pPlayer->m_backEffectId = attrInfo.backEffectId;
				pPlayer->m_crownId = attrInfo.crownId;
				pPlayer->m_skinColor = attrInfo.skinColor;
				pPlayer->m_defaultIdle = attrInfo.defaultIdle;
				pPlayer->m_headEffectId = attrInfo.headEffectId;
				pPlayer->m_bagId = attrInfo.bagId;
			}
		}
		auto sender = Server::Instance()->getNetwork()->getSender();

		Vector3i adjustPos = Vector3i((int)pPlayer->position.x, (int)pPlayer->position.y, (int)pPlayer->position.z);
		sender->sendGameInfo(clientPeer->getRakssid(), pPlayer->entityId, adjustPos,
			CraftingManager::Instance()->getRecipeNameList(), pworld->getWorldInfo().isTimeStopped(), pPlayer->m_sex, Server::Instance()->getGameType(), pPlayer->m_defaultIdle);

		// Server-authoritative worldgen: send the client the world's
		// spawn point + world type + dimension BEFORE any chunk data.
		// The client uses this to set up its WorldClient spawn location
		// (overriding the local hardcoded seed) and to know whether to
		// expect chunk data packets from the server.
		{
			auto cfg = Server::Instance()->getConfig();
			auto s2cWorldInfo = LORD::make_shared<S2CPacketWorldInfo>();
			// Hash the seed — the raw seed never leaves the server.
			// (Simple FNV-1a; collision-resistant enough for this purpose.)
			uint64_t seedHash = 14695981039346656037ULL;
			uint64_t seedBits = (uint64_t)pworld->getSeed();
			for (int b = 0; b < 64; b += 8)
			{
				seedHash ^= (seedBits >> b) & 0xff;
				seedHash *= 1099511628211ULL;
			}
			s2cWorldInfo->m_worldSeedHash = (ui64)seedHash;
			s2cWorldInfo->m_spawnX = adjustPos.x;
			s2cWorldInfo->m_spawnY = adjustPos.y;
			s2cWorldInfo->m_spawnZ = adjustPos.z;
			s2cWorldInfo->m_gameType  = (ui8)Server::Instance()->getClientGameType();
			s2cWorldInfo->m_worldType = (ui8)(cfg.worldType & 0xff);
			s2cWorldInfo->m_dimension = (ui8)0;  // overworld
			s2cWorldInfo->encode();
			Server::Instance()->getNetwork()->sendPacket(s2cWorldInfo, clientPeer->getRakssid(), /*reliable=*/true);
		}

		if (attrExisted) {
			sender->sendUserAttr(clientPeer->getRakssid(), pPlayer->entityId, attrInfo);

			LordLogInfo("C2SPacketLogin, send user attr info, session id " U64FMT ", platform uid " U64FMT ", name %s,"
				"entityId[%d],faceId[%d],hairId[%d],topsId[%d],pantsId[%d],shoesId[%d],glassesId[%d],"
				"scarfId[%d], wingId[%d], hatId[%d], tailId[%d], decoratehatId[%d],armId[%d],extrawingId[%d],footHaloId[%d],skinColor r[%f] , skinColor g[%f] , skinColor b[%f] , skinColor a[%f]",
				clientPeer->getRakssid(), packet->m_platformUserId, packet->m_playerName.c_str(), pPlayer->entityId, attrInfo.faceId, attrInfo.hairId, attrInfo.topsId, attrInfo.pantsId, attrInfo.shoesId, attrInfo.glassesId, attrInfo.scarfId, attrInfo.wingId, attrInfo.hatId, attrInfo.tailId, attrInfo.decoratehatId, attrInfo.armId, attrInfo.extrawingId, attrInfo.footHaloId, attrInfo.skinColor.r, attrInfo.skinColor.g, attrInfo.skinColor.b, attrInfo.skinColor.a);
		}
		else {
			LordLogError("C2SPacketLogin, get user attr info fail and did not send user attr, session id " U64FMT ", platform uid " U64FMT ", name %s",
				clientPeer->getRakssid(), packet->m_platformUserId, packet->m_playerName.c_str());
		}

		sender->sendSyncWorldTime(clientPeer->getRakssid(), pworld->getWorldTime());
		if (pworld->getWorldInfo().isRaining())
		{
			sender->sendSyncWorldWeather(clientPeer->getRakssid(), "rain", pworld->getWorldInfo().getRainTime());
		}
		if (pworld->getWorldInfo().isThundering())
		{
			sender->sendSyncWorldWeather(clientPeer->getRakssid(), "thunder", pworld->getWorldInfo().getThunderTime());
		}
		//sender->sendUpdateBlocks(clientPeer->getRakssid());
		sender->sendAllSetSignTexts(clientPeer->getRakssid());
		sender->sendUpdateShop(clientPeer->getRakssid());

#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		// do not check platform id when debug
#else
		Server::Instance()->getNetwork()->kickOldLogin(packet->m_platformUserId, clientPeer->getRakssid());
#endif
		clientPeer->onSpawn(packet->m_playerName, pPlayer);
		clientPeer->setPlatformUserId(packet->m_platformUserId);
		LordLogInfo("C2SPacketLogin begin lua event, session id " U64FMT ", platform uid " U64FMT ", name %s, entityId %d",
			clientPeer->getRakssid(), packet->m_platformUserId, packet->m_playerName.c_str(), pPlayer->entityId);
		clientPeer->login();
		LordLogInfo("C2SPacketLogin end lua event, session id " U64FMT ", platform uid " U64FMT ", name %s, entityId %d",
			clientPeer->getRakssid(), packet->m_platformUserId, packet->m_playerName.c_str(), pPlayer->entityId);
		pworld->spawnEntityInWorld(pPlayer);
		SCRIPT_EVENT::PlayerFirstSpawnEvent::invoke(clientPeer.get());
	}
}

void C2SInitPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer,
	std::shared_ptr<C2SPacketRebirth>& packet)
{
	LordAssert(clientPeer != NULL);

	auto pworld = Server::Instance()->getWorld();

	Vector3i spawnPos = clientPeer->getRespawnPos();
	EntityPlayerMP *pPlayer = clientPeer->getEntityPlayer();
	if (!pPlayer) {
		LordLogError("recv rebirth before login ?, session id " U64FMT ", name %s, hasLogon:%s",
			clientPeer->getRakssid(), packet->m_playerName.c_str(), clientPeer->hasLogon() ? "true" : "false");
		
		return;
	}
	String oldShowName = pPlayer->getShowName();

	int oldSex = pPlayer->m_sex;

	// save old wallet data
	i64 diamonds = -1, golds = -1, currency = -1;
	Wallet* wallet = pPlayer->m_wallet;
	if (wallet)
	{
		diamonds = wallet->getDiamonds();
		golds = wallet->getGolds();
		currency = wallet->getCurrency();
	}

	Server::Instance()->getWorld()->removeEntityFromChunk(pPlayer);

	pPlayer->reconstruct(spawnPos);
	pPlayer->setShowName(oldShowName);
	pPlayer->m_sex = oldSex;
	pPlayer->setRaknetID(clientPeer->getRakssid());
	pPlayer->setPlatformUserId(clientPeer->getPlatformUserId());
	pPlayer->setTeamName(clientPeer->getTeamName());
	pPlayer->setTeamId(clientPeer->getTeamId());

	auto sender = Server::Instance()->getNetwork()->getSender();
	sender->sendRebirth(clientPeer->getRakssid(), pPlayer->entityId, spawnPos, oldSex, oldShowName);

	UserAttrInfo attrInfo;
	attrInfo.sex = 1; // default, 1:boy, 2:girl
	if (Server::Instance()->getEnableRoom())
	{
		bool attrExisted = Server::Instance()->getRoomManager()->getUserAttrInfo(clientPeer->getPlatformUserId(), attrInfo);
		if (attrExisted)
		{
			pPlayer->m_sex = attrInfo.sex;
			pPlayer->m_faceID = attrInfo.faceId;
			pPlayer->m_hairID = attrInfo.hairId;
			pPlayer->m_topsID = attrInfo.topsId;
			pPlayer->m_pantsID = attrInfo.pantsId;
			pPlayer->m_shoesID = attrInfo.shoesId;
			pPlayer->m_glassesId = attrInfo.glassesId;
			pPlayer->m_scarfId = attrInfo.scarfId;
			pPlayer->m_wingId = attrInfo.wingId;
			pPlayer->m_hatId = attrInfo.hatId;
			pPlayer->m_decorate_hatId = attrInfo.decoratehatId;
			pPlayer->m_armId = attrInfo.armId;
			pPlayer->m_tailId = attrInfo.tailId;
			pPlayer->m_extra_wingId = attrInfo.extrawingId;
			pPlayer->m_footHaloId= attrInfo.footHaloId;
			pPlayer->m_backEffectId = attrInfo.backEffectId;
			pPlayer->m_headEffectId = attrInfo.headEffectId;
			pPlayer->m_skinColor = attrInfo.skinColor;
			pPlayer->m_bagId = attrInfo.bagId;
		}
		if (attrExisted) {
			sender->sendUserAttr(clientPeer->getRakssid(), pPlayer->entityId, attrInfo);

			LordLogInfo("C2SPacketRebirth, send user attr info, session id " U64FMT ", platform uid " U64FMT ", name %s,"
				"entityId[%d],faceId[%d],hairId[%d],topsId[%d],pantsId[%d],shoesId[%d],glassesId[%d],"
				"scarfId[%d], wingId[%d], hatId[%d],tailId[%d], decoratehatId[%d], armId[%d],extrawingId[%d],footHaloId[%d],  skinColor r[%f] , skinColor g[%f] , skinColor b[%f] , skinColor a[%f]",
				clientPeer->getRakssid(), clientPeer->getPlatformUserId(), packet->m_playerName.c_str(),pPlayer->entityId, attrInfo.faceId, attrInfo.hairId, attrInfo.topsId,
				attrInfo.pantsId, attrInfo.shoesId, attrInfo.glassesId, attrInfo.scarfId, attrInfo.wingId, attrInfo.hatId, attrInfo.tailId, attrInfo.decoratehatId, attrInfo.armId,attrInfo.extrawingId,attrInfo.footHaloId,attrInfo.skinColor.r, attrInfo.skinColor.g, attrInfo.skinColor.b, attrInfo.skinColor.a);
		}
		else {
			LordLogError("C2SPacketRebirth, get user attr info fail and did not send user attr, session id " U64FMT ", platform uid " U64FMT ", name %s",
				clientPeer->getRakssid(), clientPeer->getPlatformUserId(), packet->m_playerName.c_str());
		}
	}

	LordLogInfo("handlePacket_C2SRebirth name[%s] entityID[%d] rankID[%lld]",
		packet->m_playerName.c_str(), pPlayer->entityId, clientPeer->getRakssid());

	clientPeer->onSpawn(packet->m_playerName, pPlayer);
	pworld->spawnEntityInWorld(pPlayer);
	SCRIPT_EVENT::PlayerRespawnEvent::invoke(clientPeer.get());
	// reset wallet data
	pPlayer->setCurrency(currency);
	pPlayer->m_wallet->setDiamonds(diamonds);
	pPlayer->m_wallet->setGolds(golds);
	sender->sendInitMoney(clientPeer->getRakssid(), diamonds, golds);
	EntityTracker::Instance()->forceTeleport(pPlayer->entityId);
	pPlayer->forceTeleportStart();
}

void C2SInitPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer,
	std::shared_ptr<C2SPacketCheckCSVersion>& packet)
{
	if (!clientPeer)
	{
		LordLogError("C2SPacketCheckCSVersion clientPeer is nullptr, return");
		return;
	}

	int curVersion = EngineVersionSetting::getEngineVersion();
	bool bIsSameVersion = packet->m_version == curVersion;
	auto resultPacket = LORD::make_shared<S2CPacketCheckCSVersionResult>();
	resultPacket->m_success = bIsSameVersion;
	resultPacket->m_serverVersion = curVersion;
	Server::Instance()->getNetwork()->sendPacket(resultPacket, clientPeer->getRakssid(), true);
}

void C2SInitPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer,
	std::shared_ptr<C2SPacketUpdateUserMoney>& packet)
{
	if (!clientPeer)
		return;
	clientPeer->loadUserMoney();
}
