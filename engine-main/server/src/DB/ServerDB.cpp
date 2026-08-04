#include "ServerDB.h"
#include "Server.h"
#include "Script/GameServerEvents.h"

ServerDB::ServerDB(const std::string& ip, const std::string& userName, const std::string& passwd, const std::string& dbName)
	: m_ip(ip)
	, m_userName(userName)
	, m_password(passwd)
	, m_dbName(dbName)
	, m_bIsDBInit(false)
	, m_pConnection(NULL)
	, m_bShouldSync(false)
{
}

ServerDB::~ServerDB()
{
	if (m_bIsDBInit && m_pConnection && m_pConnection->isValid())
	{
		Statement* pStatememt = m_pConnection->createStatement();
		writeDataToDB(pStatememt);
		delete pStatememt;
		delete m_pConnection;
	}
}

void ServerDB::initDB()
{
	m_bIsDBInit = true;

	m_tableName = Server::Instance()->getGameType().c_str();
	m_tableLogName = StringUtil::Format("%s_%s",Server::Instance()->getGameType().c_str() ,"log").c_str();
	Statement* statment = NULL;
	mysql::MySQL_Driver *driver = sql::mysql::get_mysql_driver_instance();;
	try
	{
		m_pConnection = driver->connect(m_ip.c_str(), m_userName.c_str(), m_password.c_str());
		statment = m_pConnection->createStatement();
		String query = StringUtil::Format("use %s", m_dbName.c_str());
		statment->execute(query.c_str());
		//game table
		query = StringUtil::Format("create table if not exists %s(userId INT UNSIGNED, subkey INT UNSIGNED, data VARCHAR(5000), UNIQUE(userId, subkey))ENGINE=InnoDB DEFAULT CHARSET=utf8", m_tableName.c_str());
		statment->execute(query.c_str());

		//gamelog table
		query = StringUtil::Format("create table if not exists %s(id INT NOT NULL AUTO_INCREMENT, userId INT UNSIGNED,serverInfo VARCHAR(128),gameType VARCHAR(128),dataAction VARCHAR(128),  data VARCHAR(5000), logTime  DATETIME not null,PRIMARY KEY(id))ENGINE=InnoDB DEFAULT CHARSET=utf8", m_tableLogName.c_str());
		statment->execute(query.c_str());
	}
	catch (const std::exception& e)
	{
		LordLogError("init database failed! %s", e.what());
		m_bIsDBInit = false;
		std::abort();
	}

	delete statment;
	m_serverDBThread = LORD::make_shared<UThread>("ServerDBThread", &ServerDB::tick, this);
	m_serverDBThread->start();
}

bool ServerDB::setData(ui64 userId, int subKey, std::string& data, bool bImmediate)
{
	m_setMutex.lock();
	//LordLogInfo("RequestSetData %lld %d %s", userId, subKey, data.c_str());
	storeDataInMap(userId, subKey, data, &m_waitForSetMap);
	m_setMutex.unlock();

	if (bImmediate)
	{
		notifySyncDB();
	}

	return true;
}

bool ServerDB::getData(ui64 userId, int subKey)
{
	m_getMutex.lock();
	m_waitForGetQueue.push({userId, subKey});
	//LordLogInfo("RequestGetData %lld %d", userId, subKey);
	m_getMutex.unlock();
	notifySyncDB();
	return true;
}

std::string ServerDB::realGetData(ui64 userId, int subKey, Statement* statment)
{
	std::string ret("");
	if (!m_bIsDBInit || !statment)
		return ret;

	try
	{
		String query = StringUtil::Format("use %s", m_dbName.c_str());
		statment->execute(query.c_str());
		query = StringUtil::Format("select data from %s where userId=%lld AND subkey=%d", m_tableName.c_str(), userId, subKey);
		auto resultSet = statment->executeQuery(query.c_str());
		while (resultSet->next())
		{
			ret = resultSet->getString("data").c_str();
			break;
		}
	}
	catch (const std::exception& e)
	{
		LordLogError("Query database for %lld failed~ %s" , userId, e.what());
	}
	
	return ret;
}

void ServerDB::getDataFromDB(Statement * statment)
{
	if (m_dbWaitForGetQueue.size() < 1)
		return;

	if (!m_bIsDBInit || !statment)
		return;

	ui64 userId;
	int subKey;
	std::string data;

	try
	{
		String query = StringUtil::Format("use %s", m_dbName.c_str());
		statment->execute(query.c_str());

		while (m_dbWaitForGetQueue.size() > 0)
		{
			auto pair = m_dbWaitForGetQueue.front();
			userId = pair.first;
			subKey = pair.second;
			data = realGetData(userId, subKey, statment);
			storeDataInMap(userId, subKey, data, &m_dbGetResultMap);
			m_dbWaitForGetQueue.pop();
		}

		copyResultMap();
	}
	catch (const std::exception& e)
	{
		LordLogError("update database failed~~~ %s", e.what());
	}
}

void ServerDB::storeDataInMap(ui64 userId, int subKey, std::string & data, std::map<ui64, std::map<int, std::string>>* pDataMap)
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

void ServerDB::tick(StopFlag shouldStop)
{
	if (!m_bIsDBInit || m_pConnection == NULL)
		return;

	Statement* pStatement = NULL;

	while (!shouldStop)
	{
		std::unique_lock<std::mutex> lk(m_conVarMutex);
		m_conVar.wait_for(lk, std::chrono::milliseconds(10 * 1000), [this](){ return m_bShouldSync; });
		m_bShouldSync = false;
		lk.unlock();
		if (!tryReconnect(m_pConnection))
			continue;

		pStatement = m_pConnection->createStatement();
		copySetMap();
		writeDataToDB(pStatement);
		copyGetQueue();
		getDataFromDB(pStatement);
		copySetLogMap();
		writeLogDataToDB(pStatement);
		delete pStatement;
	}
}

void ServerDB::start()
{
	m_serverDBThread->start();
	LordLogInfo("ServerDBThread started, id: %d", m_serverDBThread->getNativeId());
}

void ServerDB::stop()
{
	if(m_bIsDBInit && m_serverDBThread != NULL)
		m_serverDBThread->stopSync();
}

void ServerDB::checkResultTick()
{
	ui64		userId = 0;
	int			subKey = 0;
	std::string	ret;

	m_resultMutex.lock();
	if (m_getResultMap.size() > 0)
	{
		for (auto mapIter = m_getResultMap.begin(); mapIter != m_getResultMap.end(); mapIter++)
		{
			userId = mapIter->first;
			for (auto iter = mapIter->second.begin(); iter != mapIter->second.end(); iter++)
			{
				subKey = iter->first;
				ret = iter->second;

				//LordLogInfo("checkResultTick getDBData %lld %d %s", userId, subKey, ret.c_str());
				SCRIPT_EVENT::GetDataFromDBEvent::invoke(userId, subKey, ret.c_str());
			}
			mapIter->second.clear();
		}
		m_getResultMap.clear();
	}
	m_resultMutex.unlock();
}

void ServerDB::notifySyncDB()
{
	std::unique_lock<std::mutex> lk(m_conVarMutex);
	m_bShouldSync = true;
	m_conVar.notify_one();
}

bool ServerDB::tryReconnect(Connection* pConnection)
{
	static int retryFailCnt = 0;
	int retryFailMax = 3;

	if (pConnection->isValid())
		return true;

	try
	{
		pConnection->reconnect();
		retryFailCnt = 0;
	}
	catch (const std::exception& e)
	{
		retryFailCnt++;
		LordLogError("Reconnect database failed... %s", e.what());
		if (retryFailCnt >= retryFailMax)
		{
			throw Exception("Reconnect database failed till max times!!!! Abort!");
		}
		return false;
	}

	return true;
}

void ServerDB::writeDataToDB(Statement* statment)
{
	if (m_dbWaitForSetMap.size() < 1)
		return;

	if (!m_bIsDBInit || !statment)
		return;

	ui64 userId;
	int subKey;
	std::string data;
	
	try
	{
		String query = StringUtil::Format("use %s", m_dbName.c_str());
		statment->execute(query.c_str());

		for (auto mapIter = m_dbWaitForSetMap.begin(); mapIter != m_dbWaitForSetMap.end(); mapIter++)
		{
			userId = mapIter->first;
			for (auto iter = mapIter->second.begin(); iter != mapIter->second.end(); iter++)
			{
				subKey = iter->first;
				data = iter->second;
				query = getUpdateStr(userId, subKey, data);
				statment->execute(query.c_str());
				//LordLogInfo("writeDataToDB %lld %d", userId, subKey);
			}

			mapIter->second.clear();
		}
		m_dbWaitForSetMap.clear();
	}
	catch (const std::exception& e)
	{
		LordLogError("update database failed~~~ %s", e.what());
	}
}

String ServerDB::getUpdateStr(ui64 userId, int subKey, std::string & data)
{
	return StringUtil::Format("insert into %s (userId,subkey,data) values (%lld,%d,\'%s\') on duplicate key update data=VALUES(data)",
		m_tableName.c_str(), userId, subKey, data.c_str());
}

void ServerDB::copyGetQueue()
{
	if (m_waitForGetQueue.size() < 1)
		return;

	m_getMutex.lock();
	m_dbWaitForGetQueue = m_waitForGetQueue;
	m_waitForGetQueue = {};
	m_getMutex.unlock();
}

void ServerDB::copySetMap()
{
	if (m_waitForSetMap.size() < 1)
		return;

	m_setMutex.lock();
	m_dbWaitForSetMap = m_waitForSetMap;
	m_waitForSetMap.clear();
	m_setMutex.unlock();
}

void ServerDB::copyResultMap()
{
	if (m_dbGetResultMap.size() < 1)
		return;

	m_resultMutex.lock();

	for (auto mapIter = m_dbGetResultMap.begin(); mapIter != m_dbGetResultMap.end(); mapIter++)
	{
		for (auto iter = mapIter->second.begin(); iter != mapIter->second.end(); iter++)
		{
			storeDataInMap(mapIter->first, iter->first, iter->second, &m_getResultMap);
		}

		mapIter->second.clear();
	}

	m_dbGetResultMap.clear();
	m_resultMutex.unlock();
}

void  ServerDB::storeLogDataInMap(ui64 userId, std::string gameType, std::string dataAction, std::string data)
{
	m_waitForSetLogMap.push(LogData(gameType, data, Server::Instance()->getServerInfo(), userId, dataAction));
}

bool ServerDB::setLogData(ui64 userId, std::string gameType, std::string dataAction, std::string data, bool bImmediate)
{
	m_setLogMutex.lock();

	storeLogDataInMap(userId, gameType, dataAction, data);
	m_setLogMutex.unlock();

	if (bImmediate)
	{
		notifySyncDB();
	}
	return true;
}

void ServerDB::copySetLogMap()
{
	if (m_waitForSetLogMap.size() < 1)
		return;

	m_setLogMutex.lock();

	while (m_waitForSetLogMap.size() > 0)
	{
		auto data = m_waitForSetLogMap.front();
		m_dbWaitForSetLogMap.push(data);
		m_waitForSetLogMap.pop();
	}
	m_setLogMutex.unlock();
}

void ServerDB::writeLogDataToDB(Statement* statment)
{
	if (m_dbWaitForSetLogMap.size() < 1)
		return;

	if (!m_bIsDBInit || !statment)
		return;
	ui64 userId;

	try
	{
		String query = StringUtil::Format("use %s", m_dbName.c_str());
		statment->execute(query.c_str());
		LogData data;
		
		while (m_dbWaitForSetLogMap.size() > 0)
		{
			auto data = m_dbWaitForSetLogMap.front();
			userId = data.userId;
			query = getInsterStr(userId, data);
			statment->execute(query.c_str());
			m_dbWaitForSetLogMap.pop();
		}
	}
	catch (const std::exception& e)
	{
		LordLogError("update database failed~~~ %s", e.what());
	}
}

String ServerDB::getInsterStr(ui64 userId, LogData data)
{
	return StringUtil::Format("insert into %s (userId,serverInfo,gameType,dataAction,data,logTime) values (%lld,\'%s\',\'%s\',\'%s\',\'%s\',now())",
		m_tableLogName.c_str(), userId, data.serverInfo.c_str(), data.gameType.c_str(), data.dataAction.c_str(), data.dataInfo.c_str());
}
