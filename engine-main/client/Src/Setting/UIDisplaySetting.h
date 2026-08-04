#ifndef __UI_DISPLAY_SETTING_HEADER__
#define __UI_DISPLAY_SETTING_HEADER__

#include <map>
#include "Core.h"
#include "Setting/GameTypeSetting.h"

using namespace LORD;

namespace BLOCKMAN
{
	class UIDisplay
	{
	public:
		bool isShowState = true;
		bool isShowSneak = true;
		bool isShowGunDetail = false;
		bool isShowBag = false;
		bool isShowWatchRespawn = false;
		bool isShowGetGoodsBg = true;
		bool isHidePersonalShop = false;
		bool isShowThrowGun = false;
		bool isShowBackHall = false;
		bool isRedAimBlock = true;
		bool isShowCenterTeamResource = true;
		bool isShowPotionHint = true;
		bool isShowBuyBullet = false;
		bool isRedAim = true;
		bool isShowTeamUI = true;
	};

	class UIDisplaySetting
	{
	public:
		static bool	loadSetting();
		static void	unloadSetting();
		static UIDisplay* getUIDisplay();
		static void resetCurUIDisplay() { m_curUIDisplay = NULL; }

	private:
		static UIDisplay* m_curUIDisplay;
		static std::map<ClientGameType, UIDisplay*> m_UIDisplayMap;
	};
}

#endif // !__UI_DISPLAY_SETTING_HEADER__
