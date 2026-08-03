#include "gui_ranchFarming.h"
#include "Util/LanguageKey.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "World/Ranch.h"
#include "GUI/GuiRanchBuildItem.h"
#include "GUI/GuiDef.h"
#include "Util/ClientEvents.h"
#include "Util/UICommon.h"
#include "Util/RedDotManager.h"


using namespace LORD;

namespace BLOCKMAN
{
	gui_ranchFarming::gui_ranchFarming()
		: gui_layout("RanchFarming.json")
	{
	}

	gui_ranchFarming::~gui_ranchFarming()
	{
		
	}

	void gui_ranchFarming::onLoad()
	{

		m_radioMap[ViewId::PLANT] = getWindow<GUIRadioButton>("RanchFarming-Tab-Plant");
		m_radioMap[ViewId::RAISE] = getWindow<GUIRadioButton>("RanchFarming-Tab-Raise");

		m_gvPlant = getWindow<GUIGridView>("RanchFarming-Panel-Plant");
		m_lvRaise = getWindow<GUIListHorizontalBox>("RanchFarming-Panel-Raise");

		m_gvPlant->InitConfig(0, 0, 5);

		m_radioPanelMap[ViewId::PLANT] = getWindow<GUIGridView>("RanchFarming-Panel-Plant");
		m_radioPanelMap[ViewId::RAISE] = getWindow<GUIListHorizontalBox>("RanchFarming-Panel-Raise");

		setText(m_radioMap[ViewId::PLANT], "gui_ranch_farming_tab_plant");
		setText(m_radioMap[ViewId::RAISE], "gui_ranch_farming_tab_raise");

		for (auto item : m_radioMap)
		{
			item.second->subscribeEvent(EventRadioStateChanged, std::bind(&gui_ranchFarming::onRadioChange, this, std::placeholders::_1, item.first));
		}
		m_radioMap[ViewId::PLANT]->SetSelected(true);

		m_subscriptionGuard.add(RanchFarmingBuildChangeEvent::subscribe(std::bind(&gui_ranchFarming::onRanchFarmingChange, this, std::placeholders::_1)));
	}

	void gui_ranchFarming::onShow()
	{
		initRaiseItem();
		initPlantItem();
	}

	void gui_ranchFarming::onUpdate(ui32 nTimeElapse)
	{
		if (isShown())
		{
			getWindow("RanchFarming-Tab-Raise-RedDot")->SetVisible(RedDotManager::checkRaise());
			getWindow("RanchFarming-Tab-Plant-RedDot")->SetVisible(RedDotManager::checkPlantById(500000));

			if (m_gvPlantXSize != m_gvPlant->GetPixelSize().x)
			{
				m_gvPlantXSize = m_gvPlant->GetPixelSize().x;
				plantItemSizeChange();
			}
		}
	}

	bool gui_ranchFarming::onRadioChange(const EventArgs & events, ViewId viewId)
	{
		if (m_radioMap[viewId]->IsSelected())
		{
			for (auto item : m_radioPanelMap)
			{
				item.second->SetVisible(item.first == viewId);
			}
			playSoundByType(ST_Click);
		}
		return true;
	}

	bool gui_ranchFarming::onRanchFarmingChange(i32 type)
	{

		if (type == 1)
		{
			m_raiseDataChange = true;
		}

		if (type == 3)
		{
			m_plantDataChange = true;
		}

		if (isShown())
		{
			onShow();
		}
		return false;
	}

	void gui_ranchFarming::initPlantItem()
	{
		if (!UICommon::checkRanchParam() || !m_plantDataChange)
		{
			return;
		}
		float plantOffset = m_gvPlant->GetScrollOffset();
		m_gvPlant->RemoveAllItems();
		auto items = Blockman::Instance()->m_pPlayer->m_ranch->getPlantBuild();
		plantOffset = items.size() == 0 ? 0.f : plantOffset;
		for (auto item : items)
		{
			static unsigned count = 0;
			count++;
			String itemLayout = StringUtil::Format("RanchFarming-Panel-Plant-Item-%d", count).c_str();
			if (GUIWindowManager::Instance()->GetGUIWindow(itemLayout.c_str()))
			{
				return;
			}
			float w = m_gvPlant->GetPixelSize().x / 5;
			float h = m_gvPlant->GetPixelSize().y / 2;
			GuiRanchBuildItem* itemView = (GuiRanchBuildItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_RANCH_BUILD_ITEM, itemLayout.c_str());
			itemView->initUI("RanchSeedItem.json");
			itemView->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(0.0f, w), UDim(0, h));
			itemView->SetTouchable(false);
			itemView->setType(3);
			itemView->setBuild(item);
			itemView->SetTouchable(false);
			m_gvPlant->AddItem(itemView);
		}
		m_gvPlant->SetScrollOffset(plantOffset);
		m_plantDataChange = false;
	}

	void gui_ranchFarming::initRaiseItem()
	{
		if (!UICommon::checkRanchParam() || !m_raiseDataChange)
		{
			return;
		}
		float raiseOffset = m_lvRaise->GetScrollOffset();
		m_lvRaise->ClearAllItem();
		auto items = Blockman::Instance()->m_pPlayer->m_ranch->getFarmingBuild();
		raiseOffset = items.size() == 0 ? 0.f : raiseOffset;
		for (auto item : items)
		{
			static unsigned count = 0;
			count++;
			String itemLayout = StringUtil::Format("RanchFarming-Panel-Raise-Item-%d", count).c_str();
			if (GUIWindowManager::Instance()->GetGUIWindow(itemLayout.c_str()))
			{
				return;
			}
			GuiRanchBuildItem* itemView = (GuiRanchBuildItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_RANCH_BUILD_ITEM, itemLayout.c_str());
			itemView->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(0.0f, 218.f), UDim(1.0f, 0));
			itemView->setType(1);
			itemView->setBuild(item);
			itemView->SetTouchable(false);
			itemView->SetTouchable(false);
			m_lvRaise->AddItem(itemView, false);
		}

		m_lvRaise->SetScrollOffset(raiseOffset);
		m_raiseDataChange = false;
	}

	void gui_ranchFarming::plantItemSizeChange()
	{
		for (size_t i = 0; i < m_gvPlant->GetItemCount(); i++)
		{
			float w = m_gvPlant->GetPixelSize().x / 5;
			float h = m_gvPlant->GetPixelSize().y / 2;
			m_gvPlant->GetItem(i)->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(0.0f, w), UDim(0.0f, h));
		}
	}
}
