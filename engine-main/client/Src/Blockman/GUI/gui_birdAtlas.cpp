#include "gui_birdAtlas.h"
#include "World/World.h"
#include "cWorld/Blockman.h"
#include "Util/LanguageKey.h"
#include "World/BirdSimulator.h"
#include "GUI/GuiBirdAtlasItem.h"
#include "GUI/GuiDef.h"
#include "Util/UICommon.h"
#include "cEntity/EntityPlayerSPMP.h"

namespace BLOCKMAN
{

	gui_birdAtlas::gui_birdAtlas() :
		gui_layout("BirdAtlas.json")
	{
	}

	gui_birdAtlas::~gui_birdAtlas()
	{
	}

	void gui_birdAtlas::onShow()
	{
		if (m_isAtlasUpdate)
		{
			showItemView();
			m_isAtlasUpdate = false;
		}
	}

	void gui_birdAtlas::onLoad()
	{
		setText("BirdAtlas-Title", LanguageKey::GUI_BIRD_ATLAS);
		m_lbDetails = getWindow<GUIListBox>("BirdAtlas-Details");

		getWindow("BirdAtlas-Close")->subscribeEvent(EventButtonClick, std::bind(&gui_birdAtlas::onClose, this, std::placeholders::_1));
		m_subscriptionGuard.add(UpdateBirdAtlasEvent::subscribe(std::bind(&gui_birdAtlas::updateBirdAtlas, this)));
	}

	void gui_birdAtlas::onUpdate(ui32 nTimeElapse)
	{
	}

	bool gui_birdAtlas::updateBirdAtlas()
	{
		m_isAtlasUpdate = true;
		if (isShown())
		{
			onShow();
		}
		return false;
	}

	bool gui_birdAtlas::onClose(const EventArgs & events)
	{
		RootGuiLayout::Instance()->showMainControl();
		playSoundByType(ST_ClosePanel);
		return true;
	}

	void gui_birdAtlas::showItemView()
	{
		m_lbDetails->ClearAllItem();

		if (!UICommon::checkBirdParam())
		{
			return;
		}
		auto eggs = Blockman::Instance()->m_pPlayer->m_birdSimulator->getAtlas();
		static int count = 0;
		for (auto egg : eggs)
		{
			count++;
			String itemTableViewName = StringUtil::Format("BirdAtlas-Details-Item-%d", count).c_str();
			GuiBirdAtlasItem* itemTableView = (GuiBirdAtlasItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_BIRD_ATLAS_ITEM, itemTableViewName.c_str());
			itemTableView->SetArea(UDim(0, 0.f), UDim(0, 0), UDim(1.f, 0.f), UDim(0.0f, 133.f));
			itemTableView->SetTouchable(false);
			itemTableView->setAtlas(egg);
			m_lbDetails->AddItem(itemTableView, false);
		}

	}

}
