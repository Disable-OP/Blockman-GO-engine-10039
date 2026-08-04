#include "gui_ranchHouse.h"
#include "Util/LanguageKey.h"
#include "GUI/gui_ranchHousePanel.h"
#include "GUI/gui_ranchSciencePanel.h"

using namespace LORD;

namespace BLOCKMAN
{
	gui_ranchHouse::gui_ranchHouse()
		: gui_layout("RanchHouse.json")
	{
		m_radioPanelMap[ViewId::PANEL_HOUSE] = m_radioPanelMap[ViewId::PANEL_HOUSE] ? m_radioPanelMap[ViewId::PANEL_HOUSE] : LordNew gui_ranchHousePanel;
		m_radioPanelMap[ViewId::PANEL_SCIENCE] = m_radioPanelMap[ViewId::PANEL_SCIENCE] ? m_radioPanelMap[ViewId::PANEL_SCIENCE] : LordNew gui_ranchSciencePanel;
	}

	gui_ranchHouse::~gui_ranchHouse()
	{
		map<ViewId, gui_layout*>::iterator  iter;
		for (iter = m_radioPanelMap.begin(); iter != m_radioPanelMap.end(); iter++)
		{
			LordSafeDelete(iter->second);
		}
	}

	void gui_ranchHouse::onLoad()
	{
		m_radioMap[ViewId::PANEL_HOUSE] = getWindow<GUIRadioButton>("RanchHouse-Tab-House");
		m_radioMap[ViewId::PANEL_SCIENCE] = getWindow<GUIRadioButton>("RanchHouse-Tab-Science");

		m_radioMap[ViewId::PANEL_HOUSE]->SetText(getString(LanguageKey::GUI_RANCH_HOUSE_REBUILD));
		m_radioMap[ViewId::PANEL_SCIENCE]->SetText(getString(LanguageKey::GUI_RANCH_HOUSE_SCIENCE));


		auto radioPanel = getWindow("RanchHouse-Panel");
		map<ViewId, GUIRadioButton*>::iterator  iter;
		for (iter = m_radioMap.begin(); iter != m_radioMap.end(); iter++)
		{
			iter->second->subscribeEvent(EventRadioStateChanged, std::bind(&gui_ranchHouse::onRadioChange, this, std::placeholders::_1, iter->first));
			if (m_radioPanelMap[iter->first])
			{
				m_radioPanelMap[iter->first]->attachTo(this, radioPanel);
			}
		}
		m_radioMap[ViewId::PANEL_HOUSE]->SetSelected(true);
	}

	void gui_ranchHouse::onHide()
	{
		for (auto item : m_radioPanelMap)
		{
			if (item.second)
			{
				item.second->hide();
			}
		}
	}

	void gui_ranchHouse::onShow()
	{
		for (auto item : m_radioMap)
		{
			if (item.second->IsSelected() && m_radioPanelMap[item.first]) {
				m_radioPanelMap[item.first]->show();
				break;
			}
		}
	}

	void gui_ranchHouse::onUpdate(ui32 nTimeElapse)
	{

	}

	bool gui_ranchHouse::onRadioChange(const EventArgs & events, const ViewId radioId)
	{
		if (m_radioMap[radioId]->IsSelected())
		{
			map<ViewId, GUIRadioButton*>::iterator  iter;
			for (iter = m_radioMap.begin(); iter != m_radioMap.end(); iter++)
			{

				if (m_radioPanelMap[iter->first] && m_radioPanelMap[iter->first]->isShown())
				{
					m_radioPanelMap[iter->first]->hide();
				}
			}
			if (m_radioPanelMap[radioId])
			{
				m_radioPanelMap[radioId]->show();
			}
			playSoundByType(ST_Click);
		}
		return true;
	}

	bool gui_ranchHouse::onClick(const EventArgs & events, ViewId viewId)
	{

		switch (viewId)
		{
		case ViewId::BTN_REMOVE:
			break;
		}
		return true;
	}
}
