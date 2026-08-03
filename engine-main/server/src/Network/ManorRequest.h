#pragma once

#include "Core.h"
#include "ClientPeer.h"

using namespace LORD;
namespace BLOCKMAN
{

	struct HttpResponse
	{
		i32 code = 0;
		String data = "";
	};

	class ManorRequest
	{
	public:
		const static String WEB_HTTP_SECRET;
		const static String CONTENT_TYPE;
		const static int32_t HTTP_TIMEOUT;
		const static bool ENABLED_LOG;

		static String parameterSignature(ui32 timestamp, String nonce);
		static void asyncGet(const char *url, map<String, String>::type params, std::function<void(String, bool)> fun);
		static void asyncPost(const char *url, map<String, String>::type params, const char *body, std::function<void(String, bool)> fun);

	public:
		static HttpResponse parseResponse(bool isSuccess, String logTag, String response);
		static void pay(String webPayUrl, String dataServerPayUrl, i32 currencyType, i32 uniqueId, i32 price, ui64 platformUserId, std::function<void(String, bool)> fun);
		static bool manorPayResult(ui64 userId, String result, bool isSuccess);
		static void trade(String url, ui64 platformUserId, i32 price, std::function<void(String, bool)> fun);
		static void trade(ui64 platformUserId, i32 price);
		static void loadGameMoney(ui64 platformUserId);

	public:
		static void loadManorConfig();

	public:
		static void receiveManor(ui64 userId, String userName);
		static void upgradeManor(String url, ui64 userId, i32 price, std::function<void(String, bool)> fun);
		static void praiseManor(ui64 userId, ui64 targetUserId);
		static void callOnManor(String url, ui64 userId, ui64 targetUserId, std::function<void(String, bool)> fun);
		static void loadManorCharm(String url, ui64 userId, std::function<void(String, bool)> fun);
		static void loadManorValue(String url, ui64 userId, std::function<void(String, bool)> fun);
		static void loadManorInfo(ui64 userId, bool isMaster);
		static void checkHasManor(ui64 userId, ui64 targetUserId);
		static void sellManor(ui64 userId);

	public:
		static void buyFurniture(String url, String manorId, i32 furnitureId, i32 price, i32 charm, std::function<void(String, bool)> fun);
		static void operationFurniture(ui64 userId, String manorId, i32 furnitureId, Vector3 pos, float yaw, i32 state);
		static void loadFurnitures(ui64 userId, String manorId);


	public:
		static void buyHouse(String url, String manorId, i32 houseId, i32 price, i32 charm, std::function<void(String, bool)> fun);
		static void buildHouse(ui64 userId, String manorId, i32 houseId);
		static void loadHouses(ui64 userId, String manorId);

	public:
		static void ploughCrop(String manorId, i32 row, i32 colum, String blockId, i32 fruitNum, i32 curState, i32 pluckingState, ui64 curStateTime, ui64 lastUpdateTime);
		static void ploughPlucking(String manorId, ui64 userId, i32 row, i32 colum, i32 num, i32 productId, ui64 pluckingTime);
		static void loadPloughInfo(ui64 userId, String manorId);

	public:
		static void saveManorInventory(String manorId, const char*  inventoryInfo);
		static void loadManorInventory(ui64 userId, String manorId);
	
	public:
		static void saveManorBlock(String manorId, const char* blockInfo);
		static void loadManorBlock(ui64 userId, String manorId);

	public:
		static void loadManorMessage(ui64  userId, i32 page = 1);
		static void removeManorMessageById(ui64  userId, String id);

	public:
		static void loadManorCharmRank(i32 size);
		static void loadManorPotentialRank(i32 size);
		static void loadMyManorRank(ui64 userId);

	private:
		static void parsePraiseResponse(bool isSuccess, ui64 userId, String response);
		static void parseMessageResponse(ui64 userId, String response);
		static String parseRankResponse(String response);
	};

}
