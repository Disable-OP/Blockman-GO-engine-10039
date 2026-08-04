#include "ClientHttpRequest.h"
#include "Util/sha1.h"
#include <sstream>
#include "Object/Root.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "World/CprCallBackWrap.h"
#include "Util/FriendManager.h"
#include "Util/MailManager.h"
#include "Util/ClientEvents.h"
#include "game.h"


using namespace rapidjson;

namespace BLOCKMAN
{
	const bool ClientHttpRequest::ENABLED_LOG = true;
	const int32_t ClientHttpRequest::HTTP_TIMEOUT = 6000;
	const String ClientHttpRequest::CONTENT_TYPE = "application/json; charset=UTF-8";
	

	void ClientHttpRequest::asyncGet(String url, map<String, String>::type params, std::function<void(String, bool)> fun)
	{
		cpr::Parameters param;
		std::stringstream ss;
		ss << url << "?";
		for (auto pr : params)
		{
			cpr::Parameter cprp(pr.first.c_str(), pr.second.c_str());
			param.AddParameter(cprp);
			ss << pr.first.c_str() << "=" << pr.second.c_str() << "&";
		}

		LordLogInfo("Client http get request:%s", ss.str().c_str());

		String stringUrl(url);
		auto callback = [fun, stringUrl](cpr::Response response)
		{
			if (response.status_code != 200)
			{
				int code = response.status_code;
				auto foo = [fun, code, stringUrl]()
				{
					fun("{}", false);
					LordLogError("Client Http Request async get error(%s), http status code : %d", stringUrl.c_str(), code);
				};
				GameClient::CGame::Instance()->addAsyncCall(foo);
				return;
			}
			String result = String(response.text.c_str());
			auto foo = [result, fun]()
			{
				fun(result, true);
			};
			GameClient::CGame::Instance()->addAsyncCall(foo);
		};

		CprCallBackWrap::Instance()->GetCallback(callback,
			cpr::Timeout{ HTTP_TIMEOUT },
			cpr::Url{ url.c_str() },
			param,
			getHttpHeader()
		);
	}

	void ClientHttpRequest::asyncPost(String url, map<String, String>::type params, String body, std::function<void(String, bool)> fun)
	{
		cpr::Parameters param;
		std::stringstream ss;
		ss << url << "?";
		for (auto pr : params)
		{
			cpr::Parameter cprp(pr.first.c_str(), pr.second.c_str());
			param.AddParameter(cprp);
			ss << pr.first.c_str() << "=" << pr.second.c_str() << "&";
		}
		LordLogInfo("Client http post request:%s", ss.str().c_str());

		String stringUrl(url);
		auto callback = [fun, stringUrl](cpr::Response response)
		{
			if (response.status_code != 200)
			{
				int code = response.status_code;
				auto foo = [fun, code, stringUrl]()
				{
					fun("{}", false);
					LordLogError("Client Httpa sync post error(%s), http status code : %d", stringUrl.c_str(), code);
				};
				GameClient::CGame::Instance()->addAsyncCall(foo);
				return;
			}
			String result = String(response.text.c_str());
			auto foo = [result, fun]()
			{
				fun(result, true);
			};
			GameClient::CGame::Instance()->addAsyncCall(foo);
		};

		CprCallBackWrap::Instance()->PostCallback(callback,
			cpr::Timeout{ HTTP_TIMEOUT },
			cpr::Url{ url.c_str() },
			param,
			cpr::Body{ body.c_str() },
			getHttpHeader()
		);
	}


	ClientHttpResponse ClientHttpRequest::parseResponse(bool isSuccess, String logTag, String response)
	{
		ClientHttpResponse result = ClientHttpResponse();
		Document doc = Document();
		doc.Parse(response.c_str());
		if (isSuccess && !doc.HasParseError() && doc.HasMember("code"))
		{
			int code = doc.FindMember("code")->value.GetInt();
			result.code = code;
			if (code == 1 && doc.HasMember("data"))
			{
				auto data = doc.FindMember("data");
				StringBuffer buffer;
				Writer<StringBuffer> writer(buffer);
				data->value.Accept(writer);
				result.data = String(buffer.GetString());
			}
		}

		if (result.code == 1)
		{
			if (ENABLED_LOG)
			{
				LordLogInfo("Client http  request: %s parse response succeed, result: %s", logTag.c_str(), result.data.length() < 1000 ? result.data.c_str() : "result.length > 1000");
			}
		}
		else
		{
			LordLogError("Client http  request: %s parse response error, response: %s", logTag.c_str(), response.c_str());
		}
		return result;
	}

	void ClientHttpRequest::refreshFriend()
	{
		loadFriend();
		loadClanMember();
	}

	void ClientHttpRequest::loadFriend()
	{
		String finalUrl = StringUtil::Replace("{url}/friend/api/v1/friends", "{url}", getBaseUrl().c_str());
		map<String, String>::type params = {
			{ "pageNo","0"},
			{ "pageSize","50" },
		};
		asyncGet(finalUrl, params, [=](String result, bool isSuccess) {
			ClientHttpResponse response = parseResponse(isSuccess, "loadFriend", result);
			if (response.code == 1)
			{
				FriendManager::parseFriendData(response.data);
				loadUserRanchInfo(true);
			}
		});
	}

	void ClientHttpRequest::loadClanInfo()
	{
		String finalUrl = StringUtil::Replace("{url}/clan/api/v1/clan/tribe/base", "{url}", getBaseUrl().c_str());
		asyncGet(finalUrl, map<String, String>::type(), [=](String result, bool isSuccess) {
			ClientHttpResponse response = parseResponse(isSuccess, "loadClanInfo", result);
			if (response.code == 1)
			{
				FriendManager::parseClanData(response.data);
			}
		});

	}

	void ClientHttpRequest::loadClanMember()
	{
		String finalUrl = StringUtil::Replace("{url}/clan/api/v1/clan/tribe/member", "{url}", getBaseUrl().c_str());
		asyncGet(finalUrl, map<String, String>::type(), [=](String result, bool isSuccess) {
			ClientHttpResponse response = parseResponse(isSuccess, "loadClanMember", result);
			if (response.code == 1)
			{
				FriendManager::parseClanMember(response.data);
				loadUserRanchInfo(false);
			}
		});
	}

	void ClientHttpRequest::loadUserRanchInfo(bool isFriend)
	{
		String finalUrl = StringUtil::Replace("{url}/gameaide/api/v1/farm/user-data", "{url}", getBaseUrl().c_str());
		map<String, String>::type p = map<String, String>::type();
		String str = "";
		int i = 0;
		const auto& friends = isFriend ? FriendManager::getFriends() : FriendManager::getMembers();
		for (auto pal : friends)
		{
			i++;
			if (i < (int)friends.size())
			{
				str += StringUtil::ToString(pal->userId) + ",";
			}
			else 
			{
				str += StringUtil::ToString(pal->userId);
			}
			
		}
		p["ids"] = str;
		p["userId"] = getUserId();
		asyncGet(finalUrl, p, [=](String result, bool isSuccess) {
			ClientHttpResponse response = parseResponse(isSuccess, "loadUserRanchInfo", result);
			if (response.code == 1)
			{
				FriendManager::parseRanchData(isFriend, response.data);
			}
		});
	}

	void ClientHttpRequest::loadMails()
	{
		String finalUrl = StringUtil::Replace("{url}/gameaide/api/v1/mails", "{url}", getBaseUrl().c_str());
		map<String, String>::type param = map<String, String>::type();
		param["userId"] = getUserId();
		param["gameType"] = getGameType();
		asyncGet(finalUrl, param, [=](String result, bool isSuccess) {
			ClientHttpResponse response = parseResponse(isSuccess, "loadMails", result);
			if (response.code == 1)
			{
				MailManager::parseMailData(response.data);
			}
		});
	}

	void ClientHttpRequest::loadUserInfo(i64 userId)
	{
		String finalUrl = StringUtil::Replace("{url}/friend/api/v1/friends/{userId}", "{url}", getBaseUrl().c_str());
		finalUrl = StringUtil::Replace(finalUrl, "{userId}", StringUtil::ToString(userId).c_str());
		asyncGet(finalUrl, map<String, String>::type(), [=](String result, bool isSuccess) {
			ClientHttpResponse response = parseResponse(isSuccess, "loadUserInfo", result);
			if (response.code == 1)
			{
				FriendManager::parseUserInfo(userId, response.data);
				loadUserDecoration(userId);
			}
		});
	}

	void BLOCKMAN::ClientHttpRequest::loadSeasonRule(int retry)
	{
		String finalUrl = StringUtil::Replace("{url}/gameaide/api/v1/segment/integral", "{url}", getBaseUrl().c_str());
		map<String, String>::type param = map<String, String>::type();
		param["userId"] = getUserId();
		param["gameId"] = getGameType();
		asyncGet(finalUrl, param, [=](String result, bool isSuccess) {
			ClientHttpResponse response = parseResponse(isSuccess, "loadSeasonRule", result);
			if (response.code == 1)
			{
				UpdateSeasonRuleEvent::emit(response.data);
			}
			else
			{
				if (retry > 0)
				{
					loadSeasonRule(retry - 1);
				}
			}
		});
	}

	void BLOCKMAN::ClientHttpRequest::loadSeasonRank(int type, int highRank, int page, int size, int retry)
	{
		String finalUrl = StringUtil::Replace("{url}/gameaide/api/v1/segment/integral/rank", "{url}", getBaseUrl().c_str());
		map<String, String>::type param = map<String, String>::type();
		param["userId"] = getUserId();
		param["gameId"] = getGameType();
		param["highRank"] = StringUtil::ToString(highRank);
		param["pageNo"] = StringUtil::ToString(page);
		param["pageSize"] = StringUtil::ToString(size);
		asyncGet(finalUrl, param, [=](String result, bool isSuccess) {
			ClientHttpResponse response = parseResponse(isSuccess, "loadSeasonRank", result);
			if (response.code == 1)
			{
				UpdateSeasonRankDataEvent::emit(type, response.data);
			}
			else
			{
				if (retry > 0)
				{
					loadSeasonRank(type, highRank, page, size, retry - 1);
				}
			}
		});
	}

	void ClientHttpRequest::loadUserDecoration(i64 userId)
	{
		String finalUrl = StringUtil::Replace("{url}/decoration/api/v1/decorations/{otherId}/using", "{url}", getBaseUrl().c_str());
		finalUrl = StringUtil::Replace(finalUrl, "{otherId}", StringUtil::ToString(userId).c_str());
		asyncGet(finalUrl, map<String, String>::type(), [=](String result, bool isSuccess) {
			ClientHttpResponse response = parseResponse(isSuccess, "loadUserDecoration", result);
			if (response.code == 1)
			{
				FriendManager::parseUserDress(userId, response.data);
			}
		});
	}

	String ClientHttpRequest::getBaseUrl()
	{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		return "http://120.92.158.119";
#else
		return GameClient::CGame::Instance()->getBaseUrl();
#endif
	}

	String ClientHttpRequest::getLanguage()
	{
		return GameClient::CGame::Instance()->getLanguage();
	}

	String ClientHttpRequest::getUserId()
	{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		return "1440";
#else
		i64 userId = GameClient::CGame::Instance()->getPlatformUserId();
		return StringUtil::ToString(userId);
#endif
	}

	String ClientHttpRequest::getUserToken()
	{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		return "eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiIxNDQwIiwiaWF0IjoxNTQ2ODY0MjYyLCJzdWIiOiIyMDE5LTAxLTA3IDIwOjMxOjAyIiwiaXNzIjoiU2FuZGJveC1TZWN1cml0eS1CYXNpYyIsImV4cCI6MTU0Nzc1MzI5NX0.P1mT7-wndNahq8S2zRwVGkEUAJMS1m9JYlw77sgfyAE";
#else
		return GameClient::CGame::Instance()->getUserToken();
#endif
	}

	String ClientHttpRequest::getGameType()
	{
		return GameClient::CGame::Instance()->getGameType();
	}

	cpr::Header ClientHttpRequest::getHttpHeader()
	{
		i64 userId = GameClient::CGame::Instance()->getPlatformUserId();
		return cpr::Header{
			{ "userId", getUserId().c_str() },
			{ "language", getLanguage().c_str() },
			{ "Access-Token", getUserToken().c_str() },
			{ "Content-Type", CONTENT_TYPE.c_str() },
		};
	}

}
