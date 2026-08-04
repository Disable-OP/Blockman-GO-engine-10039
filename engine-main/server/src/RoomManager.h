#ifndef ROOM_MANAGER_H
#define ROOM_MANAGER_H
#include "Core.h"
#include "Network/RoomClient.h"

using namespace LORD;

struct UserAttrInfo {
	int64_t userId = 0;
	int64_t targetUserId = 0;
	int32_t classes = 0;
	int32_t team = 0;
	int32_t regionId = 0;
	int32_t manorId = 0;
	bool pioneer = false;
	int32_t vip = 0;
	int32_t sex = 0; // 1:boy, 2:girl
	int32_t faceId = 1;
	int32_t hairId = 1;
	int32_t topsId = 1;
	int32_t pantsId = 1;
	int32_t shoesId = 1;
	int32_t glassesId = 0;
	int32_t scarfId = 0;
	int32_t wingId = 0;
	int32_t hatId = 0;
	int32_t decoratehatId = 0;
	int32_t armId = 0;
	int32_t tailId = 0;
	int32_t extrawingId = 0;
	int32_t footHaloId = 0;
	int32_t backEffectId = 0;
	int32_t headEffectId = 0;
	int32_t crownId = 0;
	int32_t bagId = 0;
	LORD::Color	 skinColor;
	uint64_t updateTimestamp = 0; // in seconds
	int32_t defaultIdle = 0;// 0 = animation_idle.0, 1 = animation_idle.1, ...
};

struct ManorAttrInfo
{
	int32_t location = 0;
	int64_t targetUserId = 0;
};

struct BroadcastMessage
{
	// Type is msg type
	i32 type = 0;

	// Scope is msg notice scope [all, game, user]
	String scope = "";

	// GameType must specific when scope = GameScope
	String gameType = "";

	// Targets is list of userId, when Scope = User Scope, Targets must not be empty
	vector<i64>::type targets;
	
	String content = "";
};

class RoomManager : public RoomMsgHandler{
public:
	typedef LORD::map<int64_t, UserAttrInfo>::type UserAttrInfoMap;
	typedef LORD::map<int64_t, ManorAttrInfo>::type ManorAttrInfoMap;

public:
	RoomManager(const std::string& host, unsigned short port, const RoomGameConfig& rgConfig);

	RoomGameConfig* getRoomGameConfig() {
		return &m_rgConfig; 
	}

	std::shared_ptr<RoomClient> getRoomClient() {
		return m_roomClient;
	}

	void tick();
	bool UserAttrExisted(int64_t userId);
	bool getUserAttrInfo(int64_t userId, UserAttrInfo& attrInfo);
	void clearWhenUserOut(int64_t userId);
	void setUserAttrInfoManorId(int64_t userId, int32_t manorId);

public:
	// implement RoomMsgHandler pure virtual function
	void onConnected();
	void onConnectReply();
	void onUserAttr(const UserAttrInfo& attr) override;
	void onUserManorRelease(int64_t userId) override;
	void onReceive(const BroadcastMessage& message) override;

private:
	void reviseUserAttrInfoMap();

private:
	RoomGameConfig m_rgConfig;
	std::shared_ptr<RoomClient> m_roomClient;

	UserAttrInfoMap m_userAttrInfoMap;
	ManorAttrInfoMap m_manorAttrInfoMap;

	bool m_isFirstConnect = true;
};

#endif // ROOM_MANAGER_H
