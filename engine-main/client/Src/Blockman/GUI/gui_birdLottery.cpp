#include "gui_birdLottery.h"
#include "RootGuiLayout.h"
#include "GuiDef.h"
#include "World/World.h"
#include "cWorld/Blockman.h"
#include "World/BirdSimulator.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "GUI/GuiBirdLotteryItem.h"
#include "Network/ClientNetwork.h"
#include "Entity/EntitySessionNpc.h"
#include "Util/ClientEvents.h"
#include "Util/UICommon.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
using namespace rapidjson;

namespace BLOCKMAN
{

	gui_birdLottery::gui_birdLottery() :
		gui_layout("BirdLottery.json")
	{
	}

	gui_birdLottery::~gui_birdLottery()
	{
	}

	void gui_birdLottery::onLoad()
	{

		m_gvView = getWindow<GUIGridView>("BirdLottery-ItemView");
		m_btnOpen = getWindow<GUIButton>("BirdLottery-Open");
		m_tvPrice = getWindow<GUIStaticText>("BirdLottery-Open-Money-Text");
		m_ivCurrencyIcon = getWindow<GUIStaticImage>("BirdLottery-Open-Money-Image");
		m_gvView->InitConfig(10.f, 1.f, 3);
		setText(m_btnOpen, "gui_bird_lottery_store");
		m_btnOpen->subscribeEvent(EventButtonClick, std::bind(&gui_birdLottery::onClick, this, std::placeholders::_1, ViewId::SURE));
		getWindow("BirdLottery")->subscribeEvent(EventWindowTouchUp, std::bind(&gui_birdLottery::onClick, this, std::placeholders::_1, ViewId::CLOSE));
		m_subscriptionGuard.add(UpdateBirdLotteryEvent::subscribe(std::bind(&gui_birdLottery::onBirdLotteryUpdate, this)));
		m_subscriptionGuard.add(ShowBirdLotteryByNpcEvent::subscribe(std::bind(&gui_birdLottery::showLottery, this, std::placeholders::_1)));
	}

	void gui_birdLottery::onUpdate(ui32 nTimeElapse)
	{
		if (isShown())
		{
			if (m_gvViewXSize != m_gvView->GetPixelSize().x)
			{
				m_gvViewXSize = m_gvView->GetPixelSize().x;
				sortItemSizeChange();
			}
		}
	}

	bool gui_birdLottery::onClick(const EventArgs & events, ViewId viewId)
	{
		switch (viewId)
		{
		case ViewId::SURE:
			

			if (m_entityId > 0 && UICommon::checkBirdParam()  && UICommon::checkMoney(m_price, m_currencyType))
			{
				auto bag = Blockman::Instance()->m_pPlayer->m_birdSimulator->getBag();

				if (bag->curCapacity >= bag->maxCapacity)
				{
					ShowBirdCapacityFullTipEvent::emit(bag->expandCapacityPrice, bag->expandCurrencyType);
				}
				else
				{
					ClientNetwork::Instance()->getSender()->sendBirdLottery(m_entityId);
				}
			}
			break;
		case ViewId::CLOSE:
			RootGuiLayout::Instance()->showMainControl();
			break;
		}
		return true;
	}

	bool gui_birdLottery::onBirdLotteryUpdate()
	{
		if (isShown() && m_entityId> 0)
		{
			showLottery(m_entityId);
		}
		return true;
	}

	bool gui_birdLottery::showLottery(i32 entityId)
	{
		auto entity = dynamic_cast<EntitySessionNpc*>(Blockman::Instance()->m_pWorld->getEntity(entityId));
		if (!entity)
		{
			m_entityId = -1;
			return false;
		}
		m_entityId = entityId;
		m_gvView->RemoveAllItems();
		auto items = parseData(entity->getSessionContent());
		static int count = 0;
		for (const auto& item : items)
		{
			count++;
			String iterLayout = StringUtil::Format("BirdLottery-ItemView-Item-%d", count).c_str();
			GuiBirdLotteryItem* itemView = (GuiBirdLotteryItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_BIRD_LOTTERY_ITEM, iterLayout.c_str());
			itemView->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(0.0f, 82.f), UDim(0.0f, 82.f));
			itemView->setBirdEgg(item);
			m_gvView->AddItem(itemView);
		}

		RootGuiLayout::Instance()->showBirdLottery();
		return true;
	}

	vector<BirdEgg>::type gui_birdLottery::parseData(const String& data)
	{
		vector<BirdEgg>::type items = vector<BirdEgg>::type();
		Document doc = Document();
		doc.Parse(data.c_str());
		if (doc.HasParseError())
		{
			return items;
		}

		if (doc.HasMember("price"))
		{
			m_price = doc.FindMember("price")->value.GetInt();
			m_tvPrice->SetText(StringUtil::ToString(m_price).c_str());
		}

		if (doc.HasMember("currencyType"))
		{
			m_currencyType = doc.FindMember("currencyType")->value.GetInt();
			m_ivCurrencyIcon->SetImage(UICommon::getCurrencyIconByType(m_currencyType).c_str());
		}

		if (doc.HasMember("items"))
		{
			auto content = doc.FindMember("items")->value.GetArray();
			for (const auto& item : content)
			{

				if (item.HasMember("id") && item.HasMember("prob") && item.HasMember("quality") && item.HasMember("icon") && item.HasMember("isHave"))
				{
					BirdEgg lottery = BirdEgg();
					lottery.id = item.FindMember("id")->value.GetInt();
					lottery.prob = item.FindMember("prob")->value.GetFloat();
					lottery.quality = item.FindMember("quality")->value.GetInt();
					lottery.icon = item.FindMember("icon")->value.GetString();
					lottery.isHave = item.FindMember("isHave")->value.GetBool();
					items.push_back(lottery);
				}
			}
		}

		return items;
	}

	void gui_birdLottery::sortItemSizeChange()
	{
		for (size_t i = 0; i < m_gvView->GetItemCount(); i++)
		{
			m_gvView->GetItem(i)->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(0.0f, 82.f), UDim(0.0f, 82.f));
		}
	}

}
