#include "GuiBirdDressItem.h"
#include "UI/GUIWindowManager.h"
#include "Network/ClientNetwork.h"
#include "GUI/GuiBirdPackAndFuseItem.h"

namespace BLOCKMAN
{
	
	void GuiBirdDressItem::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	
	vector<GUIWindow*>::type GuiBirdDressItem::getPrivateChildren()
	{
		return { m_window };
	}

	GuiBirdDressItem::GuiBirdDressItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		removeComponents();
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("BirdDressItem.json"));
		renameComponents(m_window);

		m_image = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		m_greenDot = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(1));
		m_image->subscribeEvent(EventWindowTouchUp, std::bind(&GuiBirdDressItem::onClick, this, std::placeholders::_1));
		AddWindowToDrawList(*m_window);
	}

	void GuiBirdDressItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		BaseClass::UpdateSelf(nTimeElapse);
	}

	
	void GuiBirdDressItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GuiBirdDressItem::setDress(const BirdDress& birdDress)
	{
		m_birdDress = birdDress;
	}

	void GuiBirdDressItem::setDressImage(const String & imageName)
	{
		m_image->SetImage(imageName.c_str());
	}

	void GuiBirdDressItem::setUse(bool isUse)
	{
		m_greenDot->SetVisible(isUse);
	}

	bool GuiBirdDressItem::onClick(const EventArgs& args)
	{
		if (!m_greenDot->IsVisible() && m_birdId > 0 && m_birdDress.id > 0)
		{
			ClientNetwork::Instance()->getSender()->sendBirdSetDress(m_birdId, m_birdDress.type, m_birdDress.id);
		}
		else if (m_greenDot->IsVisible() && m_birdId > 0)
		{
			ClientNetwork::Instance()->getSender()->sendBirdSetDress(m_birdId, m_birdDress.type, 0);
		}
		return true;
	}

}