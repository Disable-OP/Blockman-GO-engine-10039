#include "RedisHttpRequest.h"
#include "Server.h"
#include "Script/GameServerEvents.h"
#include "Util/sha1.h"
#include "cpr/cpr.h"
#include "Object/Root.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "Network/HttpRequest.h"
#include "World/CprCallBackWrap.h"
using namespace rapidjson;

RedisHttpRequest::RedisHttpRequest(const std::string & httpUrl)
	: m_httpUrl(httpUrl)
	, m_bIsDBInit(false)
	, m_ticksToPostIncry(WRITE_TICK_GAP)
	, m_ticksToPutExpire(WRITE_TICK_GAP)
	, m_nFailCount(0)
	, m_bIsPostingIncrby(false)
	, m_bIsPutingExpire(false)
{
}

RedisHttpRequest::~RedisHttpRequest()
{
}

void RedisHttpRequest::initDB()
{
	m_getZScoreUrl = StringUtil::concatToString(m_httpUrl, GET_ZSCORE_URL).c_str();
	m_postZIncrByUrl = StringUtil::concatToString(m_httpUrl, POST_ZINCRBY_URL).c_str();
	m_putZExpireUrl = StringUtil::concatToString(m_httpUrl, PUT_ZEXPIRE_URL).c_str();
	m_getZRangeUrl = StringUtil::concatToString(m_httpUrl, GET_ZRANGE_URL).c_str();
	m_bIsDBInit = true;
}

void RedisHttpRequest::uninitDB()
{
	if (m_bIsDBInit)
	{
		m_ticksToPostIncry = 0;
		m_ticksToPutExpire = 0;
		m_bIsPostingIncrby = false;
		m_bIsPutingExpire = false;
		UpdateWriteRequest();
	}
}

void RedisHttpRequest::setRedisDBHost(int host)
{
	m_host = host;
}

void RedisHttpRequest::ZIncrBy(std::string & setName, std::string & key, int value)
{
	m_ZIncrByQueue.push_back(ZIncrByData(setName, key, value));
}

void RedisHttpRequest::ZRem(std::string & setName, std::string & key)
{
	m_ZIncrByQueue.push_back(ZIncrByData(setName, key, 0, 1));
}

void RedisHttpRequest::ZExpireat(std::string & setName, std::string & timeStamp)
{
	m_ZExpireQueue.push_back(ZIncrByData(setName, timeStamp, 0, -1));
}

void RedisHttpRequest::ZScore(std::string setName, std::string key)
{
	auto callback = [this, setName, key](cpr::Response response)
	{
		if (response.status_code != 200)
		{
			LordLogError("RedisHttpRequest::ZScore of setName %s, key %s error, http status code : %d, response:%s",
				setName.c_str(), key.c_str(), response.status_code, response.text.c_str());
			auto failFunc = [this, setName, key]() mutable
			{
				checkFailCount(false);
				ZScore(setName, key);
			};
			Server::Instance()->addAsyncCall(failFunc);
		}
		else
		{
			std::string result = response.text.c_str();
			auto succFunc = [this, setName, key, result]()
			{
				checkFailCount(true);
				zscoreDataCallback(setName, key, result);
			};
			Server::Instance()->addAsyncCall(succFunc);
		}
	};

	auto parameters = HttpRequest::getHttpParameter();
	parameters.AddParameter({ "key", setName });
	parameters.AddParameter({ "member", key });
	parameters.AddParameter({ "isNew", StringUtil::ToString(m_host).c_str() });
	CprCallBackWrap::Instance()->GetCallback(callback,
		cpr::Url{ m_getZScoreUrl.c_str() },
		parameters,
		cpr::Header{ { "Content-Type", HttpRequest::CONTENT_TYPE.c_str() } }
	);
}

void RedisHttpRequest::ZRange(std::string setName, int start, int end)
{
	auto callback = [this, setName, start, end](cpr::Response response)
	{
		if (response.status_code != 200)
		{
			LordLogError("RedisHttpRequest::ZRange of setName %s error, http status code : %d, response:%s",
				setName.c_str(), response.status_code, response.text.c_str());
			auto failFunc = [this, setName, start, end]() mutable
			{
				checkFailCount(false);
				ZRange(setName, start, end);
			};
			Server::Instance()->addAsyncCall(failFunc);
		}
		else
		{
			std::string result = response.text.c_str();
			auto succFunc = [this, setName, result]()
			{
				zrangeDataCallback(setName, result);
			};
			Server::Instance()->addAsyncCall(succFunc);
		}
	};

	auto parameters = HttpRequest::getHttpParameter();
	parameters.AddParameter({ "key", setName.c_str() });
	parameters.AddParameter({ "start", StringUtil::ToString(start).c_str() });
	parameters.AddParameter({ "end",  StringUtil::ToString(end).c_str() });
	parameters.AddParameter({ "isNew", StringUtil::ToString(m_host).c_str() });
	LordLogInfo("RedisHttpRequest::ZRange:%s?%s", m_getZRangeUrl.c_str(), parameters.content.c_str());

	CprCallBackWrap::Instance()->GetCallback(callback,
		cpr::Url{ m_getZRangeUrl.c_str() },
		parameters,
		cpr::Header{ { "Content-Type", HttpRequest::CONTENT_TYPE.c_str() } }
	);
}

void RedisHttpRequest::UpdateWriteRequest()
{
	if (!m_bIsDBInit)
		return;

	m_ticksToPostIncry = --m_ticksToPostIncry > 0 ? m_ticksToPostIncry : 0;
	m_ticksToPutExpire = --m_ticksToPutExpire > 0 ? m_ticksToPutExpire : 0;

	if (m_ticksToPostIncry <= 0 && !m_bIsPostingIncrby)
	{
		tryPostIncrByRequest(getPosDataJson());
		m_ticksToPostIncry = WRITE_TICK_GAP;
	}

	if (m_ticksToPutExpire <= 0 && !m_bIsPutingExpire)
	{
		tryPutExpireRequest(getPutExpireJson());
		m_ticksToPutExpire = WRITE_TICK_GAP;
	}
}

bool RedisHttpRequest::checkFailCount(bool bSucc)
{
	if (bSucc)
	{
		m_nFailCount = 0;
	}
	else
	{
		m_nFailCount++;
	}

	if (m_nFailCount >= 3)
	{
		throw std::runtime_error("RedisHttpRequest::checkFailCount till 3 times, please check your http connect...");
	}

	return true;
}

std::string RedisHttpRequest::getPosDataJson()
{
	if (m_ZIncrByQueue.size() < 1 || m_bIsPostingIncrby)
		return "";

	auto doc = LORD::make_shared<Document>();
	doc->SetObject();
	Document::AllocatorType &allocator = doc->GetAllocator();
	Value dataArr(kArrayType);

	for (auto iter = m_ZIncrByQueue.begin(); iter != m_ZIncrByQueue.end(); iter++)
	{
		Value data(kObjectType);
		data.AddMember("key", StringRef((*iter).setName.c_str()), allocator);
		data.AddMember("member", StringRef((*iter).key.c_str()), allocator);
		data.AddMember("count", (*iter).value, allocator);
		data.AddMember("add", (*iter).rank == 0, allocator);
		dataArr.PushBack(data, allocator);
	}

	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	dataArr.Accept(writer);
	const char* json = buffer.GetString();
	m_ZIncrByQueue.clear();
	return std::string(json);
}

std::string RedisHttpRequest::getPutExpireJson()
{
	if (m_ZExpireQueue.size() < 1 || m_bIsPutingExpire)
		return "";

	auto doc = LORD::make_shared<Document>();
	doc->SetObject();
	Document::AllocatorType &allocator = doc->GetAllocator();
	Value dataArr(kArrayType);

	for (auto iter = m_ZExpireQueue.begin(); iter != m_ZExpireQueue.end(); iter++)
	{
		Value data(kObjectType);
		data.AddMember("key", StringRef((*iter).setName.c_str()), allocator);
		data.AddMember("expireTime", StringRef((*iter).key.c_str()), allocator);
		dataArr.PushBack(data, allocator);
	}

	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	dataArr.Accept(writer);
	const char* json = buffer.GetString();
	m_ZExpireQueue.clear();
	return std::string(json);
}

void RedisHttpRequest::tryPostIncrByRequest(const std::string& json)
{
	if (json.empty())
		return;

	auto callback = [this, json](cpr::Response response)
	{
		if (response.status_code != 200)
		{
			LordLogError("RedisHttpRequest::tryPostIncrByRequest write data error, http status code : %d, response:%s",
				response.status_code, response.text.c_str());
			auto failFunc = [this, json]()
			{
				checkFailCount(false);
				tryPostIncrByRequest(json);
			};
			Server::Instance()->addAsyncCall(failFunc);
		}
		else
		{
			auto succFunc = [this]()
			{
				checkFailCount(true);
				m_bIsPostingIncrby = false;
			};
			Server::Instance()->addAsyncCall(succFunc);
		}
	};

	auto parameters = HttpRequest::getHttpParameter();
	parameters.AddParameter({ "isNew", StringUtil::ToString(m_host).c_str() });
	m_bIsPostingIncrby = true;
	CprCallBackWrap::Instance()->PostCallback(
		callback,
		parameters,
		cpr::Url{ m_postZIncrByUrl.c_str() },
		cpr::Body{ json },
		cpr::Header{ { "Content-Type", HttpRequest::CONTENT_TYPE.c_str() } }
	);
}

void RedisHttpRequest::tryPutExpireRequest(const std::string& json)
{
	if (json.empty())
		return;

	auto callback = [this, json](cpr::Response response)
	{
		if (response.status_code != 200)
		{
			LordLogError("RedisHttpRequest::tryPutExpireRequest write data error, http status code : %d, response:%s",
				response.status_code, response.text.c_str());
			auto failFunc = [this, json]()
			{
				checkFailCount(false);
				tryPutExpireRequest(json);
			};
			Server::Instance()->addAsyncCall(failFunc);
		}
		else
		{
			auto succFunc = [this]()
			{
				checkFailCount(true);
				m_bIsPutingExpire = false;
			};
			Server::Instance()->addAsyncCall(succFunc);
		}
	};

	m_bIsPutingExpire = true;
	auto parameters = HttpRequest::getHttpParameter();
	parameters.AddParameter({ "isNew", StringUtil::ToString(m_host).c_str() });
	CprCallBackWrap::Instance()->PutCallback(
		callback,
		parameters,
		cpr::Url{ m_putZExpireUrl.c_str() },
		cpr::Body{ json },
		cpr::Header{ { "Content-Type", HttpRequest::CONTENT_TYPE.c_str() } }
	);
}

void RedisHttpRequest::zscoreDataCallback(const std::string & setName, const std::string & key, const std::string & result)
{
	int score = 0;
	int rank = 0;
	bool bRet = parseGetZScoreData(setName, key, result, score, rank);

	if (bRet)
		SCRIPT_EVENT::ZScoreFromRedisDBEvent::invoke(setName.c_str(), key.c_str(), score, rank);
}

void RedisHttpRequest::zrangeDataCallback(const std::string & setName, const std::string & result)
{
	std::string finalRet("");
	bool bRet = parseGetZRangeData(setName, result, finalRet);

	if(bRet)
		SCRIPT_EVENT::ZRangeFromRedisDBEvent::invoke(setName.c_str(), finalRet.c_str());
}

bool RedisHttpRequest::parseGetZScoreData(const std::string& setName, const std::string& key, const std::string& result, int& score, int& rank)
{
	rapidjson::Document doc;
	doc.Parse(result.c_str());
	if (doc.HasParseError()) 
	{
		LordLogError("RedisHttpRequest::parseGetZScoreData error, parse json fail, [%s]", result.c_str());
		return false;
	}

	if (!doc.HasMember("code") || !doc.HasMember("data") || !doc.HasMember("message"))
	{
		LordLogError("RedisHttpRequest::parseGetZScoreData error, lack of field, jsonStr=[%s]", result.c_str());
		return false;
	}

	int code = doc["code"].GetInt();
	if (code != 1) // 1, SUCCESS; 2, FAILED; 3, PARAM ERROR; 4, INNER ERROR; 5, TIME OUT; 6,AUTH_FAILED
	{
		const char* message = doc["message"].GetString();
		LordLogError("RedisHttpRequest::parseGetZScoreData error, code is [%d], message is [%s], jsonStr=[%s]", code, message, result.c_str());
		return false;
	}

	auto bIsNullData = doc.FindMember("data")->value.IsNull();
	if (bIsNullData)
	{
		score = 0;
		rank = 0;
		return true;
	}

	auto dataObj = doc["data"].GetObject();
	if (!dataObj.HasMember("rank") || !dataObj.HasMember("score"))
	{
		LordLogError("RedisHttpRequest::parseGetZScoreData error, lack of field, jsonStr=[%s]", result.c_str());
		return false;
	}

	score = (int)dataObj["score"].GetFloat();
	rank = dataObj["rank"].GetInt();
	return true;
}

bool RedisHttpRequest::parseGetZRangeData(const std::string & setName, const std::string & result, std::string & finalRet)
{
	rapidjson::Document doc;
	doc.Parse(result.c_str());
	if (doc.HasParseError())
	{
		LordLogError("RedisHttpRequest::parseGetZRangeData error, parse json fail, [%s]", result.c_str());
		return false;
	}

	if (!doc.HasMember("code") || !doc.HasMember("data") || !doc.HasMember("message"))
	{
		LordLogError("RedisHttpRequest::parseGetZRangeData error, lack of field, jsonStr=[%s]", result.c_str());
		return false;
	}

	int code = doc["code"].GetInt();
	if (code != 1) // 1, SUCCESS; 2, FAILED; 3, PARAM ERROR; 4, INNER ERROR; 5, TIME OUT; 6,AUTH_FAILED
	{
		const char* message = doc["message"].GetString();
		LordLogError("RedisHttpRequest::parseGetZRangeData error, code is [%d], message is [%s], jsonStr=[%s]", code, message, result.c_str());
		return false;
	}

	auto bIsNullData = doc.FindMember("data")->value.IsNull();
	if (bIsNullData)
	{
		return true;
	}

	auto dataArr = doc["data"].GetArray();
	for (size_t i = 0; i < dataArr.Size(); ++i)
	{
		auto obj = dataArr[i].GetObject();
		if (!obj.HasMember("member") || !obj.HasMember("score"))
		{
			LordLogError("RedisHttpRequest::parseGetZRangeData error, lack of field, jsonStr=[%s]", result.c_str());
			return false;
		}

		std::string member = obj["member"].GetString();
		int score = (int)obj["score"].GetFloat();

		if (!finalRet.empty())
			finalRet.append("#");

		finalRet.append(member);
		finalRet.append(":");
		finalRet.append(StringUtil::ToString(score).c_str());
	}

	return true;
}
