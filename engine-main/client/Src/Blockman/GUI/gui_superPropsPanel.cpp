#include "gui_superPropsPanel.h"
#include "GUI/GUISuperPropItem.h"
#include "GUI/GuiDef.h"

namespace BLOCKMAN
{
	gui_superPropsPanel::gui_superPropsPanel()
		: gui_layout("SuperPropsPanel.json")
	{
	}

	gui_superPropsPanel::~gui_superPropsPanel()
	{
	}

	void gui_superPropsPanel::onLoad()
	{
		auto contentLayout = getWindow("SuperPropsPanel");
		m_superProps = (GUIGridView*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_GRID_VIEW, "SuperPropsPanel-ItemList-GridView");
		m_superProps->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.f, 0), UDim(1.f, 0));
		m_superProps->InitConfig(7.f, 7.f, 3);
		contentLayout->AddChildWindow(m_superProps);
	}

	void gui_superPropsPanel::onUpdate(ui32 nTimeElapse)
	{
	}

	void gui_superPropsPanel::refreshItems(std::list<SuperProp> props, float contentSize)
	{
		m_props = props;
		float width = (contentSize - 14.0f) / 3;
		if (m_props.size() != m_superProps->GetItemCount())
			m_superProps->ResetPos();
		m_superProps->RemoveAllItems();
		static unsigned propCount = 0;
		for (SuperProp prop : m_props)
		{
			propCount++;
			GUIString itemName = StringUtil::Format("SuperPropsPanel-Item-%d", propCount).c_str();
			GUISuperPropItem* propItem = (GUISuperPropItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_SUPER_PROP_ITEM, itemName);
			propItem->SetWidth({ 0, width });
			propItem->SetHeight({ 0, 550.f });
			propItem->setSuperProp(prop);
			m_superProps->AddItem(propItem);
		}
	}

}