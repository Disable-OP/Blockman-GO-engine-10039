#pragma once

#ifndef __FRIEND_MANAGER_HEADER__
#define __FRIEND_MANAGER_HEADER__
#include "Core.h"
#include <sstream>
#include "Object/Root.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "World/Ranch.h"
#include "rapidjson/document.h"

using namespace LORD;

namespace BLOCKMAN {

	class Clan : public ObjectAlloc {
	public:
		i64 clanId = 0;
		String clanName = "";
		Clan() {}
	};

	class Friend : public ObjectAlloc {
	public:
		i32 sex = 0;
		i32 level = 0;
		i64 clanId = 0;
		i64 userId = 0;

		i32 gitfNum = 0;
		i32 prosperity = 0;

		String picUrl = "";
		String clanName = "";
		String nickName = "";
		String inGameType = "";

		bool isGive = false;
		bool isOnline = false;
		bool hasRanch = false;
		bool needHelp = false;

		vector<RanchOrderInfo>::type orders;
		Friend() {}

	};

	class DressInfo : public ObjectAlloc
	{
	public:
		String resName = "";
		String resId = "";
		DressInfo() {}
	};

	class UserActor : public ObjectAlloc
	{
	public:
		i32 sex = 1;
		i64 clanId = 0;
		i64 userId = 0;
		String clanName = "";
		String nickName = "";
		Color actorColor = Color::TRANS;
		vector<DressInfo>::type items;
		UserActor() {}
	};

	struct FriendSort
	{
		bool operator()(Friend* first, Friend* second)
		{
			if (first && second) {
				return first->prosperity > second->prosperity;
			}
			return false;
		}
	};


	class FriendManager : public ObjectAlloc
	{
	public:
		static void init();
		static void release();
		static void addFriend(Friend* pal);
		static Friend* findFriendById(i64 userId);
		static vector<Friend*>::type getFriends();


		static void addMember(Friend* member);
		static Friend* findMemberById(i64 userId);
		static vector<Friend*>::type getMembers();

		static Friend* findUserById(i64 userId);


		static void addUserActor( UserActor* info);
		static UserActor* findUserActor(i64 userId);

		static void parseFriendData(const String & data);
		static void parseClanMember(const String & data);
		static void parseClanData(const String & data);
		static void parseRanchData(bool isFirend, const String & data);
		static void parseUserInfo(i64 userId, const String & data);
		static void parseUserDress(i64 userId, const String & data);

	private:
		static void releaseFriend();
		static void releaseUserActor();
		static void releaseClanMember();

		static RanchOrderInfo parseOderInfo(const rapidjson::Value& doc);

	private:
		static Clan* m_myClan;
		static map<i64, Friend*>::type m_friendMap;
		static map<i64, Friend*>::type m_clanMemberMap;
		static map<i64, UserActor*>::type m_actorInfoMap;

	};

}
#endif