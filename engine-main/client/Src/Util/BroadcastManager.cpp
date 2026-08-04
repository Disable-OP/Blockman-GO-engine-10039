#include "BroadcastManager.h"
#include "Common.h"
#include "Util/ClientEvents.h"
#include <sstream>
#include "rapidjson/writer.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"
#include "Util/FriendManager.h"
#include "Enums/Enums.h"
#include "Util/LanguageManager.h"

using namespace rapidjson;

namespace BLOCKMAN
{

	void BroadcastManager::parseBroadcastData(i32 type, const String & content)
	{
		switch ((BroadcastType)type)
		{
		case BroadcastType::RANCH_ORDER_HELP:
			praseRanchOrderHelp(content);
			break;
		case BroadcastType::RANCH_ORDER_FINISH:
			praseRanchOrderFinish(content);
			break;
		case BroadcastType::RANCH_RANK_ENTER_GAME:
			praseRanchEnterGame(content);
			break;
		default:
			praseCommon(content);
			break;
		}

	}

	void BroadcastManager::praseRanchOrderHelp(const String & content)
	{
		Document  doc = Document();
		doc.Parse(content.c_str());
		if (!doc.HasParseError())
		{
			if (doc.HasMember("userId") && doc.HasMember("id"))
			{
				i64 userId = doc.FindMember("userId")->value.GetInt64();
				i64 helpId = doc.FindMember("id")->value.GetInt64();
				const auto& pal = FriendManager::findUserById(userId);
				if (pal && LanguageManager::Instance())
				{
					String msg = StringUtil::Format(LanguageManager::Instance()->getString("gui_ranch_broadcast_order_help").c_str(), pal->nickName.c_str());
					BroadcastReceiveEvent::emit(BroadcastMessage(msg, 5000, 5000));
				}
			}
		}
		FriendUpdateEvent::emit();
	}

	void BroadcastManager::praseRanchOrderFinish(const String & content)
	{
		Document  doc = Document();
		doc.Parse(content.c_str());
		if (!doc.HasParseError())
		{
			if (doc.HasMember("helperUserId") && doc.HasMember("id"))
			{
				i64 userId = doc.FindMember("helperUserId")->value.GetInt64();
				i64 helpId = doc.FindMember("id")->value.GetInt64();
				const auto& pal = FriendManager::findUserById(userId);
				if (pal && LanguageManager::Instance())
				{
					String msg = StringUtil::Format(LanguageManager::Instance()->getString("gui_ranch_broadcast_order_help_done").c_str(), pal->nickName.c_str());
					BroadcastReceiveEvent::emit(BroadcastMessage(msg, 5000, 5000));
				}
			}
		}
	}

	void BroadcastManager::praseRanchEnterGame(const String & content)
	{
		Document  doc = Document();
		doc.Parse(content.c_str());
		if (!doc.HasParseError())
		{
			if (doc.HasMember("name ") && doc.HasMember("msg") && doc.HasMember("stayTime") && doc.HasMember("rollTime"))
			{
				String name = doc.FindMember("name")->value.GetString();
				String msg = doc.FindMember("msg")->value.GetString();
				i32 stayTime = doc.FindMember("stayTime")->value.GetInt();
				i32 rollTime = doc.FindMember("rollTime")->value.GetInt();
				String message = StringUtil::Format(LanguageManager::Instance()->getString(msg).c_str(), name.c_str());
				BroadcastReceiveEvent::emit(BroadcastMessage(message, rollTime, stayTime));
			}
		}
	}

	void BroadcastManager::praseCommon(const String & content)
	{
		Document  doc = Document();
		doc.Parse(content.c_str());
		if (!doc.HasParseError())
		{

		}
	}
}
