#ifndef __SCORE_POINT_DEF__
#define __SCORE_POINT_DEF__
#include <map>
#include "Core.h"
using namespace LORD;

namespace BLOCKMAN
{
	class ScorePointSetting
	{
	public:
		static bool						loadSetting();
		static void						unloadSetting();
		static String					getScorePointName(int scorePointIndex);
		static int						getScorePointCount() { return m_scorePointTotal; }

	private:
		static int								 m_scorePointTotal;
		static std::map<int, String>		     m_scorePointIndexToName;
	};
}

#endif // !__SCORE_POINT_DEF__
