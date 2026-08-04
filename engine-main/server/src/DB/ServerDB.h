#pragma once
#include "Global.h"
#include "Util/UThread.h"
#include "mysql_connection.h"
#include "mysql_driver.h"
#include "cppconn/driver.h"
#include "cppconn/exception.h"
#include "cppconn/resultset.h"
#include "cppconn/statement.h"
#include "cppconn/prepared_statement.h"
#include <mutex>
#include <map>
#include <queue>
using namespace sql;

class ServerDB : public Singleton<ServerDB>, public ObjectAlloc
{
	struct LogData
	{
		std::string gameType;
		std::string dataInfo;
		std::string serverInfo;
		ui64	    userId;
		std::string	dataAction;
 
		LogData() {}
		LogData(std::string gameType, std::string dataInfo, std::string serverInfo, ui64 userId, std::string dataAction)
			: gameType(gameType)
			, dataInfo(dataInfo)
			, serverInfo(serverInfo)
			, userId(userId)
			, dataAction(dataAction)
		{}

		void operator=(const LogData& data)
		{
			gameType = data.gameType;
			dataInfo = data.dataInfo;
			serverInfo = data.serverInfo;
			userId = data.userId;
			dataAction = data.dataAction;
		}
	};

public:
	ServerDB(const std::string& ip, const std::string& userName, const std::string& passwd, const std::string& dbName);
	~ServerDB();

	void			initDB();
	bool			setData(ui64 userId, int subKey, std::string& data, bool bImmediate);
	bool			getData(ui64 userId, int subKey);
	void			tick(StopFlag shouldStop);
	void			start();
	void			stop();
	void			checkResultTick();
	bool			setLogData(ui64 userId, std::string gameType, std::string dataAction, std::string data, bool bImmediate);

private:
	void		   notifySyncDB();
	bool           tryReconnect(Connection* pConnection);
	void		   writeDataToDB(Statement* statment);
	String		   getUpdateStr(ui64 userId, int subKey, std::string& data);
	void           copyGetQueue();
	void		   copySetMap();
	void		   copyResultMap();
	std::string	   realGetData(ui64 userId, int subKey, Statement* statment);
	void		   getDataFromDB(Statement* statment);
	void		   storeDataInMap(ui64 userId, int subKey, std::string& data, std::map<ui64, std::map<int, std::string>>* pDataMap);
	void		   storeLogDataInMap(ui64 userId, std::string gameType, std::string dataAction, std::string data);
	void		   copySetLogMap();
	void		   writeLogDataToDB(Statement* statment);
	String		   getInsterStr(ui64 userId, LogData data);

	std::string						m_ip;
	std::string						m_userName;
	std::string						m_password;
	std::string						m_dbName;
	std::string						m_tableName;
	std::mutex						m_setMutex;
	std::mutex						m_getMutex;
	std::mutex						m_resultMutex;
	std::condition_variable		    m_conVar;
	std::mutex						m_conVarMutex;
	bool							m_bShouldSync;
	std::mutex						m_setLogMutex;

	bool							m_bIsDBInit;
	Connection*						m_pConnection;
	std::string						m_tableLogName;

	std::shared_ptr<UThread>		m_serverDBThread;
	std::map<ui64, std::map<int, std::string>>     m_waitForSetMap;
	std::map<ui64, std::map<int, std::string>>     m_dbWaitForSetMap;
	std::queue<std::pair<ui64, int>>				m_waitForGetQueue;
	std::queue<std::pair<ui64, int>>				m_dbWaitForGetQueue;
	std::map<ui64, std::map<int, std::string>>     m_getResultMap;
	std::map<ui64, std::map<int, std::string>>     m_dbGetResultMap;

	std::queue<LogData>							   m_waitForSetLogMap;
	std::queue<LogData>							   m_dbWaitForSetLogMap;
};