#include "BattleSummary.h"

#include "cWorld/Blockman.h"
#include "World/GameSettings.h"
#include "Render/CameraGod.h"

#include "Core.h"
#include "External/cjson.h"
#include "Util/QueryObject.h"
#include "Object/Root.h"
#include "GLES2Render.h"
#include "UI/GUISystem.h"
#include "iniFileObj.h"

#include "Memory/MemTracker.h"

#include "Render/Material.h"
#include "Audio/SoundSystem.h"
#include "Resource/ResourcePack.h"
#include "UI/GUIWindowManager.h"
#include "UI/GUIWindow.h"
#include "UI/Touch.h"
#include "UI/GUIFontManager.h"
#include "UI/GUIRenderText.h"
#include "UI/GUIRenderManager.h"
#include "Util/ClientEvents.h"
#include "Util/md5.h"
#include "Common.h"
#include "ShellInterface.h"
#include "Util/LanguageManager.h"
#include "Util/Event.h"
#include "cWorld/MapManager.h"
#include "Util/HttpPromise.h"
#include "Util/Concurrent/WorkerThread.h"
#include "Util/ClientDataReport.h"
#include "Setting/ScorePointSetting.h"
#include "Setting/GameTypeSetting.h"
#include "game.h"

using namespace BLOCKMAN;

namespace GameClient
{
	HungerGameResult::HungerGameResult(LORD::String & gameResult)
	{
		//Todo, parse from json string 
		kills = 0;
		exp = 100;
		goldCoins = 3;
		activeValues = 100;
	}

	ResultEntry::ResultEntry(String& sName, int nRank,int nKillNum, int bIsWin) :
		name(sName),
		rank(nRank),
		killNum(nKillNum),
		isWin(bIsWin)
	{
		isSelf = false;
		reward = 0;
		todayGetReward = 0;
		canGetReward = 0;
		vip = 0;
		user_id = 0;
		build_war_score = 0;
	}

	ResultEntry::~ResultEntry()
	{
		pointMap.clear();
	}

	ResultEntry::ResultEntry(const String& gameResult)
	{
		//LordLogInfo("ResultEntry is %s", gameResult.c_str());
		//String testResult = "{\"score\":\"0(0 % %)\",\"isWin\":0,\"money\":0,\"userId\":4195329,\"flags\":0,\"name\":\"test1632\",\"exp\":0,\"killanddie\":\"0 / 0\",\"clanName\":\"46464\",\"energy\":0,\"rank\":1,\"points\":{\"1\":2,\"3\": 3}}";

		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(gameResult.c_str());

		if (doc->HasParseError())
		{
			LordLogError("The game result content  HasParseError.");
			LordDelete(doc);
		}

		if (!doc->HasMember("name") || !doc->HasMember("rank") || !doc->HasMember("points") || !doc->HasMember("gold") || !doc->HasMember("hasGet") || !doc->HasMember("available") || !doc->HasMember("vip"))
		{
			LordLogError("The game result content missed some field.");
			LordDelete(doc);
		}

		this->name = doc->FindMember("name")->value.GetString();
		this->rank = doc->FindMember("rank")->value.GetInt();
		this->killNum = 0;
		if (doc->HasMember("kills")) {
			this->killNum = doc->FindMember("kills")->value.GetInt();
		}
		this->reward = doc->FindMember("gold")->value.GetInt();
		this->todayGetReward = doc->FindMember("hasGet")->value.GetInt();
		this->canGetReward = doc->FindMember("available")->value.GetInt();
		this->vip = doc->FindMember("vip")->value.GetInt();
		auto pointData = doc->FindMember("points")->value.GetObject();
		for (int i = 1; i <= ScorePointSetting::getScorePointCount(); i++)
		{
			bool hasMember = pointData.HasMember(StringUtil::ToString(i).c_str());
			if (hasMember)
			{
				this->pointMap[i] = pointData.FindMember(StringUtil::ToString(i).c_str())->value.GetInt();
			}
		}

		this->adSwitch = 0;
		if (doc->HasMember("adSwitch"))
		{
			this->adSwitch = doc->FindMember("adSwitch")->value.GetInt();
		}

		LordDelete(doc);
	}

	ResultEntry::ResultEntryListPtr ResultEntry::getResultEntryList(const String& gameResult)
	{
		ResultEntry::ResultEntryListPtr ptr = LORD::make_shared<list<ResultEntryPtr>::type>();

		//String testResult = "{\"own\":{\"name\":\"test1632\",\"rank\":1,\"isWin\":0},\"players\":[{\"name\":\"test111\",\"rank\":2,\"isWin\":1},{\"name\":\"test1632\",\"rank\":1,\"isWin\":0}]}";
		//String testResult = gameResult;
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(gameResult.c_str());

		if (doc->HasParseError())
		{
			LordLogError("The game result content  HasParseError.");
			LordDelete(doc);
			return ptr;
		}

		if (!doc->HasMember("own") || !doc->HasMember("players"))
		{
			LordLogError("The game result content missed some field.");
			LordDelete(doc);
			return ptr;
		}

		String myName = doc->FindMember("own")->value.FindMember("name")->value.GetString();

		rapidjson::Value arr = doc->FindMember("players")->value.GetArray();
		for (size_t i = 0; i < arr.Size(); ++i)
		{
			if (!arr[i].HasMember("name") || !arr[i].HasMember("rank") || !arr[i].HasMember("isWin") || !arr[i].HasMember("points")
				|| !arr[i].HasMember("gold") || !arr[i].HasMember("hasGet") || !arr[i].HasMember("available") || !arr[i].HasMember("vip"))
			{
				LordLogError("The game result content missed some field.");
				LordDelete(doc);
				return ptr;
			}

			String playerName = arr[i].FindMember("name")->value.GetString();
			int playerRank = arr[i].FindMember("rank")->value.GetInt();
			int isWin = arr[i].FindMember("isWin")->value.GetInt();
			int playerKillNum = 0;
			if (arr[i].HasMember("kills"))
			{
				playerKillNum = arr[i].FindMember("kills")->value.GetInt();
			}
			ResultEntryPtr re = LORD::make_shared<ResultEntry>(playerName, playerRank, playerKillNum, isWin);
			re->isSelf = re->name == myName;
			re->reward = arr[i].FindMember("gold")->value.GetInt();
			re->todayGetReward = arr[i].FindMember("hasGet")->value.GetInt();
			re->canGetReward = arr[i].FindMember("available")->value.GetInt();
			re->vip = arr[i].FindMember("vip")->value.GetInt();

			if (arr[i].HasMember("jewelKnight"))
			{
				re->jewelKnight = arr[i].FindMember("jewelKnight")->value.GetString();
			}

			if (arr[i].HasMember("adSwitch"))
			{
				re->adSwitch = arr[i].FindMember("adSwitch")->value.GetInt();
			}

			auto pointData = arr[i].FindMember("points")->value.GetObject();
			for (int i = 1; i <= ScorePointSetting::getScorePointCount(); i++)
			{
				bool hasMember = pointData.HasMember(StringUtil::ToString(i).c_str());
				if (hasMember)
				{
					re->pointMap[i] = pointData.FindMember(StringUtil::ToString(i).c_str())->value.GetInt();
				}
			}

			if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::Build_War)
			{
				if (arr[i].HasMember("user_id"))
				{
					re->user_id = arr[i].FindMember("user_id")->value.GetUint64();
				}

				if (arr[i].HasMember("build_war_score"))
				{
					re->build_war_score = arr[i].FindMember("build_war_score")->value.GetInt();
				}
			}

			ptr->push_back(re);
		}

		ptr->sort(ResultEntry::CompareResultEntry);
		LordDelete(doc);
		return ptr;
	}

	int ResultEntry::getIsWin(const String & gameResult)
	{
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(gameResult.c_str());

		if (doc->HasParseError())
		{
			LordLogError("The game result content  HasParseError.");
			LordDelete(doc);
			return -1;
		}

		if (!doc->HasMember("own") || !doc->HasMember("players"))
		{
			LordLogError("The game result content missed some field.");
			LordDelete(doc);
			return -1;
		}

		int isWin = doc->FindMember("own")->value.FindMember("isWin")->value.GetInt();

		return isWin;
	}

	bool ResultEntry::CompareResultEntry(const ResultEntryPtr a, const ResultEntryPtr b)
	{
		return a->rank < b->rank;
	}
}