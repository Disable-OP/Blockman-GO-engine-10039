#pragma once
#ifndef __GUI_CHEST_HEADER__
#define __GUI_CHEST_HEADER__
#include "Core.h"
#include "gui_layout.h"
#include "GuiScrollableInventory.h"
#include "UI/GUIButton.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIProgressBar.h"
#include "Util/ClientEvents.h"
#include "GUI/RootGuiLayout.h"
#include "Inventory/IInventory.h"
#include <atomic>
#include "GUI/GUIWindowMoveAnimator.h"
#include "UI/GUISystem.h"
#include "UI/GUIStaticImage.h"

using namespace LORD;

namespace BLOCKMAN
{
	class ChestItemExchangeAnimator
	{
	private:
		vector<GuiItemStack*>::type m_guiItemStackPool;
		GUIWindowMoveAnimator m_animator;

		static int getDestinationIndex(IInventory * targetInventory, int itemId, int meta);
		GuiItemStack* cloneGuiItemStack(GuiScrollableInventory* scrollableInventory, size_t index);
		void releaseGuiItemStack(GuiItemStack* guiItemStack)
		{
			GUISystem::Instance()->GetRootWindow()->RemoveChildWindow(guiItemStack);
			m_guiItemStackPool.push_back(guiItemStack);
		}
		
	public:
		ChestItemExchangeAnimator();
		~ChestItemExchangeAnimator();
		void startAnimation(GuiScrollableInventory* source, GuiScrollableInventory* destination, size_t index);
		void update(ui64 nTimeElapse)
		{
			m_animator.update(nTimeElapse);
		}
	};

	class gui_chest : public  gui_layout
	{
	public:
		gui_chest();
		virtual ~gui_chest();
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool onPlayerInventoryItemStackTouchDown(const EventArgs& event);
		bool onChestInventoryItemStackTouchDown(const EventArgs& event);
		bool onPlayerInventoryItemStackTouchUp(const EventArgs& event);
		bool onChestInventoryItemStackTouchUp(const EventArgs& event);
		bool onPlayerInventoryItemStackLongTouchStart(const EventArgs& event);
		bool onPlayerInventoryItemStackLongTouchEnd(const EventArgs& event);
		bool onChestInventoryItemStackLongTouchStart(const EventArgs& event);
		bool onChestInventoryItemStackLongTouchEnd(const EventArgs& event);
		void setChestInventory(IInventory* chestInventory, const Vector3i & blockPos, int face, const Vector3 & hisPos) {
			this->m_chestInventory = chestInventory;
			this->face = face;
			this->hisPos = hisPos;
			this->blockPos = blockPos;
		}
	private:
		bool btnQuitClick(const EventArgs);
		ItemStackPtr  getClickItem(IInventory* inventory, const EventArgs& event);
		void exchangeItem(bool isPlayerInventoryToChest, const EventArgs & event,i32 touchTime);
		void showItemName(ItemStackPtr itemStack);
		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}

	private:
		const int m_closeChestDis = 10;
		int m_itemStackSize = 0;
		int m_exchangeItemNumber = 0;
		ui32 m_touchTime = 0;
		ui32 m_itemNameShowTime = 0;
		bool m_isShowItemName = false;
		bool m_isShowItemProgress = false;
		IInventory* m_chestInventory = nullptr;
		GUIButton* m_btnQuit = nullptr;
		GUIStaticText * m_itemName = nullptr;
		GUIProgressBar* m_itemProgressBar = nullptr;
		GuiScrollableInventory* m_playerGuiScrollableInventory = nullptr;
		GuiScrollableInventory* m_chestGuiScrollableInventory = nullptr;

		int face = 0;
		Vector3 hisPos ;
		Vector3i blockPos;
		ChestItemExchangeAnimator m_animator;
	};
}
#endif
