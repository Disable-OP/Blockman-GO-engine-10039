#pragma once
#include "Core.h"
#include "Server.h"
#include "RoomManager.h"
#include "CommodityManager.h"
#include "Object/Root.h"
#include "Network/ServerNetwork.h"
#include "Network/ClientPeer.h"
#include "Network/ServerPacketSender.h"
#include "Network/RoomClient.h"
#include "Blockman/Entity/EntityPlayerMP.h"
#include "Blockman/World/BlockChangeRecorderServer.h"
#include "Chat/ChatComponent.h"
#include "Inventory/CoinManager.h"
#include "Setting/LogicSetting.h"
#include "DB/ServerDB.h"
#include "DB/ServerRedisDB.h"
#include "Setting/MultiLanTipSetting.h"
#include "Setting/GameRuleSetting.h"
#include "Network/ManorRequest.h"
#include "DB/MysqlHttpRequest.h"
#include "DB/RedisHttpRequest.h"
#include "rapidjson/document.h"
#include "Network/HttpRequest.h"
#include "World/BirdSimulator.h"
#include "Util/NewRandom.h"

#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
#define I64FMT "%I64d"
#define U64FMT "%I64u"
#else
#define I64FMT "%lld"
#define U64FMT "%llu"
#endif

using namespace LORD;
struct HostApi
{
	static void sendPlaySound(ui64 targetId/*ssid*/, uint soundType, bool bForce = false)
	{
		// win lose draw sound move to play on the PlayEndingAnimationEvent when bForce is false
		if (!bForce && (soundType == ST_GameCompleteWin
			|| soundType == ST_GameCompleteWin
			|| soundType == ST_GameCompleteWin))
		{
			return;
		}

		if (targetId == 0) {
			ServerNetwork::Instance()->getSender()->broadCastPlaySoundByType(soundType);
		}
		else {
			ServerNetwork::Instance()->getSender()->sendPlaySoundByType(targetId, soundType);
		}
	}

	static void broadCastMsg(ui64 targetId/*ssid*/, ui8 type, i32 time, const char *msg)
	{
		ChatComponentNotification::Type chatType = static_cast<ChatComponentNotification::Type>(type);
		auto notify = LORD::make_shared<ChatComponentNotification>(String(msg), time, chatType);

		if (targetId == 0)
		{
			ServerNetwork::Instance()->getSender()->sendNotification(notify);
		}
		else
		{
			ServerNetwork::Instance()->getSender()->sendNotification(targetId, notify);
		}
	}

	static void broadCastMsgByType(ui64 targetId, ui8 type, i32 time, int msgType, const char* args)
	{
		//ServerNetwork::Instance()->broadCastMsgByType(targetId, type, time, msgType, args);
		ServerNetwork::Instance()->broadCastGameTipByType(targetId, type, time, msgType, args);
	}

	static void sendSettlement(ui64 rakssId, String gameType, String result)
	{
		ServerNetwork::Instance()->getSender()->sendSettlement(rakssId, gameType, result);
	}

	static void sendGameover(ui64 rakssId, int msgType, int code)
	{
		ServerNetwork::Instance()->sendGameOver(rakssId, msgType, code);
	}

	static void resetPos(ui64 id, float x, float y, float z)
	{
		LordLogInfo("ScriptLog-------resetpos %llx, %f, %f, %f", id, x, y, z);
		auto clientPeer = ServerNetwork::Instance()->findPlayerByRakssid(id);
		if (clientPeer)
			clientPeer->resetPos(x, y, z);
	}

	static void resetPosWithYaw(ui64 id, float x, float y, float z, float yaw)
	{
		LordLogInfo("ScriptLog-------resetpos %llx, %f, %f, %f", id, x, y, z);
		auto clientPeer = ServerNetwork::Instance()->findPlayerByRakssid(id);
		if (clientPeer)
			clientPeer->resetPos(x, y, z, yaw);
	}

	static void log(const char *log, int level = 0)
	{
		if (level == 0)
			LOG(G3LOG_DEBUG) << log;
		if (level == 1)
			LOG(G3LOG_WARNING) << log;
		if (level == 2)
			LOG(G3LOG_ERROR) << log;
	}

	static void stopServer()
	{
		Server::Instance()->stop();
	}

	static void sendStartGame(int curPlayers, bool forceEnterPlayer = false)
	{
		LordLogInfo("sendStartGame %d", curPlayers);
		RoomGameConfig cfg = Server::Instance()->getConfig();
		if (Server::Instance()->getEnableRoom())
		{
			Server::Instance()->getRoomManager()->getRoomClient()
				->sendGameStatus(cfg.gameId, cfg.maxPlayers, curPlayers,
					forceEnterPlayer ? (int)RoomGameStatus::STARTED_CAN_ENTER : (int)RoomGameStatus::STARTED);
		}
	}

	static void sendResetGame(int curPlayers)
	{
		LordLogInfo("sendResetGame %d", curPlayers);
		if (Server::Instance()->getEnableRoom())
		{
			RoomGameConfig cfg = Server::Instance()->getConfig();
			Server::Instance()->getRoomManager()->getRoomClient()
				->sendGameStatus(cfg.gameId, cfg.maxPlayers, curPlayers, (int)RoomGameStatus::PREPARING);
		}
	}

	static String curTimeString()
	{
		LORD::ui32 timestamp = LORD::Root::Instance()->getCurrentTime();
		char ibuf[20];
		memset(ibuf, 0, 20);
		sprintf(ibuf, "%d", LORD::Root::Instance()->getCurrentTime());
		return ibuf;
	}

	static void resetMap()
	{
		BlockChangeRecorderServer::Instance()->reset();
	}

	// 1 start 2 resetaddCoinMapping
	static void sendGameStatus(ui64 rakssId, int status)
	{
		ServerNetwork::Instance()->getSender()->sendGameStatus(rakssId, status);
	}

	static void sendPlayerSettlement(ui64 rakssId, String result, bool isNextServer = true)
	{
		ServerNetwork::Instance()->getSender()->sendPlayerSettlement(rakssId, result, isNextServer);
	}

	static void sendGameSettlement(ui64 rakssId, String result, bool isNextServer = true)
	{
		ServerNetwork::Instance()->getSender()->sendGameSettlement(rakssId, result, isNextServer);
	}

	static void addCoinMapping(int coinId, int itemId)
	{
		CoinManager::Instance()->addCoinMapping(coinId, itemId);
	}

	static void broadCastPlayerLifeStatus(ui64 platformUserId, bool isLife)
	{
		ServerNetwork::Instance()->getSender()->broadCastPlayerLifeStatus(platformUserId, isLife);
	}

	static void addCommodity(int index, ui8 category, Commodity commodity)
	{
		CommodityManager::Instance()->addCommodity(index, CommodityCategory(category), commodity);
	}

	static void changeCommodityList(int entityId, ui64 rakssId, int index)
	{
		CommodityManager::Instance()->addMapping(entityId, rakssId, index);
	}

	static void sendBedDestroy(int teamId)
	{
		ServerNetwork::Instance()->getSender()->broadCastBedDestroy(teamId);
	}

	static void sendRespawnCountDown(ui64 rakssId, ui32 second)
	{
		ServerNetwork::Instance()->getSender()->sendRespawnCountdown(rakssId, second);
	}

	static void setServerGameType(String gameType)
	{
		Server::Instance()->setGameType(gameType);
	}

	static void sendPlayerTeamInfo(String result)
	{
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(result.c_str());
		if (doc->HasParseError())
		{
			LordLogError("The PlayerTeamInfo HasParseError.");
			LordDelete(doc);
			return;
		}
		rapidjson::Value arr = doc->GetArray();
		for (size_t i = 0; i < arr.Size(); ++i)
		{
			if (!arr[i].HasMember("teamId") || !arr[i].HasMember("userId"))
			{
				LordLogError("The PlayerTeamInfo content missed some field.");
				LordDelete(doc);
				return;
			}
			int teamId = arr[i].FindMember("teamId")->value.GetInt();
			int userId = arr[i].FindMember("userId")->value.GetInt();
			auto clientpeer = ServerNetwork::Instance()->findPlayerByPlatformUserId(teamId);
			if (clientpeer)
			{
				clientpeer->setTeamId(teamId);
				if (clientpeer->getEntityPlayer())
				{
					clientpeer->getEntityPlayer()->setTeamId(teamId);
				}
			}
		}
		LordDelete(doc);

		ServerNetwork::Instance()->getSender()->broadAllPlayerTeamInfo(result);
	}

	static void changePlayerTeam(ui64 rakssId, ui64 platformUserId, int teamId)
	{
		auto clientpeer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
		if (clientpeer)
		{
			clientpeer->setTeamId(teamId);
			if (clientpeer->getEntityPlayer())
			{
				clientpeer->getEntityPlayer()->setTeamId(teamId);
			}
		}
		if (rakssId == 0)
		{
			ServerNetwork::Instance()->getSender()->broadCastChangePlayerTeam(platformUserId, teamId);
		}
		else
		{
			ServerNetwork::Instance()->getSender()->sendChangePlayerTeam(rakssId, platformUserId, teamId);
		}
	}

	static void setMaxInventorySize(int size)
	{
		LogicSetting::Instance()->setMaxInventorySize(size);
	}

	static void setGunSetting(GunPluginSetting setting)
	{
		LogicSetting::Instance()->addGunPluginSetting(setting);
	}

	static void setPlayerKnockBackCoefficient(ui64 rakssId, float knockBackCoefficient)
	{
		auto  player = ServerNetwork::Instance()->findPlayerByRakssid(rakssId);
		if (player && player->getEntityPlayer())
		{
			player->getEntityPlayer()->SetKnockBackCoefficient(knockBackCoefficient);
		}
	}

	static void setBulletClipSetting(int bulletId, int maxStack)
	{
		LogicSetting::Instance()->addBulletClipPluginSetting(bulletId, maxStack);
	}

	static void startDBService()
	{
		Server::Instance()->initDB("");
	}

	static void startDBServiceByGameType(std::string gameType)
	{
		Server::Instance()->initDB(gameType);
	}

	static void setDBData(ui64 userId, int subKey, std::string data, bool immediate)
	{
		MysqlHttpRequest::Instance()->setData(userId, subKey, data, immediate);
	}

	static void getDBData(ui64 userId, int subKey)
	{
		MysqlHttpRequest::Instance()->getData("", userId, subKey);
	}

	static void getDBDataByGameType(std::string gameType, ui64 userId, int subKey)
	{
		MysqlHttpRequest::Instance()->getData(gameType, userId, subKey);
	}

	static void sendSelectRoleData(ui64 rakssId, String result)
	{
		ServerNetwork::Instance()->getSender()->sendSelectRoleData(rakssId, result);
	}

	static void setSneakShowName(bool isShowName)
	{
		LogicSetting::Instance()->setSneakShowName(isShowName);
	}

	static void setBreakBlockSoon(bool soon)
	{
		LogicSetting::Instance()->setBreakBlockSoon(soon);
	}

	static void setCanDamageItem(bool canDamageItem)
	{
		LogicSetting::Instance()->setCanDamageItem(canDamageItem);
	}

	static void showBuyRespawn(ui64 rakssId, int times)
	{
		Server::Instance()->getWorld()->showBuyRespawnToPlayer(rakssId, times);
	}

	static void setBlockAttr(int blockId, float hardness)
	{
		LogicSetting::Instance()->addBlockDynamicAttr(blockId, hardness);
	}

	static void setToolAttr(int itemId, float distance, float efficiency)
	{
		LogicSetting::Instance()->addToolItemDynamicAttr(itemId, distance, efficiency);
	}

	static void showGoNpcMerchant(ui64 rakssId, int entityId, float x, float y, float z, float yaw)
	{
		ServerNetwork::Instance()->getSender()->sendShowGoNpcMerchant(rakssId, entityId, x, y, z, yaw);
	}

	static void ZScore(std::string setName, std::string key)
	{
		//ServerRedisDB::Instance()->ZScore(setName, key);
		RedisHttpRequest::Instance()->ZScore(setName, key);
	}

	static void ZIncrBy(std::string setName, std::string key, int value)
	{
		//ServerRedisDB::Instance()->ZIncrBy(setName, key, value);
		RedisHttpRequest::Instance()->ZIncrBy(setName, key, value);
	}

	static void ZRange(std::string setName, int start, int end)
	{
		RedisHttpRequest::Instance()->ZRange(setName, start, end);
		//ServerRedisDB::Instance()->ZRange(setName, start, end);
	}

	static void ZExpireat(std::string setName, std::string timeStamp)
	{
		RedisHttpRequest::Instance()->ZExpireat(setName, timeStamp);
		//ServerRedisDB::Instance()->ZExpireat(setName, timeStamp);
	}

	static void ZRemove(std::string setName, std::string key)
	{
		RedisHttpRequest::Instance()->ZRem(setName, key);
		//ServerRedisDB::Instance()->ZRem(setName, key);
	}

	static void startRedisDBService()
	{
		Server::Instance()->startRedis();
	}

	static void setRedisDBHost(int host)
	{
		RedisHttpRequest::Instance()->setRedisDBHost(host);
	}

	static void sendRankData(ui64 rakssId, int entityId, String result)
	{
		ServerNetwork::Instance()->getSender()->sendRankData(rakssId, entityId, result);
	}

	static void syncSetBlock()
	{
		ServerNetwork::Instance()->getSender()->broadCastSyncBlock();
	}

	static void setAttackCoefficientX(float x)
	{
		LogicSetting::Instance()->setAttackCoefficientX(x);
	}

	static void sendChangeAircraftUI(ui64 targetId, bool isShow)
	{
		if (targetId != 0)
		{
			ServerNetwork::Instance()->getSender()->sendChangeAircraftUI(targetId, isShow);
		}
		else
		{
			ServerNetwork::Instance()->getSender()->broadCastChangeAircraftUI(isShow);
		}
	}

	static void setFallingSpeed(float horizontal, float vertical)
	{
		LogicSetting::Instance()->setParachuteDownSpeed(vertical);
		LogicSetting::Instance()->setParachuteMoveSpeed(horizontal);
	}

	static void setMedichineHealAmount(int id, float amount)
	{
		ItemMedichine* medichine = dynamic_cast<ItemMedichine*>(Item::getItemById(id));
		if (medichine == NULL)
			return;

		medichine->setHealAmount(amount);
	}

	static void sendMemberLeftAndKill(ui64 targetId, int left, int kill)
	{
		ServerNetwork::Instance()->getSender()->sendMemberLeftAndKill(targetId, left, kill);
	}

	static void changeSkillType(ui64 rakssId, int type, float duraction, float coldDownTime)
	{
		ServerNetwork::Instance()->getSender()->sendSkillType(rakssId, type, duraction, coldDownTime);
	}

	static void sendShowHideAndSeekBtnStatus(ui64 rakssId, bool isActorBtnStatus, bool isCameraBtnStatus, bool isOtherBtnStatus)
	{
		ServerNetwork::Instance()->getSender()->sendShowHideAndSeekBtnStatus(rakssId,isActorBtnStatus, isCameraBtnStatus, isOtherBtnStatus);
	}

	static void syncGameTimeShowUi(ui64 targetId, bool isShow, int time)
	{
		if (targetId != 0)
		{
			ServerNetwork::Instance()->getSender()->sendSyncGameTimeShowUi(targetId, isShow, time);
		}
		else
		{
			ServerNetwork::Instance()->getSender()->broadCastGameTimeShowUi(isShow, time);
		}
	}

	static void createArrowTipMark(ui64 rakssId, Vector3 sourcePostion, Vector3 targetPostion, int  destoryTime, float distances)
	{
		ServerNetwork::Instance()->getSender()->createArrowTipMark(rakssId, sourcePostion, targetPostion, destoryTime, distances);
	}

	static void setDBLogData(ui64 userId, std::string GameType, std::string dataAction, std::string data, bool immediate)
	{
		//ServerDB::Instance()->setLogData(userId, GameType, dataAction, data, immediate);
		MysqlHttpRequest::Instance()->setLogData(userId, GameType, dataAction, data, immediate);
	}

	static void setDisableSelectEntity(bool isDisable)
	{
		LogicSetting::Instance()->setDisableSelectEntity(isDisable);
	}
	
	static void sendPickUpItemOrder(ui64 rakssId, i32 itemEntityId, i32 itemId, i32 pickUpPrice, i32 moneyType)
	{
		ServerNetwork::Instance()->getSender()->sendPickUpItemOrder(rakssId, itemEntityId, itemId, pickUpPrice, moneyType);
	}

	static void sendRefreshActor(int entityId)
	{
		ServerNetwork::Instance()->getSender()->sendDeathUnequipArmor(entityId);
	}

	static void changePlayerPerspece(ui64 rakssId, int view)
	{
		ServerNetwork::Instance()->getSender()->sendChangeEntityPerspece(rakssId, view);
	}

	static void setNeedFoodStats(bool isNeedFoodStats)
	{
		LogicSetting::Instance()->setNeedFoodStats(isNeedFoodStats);
	}

	static void updateManorInfo(ui64 rakssId, ManorInfo info, ManorInfo nextInfo, std::vector<ManorHouse> houses, std::vector<ManorFurniture> furnitures)
	{
		auto player = ServerNetwork::Instance()->findPlayerByRakssid(rakssId)->getEntityPlayer();
		if (player)
		{
			player->m_manor->setInfo(&info);
			player->m_manor->setNextInfo(&nextInfo);
			for (auto house : houses)
			{
				player->m_manor->addHouse(&house);
			}

			for (auto furniture : furnitures)
			{
				player->m_manor->addFurniture(&furniture);
			}
			
			Server::Instance()->getNetwork()->getSender()->sendUpdateManor(player);
		}
	}

	static void showPlayerOperation(ui64 rakssId, ui64 targetUserId)
	{
		Server::Instance()->getNetwork()->getSender()->sendShowPlayerOperation(rakssId, targetUserId);
	}
	
	static void updateRealTimeRankInfo(ui64 rakssId, String result)
	{
		ServerNetwork::Instance()->getSender()->sendUpdateRealTimeRankInfo(rakssId, result);
	}

	static void updateMonsterInfo(int currCheckPointNum, int surplusMonsterNum, int surplusCheckPointNum)
	{
		ServerNetwork::Instance()->getSender()->broadCastGameMonsterInfo(currCheckPointNum, surplusMonsterNum, surplusCheckPointNum);
	}

	static void updateBossBloodStrip(int currBloodStrip, int maxBloodStrip)
	{
		ServerNetwork::Instance()->getSender()->broadCastBossBloodStrip(currBloodStrip, maxBloodStrip);
	}

	static void setGunFireIsNeedBullet(bool gunIsNeedBullet)
	{
		LogicSetting::Instance()->setGunIsNeedBulletStatus(gunIsNeedBullet);
	}

	static void addMonsterSetting(MonsterSetting setting)
	{
		LogicSetting::Instance()->addMonsterSetting(setting);
	}

	static void addSkillEffect(SkillEffect effect)
	{
		LogicSetting::Instance()->addSkillEffect(effect);
	}

	static void showUpgradeResourceUI(ui64 rakssId, int costItemId, int costItemNum, int resourceId, int msgType, const char* args)
	{
		auto clientPeer = ServerNetwork::Instance()->findPlayerByRakssid(rakssId);
		std::string tipStr = "";
		if (clientPeer)
		{
			String lanType = clientPeer->getLanguage();
			tipStr = MultiLanTipSetting::getMessage(lanType, msgType, args);
		}
		ServerNetwork::Instance()->getSender()->showUpgradeResourceUI(rakssId, costItemId, costItemNum, resourceId, String(tipStr.c_str()));
	}

	static void sendCommonTip(ui64 rakssId, const char* msg)
	{
		ServerNetwork::Instance()->getSender()->sendRequestResult(rakssId, 0, ResultCode::COMMON, msg);
	}

	static void sendManorOperationTip(ui64 rakssId, const char* msg)
	{
		ServerNetwork::Instance()->getSender()->sendRequestResult(rakssId, 0, ResultCode::MANOR_BAN_OPERATION, msg);
	}

	static void setManorBtnVisible(ui64 rakssId, bool visible)
	{
		ServerNetwork::Instance()->getSender()->sendManorBtnVisible(rakssId, visible);
	}

	static void loadHouses(ui64 userId, const char* manorId)
	{
		ManorRequest::loadHouses(userId, manorId);
	}

	static void loadFurnitures(ui64 userId , const char* manorId)
	{
		ManorRequest::loadFurnitures(userId, manorId);
	}

	static void loadPloughInfo(ui64 userId, const char* manorId)
	{
		ManorRequest::loadPloughInfo(userId, manorId);
	}

	static void loadManorInfo(ui64 userId, bool isMaster)
	{
		ManorRequest::loadManorInfo(userId, isMaster);
	}

	static void loadManorConfig()
	{
		ManorRequest::loadManorConfig();
	}

	static void loadManorInventory(ui64 userId, const char* manorId)
	{
		ManorRequest::loadManorInventory(userId, manorId);
	}

	static void saveManorInventory(const char* manorId , const char* inventoryInfo)
	{
		ManorRequest::saveManorInventory(manorId, inventoryInfo);
	}

	static void loadGameMoney(ui64 platformUserId)
	{
		ManorRequest::loadGameMoney(platformUserId);
	}

	static void operationFurniture(ui64 userId, String manorId, i32 furnitureId, Vector3 pos, float yaw, i32 state)
	{
		ManorRequest::operationFurniture(userId, manorId, furnitureId, pos, yaw, state);
	}

	static void buildHouse(ui64 userId, String manorId, i32 houseId)
	{
		ManorRequest::buildHouse(userId, manorId, houseId);
	}

	static void loadManorBlock(ui64 userId, const char* manorId)
	{
		ManorRequest::loadManorBlock(userId, manorId);
	}

	static void saveManorBlock(const char* manorId, const char* blockInfo)
	{
		ManorRequest::saveManorBlock(manorId, blockInfo);
	}

	static void loadManorMessage(ui64 userId, i32 page)
	{
		ManorRequest::loadManorMessage(userId, page);
	}
	
	static void ploughCrop(const char* manorId, i32 row, i32 column,  String blockId, i32 fruitNum, i32 curState, i32 pluckingState, ui64 curStateTime, ui64 lastUpdateTime)
	{
		ManorRequest::ploughCrop(manorId, row, column, blockId, fruitNum, curState, pluckingState, curStateTime, lastUpdateTime);
	}

	static void ploughPlucking(const char* manorId, ui64 userId, i32 row, i32 colum, i32 num, i32 productId, ui64 pluckingTime)
	{
		ManorRequest::ploughPlucking(manorId, userId, row, colum, num, productId, pluckingTime);
	}

	static void updateBasementLife(float curLife, float maxLife)
	{
		ServerNetwork::Instance()->getSender()->broadCastBasementLife(curLife, maxLife);
	}

	static void sendCustomTip(ui64 rakssId, const char* messageLang, const char* extra)
	{
		ServerNetwork::Instance()->getSender()->sendCustomTipMsg(rakssId, messageLang, extra);
	}

	static void addSkillItem(SkillItem skillItem)
	{
		LogicSetting::Instance()->addSkillItem(skillItem);
	}
	
	static void loadManorCharmRank(i32 size)
	{
		ManorRequest::loadManorCharmRank(size);
	}

	static void loadManorPotentialRank(i32 size)
	{
		ManorRequest::loadManorPotentialRank(size);
	}

	static void loadMyManorRank(ui64 userId)
	{
		ManorRequest::loadMyManorRank(userId);
	}

	static void manorTrade(ui64 platformUserId, i32 price)
	{
		ManorRequest::trade(platformUserId, price);
	}

	static void notifyGetItem(ui64 rakssId, int itemId, int meta, int num)
	{
		ServerNetwork::Instance()->getSender()->notifyGetItem(rakssId, itemId, meta, num);
	}

	static void showBuildWarBlockBtn(ui64 userId, bool isNeedOpen)
	{
		ServerNetwork::Instance()->getSender()->sendBuildWarBlockBtn(userId, isNeedOpen);
	}

	static void showBuildWarPlayerNum(ui8 cur_num, ui8 max_num)
	{
		ServerNetwork::Instance()->getSender()->sendbroadcastBuildWarPlayNum(cur_num, max_num);
	}

	static void showBuildWarLeftTime(ui64 userId, bool show, ui16 left_time, ui32 theme)
	{
		ServerNetwork::Instance()->getSender()->sendbroadcastBuildWarLeftTime(userId, show, left_time, theme);
	}

	static void shopOpenShopByEntityId(ui64 userId, int entity_id)
	{
		ServerNetwork::Instance()->getSender()->sendOpenShopByEntityId(userId, entity_id);
	}

	static void showBuildGrade(ui64 userId, bool openOrClose, bool show, ui32 msg, ui32 grade_msg, ui8 left_time)
	{
		ServerNetwork::Instance()->getSender()->sendShowBuildGrade(userId, openOrClose, show, msg, grade_msg, left_time);
	}

	static void showBuildGuessResult(ui64 userId, bool right, int rank)
	{
		ServerNetwork::Instance()->getSender()->sendShowBuildGuessResult(userId, right, rank);
	}

	static void showBuildGuessUi(ui64 userId, int guess_room_id, String info)
	{
		ServerNetwork::Instance()->getSender()->showBuildGuessUi(userId, guess_room_id, info);
	}

	static void sendGameSettlementExtra(ui64 userId, bool guess_right, String guess_name, int guess_reward)
	{
		ServerNetwork::Instance()->getSender()->sendGameSettlementExtra(userId, guess_right, guess_name, guess_reward);
	}

	static void closeBGM(ui64 rakssId)
	{
		ServerNetwork::Instance()->getSender()->sendCloseBGM(rakssId);
	}
	
	static void updateTeamResources(String data)
	{
		ServerNetwork::Instance()->getSender()->sendTeamResourcesUpdate(data);
	}

	static void setHideClouds(bool isHide)
	{
		LogicSetting::Instance()->setHideClouds(isHide);
	}

	static void showConsumeCoinTip(ui64 rakssId, String message, int coinId, int price, String extra)
	{
		ServerNetwork::Instance()->getSender()->sendConsumeCoinTip(rakssId, message, coinId, price, extra);
	}

	static void updateBuildProgress(String name, int curProgress, int maxProgress)
	{
		ServerNetwork::Instance()->getSender()->broadCastBuildProgress(name, curProgress, maxProgress);
	}

	static void setFoodHeal(bool isFoodHeal)
	{
		LogicSetting::Instance()->setFoodHeal(isFoodHeal);
	}

	static void setEntityItemLife(int life)
	{
		LogicSetting::Instance()->setEntityItemLife(life);
	}

	static void setCanCloseChest(bool canCloseChest)
	{
		LogicSetting::Instance()->setCanCloseChest(canCloseChest);
	}

	static void setShowGunEffectWithSingle(bool showGunEffectWithSingle)
	{
		LogicSetting::Instance()->setShowGunEffectWithSingleStatus(showGunEffectWithSingle);
	}

	static void sendEnterOtherGame(ui64 rakssId, String gameType, ui64 targetId, String mapId = "")
	{
		ServerNetwork::Instance()->getSender()->sendEnterOtherGame(rakssId, gameType, targetId, mapId);
	}

	static void sendKillMsg(ui64 rakssId, String data)
	{
		ServerNetwork::Instance()->getSender()->sendKillMsg(rakssId, data);
	}

	static void sendShowPersonalShop(ui64 rakssId)
	{
		ServerNetwork::Instance()->getSender()->sendShowPersonalShop(rakssId);
	}

	static void setAllowHeadshot(bool allowHeadshot)
	{
		LogicSetting::Instance()->setAllowHeadshotStatus(allowHeadshot);
	}

	static void sendCallOnManorResetClient(ui64 rakssId, ui64 targetId)
	{
		ServerNetwork::Instance()->getSender()->sendCallOnManorResetClient(rakssId, targetId);
	}

	static void sendRanchGain(ui64 rakssId, std::vector<RanchCommon> items)
	{
		if (items.size() == 0)
		{
			return;
		}
		vector<RanchCommon>::type newItems = vector<RanchCommon>::type();
		for (auto item : items)
		{
			newItems.push_back(item);
		}
		ServerNetwork::Instance()->getSender()->sendRanchGain(rakssId, newItems);
	}

	static void showRanchExTip(ui16 type, ui16 num)
	{
		ServerNetwork::Instance()->getSender()->sendbroadcastRanchExTip(type, num);
	}

	// type[1:yellow 2:green 3:red]; direction[1:Left 2:Right]
	static void changeCarDirection(int type, int direction)
	{
		if (type < 0 || type > 3)
			return;
		ServerNetwork::Instance()->getSender()->broadCastCarDirection(type, direction);
	}

	// type[1:yellow 2:green 3:red]; progress[0-100]
	static void changeCarProgress(int type, float progress)
	{
		if (type < 0 || type > 3)
			return;
		if (progress < 0 || progress > 100)
			return;
		ServerNetwork::Instance()->getSender()->broadCastCarProgress(type, progress);
	}

	static void showRanchExTask(bool show, String task)
	{
		ServerNetwork::Instance()->getSender()->sendbroadcastRanchExTask(show, task);
	}

	static void showRanchExCurrentItemInfo(ui64 rakssId, bool show, String itemInfo, int type, int num)
	{
		ServerNetwork::Instance()->getSender()->sendShowRanchExCurrentItemInfo(rakssId, show, itemInfo, type, num);
	}

	static void showRanchExItem(ui64 rakssId, bool show, String item)
	{
		ServerNetwork::Instance()->getSender()->sendShowRanchExItem(rakssId, show, item);
	}

	static void sendRanchOrderHelpResult(ui64 rakssId, const char* msg)
	{
		ServerNetwork::Instance()->getSender()->sendRequestResult(rakssId, 0, ResultCode::RANCH_ORDER_HELP_RESULT, msg);
	}

	static void sendGotoOtherGame(ui64 rakssId, ui64 targetUserId, String gameType, String mapId = "")
	{
		ServerNetwork::Instance()->getSender()->sendGotoOtherGame(rakssId, targetUserId, gameType, mapId);
	}

	static void sendRanchUnlockItem(ui64 rakssId, std::vector<i32> items)
	{
		if (items.empty())
		{
			return;
		}
		vector<i32>::type newItems = vector<i32>::type();
		for (auto item : items)
		{
			newItems.push_back(item);
		}

		ServerNetwork::Instance()->getSender()->sendRanchUnlockItem(rakssId, newItems);
	}

	static void sendMailReceiveResult(ui64 rakssId, const char* msg)
	{
		ServerNetwork::Instance()->getSender()->sendRequestResult(rakssId, 0, ResultCode::MAIL_RECEIVE_RESULT, msg);
	}

	static void sendGiveGiftResult(ui64 rakssId, const char* msg)
	{
		ServerNetwork::Instance()->getSender()->sendRequestResult(rakssId, 0, ResultCode::GIVE_GIFT, msg);
	}

	static void sendOpenChest(bool show, int entityId, Vector3i pos)
	{
		ServerNetwork::Instance()->getSender()->sendOpenChest(show, entityId, pos);
	}

	static void sendReleaseManor(ui64 userId)
	{
		Server::Instance()->getRoomManager()->getRoomClient()->sendUserSellManor(userId);
	}

	static void platformOrderDispose(ui64 userId, String url, String orderId, bool isConsume)
	{
		if (isConsume)
		{
			HttpRequest::resumeOrder(url, orderId);
		}
		else
		{
			HttpRequest::cancelOrder(url, orderId, userId);
		}

		LordLogInfo("HostApi::platformOrderDispose orderId [%s]", orderId.c_str());
	}

	static void sendRanchWarehouseResult(ui64 rakssId, const char* msg)
	{
		ServerNetwork::Instance()->getSender()->sendRequestResult(rakssId, 0, ResultCode::RANCH_STORAGE_FULL, msg);
	}

	static void sendAppExpResult(i64 userId, int curLv, int toLv, int addExp, int curExp, int toExp, int upExp, int status)
	{
		ServerNetwork::Instance()->getSender()->sendAppExpResult(userId, curLv, toLv, addExp, curExp, toExp, upExp, status);
	}

	static void setBlockmanEmptyHitTimes(int times)
	{
		GameRuleSetting::getGameRule()->blockmanLongHitTimes = times;
	}

	static void sendOccupationUnlock(ui64 rakssId, String msg)
	{
		ServerNetwork::Instance()->getSender()->sendOccupationUnlock(rakssId, msg);
	}

	static void sendBuyFlying(ui64 rakssId)
	{
		ServerNetwork::Instance()->getSender()->sendBuyFlying(rakssId);
	}

	static void sendKeepItemTip(ui64 rakssId, int coinType, int coin, int time)
	{
		ServerNetwork::Instance()->getSender()->sendKeepItemTip(rakssId, coinType, coin, time);
	}

	static void sendOpenChantment(ui64 rakssId, String data)
	{
		ServerNetwork::Instance()->getSender()->sendOpenEnchantment(rakssId, data);
	}

	static void setLotteryData(ui64 rakssId, String data)
	{
		ServerNetwork::Instance()->getSender()->sendLotteryData(rakssId, data);
	}

	static void sendLotteryResult(ui64 rakssId, String firstId, String secondId, String thirdId)
	{
		ServerNetwork::Instance()->getSender()->sendLotteryResult(rakssId, firstId, secondId, thirdId);
	}

	static void sendHideAndSeekHallResult(ui64 rakssId, String itemInfo)
	{
		ServerNetwork::Instance()->getSender()->sendHideAndSeekHallResult(rakssId, itemInfo);
	}

	static void sendOpenPixelGunHallModeSelect(ui64 rakssId, bool open, String data)
	{
		ServerNetwork::Instance()->getSender()->sendOpenPixelGunHallModeSelect(rakssId, open, data);
	}	
	
	static void sendOpenArmorUpgrade(ui64 rakssId, bool open, String data)
	{
		ServerNetwork::Instance()->getSender()->sendOpenArmorUpgrade(rakssId, open, data);
	}

	static void sendUpdateStoreGunData(ui64 rakssId, String data) 
	{
		ServerNetwork::Instance()->getSender()->sendUpdateStoreGunData(rakssId, data);
	}

	static void sendUpdateStorePropData(ui64 rakssId, String data)
	{
		ServerNetwork::Instance()->getSender()->sendUpdateStorePropData(rakssId, data);
	}

	static void sendShowGunStore(ui64 rakssId)
	{
		ServerNetwork::Instance()->getSender()->sendShowGunStore(rakssId);
	}

	static void sendShowPixelGunHallInfo(ui64 rakssId, int lv, int cur_exp, int max_exp, int yaoshi, bool is_max)
	{
		ServerNetwork::Instance()->getSender()->sendShowPixelGunHallInfo(rakssId, lv, cur_exp, max_exp, yaoshi, is_max);
	}

	static void sendOpenPixel1v1(ui64 rakssId, bool open, String data)
	{
		ServerNetwork::Instance()->getSender()->sendOpenPixel1v1(rakssId, open, data);
	}

	static void sendShowChestLottery(ui64 rakssId, bool open, int curIntegral, int needIntegral)
	{
		ServerNetwork::Instance()->getSender()->sendShowChestLottery(rakssId, open, curIntegral, needIntegral);
	}

	static void sendChestLotteryResult(ui64 rakssId, int rewardId, bool hasGet)
	{
		ServerNetwork::Instance()->getSender()->sendChestLotteryResult(rakssId, rewardId, hasGet);
	}

	static void sendOpenPixelRevive(ui64 rakssId, bool open, String data)
	{
		ServerNetwork::Instance()->getSender()->sendOpenPixelRevive(rakssId, open, data);
	}

	static void sendUpdateLotteryChestData(ui64 rakssId, String data)
	{
		ServerNetwork::Instance()->getSender()->sendUpdateLotteryChestData(rakssId, data);
	}

	static void sendShowPixelGunGamePerson(ui64 rakssId, int time, int rank, int kill_num)
	{
		ServerNetwork::Instance()->getSender()->sendShowPixelGunGamePerson(rakssId, time, rank, kill_num);
	}

	static void sendShowPixelGunGame1v1(ui64 rakssId, int time, int red_kill_num, int blue_kill_num, int self_team)
	{
		ServerNetwork::Instance()->getSender()->sendShowPixelGunGame1v1(rakssId, time, red_kill_num, blue_kill_num, self_team);
	}

	static void sendShowPixelGunGameTeam(ui64 rakssId, int time, int red_kill_num, int red_cur_player_num, int red_max_player_num, int blue_kill_num, int blue_cur_player_num, int blue_max_player_num, int self_team)
	{
		ServerNetwork::Instance()->getSender()->sendShowPixelGunGameTeam(rakssId, time, red_kill_num, red_cur_player_num, red_max_player_num, blue_kill_num, blue_cur_player_num, blue_max_player_num, self_team);
	}

	static void sendOpenPixelResult(ui64 rakssId, bool open, String data, int result_type)
	{
		ServerNetwork::Instance()->getSender()->sendOpenPixelResult(rakssId, open, data, result_type);
	}

	static void sendPlayerCurrentSeasonInfo(ui64 rakssId, int honorId, int rank, int honor, int endTime)
	{
		ServerNetwork::Instance()->getSender()->sendPlayerCurrentSeasonInfo(rakssId, honorId, rank, honor, endTime);
	}

	static void sendPlayerLastSeasonInfo(ui64 rakssId, int honorId, int rank, int honor)
	{
		ServerNetwork::Instance()->getSender()->sendPlayerLastSeasonInfo(rakssId, honorId, rank, honor);
	}

	static void setAutoShootEnable(ui64 rakssId, bool enableAutoShoot)
	{
		if (rakssId == 0)
		{
			LogicSetting::Instance()->setAutoShootEnable(enableAutoShoot);
		}
		ServerNetwork::Instance()->getSender()->sendEnableAutoShoot(rakssId, enableAutoShoot);
	}

	static void setDisarmament(ui64 rakssId, bool isDisarmament)
	{
		ServerNetwork::Instance()->getSender()->sendDisarmament(rakssId, isDisarmament);
	}

	static void setPlayerOpacity(int entityId, float opacityValue)
	{
		ServerNetwork::Instance()->getSender()->broadCastPlayerOpacity(entityId, opacityValue);
	}

	static void sendOpenPixelLvUp(ui64 rakssId, bool open, String data)
	{
		ServerNetwork::Instance()->getSender()->sendOpenPixelLvUp(rakssId, open, data);
	}

	static void sendOpenPixelResultSpecialInfo(ui64 rakssId, bool btnRevengeEnable)
	{
		ServerNetwork::Instance()->getSender()->sendOpenPixelResultSpecialInfo(rakssId, btnRevengeEnable);
	}

	static void notifyGetGoods(ui64 rakssId, String icon, int num)
	{
		ServerNetwork::Instance()->getSender()->notifyGetGoods(rakssId, icon, num);
	}

	static void sendShowSeasonRank(ui64 rakssId)
	{
		ServerNetwork::Instance()->getSender()->sendShowSeasonRank(rakssId);
	}
	
	static void sendBirdGain(ui64 rakssId, std::vector<BirdGain> items)
	{
		if (items.size() == 0)
		{
			return;
		}
		vector<BirdGain>::type newItems = vector<BirdGain>::type();
		for (auto item : items)
		{
			newItems.push_back(item);
		}
		ServerNetwork::Instance()->getSender()->sendBirdGain(rakssId, newItems);
	}

	static void sendBirdNestOperation(ui64 rakssId, bool isUnlock, i64 birdId)
	{
		ServerNetwork::Instance()->getSender()->sendBirdNestOperation(rakssId, isUnlock, birdId);
	}

	static void sendBirdLotteryResult(ui64 rakssId, String eggBodyId,  BirdInfo  info)
	{
		ServerNetwork::Instance()->getSender()->sendBirdLotteryResult(rakssId, eggBodyId, info);
	}

	static void setThirdPersonDistance(float distance)
	{
		LogicSetting::Instance()->setThirdPersonDistance(distance);
	}

	static void sendBirdAddScore(ui64 rakssId, i32 score, i32 scoreType)
	{
		ServerNetwork::Instance()->getSender()->sendBirdAddScore(rakssId, score, scoreType);
	}

	static void sendBirdReceiveTaskResult(ui64 rakssId)
	{
		ServerNetwork::Instance()->getSender()->sendRequestResult(rakssId, 0, ResultCode::BIRD_RECEIVE_TASK, "");
	}

	static void sendBirdSimulatorStoreItem(ui64 rakssId, const  BLOCKMAN::BirdStore& store)
	{
		auto clientPeer = ServerNetwork::Instance()->findPlayerByRakssid(rakssId);
		if (clientPeer && clientPeer->getEntityPlayer())
		{
			clientPeer->getEntityPlayer()->m_birdSimulator->setStoreItem(store);
		}
		ServerNetwork::Instance()->getSender()->syncBirdSimulatorStoreItem(rakssId, store);
	}

	static void sendBirdSimulatorTaskItem(ui64 rakssId, bool isRemove, const BLOCKMAN::BirdTask & taskItem)
	{
		auto clientPeer = ServerNetwork::Instance()->findPlayerByRakssid(rakssId);
		if (clientPeer && clientPeer->getEntityPlayer())
		{
			clientPeer->getEntityPlayer()->m_birdSimulator->setTaskItem(isRemove, taskItem);
		}
		ServerNetwork::Instance()->getSender()->syncBirdSimulatorTaskItem(rakssId, isRemove, taskItem);
	}

	static void sendBirdSimulatorPersonStoreTab(ui64 rakssId, const BLOCKMAN::BirdPersonalStoreTab & storeTab)
	{
		auto clientPeer = ServerNetwork::Instance()->findPlayerByRakssid(rakssId);
		if (clientPeer && clientPeer->getEntityPlayer())
		{
			clientPeer->getEntityPlayer()->m_birdSimulator->setPersonalStoreTab(storeTab);
		}
		ServerNetwork::Instance()->getSender()->syncBirdSimulatorPersonStoreTab(rakssId, storeTab);
	}

	static int random(LORD::String name, int begin, int end)
	{
		return NewRandom::Instance()->random(begin, end, name);
	}

	static void changeGuideArrowStatus(ui64 rakssId, Vector3 arrowPos, bool flag)
	{
		ServerNetwork::Instance()->getSender()->changeGuideArrowStatus(rakssId, arrowPos, flag);
	}

	static void sendCommonTipByType(ui64 rakssId, i32 commonTipType, const char* msg)
	{
		ServerNetwork::Instance()->getSender()->sendRequestResult(rakssId, 0, (ResultCode)commonTipType, msg);
	}

	static void addWallText(String text, Vector3 textPos, float scale, float yaw, float pitch, float r, float g, float b, float a)
	{
		ServerNetwork::Instance()->getSender()->broadCastAddWallText(text, textPos, scale, yaw, pitch, r, g, b, a);
	}

	static void deleteWallText(Vector3 textPos)
	{
		ServerNetwork::Instance()->getSender()->broadCastDeleteWallText(textPos);
	}

};

