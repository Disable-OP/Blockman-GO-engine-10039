#pragma once
#include "Global.h"
#include <queue>

class MysqlHttpRequest : public Singleton<MysqlHttpRequest>, public ObjectAlloc
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
	const String GET_DATA_URL = "/api/v1/game/data";
	const String POST_DATA_URL = "/api/v1/game/data";
	const String POST_LOG_URL = "/api/v1/game/log";

	MysqlHttpRequest(const std::string& url);
	~MysqlHttpRequest();

	void			initDB(std::string gameType);
	void			uninitDB();
	bool			setData(ui64 userId, int subKey, std::string& data, bool bImmediate);
	bool			getData(std::string gameType, ui64 userId, int subKey);
	bool			setLogData(ui64 userId, std::string gameType, std::string dataAction, std::string data, bool bImmediate);
	void			updateWriteRequest();

private:
	void		   storeDataInMap(ui64 userId, int subKey, std::string& data, std::map<ui64, std::map<int, std::string>>* pDataMap);
	void		   storeLogDataInMap(ui64 userId, std::string gameType, std::string dataAction, std::string data);
	std::string	   getPosDataJson(); 
	std::string	   getPostLogJson();
	void		   getDataCallBack(ui64 userId, int subKey, const String& data);
	bool           checkFailCount(bool bSucc);
	bool           checkLoadDateFailCount(bool bSucc, ui64 userId, int subKey);
	bool           parseGetData(const String& data, ui64 userId, int subKey, String& ret);
	void		   clearDataMap();
	void		   clearLogMap();

	void		   postWriteRequest();
	bool		   tryPostDataRequest(const std::string json);
	bool		   tryPostLogRequest(const std::string json);

	const int						WRITE_TICK_GAP = 10 * 20;
	int								m_ticksToPostData = 0;
	int								m_ticksToPostLog = 0;
	bool							m_bIsDBInit = false;
	int								m_nFailCount = 0;
	bool							m_bIsPostingData = false;
	bool							m_bIsPostingLog = false;

	std::string						m_httpUrl = "";
	std::string						m_getDataUrl = "";
	std::string						m_postDataUrl = "";
	std::string						m_postLogUrl = "";
	std::string						m_tableName = "";
	std::string						m_tableLogName = "";

	std::map<ui64, std::map<int, std::string>>     m_waitForSetMap;
	std::list<LogData>							   m_waitForSetLogMap;
	std::map<String, i32>				m_failCountMap;
};