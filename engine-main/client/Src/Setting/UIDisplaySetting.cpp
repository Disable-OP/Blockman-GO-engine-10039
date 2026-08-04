#include "UIDisplaySetting.h"
#include "Setting/LogicSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"

namespace BLOCKMAN
{
	UIDisplay* UIDisplaySetting::m_curUIDisplay = NULL;
	std::map<ClientGameType, UIDisplay*>  UIDisplaySetting::m_UIDisplayMap = {};

	bool UIDisplaySetting::loadSetting()
	{
		m_UIDisplayMap.clear();
		UIDisplay* pUIDisplay = NULL;
		CsvReader* pReader = new CsvReader();

		std::string path = (PathUtil::ConcatPath(Root::Instance()->getRootPath(), "Media/Setting/UIDisplay.csv")).c_str();
		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordLogError("Can not open game rule setting file %s", path.c_str());
			return false;
		}
		pReader->Read(); // discard the second line
		while (pReader->Read())
		{
			String gameType;
			pReader->GetString("GameType", gameType);
			ClientGameType type = GameTypeSetting::GetGameType(gameType);
			if (m_UIDisplayMap.find(type) != m_UIDisplayMap.end())
				continue;
			pUIDisplay = new UIDisplay();
			pReader->GetBool("IsShowState", pUIDisplay->isShowState);
			pReader->GetBool("IsShowSneak", pUIDisplay->isShowSneak);
			pReader->GetBool("IsShowGunDetail", pUIDisplay->isShowGunDetail);
			pReader->GetBool("IsShowBag", pUIDisplay->isShowBag);
			pReader->GetBool("IsShowWatchRespawn", pUIDisplay->isShowWatchRespawn);
			pReader->GetBool("IsShowGetGoodsBg", pUIDisplay->isShowGetGoodsBg);
			pReader->GetBool("IsHidePersonalShop", pUIDisplay->isHidePersonalShop);
			pReader->GetBool("IsShowThrowGun", pUIDisplay->isShowThrowGun);
			pReader->GetBool("IsShowBackHall", pUIDisplay->isShowBackHall);	
			pReader->GetBool("IsRedAimBlock", pUIDisplay->isRedAimBlock);
			pReader->GetBool("IsShowCenterTeamResource", pUIDisplay->isShowCenterTeamResource);
			pReader->GetBool("IsShowPotionHint", pUIDisplay->isShowPotionHint);
			pReader->GetBool("IsShowBuyBullet", pUIDisplay->isShowBuyBullet);
			pReader->GetBool("IsRedAim", pUIDisplay->isRedAim);
			pReader->GetBool("IsShowTeamUI", pUIDisplay->isShowTeamUI);
			
			m_UIDisplayMap[type] = pUIDisplay;
		}

		pReader->Close();
		LordSafeDelete(pReader);
		return true;
	}

	void UIDisplaySetting::unloadSetting()
	{
		for (auto iter = m_UIDisplayMap.begin(); iter != m_UIDisplayMap.end(); iter++)
		{
			delete iter->second;
		}
		m_UIDisplayMap.clear();
		m_curUIDisplay = NULL;
	}

	UIDisplay * UIDisplaySetting::getUIDisplay()
	{
		if (m_curUIDisplay == NULL)
		{
			auto pRule = m_UIDisplayMap.find(LogicSetting::Instance()->getGameType());
			if (pRule == m_UIDisplayMap.end())
			{
				pRule = m_UIDisplayMap.find(ClientGameType::COMMON);
			}
			m_curUIDisplay = pRule->second;
		}
		return m_curUIDisplay;
	}
}