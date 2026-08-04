#include "GuiRanchBuildFactoryQueue.h"
#include "UI/GUIWindowManager.h"
#include "Util/ClientEvents.h"
#include "Setting/RanchSetting.h"

namespace BLOCKMAN
{

	void GuiRanchBuildFactoryQueue::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	vector<GUIWindow*>::type GuiRanchBuildFactoryQueue::getPrivateChildren()
	{
		return { m_window };
	}

	GuiRanchBuildFactoryQueue::GuiRanchBuildFactoryQueue(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("RanchBuildFactoryQueue.json"));
		renameComponents(m_window);

		m_ivIconbg = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		m_ivIcon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(1));

		AddWindowToDrawList(*m_window);
	}

	void GuiRanchBuildFactoryQueue::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		BaseClass::UpdateSelf(nTimeElapse);
	}


	void GuiRanchBuildFactoryQueue::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GuiRanchBuildFactoryQueue::setProductQueue(ProductQueue productQueue)
	{
		m_productQueue = productQueue;
		m_ivIcon->SetImage(RanchSetting::getRanchItemIcon(productQueue.productId).c_str());
		switch (m_productQueue.state)
		{
		case QueueState::QUEUE_LOCK:
			m_ivIconbg->SetImage("set:ranch_build.json image:add");
			break;
		case QueueState::QUEUE_LEISURE:
			m_ivIconbg->SetImage("set:ranch_build.json image:build_queue_empty");
			break;
		case QueueState::QUEUE_AWAIT:
			m_ivIconbg->SetImage("set:ranch_build.json image:build_queue_full");
			break;
		case QueueState::QUEUE_WORKING:
			m_ivIconbg->SetImage("set:ranch_build.json image:build_queue_full");
			break;
		case QueueState::QUEUE_DONE:
			m_ivIconbg->SetImage("set:ranch_build.json image:build_queue_full");
			break;
		}
		
	}

}