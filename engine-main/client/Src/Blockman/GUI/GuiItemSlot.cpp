#include "GuiItemSlot.h"
#include "UI/GUIWindowManager.h"
#include "GUI/GuiDef.h"

using namespace LORD;
namespace BLOCKMAN
{
	void GuiItemSlot::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
		m_guiItemStack = nullptr;
		m_dropItemProgress = nullptr;
		m_armorTypeImage = nullptr;
		m_skillCdProgress = nullptr;
	}

	vector<GUIWindow*>::type GuiItemSlot::getPrivateChildren()
	{
		return { m_window };
	}

	GuiItemSlot::GuiItemSlot(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUIStaticImage*>(GUIWindowManager::Instance()->LoadWindowFromJSON("ItemSlot.json"));
		renameComponents(m_window);
		m_dropItemProgress = dynamic_cast<GUIProgressBar*>(m_window->GetChildByIndex(0));
		m_enchantmentIndicator = m_window->GetChildByIndex(1);
		m_armorTypeImage = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(2));
		m_guiItemStack = dynamic_cast<GuiItemStack*>(m_window->GetChildByIndex(3));
		m_skillCdProgress = dynamic_cast<GUIProgressBar*>(m_window->GetChildByIndex(4));
		m_guiItemStack->setGunNum(dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(5)));
		m_window->subscribeEvent(EventWindowTouchDown, SubscriberSlot(&GuiItemSlot::onItemStackTouchDown, this));
		m_window->subscribeEvent(EventWindowTouchUp, SubscriberSlot(&GuiItemSlot::onItemStackTouchUp, this));
		m_window->subscribeEvent(EventWindowLongTouchStart, SubscriberSlot(&GuiItemSlot::onItemStackLongTouchStart, this));
		m_window->subscribeEvent(EventWindowLongTouchEnd, SubscriberSlot(&GuiItemSlot::onItemStackLongTouchEnd, this));
		m_window->subscribeEvent(EventMotionRelease, SubscriberSlot(&GuiItemSlot::onItemStackLongTouchEnd, this));
		m_window->setEnableLongTouch(true);
		m_skillCdProgress->SetVisible(false);
		setSelected(false);
		AddWindowToDrawList(*m_window);
		subscribeToBubbleUpMouseEvents();
	}

	void GuiItemSlot::updateStyle()
	{
		if (!is_modified_style) return;

		switch (m_style)
		{
		case BLOCKMAN::GuiItemSlot::Style::ROCK_TRANS:
			if (!m_selected)
			{
				m_window->SetImage("set:new_gui_material.json image:wupinkuang");
				//m_window->SetImage("set:ItemSlotImage.json image:item_stack_bg");
				//m_window->SetScale({ 1, 1, 1 });
			}
			else
			{
				m_window->SetImage("set:new_gui_material.json image:wupinkuang_xuanzhong");
				//m_window->SetScale({ 1.1f, 1.1f, 1.1f });
			}

			m_window->SetProperty(GWP_WINDOW_STRETCH_TYPE, "NineGrid");
			m_window->SetProperty(GWP_WINDOW_STRETCH_OFFSET, "8 11 8 13");
			break;
		case BLOCKMAN::GuiItemSlot::Style::ROCK_SOLID:
			if (!m_selected)
			{
				m_window->SetImage("set:new_gui_material.json image:wupinkuang_shixin");
				//m_window->SetScale({ 1, 1, 1 });
			}
			else
			{
				m_window->SetImage("set:new_gui_material.json image:wupinkuang_shixin_xuanzhong");
				//m_window->SetScale({ 1.1f, 1.1f, 1.1f });
			}

			m_window->SetProperty(GWP_WINDOW_STRETCH_TYPE, "NineGrid");
			m_window->SetProperty(GWP_WINDOW_STRETCH_OFFSET, m_selected ?   "7 8 7 8" : "6 6 6 6" );
			break;
		case BLOCKMAN::GuiItemSlot::Style::METAL:
			if (!m_selected)
			{
				m_window->SetImage("set:new_gui_material.json image:wupinkuang_jinshu");
				//m_window->SetScale({ 1, 1, 1 });
			}
			else
			{
				m_window->SetImage("set:new_gui_material.json image:wupinkuang_shixin_xuanzhong");
				//m_window->SetScale({ 1.1f, 1.1f, 1.1f });
			}

			m_window->SetProperty(GWP_WINDOW_STRETCH_TYPE, "NineGrid");
			m_window->SetProperty(GWP_WINDOW_STRETCH_OFFSET, m_selected ? "7 8 7 8" : "6 6 6 6");
			break;
		default:
			// TODO: log error
			break;
		}

		is_modified_style = false;
	}

	void GuiItemSlot::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		updateStyle();
		NBTTagList* tagList = m_guiItemStack->getItemStack() ? m_guiItemStack->getItemStack()->getEnchantmentTagList() : nullptr;
		m_enchantmentIndicator->SetVisible(tagList && tagList->tagCount() > 0);
		m_armorTypeImage->SetVisible(m_armor_type != NONE && !m_guiItemStack->getItemStack());
		BaseClass::UpdateSelf(nTimeElapse);
	}

	void GuiItemSlot::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	GuiItemStack * GuiItemSlot::cloneGuiItemStack(const GUIString & newName)
	{
		auto newItemStack = GUIWindowManager::Instance()->CreateGUIWindow(GWT_ITEM_STACK, newName);
		newItemStack->Clone(newName, m_guiItemStack);
		return static_cast<GuiItemStack*>(newItemStack);
	}

	void GuiItemSlot::setArmorType(ArmorType type)
	{
		m_armor_type = type;
		switch (m_armor_type)
		{
		case HELMET:
			m_armorTypeImage->SetImage("set:new_gui_material.json image:armor_type_helmet");
			break;
		case CHESTPLATE:
			m_armorTypeImage->SetImage("set:new_gui_material.json image:armor_type_chestplate");
			break;
		case LEGGINGS:
			m_armorTypeImage->SetImage("set:new_gui_material.json image:armor_type_leggings");
			break;
		case BOOTS:
			m_armorTypeImage->SetImage("set:new_gui_material.json image:armor_type_boots");
			break;
		default:
			break;
		}
	}

	void GuiItemSlot::setDropItemProgress(float progress)
	{
		if (m_dropItemProgress)
		{
			m_dropItemProgress->SetProgress(progress);
		}
	}

	void GuiItemSlot::setItemSkillCdProgress(int curValue, int maxValue)
	{
		if (curValue == 0 || maxValue == 0 || curValue == maxValue)
		{
			m_skillCdProgress->SetVisible(false);
			return;
		}
		m_skillCdProgress->SetVisible(true);
		if (m_skillCdProgress)
		{
			m_skillCdProgress->SetProgress((float)curValue/(float)maxValue);
		}
	}
}
