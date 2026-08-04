#include "GuiRanchBuildFarmQueue.h"
#include "UI/GUIWindowManager.h"
#include "Util/ClientEvents.h"
#include "Setting/RanchSetting.h"
#include "Util/UICommon.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"

namespace BLOCKMAN
{

	void GuiRanchBuildFarmQueue::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	vector<GUIWindow*>::type GuiRanchBuildFarmQueue::getPrivateChildren()
	{
		return { m_window };
	}

	GuiRanchBuildFarmQueue::GuiRanchBuildFarmQueue(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("RanchBuildFarmQueue.json"));
		renameComponents(m_window);

		m_leftTimeLayout = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(0));
		m_ivIconbg = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(1));
		m_ivIcon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(2));
		m_buyTimeLayout = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(3));

		m_pbTimeBar = dynamic_cast<GUIProgressBar*>(m_leftTimeLayout->GetChildByIndex(0));

		m_buyTimeLayout->GetChildByIndex(0)->subscribeEvent(EventButtonClick, std::bind(&GuiRanchBuildFarmQueue::onClick, this, std::placeholders::_1));

		AddWindowToDrawList(*m_window);
	}

	void GuiRanchBuildFarmQueue::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		BaseClass::UpdateSelf(nTimeElapse);
		if (!UICommon::checkRanchParam())
		{
			return;
		}

		if (m_productQueue.state == QueueState::QUEUE_WORKING)
		{
			m_productQueue.timeLeft -= nTimeElapse;
		}
		
		m_productQueue.timeLeft = (m_productQueue.timeLeft < 0 ? 0 : m_productQueue.timeLeft);
		m_pbTimeBar->GetChildByIndex(0)->SetText(timeFormat().c_str());
		float pb = m_productQueue.timeLeft * 1.0f / m_productQueue.needTime * 1.f;
		m_pbTimeBar->SetProgress(1.f - pb);
		String timePrice = StringUtil::ToString(Blockman::Instance()->m_pPlayer->m_ranch->getItemTimePrice(m_productQueue.timeLeft / 1000, m_productQueue.productId));
		m_buyTimeLayout->GetChildByIndex(0)->GetChildByIndex(1)->SetText(timePrice.c_str());
	}


	void GuiRanchBuildFarmQueue::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GuiRanchBuildFarmQueue::setProductQueue(ProductQueue productQueue)
	{
		m_productQueue = productQueue;
		m_ivIcon->SetImage(RanchSetting::getRanchItemIcon(productQueue.productId).c_str());
		switch (m_productQueue.state)
		{
		case QueueState::QUEUE_LOCK:
			m_ivIconbg->SetImage("set:ranch_build.json image:farm_add");
			break;
		case QueueState::QUEUE_LEISURE:
			m_ivIconbg->SetImage("set:ranch_build.json image:build_farm_queue_empty");
			m_pbTimeBar->SetVisible(false);
			m_buyTimeLayout->GetChildByIndex(0)->SetVisible(false);
			break;
		case QueueState::QUEUE_AWAIT:
			m_pbTimeBar->SetVisible(true);
			m_buyTimeLayout->GetChildByIndex(0)->SetVisible(false);
			m_ivIconbg->SetImage("set:ranch_build.json image:build_farm_queue_full");
			break;
		case QueueState::QUEUE_WORKING:
			m_ivIconbg->SetImage("set:ranch_build.json image:build_farm_queue_full");
			m_pbTimeBar->SetVisible(true);
			m_buyTimeLayout->GetChildByIndex(0)->SetVisible(true);
			break;
		case QueueState::QUEUE_DONE:
			m_ivIconbg->SetImage("set:ranch_build.json image:build_farm_queue_full");
			break;
		}
		
	}

	bool GuiRanchBuildFarmQueue::onClick(const EventArgs & args)
	{
		//todo  buy time
		if (m_entityId > 0 && m_productQueue.productId > 0 && m_productQueue.timeLeft >0)
		{
			ShowSpeedUpQueueEvent::emit(m_entityId, m_productQueue.queueId, m_productQueue.productId, m_productQueue.needTime, m_productQueue.timeLeft);
			
		}
		return true;
	}

	String GuiRanchBuildFarmQueue::timeFormat()
	{
		String h = StringUtil::ToString(m_productQueue.timeLeft / 1000 / 60 / 60);
		h = (h.length() == 1 ? ("0" + h) : h);
		String m = StringUtil::ToString(m_productQueue.timeLeft / 1000 / 60 % 60);
		m = (m.length() == 1 ? ("0" + m) : m);
		String s = StringUtil::ToString(m_productQueue.timeLeft / 1000 % 60);
		s = (s.length() == 1 ? ("0" + s) : s);
		return StringUtil::Format("%s:%s:%s", h.c_str(), m.c_str(), s.c_str());
	}

}