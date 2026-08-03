#include "Server.h"
#include "Script/GameServerEvents.h"
#include "MysqlHttpRequest.h"
#include "Util/sha1.h"
#include "cpr/cpr.h"
#include "Object/Root.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "Network/HttpRequest.h"
#include "World/CprCallBackWrap.h"

using namespace rapidjson;

void MysqlHttpRequest::initDB(std::string gameType)
{
	if (gameType.length() == size_t(0))
		gameType = Server::Instance()->getGameType().c_str();
	m_tableName = gameType;
	m_tableLogName = StringUtil::Format("%s_%s", m_tableName.c_str(), "log").c_str();
	if (m_tableName.empty())
	{
		throw std::runtime_error("MysqlHttpRequest::initDB Error: table name is empty!");
	}

	m_getDataUrl = StringUtil::concatToString(m_httpUrl, GET_DATA_URL).c_str();
	m_postDataUrl = StringUtil::concatToString(m_httpUrl, POST_DATA_URL).c_str();
	m_postLogUrl = StringUtil::concatToString(m_httpUrl, POST_LOG_URL).c_str();

	m_bIsDBInit = true;
}

void MysqlHttpRequest::uninitDB()
{
	if (m_bIsDBInit)
	{
		m_bIsPostingLog = false;
		m_bIsPostingData = false;
		m_ticksToPostData = 0;
		m_ticksToPostLog = 0;
		postWriteRequest();
	}
}

bool MysqlHttpRequest::setData(ui64 userId, int subKey, std::string& data, bool bImmediate)
{
	storeDataInMap(userId, subKey, data, &m_waitForSetMap);
	if (bImmediate)
	{
		m_ticksToPostData = 0;
		updateWriteRequest();
	}

	return true;
}

bool MysqlHttpRequest::getData(std::string gameType, ui64 userId, int subKey)
{
	if (gameType.length() == size_t(0))
		gameType = m_tableName;
	auto callback = [this, gameType, userId, subKey](cpr::Response response)
	{
		if (response.status_code != 200)
		{
			LordLogError("MysqlHttpRequest::getData of userId %lld subKey %d error, http status code : %d", userId, subKey, response.status_code);
			auto failFunc = [this, gameType, userId, subKey]()
			{
				checkLoadDateFailCount(false, userId, subKey);
				getData(gameType, userId, subKey);
			};
			Server::Instance()->addAsyncCall(failFunc);
		}
		else
		{
			String result = String(response.text.c_str());
			auto succFunc = [this, userId, subKey, result]()
			{
				checkLoadDateFailCount(true, userId, subKey);
				getDataCallBack(userId, subKey, result);
			};
			Server::Instance()->addAsyncCall(succFunc);
		}
	};

	char buff[20];
	sprintf(buff, "%lld", userId);
	auto parameters = HttpRequest::getHttpParameter();
	parameters.AddParameter({ "userId", buff });
	parameters.AddParameter({ "subKey", StringUtil::ToString(subKey).c_str() });
	parameters.AddParameter({ "tableName", gameType });
	CprCallBackWrap::Instance()->GetCallback(callback,
		cpr::Url{ m_getDataUrl.c_str() },
		parameters,
		cpr::Header{ { "Content-Type", HttpRequest::CONTENT_TYPE.c_str() } }
	);

	return true;
}

void MysqlHttpRequest::storeDataInMap(ui64 userId, int subKey, std::string & data, std::map<ui64, std::map<int, std::string>>* pDataMap)
{
	auto mapIter = pDataMap->find(userId);
	if (mapIter != pDataMap->end())
	{
		auto iter = mapIter->second.find(subKey);
		if (iter != mapIter->second.end())
		{
			iter->second = data;
		}
		else
		{
			mapIter->second.insert(std::map<int, std::string>::value_type(subKey, data));
		}
	}
	else
	{
		std::map<int, std::string> pMap;
		pMap.insert(std::map<int, std::string>::value_type(subKey, data));
		(*pDataMap)[userId] = pMap;
	}
}

void MysqlHttpRequest::updateWriteRequest()
{
	if (!m_bIsDBInit)
		return;

	m_ticksToPostData = --m_ticksToPostData > 0 ? m_ticksToPostData : 0;
	m_ticksToPostLog = --m_ticksToPostLog > 0 ? m_ticksToPostLog : 0;

	postWriteRequest();
}

void  MysqlHttpRequest::storeLogDataInMap(ui64 userId, std::string gameType, std::string dataAction, std::string data)
{
	m_waitForSetLogMap.push_back(LogData(gameType, data, Server::Instance()->getServerInfo(), userId, dataAction));
}

bool MysqlHttpRequest::setLogData(ui64 userId, std::string gameType, std::string dataAction, std::string data, bool bImmediate)
{
	storeLogDataInMap(userId, gameType, dataAction, data);

	if (bImmediate)
	{
		m_ticksToPostLog = 0;
		updateWriteRequest();
	}
	return true;
}

std::string MysqlHttpRequest::getPosDataJson()
{
	if (m_waitForSetMap.size() < 1 || m_bIsPostingData)
		return "";

	auto doc = LORD::make_shared<Document>();
	doc->SetObject();
	Document::AllocatorType &allocator = doc->GetAllocator();
	Value dataArr(kArrayType);
	for (auto mapIter = m_waitForSetMap.begin(); mapIter != m_waitForSetMap.end(); mapIter++)
	{
		for (auto iter = mapIter->second.begin(); iter != mapIter->second.end(); iter++)
		{
			Value userId(kNumberType);
			userId.SetInt64(mapIter->first);
			Value data(kObjectType);
			data.AddMember("subKey", iter->first, allocator);
			data.AddMember("userId", userId, allocator);
			auto ref = StringRef(iter->second.c_str());
			data.AddMember("data", ref, allocator);
			dataArr.PushBack(data, allocator);
		}
	}

	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	dataArr.Accept(writer);
	const char* json = buffer.GetString();
	clearDataMap();
	return std::string(json);
}

std::string MysqlHttpRequest::getPostLogJson()
{
	if (m_waitForSetLogMap.size() < 1 || m_bIsPostingLog)
		return "";

	auto doc = LORD::make_shared<Document>();
	doc->SetObject();
	Document::AllocatorType &allocator = doc->GetAllocator();

	Value logDataArr(kArrayType);

	for (auto iter = m_waitForSetLogMap.begin(); iter != m_waitForSetLogMap.end(); iter++)
	{
		Value logData(kObjectType);
		Value logUserId(kNumberType);
		logUserId.SetInt64((*iter).userId);
		logData.AddMember("userId", logUserId, allocator);
		logData.AddMember("serverInfo", StringRef((*iter).serverInfo.c_str()), allocator);
		logData.AddMember("gameType", StringRef((*iter).gameType.c_str()), allocator);
		logData.AddMember("dataAction", StringRef((*iter).dataAction.c_str()), allocator);
		logData.AddMember("data", StringRef((*iter).dataInfo.c_str()), allocator);
		logDataArr.PushBack(logData, allocator);
	}

	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	logDataArr.Accept(writer);
	const char* json = buffer.GetString();
	clearLogMap();
	return std::string(json);
}

void MysqlHttpRequest::getDataCallBack(ui64 userId, int subKey, const String & data)
{
	String retData("");
	bool bRet = parseGetData(data, userId, subKey, retData);

	if(bRet)
		SCRIPT_EVENT::GetDataFromDBEvent::invoke(userId, subKey, retData);
}

bool MysqlHttpRequest::checkFailCount(bool bSucc)
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
		m_nFailCount = 0;
		throw std::runtime_error("MysqlHttpRequest::checkFailCount till 3 times, please check your http connect...");
	}

	return true;
}

bool MysqlHttpRequest::checkLoadDateFailCount(bool bSucc, ui64 userId, int subKey)
{
	String key = StringUtil::Format("%lld_%d", userId, subKey);
	if (bSucc)
	{
		m_failCountMap[key] = 0;
	}
	else
	{
		m_failCountMap[key]++;
	}

	if (m_failCountMap[key] >= 3)
	{
		m_failCountMap[key] = 0;
		SCRIPT_EVENT::GetDataFromDBErrorEvent::invoke(userId, subKey);
		throw std::runtime_error("MysqlHttpRequest::checkLoadDateFailCount till 3 times, please check your http connect...");
	}
	return true;
}

bool MysqlHttpRequest::parseGetData(const String & data, ui64 userId, int subKey, String& ret)
{
	rapidjson::Document doc;
	doc.Parse(data.c_str());
	if (doc.HasParseError()) 
	{
		LordLogError("MysqlHttpRequest::parseGetData error, parse json fail, [%s]", data.c_str());
		return false;
	}

	if (!doc.HasMember("code") || !doc.HasMember("data") || !doc.HasMember("message"))
	{
		LordLogError("MysqlHttpRequest::parseGetData error, lack of field, jsonStr=[%s]", data.c_str());
		return false;
	}

	int code = doc["code"].GetInt();
	if (code != 1) // 1, SUCCESS; 2, FAILED; 3, PARAM ERROR; 4, INNER ERROR; 5, TIME OUT; 6,AUTH_FAILED
	{
		const char* message = doc["message"].GetString();
		LordLogError("MysqlHttpRequest::parseGetData error, code is [%d], message is [%s], jsonStr=[%s]", code, message, data.c_str());
		return false;
	}

	auto bIsNullData = doc.FindMember("data")->value.IsNull();
	if (bIsNullData)
	{
		ret = "";
		return true;
	}

	auto dataObj = doc["data"].GetObject();
	if (!dataObj.HasMember("data") || !dataObj.HasMember("subKey") || !dataObj.HasMember("userId"))
	{
		LordLogError("MysqlHttpRequest::parseGetData error, lack of field, jsonStr=[%s]", data.c_str());
		return false;
	}

	ui64 uId = dataObj["userId"].GetUint64();
	int skey = dataObj["subKey"].GetInt();
	if (userId != uId || skey != subKey)
	{
		LordLogError("MysqlHttpRequest::parseGetData error, origin userId=[%lld] subKey=[%d], now userId=[%lld] subKey=[%d], jsonStr=[%s]", 
			userId, subKey, uId, skey, data.c_str());
		return false;
	}

	ret = dataObj["data"].GetString();
	return true;
}


void MysqlHttpRequest::clearDataMap()
{
	for (auto mapIter = m_waitForSetMap.begin(); mapIter != m_waitForSetMap.end(); mapIter++)
	{
		mapIter->second.clear();
	}
	m_waitForSetMap.clear();
}

void MysqlHttpRequest::clearLogMap()
{
	m_waitForSetLogMap.clear();
}

void MysqlHttpRequest::postWriteRequest()
{
	if (m_ticksToPostData <= 0 && !m_bIsPostingData)
	{
		tryPostDataRequest(getPosDataJson());
		m_ticksToPostData = WRITE_TICK_GAP;
	}

	if (m_ticksToPostLog <= 0 && !m_bIsPostingLog)
	{
		tryPostLogRequest(getPostLogJson());
		m_ticksToPostLog = WRITE_TICK_GAP;
	}
}

bool MysqlHttpRequest::tryPostDataRequest(const std::string json)
{
	if(json.empty())
		return false;

	auto callback = [this, json](cpr::Response response)
	{
		if (response.status_code != 200 && response.status_code != 201)
		{
			LordLogError("MysqlHttpRequest::tryPostDataRequest error, http status code : %d", response.status_code);
			auto failFunc = [this, json]()
			{
				checkFailCount(false);
				tryPostDataRequest(json);
			};
			Server::Instance()->addAsyncCall(failFunc);
		}
		else
		{
			auto succFunc = [this]()
			{
				checkFailCount(true);
				m_bIsPostingData = false;
			};
			Server::Instance()->addAsyncCall(succFunc);
		}
	};

	m_bIsPostingData = true;
	auto parameters = HttpRequest::getHttpParameter();
	parameters.AddParameter({ "tableName", m_tableName });
	CprCallBackWrap::Instance()->PostCallback(callback,
		cpr::Url{ m_postDataUrl.c_str() },
		parameters,
		cpr::Body{ json },
		cpr::Header{ { "Content-Type", HttpRequest::CONTENT_TYPE.c_str() } }
	);

	return true;
}

bool MysqlHttpRequest::tryPostLogRequest(const std::string json)
{
	if (json.empty())
		return false;

	auto callback = [this, json](cpr::Response response)
	{
		if (response.status_code != 200 && response.status_code != 201)
		{
			LordLogError("MysqlHttpRequest::tryPostLogRequest error, http status code : %d", response.status_code);
			auto failFunc = [this, json]()
			{
				checkFailCount(false);
				tryPostLogRequest(json);
			};
			Server::Instance()->addAsyncCall(failFunc);
		}
		else
		{
			auto succFunc = [this]()
			{
				checkFailCount(true);
				m_bIsPostingLog = false;
			};
			Server::Instance()->addAsyncCall(succFunc);
		}
	};

	m_bIsPostingLog = true;
	auto parameters = HttpRequest::getHttpParameter();
	parameters.AddParameter({ "tableName", m_tableLogName });
	CprCallBackWrap::Instance()->PostCallback(callback,
		cpr::Url{ m_postLogUrl.c_str() },
		parameters,
		cpr::Body{ json },
		cpr::Header{ { "Content-Type", HttpRequest::CONTENT_TYPE.c_str() } }
	);

	return true;
}

MysqlHttpRequest::MysqlHttpRequest(const std::string & url)
	: m_httpUrl(url)
	, m_bIsDBInit(false)
	, m_ticksToPostData(WRITE_TICK_GAP)
	, m_ticksToPostLog(WRITE_TICK_GAP)
	, m_nFailCount(0)
	, m_bIsPostingLog(false)
	, m_bIsPostingData(false)
{

}

MysqlHttpRequest::~MysqlHttpRequest()
{
}
