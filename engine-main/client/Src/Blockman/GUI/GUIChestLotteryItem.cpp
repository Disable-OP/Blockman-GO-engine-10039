#include "GUIChestLotteryItem.h"
#include "UI/GUIWindowManager.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIButton.h"
#include "Util/ClientEvents.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "Audio/SoundSystem.h"
#include "Audio/SoundDef.h"

namespace BLOCKMAN
{
	GUIChestLotteryItem::GUIChestLotteryItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("ChestLotteryItem.json"));
		renameComponents(m_window);
		initView();
		AddWindowToDrawList(*m_window);
	}

	void GUIChestLotteryItem::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	void GUIChestLotteryItem::initView()
	{
		m_key_icon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(2));	
		auto Question = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(4));
		Question->subscribeEvent(EventButtonClick, std::bind(&GUIChestLotteryItem::onQuestionClick, this, std::placeholders::_1));
		auto Open = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(5));
		Open->subscribeEvent(EventButtonClick, std::bind(&GUIChestLotteryItem::onOpenClick, this, std::placeholders::_1));
	}

	bool GUIChestLotteryItem::onQuestionClick(const EventArgs & events)
	{
		SoundSystem::Instance()->playEffectByType(ST_Click);
		ShowLotteryChestDetailEvent::emit((int)m_data->Type);
		return true;
	}

	bool GUIChestLotteryItem::onOpenClick(const EventArgs & events)
	{
		if (m_data == nullptr)
			return false;
		SoundSystem::Instance()->playEffectByType(ST_Click);
		OpenLotteryChestEvent::emit(m_data->Id);
		return true;
	}

	void GUIChestLotteryItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x }, { 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		BaseClass::UpdateSelf(nTimeElapse);
		if (m_data != nullptr && m_data->HasChange)
		{
			SetLotteryChest(m_data);
		}
	}

	void GUIChestLotteryItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GUIChestLotteryItem::SetLotteryChest(LotteryChest * data)
	{
		if (data == nullptr)
			return;
		m_data = data;
		m_data->HasChange = false;
		auto Mat = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		Mat->SetImage(data->MatImage.c_str());
		auto Integral = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(1)->GetChildByIndex(0));
		Integral->SetText(StringUtil::ToString(data->AddIntegral).c_str());
		m_window->GetChildByIndex(2)->SetVisible(m_data->HasNum <= 0);
		auto KeyNum = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(2)->GetChildByIndex(0));
		KeyNum->SetText(StringUtil::ToString(data->NeedKey).c_str());
		auto Image = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(3));
		Image->SetNormalImage(data->Image.c_str());
		Image->SetPushedImage(data->Image.c_str());
		auto Open = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(5));
		if (m_data->HasNum > 0)
			Open->SetText(StringUtil::Format("%s(%d)", LanguageManager::Instance()->getString(LanguageKey::GUI_CHEST_LOTTERY_OPEN).c_str(), m_data->HasNum).c_str());
		else
			Open->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_CHEST_LOTTERY_OPEN).c_str());
	}

}
