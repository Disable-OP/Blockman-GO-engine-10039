#pragma once
#include "Global.h"
#include "Util/UThread.h"
#include "hiredis/hiredis.h"
#include <mutex>
#include <map>
#include <queue>

class ServerRedisDB : public Singleton<ServerRedisDB>, public ObjectAlloc
{
	struct ZIncrByData 
	{
		std::string setName;
		std::string key;
		int			value;
		int			rank; // when set: rank = 0 -> zincrby; rank = -1 ->zexpireat; rank = 1 -> zrem 

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
		std::string  setName;
		int			 start;
		int			 end;

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
	ServerRedisDB(const std::string& ip, const std::string& passwd, int port);
	~ServerRedisDB();

	void			initDB();
	void            ZExpireat(std::string& setName, std::string& timeStamp);
	void            ZScore(std::string& setName, std::string& key);
	void			ZIncrBy(std::string& setName, std::string& key, int value);
	void			ZRange(std::string& setName, int start, int end);
	void			ZRem(std::string& setName, std::string& key);
	void			tick(StopFlag shouldStop);
	void			stop();
	void			checkResultTick();

private:
	void		   notifySyncDB();
	bool           tryConnect();
	bool           Auth();
	
	void		   copyZIncrByQueue();
	void		   writeZIncrByToDB();

	void		   copyZScoreQueue();
	void		   getZScoreFromDB();
	void		   copyZScoreResultQ();

	void		   copyZRangeQueue();
	void		   getZRangeFromDB();
	void		   copyZRangeResultQ();

	std::string						m_ip;
	std::string						m_password;
	int								m_port;
	redisContext*					m_context;

	std::mutex						m_zscoreMutex;
	std::mutex						m_zincrbyMutex;
	std::mutex						m_zrangeMutex;
	std::mutex						m_zscoreResultMutex;
	std::mutex						m_zrangeResultMutex;

	std::condition_variable		    m_conVar;
	std::mutex						m_conVarMutex;
	bool							m_bShouldSync;

	bool							m_bIsDBInit;
	bool							m_bNeedDB;

	std::shared_ptr<UThread>						   m_serverDBThread;

	std::queue<std::pair<std::string, std::string>>    m_ZScoreQueue;
	std::queue<std::pair<std::string, std::string>>	   m_dbZScoreQueue;
	std::queue<ZIncrByData>							   m_ZIncrByQueue;
	std::queue<ZIncrByData>							   m_dbZIncrByQueue;
	std::queue<ZRangeData>							   m_ZRangeQueue;
	std::queue<ZRangeData>							   m_dbZRangeQueue;

	std::queue<ZIncrByData>							   m_dbZScoreResultQ;
	std::queue<ZIncrByData>							   m_ZScoreResultQ;
	std::queue<std::pair<std::string, std::string>>    m_dbZRangeResultQ;
	std::queue<std::pair<std::string, std::string>>	   m_ZRangeResultQ;
};