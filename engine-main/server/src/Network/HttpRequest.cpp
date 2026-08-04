#include "HttpRequest.h"
#include "Util/sha1.h"
#include "Object/Root.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "Server.h"
#include "ServerNetwork.h"
#include "World/World.h"
#include "World/Shop.h"
#include <sstream>
#include "Script/GameServerEvents.h"
#include "World/CprCallBackWrap.h"

using namespace rapidjson;

namespace BLOCKMAN
{
	const String HttpRequest::GAME_TYPE = "g1001";
	const String HttpRequest::CONTENT_TYPE = "application/json; charset=UTF-8";
	const String HttpRequest::WEB_HTTP_BASE_URL = "http://dev.sandboxol.com";
	const String HttpRequest::WEB_HTTP_SECRET = "pq0194mxoqfh48L362G6R09T737E273X";

	const String HttpRequest::GET_MONEY_URL = "{url}/pay/i/api/v1/wealth/users/{userId}";
	const String HttpRequest::PAY_ORDER_URL = "{url}/pay/api/v1/pay/users/purchase/game/props";
	const String HttpRequest::RESUME_ORDER_URL = "{url}/pay/api/v1/pay/users/game/props/billings";
	const String HttpRequest::CANCEL_ORDER_URL = "{url}/pay/api/v1/pay/inner/users/game/props/billings/refund";

	const int32_t HttpRequest::HTTP_TIMEOUT = 6000;

	String HttpRequest::syncGet(const char *url, std::vector<std::vector<String>> params)
	{
		auto param = getHttpParameter(); 

		for (auto pr : params)
		{
			cpr::Parameter cprp(pr[0].c_str(), pr[1].c_str());
			param.AddParameter(cprp);
		}

		LordLogInfo("http get request:%s?%s", url, param.content.c_str());
		auto r = cpr::Get(
			cpr::Timeout{ HTTP_TIMEOUT },
			cpr::Url{ url },
			param,
			cpr::Header{ { "Content-Type", CONTENT_TYPE.c_str() } }
		);

		return r.text.c_str();
	}

	String HttpRequest::syncPost(const char *url, std::vector<std::vector<String>> params, const char *body)
	{
		auto param = getHttpParameter();

		for (auto pr : params)
		{
			cpr::Parameter cprp(pr[0].c_str(), pr[1].c_str());
			param.AddParameter(cprp);
		}

		LordLogInfo("http post request:%s?%s", url, param.content.c_str());
		auto r = cpr::Post(
			cpr::Timeout{ HTTP_TIMEOUT },
			cpr::Url{ url },
			param,
			cpr::Body{ body },
			cpr::Header{ { "Content-Type", CONTENT_TYPE.c_str() } }
		);
		
		return r.text.c_str();
	}

	void HttpRequest::asyncGet(const char * url, std::vector<std::vector<String>> params, String marked)
	{
		auto param = getHttpParameter();
		for (auto pr : params)
		{
			cpr::Parameter cprp(pr[0].c_str(), pr[1].c_str());
			param.AddParameter(cprp);
		}

		LordLogInfo("http get request:%s?%s", url, param.content.c_str());

		auto fun = [](String result, String marked)
		{
			SCRIPT_EVENT::HttpResponseEvent::invoke(result, marked);
		};

		String stringUrl(url);
		auto callback = [fun, marked, stringUrl](cpr::Response response)
		{
			if (response.status_code != 200)
			{
				int code = response.status_code;
				std::string text = response.text;
				auto foo = [fun, marked, stringUrl, code, text]()
				{
					fun("", marked);
					LordLogError("http get request(%s) error, http status code : %d, response:%s", stringUrl.c_str(), code, text.c_str());
				};
				Server::Instance()->addAsyncCall(foo);
				return;
			}

			String result = String(response.text.c_str());
			auto foo = [result, marked, fun]()
			{
				fun(result, marked);
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

	void HttpRequest::asyncPost(const char * url, std::vector<std::vector<String>> params, const char * body, String marked)
	{
		auto param = getHttpParameter();
		for (auto pr : params)
		{
			cpr::Parameter cprp(pr[0].c_str(), pr[1].c_str());
			param.AddParameter(cprp);
		}
		LordLogInfo("http post request:%s?%s", url, param.content.c_str());

		auto fun = [](String result, String marked)
		{
			SCRIPT_EVENT::HttpResponseEvent::invoke(result, marked);
		};

		String stringUrl(url);
		auto callback = [fun, marked, stringUrl](cpr::Response response)
		{
			if (response.status_code != 200)
			{
				int code = response.status_code;
				std::string text = response.text;
				auto foo = [fun, marked, stringUrl, code, text]()
				{
					fun("", marked);
					LordLogError("http post request(%s) error, http status code : %d, response:%s", stringUrl.c_str(), code, text.c_str());
				};
				Server::Instance()->addAsyncCall(foo);
				return;
			}
			String result = String(response.text.c_str());
			auto foo = [result, marked, fun]()
			{
				fun(result, marked);
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

	void HttpRequest::loadUserMoney(String url, i64 userId, std::function<void(i64, String)> fun)
	{
		String finalUrl = StringUtil::Replace(GET_MONEY_URL, "{url}", url.c_str());
		finalUrl = StringUtil::Replace(finalUrl, "{userId}", StringUtil::ToString(userId).c_str());
		auto callback = [userId, fun](cpr::Response response)
		{
			if (response.status_code != 200)
			{
				LordLogError("load user money error, http status code : %d", response.status_code);
				return;
			}
			String result = String(response.text.c_str());
			auto foo = [userId, result, fun]()
			{
				fun(userId, result);
			};
			Server::Instance()->addAsyncCall(foo);
		};

		auto parameters = getHttpParameter();
		CprCallBackWrap::Instance()->GetCallback(callback,
			cpr::Timeout{ HTTP_TIMEOUT },
			cpr::Url{ finalUrl.c_str() },
			parameters,
			cpr::Header{ { "Content-Type", CONTENT_TYPE.c_str() } }
		);
	}

	void HttpRequest::buyGoods(String url, i32 price, i64 uid, i32 groupIndex, i32 goodsIndex, bool isAddItem,
		std::function<void(i64, String, i32, i32, bool, bool)> fun)
	{
		String finalUrl = StringUtil::Replace(PAY_ORDER_URL, "{url}", url.c_str());
		auto goods = Server::Instance()->getWorld()->getShop()->getGoods()[groupIndex][goodsIndex];
		auto doc = LORD::make_shared<Document>();
		doc->SetObject();
		Document::AllocatorType &allocator = doc->GetAllocator();
		doc->AddMember("currency", goods.coinId, allocator);
		doc->AddMember("gameId", StringRef(Server::Instance()->getGameType().c_str()), allocator);
		doc->AddMember("propsId", goods.uniqueId, allocator);
		doc->AddMember("quantity", price, allocator);
		Value userId(kNumberType);
		userId.SetInt64(uid);
		doc->AddMember("userId", userId, allocator);
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		doc->Accept(writer);
		const char* json = buffer.GetString();
		
		auto callback = [uid, groupIndex, goodsIndex, isAddItem, fun](cpr::Response response)
		{
			if (response.status_code != 200)
			{
				LordLogError("buy goods error, http status code : %d", response.status_code);
				fun(uid, "{}", groupIndex, goodsIndex, isAddItem, false);
				return;
			}
			String result = String(response.text.c_str());
			auto foo = [uid, result, groupIndex, goodsIndex, isAddItem, fun]()
			{
				fun(uid, result, groupIndex, goodsIndex, isAddItem, true);
			};
			Server::Instance()->addAsyncCall(foo);
		};

		auto parameters = getHttpParameter();
		CprCallBackWrap::Instance()->PostCallback(callback,
			cpr::Timeout { HTTP_TIMEOUT },
			cpr::Url { finalUrl.c_str() },
			parameters,
			cpr::Body { json },
			cpr::Header{ { "Content-Type", CONTENT_TYPE.c_str() } }
		);
	}

	void HttpRequest::buyRespawn(String url, i32 uniqueId, i32 price, i64 uid, std::function<void(i64, String, bool)> fun)
	{
		String finalUrl = StringUtil::Replace(PAY_ORDER_URL, "{url}", url.c_str());
		auto doc = LORD::make_shared<Document>();
		doc->SetObject();
		Document::AllocatorType &allocator = doc->GetAllocator();
		doc->AddMember("currency", 1, allocator);
		doc->AddMember("gameId", StringRef(Server::Instance()->getGameType().c_str()), allocator);
		doc->AddMember("propsId", uniqueId, allocator);
		doc->AddMember("quantity", price, allocator);
		Value userId(kNumberType);
		userId.SetInt64(uid);
		doc->AddMember("userId", userId, allocator);
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		doc->Accept(writer);
		const char* json = buffer.GetString();
		
		auto callback = [uid, fun](cpr::Response response)
		{
			if (response.status_code != 200)
			{
				LordLogError("buy respawn error, http status code : %d", response.status_code);
				fun(uid, "{}", false);
				return;
			}
			String result = String(response.text.c_str());
			auto foo = [uid, result, fun]()
			{
				fun(uid, result, true);
			};
			Server::Instance()->addAsyncCall(foo);
		};

		auto parameters = getHttpParameter();
		CprCallBackWrap::Instance()->PostCallback(callback,
			cpr::Timeout{ HTTP_TIMEOUT },
			cpr::Url{ finalUrl.c_str() },
			parameters,
			cpr::Body{ json },
			cpr::Header{ { "Content-Type", CONTENT_TYPE.c_str() } }
		);
	}

	void HttpRequest::buyChangeActor(String url, i32 uniqueId, i32 price, i64 uid, std::function<void(i64, String, bool)> fun)
	{
		String finalUrl = StringUtil::Replace(PAY_ORDER_URL, "{url}", url.c_str());
		auto doc = LORD::make_shared<Document>();
		doc->SetObject();
		Document::AllocatorType &allocator = doc->GetAllocator();
		doc->AddMember("currency", 1, allocator);
		doc->AddMember("gameId", StringRef(Server::Instance()->getGameType().c_str()), allocator);
		doc->AddMember("propsId", uniqueId, allocator);
		doc->AddMember("quantity", price, allocator);
		Value userId(kNumberType);
		userId.SetInt64(uid);
		doc->AddMember("userId", userId, allocator);
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		doc->Accept(writer);
		const char* json = buffer.GetString();
		
		auto callback = [uid, fun](cpr::Response response)
		{
			if (response.status_code != 200)
			{
				LordLogError("buy change actor error, http status code : %d", response.status_code);
				fun(uid, "{}", false);
				return;
			}
			String result = String(response.text.c_str());
			auto foo = [uid, result, fun]()
			{
				fun(uid, result, true);
			};
			Server::Instance()->addAsyncCall(foo);
		};

		auto parameters = getHttpParameter();
		CprCallBackWrap::Instance()->PostCallback(callback,
			cpr::Timeout{ HTTP_TIMEOUT },
			cpr::Url{ finalUrl.c_str() },
			parameters,
			cpr::Body{ json },
			cpr::Header{ { "Content-Type", CONTENT_TYPE.c_str() } }
		);
	}

	void HttpRequest::resumeOrder(String url, String orderId)
	{
		LordLogInfo("HttpRequest::resumeOrder orderId [%s]", orderId.c_str());

		String finalUrl = StringUtil::Replace(RESUME_ORDER_URL, "{url}", url.c_str());
		auto callback = [](cpr::Response response)
		{
			if (response.status_code != 200)
			{
				LordLogError("resume order error, http status code : %d", response.status_code);
				return;
			}
		};

		auto parameters = getHttpParameter();
		parameters.AddParameter({ "orderId", orderId.c_str() });

		CprCallBackWrap::Instance()->PutCallback(callback,
			cpr::Timeout{ HTTP_TIMEOUT },
			cpr::Url{ finalUrl.c_str() },
			parameters,
			cpr::Header{ { "Content-Type", CONTENT_TYPE.c_str() } }
		);
	}

	void HttpRequest::cancelOrder(String url, String orderId, ui64 userId)
	{
		LordLogInfo("HttpRequest::cancelOrder orderId [%s]", orderId.c_str());
		String finalUrl = StringUtil::Replace(CANCEL_ORDER_URL, "{url}", url.c_str());
		auto callback = [userId](cpr::Response response)
		{
			if (response.status_code != 200)
			{
				LordLogError("resume order error, http status code : %d", response.status_code);
				return;
			}

			LordLogInfo("HttpRequest::cancelOrder text [%s]", response.text.c_str());

			auto foo = [userId]()
			{
				auto pree = ServerNetwork::Instance()->findPlayerByPlatformUserId(userId);
				if (pree)
				{
					pree->loadUserMoney();
				}
			};
			Server::Instance()->addAsyncCall(foo);
		};

		auto parameters = getHttpParameter();
		parameters.AddParameter({ "orderId", orderId.c_str() });

		CprCallBackWrap::Instance()->PutCallback(callback,
			cpr::Timeout{ HTTP_TIMEOUT },
			cpr::Url{ finalUrl.c_str() },
			parameters,
			cpr::Header{ { "Content-Type", CONTENT_TYPE.c_str() } }
		);
	}

	void HttpRequest::pay(String url, i32 currencyType, i32 uniqueId, i32 price, i64 platformUserId, std::function<void(i64, String, bool)> fun)
	{
		String finalUrl = StringUtil::Replace(PAY_ORDER_URL, "{url}", url.c_str());
		auto doc = LORD::make_shared<Document>();
		doc->SetObject();
		Document::AllocatorType &allocator = doc->GetAllocator();
		doc->AddMember("currency", currencyType, allocator);
		doc->AddMember("gameId", StringRef(Server::Instance()->getGameType().c_str()), allocator);
		doc->AddMember("propsId", uniqueId, allocator);
		doc->AddMember("quantity", price, allocator);
		Value userId(kNumberType);
		userId.SetInt64(platformUserId);
		doc->AddMember("userId", userId, allocator);
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		doc->Accept(writer);
		const char* json = buffer.GetString();
		
		auto callback = [platformUserId, fun](cpr::Response response)
		{
			if (response.status_code != 200)
			{
				LordLogError("pay error, http status code : %d", response.status_code);
				fun(platformUserId, "{}", false);
				return;
			}
			String result = String(response.text.c_str());
			auto foo = [platformUserId, result, fun]()
			{
				fun(platformUserId, result, true);
			};
			Server::Instance()->addAsyncCall(foo);
		};

		auto parameters = getHttpParameter();
		CprCallBackWrap::Instance()->PostCallback(callback,
			cpr::Timeout{ HTTP_TIMEOUT },
			cpr::Url{ finalUrl.c_str() },
			parameters,
			cpr::Body{ json },
			cpr::Header{ { "Content-Type", CONTENT_TYPE.c_str() } }
		);
	}

	cpr::Parameters HttpRequest::getHttpParameter()
	{
		LORD::ui32 timestamp = LORD::Root::Instance()->getCurrentTime();
		char ibuf[10] = { 0 };
		snprintf(ibuf, sizeof(ibuf) - 1, "%d", timestamp);
		cpr::Parameters parameters =
		{
			cpr::Parameter{ "timestamp", StringUtil::ToString(timestamp).c_str() },
			cpr::Parameter{ "nonce", ibuf },
			cpr::Parameter{ "signature", parameterSignature(timestamp, ibuf).c_str() }
		};

		return parameters;
	}

	String HttpRequest::parameterSignature(ui32 timestamp, String nonce)
	{
		String info = WEB_HTTP_SECRET + nonce + StringUtil::ToString(timestamp);
		SHA1 sha1;
        sha1.update(info.c_str());
        const std::string hash = sha1.final();
		return hash.c_str();
	}

}
