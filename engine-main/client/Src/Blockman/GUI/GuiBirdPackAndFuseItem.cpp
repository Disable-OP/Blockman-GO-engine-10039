#include "GuiBirdPackAndFuseItem.h"
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
#include "GUI/RootGuiLayout.h"

namespace BLOCKMAN
{
	
	void GuiBirdPackAndFuseItem::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	
	vector<GUIWindow*>::type GuiBirdPackAndFuseItem::getPrivateChildren()
	{
		return { m_window };
	}

	GuiBirdPackAndFuseItem::GuiBirdPackAndFuseItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		removeComponents();
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("BirdPackAndFuseItem.json"));
		renameComponents(m_window);

		m_image = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		m_tick = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(1));
		m_greenDot = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(2));
		m_level = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(3));
		m_greenDot->SetVisible(false);
		m_tick->SetVisible(false);

		m_image->subscribeEvent(EventWindowTouchUp, std::bind(&GuiBirdPackAndFuseItem::onClick, this, std::placeholders::_1));
		
		AddWindowToDrawList(*m_window);
	}

	GuiBirdPackAndFuseItem::~GuiBirdPackAndFuseItem()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void GuiBirdPackAndFuseItem::UpdateSelf(ui32 nTimeElapse)
	{
 		const auto& windowSize = GetPixelSize();
 		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
 		m_window->SetPosition(getAbsolutePositionFromRoot());
 		BaseClass::UpdateSelf(nTimeElapse);
		changeImage();
		m_tick->SetVisible(m_isSelect && !m_isShowPackView);
		m_greenDot->SetVisible(m_bird.isCarry);
	}
	
	void GuiBirdPackAndFuseItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GuiBirdPackAndFuseItem::setBirdInfo(BirdInfo data)
	{
		m_bird = data;
		m_level->SetText(StringUtil::Format("Lv %d", data.level).c_str());
		String imageName = StringUtil::Format("set:bird_icon.json image:body_%s", data.bodyId.c_str());
		m_image->SetImage(imageName.c_str());
	}

	void GuiBirdPackAndFuseItem::setSelect(i64 birdId)
	{
		if (m_isShowPackView)
		{
			m_isSelect = (m_bird.id == birdId);
		}
		else
		{
			if (m_bird.id == birdId)
			{
				m_isSelect = true;
			}
		}
	}

	void GuiBirdPackAndFuseItem::unSelect(i64 birdId)
	{
		if (m_bird.id == birdId)
		{
			m_isSelect = false;
		}
	}

	bool GuiBirdPackAndFuseItem::onClick(const EventArgs& args)
	{
		if (RootGuiLayout::Instance()->isShowPack())
		{
			ShowBirdInfoClickEvent::emit(m_bird.id);
		}
		else
		{
			if (m_isSelect)
			{
				unSelect(m_bird.id);
				BirdRemoveFuseItemEvent::emit(m_bird.id);
			}
			else
			{
				BirdFusePutItemEvent::emit(m_bird.id, StringUtil::Format("set:bird_icon.json image:body_%s", m_bird.bodyId.c_str()));
			}
		}
		return true;
	}

	void GuiBirdPackAndFuseItem::changeImage()
	{
		switch (m_bird.quality)
		{
		case 1:
			m_window->SetBackImage(m_isSelect && m_isShowPackView ? "set:bird_pack.json image:item_1_pre" : "set:bird_pack.json image:item_1_nor");
			break;
		case 2:
			m_window->SetBackImage(m_isSelect && m_isShowPackView ? "set:bird_pack.json image:item_2_pre" : "set:bird_pack.json image:item_2_nor");
			break;
		case 3:
			m_window->SetBackImage(m_isSelect && m_isShowPackView ? "set:bird_pack.json image:item_3_pre" : "set:bird_pack.json image:item_3_nor");
			break;
		case 4:
			m_window->SetBackImage(m_isSelect && m_isShowPackView ? "set:bird_pack.json image:item_4_pre" : "set:bird_pack.json image:item_4_nor");
			break;
		}
	}

}