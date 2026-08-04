#pragma once
#include "Global.h"
#include "Util/UThread.h"
#include <map>
#include <queue>

class RedisHttpRequest : public Singleton<RedisHttpRequest>, public ObjectAlloc
{
	struct ZIncrByData
	{
		std::string setName = "";
		std::string key = "";
		int			value = 0;
		int			rank = 0; // when set: rank = 0 -> zincrby; rank = -1 ->zexpireat; rank = 1 -> zrem 

		ZIncrByData() {}
		ZIncrByData(std::string& set, std::string& k, int v, int r = 0)
			: setName(set)
			, key(k)
			, value(v)
			, rank(r)
		{}

		ZIncrByData& operator=(const ZIncrByData& data)
		{
			setName = data.setName;
			key = data.key;
			value = data.value;
			rank = data.rank;
			return *this;
		}
	};

	struct ZRangeData
	{
		std::string  setName = "";
		int			 start = 0;
		int			 end = 0;

		ZRangeData() {}
		ZRangeData(std::string& set, int s, int e)
			: setName(set)
			, start(s)
			, end(e)
		{}

		ZRangeData& operator=(const ZRangeData& data)
		{
			setName = data.setName;
			start = data.start;
			end = data.end;
			return *this;
		}
	};


public:
	const String GET_ZSCORE_URL = "/api/v1/game/rank";
	const String POST_ZINCRBY_URL = "/api/v1/game/rank";
	const String PUT_ZEXPIRE_URL = "/api/v1/game/rank/expire";
	const String GET_ZRANGE_URL = "/api/v1/game/rank/list";

	RedisHttpRequest(const std::string& httpUrl);
	~RedisHttpRequest();

	void			initDB();
	void			uninitDB();
	void			setRedisDBHost(int host);
	void            ZExpireat(std::string& setName, std::string& timeStamp);
	void            ZScore(std::string setName, std::string key);
	void			ZIncrBy(std::string& setName, std::string& key, int value);
	void			ZRange(std::string setName, int start, int end);
	void			ZRem(std::string& setName, std::string& key);
	void			UpdateWriteRequest();

private:
	bool           checkFailCount(bool bSucc);
	std::string    getPosDataJson();
	std::string    getPutExpireJson();
	void		   tryPostIncrByRequest(const std::string& json);
	void		   tryPutExpireRequest(const std::string& json);
	void		   zscoreDataCallback(const std::string& setName, const std::string& key, const std::string& result);
	void		   zrangeDataCallback(const std::string& setName, const std::string& result);
	bool           parseGetZScoreData(const std::string& setName, const std::string& key, const std::string& result, int& score, int& rank);
	bool		   parseGetZRangeData(const std::string& setName, const std::string& result, std::string &finalRet);


	std::string						m_getZScoreUrl = "";
	std::string						m_postZIncrByUrl = "";
	std::string						m_putZExpireUrl = "";
	std::string						m_getZRangeUrl = "";

	std::string						m_httpUrl = "";
	bool							m_bIsDBInit = false;
	const int						WRITE_TICK_GAP = 10 * 20;
	bool							m_bIsPostingIncrby = false;
	bool							m_bIsPutingExpire = false;
	int								m_nFailCount = 0;
	int								m_ticksToPostIncry = 0;
	int								m_ticksToPutExpire = 0;
	int								m_host = 0;

	std::list<ZIncrByData>							   m_ZIncrByQueue;
	std::list<ZIncrByData>							   m_ZExpireQueue;
};