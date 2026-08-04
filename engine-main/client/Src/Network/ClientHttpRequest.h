#pragma once

#include "Core.h"
#include "cpr/cpr.h"

using namespace LORD;
namespace BLOCKMAN 
{
	struct ClientHttpResponse
	{
		i32 code = 0;
		String data = "";
	};

	class ClientHttpRequest
	{

		
	public:

		const static bool ENABLED_LOG;
		const static int32_t HTTP_TIMEOUT;
		const static String CONTENT_TYPE;

		static void asyncGet(String url, map<String, String>::type params, std::function<void(String, bool)> fun);
		static void asyncPost(String url, map<String, String>::type params, String body, std::function<void(String, bool)> fun);
		static ClientHttpResponse parseResponse(bool isSuccess, String logTag, String response);

	public:
		static void refreshFriend();
		static void loadFriend();
		static void loadClanInfo();
		static void loadClanMember();
		static void loadUserRanchInfo(bool isFriend);
		static void loadMails();
		static void loadUserInfo(i64 userId);

		static void loadSeasonRule(int retry = 3);
		static void loadSeasonRank(int type, int highRank, int page, int size, int retry = 3);

	private:
		static void loadUserDecoration(i64 userId);

	private:
		
		static String getUserId();
		static String getBaseUrl();
		static String getLanguage();
		static String getUserToken();
		static String getGameType();
		static cpr::Header getHttpHeader();
		
	};

}
