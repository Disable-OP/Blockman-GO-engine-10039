#include "gui_birdStore.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Network/ClientNetwork.h"
#include "Util/UICommon.h"
#include "UI/GUICommonDef.h"
#include "GUI/RootGuiLayout.h"
#include "Util/ClientEvents.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
using namespace rapidjson;

namespace BLOCKMAN
{

	gui_birdStore::gui_birdStore() :
		gui_layout("BirdStore.json")
	{
	}

	gui_birdStore::~gui_birdStore()
	{
	}

	void gui_birdStore::onLoad()
	{
		
		setText("BirdStore-DescContent-Title", "gui_bird_store_desc");
		m_tvTitle = getWindow<GUIStaticText>("BirdStore-TitleName");
		m_tvDesc = getWindow<GUIStaticText>("BirdStore-DescContent-Info");
		m_tvPrice = getWindow<GUIStaticText>("BirdStore-DescPrice-Value");
		m_ivIcon = getWindow<GUIStaticImage>("BirdStore-Icon");
		m_ivCurrencyIcon = getWindow<GUIStaticImage>("BirdStore-DescPrice-Icon");
		m_btnOperation = getWindow<GUIButton>("BirdStore-BtnOperation");

		m_subscriptionGuard.add(UpdateBirdStoreEvent::subscribe(std::bind(&gui_birdStore::onStoreUpdate, this)));
		m_subscriptionGuard.add(ShowBirdStoreByNpcEvent::subscribe(std::bind(&gui_birdStore::openStoreByNpc, this, std::placeholders::_1)));
		m_subscriptionGuard.add(ShowBirdStoreEvent::subscribe(std::bind(&gui_birdStore::openStore, this, std::placeholders::_1, std::placeholders::_2)));
		getWindow("BirdStore-Close")->subscribeEvent(EventWindowTouchUp, std::bind(&gui_birdStore::onClick, this, std::placeholders::_1, ViewId::CLOSE));
		getWindow<GUIButton>("BirdStore-BtnUp")->subscribeEvent(EventButtonClick, std::bind(&gui_birdStore::onClick, this, std::placeholders::_1, ViewId::UP));
		getWindow<GUIButton>("BirdStore-BtnNext")->subscribeEvent(EventButtonClick, std::bind(&gui_birdStore::onClick, this, std::placeholders::_1, ViewId::NEXT));
		getWindow<GUIButton>("BirdStore-BtnOperation")->subscribeEvent(EventButtonClick, std::bind(&gui_birdStore::onClick, this, std::placeholders::_1, ViewId::OPERATION));
		
	}

	void gui_birdStore::onUpdate(ui32 nTimeElapse)
	{
	}

	bool gui_birdStore::onClick(const EventArgs & args, ViewId viewId)
	{
		switch (viewId)
		{
		case ViewId::CLOSE:
			RootGuiLayout::Instance()->showMainControl();
			break;
		case ViewId::UP:
		{
			auto store = Blockman::Instance()->m_pPlayer->m_birdSimulator->getStoreById(m_storeId);
			if (store.items.size() > 0)
			{
				m_itemIndex -= 1;
				if (m_itemIndex < 0)
				{
					m_itemIndex = store.items.size() - 1;
				}
				changeUI(store.items[m_itemIndex], m_itemIndex);
			}
		}
		break;
		case ViewId::NEXT:
		{
			auto store = Blockman::Instance()->m_pPlayer->m_birdSimulator->getStoreById(m_storeId);
			if (store.items.size() > 0)
			{
				m_itemIndex += 1;
				if (m_itemIndex >= (i32)store.items.size())
				{
					m_itemIndex = 0;
				}
				changeUI(store.items[m_itemIndex], m_itemIndex);
			}
		}
		break;
		case ViewId::OPERATION:
			onSure();
			break;
		}
		return false;
	}

	bool gui_birdStore::onStoreUpdate()
	{
		if (isShown())
		{
			openStore(m_storeId, m_storeItem.id);
		}
		return true;
	}

	bool gui_birdStore::openStore(i32 storeId, i32 itemId)
	{
		m_storeId = storeId;
		auto store = Blockman::Instance()->m_pPlayer->m_birdSimulator->getStoreById(storeId);
		for (size_t i = 0; i < store.items.size(); i++)
		{
			if (itemId == 0 && i == 0 || store.items[i].id == itemId)
			{
				changeUI(store.items[i], (i32)i);
				RootGuiLayout::Instance()->showBirdStore();
				return true;
			}
		}
		return false;
	}

	bool gui_birdStore::openStoreByNpc(const String & msg)
	{
		Document doc = Document();
		doc.Parse(msg.c_str());
		if (doc.HasParseError())
		{
			return false;
		}

		if (doc.HasMember("itemId") && doc.HasMember("storeId"))
		{
			openStore(doc.FindMember("storeId")->value.GetInt(), doc.FindMember("itemId")->value.GetInt());
		}
		return true;
	}

	void gui_birdStore::changeUI(BirdStoreItem storeItem, i32 index)
	{
		m_itemIndex = index;
		m_storeItem = storeItem;
		setText(m_tvTitle, storeItem.name);
		setText(m_tvDesc, storeItem.desc);
		m_ivIcon->SetImage(storeItem.icon.c_str());
		m_tvPrice->SetText(StringUtil::ToString(storeItem.price).c_str());
		m_ivCurrencyIcon->SetImage(UICommon::getCurrencyIcon().c_str());
		switch (storeItem.status)
		{
		case 1:
			setText(m_btnOperation, "gui_bird_store_btn_operation_buy");
			m_btnOperation->SetPushedImage("set:bird_personalShop.json image:buyButtonNor");
			m_btnOperation->SetNormalImage("set:bird_personalShop.json image:buyButtonPre");
			break;
		case 2:
			setText(m_btnOperation, "gui_bird_store_btn_operation_equip");
			m_btnOperation->SetPushedImage("set:bird_personalShop.json image:buyButtonNor");
			m_btnOperation->SetNormalImage("set:bird_personalShop.json image:buyButtonPre");
			break;
		case 3:
			setText(m_btnOperation, "gui_bird_store_btn_operation_unequip");
			m_btnOperation->SetPushedImage("set:bird_pack.json image:reset_delete");
			m_btnOperation->SetNormalImage("set:bird_pack.json image:reset_delete");
			break;
		}
	}

	void gui_birdStore::onSure()
	{
		switch (m_storeItem.status)
		{
		case 1:
			if (UICommon::checkMoney(m_storeItem.price , m_storeItem.currencyType))
			{
				ClientNetwork::Instance()->getSender()->sendBirdStoreOperation(m_storeItem.storeId, m_storeItem.id, 1);
			}
			break;
		case 2:
			ClientNetwork::Instance()->getSender()->sendBirdStoreOperation(m_storeItem.storeId, m_storeItem.id, 2);
			break;
		}
	}

}
