#include "ScorePointSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"
#include "Util/LanguageKey.h"
#include "Util/LanguageManager.h"

namespace BLOCKMAN 
{
	std::map<int, String>      ScorePointSetting::m_scorePointIndexToName	= {};
	int						   ScorePointSetting::m_scorePointTotal			= 0;

	bool ScorePointSetting::loadSetting()
	{
		m_scorePointIndexToName.clear();
		CsvReader* pReader = new CsvReader();
		std::string path = (PathUtil::ConcatPath(Root::Instance()->getRootPath(), "Media/Setting/scorepoint.csv")).c_str();;

		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordLogError("Can not open score point setting file %s", path.c_str());
			LordSafeDelete(pReader);
			return false;
		}

		int scoreId;
		String scoreName;
		pReader->Read(); // discard the second line
		while (pReader->Read())
		{
			pReader->GetInt("ScoreId", scoreId);
			pReader->GetString("ScoreName", scoreName);
			m_scorePointIndexToName[scoreId] = scoreName;

			if (scoreId > m_scorePointTotal)
				m_scorePointTotal = scoreId;
		}

		pReader->Close();
		LordSafeDelete(pReader);
		return true;
	}

	void ScorePointSetting::unloadSetting()
	{
		m_scorePointIndexToName.clear();
	}

	String ScorePointSetting::getScorePointName(int scorePointIndex)
	{
		String errorName("unknown ScorePoint");
		if (scorePointIndex < 1 || scorePointIndex > m_scorePointTotal)
			return errorName;

		auto iter = m_scorePointIndexToName.find(scorePointIndex);
		if (iter == m_scorePointIndexToName.end())
			return errorName;

		return LanguageManager::Instance()->getString(iter->second);
	}
}


