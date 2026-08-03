#include "ManorRequest.h"
#include "Util/sha1.h"
#include "cpr/cpr.h"
#include "Object/Root.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "Server.h"
#include "ServerNetwork.h"
#include "World/World.h"
#include "World/Shop.h"
#include "Script/GameServerEvents.h"
#include <sstream>
#include "Util/LanguageKey.h"
#include "World/CprCallBackWrap.h"

using namespace rapidjson;

namespace BLOCKMAN
{
	const String ManorRequest::CONTENT_TYPE = "application/json; charset=UTF-8";
	const String ManorRequest::WEB_HTTP_SECRET = "pq0194mxoqfh48L362G6R09T737E273X";

	const int32_t ManorRequest::HTTP_TIMEOUT = 6000;
	const bool ManorRequest::ENABLED_LOG = false;

	void ManorRequest::asyncGet(const char * url, map<String, String>::type params, std::function<void(String, bool)> fun)
	{
		LORD::ui32 timestamp = LORD::Root::Instance()->getCurrentTime();
		char ibuf[10];
		sprintf(ibuf, "%d", LORD::Root::Instance()->getCurrentTime());

		auto param = cpr::Parameters
		{
			{ "timestamp", StringUtil::ToString(timestamp).c_str() },
			{ "nonce", ibuf },
			{ "signature", parameterSignature(timestamp, ibuf).c_str() }
		};

		//std::stringstream ss;
		//ss << url << "?";
		for (auto pr : params)
		{
			cpr::Parameter cprp(pr.first.c_str(), pr.second.c_str());
			param.AddParameter(cprp);
			//ss << pr.first.c_str() << "=" << pr.second.c_str() << "&";
		}
		//LordLogInfo("http get request:%s", ss.str().c_str());

		String stringUrl(url);
		auto callback = [fun, stringUrl](cpr::Response response)
		{
			if (response.status_code != 200)
			{
				int code = response.status_code;
				auto foo = [fun, code, stringUrl]()
				{
					fun("{}", false);
					LordLogError("async get error(%s), http status code : %d", stringUrl.c_str(), code);
				};
				Server::Instance()->addAsyncCall(foo);
				return;
			}
			String result = String(response.text.c_str());
			auto foo = [result, fun]()
			{
				fun(result, true);
			};
			Server::Instance()->addAsyncCall(foo);
		};

		CprCallBackWrap::Instance()->GetCallback(callback,
			cpr::Timeout{ HTTP_TIMEOUT },
			cpr::Url{ url },
			param,
			cpr::Header{ { "Content-Type", CONTENT_TYPE.c_str() } }
		);
	}

	void ManorRequest::asyncPost(const char * url, map<String, String>::type  params, const char * body, std::function<void(String, bool)> fun)
	{
		LORD::ui32 timestamp = LORD::Root::Instance()->getCurrentTime();
		char ibuf[10];
		sprintf(ibuf, "%d", LORD::Root::Instance()->getCurrentTime());

		auto param = cpr::Parameters{
			{ "timestamp", StringUtil::ToString(timestamp).c_str() },
			{ "nonce", ibuf },
			{ "signature", parameterSignature(timestamp, ibuf).c_str() }
		};
		//std::stringstream ss;
		//ss << url << "?";
		for (auto pr : params)
		{
			cpr::Parameter cprp(pr.first.c_str(), pr.second.c_str());
			param.AddParameter(cprp);
			//ss << pr.first.c_str() << "=" << pr.second.c_str() << "&";
		}
		//LordLogInfo("http post request:%s", ss.str().c_str());

		String stringUrl(url);
		auto callback = [fun, stringUrl](cpr::Response response)
		{
			if (response.status_code != 200)
			{
				int code = response.status_code;
				auto foo = [fun, code, stringUrl]()
				{
					fun("{}", false);
					LordLogError("async post error(%s), http status code : %d", stringUrl.c_str(), code);
				};
				Server::Instance()->addAsyncCall(foo);
				return;
			}
			String result = String(response.text.c_str());
			auto foo = [result, fun]()
			{
				fun(result, true);
			};
			Server::Instance()->addAsyncCall(foo);
		};

		CprCallBackWrap::Instance()->PostCallback(callback,
			cpr::Timeout{ HTTP_TIMEOUT },
			cpr::Url{ url },
			param,
			cpr::Body{ body },
			cpr::Header{ { "Content-Type", CONTENT_TYPE.c_str() } }
		);
	}

	HttpResponse ManorRequest::parseResponse(bool isSuccess, String logTag, String response)
	{
		HttpResponse result = HttpResponse();
		Document doc = Document();
		doc.Parse(response.c_str());
		String log = "";
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
				LordLogInfo("manor request: %s parse response succeed, result: %s", logTag.c_str(), result.data.length() < 800 ? result.data.c_str() : "result.length > 1000");
			}
		}
		else
		{
			LordLogError("manor request: %s parse response error, response: %s", logTag.c_str(), response.c_str());
		}
		return result;
	}

	void ManorRequest::pay(String webPayUrl, String dataServerPayUrl, i32 currencyType, i32 uniqueId, i32 price, ui64 platformUserId, std::function<void(String, bool)> fun)
	{
		if (currencyType <= 2)
		{
			String finalUrl = StringUtil::Replace("{url}/pay/api/v1/pay/users/purchase/game/props", "{url}", webPayUrl.c_str());
			Document  doc = Document();
			doc.SetObject();
			Document::AllocatorType &allocator = doc.GetAllocator();
			Value userId(kNumberType);
			userId.SetInt64(platformUserId);
			doc.AddMember("userId", userId, allocator);
			doc.AddMember("currency", currencyType, allocator);
			doc.AddMember("gameId", StringRef(Server::Instance()->getGameType().c_str()), allocator);
			doc.AddMember("propsId", uniqueId, allocator);
			doc.AddMember("quantity", price, allocator);
			StringBuffer buffer;
			Writer<StringBuffer> writer(buffer);
			doc.Accept(writer);
			const char* json = buffer.GetString();
			map<String, String>::type params;
			asyncPost(finalUrl.c_str(), params, json, fun);
		}
		else
		{
			trade(dataServerPayUrl, platformUserId, -price, fun);
		}
	}

	bool ManorRequest::manorPayResult(ui64 userId, String result, bool isSuccess)
	{
		auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(userId);
		if (!peer || !peer->getEntityPlayer())
		{
			return false;
		}
		bool isBuySuccess = false;
		String logTag = StringUtil::Format("manorPayResult(UserId=%lld)", userId);
		HttpResponse  response = ManorRequest::parseResponse(isSuccess, logTag, result);
		if (response.code == 1)
		{
			Document doc = Document();
			doc.Parse(response.data.c_str());
			if (doc.HasMember("amount"))
			{
				auto player = peer->getEntityPlayer();
				player->setCurrency(doc.FindMember("amount")->value.GetInt64());
			}
			isBuySuccess = true;
		}
		return isBuySuccess;
	}

	void ManorRequest::trade(String url, ui64 platformUserId, i32 price, std::function<void(String, bool)> fun)
	{
		String finalUrl = StringUtil::Replace("{url}/charmingtown/api/v1/inner/user/money/trade", "{url}", url.c_str());
		Document doc = Document();
		doc.SetObject();
		Document::AllocatorType &allocator = doc.GetAllocator();
		Value userId(kNumberType);
		userId.SetInt64(platformUserId);
		doc.AddMember("userId", userId, allocator);
		doc.AddMember("amount", price, allocator);
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		doc.Accept(writer);
		const char* json = buffer.GetString();
		map<String, String>::type params;
		asyncPost(finalUrl.c_str(), params, json, fun);
	}

	void ManorRequest::trade(ui64 platformUserId, i32 price)
	{
		if (Server::Instance()->getGameType() != "g1020")
		{
			return;
		}

		String url = Server::Instance()->getDataServerUrl();
		if (url.length() == 0) { LordLogError("data server url  is null"); return; }
		String finalUrl = StringUtil::Replace("{url}/charmingtown/api/v1/inner/user/money/trade", "{url}", url.c_str());
		Document doc = Document();
		doc.SetObject();
		Document::AllocatorType &allocator = doc.GetAllocator();
		Value userId(kNumberType);
		userId.SetInt64(platformUserId);
		doc.AddMember("userId", userId, allocator);
		doc.AddMember("amount", price, allocator);
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		doc.Accept(writer);
		const char* json = buffer.GetString();
		map<String, String>::type params;
		asyncPost(finalUrl.c_str(), params, json, [platformUserId](String r, bool s) {
			manorPayResult(platformUserId, r, s);
		});

	}

	void ManorRequest::loadGameMoney(ui64 userId)
	{
		String url = Server::Instance()->getDataServerUrl();
		if (url.length() == 0)
		{
			LordLogError("data server url  is null"); return;
		}
		String finalUrl = StringUtil::Replace("{url}/charmingtown/api/v1/inner/user/money/{userId}", "{url}", url.c_str());
		finalUrl = StringUtil::Replace(finalUrl, "{userId}", StringUtil::ToString((long long)userId).c_str());
		map<String, String>::type params;
		asyncGet(finalUrl.c_str(), params, [userId](String r, bool s) {
			manorPayResult(userId, r, s);
		});
	}

	void ManorRequest::loadManorConfig()
	{
		String url = Server::Instance()->getDataServerUrl();
		if (url.length() == 0)
		{
			LordLogError("data server url  is null"); return;
		}
		String finalUrl = StringUtil::Replace("{url}/charmingtown/api/v1/inner/manor/level/config", "{url}", url.c_str());
		map<String, String>::type params;
		asyncGet(finalUrl.c_str(), params, [](String result, bool isSuccess) {
			HttpResponse  response = ManorRequest::parseResponse(isSuccess, "loadManorConfig", result);
			SCRIPT_EVENT::LoadManorConfigEvent::invoke(response.data);
		});
	}

	void ManorRequest::receiveManor(ui64 userId, String userName)
	{
		String url = Server::Instance()->getDataServerUrl();
		if (url.length() == 0)
		{
			LordLogError("receive manor url is null");
			auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(userId);
			if (peer)
			{
				ServerNetwork::Instance()->getSender()->sendRequestResult(peer->getRakssid(), 0, ResultCode::COMMON, LanguageKey::GUI_MANOR_RECEIVE_FAILURE);
			}
			return;
		}
		String finalUrl = StringUtil::Replace("{url}/charmingtown/api/v1/inner/manor/receive", "{url}", url.c_str());
		Document  doc = Document();
		doc.SetObject();
		Document::AllocatorType &allocator = doc.GetAllocator();
		Value uid(kNumberType);
		uid.SetInt64(userId);
		doc.AddMember("userId", uid, allocator);
		doc.AddMember("name", StringRef(userName.c_str()), allocator);
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		doc.Accept(writer);
		const char* json = buffer.GetString();
		map<String, String>::type params;
		asyncPost(finalUrl.c_str(), params, json, [userId](String result, bool isSuccess) {
			String logTag = StringUtil::Format("receiveManor(UserId=%lld)", userId);
			HttpResponse  response = parseResponse(isSuccess, logTag, result);
			auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(userId);
			if (peer)
			{
				if (response.code == 1)
				{
					SCRIPT_EVENT::PlayerGetManorEvent::invoke(peer->getRakssid(), response.data);
				}
				else
				{
					ServerNetwork::Instance()->getSender()->sendRequestResult(peer->getRakssid(), 0, ResultCode::COMMON, LanguageKey::GUI_MANOR_RECEIVE_FAILURE);
				}
			}
		});
	}

	void ManorRequest::upgradeManor(String url, ui64 userId, i32 price, std::function<void(String, bool)> fun)
	{
		String finalUrl = StringUtil::Replace("{url}/charmingtown/api/v1/inner/manor/upgrade/{userId}", "{url}", url.c_str());
		finalUrl = StringUtil::Replace(finalUrl, "{userId}", StringUtil::ToString((long long)userId).c_str());
		Document  doc = Document();
		doc.SetObject();
		Document::AllocatorType &allocator = doc.GetAllocator();
		Value uid(kNumberType);
		uid.SetInt64(userId);
		doc.AddMember("userId", uid, allocator);
		doc.AddMember("price", price, allocator);
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		doc.Accept(writer);
		const char* json = buffer.GetString();
		map<String, String>::type params;
		asyncPost(finalUrl.c_str(), params, json, fun);

	}

	void ManorRequest::praiseManor(ui64 userId, ui64 targetUserId)
	{
		String url = Server::Instance()->getDataServerUrl();
		if (url.length() == 0)
		{
			LordLogError("data server url  is null");
			return;
		}
		auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(userId);
		String nickName = "";
		if (peer)
		{
			nickName = peer->getName();
		}

		String finalUrl = StringUtil::Replace("{url}/charmingtown/api/v1/inner/manor/praise", "{url}", url.c_str());
		Document  doc = Document();
		doc.SetObject();
		Document::AllocatorType &allocator = doc.GetAllocator();
		Value uid(kNumberType);
		uid.SetInt64(userId);
		Value tuid(kNumberType);
		tuid.SetInt64(targetUserId);
		doc.AddMember("userId", uid, allocator);
		doc.AddMember("targetUserId", tuid, allocator);
		doc.AddMember("nickName", StringRef(nickName.c_str()), allocator);
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		doc.Accept(writer);
		const char* json = buffer.GetString();
		map<String, String>::type params;
		asyncPost(finalUrl.c_str(), params, json, [userId](String result, bool isSuccess) {
			parsePraiseResponse(isSuccess, userId, result);
		});

	}

	void ManorRequest::callOnManor(String url, ui64 userId, ui64 targetUserId, std::function<void(String, bool)> fun)
	{
		auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(userId);
		String nickName = "";
		if (peer)
		{
			nickName = peer->getName();
		}

		String finalUrl = StringUtil::Replace("{url}/charmingtown/api/v1/inner/manor/visit", "{url}", url.c_str());
		Document  doc = Document();
		doc.SetObject();
		Document::AllocatorType &allocator = doc.GetAllocator();
		Value uid(kNumberType);
		uid.SetInt64(userId);
		Value tuid(kNumberType);
		tuid.SetInt64(targetUserId);
		doc.AddMember("userId", uid, allocator);
		doc.AddMember("targetUserId", tuid, allocator);
		doc.AddMember("nickName", StringRef(nickName.c_str()), allocator);
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		doc.Accept(writer);
		const char* json = buffer.GetString();
		map<String, String>::type params;
		asyncPost(finalUrl.c_str(), params, json, fun);

	}

	void ManorRequest::loadManorCharm(String url, ui64 userId, std::function<void(String, bool)> fun)
	{
		String finalUrl = StringUtil::Replace("{url}/charmingtown/api/v1/inner/manor/charm", "{url}", url.c_str());
		map<String, String>::type params;
		asyncGet(finalUrl.c_str(), params, fun);
	}

	void ManorRequest::loadManorValue(String url, ui64 userId, std::function<void(String, bool)> fun)
	{
		String finalUrl = StringUtil::Replace("{url}/charmingtown/api/v1/inner/manor/cost/{userId}", "{url}", url.c_str());
		finalUrl = StringUtil::Replace(finalUrl, "{userId}", StringUtil::ToString((long long)userId).c_str());
		map<String, String>::type params;
		asyncGet(finalUrl.c_str(), params, fun);
	}

	void ManorRequest::loadManorInfo(ui64 userId, bool isMaster)
	{
		String url = Server::Instance()->getDataServerUrl();
		if (url.length() == 0)
		{
			LordLogError("data server url  is null"); return;
		}
		String finalUrl = StringUtil::Replace("{url}/charmingtown/api/v1/inner/manor/find/{userId}", "{url}", url.c_str());
		finalUrl = StringUtil::Replace(finalUrl, "{userId}", StringUtil::ToString((long long)userId).c_str());
		map<String, String>::type params;
		asyncGet(finalUrl.c_str(), params, [userId, isMaster](String result, bool isSuccess) {
			String logTag = StringUtil::Format("loadManorInfo(UserId=%lld)", userId);
			HttpResponse  response = ManorRequest::parseResponse(isSuccess, logTag, result);
			if (isMaster)
			{
				SCRIPT_EVENT::PlayerLoadManorMasterInfoEvent::invoke(userId, response.data);
			}
			else
			{
				SCRIPT_EVENT::PlayerLoadManorInfoEvent::invoke(userId, response.data);
			}

		});
	}

	void ManorRequest::checkHasManor(ui64 userId, ui64 targetUserId)
	{
		String url = Server::Instance()->getDataServerUrl();
		if (url.length() == 0)
		{
			LordLogError("data server url  is null"); return;
		}
		String finalUrl = StringUtil::Replace("{url}/charmingtown/api/v1/inner/manor/receive/{userId}", "{url}", url.c_str());
		finalUrl = StringUtil::Replace(finalUrl, "{userId}", StringUtil::ToString((long long)targetUserId).c_str());
		map<String, String>::type params;
		asyncGet(finalUrl.c_str(), params, [userId, targetUserId](String result, bool isSuccess) {
			String logTag = StringUtil::Format("checkHasManor(UserId=%lld)", userId);
			HttpResponse  response = ManorRequest::parseResponse(isSuccess, logTag, result);
			auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(userId);
			if (!peer)
			{
				LordLogInfo("ManorRequest:checkHasManor peer is null");
				return;
			}
			if (response.code == 1)
			{
				ServerNetwork::Instance()->getSender()->sendCallOnManorResetClient(peer->getRakssid(), targetUserId);
			}
			else
			{
				ServerNetwork::Instance()->getSender()->sendRequestResult(peer->getRakssid(), 0, ResultCode::COMMON, LanguageKey::GUI_MANOR_TARGET_USER_NOT_MANOR);
			}
		});
	}

	void ManorRequest::sellManor(ui64 userId)
	{
		String url = Server::Instance()->getDataServerUrl();
		if (url.length() == 0)
		{
			LordLogError("data server url  is null");
			return;
		}
		String finalUrl = StringUtil::Replace("{url}/charmingtown/api/v1/inner/manor/sale/{userId}", "{url}", url.c_str());
		finalUrl = StringUtil::Replace(finalUrl, "{userId}", StringUtil::ToString((long long)userId).c_str());
		map<String, String>::type params;
		asyncGet(finalUrl.c_str(), params, [userId](String result, bool isSuccess) {
			manorPayResult(userId, result, isSuccess);
			String logTag = StringUtil::Format("sellManor(UserId=%lld)", userId);
			HttpResponse response = parseResponse(isSuccess, logTag, result);
			auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(userId);
			if (!peer || !peer->getEntityPlayer()) {
				if (response.code == 1) {
					SCRIPT_EVENT::PlayerSellManorEvent::invoke(userId, isSuccess, response.data);
					Server::Instance()->getRoomManager()->setUserAttrInfoManorId(userId, 0);
					Server::Instance()->getRoomManager()->getRoomClient()->sendUserSellManor(userId);
				}
				return;
			}

			if (response.code == 1)
			{
				SCRIPT_EVENT::PlayerSellManorEvent::invoke(userId, isSuccess, response.data);
				Server::Instance()->getRoomManager()->setUserAttrInfoManorId(userId, 0);
				Server::Instance()->getRoomManager()->getRoomClient()->sendUserSellManor(userId);
				ServerNetwork::Instance()->getSender()->sendRequestResult(peer->getRakssid(), 0, ResultCode::SELL_MANOR_SUCCEED, LanguageKey::GUI_MANOR_SELL_SUCCEED);
				peer->getEntityPlayer()->m_manor->dealloc();
			}
			else if (response.code == 208)
			{
				ServerNetwork::Instance()->getSender()->sendRequestResult(peer->getRakssid(), 0, ResultCode::COMMON, LanguageKey::GUI_MANOR_SELL_FAILURE_BY_TIMES);
			}
			else
			{
				ServerNetwork::Instance()->getSender()->sendRequestResult(peer->getRakssid(), 0, ResultCode::COMMON, LanguageKey::GUI_MANOR_SELL_FAILURE);
			}
		});
	}

	void ManorRequest::buyFurniture(String url, String manorId, i32 furnitureId, i32 price, i32 charm, std::function<void(String, bool)> fun)
	{
		String finalUrl = StringUtil::Replace("{url}/charmingtown/api/v1/inner/furniture/buy", "{url}", url.c_str());
		ui32 time = LORD::Root::Instance()->getCurrentTime();
		Document  doc = Document();
		doc.SetObject();
		Document::AllocatorType &allocator = doc.GetAllocator();
		doc.AddMember("manorId", StringRef(manorId.c_str()), allocator);
		doc.AddMember("tempId", furnitureId, allocator);
		doc.AddMember("charm", charm, allocator);
		doc.AddMember("price", price, allocator);
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		doc.Accept(writer);
		const char* json = buffer.GetString();
		map<String, String>::type params;
		asyncPost(finalUrl.c_str(), params, json, fun);

	}

	void ManorRequest::operationFurniture(ui64 userId, String manorId, i32 furnitureId, Vector3 pos, float yaw, i32 state)
	{
		String url = Server::Instance()->getDataServerUrl();
		if (url.length() == 0)
		{
			LordLogError("data server url  is null"); return;
		}
		String finalUrl = StringUtil::Replace("{url}/charmingtown/api/v1/inner/furniture/put", "{url}", url.c_str());
		Document  doc = Document();
		doc.SetObject();
		Document::AllocatorType &allocator = doc.GetAllocator();
		doc.AddMember("manorId", StringRef(manorId.c_str()), allocator);
		doc.AddMember("tempId", furnitureId, allocator);
		doc.AddMember("offset", yaw, allocator);
		doc.AddMember("positionX", pos.x, allocator);
		doc.AddMember("positionY", pos.y, allocator);
		doc.AddMember("positionZ", pos.z, allocator);
		doc.AddMember("state", state, allocator);
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		doc.Accept(writer);
		const char* json = buffer.GetString();
		map<String, String>::type params;
		asyncPost(finalUrl.c_str(), params, json, [userId](String result, bool isSuccess) {
			String logTag = StringUtil::Format("operationFurniture(UserId=%lld)", userId);
			HttpResponse  response = ManorRequest::parseResponse(isSuccess, logTag, result);
			//TODO
		});

	}

	void ManorRequest::loadFurnitures(ui64 userId, String manorId)
	{
		String url = Server::Instance()->getDataServerUrl();
		if (url.length() == 0)
		{
			LordLogError("data server url  is null");
			return;
		}
		String finalUrl = StringUtil::Replace("{url}/charmingtown/api/v1/inner/furniture/{manorId}", "{url}", url.c_str());
		finalUrl = StringUtil::Replace(finalUrl, "{manorId}", manorId.c_str());
		map<String, String>::type params;
		asyncGet(finalUrl.c_str(), params, [userId](String result, bool isSuccess) {
			String logTag = StringUtil::Format("loadFurnitures(UserId=%lld)", userId);
			HttpResponse  response = ManorRequest::parseResponse(isSuccess, logTag, result);
			//TODO
		});
	}

	void ManorRequest::buyHouse(String url, String manorId, i32 houseId, i32 price, i32 charm, std::function<void(String, bool)> fun)
	{
		String finalUrl = StringUtil::Replace("{url}/charmingtown/api/v1/inner/house/unlock", "{url}", url.c_str());
		ui32 time = LORD::Root::Instance()->getCurrentTime();
		Document  doc = Document();
		doc.SetObject();
		Document::AllocatorType &allocator = doc.GetAllocator();
		doc.AddMember("manorId", StringRef(manorId.c_str()), allocator);
		doc.AddMember("tempId", houseId, allocator);
		doc.AddMember("charm", charm, allocator);
		doc.AddMember("price", price, allocator);
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		doc.Accept(writer);
		const char* json = buffer.GetString();
		map<String, String>::type params;
		asyncPost(finalUrl.c_str(), params, json, fun);

	}

	void ManorRequest::buildHouse(ui64 userId, String manorId, i32 houseId)
	{
		String url = Server::Instance()->getDataServerUrl();
		if (url.length() == 0)
		{
			LordLogError("data server url  is null"); return;
		}
		String finalUrl = StringUtil::Replace("{url}/charmingtown/api/v1/inner/house/build", "{url}", url.c_str());
		char ibuf[10];
		sprintf(ibuf, "%d", LORD::Root::Instance()->getCurrentTime());
		Document  doc = Document();
		doc.SetObject();
		Document::AllocatorType &allocator = doc.GetAllocator();
		doc.AddMember("manorId", StringRef(manorId.c_str()), allocator);
		doc.AddMember("tempId", houseId, allocator);
		doc.AddMember("state", 1, allocator);
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		doc.Accept(writer);
		const char* json = buffer.GetString();
		map<String, String>::type params;
		asyncPost(finalUrl.c_str(), params, json, [userId](String result, bool isSuccess) {
			String logTag = StringUtil::Format("buildHouse(UserId=%lld)", userId);
			HttpResponse  response = ManorRequest::parseResponse(isSuccess, logTag, result);
			//TODO
		});

	}

	void ManorRequest::loadHouses(ui64 userId, String manorId)
	{
		String url = Server::Instance()->getDataServerUrl();
		if (url.length() == 0)
		{
			LordLogError("data server url  is null"); return;
		}
		String finalUrl = StringUtil::Replace("{url}/charmingtown/api/v1/inner/house/{manorId}", "{url}", url.c_str());
		finalUrl = StringUtil::Replace(finalUrl, "{manorId}", manorId.c_str());
		map<String, String>::type params;
		asyncGet(finalUrl.c_str(), params, [userId](String result, bool isSuccess) {
			String logTag = StringUtil::Format("loadHouses(UserId=%lld)", userId);
			HttpResponse  response = ManorRequest::parseResponse(isSuccess, logTag, result);
			//TODO
		});
	}

	void ManorRequest::ploughCrop(String manorId, i32 row, i32 colum, String blockId, i32 fruitNum, i32 curState, i32 pluckingState, ui64 curStateTime, ui64 lastUpdateTime)
	{
		String url = Server::Instance()->getDataServerUrl();
		if (url.length() == 0)
		{
			LordLogError("data server url  is null");
			return;
		}
		String finalUrl = StringUtil::Replace("{url}/charmingtown/api/v1/inner/plough/crop", "{url}", url.c_str());
		char ibuf[10];
		sprintf(ibuf, "%d", LORD::Root::Instance()->getCurrentTime());
		Document  doc = Document();
		doc.SetObject();
		Document::AllocatorType &allocator = doc.GetAllocator();
		doc.AddMember("manorId", StringRef(manorId.c_str()), allocator);
		doc.AddMember("ploughColumn", colum, allocator);
		doc.AddMember("ploughRow", row, allocator);
		doc.AddMember("blockId", StringRef(blockId.c_str()), allocator);
		doc.AddMember("curState", curState, allocator);
		doc.AddMember("curStateTime", curStateTime, allocator);
		doc.AddMember("lastUpdateTime", lastUpdateTime, allocator);
		doc.AddMember("pluckingTime", 0, allocator);
		doc.AddMember("pluckingState", pluckingState, allocator);
		doc.AddMember("fruitNum", fruitNum, allocator);
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		doc.Accept(writer);
		const char* json = buffer.GetString();
		map<String, String>::type params;
		asyncPost(finalUrl.c_str(), params, json, [manorId](String result, bool isSuccess) {
			String logTag = StringUtil::Format("ploughCrop(manorId=%s)", manorId.c_str());
			HttpResponse  response = ManorRequest::parseResponse(isSuccess, logTag, result);
			//TODO
		});

	}

	void ManorRequest::ploughPlucking(String manorId, ui64 userId, i32 row, i32 colum, i32 num, i32 productId, ui64 pluckingTime)
	{

		auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(userId);
		String nickName = "";
		if (peer)
		{
			nickName = peer->getName();
		}

		String url = Server::Instance()->getDataServerUrl();
		if (url.length() == 0)
		{
			LordLogError("data server url  is null");
			return;
		}

		String finalUrl = StringUtil::Replace("{url}/charmingtown/api/v1/inner/plough/plucking", "{url}", url.c_str());
		char ibuf[10];
		sprintf(ibuf, "%d", LORD::Root::Instance()->getCurrentTime());
		Document  doc = Document();
		doc.SetObject();
		Document::AllocatorType &allocator = doc.GetAllocator();
		doc.AddMember("manorId", StringRef(manorId.c_str()), allocator);
		Value uid(kNumberType);
		uid.SetInt64(userId);
		doc.AddMember("userId", uid, allocator);
		doc.AddMember("nickName", StringRef(nickName.c_str()), allocator);
		doc.AddMember("ploughColumn", colum, allocator);
		doc.AddMember("ploughRow", row, allocator);
		doc.AddMember("pluckingNum", num, allocator);
		doc.AddMember("tempId", productId, allocator);
		doc.AddMember("pluckingTime", pluckingTime, allocator);
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		doc.Accept(writer);
		const char* json = buffer.GetString();
		map<String, String>::type params;
		asyncPost(finalUrl.c_str(), params, json, [userId, manorId](String result, bool isSuccess) {
			String logTag = StringUtil::Format("ploughPlucking(UserId=%lld, manorId = %s)", userId, manorId.c_str());
			HttpResponse  response = ManorRequest::parseResponse(isSuccess, logTag, result);
			//TODO
		});

	}

	void ManorRequest::loadPloughInfo(ui64 userId, String manorId)
	{
		String url = Server::Instance()->getDataServerUrl();
		if (url.length() == 0)
		{
			LordLogError("data server url  is null"); return;
		}
		String finalUrl = StringUtil::Replace("{url}/charmingtown/api/v1/inner/plough/{manorId}", "{url}", url.c_str());
		finalUrl = StringUtil::Replace(finalUrl, "{manorId}", manorId.c_str());
		map<String, String>::type params;
		asyncGet(finalUrl.c_str(), params, [userId](String result, bool isSuccess) {
			String logTag = StringUtil::Format("loadPloughInfo(UserId=%lld)", userId);
			HttpResponse  response = ManorRequest::parseResponse(isSuccess, logTag, result);
			//TODO
		});
	}

	void ManorRequest::saveManorInventory(String manorId, const char * inventoryInfo)
	{
		String url = Server::Instance()->getDataServerUrl();
		if (url.length() == 0)
		{
			LordLogError("data server url  is null"); return;
		}
		String finalUrl = StringUtil::Replace("{url}/charmingtown/api/v1/inner/manor/backpack/{manorId}", "{url}", url.c_str());
		finalUrl = StringUtil::Replace(finalUrl, "{manorId}", manorId.c_str());
		map<String, String>::type params;
		asyncPost(finalUrl.c_str(), params, inventoryInfo, [manorId](String result, bool isSuccess) {
			String logTag = StringUtil::Format("saveManorInventory(manorId=%s)", manorId.c_str());
			HttpResponse  response = ManorRequest::parseResponse(isSuccess, logTag, result);
		});
	}

	void ManorRequest::loadManorInventory(ui64 userId, String manorId)
	{
		String url = Server::Instance()->getDataServerUrl();
		if (url.length() == 0)
		{
			LordLogError("data server url  is null");
			return;
		}
		String finalUrl = StringUtil::Replace("{url}/charmingtown/api/v1/inner/manor/backpack/{manorId}", "{url}", url.c_str());
		finalUrl = StringUtil::Replace(finalUrl, "{manorId}", manorId.c_str());
		map<String, String>::type params;
		asyncGet(finalUrl.c_str(), params, [userId, manorId](String result, bool isSuccess) {
			String logTag = StringUtil::Format("loadManorInventory(UserId=%lld)", userId);
			HttpResponse  response = ManorRequest::parseResponse(isSuccess, logTag, result);
			if (response.code == 1)
			{
				SCRIPT_EVENT::LoadManorInventoryEvent::invoke(userId, manorId, response.data);
			}
		});
	}

	void ManorRequest::saveManorBlock(String manorId, const char * blockInfo)
	{
		String url = Server::Instance()->getDataServerUrl();
		if (url.length() == 0)
		{
			LordLogError("data server url  is null");
			return;
		}
		String finalUrl = StringUtil::Replace("{url}/charmingtown/api/v1/inner/block/{manorId}", "{url}", url.c_str());
		finalUrl = StringUtil::Replace(finalUrl, "{manorId}", manorId.c_str());
		map<String, String>::type params;
		asyncPost(finalUrl.c_str(), params, blockInfo, [manorId](String result, bool isSuccess) {
			String logTag = StringUtil::Format("saveManorBlock(manorId=%s)", manorId.c_str());
			HttpResponse  response = ManorRequest::parseResponse(isSuccess, logTag, result);
		});
	}

	void ManorRequest::loadManorBlock(ui64 userId, String manorId)
	{
		String url = Server::Instance()->getDataServerUrl();
		if (url.length() == 0)
		{
			LordLogError("data server url  is null");
			return;
		}
		String finalUrl = StringUtil::Replace("{url}/charmingtown/api/v1/inner/block/{manorId}", "{url}", url.c_str());
		finalUrl = StringUtil::Replace(finalUrl, "{manorId}", manorId.c_str());
		map<String, String>::type params;
		asyncGet(finalUrl.c_str(), params, [userId, manorId](String result, bool isSuccess) {
			String logTag = StringUtil::Format("loadManorBlock(UserId=%lld)", userId);
			HttpResponse  response = ManorRequest::parseResponse(isSuccess, logTag, result);
			if (response.code == 1)
			{
				SCRIPT_EVENT::LoadManorBlockEvent::invoke(userId, manorId, response.data);
			}
		});
	}

	void ManorRequest::loadManorMessage(ui64 userId, i32 page)
	{
		String url = Server::Instance()->getDataServerUrl();
		if (url.length() == 0)
		{
			LordLogError("data server url  is null");
			return;
		}
		String finalUrl = StringUtil::Replace("{url}/charmingtown/api/v1/inner/message/{userId}", "{url}", url.c_str());
		finalUrl = StringUtil::Replace(finalUrl, "{userId}", StringUtil::ToString((long long)userId).c_str());
		map<String, String>::type params;
		params["page"] = StringUtil::ToString(page);
		params["size"] = "20";
		asyncGet(finalUrl.c_str(), params, [userId](String result, bool isSuccess) {
			String logTag = StringUtil::Format("loadManorMessage(UserId=%lld)", userId);
			HttpResponse  response = ManorRequest::parseResponse(isSuccess, logTag, result);
			if (response.code == 1)
			{
				parseMessageResponse(userId, response.data);
			}
			else
			{
				LordLogError("Manor request load userId(%lld) manor message error", userId);
			}
		});
	}

	void ManorRequest::removeManorMessageById(ui64 userId, String id)
	{
		String url = Server::Instance()->getDataServerUrl();
		if (url.length() == 0)
		{
			LordLogError("data server url  is null");
			return;
		}
		String finalUrl = StringUtil::Replace("{url}/charmingtown/api/v1/inner/message/remove/{id}", "{url}", url.c_str());
		finalUrl = StringUtil::Replace(finalUrl, "{id}", id.c_str());
		map<String, String>::type params;
		asyncPost(finalUrl.c_str(), params, "", [userId, id](String result, bool isSuccess) {
			String logTag = StringUtil::Format("removeManorMessageById(UserId=%lld)", userId);
			HttpResponse  response = ManorRequest::parseResponse(isSuccess, logTag, result);
			if (response.code == 1)
			{
				auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(userId);
				if (peer && peer->getEntityPlayer() && peer->getEntityPlayer()->m_manor)
				{
					peer->getEntityPlayer()->m_manor->removeMessage(peer->getEntityPlayer()->m_manor->findMessageById(id));
				}
			}
			else
			{
				LordLogError("Manor request remove userId(%lld) manor message error", userId);
			}
		});
	}

	void ManorRequest::loadManorCharmRank(i32 size)
	{
		String url = Server::Instance()->getDataServerUrl();
		if (url.length() == 0)
		{
			LordLogError("data server url  is null");
			return;
		}
		String finalUrl = StringUtil::Replace("{url}/charmingtown/api/v1/inner/manor/charm", "{url}", url.c_str());
		map<String, String>::type params;
		params["page"] = "1";
		params["size"] = StringUtil::ToString(size);
		asyncGet(finalUrl.c_str(), params, [](String result, bool isSuccess) {
			HttpResponse  response = ManorRequest::parseResponse(isSuccess, "loadManorCharmRank", result);
			String data = response.data;
			if (response.code == 1)
			{
				data = parseRankResponse(response.data);
			}
			SCRIPT_EVENT::LoadManorCharmRankEvent::invoke(response.code == 1, data);
		});
	}

	void ManorRequest::loadManorPotentialRank(i32 size)
	{
		String url = Server::Instance()->getDataServerUrl();
		if (url.length() == 0)
		{
			LordLogError("data server url  is null");
			return;
		}
		String finalUrl = StringUtil::Replace("{url}/charmingtown/api/v1/inner/manor/potential", "{url}", url.c_str());
		map<String, String>::type params;
		params["page"] = "1";
		params["size"] = StringUtil::ToString(size);
		asyncGet(finalUrl.c_str(), params, [](String result, bool isSuccess) {
			HttpResponse  response = ManorRequest::parseResponse(isSuccess, "loadManorPotentialRank", result);
			String data = response.data;
			if (response.code == 1)
			{
				data = parseRankResponse(response.data);
			}
			SCRIPT_EVENT::LoadManorPotentialRankEvent::invoke(response.code == 1, data);
		});
	}

	void ManorRequest::loadMyManorRank(ui64 userId)
	{
		String url = Server::Instance()->getDataServerUrl();
		if (url.length() == 0)
		{
			LordLogError("data server url  is null");
			return;
		}
		String finalUrl = StringUtil::Replace("{url}/charmingtown/api/v1/inner/manor/ranking/{userId}", "{url}", url.c_str());
		finalUrl = StringUtil::Replace(finalUrl, "{userId}", StringUtil::ToString((long long)userId).c_str());
		map<String, String>::type params;
		asyncGet(finalUrl.c_str(), params, [userId](String result, bool isSuccess) {
			String logTag = StringUtil::Format("loadMyManorRank(UserId=%lld)", userId);
			HttpResponse  response = ManorRequest::parseResponse(isSuccess, logTag, result);
			SCRIPT_EVENT::LoadMyManorRankEvent::invoke(response.code == 1, userId, response.data);
		});
	}

	void ManorRequest::parsePraiseResponse(bool isSuccess, ui64 userId, String response)
	{
		auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(userId);
		if (!peer || !peer->getEntityPlayer())
		{
			LordLogError("manor request parse praise response: clientpeer[%lld] is null", userId);
			return;
		}
		String langKey = LanguageKey::GUI_MANOR_GIVE_ROSE_FAILURE;
		Document  doc = Document();
		doc.Parse(response.c_str());
		if (isSuccess && !doc.HasParseError() && doc.HasMember("code"))
		{
			int code = doc.FindMember("code")->value.GetInt();
			if (code == 1)
			{
				if (doc.HasMember("data"))
				{
					Value data = doc.FindMember("data")->value.GetObject();
					if (data.HasMember("money"))
					{
						peer->getEntityPlayer()->addCurrency(data.FindMember("money")->value.GetInt());
					}
					if (data.HasMember("praiseNum"))
					{
						SCRIPT_EVENT::PlayerPraiseResultEvent::invoke(userId, data.FindMember("praiseNum")->value.GetInt());
					}
				}
				langKey = LanguageKey::GUI_MANOR_GIVE_ROSE_SUCCEED;
			}
			else if (code == 204)
			{
				langKey = LanguageKey::GUI_MANOR_GIVE_ROSE_FAILURE_NUM_IS_ZERO;
			}
			else if (code == 208)
			{
				langKey = LanguageKey::GUI_MANOR_GIVE_ROSE_FAILURE_GIVEN;
			}
			else if (code == 404)
			{
				langKey = LanguageKey::GUI_MANOR_GIVE_ROSE_FAILURE_NOT_MANOR;
			}
		}
		ServerNetwork::Instance()->getSender()->sendRequestResult(peer->getRakssid(), 0, ResultCode::COMMON, langKey);
	}

	void ManorRequest::parseMessageResponse(ui64 userId, String response)
	{
		auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(userId);
		if (!peer || !peer->getEntityPlayer() || !peer->getEntityPlayer()->m_manor)
		{
			LordLogError("manor request parse message response: clientpeer[%lld] is null", userId);
			return;
		}
		Document  doc = Document();
		doc.Parse(response.c_str());
		if (!doc.HasParseError() && doc.HasMember("content"))
		{
			auto content = doc.FindMember("content")->value.GetArray();

			if (doc.HasMember("number") && doc.FindMember("number")->value.GetInt() == 0)
			{
				peer->getEntityPlayer()->m_manor->clearMessage();
			}

			for (const auto& item : content)
			{
				ManorMessage message = ManorMessage();
				if (item.HasMember("id"))
				{
					message.id = item.FindMember("id")->value.GetString();
				}

				if (item.HasMember("userId"))
				{
					message.userId = item.FindMember("userId")->value.GetInt64();
				}

				if (item.HasMember("category"))
				{
					message.messageType = item.FindMember("category")->value.GetInt();
				}

				if (item.HasMember("amount"))
				{
					message.num = item.FindMember("amount")->value.GetInt();
				}

				if (item.HasMember("tempId"))
				{
					message.productId = atoi(item.FindMember("tempId")->value.GetString());
				}

				if (item.HasMember("nickName"))
				{
					message.userName = item.FindMember("nickName")->value.GetString();
				}

				peer->getEntityPlayer()->m_manor->addMessage(&message);
			}

			ServerNetwork::Instance()->getSender()->sendUpdateManor(peer->getEntityPlayer());
		}
	}

	String ManorRequest::parseRankResponse(String response)
	{
		Document  doc = Document();
		doc.Parse(response.c_str());
		if (!doc.HasParseError() && doc.HasMember("content"))
		{
			auto content = doc.FindMember("content");
			StringBuffer buffer;
			Writer<StringBuffer> writer(buffer);
			content->value.Accept(writer);
			String response = String(buffer.GetString());
			if (response.length() > 0)
			{
				response = StringUtil::Replace(response.c_str(), "sort", "rank");
				response = StringUtil::Replace(response.c_str(), "total", "score");
			}
			return response;
		}
		return "";
	}

	String ManorRequest::parameterSignature(ui32 timestamp, String nonce)
	{
		String info = WEB_HTTP_SECRET + StringUtil::ToString(timestamp) + nonce;
		SHA1 sha1;
		sha1.update(info.c_str());
		const std::string hash = sha1.final();
		return hash.c_str();
	}

}
