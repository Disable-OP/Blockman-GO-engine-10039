#pragma once

#include "Core.h"
#include "ClientPeer.h"
#include "cpr/cpr.h"

using namespace LORD;
namespace BLOCKMAN 
{
	class HttpRequest
	{
	public:
		const static String WEB_HTTP_SECRET;
		const static String CONTENT_TYPE;
		const static String GET_MONEY_URL;
		const static String PAY_ORDER_URL;
		const static String RESUME_ORDER_URL;
		const static String CANCEL_ORDER_URL;
		const static int32_t HTTP_TIMEOUT;
		const static String GAME_TYPE;
		const static String WEB_HTTP_BASE_URL;


		static String syncGet(const char *url, std::vector<std::vector<String>> params);
		static String syncPost(const char *url, std::vector<std::vector<String>> params, const char *body);

		static void asyncGet(const char *url, std::vector<std::vector<String>> params, String marked);
		static void asyncPost(const char *url, std::vector<std::vector<String>> params, const char *body, String marked);
		
		static void loadUserMoney(String url, i64 userId, std::function<void(i64, String)> fun);
		static void buyGoods(String url, i32 price, i64 uid, i32 groupIndex, i32 goodsIndex, bool isAddItem, std::function<void(i64, String, i32, i32, bool, bool)> fun);

		static void buyRespawn(String url, i32 uniqueId, i32 price, i64 uid, std::function<void(i64, String, bool)> fun);
		static void buyChangeActor(String url, i32 uniqueId, i32 price, i64 uid, std::function<void(i64, String, bool)> fun);
		static void resumeOrder(String url, String orderId);
		static void cancelOrder(String url, String orderId, ui64 userId);
		static void pay(String ur, i32 currencyType, i32 uniqueId, i32 price, i64 platformUserId, std::function<void(i64, String, bool)> fun);
		static cpr::Parameters getHttpParameter();


		static String parameterSignature(ui32 timestamp, String nonce);

	private:
	};

}
