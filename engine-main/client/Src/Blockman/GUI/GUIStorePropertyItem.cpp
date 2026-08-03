#include "GUIStorePropertyItem.h"
#include "UI/GUIWindowManager.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "Util/LanguageManager.h"

namespace BLOCKMAN
{
	GUIStorePropertyItem::GUIStorePropertyItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("StorePropertyItem.json"));
		renameComponents(m_window);

		AddWindowToDrawList(*m_window);
		SetTouchable(false);
	}

	void GUIStorePropertyItem::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	void GUIStorePropertyItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		BaseClass::UpdateSelf(nTimeElapse);
	}

	void GUIStorePropertyItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GUIStorePropertyItem::SetStoreProperty(StoreProperty* property, bool isDetail)
	{
		GUIStaticImage* background = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		GUIStaticImage* icon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(1));
		GUIStaticText* text = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(2));
		icon->SetImage(property->Icon.c_str());
		text->SetText(LanguageManager::Instance()->getString(property->Name).c_str());
		if (isDetail)
			background->SetImage("set:gun_store.json image:detail_infos_item");
		else
			background->SetImage("set:gun_store.json image:bg_infos_item");
	}

}
