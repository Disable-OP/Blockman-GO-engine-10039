#include "gui_chest.h"
#include "GuiScrollableInventory.h"

#include "cEntity/EntityPlayerSPMP.h"
#include "cWorld/Blockman.h"

#include "Inventory/InventoryPlayer.h"
#include "Inventory/IInventory.h"
#include "Inventory/InventoryEnderChest.h"
#include "Inventory/InventoryLargeChest.h"

#include "UI/GUIButton.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIProgressBar.h"
#include "Util/LanguageKey.h"
#include "Network/ClientNetwork.h"
#include "Network/ClientPacketSender.h"
#include "GUI/GuiDef.h"
#include "Entity/Enchantment.h"

namespace BLOCKMAN {

	gui_chest::gui_chest() :
		gui_layout("Chest.json")
	{
		//IInventory* upper = LordNew InventoryBasic("upper", "upperChest", 36);
		//IInventory* lower = LordNew InventoryBasic("lower", "lowerChest", 18);
		//m_chestInventory = LordNew InventoryBasic("upper", "upperChest", 36);
		m_chestInventory = nullptr;
	}

	gui_chest::~gui_chest()
	{
//        LordDelete(m_btnQuit);
//        LordDelete(m_itemName);
//        LordDelete(m_itemProgressBar);
//        LordDelete(m_playerGuiScrollableInventory);
//        LordDelete(m_chestGuiScrollableInventory);
		m_chestInventory = nullptr;
	}

	void gui_chest::onLoad()
	{
		m_rootWindow->SetTouchable(true);
		m_btnQuit = getWindow<GUIButton>("Chest-Btn-Quit");
		m_itemName = getWindow<GUIStaticText>("Chest-Item-Name");
		m_itemProgressBar = getWindow<GUIProgressBar>("Chest-Item-Progress");
		m_btnQuit->subscribeEvent(EventButtonClick, std::bind(&gui_chest::btnQuitClick, this, std::placeholders::_1));
		m_playerGuiScrollableInventory = getWindow<GuiScrollableInventory>("Chest-Content-Inventory");
		m_chestGuiScrollableInventory = getWindow<GuiScrollableInventory>("Chest-Content-Chest");

		getWindow<GUIStaticText>("Chest-Content-Inventory-Title")->SetText(getString(LanguageKey::WORDART_INVENTORY_TABS));
		getWindow<GUIStaticText>("Chest-Content-Chest-Title")->SetText(getString(LanguageKey::WORDART_CHEST));

		m_chestGuiScrollableInventory->subscribeEvent(GuiScrollableInventory::ITEM_STACK_TOUCH_DOWN.c_str(), SubscriberSlot(&gui_chest::onChestInventoryItemStackTouchDown, this));
		m_chestGuiScrollableInventory->subscribeEvent(GuiScrollableInventory::ITEM_STACK_TOUCH_UP.c_str(), SubscriberSlot(&gui_chest::onChestInventoryItemStackTouchUp, this));
		m_chestGuiScrollableInventory->subscribeEvent(GuiScrollableInventory::ITEM_STACK_LONG_TOUCH_START.c_str(), SubscriberSlot(&gui_chest::onChestInventoryItemStackLongTouchStart, this));
		m_chestGuiScrollableInventory->subscribeEvent(GuiScrollableInventory::ITEM_STACK_LONG_TOUCH_END.c_str(), SubscriberSlot(&gui_chest::onChestInventoryItemStackLongTouchEnd, this));

		m_playerGuiScrollableInventory->subscribeEvent(GuiScrollableInventory::ITEM_STACK_TOUCH_DOWN.c_str(), SubscriberSlot(&gui_chest::onPlayerInventoryItemStackTouchDown, this));
		m_playerGuiScrollableInventory->subscribeEvent(GuiScrollableInventory::ITEM_STACK_TOUCH_UP.c_str(), SubscriberSlot(&gui_chest::onPlayerInventoryItemStackTouchUp, this));
		m_playerGuiScrollableInventory->subscribeEvent(GuiScrollableInventory::ITEM_STACK_LONG_TOUCH_START.c_str(), SubscriberSlot(&gui_chest::onPlayerInventoryItemStackLongTouchStart, this));
		m_playerGuiScrollableInventory->subscribeEvent(GuiScrollableInventory::ITEM_STACK_LONG_TOUCH_END.c_str(), SubscriberSlot(&gui_chest::onPlayerInventoryItemStackLongTouchEnd, this));
	}

	void gui_chest::onUpdate(ui32 nTimeElapse)
	{
		m_animator.update(nTimeElapse);

		if (m_chestInventory == nullptr) return;
		auto playerInventory = Blockman::Instance()->m_pPlayer->inventory;
		m_playerGuiScrollableInventory->setInventory(playerInventory->getMainInventory());
		m_chestGuiScrollableInventory->setInventory(m_chestInventory);
		//判断是否展示物品名称
		m_isShowItemName = m_isShowItemName ? m_itemNameShowTime < 2 * 1000 : false;
		m_itemNameShowTime = m_isShowItemName ? (m_itemNameShowTime < 2 * 1000 ? m_itemNameShowTime + nTimeElapse : 0) : 0;
		m_itemName->SetVisible(m_isShowItemName);

		if (m_isShowItemProgress && m_itemStackSize != 0)
		{
			m_touchTime = m_touchTime + nTimeElapse;
			m_exchangeItemNumber = (m_touchTime / 25);
			m_exchangeItemNumber = m_exchangeItemNumber > m_itemStackSize ? m_itemStackSize : m_exchangeItemNumber;
			m_itemProgressBar->SetProgress(((float)m_exchangeItemNumber) / m_itemStackSize);
		}
		m_itemProgressBar->SetVisible(m_isShowItemProgress);

		// 玩家离开箱子范围 关闭箱子和UI
		if (getParent()->isChestInventoryShown())
		{
			Vector3 m_pPlayer_pos = Blockman::Instance()->m_pPlayer->position;
			float dis = m_pPlayer_pos.distanceTo(blockPos);
			if (dis > m_closeChestDis)
			{
				m_chestInventory = nullptr;
				getParent()->showChestManage(false);
			}
		}
	}

	bool gui_chest::onPlayerInventoryItemStackTouchDown(const EventArgs & event)
	{
		auto playerInventory = Blockman::Instance()->m_pPlayer->inventory;
		showItemName(getClickItem(playerInventory, event));
		return true;
	}

	bool gui_chest::onChestInventoryItemStackTouchDown(const EventArgs & event)
	{
		showItemName(getClickItem(m_chestInventory, event));
		return true;
	}

	bool gui_chest::onPlayerInventoryItemStackTouchUp(const EventArgs & event)
	{
		m_exchangeItemNumber = 1;
		//exchangeItem(Blockman::Instance()->m_pPlayer->inventory, m_chestInventory, event);
		exchangeItem(true, event, 0);
		return true;
	}

	bool gui_chest::onChestInventoryItemStackTouchUp(const EventArgs & event)
	{
		m_exchangeItemNumber = 1;
		//exchangeItem(m_chestInventory, Blockman::Instance()->m_pPlayer->inventory, event);
		exchangeItem(false, event, 0);
		return true;
	}

	bool gui_chest::onPlayerInventoryItemStackLongTouchStart(const EventArgs & event)
	{
		m_touchTime = 75;
		m_isShowItemProgress = getClickItem(Blockman::Instance()->m_pPlayer->inventory, event) == nullptr ? false : true;
		return true;
	}

	bool gui_chest::onPlayerInventoryItemStackLongTouchEnd(const EventArgs & event)
	{
		//exchangeItem(Blockman::Instance()->m_pPlayer->inventory, m_chestInventory, event);
		exchangeItem(true, event, m_touchTime);
		return true;
	}

	bool gui_chest::onChestInventoryItemStackLongTouchStart(const EventArgs & event)
	{
		m_touchTime = 75;
		m_isShowItemProgress = getClickItem(m_chestInventory, event) == nullptr ? false : true;
		return true;
	}

	bool gui_chest::onChestInventoryItemStackLongTouchEnd(const EventArgs & event)
	{
		//exchangeItem(m_chestInventory, Blockman::Instance()->m_pPlayer->inventory, event);
		exchangeItem(false, event, m_touchTime);
		return true;
	}

	bool gui_chest::btnQuitClick(const EventArgs)
	{
		//m_chestInventory = LordNew InventoryBasic("upper", "upperChest", 36);
		m_chestInventory = nullptr;
		getParent()->showChestManage(false);
		return true;
	}

	void gui_chest::exchangeItem(bool isPlayerInventoryToChest, const EventArgs & event, i32 touchTime)
	{
		int index = dynamic_cast<const GuiScrollableInventory::ItemStackTouchEventArgs&>(event).index;
		LordLogInfo("exchangeItem touchTime[%d]", touchTime);
		ClientNetwork::Instance()->getSender()->sendExchangeItemStack(isPlayerInventoryToChest, touchTime, index, blockPos, face, hisPos);
		m_isShowItemProgress = false;
		m_exchangeItemNumber = 0;
		m_itemStackSize = 0;
		m_touchTime = 0;

		if (isPlayerInventoryToChest)
		{
			m_animator.startAnimation(m_playerGuiScrollableInventory, m_chestGuiScrollableInventory, index);
		}
		else
		{
			m_animator.startAnimation(m_chestGuiScrollableInventory, m_playerGuiScrollableInventory, index);
		}
	}

	void gui_chest::showItemName(ItemStackPtr itemStack)
	{
		if (!itemStack)
		{
			return;
		}

		StringStream ss;
		ss << LanguageManager::Instance()->getItemName(itemStack->getItemName());
		auto enchantmentList = itemStack->getEnchantmentTagList();
		if (enchantmentList && enchantmentList->tagCount() > 0)
		{
			for (int i = 0; i < enchantmentList->tagCount(); ++i)
			{
				auto id = static_cast<NBTTagCompound*>(enchantmentList->tagAt(i))->getShort("id");
				auto level = static_cast<NBTTagCompound*>(enchantmentList->tagAt(i))->getShort("lvl");
				auto enchantment = Enchantment::enchantmentsList[id];
				ss << "#n"
					<< LanguageManager::Instance()->getString(enchantment->getName())
					<< ' '
					<< LanguageManager::Instance()->getString("enchantment.level." + StringUtil::ToString(level));
			}
		}

		auto displayString = ss.str();
		StringUtil::Trim(displayString);
		m_itemName->SetText(displayString.c_str());
		m_isShowItemName = true;
		m_itemNameShowTime = 0;
		m_itemStackSize = itemStack->stackSize;
	}

	ItemStackPtr gui_chest::getClickItem(IInventory* inventory, const EventArgs & event)
	{
		return inventory ? inventory->getStackInSlot(dynamic_cast<const GuiScrollableInventory::ItemStackTouchEventArgs&>(event).index) : nullptr;
	}

	int ChestItemExchangeAnimator::getDestinationIndex(IInventory * targetInventory, int itemId, int meta)
	{
		ItemStackPtr targetStack = targetInventory->findCanSaveItemStackById(itemId, meta);
		int index;
		if (targetStack && targetStack->stackSize < targetStack->getMaxStackSize())
		{
			index = targetInventory->findItemStack(targetStack);
		}
		else
		{
			for (index = 0; index < targetInventory->getSizeInventory(); index++)
			{
				if (!targetInventory->getStackInSlot(index))
				{
					break;
				}
			}
		}
		return index;
	}

	GuiItemStack* ChestItemExchangeAnimator::cloneGuiItemStack(GuiScrollableInventory* scrollableInventory, size_t index)
	{
		GuiItemStack* newGuiItemStack;
		if (m_guiItemStackPool.empty())
		{
			static size_t count = 0;
			newGuiItemStack = scrollableInventory->cloneGuiItemStack(index, StringUtil::Format("MovingGuiItemStack-%u", count++).c_str());
		}
		else
		{
			newGuiItemStack = m_guiItemStackPool.back();
			m_guiItemStackPool.pop_back();
			newGuiItemStack->setItemStack(scrollableInventory->getItemStack(index));
		}
		newGuiItemStack->setNumberVisibility(GuiItemStack::NumberVisibility::NEVER_VISIBLE);
		newGuiItemStack->SetPosition(scrollableInventory->getGuiItemStackPosition(index));
		newGuiItemStack->SetHorizontalAlignment(HA_LEFT);
		newGuiItemStack->SetVerticalAlignment(VA_TOP);
		GUISystem::Instance()->GetRootWindow()->AddChildWindow(newGuiItemStack);
		return newGuiItemStack;
	}

	ChestItemExchangeAnimator::ChestItemExchangeAnimator()
	{
		m_animator.onFinish([this](GUIWindow* window)
		{
			if (auto guiItemStack = dynamic_cast<GuiItemStack*>(window))
			{
				releaseGuiItemStack(guiItemStack);
			}
			else
			{
				LordLogError("Animated GUIWindow is not a GuiItemStack");
			}
		});
	}

	ChestItemExchangeAnimator::~ChestItemExchangeAnimator()
	{
		for (auto guiItemStack : m_guiItemStackPool)
		{
			GUIWindowManager::Instance()->DestroyGUIWindow(guiItemStack);
		}
	}

	void ChestItemExchangeAnimator::startAnimation(GuiScrollableInventory * source, GuiScrollableInventory * destination, size_t index)
	{
		auto itemStack = source->getInventory()->getStackInSlot(index);
		if (!itemStack)
		{
			return;
		}
		int dstIndex = getDestinationIndex(destination->getInventory(), itemStack->itemID, itemStack->itemDamage);
		if (dstIndex >= destination->getInventory()->getSizeInventory())
		{
			return;
		}
		auto movingGuiItemStack = cloneGuiItemStack(source, index);
		m_animator.startMove(movingGuiItemStack, destination->getGuiItemStackPosition(dstIndex));
	}

}
