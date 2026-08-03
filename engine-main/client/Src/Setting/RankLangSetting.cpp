#include "RankLangSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"

namespace BLOCKMAN
{
	std::list<RankLang*> RankLangSetting::m_rankLangs = {};
	std::list<RealTimeRankLang*> RealTimeRankLangSetting::m_realTimeRankLangs = {};

	bool RankLangSetting::loadSetting()
	{
		m_rankLangs.clear();
		RankLang* pLang = NULL;
		CsvReader* pReader = new CsvReader();

		std::string path = (PathUtil::ConcatPath(Root::Instance()->getRootPath(), "Media/Setting/ranklang.csv")).c_str();
		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordLogError("Can not open rank lang setting file %s", path.c_str());
			return false;
		}
		pReader->Read(); // discard the second line
		while (pReader->Read())
		{
			String gameType;
			pReader->GetString("GameType", gameType);
			ClientGameType type = GameTypeSetting::GetGameType(gameType);
			if (type == ClientGameType::COMMON)
			{
				continue;
			}
			pLang = new RankLang();
			pLang->gameType = type;
			pReader->GetInt("RankType", pLang->rankType);
			pReader->GetString("Title", pLang->title);
			pReader->GetString("LeftSubTitle", pLang->leftSubTitle);
			pReader->GetString("LeftScore", pLang->leftScore);
			pReader->GetString("LeftMyRank", pLang->leftMyRank);
			pReader->GetString("LeftMyScore", pLang->leftMyScore);
			pReader->GetString("RightSubTitle", pLang->rightSubTitle);
			pReader->GetString("RightScore", pLang->rightScore);
			pReader->GetString("RightMyRank", pLang->rightMyRank);
			pReader->GetString("RightMyScore", pLang->rightMyScore);
			m_rankLangs.push_back(pLang);
		}

		pReader->Close();
		LordSafeDelete(pReader);
		return true;
	}

	void RankLangSetting::unloadSetting()
	{
		for (auto iter = m_rankLangs.begin(); iter != m_rankLangs.end(); iter++)
		{
			delete *iter;
		}
		m_rankLangs.clear();
	}

	RankLang * RankLangSetting::getRankLang(ClientGameType gameType, int rankType)
	{
		for (auto iter = m_rankLangs.begin(); iter != m_rankLangs.end(); iter++)
		{
			auto lang = *iter;
			if (lang->gameType == gameType && lang->rankType == rankType)
			{
				return lang;
			}
		}
		return nullptr;
	}


	bool RealTimeRankLangSetting::loadSetting()
	{
		m_realTimeRankLangs.clear();
		RealTimeRankLang* pLang = NULL;
		CsvReader* pReader = new CsvReader();

		std::string path = (PathUtil::ConcatPath(Root::Instance()->getRootPath(), "Media/Setting/realtimeranklang.csv")).c_str();
		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordLogError("Can not open rank lang setting file %s", path.c_str());
			return false;
		}
		pReader->Read(); // discard the second line
		while (pReader->Read())
		{
			String gameType;
			pReader->GetString("GameType", gameType);
			ClientGameType type = GameTypeSetting::GetGameType(gameType);
			if (type == ClientGameType::COMMON)
			{
				continue;
			}
			pLang = new RealTimeRankLang();
			pLang->gameType = type;
			pReader->GetInt("ColumnCount", pLang->ColumnCount);
			pReader->GetString("ColumnOne", pLang->ColumnOne);
			pReader->GetString("ColumnTwo", pLang->ColumnTwo);
			pReader->GetString("ColumnThree", pLang->ColumnThree);
			pReader->GetString("ColumnFour", pLang->ColumnFour);
			pReader->GetString("ColumnFive", pLang->ColumnFive);
			m_realTimeRankLangs.push_back(pLang);
		}

		pReader->Close();
		LordSafeDelete(pReader);
		return true;
	}

	void RealTimeRankLangSetting::unloadSetting()
	{
		for (auto iter = m_realTimeRankLangs.begin(); iter != m_realTimeRankLangs.end(); iter++)
		{
			delete *iter;
		}
		m_realTimeRankLangs.clear();
	}

	RealTimeRankLang * RealTimeRankLangSetting::getRankLang(ClientGameType gameType)
	{
		for (auto iter = m_realTimeRankLangs.begin(); iter != m_realTimeRankLangs.end(); iter++)
		{
			auto lang = *iter;
			if (lang->gameType == gameType)
			{
				return lang;
			}
		}
		return nullptr;
	}

}