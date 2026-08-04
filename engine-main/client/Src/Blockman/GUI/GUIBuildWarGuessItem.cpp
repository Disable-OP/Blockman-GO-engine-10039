#include "GUIBuildWarGuessItem.h"
#include "UI/GUIWindowManager.h"
#include "GUI/GuiDef.h"
#include "RootGuiLayout.h"
#include "game.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"


using namespace LORD;
namespace BLOCKMAN
{
	vector<GUIWindow*>::type GuiBuildWarGuessItem::getPrivateChildren()
	{
		return { m_window };
	}

	GuiBuildWarGuessItem::GuiBuildWarGuessItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = GUIWindowManager::Instance()->LoadWindowFromJSON("BuildWarGuessItem.json");
		renameComponents(m_window);
		m_HeartImage = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		m_HeartImage->SetVisible(false);
		m_nameTxt = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(1));
		m_roomTxt = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(4));

		m_visitBtn = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(3));
		m_visitBtn->subscribeEvent(EventButtonClick, std::bind(&GuiBuildWarGuessItem::onClickVisit, this, std::placeholders::_1));
		m_visitBtn->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_BUILD_WAR_GUESS_ITEM_VISIT_BUTTON).c_str());

		m_guessBtn = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(5));
		m_guessBtn->subscribeEvent(EventButtonClick, std::bind(&GuiBuildWarGuessItem::onClickGuess, this, std::placeholders::_1));
		m_guessBtn->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_BUILD_WAR_GUESS_ITEM_GUESS_BUTTON).c_str());

		AddWindowToDrawList(*m_window);
	}

	void GuiBuildWarGuessItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		BaseClass::UpdateSelf(nTimeElapse);
	}

	void GuiBuildWarGuessItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GuiBuildWarGuessItem::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
		m_HeartImage = nullptr;
		m_nameTxt = nullptr;
		m_roomTxt = nullptr;
		m_visitBtn = nullptr;
		m_guessBtn = nullptr;
	}

	void GuiBuildWarGuessItem::RefreshItem(bool is_heart, int room, String name, int guess_room_id, ui64 user_id)
	{
		m_nameTxt->SetText(name.c_str());
		m_HeartImage->SetVisible(is_heart);
		m_roomTxt->SetText(StringUtil::ToString(room).c_str());
		
		m_roomId = room;
		m_name = name;
		m_userId = user_id;

		if (guess_room_id > 0)
		{
			if (guess_room_id == m_roomId)
			{
				m_guessBtn->SetVisible(true);
				m_guessBtn->SetEnabled(false);
				m_guessBtn->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_BUILD_WAR_GUESS_ITEM_HAS_GUESS_BUTTON).c_str());
			}
			else
			{
				m_guessBtn->SetVisible(false);
			}
		}
		else
		{
			m_guessBtn->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_BUILD_WAR_GUESS_ITEM_GUESS_BUTTON).c_str());
			m_guessBtn->SetVisible(true);
		}
	}

	bool GuiBuildWarGuessItem::onClickVisit(const EventArgs &)
	{
		SoundSystem::Instance()->playEffectByType(ST_Click);
		ClientNetwork::Instance()->getSender()->sendBuildWarGuessVisit(m_userId);
		return true;
	}

	bool GuiBuildWarGuessItem::onClickGuess(const EventArgs &)
	{
		SoundSystem::Instance()->playEffectByType(ST_Click);
		ShowBuildWarGuessTipDialogEvent::emit(m_roomId, m_name);
		return true;
	}
}
