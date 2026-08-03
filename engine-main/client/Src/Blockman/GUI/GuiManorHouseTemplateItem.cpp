#include "GuiManorHouseTemplateItem.h"
#include "UI/GUIWindowManager.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Util/ClientEvents.h"

namespace BLOCKMAN
{
	void GuiManorHouseTemplateItem::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	vector<GUIWindow*>::type GuiManorHouseTemplateItem::getPrivateChildren()
	{
		return { m_window };
	}

	GuiManorHouseTemplateItem::GuiManorHouseTemplateItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("ManorHouseTemplateItem.json"));
		renameComponents(m_window);
		m_templateIcon = dynamic_cast<GUIStaticImage*>( m_window->GetChildByIndex(0));
		m_templateLockIcon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(1));
		m_templateCanUnlockIcon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(2));
		m_window->subscribeEvent(EventWindowTouchUp, std::bind(&GuiManorHouseTemplateItem::onClick, this, std::placeholders::_1));
		AddWindowToDrawList(*m_window);
	}

	void GuiManorHouseTemplateItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		BaseClass::UpdateSelf(nTimeElapse);
		if (m_manorHouse && Blockman::Instance()->m_pPlayer->m_manor->getInfo())
		{
			m_window->SetBackImage(m_manorHouse->isSelect ? "set:manor_atlas.json image:manor_item_pre" : "set:manor_atlas.json image:manor_item_nor");
			m_templateLockIcon->SetVisible(m_manorHouse->unlockLevel > Blockman::Instance()->m_pPlayer->m_manor->getInfo()->level);
			m_templateIcon->SetImage(m_manorHouse->icon.c_str());
			m_templateCanUnlockIcon->SetVisible(!m_manorHouse->isBuy);
		}
	}

	void GuiManorHouseTemplateItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	bool GuiManorHouseTemplateItem::onClick(const EventArgs& events)
	{
		ManorHouseSelectEvent::emit(m_manorHouse);
		return true;
	}
}