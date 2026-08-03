#include "gui_countdown.h"
#include "game.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "Setting/GameTypeSetting.h"

namespace BLOCKMAN
{

	gui_countdown::gui_countdown() : gui_layout("Countdown.json")
	{
	}

	gui_countdown::~gui_countdown()
	{
	}

	void gui_countdown::onLoad()
	{
		m_lastTime = getWindow<GUIStaticText>("Countdown-Last-Time");
		m_tip = getWindow<GUIStaticText>("Countdown-Respawn");
		m_tip->SetText(LanguageManager::Instance()->getString(LanguageKey::WAIT_REBIRTH).c_str());
	}

	void gui_countdown::onUpdate(ui32 nTimeElapse)
	{
		lastTime = lastTime > 0 ? lastTime - nTimeElapse : 0;
		if (lastTime > 0)
		{
			m_lastTime->SetText(StringUtil::ToString((lastTime / 1000) + 1).c_str());
			canRespawn = true;
		}
		else
		{
			if (canRespawn)
			{
				if (GameClient::CGame::Instance()->GetGameType() != ClientGameType::BirdSimulator)
				{
					GameClient::CGame::Instance()->getNetwork()->getSender()->sendRebirth();
				}
				getParent()->hideCountdown();
				canRespawn = false;
			}
		}
	}

	void gui_countdown::setCountdown(i8 second)
	{
		if (second == 0)
		{
			canRespawn = true;
			return;
		}
		lastTime = second * 1000;
	}

}
