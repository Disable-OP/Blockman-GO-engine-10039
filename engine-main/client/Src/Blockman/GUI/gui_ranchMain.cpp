#include "gui_ranchMain.h"
#include "Util/LanguageKey.h"
#include "GUI/GuiRanchFunctionItem.h"
#include "Util/RedDotManager.h"

using namespace LORD;

namespace BLOCKMAN
{
	gui_ranchMain::gui_ranchMain()
		: gui_layout("RanchMain.json")
	{
	}

	gui_ranchMain::~gui_ranchMain()
	{
		
	}

	void gui_ranchMain::onLoad()
	{
		getWindow<GuiRanchFunctionItem>("RanchMain-House")->setName(getString(LanguageKey::GUI_RANCH_MAIN_HOUSE ));
		getWindow<GuiRanchFunctionItem>("RanchMain-Storage")->setName(getString(LanguageKey::GUI_RANCH_MAIN_STORAGE));
		getWindow<GuiRanchFunctionItem>("RanchMain-Factory")->setName(getString(LanguageKey::GUI_RANCH_MAIN_FACTORY));
		getWindow<GuiRanchFunctionItem>("RanchMain-Farming")->setName(getString(LanguageKey::GUI_RANCH_MAIN_FARMING));
		getWindow<GuiRanchFunctionItem>("RanchMain-Community")->setName(getString(LanguageKey::GUI_RANCH_MAIN_COMMUNITY));
		getWindow<GuiRanchFunctionItem>("RanchMain-Achievement")->setName(getString(LanguageKey::GUI_RANCH_MAIN_ACHIEVEMENT));

		getWindow<GuiRanchFunctionItem>("RanchMain-House")->subscribeEvent(EventWindowTouchUp, std::bind(&gui_ranchMain::onClick, this, std::placeholders::_1, gui_ranch::ViewId::HOUSE));
		getWindow<GuiRanchFunctionItem>("RanchMain-Storage")->subscribeEvent(EventWindowTouchUp, std::bind(&gui_ranchMain::onClick, this, std::placeholders::_1, gui_ranch::ViewId::STORAGE));
		getWindow<GuiRanchFunctionItem>("RanchMain-Factory")->subscribeEvent(EventWindowTouchUp, std::bind(&gui_ranchMain::onClick, this, std::placeholders::_1, gui_ranch::ViewId::FACTORY));
		getWindow<GuiRanchFunctionItem>("RanchMain-Farming")->subscribeEvent(EventWindowTouchUp, std::bind(&gui_ranchMain::onClick, this, std::placeholders::_1, gui_ranch::ViewId::FARMING));
		getWindow<GuiRanchFunctionItem>("RanchMain-Community")->subscribeEvent(EventWindowTouchUp, std::bind(&gui_ranchMain::onClick, this, std::placeholders::_1, gui_ranch::ViewId::COMMUNITY));
		getWindow<GuiRanchFunctionItem>("RanchMain-Achievement")->subscribeEvent(EventWindowTouchUp, std::bind(&gui_ranchMain::onClick, this, std::placeholders::_1, gui_ranch::ViewId::ACHIEVEMENT));
	}

	void gui_ranchMain::onUpdate(ui32 nTimeElapse)
	{
		if (isShown())
		{
			getWindow<GuiRanchFunctionItem>("RanchMain-House")->setRedPointVisible(RedDotManager::checkHouse());
			getWindow<GuiRanchFunctionItem>("RanchMain-Storage")->setRedPointVisible(RedDotManager::checkStorage());
			getWindow<GuiRanchFunctionItem>("RanchMain-Factory")->setRedPointVisible(RedDotManager::checkFactory());
			getWindow<GuiRanchFunctionItem>("RanchMain-Farming")->setRedPointVisible(RedDotManager::checkFarming());
			getWindow<GuiRanchFunctionItem>("RanchMain-Community")->setRedPointVisible(RedDotManager::checkCommunity());
			getWindow<GuiRanchFunctionItem>("RanchMain-Achievement")->setRedPointVisible(RedDotManager::checkAchievement());
		}
	}

	bool gui_ranchMain::onClick(const EventArgs & events, const gui_ranch::ViewId& viewId)
	{
		getParent()->onClick(viewId);
		return true;
	}
}
