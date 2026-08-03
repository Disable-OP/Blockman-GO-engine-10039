#include "GuiRanchBuildProduct.h"
#include "UI/GUIWindowManager.h"
#include "Util/ClientEvents.h"
#include "World/Ranch.h"
#include "Network/ClientNetwork.h"
#include "Util/UICommon.h"

namespace BLOCKMAN
{

	void GuiRanchBuildProduct::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	vector<GUIWindow*>::type GuiRanchBuildProduct::getPrivateChildren()
	{
		return { m_window };
	}

	GuiRanchBuildProduct::GuiRanchBuildProduct(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("RanchBuildProduct.json"));
		renameComponents(m_window);
		m_ivIcon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		m_ivIcon->subscribeEvent(EventWindowTouchUp, std::bind(&GuiRanchBuildProduct::onClick, this, std::placeholders::_1));
		AddWindowToDrawList(*m_window);
	}

	void GuiRanchBuildProduct::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		BaseClass::UpdateSelf(nTimeElapse);
	}


	void GuiRanchBuildProduct::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GuiRanchBuildProduct::setProduct(RanchCommon product)
	{
		m_product = product;
		m_ivIcon->SetImage(RanchSetting::getRanchItemIcon(m_product.itemId).c_str());
	}


	bool GuiRanchBuildProduct::onClick(const EventArgs & args)
	{
		vector<RanchCommon>::type items = vector<RanchCommon>::type();
		items.push_back(m_product);
		if (!UICommon::checkRanchStorageCapacity(items))
		{
			return false;
		}

		if (m_product.itemId > 0 && m_index > 0)
		{
			m_ivIcon->SetTouchable(false);
			ClientNetwork::Instance()->getSender()->sendRanchBuildTakeProduct(m_entityId, m_product.itemId, m_index);
		}
		return true;
	}

}