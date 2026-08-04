#include "FriendManager.h"
#include "Util/ClientEvents.h"
#include "game.h"
#include "LanguageManager.h"

using namespace rapidjson;

namespace BLOCKMAN
{
	Clan* FriendManager::m_myClan = new Clan();
	map<i64, Friend*>::type  FriendManager::m_friendMap = {};
	map<i64, Friend*>::type  FriendManager::m_clanMemberMap = {};
	map<i64, UserActor*>::type  FriendManager::m_actorInfoMap = {};

	void FriendManager::init()
	{
		releaseFriend();
		releaseUserActor();
		releaseClanMember();
	}

	void FriendManager::release()
	{
		releaseFriend();
		releaseUserActor();
		releaseClanMember();
		if (m_myClan)
		{
			delete m_myClan;
			m_myClan = nullptr;
		}
	}

	void FriendManager::addFriend(Friend * pal)
	{
		m_friendMap[pal->userId] = pal;
	}

	Friend * FriendManager::findFriendById(i64 userId)
	{
		auto iter = m_friendMap.find(userId);
		if (iter != m_friendMap.end())
		{
			return iter->second;
		}
		return nullptr;
	}

	vector<Friend*>::type FriendManager::getFriends()
	{
		vector<Friend*>::type frends = vector<Friend*>::type();
		for (auto item : m_friendMap)
		{
			if (item.second)
			{
				frends.push_back(item.second);
			}
			else
			{
				LordLogError("getFriends is null");
			}
		}
		if (frends.size() >= 2) {
			sort(frends.begin(), frends.end(), FriendSort());
		}
		return frends;
	}

	void FriendManager::addMember(Friend * member)
	{
		m_clanMemberMap[member->userId] = member;
	}

	Friend * FriendManager::findMemberById(i64 userId)
	{
		auto iter = m_clanMemberMap.find(userId);
		if (iter != m_clanMemberMap.end())
		{
			return iter->second;
		}
		return nullptr;
	}

	vector<Friend*>::type FriendManager::getMembers()
	{
		vector<Friend*>::type frends = vector<Friend*>::type();
		for (auto item : m_clanMemberMap)
		{
			if (item.second)
			{
				frends.push_back(item.second);
			}
			else
			{
				LordLogError("getMembers is null");
			}
		}
		if (frends.size() >= 2) {
			sort(frends.begin(), frends.end(), FriendSort());
		}
		return frends;
	}

	Friend * FriendManager::findUserById(i64 userId)
	{
		auto pal = findFriendById(userId);
		return pal ? pal : findMemberById(userId);
	}

	void FriendManager::addUserActor(UserActor* info)
	{
		m_actorInfoMap[info->userId] = info;
	}

	UserActor* FriendManager::findUserActor(i64 userId)
	{
		auto iter = m_actorInfoMap.find(userId);
		if (iter != m_actorInfoMap.end())
		{
			return iter->second;
		}
		return nullptr;
	}

	void FriendManager::parseFriendData(const String & data)
	{
		releaseFriend();
		Document  doc = Document();
		doc.Parse(data.c_str());
		if (!doc.HasParseError() && doc.HasMember("data"))
		{
			auto content = doc.FindMember("data")->value.GetArray();
			for (const auto& item : content)
			{
				Friend* pal = new Friend();

				if (item.HasMember("sex"))
				{
					pal->sex = item.FindMember("sex")->value.GetInt();
				}

				if (item.HasMember("status"))
				{
					pal->isOnline = item.FindMember("status")->value.GetInt() > 10;
				}


				if (item.HasMember("userId"))
				{
					pal->userId = item.FindMember("userId")->value.GetInt64();
				}

				if (item.HasMember("clanId"))
				{
					pal->clanId = item.FindMember("clanId")->value.GetInt64();
				}

				if (item.HasMember("clanName"))
				{
					if (!item.FindMember("clanName")->value.IsNull())
					{
						pal->clanName = item.FindMember("clanName")->value.GetString();
					}
				}

				if (item.HasMember("picUrl"))
				{
					if (!item.FindMember("picUrl")->value.IsNull())
					{
						pal->picUrl = item.FindMember("picUrl")->value.GetString();
					}
				}

				if (item.HasMember("nickName"))
				{
					if (!item.FindMember("nickName")->value.IsNull())
					{
						pal->nickName = item.FindMember("nickName")->value.GetString();
					}

				}

				if (item.HasMember("gameId"))
				{
					if (!item.FindMember("gameId")->value.IsNull())
					{
						pal->inGameType = item.FindMember("gameId")->value.GetString();
					}
				}

				addFriend(pal);
			}
		}
	}

	void FriendManager::parseClanMember(const String & data)
	{
		releaseClanMember();
		Document  doc = Document();
		doc.Parse(data.c_str());
		if (!doc.HasParseError() && doc.IsArray())
		{
			auto content = doc.GetArray();
			for (const auto& item : content)
			{
				Friend* pal = new Friend();

				if (item.HasMember("sex"))
				{
					pal->sex = item.FindMember("sex")->value.GetInt();
				}

				if (item.HasMember("status"))
				{
					pal->isOnline = item.FindMember("status")->value.GetInt() > 10;
				}


				if (item.HasMember("userId"))
				{
					pal->userId = item.FindMember("userId")->value.GetInt64();
				}

				if (item.HasMember("clanId"))
				{
					pal->clanId = item.FindMember("clanId")->value.GetInt64();
				}

				if (item.HasMember("headPic"))
				{
					if (!item.FindMember("headPic")->value.IsNull())
					{
						pal->picUrl = item.FindMember("headPic")->value.GetString();
					}
				}

				if (item.HasMember("clanName"))
				{
					if (!item.FindMember("clanName")->value.IsNull())
					{
						pal->clanName = item.FindMember("clanName")->value.GetString();
					}
				}

				if (item.HasMember("nickName"))
				{
					if (!item.FindMember("nickName")->value.IsNull())
					{
						pal->nickName = item.FindMember("nickName")->value.GetString();
					}

				}

				if (item.HasMember("gameId"))
				{
					if (!item.FindMember("gameId")->value.IsNull())
					{
						pal->inGameType = item.FindMember("gameId")->value.GetString();
					}
				}
				if (pal->userId != GameClient::CGame::Instance()->getPlatformUserId())
				{
					addMember(pal);
				}
				else
				{
					delete pal;
					pal = nullptr;
				}

			}
		}
	}

	void FriendManager::parseClanData(const String & data)
	{
		if (!m_myClan)
		{
			m_myClan = new Clan();
		}
		Document  doc = Document();
		doc.Parse(data.c_str());
		if (!doc.HasParseError())
		{
			if (doc.HasMember("clanId"))
			{
				m_myClan->clanId = doc.FindMember("clanId")->value.GetInt64();
			}

			if (doc.HasMember("name"))
			{
				if (!doc.FindMember("name")->value.IsNull())
				{
					m_myClan->clanName = doc.FindMember("name")->value.GetString();
				}
			}
		}
	}

	void FriendManager::parseRanchData(bool isFirend, const String & data)
	{
		Document  doc = Document();
		doc.Parse(data.c_str());
		if (!doc.HasParseError() && doc.IsArray())
		{
			const auto& content = doc.GetArray();
			for (const auto& item : content)
			{
				Friend* pal = nullptr;
				if (item.HasMember("userId"))
				{
					i64 userId = item.FindMember("userId")->value.GetInt64();
					pal = isFirend ? findFriendById(userId) : findMemberById(userId);
				}

				if (pal)
				{
					pal->hasRanch = true;
				}

				if (pal && item.HasMember("level"))
				{
					pal->level = item.FindMember("level")->value.GetInt();
				}

				if (pal && item.HasMember("gift") && !item.FindMember("gift")->value.IsNull())
				{
					pal->gitfNum = item.FindMember("gift")->value.GetInt();
				}

				if (pal && item.HasMember("hasSendGift") && !item.FindMember("hasSendGift")->value.IsNull())
				{
					pal->isGive = item.FindMember("hasSendGift")->value.GetBool();
				}

				if (pal && item.HasMember("prosperity") && !item.FindMember("prosperity")->value.IsNull())
				{
					pal->prosperity = item.FindMember("prosperity")->value.GetInt();
				}

				if (pal && item.HasMember("helps") && item.FindMember("helps")->value.IsArray())
				{
					const auto& helps = item.FindMember("helps")->value.GetArray();
					pal->orders.clear();
					for (const auto& help : helps)
					{
						RanchOrderInfo info = parseOderInfo(help);
						if (info.helpId > 0 && info.fullBoxNum < info.boxAmount && info.helperId == 0)
						{
							pal->orders.push_back(parseOderInfo(help));
						}
						else
						{
							LordLogWarning("parseRanchData RanchOrderInfo info.helpId = [%lld], info.fullBoxNum  >= info.boxAmount ||  info.helperId = [%lld]", info.helpId, info.helperId)
						}
					}
					pal->needHelp = pal->orders.size() > 0;
				}
			}
		}

		if (isFirend)
		{
			FrendDataChangeEvent::emit();
		}
		else
		{
			ClanMemberDataChangeEvent::emit();
		}
	}

	void FriendManager::parseUserInfo(i64 userId, const String & data)
	{
		Document doc = Document();
		doc.Parse(data.c_str());
		if (!doc.HasParseError() && doc.IsObject())
		{
			UserActor* userActor = findUserActor(userId);
			if (!userActor) userActor = new UserActor();

			userActor->userId = userId;

			if (doc.HasMember("sex") && !doc.FindMember("sex")->value.IsNull())
			{
				userActor->sex = doc.FindMember("sex")->value.GetInt();
			}

			if (doc.HasMember("clanId") && !doc.FindMember("clanId")->value.IsNull())
			{
				userActor->clanId = doc.FindMember("clanId")->value.GetInt64();
			}

			if (doc.HasMember("clanName") && !doc.FindMember("clanName")->value.IsNull())
			{
				userActor->clanName = doc.FindMember("clanName")->value.GetString();
			}

			if (doc.HasMember("nickName") && !doc.FindMember("nickName")->value.IsNull())
			{
				userActor->nickName = doc.FindMember("nickName")->value.GetString();
			}
			addUserActor(userActor);
		}
	}

	void FriendManager::parseUserDress(i64 userId, const String & data)
	{
		Document doc = Document();
		doc.Parse(data.c_str());
		if (!doc.HasParseError() && doc.IsArray())
		{
			auto userActor = findUserActor(userId);
			if (userActor)
			{
				userActor->items.clear();
				const auto& content = doc.GetArray();
				for (const auto& item : content)
				{
					i32 type = 0;
					if (item.HasMember("typeId") && !item.FindMember("typeId")->value.IsNull())
					{
						type = item.FindMember("typeId")->value.GetInt();
					}

					if (item.HasMember("resourceId") && !item.FindMember("resourceId")->value.IsNull())
					{
						String resourceId = item.FindMember("resourceId")->value.GetString();
						if (type != 6)
						{
							StringArray  resourceArray = StringUtil::Split(resourceId, ".");
							LordAssert(resourceArray.size() == 2);
							DressInfo info = DressInfo();
							info.resName = resourceArray[0];
							info.resId = resourceArray[1];
							userActor->items.push_back(info);
						}
						else
						{
							StringArray  resourceArray = StringUtil::Split(resourceId, "-");
							LordAssert(resourceArray.size() == 4);
							resourceArray[0].c_str();
							float r = (float)atof(resourceArray[0].c_str()) / 255.f;
							float g = (float)atof(resourceArray[1].c_str()) / 255.f;
							float b = (float)atof(resourceArray[2].c_str()) / 255.f;
							float a = (float)atof(resourceArray[3].c_str()) / 255.f;

							userActor->actorColor = Color(r, g, b, a);
						}

					}
				}
				UserActorChangeEvent::emit(userId);
			}
		}
	}

	void FriendManager::releaseFriend()
	{
		for (auto item : m_friendMap)
		{
			if (item.second)
			{
				delete item.second;
				item.second = nullptr;
			}
		}
		m_friendMap.clear();
	}

	void FriendManager::releaseUserActor()
	{
		for (auto item : m_actorInfoMap)
		{
			if (item.second)
			{
				delete item.second;
				item.second = nullptr;
			}
		}
		m_actorInfoMap.clear();
	}

	void FriendManager::releaseClanMember()
	{
		for (auto item : m_clanMemberMap)
		{
			if (item.second)
			{
				delete item.second;
				item.second = nullptr;
			}
		}
		m_clanMemberMap.clear();
	}

	RanchOrderInfo FriendManager::parseOderInfo(const Value & doc)
	{
		RanchOrderInfo orderInfo = RanchOrderInfo();

		if (doc.HasMember("id") && !doc.FindMember("id")->value.IsNull())
		{
			orderInfo.helpId = doc.FindMember("id")->value.GetInt64();
		}

		if (doc.HasMember("itemId") && !doc.FindMember("itemId")->value.IsNull())
		{
			orderInfo.itemId = doc.FindMember("itemId")->value.GetInt();
		}

		if (doc.HasMember("boxId") && !doc.FindMember("boxId")->value.IsNull())
		{
			orderInfo.index = doc.FindMember("boxId")->value.GetInt();
		}

		if (doc.HasMember("boxAmount") && !doc.FindMember("boxAmount")->value.IsNull())
		{
			orderInfo.boxAmount = doc.FindMember("boxAmount")->value.GetInt();
		}

		if (doc.HasMember("fullBoxNum") && !doc.FindMember("fullBoxNum")->value.IsNull())
		{
			orderInfo.fullBoxNum = doc.FindMember("fullBoxNum")->value.GetInt();
		}

		if (doc.HasMember("uniqueId") && !doc.FindMember("uniqueId")->value.IsNull())
		{
			orderInfo.uniqueId = doc.FindMember("uniqueId")->value.GetString();
		}

		if (doc.HasMember("itemCount") && !doc.FindMember("itemCount")->value.IsNull())
		{
			orderInfo.needNum = doc.FindMember("itemCount")->value.GetInt();
		}

		if (doc.HasMember("helperId") && !doc.FindMember("helperId")->value.IsNull())
		{
			orderInfo.helperId = doc.FindMember("helperId")->value.GetInt64();
		}

		if (doc.HasMember("isHot") && !doc.FindMember("isHot")->value.IsNull())
		{
			orderInfo.isHot = doc.FindMember("isHot")->value.GetBool();
		}

		return orderInfo;
	}

}