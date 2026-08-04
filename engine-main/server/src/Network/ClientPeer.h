/********************************************************************
filename: 	ServerNetPlayer
file path:	H:\sandboxol\client\blockmango-client\dev\server\src\ServerNetPlayer.h

version:	1
author:		qutianxiang
company:    sandboxol
date:		2017/06/23
*********************************************************************/

#ifndef _SERVERPLAYER_H__
#define _SERVERPLAYER_H__
#include "Global.h"
#include "Server.h"

#include "Blockman/Entity/EntityPlayerMP.h"
#include "World/Shop.h"
#include "Setting/GunSetting.h"

class ClientPeer :public ObjectAlloc
{
public:
	enum PlayerState
	{
		emConnected,
		emLogined,
		emPlaying,
		emDisconnected
	};

private:
	class OrderPayResult
	{
	public:
		i32 code = 0;
		String orderId = "";
		bool isConsume = true;
	};

public:
	ClientPeer(ui64 rakssid);
	virtual ~ClientPeer();

	static std::shared_ptr<ClientPeer> createPlayer(ui64 rakssid)
	{
		auto player = LORD::make_shared<ClientPeer>(rakssid);
		return player;
	}

	void onTick();

	void removePlayerEntity();

	int getEntityRuntimeID();

	bool hasLogon() { return m_logon; }

	void setTeamId(int teamId) { m_teamId = teamId; }

	int getTeamId() { return m_teamId; }

	void setTeamName(String teamName) { m_teamName = teamName; }

	String getTeamName() { return m_teamName; }

	void setTeamInfo(int teamId, String teamName);

	bool isVisitor();

	void setClientReady(i32 uiStatus) { clientUiStatus = uiStatus; }

	ui64 getPlatformUserId() { return m_platformUserId; }

	ui64 getRakssid() { return m_rakSsid; }

	String getName() { return m_name; }

	String getLanguage() { return m_Language; };

	void   setLanguage(String lan) { m_Language = lan; };

	void   setShowName(String name, ui64 targetId = 0);

	String getShowName() { return m_showName; }

	void setPlayerLocation(const Vector3& pos, float yaw, float pitch);

	void setPlatformUserId(ui64 platformUserId);

	void onSpawn(String name, EntityPlayerMP *mpPlayer);

	void login();

	void logout();

	void resetPos(float x, float y, float z, float yaw = 361);

	void movePlayer(float x, float y, float z, float yaw, float pitch, bool onGround, bool toMoving, bool rotating, bool fromServer = false);

	void doAttack(int targetEngityRuntimeId, float targetX, float targetY, float targetZ);

	void sendLoginResult(bool suc, int teamId, String teamName, int curPlayer, int maxPlayer);

	void doGunShootTarget(ui32 tracyType, ui32 hitEntityID, const C2SPACKET_DETAIL::Vector3& hitpos,
		bool isHeadshot, const GunSetting* pGunSetting);

	void reloadBullet();

	void loadUserMoney();

	BLOCKMAN::EntityPlayerMP * getEntityPlayer() { return m_entityPlayer; }

	void buyGoods(i32 groupIndex, i32 goodsIndex, i32 goodsId);

	String getHttpUrl();

	bool isPioneer();

	bool clientUiIsReady() { return clientUiStatus == 1; }

	void buyRespawn(bool isRespawn, i32 uniqueId, i32 index);

	void buyChangeActor(bool isCanChangeActor, int changeActorCount, int needMoneyCount);

	void pickupItemPay(i32 itemId, i32 itemMeta, i32 addNum, i32 price, i32 entityItemId, i32 currencyType);

	bool checkParam(String url, i32 currencyType, i32 price);

	void upgradeManor();

	void buyHouse(i32 houseId);

	void buyFurniture(i32 furnitureId);

	void consumeDiamonds(i32 uniqueId, i32 diamonds, String remark, bool isConsume);
	
	void buyRanchItem(i32 itemId, i32 addNum, i32 totalPrice, i32 currencyType);

	void buyRanchBuildItem(i32 itemId, i32 addNum, i32 type, i32 currencyType);

	void setRespawnPos(Vector3i position) { respawnPos = position; };

	void ranchShortcut(i32 landCode, i32 currencyType, i32 totalPrice);

	void buyRanchItems(vector<RanchCommon>::type items);

	void ranchBuildQueueOperation(i32 entityId, i32 type, i32 queueId, i32 productId);

	void speedUpCrop(Vector3i blockPos, i32 blockId, i32 totalPrice);

	void useCubeFinshOrder(i32 orderId, i32 index);

public:
	Vector3i getRespawnPos() { return respawnPos; }

	void stopLaserGun();

private:
	bool payResult(i64 platformUserId, String result, bool isSuccess);
	OrderPayResult payResultByConsume(i64 platformUserId, bool isSuccess , bool isConsume ,String result);

private:
	bool m_logon = false;
	int m_teamId = 0;
	ui64 m_rakSsid = 0;
	ui64 m_platformUserId = 0;
	String m_name = "noname";
	String m_showName = "noshowname";
	String m_teamName = "noteamname";
	PlayerState m_state = PlayerState::emDisconnected;
	String m_Language = "en_US";

	BLOCKMAN::EntityPlayerMP * m_entityPlayer = nullptr;
	i64 currentTicks = 0;
	int ticksForFloatKick = 0;
	int keepAliveRandomID = 0;
	i32 clientUiStatus = 0;
	i64 keepAliveTimeSent = 0;
	i64 ticksOfLastKeepAlive = 0;
	int chatSpamThresholdCount = 0;
	int creativeItemCreationSpamThresholdTally = 0;
	Vector3 lastPos{0,0,0};
	bool hasMoved = false;
	Vector3i respawnPos;
};

#endif