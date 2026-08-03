#include "gui_manor.h"
#include "UI/GUIButton.h"
#include "Util/LanguageKey.h"
#include "gui_manorInfo.h"
#include "gui_manorHouse.h"
#include "gui_manorFurniture.h"
#include "gui_manorMessage.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Object/Root.h"
#include "UI/GUIImageset.h"
#include "UI/GUIImagesetManager.h"

using namespace LORD;

namespace BLOCKMAN
{

	gui_manor::gui_manor()
		: gui_layout("Manor.json")
	{
		m_radioPanelMap[ViewId::INFO] = m_radioPanelMap[ViewId::INFO] ? m_radioPanelMap[ViewId::INFO] : LordNew gui_manorInfo;
		m_radioPanelMap[ViewId::HOUSE] = m_radioPanelMap[ViewId::HOUSE] ? m_radioPanelMap[ViewId::HOUSE] : LordNew gui_manorHouse;
		m_radioPanelMap[ViewId::FURNITURE] = m_radioPanelMap[ViewId::FURNITURE] ? m_radioPanelMap[ViewId::FURNITURE] : LordNew gui_manorFurniture;
		m_radioPanelMap[ViewId::MESSAGE] = m_radioPanelMap[ViewId::MESSAGE] ? m_radioPanelMap[ViewId::MESSAGE] : LordNew gui_manorMessage;
		
	}

	gui_manor::~gui_manor()
	{
		map<ViewId, gui_layout*>::iterator  iter;
		for (iter = m_radioPanelMap.begin(); iter != m_radioPanelMap.end(); iter++)
		{
			LordSafeDelete(iter->second);
		}
	}

	void gui_manor::onLoad()
	{

		m_radioMap[ViewId::INFO] = getWindow<GUIRadioButton>("Manor-Radio-Info");
		m_radioMap[ViewId::HOUSE] = getWindow<GUIRadioButton>("Manor-Radio-House");
		m_radioMap[ViewId::FURNITURE] = getWindow<GUIRadioButton>("Manor-Radio-Furniture");
		m_radioMap[ViewId::MESSAGE] = getWindow<GUIRadioButton>("Manor-Radio-Message");

		getWindow<GUIRadioButton>("Manor-Radio-Info")->SetText(getString(LanguageKey::GUI_MANOR_RADIO_INFO));
		getWindow<GUIRadioButton>("Manor-Radio-House")->SetText(getString(LanguageKey::GUI_MANOR_RADIO_HOUSE));
		getWindow<GUIRadioButton>("Manor-Radio-Furniture")->SetText(getString(LanguageKey::GUI_MANOR_RADIO_FURNITURE));
		getWindow<GUIRadioButton>("Manor-Radio-Message")->SetText(getString(LanguageKey::GUI_MANOR_RADIO_MESSAGE));
		getWindow<GUIButton>("Manor-Title-Btn-Close")->subscribeEvent(EventButtonClick, std::bind(&gui_manor::onClose, this, std::placeholders::_1));

		auto radioPanel = getWindow("Manor-Radio-Panel");
		map<ViewId,GUIRadioButton*>::iterator  iter;
		for (iter = m_radioMap.begin(); iter != m_radioMap.end(); iter++)
		{
			iter->second->subscribeEvent(EventRadioStateChanged, std::bind(&gui_manor::onRadioChange, this, std::placeholders::_1, iter->first));
			if (m_radioPanelMap[iter->first])
			{
				m_radioPanelMap[iter->first]->attachTo(this, radioPanel);
			}
		}
		m_radioMap[ViewId::INFO]->SetSelected(true);

	}

	void gui_manor::onUpdate(ui32 nTimeElapse)
	{
		if (Blockman::Instance()->m_pPlayer)
		{
			String name = StringUtil::Format(getString(LanguageKey::GUI_MANOR_TITLE_NAME).c_str(), Blockman::Instance()->m_pPlayer->getEntityName().c_str());
			getWindow("Manor-Title-Name")->SetText(name.c_str());
		}
	}

	bool gui_manor::onRadioChange(const EventArgs & events, const ViewId radioId)
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
			onRadioSelected(radioId);
		}
		return true;
	}

	bool gui_manor::onClose(const EventArgs & events)
	{
		getParent()->showMainControl();
		return true;
	}

	void gui_manor::openManor()
	{
		m_radioMap[ViewId::INFO]->SetSelected(true);
	}


	void gui_manor::onRadioSelected(const ViewId radioId)
	{
		switch (radioId)
		{
		case ViewId::INFO:
			break;
		case ViewId::HOUSE:
			break;
		case ViewId::FURNITURE:
			break;
		case ViewId::MESSAGE:
			break;
		}
		playSoundByType(ST_Click);
	}

}
