#include "gui_ranchTimeTip.h"
#include "Util/LanguageKey.h"
#include "UI/GUIButton.h"
#include "Util/ClientEvents.h"
#include "Util/UICommon.h"
#include "Inventory/Wallet.h"
#include "Network/ClientNetwork.h"
#include "World/Ranch.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Setting/CropsSetting.h"
#include "Setting/RanchSetting.h"

using namespace LORD;

namespace BLOCKMAN
{
	gui_ranchTimeTip::gui_ranchTimeTip()
		: gui_layout("RanchTimeTip.json")
	{
	}

	gui_ranchTimeTip::~gui_ranchTimeTip()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_ranchTimeTip::onLoad()
	{
		m_tvTitleName = getWindow<GUIStaticText>("RanchTimeTip-Title-Name");
		m_tvDesc = getWindow<GUIStaticText>("RanchTimeTip-Time-Msg");
		m_price = getWindow<GUIStaticText>("RanchTimeTip-Currency-Value");
		m_tvTimeLeftValue = getWindow<GUIStaticText>("RanchTimeTip-Time-Left-Value");
		m_timeProgress = getWindow<GUIProgressBar>("RanchTimeTip-Time-Left");
		getWindow<GUIButton>("RanchTimeTip-Btn-Close")->subscribeEvent(EventButtonClick, std::bind(&gui_ranchTimeTip::onClick, this, std::placeholders::_1, ViewId::CLOSE));
		getWindow<GUIButton>("RanchTimeTip-Btn-Sure")->subscribeEvent(EventButtonClick, std::bind(&gui_ranchTimeTip::onClick, this, std::placeholders::_1, ViewId::SURE));
		getWindow<GUIButton>("RanchTimeTip-Btn-Sure")->SetText(getString(LanguageKey::GUI_RANCH_TIME_TIP_SURE));

		m_tvTitleName->SetText(getString(LanguageKey::GUI_RANCH_TIME_TIP_EXTEND));
		m_tvDesc->SetText(getString(LanguageKey::GUI_RANCH_TIME_TIP_DESC));
		m_subscriptionGuard.add(ShowRanchTimeTipEvent::subscribe(std::bind(&gui_ranchTimeTip::showTip, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)));
		m_subscriptionGuard.add(ShowCropInfoEvent::subscribe(std::bind(&gui_ranchTimeTip::showCropInfo, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5)));
		m_subscriptionGuard.add(ShowSpeedUpQueueEvent::subscribe(std::bind(&gui_ranchTimeTip::showSpeedUpQueue, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5)));

	}

	void gui_ranchTimeTip::onUpdate(ui32 nTimeElapse)
	{
		if (m_timeProgress && m_tvTimeLeftValue && isShown() && m_timeLeft > 0 && UICommon::checkRanchParam())
		{
			m_timeLeft = (m_timeLeft - nTimeElapse) > 0 ? (m_timeLeft - nTimeElapse) : 0;
			m_timeProgress->SetProgress(m_timeLeft / m_totalTime);
			m_tvTimeLeftValue->SetText(timeFormat((i32) m_timeLeft ).c_str());
			m_priceNum = Blockman::Instance()->m_pPlayer->m_ranch->getItemTimePrice((int)(m_timeLeft / 1000), m_itemId);
			m_price->SetText(StringUtil::ToString(m_priceNum).c_str());
			if (m_timeLeft == 0)
			{
				RootGuiLayout::Instance()->setRanchTimeTipVisible(false);
			}
		}
	}

	bool gui_ranchTimeTip::onClick(const EventArgs & events, ViewId viewId)
	{
		switch (viewId)
		{
		case ViewId::CLOSE:
			RootGuiLayout::Instance()->setRanchTimeTipVisible(false);
			break;
		case ViewId::SURE:
			if (UICommon::checkMoney(m_priceNum, CurrencyType::PLATFORM_MONEY))
			{
				switch (m_showType)
				{
				case ShowType::EXTEND_LAND:
					ClientNetwork::Instance()->getSender()->sendRanchShortcut(m_landCode, (i32)CurrencyType::PLATFORM_MONEY);
					break;
				case ShowType::CROP_INFO:
				{
					auto item = CropsSetting::getCropsSetting(m_blockId);
					if (item)
					{
						if (!Blockman::Instance()->m_pPlayer->m_ranch->getStorage()->canSave(item->minQuantity))
						{
							//ShowRanchCommonTipEvent::emit(getString("gui_ranch_speed_up_error_storage_full").c_str());
							RootGuiLayout::Instance()->setRanchTimeTipVisible(false);
							ShowRanchStorageFullTipEvent::emit("gui_ranch_storage_full_operation_failure");
							return false;
						}
						ClientNetwork::Instance()->getSender()->sendSpeedUpCrop(m_blockPos, m_blockId);
					}
				}
					break;
				case ShowType::SPEED_UP_QUEUE:
					//1 production 2 speed up
					ClientNetwork::Instance()->getSender()->sendRanchBuildQueueOperation(2, m_entityId, m_queueId, 0);
					break;
				}

				RootGuiLayout::Instance()->setRanchTimeTipVisible(false);
			}
			break;
		}
		return false;
	}

	bool gui_ranchTimeTip::showTip(i32 landCode, i32 totalTime, i32 timeLeft, const String & titlle)
	{
		m_itemId = 0;
		m_showType = ShowType::EXTEND_LAND;
		m_landCode = landCode;
		m_timeLeft = (float)timeLeft;
		m_totalTime = (float)totalTime;
		m_tvTitleName->SetText(getString(titlle));
		RootGuiLayout::Instance()->setRanchTimeTipVisible(true);
		return true;
	}

	bool gui_ranchTimeTip::showCropInfo(Vector3i blockPos, i32 blockId, i32 stage, i32 curStageTime, i32 residueHarvestNum)
	{
		m_showType = ShowType::CROP_INFO;
		m_blockPos = blockPos;
		m_blockId = blockId;
		auto item = CropsSetting::getCropsSetting(blockId);
		if (item)
		{
			m_itemId = item->productionId;
			m_totalTime = item->getTotalTime() * 1000.f;
			i32 curTime = curStageTime;
			for (int i = 1; i < stage; i++)
			{
				curTime += item->getStateTime(i);
			}
			m_timeLeft = m_totalTime - curTime * 1000.f;
			m_tvTitleName->SetText(getString(RanchSetting::getRanchItemName(m_itemId)));
		}
		else
		{
			m_tvTitleName->SetText(getString("gui_ranch_crop_info"));
		}
		m_tvDesc->SetText(StringUtil::Format(getString("gui_ranch_speed_up_crops_info").c_str(), residueHarvestNum).c_str());
		RootGuiLayout::Instance()->setRanchTimeTipVisible(true);
		return false;
	}

	bool gui_ranchTimeTip::showSpeedUpQueue(i32 entityId, i32 queueId, i32 itemId, i32 totalTime, i32 timeLeft)
	{
		m_entityId = entityId;
		m_queueId = queueId;
		m_timeLeft = (float) timeLeft;
		m_totalTime = (float)totalTime;
		m_itemId = itemId;
		m_showType = ShowType::SPEED_UP_QUEUE;
		
		m_tvTitleName->SetText(getString("gui_ranch_build_speed_up_queue_title"));
		m_tvDesc->SetText(getString("gui_ranch_build_speed_up_queue_message"));
		RootGuiLayout::Instance()->setRanchTimeTipVisible(true);
		return false;
	}

	String gui_ranchTimeTip::timeFormat(i32 time)
	{
		String h = StringUtil::ToString(time / 1000 / 60 / 60);
		h = (h.length() == 1 ? ("0" + h) : h);
		String m = StringUtil::ToString(time / 1000 / 60 % 60);
		m = (m.length() == 1 ? ("0" + m) : m);
		String s = StringUtil::ToString(time / 1000 % 60);
		s = (s.length() == 1 ? ("0" + s) : s);
		return StringUtil::Format("%s:%s:%s", h.c_str(), m.c_str(), s.c_str());
	}
}
