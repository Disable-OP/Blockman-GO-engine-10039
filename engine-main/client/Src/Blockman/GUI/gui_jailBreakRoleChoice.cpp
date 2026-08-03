#include "gui_jailBreakRoleChoice.h"
#include "Util/LanguageKey.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "GUI/GuiActorWindow.h"
#include "Actor/ActorObject.h"

#include "game.h"

using namespace LORD;

namespace BLOCKMAN
{

	gui_jailBreakRoleChoice::gui_jailBreakRoleChoice()
		: gui_layout("JailBreakRoleChoice.json")
	{
	}


	gui_jailBreakRoleChoice::~gui_jailBreakRoleChoice()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_jailBreakRoleChoice::onLoad()
	{
		getWindow("JailBreakRoleChoice-Police-Title-Name")->SetText(getString(LanguageKey::GUI_JAIL_BREAK_ROLE_CHOICE_TITLE_POLICE));
		getWindow("JailBreakRoleChoice-Bandit-Title-Name")->SetText(getString(LanguageKey::GUI_JAIL_BREAK_ROLE_CHOICE_TITLE_BANDIT));
		getWindow("JailBreakRoleChoice-Police-Btn")->SetText(getString(LanguageKey::GUI_JAIL_BREAK_ROLE_CHOICE_ENTER_GAME));
		getWindow("JailBreakRoleChoice-Bandit-Btn")->SetText(getString(LanguageKey::GUI_JAIL_BREAK_ROLE_CHOICE_ENTER_GAME));
		getWindow("JailBreakRoleChoice-Police-Info-Title")->SetText(getString(LanguageKey::GUI_JAIL_BREAK_ROLE_CHOICE_TITLE));
		getWindow("JailBreakRoleChoice-Bandit-Info-Title")->SetText(getString(LanguageKey::GUI_JAIL_BREAK_ROLE_CHOICE_TITLE));

		getWindow("JailBreakRoleChoice-Police-Tip")->SetText(getString(LanguageKey::GUI_JAIL_BREAK_ROLE_CHOICE_POLICE_TIP));
		getWindow("JailBreakRoleChoice-Bandit-Tip")->SetText(getString(LanguageKey::GUI_JAIL_BREAK_ROLE_CHOICE_BANDIT_TIP));

		getWindow("JailBreakRoleChoice-Police-Info-Title-Name")->SetText("");
		getWindow("JailBreakRoleChoice-Bandit-Info-Title-Name")->SetText("");
		getWindow("JailBreakRoleChoice-Police-Info-Currency-Value")->SetText("0");
		getWindow("JailBreakRoleChoice-Bandit-Info-Currency-Value")->SetText("0");

		getWindow("JailBreakRoleChoice-Close")->subscribeEvent(EventButtonClick, std::bind(&gui_jailBreakRoleChoice::onBtnClick, this, std::placeholders::_1, VIewId::BTN_CLOSE));
		getWindow("JailBreakRoleChoice-Police-Btn")->subscribeEvent(EventButtonClick, std::bind(&gui_jailBreakRoleChoice::onBtnClick, this, std::placeholders::_1, VIewId::BTN_POLICE));
		getWindow("JailBreakRoleChoice-Bandit-Btn")->subscribeEvent(EventButtonClick, std::bind(&gui_jailBreakRoleChoice::onBtnClick, this, std::placeholders::_1, VIewId::BTN_BANDIT));
	}

	void gui_jailBreakRoleChoice::onShow()
	{
		getWindow("JailBreakRoleChoice-Close")->SetVisible(!m_choiceRole.forceOpen);

		String defaultIdle = "idle";
		if (Blockman::Instance()->m_pPlayer && Blockman::Instance()->m_pPlayer->m_defaultIdle > 0)
		{
			defaultIdle = "idle_" + StringUtil::Format("%d", Blockman::Instance()->m_pPlayer->m_defaultIdle);
		}

		if (m_choiceRole.type == 0)
		{
			if (Blockman::Instance()->m_pPlayer->m_sex == 2)
			{
				getWindow<GuiActorWindow>("JailBreakRoleChoice-Police-Actor")->SetActor("girl.actor", defaultIdle.c_str());
				getWindow<GuiActorWindow>("JailBreakRoleChoice-Bandit-Actor")->SetActor("girl.actor", defaultIdle.c_str());
			}
			else
			{
				getWindow<GuiActorWindow>("JailBreakRoleChoice-Police-Actor")->SetActor("boy.actor", defaultIdle.c_str());
				getWindow<GuiActorWindow>("JailBreakRoleChoice-Bandit-Actor")->SetActor("boy.actor", defaultIdle.c_str());
			}
		}
		else
		{
			if (!m_choiceRole.police.model.empty())
			{
				getWindow<GuiActorWindow>("JailBreakRoleChoice-Police-Actor")->SetActor(m_choiceRole.police.model.c_str(), defaultIdle.c_str());
			}
			else
			{
				if (Blockman::Instance()->m_pPlayer->m_sex == 2)
				{
					getWindow<GuiActorWindow>("JailBreakRoleChoice-Police-Actor")->SetActor("girl.actor", defaultIdle.c_str());
				}
				else
				{
					getWindow<GuiActorWindow>("JailBreakRoleChoice-Police-Actor")->SetActor("boy.actor", defaultIdle.c_str());
				}
			}

			if (!m_choiceRole.criminal.model.empty())
			{
				getWindow<GuiActorWindow>("JailBreakRoleChoice-Bandit-Actor")->SetActor(m_choiceRole.criminal.model.c_str(), defaultIdle.c_str());
			}
			else
			{
				if (Blockman::Instance()->m_pPlayer->m_sex == 2)
				{
					getWindow<GuiActorWindow>("JailBreakRoleChoice-Bandit-Actor")->SetActor("girl.actor", defaultIdle.c_str());
				}
				else
				{
					getWindow<GuiActorWindow>("JailBreakRoleChoice-Bandit-Actor")->SetActor("boy.actor", defaultIdle.c_str());
				}
			}
		}
		setChoiceRole(m_choiceRole);
	}

	void gui_jailBreakRoleChoice::onUpdate(ui32 nTimeElapse)
	{
	}

	bool gui_jailBreakRoleChoice::onBtnClick(const EventArgs, VIewId viewId)
	{
		switch (viewId)
		{
		case VIewId::BTN_POLICE:
			getParent()->showMainControl();
			GameClient::CGame::Instance()->getNetwork()->getSender()->sendSelectRole(1);
			break;
		case VIewId::BTN_BANDIT:
			getParent()->showMainControl();
			GameClient::CGame::Instance()->getNetwork()->getSender()->sendSelectRole(2);
			break;
		case VIewId::BTN_CLOSE:
			if (m_choiceRole.forceOpen)
				return true;
			getParent()->showMainControl();
			break;
		}
		return true;
	}

	void gui_jailBreakRoleChoice::setChoiceRole(const ChoiceRole & choiceRole)
	{
		this->m_choiceRole = choiceRole;
		changeClothes(true, m_choiceRole.police.actors);
		if (m_choiceRole.criminal.lv == 1)
		{
			changeClothes(false, m_choiceRole.criminal.actors);
		}

		if (m_choiceRole.criminal.title.length() > size_t(0))
			getWindow("JailBreakRoleChoice-Bandit-Title-Name")->SetText(getString(m_choiceRole.criminal.title));
		getWindow("JailBreakRoleChoice-Bandit-Tip")->SetVisible(!m_choiceRole.criminal.canSelect);
		getWindow("JailBreakRoleChoice-Bandit-Btn")->SetEnabled(m_choiceRole.criminal.canSelect);
		getWindow("JailBreakRoleChoice-Bandit-Info-Title-Name")->SetText(getString(m_choiceRole.criminal.name));
		if (m_choiceRole.criminal.money >= 0)
		{
			getWindow("JailBreakRoleChoice-Bandit-Info-Currency-Icon")->SetVisible(true);
			getWindow("JailBreakRoleChoice-Bandit-Info-Currency-Value")->SetText(StringUtil::ToString(m_choiceRole.criminal.money).c_str());
		}
		else
		{
			getWindow("JailBreakRoleChoice-Bandit-Info-Currency-Icon")->SetVisible(false);
			getWindow("JailBreakRoleChoice-Bandit-Info-Currency-Value")->SetVisible(false);
		}

		if (m_choiceRole.police.title.length() > size_t(0))
			getWindow("JailBreakRoleChoice-Police-Title-Name")->SetText(getString(m_choiceRole.police.title));
		getWindow("JailBreakRoleChoice-Police-Tip")->SetVisible(!m_choiceRole.police.canSelect);
		getWindow("JailBreakRoleChoice-Police-Btn")->SetEnabled(m_choiceRole.police.canSelect);
		getWindow("JailBreakRoleChoice-Police-Info-Title-Name")->SetText(getString(m_choiceRole.police.name));
		if (m_choiceRole.police.money >= 0)
		{
			getWindow("JailBreakRoleChoice-Police-Info-Currency-Icon")->SetVisible(true);
			getWindow("JailBreakRoleChoice-Police-Info-Currency-Value")->SetText(StringUtil::ToString(m_choiceRole.police.money).c_str());
		}
		else
		{
			getWindow("JailBreakRoleChoice-Police-Info-Currency-Icon")->SetVisible(false);
			getWindow("JailBreakRoleChoice-Police-Info-Currency-Value")->SetVisible(false);
		}
	}

	void gui_jailBreakRoleChoice::changeClothes(bool isPolice, list<ResInfo>::type resList)
	{
		if (m_choiceRole.type != 0)
			return;
		list<ResInfo>::iterator iter;
		for (iter = resList.begin(); iter != resList.end(); iter++)
		{
			ResInfo info = *iter ;
			if (info.resId > 0 )
			{
				if (isPolice)
				{
					getWindow<GuiActorWindow>("JailBreakRoleChoice-Police-Actor")->UseBodyPart(info.resName, StringUtil::ToString(info.resId));
				}
				else
				{
					getWindow<GuiActorWindow>("JailBreakRoleChoice-Bandit-Actor")->UseBodyPart(info.resName, StringUtil::ToString(info.resId));
				}
			}
		}
		if (Blockman::Instance()->m_pPlayer)
		{
			getWindow<GuiActorWindow>("JailBreakRoleChoice-Police-Actor")->GetActor()->SetCustomColor(Blockman::Instance()->m_pPlayer->m_skinColor);
			getWindow<GuiActorWindow>("JailBreakRoleChoice-Bandit-Actor")->GetActor()->SetCustomColor(Blockman::Instance()->m_pPlayer->m_skinColor);
		}

	}
}
