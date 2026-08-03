#include "gui_upgradePropsPanel.h"
#include "GUIUpgradePropItem.h"
#include "GUI/GuiDef.h"

namespace BLOCKMAN
{
	gui_upgradePropsPanel::gui_upgradePropsPanel()
		: gui_layout("UpgradePropsPanel.json")
	{
	}

	gui_upgradePropsPanel::~gui_upgradePropsPanel()
	{
	}

	void gui_upgradePropsPanel::onLoad()
	{
		auto contentLayout = getWindow("UpgradePropsPanel");
		m_upgradeProps = (GUIGridView*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_GRID_VIEW, "UpgradePropsPanel-ItemList-GridView");
		m_upgradeProps->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.f, 0), UDim(1.f, 0));
		m_upgradeProps->InitConfig(7.f, 7.f, 4);
		contentLayout->AddChildWindow(m_upgradeProps);
	}

	void gui_upgradePropsPanel::onUpdate(ui32 nTimeElapse)
	{
	}

	void gui_upgradePropsPanel::refreshItems(std::list<UpgradeProp> props, float contentSize)
	{
		m_props = props;
		if (m_props.size() != m_upgradeProps->GetItemCount())
			m_upgradeProps->ResetPos();
		m_upgradeProps->RemoveAllItems();
		static unsigned propCount = 0;
		float maxWidth = (contentSize - 7.0f * 3) / 4;
		for (UpgradeProp prop : m_props)
		{
			propCount++;
			GUIString itemName = StringUtil::Format("UpgradePropsPanel-Item-%d", propCount).c_str();
			GUIUpgradePropItem* propItem = (GUIUpgradePropItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_UPGRADE_PROP_ITEM, itemName);
			propItem->SetWidth({ 0, Math::Min(maxWidth, 190.0f) });
			propItem->SetHeight({ 0, 550.f });
			propItem->setUpgradeProp(prop);
			m_upgradeProps->AddItem(propItem);
		}
	}

}