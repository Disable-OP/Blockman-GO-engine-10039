#include "GuiBirdActivityItem.h"
#include "UI/GUIWindowManager.h"
#include "cWorld/Blockman.h"

namespace BLOCKMAN
{
	
	void GuiBirdActivityItem::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	
	vector<GUIWindow*>::type GuiBirdActivityItem::getPrivateChildren()
	{
		return { m_window };
	}

	GuiBirdActivityItem::GuiBirdActivityItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		removeComponents();
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("BirdActivityItem.json"));
		renameComponents(m_window);

		m_num = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(0));
		m_image = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(1)->GetChildByIndex(0));		

		AddWindowToDrawList(*m_window);
	}

	void GuiBirdActivityItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		BaseClass::UpdateSelf(nTimeElapse);

	}

	
	void GuiBirdActivityItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GuiBirdActivityItem::setGiftItem(String icon, i32 num)
	{
		m_image->SetImage(icon.c_str());
		m_num->SetText(StringUtil::Format("X%d", num).c_str());
	}

}