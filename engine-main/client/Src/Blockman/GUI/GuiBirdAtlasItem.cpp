#include "GuiBirdAtlasItem.h"
#include "UI/GUIWindowManager.h"
#include "GUI/GuiBirdPackAndFuseItem.h"
#include "GUI/GuiBirdLotteryItem.h"
#include "GUI/GuiDef.h"

namespace BLOCKMAN
{
	
	void GuiBirdAtlasItem::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	
	vector<GUIWindow*>::type GuiBirdAtlasItem::getPrivateChildren()
	{
		return { m_window };
	}

	GuiBirdAtlasItem::GuiBirdAtlasItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		removeComponents();
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("BirdAtlasItem.json"));
		renameComponents(m_window);
		m_ivTableIcon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0)->GetChildByIndex(1));
		m_itemList = dynamic_cast<GUIListHorizontalBox*>(m_window->GetChildByIndex(1)->GetChildByIndex(1));
		AddWindowToDrawList(*m_window);
	}

	void GuiBirdAtlasItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		BaseClass::UpdateSelf(nTimeElapse);

		if (m_listWidth != m_itemList->GetPixelSize().x)
		{
			m_listWidth = m_itemList->GetPixelSize().x;
			m_itemList->SetInterval((m_listWidth - 729.f) / 8.f);
		}
	}

	
	void GuiBirdAtlasItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GuiBirdAtlasItem::setAtlas(const BirdAtlas & table)
	{
		m_ivTableIcon->SetImage(table.eggIcon.c_str());
		setItems(table.items);
	}

	void GuiBirdAtlasItem::setItems(const std::vector<BirdEgg>& items)
	{
		static int count = 0;
		for (const auto& item : items)
		{
			count++;
			String itemName = String(GetName().c_str()) + "-item-" + StringUtil::ToString(count);
			GuiBirdLotteryItem* itemView = (GuiBirdLotteryItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_BIRD_LOTTERY_ITEM, itemName.c_str());
			itemView->SetArea(UDim(0, 0), UDim(0, 0), UDim(0.0f, 81.f), UDim(0.0f, 85.f));
			itemView->setAtlasItem(item);
			itemView->SetTouchable(false);
			m_itemList->AddItem(itemView, false);
		}
	}
}