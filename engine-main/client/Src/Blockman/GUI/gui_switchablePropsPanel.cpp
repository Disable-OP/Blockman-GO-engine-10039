#include "gui_switchablePropsPanel.h"
#include "GUISwitchablePropItem.h"
#include "GUI/GuiDef.h"

namespace BLOCKMAN
{
	gui_switchablePropsPanel::gui_switchablePropsPanel()
		: gui_layout("SwitchablePropsPanel.json")
	{
	}

	gui_switchablePropsPanel::~gui_switchablePropsPanel()
	{
	}

	void gui_switchablePropsPanel::onLoad()
	{
		auto contentLayout = getWindow("SwitchablePropsPanel");
		m_switchableProps = (GUIGridView*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_GRID_VIEW, "SwitchablePropsPanel-ItemList-GridView");
		m_switchableProps->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.f, 0), UDim(1.f, 0));
		m_switchableProps->InitConfig(7.f, 7.f, 4);
		contentLayout->AddChildWindow(m_switchableProps);
	}

	void gui_switchablePropsPanel::onUpdate(ui32 nTimeElapse)
	{
	}

	void gui_switchablePropsPanel::refreshItems(std::list<SwitchableProp> props, float contentSize)
	{
		m_props = props;
		if (m_props.size() != m_switchableProps->GetItemCount())
			m_switchableProps->ResetPos();
		m_switchableProps->RemoveAllItems();
		static unsigned propCount = 0;
		float maxWidth = (contentSize - 7.0f * 3) / 4;
		for (SwitchableProp prop : m_props)
		{
			propCount++;
			GUIString itemName = StringUtil::Format("SwitchablePropsPanel-Item-%d", propCount).c_str();
			GUISwitchablePropItem* propItem = (GUISwitchablePropItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_SWITCHABLE_PROP_ITEM, itemName);
			propItem->SetWidth({ 0, Math::Min(maxWidth, 190.0f) });
			propItem->SetHeight({ 0, 260.f });
			propItem->setSwitchableProp(prop);
			m_switchableProps->AddItem(propItem);
		}
	}

}