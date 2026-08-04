#include "gui_enchantmentPropsPanel.h"
#include "GUIEnchantmentPropItem.h"
#include "GUI/GuiDef.h"

namespace BLOCKMAN
{
	gui_enchantmentPropsPanel::gui_enchantmentPropsPanel()
		: gui_layout("EnchantmentPropsPanel.json")
	{
	}

	gui_enchantmentPropsPanel::~gui_enchantmentPropsPanel()
	{
	}

	void gui_enchantmentPropsPanel::onLoad()
	{
		auto contentLayout = getWindow("EnchantmentPropsPanel");
		m_enchantmentProps = (GUIGridView*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_GRID_VIEW, "EnchantmentPropsPanel-ItemList-GridView");
		m_enchantmentProps->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.f, 0), UDim(1.f, 0));
		m_enchantmentProps->InitConfig(7.f, 7.f, 4);
		contentLayout->AddChildWindow(m_enchantmentProps);
	}

	void gui_enchantmentPropsPanel::onUpdate(ui32 nTimeElapse)
	{
	}

	void gui_enchantmentPropsPanel::refreshItems(std::list<EnchantmentProp> props, float contentSize)
	{
		m_props = props;
		if (m_props.size() != m_enchantmentProps->GetItemCount())
			m_enchantmentProps->ResetPos();
		m_enchantmentProps->RemoveAllItems();
		static unsigned propCount = 0;
		float maxWidth = (contentSize - 7.0f * 3) / 4;
		for (EnchantmentProp prop : m_props)
		{
			propCount++;
			GUIString itemName = StringUtil::Format("EnchantmentPropsPanel-Item-%d", propCount).c_str();
			GUIEnchantmentPropItem* propItem = (GUIEnchantmentPropItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_ENCHANTMENT_PROP_ITEM, itemName);
			propItem->SetWidth({ 0, Math::Min(maxWidth, 190.0f) });
			propItem->SetHeight({ 0, 550.f });
			propItem->setEnchantmentProp(prop);
			m_enchantmentProps->AddItem(propItem);
		}
	}

}