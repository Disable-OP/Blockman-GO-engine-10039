#include "GUIShopGridView.h"
#include "UI/GUIWindowManager.h"
#include "GUI/GuiDef.h"
#include "GUI/GUIShopGoods.h"
#include "Math/LordMath.h"

namespace BLOCKMAN
{
	GuiShopGridView::GuiShopGridView(GUIWindowType nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_gridView = dynamic_cast<GUIGridView*>(GUIWindowManager::Instance()->CreateGUIWindow(GWT_GRID_VIEW, "GridView"));
		renameComponents(m_gridView);
		m_gridView->SetArea({ 0, 0 }, { 0, 0 }, { 1.0, 0 }, { 1.0, 0 });
		m_gridView->InitConfig(6.0f, 6.0f, 4);
		AddChildWindow(m_gridView);
	}

	void GuiShopGridView::updateGridView(CommodityGroup group, int merchantId)
	{
		clearGridView();
		static unsigned goodsCount = 0;
		vector<Commodity>::type items = group.getCommodities();
		float maxWidth = (m_gridView->GetPixelSize().x - 6.0f * 3) / 4;
		for (Commodity item : items)
		{
			goodsCount++;
			GUIString strTabName = StringUtil::Format("Shop-Content-Goods-Grid-%d", goodsCount).c_str();
			GUIShopGoods* goodsItem = (GUIShopGoods*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_SHOP_GOODS, strTabName);
			goodsItem->SetWidth({ 0, Math::Min(maxWidth, 180.0f) });
			goodsItem->SetHeight({ 0, 228.f });
			goodsItem->setCommodity(item, merchantId, group.getIndex());
			m_gridView->AddItem(goodsItem);
			goodsItem->subscribeEvent("LackOfMoney", std::bind(&GuiShopGridView::onLackOfMoney, this, std::placeholders::_1));
		}
	}

	void GuiShopGridView::clearGridView()
	{
		m_gridView->RemoveAllItems();
		m_gridView->ResetPos();
	}

	void GuiShopGridView::Destroy()
	{
		if (m_gridView)
		{
			RemoveWindowFromDrawList(*m_gridView);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_gridView);
			m_gridView = nullptr;
		}
	}

	void GuiShopGridView::updateUnlockedState()
	{
		int childSize = m_gridView->GetItemCount();
		for (int i = 0; i < childSize; i++)
		{
			GUIShopGoods* goodsItem = dynamic_cast<GUIShopGoods*>(m_gridView->getChildWindow(i));
			if (goodsItem)
			{
				goodsItem->updateUnlockedState();
			}
		}
	}
}
