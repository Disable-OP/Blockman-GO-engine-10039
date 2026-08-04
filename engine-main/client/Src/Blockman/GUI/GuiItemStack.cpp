#include "GuiItemStack.h"

#include "cBlock/cBlockManager.h"
#include "cBlock/cBlock.h"
#include "cItem/cItem.h"
#include "Render/RenderBlocks.h"
#include "Render/TextureAtlas.h"
#include "GUI/GuiDef.h"

#include "UI/GUIWindowManager.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "Item/Ingredient.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Setting/LogicSetting.h"
#include "game.h"
#include "Util/UICommon.h"

namespace BLOCKMAN
{
	gui_renderItems*  GuiItemStack::pRenderItems = nullptr;

	GuiItemStack::GuiItemStack(const GUIWindowType& nType, const GUIString& strName, ItemStackPtr itemStack) :
		BaseClass(nType, strName),
		m_window(GUIWindowManager::Instance()->LoadWindowFromJSON(JSON_NAME)),
		m_itemStack(itemStack)
	{
		renameComponents(m_window);
		if (!isEditorMode && !itemStack)
		{
			m_window->SetVisible(false);
		}
		m_iconImage = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		m_countText = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(1));
		AddWindowToDrawList(*m_window);
		subscribeToBubbleUpMouseEvents();
	}

	void GuiItemStack::initialize() {
		pRenderItems = LordNew gui_renderItems;
	}

	void GuiItemStack::unInitialize() {
		LordSafeDelete(pRenderItems);
	}

	bool GuiItemStack::SetProperty(const GUIString& strName, const GUIString& strValue)
	{
		if (BaseClass::SetProperty(strName, strValue))
		{
			return true;
		}

		if (strName == GWP_ITEM_STACK_NUMBER_VISIBILITY)
		{
			this->m_numberVisibility = static_cast<NumberVisibility>(StringUtil::ParseInt(strValue.c_str()));
			return true;
		}
		return false;
	}

	bool GuiItemStack::GetProperty(const GUIString& strName, GUIString& strValue)
	{
		if (BaseClass::GetProperty(strName, strValue))
		{
			return true;
		}

		if (strName == GWP_ITEM_STACK_NUMBER_VISIBILITY)
		{
			strValue = StringUtil::ToString(static_cast<int>(this->m_numberVisibility)).c_str();
			return true;
		}
		return false;
	}

	void GuiItemStack::GetPropertyList(GUIPropertyList& properlist)
	{
		BaseClass::GetPropertyList(properlist);
		properlist.push_back(GWP_ITEM_STACK_NUMBER_VISIBILITY);
	}

	bool GuiItemStack::GetPropertyType(const GUIString& strName, GUIPropertyType& type)
	{
		if (BaseClass::GetPropertyType(strName, type))
		{
			return true;
		}

		if (strName == GWP_ITEM_STACK_NUMBER_VISIBILITY)
		{
			type = GPT_INT;
			return true;
		}
		return false;
	}

	String GuiItemStack::getItemSpriteName(int itemId, int damage, int pass)
	{
		if (itemId >= 10000)
		{
			return UICommon::getCurrencyIcon();
		}

		cItem* pItem = ItemClient::citemsList[itemId];
		if (pItem == nullptr)
			return StringUtil::BLANK;

		StringStream ss;
		ss << "set:";
		switch (pItem->getRenderType())
		{
		case ItemRenderType::BLOCK:
			ss << "Blocks.json";
			break;
		case ItemRenderType::MESH:
		case ItemRenderType::TEXTURE:
			ss << SwitchRenderItemsJson(itemId);
			break;
		case ItemRenderType::COMPASS:
			ss << "compass_gui.json";
			break;
		case ItemRenderType::CLOCK:
			ss << "clock_gui.json";
			break;
		default:
			break;
		}
		ss << " image:";
		AtlasSprite* atlas = pItem->getIconFromDamageForRenderPass(damage, pass);
		if (atlas != NULL)
		{
			ss << atlas->getSpriteName();
		}
		return ss.str();
	}

	gui_renderItems * GuiItemStack::getRenderItems()
	{
		return pRenderItems;
	}

	void GuiItemStack::showItemIcon(GUIStaticImage* pItemIcon, ItemStackPtr pStack, int itemId, int damage, String image)
	{
		if (image.length() > 0)
		{
			if(pItemIcon)
				pItemIcon->SetImage(image.c_str());
			return;
		}

		if (itemId >= 10000)
		{
			if (pItemIcon)
			{
				pItemIcon->SetImage(UICommon::getCurrencyIcon().c_str());
			}
				
			return;
		}

		Item* pItem = Item::itemsList[itemId];
		auto pcItem = ItemClient::citemsList[itemId];
		if (!pItem || !pcItem )
			return;

		Block* pBlock = NULL;
		cBlock* pcBlock = NULL;
		int blockId = -1;
		if (auto blockItem = dynamic_cast<cItemBlock*>(pcItem))
		{
			blockId = blockItem->getBlockId();
			pBlock = BlockManager::sBlocks[blockId];
			pcBlock = cBlockManager::scBlocks[blockId];
		}

		//render as block
		if (pItem && pcItem->getRenderType() == ItemRenderType::BLOCK && pBlock && RenderBlocks::renderItemIn3d(pcBlock->getRenderType()))
		{
			if (pItemIcon)
			{
				getRenderItems()->renderBlockAsItem(pItemIcon, blockId, damage);
				pItemIcon->SetRenderAsBlock(true);
			}
		}
		//render as item
		else
		{
			Color color = Color(pcItem->getColorFromItemStack(pStack, 0));
			if (pItem && (pItem->itemID == BLOCK_ID_WATERLILY || pItem->itemID == BLOCK_ID_VINE))
				color = pcBlock->getBlockColor();
			//std::swap(color.r, color.b);
			color.a = 1.f;
			String spriteName = getItemSpriteName(itemId, damage, 0);
			if (pItemIcon)
			{
				pItemIcon->SetHexagonImage("", "");
				pItemIcon->SetDrawColor(color);
				pItemIcon->SetImage(spriteName.c_str());
				pItemIcon->SetRenderAsBlock(false);
			}

			if (pcItem->requiresMultipleRenderPasses())
			{
				String overlayName = getItemSpriteName(itemId, damage, 1);
				if (pItemIcon)
					pItemIcon->SetOverlayImage(overlayName.c_str());
				Color color2 = Color(pcItem->getColorFromItemStack(pStack, 1));
				color2.a = 1.f;
				std::swap(color2.r, color2.b);
				if (pItemIcon)
					pItemIcon->SetOverlayColor(color2);
			}
			else
			{
				if (pItemIcon)
					pItemIcon->SetOverlayImage("");
			}
		}
	}

	void GuiItemStack::updateCount(int count)
	{
		switch (m_numberVisibility)
		{
		case NumberVisibility::AUTO:
			if (count > 1)
			{
				m_countText->SetVisible(true);
				m_countText->SetText(StringUtil::ToString(count).c_str());
			}
			else
			{
				m_countText->SetVisible(false);
			}
			break;
		case NumberVisibility::ALWAYS_VISIBLE:
			m_countText->SetVisible(true);
			m_countText->SetText(StringUtil::ToString(count).c_str());
			break;
		case NumberVisibility::NEVER_VISIBLE:
			m_countText->SetVisible(false);
			break;
		default:
			LordLogError("Unkown NumberVisibility value");
			break;
		}
	}

	void GuiItemStack::updateIcon(int itemId, int damage)
	{
		if (itemId == m_oldItemId && damage == m_oldItemDamage)
		{
			return;
		}
		m_oldItemId = itemId;
		m_oldItemDamage = damage;
		if(m_iconImage)
			showItemIcon(m_iconImage, m_itemStack, itemId, damage);
	}

	void GuiItemStack::updateGunNum()
	{
		if (!m_gunNum)
		{
			return;
		}
		ItemGun* pGun = dynamic_cast<ItemGun*>(m_itemStack->getItem());
		if (pGun)
		{
			if (LogicSetting::Instance()->getGunIsNeedBulletStatus())
			{
				int pSpareBulletNum = 0;
				auto setting = pGun->getGunSetting();
				auto player = Blockman::Instance()->m_pPlayer;
				if (setting && player && player->inventory)
				{
					pSpareBulletNum = player->inventory->getItemNum(setting->adaptedBulletId);
				}
				if (m_itemStack->getCurrentClipBulletNum() == 0)
				{
					if (pSpareBulletNum == 0)
					{
						m_gunNum->SetText(StringUtil::Format("▢FFFF0000%d", m_itemStack->getCurrentClipBulletNum()).c_str());
					}
					else
					{
						m_gunNum->SetText(StringUtil::Format("▢FFFF0000%d▢FFFFFFFF/%d", m_itemStack->getCurrentClipBulletNum(), pSpareBulletNum).c_str());
					}
				}
				else
				{
					if (pSpareBulletNum == 0)
					{
						m_gunNum->SetText(StringUtil::Format("%d", m_itemStack->getCurrentClipBulletNum()).c_str());
					}
					else
					{
						m_gunNum->SetText(StringUtil::Format("%d/%d", m_itemStack->getCurrentClipBulletNum(), pSpareBulletNum).c_str());
					}
				}
			}
			else
			{
				if (m_itemStack->getCurrentClipBulletNum() == 0)
				{
					m_gunNum->SetText(StringUtil::Format("▢FFFF0000%d▢FFFFFFFF/%d", m_itemStack->getCurrentClipBulletNum(), m_itemStack->getTotalBulletNum()).c_str());
				}
				else
				{
					m_gunNum->SetText(StringUtil::Format("%d/%d", m_itemStack->getCurrentClipBulletNum(), m_itemStack->getTotalBulletNum()).c_str());
				}
			}
			return;
		}
		m_gunNum->SetText("");
	}

	vector<GUIWindow*>::type GuiItemStack::getPrivateChildren()
	{
		return { m_window };
	}


	void GuiItemStack::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		if(m_iconImage)
			m_iconImage->setGray(m_isGrayedOut);

		if (m_gunNum)
		{
			m_gunNum->SetText("");
		}
		if (!m_itemStack && !m_ingredient)
		{
			m_window->SetVisible(false);
		}
		else if (m_itemStack)
		{
			m_window->SetVisible(true);
			updateIcon(m_itemStack->itemID, m_itemStack->getItemDamage());
			updateCount(m_itemStack->stackSize);
			updateGunNum();
		}
		else
		{
			m_window->SetVisible(true);
			updateIcon(m_ingredient->getItemId(), m_ingredient->getCandidateItemDataList()[0]);
			updateCount(1);
		}
		if (isEditorMode)
		{
			m_window->SetVisible(true);
		}
		BaseClass::UpdateSelf(nTimeElapse);
	}

	void GuiItemStack::Destroy()
	{
		RemoveWindowFromDrawList(*m_window);
		GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		BaseClass::Destroy();
	}

	void GuiItemStack::Clone(const GUIString & strNamePreffix, GUIWindow * pWindow)
	{
		auto guiItemStack = dynamic_cast<GuiItemStack*>(pWindow);
		if (!guiItemStack)
		{
			LordLogError("cannot clone GuiItemStack from GUIWindow that isn't a GuiItemStack");
			return;
		}
		BaseClass::Clone(strNamePreffix, guiItemStack);
		setItemStack(guiItemStack->getItemStack());
		setNumberVisibility(guiItemStack->m_numberVisibility);
		setGrayedOut(guiItemStack->m_isGrayedOut);
	}

	String GuiItemStack::SwitchRenderItemsJson(int itemId)
	{
		String ItemsJsonConfig = "items.json";
		if (itemId >= Item::itemSkillFlashBook->itemID && itemId <= Item::itemSurpriseTreasureChest_03->itemID)
		{
			ItemsJsonConfig = "throwpot.json";
			return ItemsJsonConfig;
		}
		return ItemsJsonConfig;
	}
}
