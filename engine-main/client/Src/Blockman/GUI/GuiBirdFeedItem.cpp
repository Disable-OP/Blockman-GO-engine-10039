#include "GuiBirdFeedItem.h"
#include "UI/GUIWindowManager.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Network/ClientNetwork.h"
#include "Inventory/Wallet.h"
#include "Util/ClientEvents.h"
#include "Util/UICommon.h"
#include "Util/RedDotManager.h"
#include "GUI/GuiBirdPackAndFuseItem.h"

namespace BLOCKMAN
{
	
	void GuiBirdFeedItem::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	
	vector<GUIWindow*>::type GuiBirdFeedItem::getPrivateChildren()
	{
		return { m_window };
	}

	GuiBirdFeedItem::GuiBirdFeedItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		removeComponents();
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("BirdFeedItem.json"));
		renameComponents(m_window);

		m_image = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0)->GetChildByIndex(0));
		m_num = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(0)->GetChildByIndex(1));
		m_des = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(1));

		m_image->subscribeEvent(EventWindowTouchUp, std::bind(&GuiBirdFeedItem::onClick, this, std::placeholders::_1));
		AddWindowToDrawList(*m_window);
	}

	void GuiBirdFeedItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		BaseClass::UpdateSelf(nTimeElapse);
		dynamic_cast<GUILayout*>(m_window->GetChildByIndex(0))->SetBackImage(m_food.id == m_selectFoodId ? "set:bird_pack.json image:item_pre" : "set:bird_pack.json image:item");
	}

	
	void GuiBirdFeedItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	bool GuiBirdFeedItem::onClick(const EventArgs & args)
	{
		BirdFoodItemClickEvent::emit(m_food.id, m_food.num);
		return true;
	}

	void GuiBirdFeedItem::setFood(BirdFood food)
	{
		m_food = food;
		m_image->SetImage(m_food.icon.c_str());
		m_num->SetText(StringUtil::Format("X%d", m_food.num).c_str());
		m_des->SetText(LanguageManager::Instance()->getString(m_food.desc).c_str());
	}

}