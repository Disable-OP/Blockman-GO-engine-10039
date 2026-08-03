#include "ActionStateSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"

namespace BLOCKMAN
{
	std::map<i64, String>      ActionStateSetting::m_stateToSkill = {};

	bool ActionStateSetting::loadSetting()
	{
		m_stateToSkill.clear();
		CsvReader* pReader = new CsvReader();
		std::string path = (PathUtil::ConcatPath(Root::Instance()->getRootPath(), "Media/Setting/actionstate.csv")).c_str();;

		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordLogError("Can not open action state setting file %s", path.c_str());
			LordSafeDelete(pReader);
			return false;
		}

		int actionState;
		int itemId;
		i64 index;
		int occupation;
		String skillName;
		String itemIdList;
		pReader->Read(); // discard the second line
		while (pReader->Read())
		{
			pReader->GetInt("ActionState", actionState);
			pReader->GetString("SkillName", skillName);
			pReader->GetString("HoldItemId", itemIdList);
			pReader->GetInt("Occupation", occupation);
			auto arr = StringUtil::Split(itemIdList, "#", 50);
			for (auto i = 0; i < (int)arr.size(); i++)
			{
				itemId = StringUtil::ParseI32(arr[i]);
				index = getIndex(actionState, itemId, occupation);
				m_stateToSkill[index] = skillName;
			}
		}

		pReader->Close();
		LordSafeDelete(pReader);
		return true;
	}

	void ActionStateSetting::unloadSetting()
	{
		m_stateToSkill.clear();
	}

	String ActionStateSetting::getSkillName(int state, int itemId, int occupation)
	{
		i64 index = getIndex(state, itemId, occupation);
		auto iter = m_stateToSkill.find(index);
		if (iter != m_stateToSkill.end())
		{
			return iter->second;
		}

		index = getIndex(state, itemId, 0);
		iter = m_stateToSkill.find(index);
		if (iter != m_stateToSkill.end())
		{
			return iter->second;
		}

		index = getIndex(state, 0, 0);
		iter = m_stateToSkill.find(index);
		if (iter != m_stateToSkill.end())
		{
			return iter->second;
		}

		return StringUtil::BLANK;
	}
	i64 ActionStateSetting::getIndex(int state, int itemId, int occupation)
	{
		int16_t as_16 = (int16_t)state;
		int16_t item_16 = (int16_t)itemId;
		int64_t occ_16 = (int64_t) occupation;
		i64 index = (as_16 << 16) + item_16 + (occ_16 << 32);
		return index;
	}
}


