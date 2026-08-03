#include "ServerRedisDB.h"
#include "Server.h"
#include "Script/GameServerEvents.h"

ServerRedisDB::ServerRedisDB(const std::string & ip, const std::string& passwd, int port)
	: m_ip(ip)
	, m_password(passwd)
	, m_port(port)
	, m_context(nullptr)
	, m_bNeedDB(false)
	, m_bIsDBInit(false)
	, m_bShouldSync(false)
{
}

ServerRedisDB::~ServerRedisDB()
{
	if (m_bNeedDB)
	{
		if (!m_bIsDBInit || m_context == nullptr)
		{
			tryConnect();
		}

		copyZIncrByQueue();
		writeZIncrByToDB();
		redisFree(m_context);
		m_context = nullptr;
	}
}

void ServerRedisDB::initDB()
{
	m_bNeedDB = true;

	if (!tryConnect())
	{
		LordLogInfo("Connect to RedisServer failed!!!");
		std::abort();
	}

	m_serverDBThread = LORD::make_shared<UThread>("ServerRedisDBThread", &ServerRedisDB::tick, this);
	m_serverDBThread->start();
}

void ServerRedisDB::ZExpireat(std::string & setName, std::string & timeStamp)
{
	m_zincrbyMutex.lock();
	m_ZIncrByQueue.push(ZIncrByData(setName, timeStamp, 0, -1));
	m_zincrbyMutex.unlock();
}

void ServerRedisDB::ZScore(std::string & setName, std::string & key)
{
	m_zscoreMutex.lock();
	m_ZScoreQueue.push({setName, key});
	m_zscoreMutex.unlock();
	notifySyncDB();
}

void ServerRedisDB::ZIncrBy(std::string & setName, std::string & key, int value)
{
	m_zincrbyMutex.lock();
	m_ZIncrByQueue.push(ZIncrByData(setName, key, value));
	m_zincrbyMutex.unlock();
}

void ServerRedisDB::ZRange(std::string & setName, int start, int end)
{
	m_zincrbyMutex.lock();
	m_ZRangeQueue.push(ZRangeData(setName, start, end));
	m_zincrbyMutex.unlock();
	notifySyncDB();
}

void ServerRedisDB::ZRem(std::string & setName, std::string & key)
{
	m_zincrbyMutex.lock();
	m_ZIncrByQueue.push(ZIncrByData(setName, key, 0, 1));
	m_zincrbyMutex.unlock();
}

void ServerRedisDB::tick(StopFlag shouldStop)
{
	if (!m_bNeedDB)
		return;

	while (!shouldStop)
	{
		std::unique_lock<std::mutex> lk(m_conVarMutex);
		m_conVar.wait_for(lk, std::chrono::milliseconds(5 * 1000), [this]() { return m_bShouldSync; });
		m_bShouldSync = false;
		lk.unlock();

		if (!m_bIsDBInit || m_context == nullptr)
		{
			if (!tryConnect())
				continue;
		}

		copyZIncrByQueue();
		writeZIncrByToDB();

		copyZScoreQueue();
		getZScoreFromDB();
		copyZScoreResultQ();

		copyZRangeQueue();
		getZRangeFromDB();
		copyZRangeResultQ();
	}

}

void ServerRedisDB::stop()
{
	if (m_bNeedDB && m_serverDBThread != NULL)
		m_serverDBThread->stopSync();
}

void ServerRedisDB::checkResultTick()
{
	if (m_ZScoreResultQ.size() > 0)
	{
		m_zscoreResultMutex.lock();
		while (m_ZScoreResultQ.size() > 0)
		{
			auto data = m_ZScoreResultQ.front();
			SCRIPT_EVENT::ZScoreFromRedisDBEvent::invoke(data.setName.c_str(), data.key.c_str(), data.value, data.rank);
			m_ZScoreResultQ.pop();
		}
		m_zscoreResultMutex.unlock();

	}

	if (m_ZRangeResultQ.size() > 0)
	{
		m_zrangeResultMutex.lock();
		while (m_ZRangeResultQ.size() > 0)
		{
			auto data = m_ZRangeResultQ.front();
			SCRIPT_EVENT::ZRangeFromRedisDBEvent::invoke(data.first.c_str(), data.second.c_str());
			m_ZRangeResultQ.pop();
		}
		m_zrangeResultMutex.unlock();
	}
}

void ServerRedisDB::notifySyncDB()
{
	std::unique_lock<std::mutex> lk(m_conVarMutex);
	m_bShouldSync = true;
	m_conVar.notify_one();
}

bool ServerRedisDB::tryConnect()
{
	static int retryFailCnt = 0;
	static int retryFailMax = 3;

	m_bIsDBInit = false;

	if (m_context != nullptr) {
		redisFree(m_context);
		m_context = nullptr;
	}

	struct timeval timeout = { 1, 500000 }; // 1.5 seconds
	m_context = redisConnectWithTimeout(m_ip.c_str(), m_port, timeout);
	if (m_context == NULL || m_context->err) {
		std::stringstream msg;
		if (m_context) {
			msg << "Connection error: " << m_context->errstr;
		}
		else {
			msg << "Connection error: can not allocate redis context";
		}

		LordLogError(msg.str().c_str());

		if (++retryFailCnt >= retryFailMax) {
			throw Exception(msg.str().c_str());
		}

		return false;
	}

	//if (!Auth())
	//	return false;

	redisSetTimeout(m_context, timeout);

	m_bIsDBInit = true;
	retryFailCnt = 0;
	return true;
}

bool ServerRedisDB::Auth()
{
	bool succ = false;
	if (m_context == NULL)
		return succ;

	redisReply *reply = static_cast<redisReply *>(redisCommand(m_context, "AUTH %s", m_password.c_str()));
	if (m_context->err != 0 || !reply) {
		LordLogInfo("AUTH failed: %s", m_context->errstr);
		if (reply) {
			freeReplyObject(reply);
		}

		return succ;
	}

	switch (reply->type) {
	case REDIS_REPLY_STATUS:
		succ = true;
		break;

	default:
		succ = false;
		LordLogError("AUTH failed: %s", reply->str);
	}

	freeReplyObject(reply);
	return succ;
}

void ServerRedisDB::copyZScoreQueue()
{
	if (m_ZScoreQueue.size() < 1)
		return;

	m_zscoreMutex.lock();
	while (m_ZScoreQueue.size() > 0)
	{
		m_dbZScoreQueue.push(m_ZScoreQueue.front());
		m_ZScoreQueue.pop();
	}
	m_zscoreMutex.unlock();
}

void ServerRedisDB::copyZIncrByQueue()
{
	if (m_ZIncrByQueue.size() < 1)
		return;

	m_zincrbyMutex.lock();
	while (m_ZIncrByQueue.size() > 0)
	{
		m_dbZIncrByQueue.push(m_ZIncrByQueue.front());
		m_ZIncrByQueue.pop();
	}

	m_zincrbyMutex.unlock();
}

void ServerRedisDB::copyZRangeQueue()
{
	if (m_ZRangeQueue.size() < 1)
		return;

	m_zrangeMutex.lock();
	while (m_ZRangeQueue.size() > 0)
	{
		m_dbZRangeQueue.push(m_ZRangeQueue.front());
		m_ZRangeQueue.pop();
	}
	m_zrangeMutex.unlock();
}

void ServerRedisDB::copyZScoreResultQ()
{
	if (m_dbZScoreResultQ.size() < 1)
		return;

	m_zscoreResultMutex.lock();
	while (m_dbZScoreResultQ.size() > 0)
	{
		m_ZScoreResultQ.push(m_dbZScoreResultQ.front());
		m_dbZScoreResultQ.pop();
	}
	m_zscoreResultMutex.unlock();
}

void ServerRedisDB::copyZRangeResultQ()
{
	if (m_dbZRangeResultQ.size() < 1)
		return;

	m_zrangeResultMutex.lock();
	while (m_dbZRangeResultQ.size() > 0)
	{
		m_ZRangeResultQ.push(m_dbZRangeResultQ.front());
		m_dbZRangeResultQ.pop();
	}
	m_zrangeResultMutex.unlock();
}

void ServerRedisDB::writeZIncrByToDB()
{
	if (!m_bIsDBInit || m_context == nullptr || m_dbZIncrByQueue.size() < 1)
		return;

	ZIncrByData data;
	while (m_dbZIncrByQueue.size() > 0)
	{
		data = m_dbZIncrByQueue.front();
		redisReply* reply = NULL;
		if (data.rank == -1) //expire command
		{
			reply = (redisReply *)redisCommand(m_context, "EXPIREAT %s %s", data.setName.c_str(), data.key.c_str());
		}
		else if (data.rank == 1) //remove command
		{
			reply = (redisReply *)redisCommand(m_context, "ZREM %s %s", data.setName.c_str(), data.key.c_str());
		}
		else
		{
			reply = (redisReply *)redisCommand(m_context, "ZINCRBY %s %d %s", data.setName.c_str(), data.value, data.key.c_str());
		}

		if (m_context->err != 0 || !reply) {
			LordLogError("writeZIncrByToDB connection/reply Error! %s", m_context->errstr);
			if (reply) {
				freeReplyObject(reply);
			}

			tryConnect();
			return;
		}

		m_dbZIncrByQueue.pop();
		freeReplyObject(reply);
	}
}

void ServerRedisDB::getZScoreFromDB()
{
	if (!m_bIsDBInit || m_context == nullptr || m_dbZScoreQueue.size() < 1)
		return;

	while (m_dbZScoreQueue.size() > 0)
	{
		auto data = m_dbZScoreQueue.front();
		bool succ = false;
		int value = 0;
		int rank = 0;
		redisReply* reply = (redisReply *)redisCommand(m_context, "ZSCORE %s %s", data.first.c_str(), data.second.c_str());
		if (m_context->err != 0 || !reply) {
			LordLogError("getZScoreFromDB ZSCORE connection/reply Error! %s", m_context->errstr);
			if (reply) 
				freeReplyObject(reply);

			tryConnect();
			return;
		}

		switch (reply->type) 
		{
		case REDIS_REPLY_STRING:
			value =  StringUtil::ParseI32(reply->str);
			succ = true;
			break;
		case REDIS_REPLY_NIL:
			value = 0;
			succ = true;
			break;
		case REDIS_REPLY_ERROR:
			value = 0;
			succ = false;
			LordLogError("getZScoreFromDB ZSCORE result Error, set result 0! %s", reply->str);
			break;
		default:
			value = 0;
			succ = false;
			LordLogError("getZScoreFromDB ZSCORE result Error, set result 0!");
		}

		freeReplyObject(reply);
		if (!succ) 
		{
			tryConnect();
		}

		succ = false;
		reply = (redisReply *)redisCommand(m_context, "ZREVRANK %s %s", data.first.c_str(), data.second.c_str());
		if (m_context->err != 0 || !reply) {
			LordLogError("getZScoreFromDB ZREVRANK connection/reply Error! %s", m_context->errstr);
			if (reply)
				freeReplyObject(reply);

			tryConnect();
			return;
		}

		switch (reply->type)
		{
		case REDIS_REPLY_INTEGER:
			rank = (int)reply->integer;
			rank++;
			succ = true;
			break;
		case REDIS_REPLY_NIL:
			rank = 100000;
			succ = true;
			break;
		case REDIS_REPLY_ERROR:
			rank = 100000;
			succ = false;
			LordLogError("getZScoreFromDB ZSCORE result Error, set result 100000! %s", reply->str);
			break;
		default:
			rank = 100000;
			succ = false;
			LordLogError("getZScoreFromDB ZREVRANK result Error! set result 100000!");
		}

		freeReplyObject(reply);
		if (!succ)
		{
			tryConnect();
		}

		m_dbZScoreResultQ.push(ZIncrByData(data.first, data.second, value, rank));
		m_dbZScoreQueue.pop();
	}
}

void ServerRedisDB::getZRangeFromDB()
{
	if (!m_bIsDBInit || m_context == nullptr || m_dbZRangeQueue.size() < 1)
		return;

	while (m_dbZRangeQueue.size() > 0)
	{
		auto data = m_dbZRangeQueue.front();
		redisReply* reply = (redisReply *)redisCommand(m_context, "ZREVRANGE %s %d %d WITHSCORES", data.setName.c_str(), data.start, data.end);
		if (m_context->err != 0 || !reply) {
			LordLogError("getZRangeFromDB ZREVRANGE connection/reply Error! %s", m_context->errstr);
			if (reply)
				freeReplyObject(reply);

			tryConnect();
			return;
		}

		bool succ = false;
		std::string result("");
		switch (reply->type)
		{
		case REDIS_REPLY_ARRAY:
			succ = true;
			for (unsigned int j = 0; j < reply->elements; j++)
			{
				if (!result.empty())
					result.append("#");

				auto s = reply->element[j]->str;
				result.append(s);
				result.append(":");
				redisReply* reply_sec = (redisReply *)redisCommand(m_context, "ZSCORE %s %s", data.setName.c_str(), reply->element[j]->str);
				if (m_context->err != 0 || !reply_sec)
				{
					LordLogError("getZRangeFromDB ZSCORE connection/reply Error! %s", m_context->errstr);
					if (reply_sec)
						freeReplyObject(reply_sec);

					freeReplyObject(reply);
					tryConnect();
					return;
				}
				j++;
				auto ss = reply_sec->str;
				result.append(ss);
				freeReplyObject(reply_sec);
			}
			break;
		case REDIS_REPLY_NIL:
			succ = true;
			break;
		case REDIS_REPLY_ERROR:
			succ = false;
			LordLogError("getZRangeFromDB ZSCORE result Error, set result Empty string %s!", reply->str);
			break;
		default:
			succ = false;
			LordLogError("getZRangeFromDB ZSCORE result Error, set result Empty string!");
		}

		freeReplyObject(reply);
		if (!succ)
		{
			tryConnect();
		}

		m_dbZRangeResultQ.push({data.setName, result});
		m_dbZRangeQueue.pop();
	}
}
