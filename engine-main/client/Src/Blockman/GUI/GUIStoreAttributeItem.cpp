#include "GUIStoreAttributeItem.h"
#include "UI/GUIWindowManager.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "Util/LanguageManager.h"

namespace BLOCKMAN
{
	GUIStoreAttributeItem::GUIStoreAttributeItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("StoreAttributeItem.json"));
		renameComponents(m_window);

		AddWindowToDrawList(*m_window);
		SetTouchable(false);
	}

	void GUIStoreAttributeItem::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	void GUIStoreAttributeItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		BaseClass::UpdateSelf(nTimeElapse);
	}

	void GUIStoreAttributeItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GUIStoreAttributeItem::SetStoreAttribute(StoreAttribute* attribute, int addValue, bool isDetail)
	{
		GUIStaticImage* background = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		GUIStaticImage* icon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(1));
		GUIStaticText* text = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(2));
		GUIStaticText* value = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(3));
		icon->SetImage(attribute->Icon.c_str());
		text->SetText(LanguageManager::Instance()->getString(attribute->Name).c_str());
		if (addValue > 0)
			if (attribute->IsPlus)
				value->SetText(StringUtil::Format("%d▢FF45EA1C +%d", attribute->Value, addValue).c_str());
			else
				value->SetText(StringUtil::Format("%d▢FF45EA1C -%d", attribute->Value, addValue).c_str());
		else
			value->SetText(StringUtil::ToString(attribute->Value).c_str());
		if (isDetail)
			background->SetImage("set:gun_store.json image:detail_infos_item");
		else
			background->SetImage("set:gun_store.json image:bg_infos_item");

	}

}
