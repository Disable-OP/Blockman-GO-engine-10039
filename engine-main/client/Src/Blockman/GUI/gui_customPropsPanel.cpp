#include "gui_customPropsPanel.h"
#include "UI/GUIWindowManager.h"
#include "UI/GUILayout.h"
#include "GUI/GUICustomPropItem.h"
#include "GUI/GuiDef.h"
#include "game.h"

namespace BLOCKMAN
{
	gui_customPropsPanel::gui_customPropsPanel()
		: gui_layout("CustomPropsPanel.json")
	{
	}

	void gui_customPropsPanel::onLoad()
	{
		auto contentLayout = getWindow("CustomPropsPanel");
		m_customProps = (GUIGridView*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_GRID_VIEW, "CustomPropsPanel-ItemList-GridView");
		m_customProps->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.f, 0), UDim(1.f, 0));
		m_customProps->InitConfig(7.f, 7.f, 4);
		contentLayout->AddChildWindow(m_customProps);
	}

	void gui_customPropsPanel::onUpdate(ui32 nTimeElapse)
	{
	}

	void gui_customPropsPanel::setCustomPropGroup(String groupId, std::list<CustomProp> props, float contentSize)
	{
		m_groupId = groupId;
		m_props = props;
		if (m_props.size() != m_customProps->GetItemCount())
			m_customProps->ResetPos();
		m_customProps->RemoveAllItems();
		static unsigned propCount = 0;
		float maxWidth = (contentSize - 7.0f * 3) / 4;
		for (CustomProp prop : m_props)
		{
			propCount++;
			GUIString itemName = StringUtil::Format("CustomPropsPanel-Item-%d", propCount).c_str();
			GUICustomPropItem* propItem = (GUICustomPropItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_CUSTOM_PROP_ITEM, itemName);
			propItem->SetWidth({ 0, Math::Min(maxWidth, 190.0f) });
			propItem->SetHeight({ 0, 550.f });
			propItem->setCustomProp(prop);
			m_customProps->AddItem(propItem);
		}
		
	}

	void gui_customPropsPanel::updateProp(CustomProp prop)
	{
		size_t count = m_customProps->GetItemCount();
		for (size_t i = 0; i < count; i++)
		{
			GUICustomPropItem* propItem = (GUICustomPropItem*)m_customProps->GetItem(i);
			if (propItem && propItem->getCustomProp().m_uniqueId == prop.m_uniqueId)
			{
				propItem->updateProp(prop);
				break;
			}
		}
	}

}
