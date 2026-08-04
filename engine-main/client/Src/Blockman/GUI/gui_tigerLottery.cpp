#include "gui_tigerLottery.h"
#include "rapidjson/document.h"
#include "Util/Random.h"
#include "Util/LanguageKey.h"
#include "game.h"

namespace BLOCKMAN
{
	gui_tigerLottery::gui_tigerLottery() :
		gui_layout("TigerLottery.json")
	{
	}

	gui_tigerLottery::~gui_tigerLottery()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_tigerLottery::onLoad()
	{
		m_item1_scroll = getWindow<GUILayout>("TigerLottery-Item1-Scroll");
		m_item1_name = getWindow<GUIStaticText>("TigerLottery-Item1-Name");
		m_item1_mask = getWindow<GUIStaticImage>("TigerLottery-Item1-Mask");
		m_item1_lock = getWindow<GUIStaticImage>("TigerLottery-Item1-Lock");
		m_item1_choose = getWindow<GUIButton>("TigerLottery-Item1-Choose");

		m_item2_scroll = getWindow<GUILayout>("TigerLottery-Item2-Scroll");
		m_item2_name = getWindow<GUIStaticText>("TigerLottery-Item2-Name");
		m_item2_mask = getWindow<GUIStaticImage>("TigerLottery-Item2-Mask");
		m_item2_lock = getWindow<GUIStaticImage>("TigerLottery-Item2-Lock");
		m_item2_choose = getWindow<GUIButton>("TigerLottery-Item2-Choose");

		m_item3_scroll = getWindow<GUILayout>("TigerLottery-Item3-Scroll");
		m_item3_name = getWindow<GUIStaticText>("TigerLottery-Item3-Name");
		m_item3_mask = getWindow<GUIStaticImage>("TigerLottery-Item3-Mask");
		m_item3_lock = getWindow<GUIStaticImage>("TigerLottery-Item3-Lock");
		m_item3_choose = getWindow<GUIButton>("TigerLottery-Item3-Choose");

		m_lottery_again = getWindow<GUIButton>("TigerLottery-Lottery-Again");
		m_countdown = getWindow<GUIStaticText>("TigerLottery-Countdown");

		m_subscriptionGuard.add(TigerLotteryDataEvent::subscribe(std::bind(&gui_tigerLottery::onLotteryData, this, std::placeholders::_1)));
		m_subscriptionGuard.add(TigerLotteryResultEvent::subscribe(std::bind(&gui_tigerLottery::onLotteryResult, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
	
		m_item1_choose->subscribeEvent(EventButtonClick, std::bind(&gui_tigerLottery::onSelectFirst, this, std::placeholders::_1));
		m_item2_choose->subscribeEvent(EventButtonClick, std::bind(&gui_tigerLottery::onSelectSecond, this, std::placeholders::_1));
		m_item3_choose->subscribeEvent(EventButtonClick, std::bind(&gui_tigerLottery::onSelectThird, this, std::placeholders::_1));
		m_lottery_again->subscribeEvent(EventButtonClick, std::bind(&gui_tigerLottery::onLotteryAgain, this, std::placeholders::_1));

		m_item1_choose->SetText(getString(LanguageKey::GUI_TIGER_LOTTERY_LOCK).c_str());
		m_item2_choose->SetText(getString(LanguageKey::GUI_TIGER_LOTTERY_LOCK).c_str());
		m_item3_choose->SetText(getString(LanguageKey::GUI_TIGER_LOTTERY_LOCK).c_str());
		m_lottery_again->SetText(getString(LanguageKey::GUI_TIGER_LOTTERY_AGAIN).c_str());

	}

	void gui_tigerLottery::onUpdate(ui32 nTimeElapse)
	{
		if (!isShown())
			return;

		bool isStartHasScroll = m_firstScroll || m_secondScroll || m_thirdScroll;

		if (m_firstScroll)
			m_firstScroll = onUpdateChild(m_firstId, m_firstThroughTimes, m_firstMoveSpeed, m_item1_childs, m_firstTopPosY);
		if (m_secondScroll)
			m_secondScroll = onUpdateChild(m_secondId, m_secondThroughTimes, m_secondMoveSpeed, m_item2_childs, m_secondTopPosY);
		if (m_thirdScroll)
			m_thirdScroll = onUpdateChild(m_thirdId, m_thirdThroughTimes, m_thirdMoveSpeed, m_item3_childs, m_thirdTopPosY);

		bool isEndHasScroll = m_firstScroll || m_secondScroll || m_thirdScroll;

		m_item1_name->SetVisible(!m_firstScroll);
		m_item2_name->SetVisible(!m_secondScroll);
		m_item3_name->SetVisible(!m_thirdScroll);

		if (isStartHasScroll != isEndHasScroll)
		{
			m_countdownTime = 10000;
		}

		if (m_countdownTime > 0)
		{
			m_countdownTime -= nTimeElapse;
			m_countdown->SetVisible(true);
			if (m_countdownTime == 0)
				m_countdownTime--;
			if(m_countdownTime > 0)
				m_countdown->SetText(StringUtil::ToString(m_countdownTime / 1000).c_str());
		}
		else
		{
			m_countdown->SetVisible(false);
		}

		if (m_countdownTime < 0)
		{
			m_countdownTime = 0;
			onSelectRandom();
		}
	}

	bool gui_tigerLottery::onUpdateChild(String selectId, int& throughTimes, float& moveSpeed, map<String, GUIStaticImage*>::type childs, float topPosY)
	{
		bool needSubSpeed = false;
		bool finded = false;
		float restoreY = 0.0f;
		int round = 30 / childs.size();
		for (auto iter = childs.begin(); iter != childs.end(); iter++)
		{
			auto child = iter->second;
			child->SetYPosition({ 0, child->GetYPosition().d_offset + moveSpeed });
			if (throughTimes >= round && iter->first == selectId)
			{
				if (child->GetYPosition().d_offset > 59)
				{
					restoreY = child->GetYPosition().d_offset - 59;
					finded = true;
				}
			}
			if (child->GetYPosition().d_offset > 332)
			{
				if (throughTimes >= round)
				{
					needSubSpeed = true;
				}
				child->SetYPosition({ 0, topPosY + (child->GetYPosition().d_offset - 332) });
				if (iter->first == selectId)
				{
					throughTimes++;
				}
			}
			child->SetVisible(child->GetYPosition().d_offset >= -100);
		}

		if (finded)
		{
			moveSpeed = 0;
		}

		if (needSubSpeed && moveSpeed > 0)
		{
			moveSpeed -= m_subMoveSpeed;
			moveSpeed = Math::Max(childs.size() * 0.2f, moveSpeed);
		}

		if (finded)
		{
			for (auto iter = childs.begin(); iter != childs.end(); iter++)
			{
				auto child = iter->second;
				child->SetYPosition({ 0, child->GetYPosition().d_offset - restoreY });
				if (child->GetYPosition().d_offset < topPosY)
				{
					child->SetYPosition({ 0, 332 - (topPosY - child->GetYPosition().d_offset) });
				}
			}
			return false;
		}
		else
		{
			return true;
		}
	}

	bool gui_tigerLottery::onLotteryData(const String & data)
	{
		m_items.clear();
		m_firstIds.clear();
		m_secondIds.clear();
		m_thirdIds.clear();

		if (data.length() == 0)
			return false;
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(data.c_str());

		if (doc->HasParseError())
		{
			LordLogError("The lottery data content HasParseError.");
			LordDelete(doc);
			return false;
		}

		if (doc->HasMember("items"))
		{
			auto doc_items = doc->FindMember("items");
			if (doc_items->value.IsArray())
			{
				auto items = doc_items->value.GetArray();
				for (auto& item : items)
				{
					if (!item.HasMember("id") || !item.HasMember("name") || !item.HasMember("image"))
					{
						continue;
					}
					String id = item.FindMember("id")->value.GetString();
					String name = item.FindMember("name")->value.GetString();
					String image = item.FindMember("image")->value.GetString();
					m_items.insert({ id, LotteryItem(id, name, image) });
				}
			}
		}

		if (doc->HasMember("firstIds"))
		{
			auto doc_firstIds = doc->FindMember("firstIds");
			if (doc_firstIds->value.IsArray())
			{
				auto firstIds = doc_firstIds->value.GetArray();
				for (auto& id : firstIds)
				{
					if (!id.IsString())
					{
						continue;
					}
					m_firstIds.push_back(id.GetString());
				}
			}
		}

		if (doc->HasMember("secondIds"))
		{
			auto doc_secondIds = doc->FindMember("secondIds");
			if (doc_secondIds->value.IsArray())
			{
				auto secondIds = doc_secondIds->value.GetArray();
				for (auto& id : secondIds)
				{
					if (!id.IsString())
					{
						continue;
					}
					m_secondIds.push_back(id.GetString());
				}
			}
		}

		if (doc->HasMember("thirdIds"))
		{
			auto doc_thirdIds = doc->FindMember("thirdIds");
			if (doc_thirdIds->value.IsArray())
			{
				auto thirdIds = doc_thirdIds->value.GetArray();
				for (auto& id : thirdIds)
				{
					if (!id.IsString())
					{
						continue;
					}
					m_thirdIds.push_back(id.GetString());
				}
			}
		}
		initAllChilds();
		return true;
	}

	void gui_tigerLottery::initAllChilds()
	{
		m_firstTopPosY = initItemChilds(m_item1_scroll, m_item1_childs, m_firstIds);
		m_secondTopPosY = initItemChilds(m_item2_scroll, m_item2_childs, m_secondIds);
		m_thirdTopPosY = initItemChilds(m_item3_scroll, m_item3_childs, m_thirdIds);
	}

	float gui_tigerLottery::initItemChilds(GUILayout* parent, map<String, GUIStaticImage*>::type& childs, list<String>::type ids)
	{
		parent->CleanupChildren();
		childs.clear();
		static int index = 0;
		float positionY = 332;
		for (auto id : ids)
		{
			auto iter = m_items.find(id);
			if (iter == m_items.end())
				continue;
			index++;
			positionY -= 273;
			LotteryItem item = iter->second;
			String vName = StringUtil::Format("Lottery-Item-Child-%d", index);
			GUIStaticImage* child = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, vName.c_str());
			child->SetHorizontalAlignment(HA_CENTRE);
			child->SetTouchable(false);
			child->SetArea({ 0, 0 }, { 0, positionY }, { 0, 155.0f }, { 0, 155.0f });
			child->SetImage(item.m_image.c_str());
			child->SetVisible(positionY >= -100);
			parent->AddChildWindow(child);
			childs.insert({ item.m_uniqueId, child });
		}
		return positionY;
	}

	bool gui_tigerLottery::onLotteryResult(const String& firstId, const String& secondId, const String& thirdId)
	{	
		m_firstId = firstId;
		m_secondId = secondId;
		m_thirdId = thirdId;
		
		m_firstThroughTimes = 0;
		m_secondThroughTimes = 0;
		m_thirdThroughTimes = 0;

		if (!m_items.empty())
		{
			m_subMoveSpeed = 100.0f / m_items.size() - 0.2f;
			if (m_subMoveSpeed <= 0.0f)
			{
				m_subMoveSpeed = 0.1f;
			}
		}

		m_firstMoveSpeed = m_firstIds.size() * m_subMoveSpeed;
		m_secondMoveSpeed = m_secondIds.size() * m_subMoveSpeed;
		m_thirdMoveSpeed = m_thirdIds.size() * m_subMoveSpeed;

		m_firstScroll = m_firstMoveSpeed > m_subMoveSpeed;
		m_secondScroll = m_secondMoveSpeed > m_subMoveSpeed;
		m_thirdScroll = m_thirdMoveSpeed > m_subMoveSpeed;

		bool isFirstLock = m_items.find(m_firstId) == m_items.end();
		bool isSecondLock = m_items.find(m_secondId) == m_items.end();
		bool isThirdLock = m_items.find(m_thirdId) == m_items.end();

		m_firstScroll = m_firstScroll && !isFirstLock;
		m_secondScroll = m_secondScroll && !isSecondLock;
		m_thirdScroll = m_thirdScroll && !isThirdLock;

		m_item1_mask->SetVisible(isFirstLock);
		m_item1_lock->SetVisible(isFirstLock);
		m_item1_choose->SetEnabled(!isFirstLock);
		m_item1_choose->SetText(getString(isFirstLock ? LanguageKey::GUI_TIGER_LOTTERY_LOCK : LanguageKey::GUI_TIGER_LOTTERY_CHOOSE).c_str());

		m_item2_mask->SetVisible(isSecondLock);
		m_item2_lock->SetVisible(isSecondLock);
		m_item2_choose->SetEnabled(!isSecondLock);
		m_item2_choose->SetText(getString(isSecondLock ? LanguageKey::GUI_TIGER_LOTTERY_LOCK : LanguageKey::GUI_TIGER_LOTTERY_CHOOSE).c_str());

		m_item3_mask->SetVisible(isThirdLock);
		m_item3_lock->SetVisible(isThirdLock);
		m_item3_choose->SetEnabled(!isThirdLock);
		m_item3_choose->SetText(getString(isThirdLock ? LanguageKey::GUI_TIGER_LOTTERY_LOCK : LanguageKey::GUI_TIGER_LOTTERY_CHOOSE).c_str());

		setLotteryResultName(m_firstId, m_firstIds, m_item1_name);
		setLotteryResultName(m_secondId, m_secondIds, m_item2_name);
		setLotteryResultName(m_thirdId, m_thirdIds, m_item3_name);

		m_countdownTime = 0;
		m_countdown->SetText("");
		m_lottery_again->SetVisible(m_firstScroll || m_secondScroll || m_thirdScroll);

		if ((m_firstMoveSpeed > 0 || m_secondMoveSpeed > 0 || m_thirdMoveSpeed > 0) && !(isFirstLock && isSecondLock && isThirdLock))
		{
			getParent()->showTigerLottery();
		}

		return true;
	}

	void gui_tigerLottery::setLotteryResultName(String uniqueId, list<String>::type ids, GUIStaticText* item)
	{
		auto iter = m_items.find(uniqueId);
		if (iter == m_items.end())
		{
			if (!ids.empty())
			{
				iter = m_items.find(*ids.begin());
				if (iter != m_items.end())
				{
					item->SetText(getString(iter->second.m_name).c_str());
				}
				else
				{
					item->SetText("");
				}
			}
			else
			{
				item->SetText("");
			}
		}
		else
		{
			item->SetText(getString(iter->second.m_name).c_str());
		}
	}

	void gui_tigerLottery::onSelectRandom()
	{
		vector<String>::type seed;
		if (m_items.find(m_firstId) != m_items.end())
			seed.push_back(m_firstId);
		if (m_items.find(m_secondId) != m_items.end())
			seed.push_back(m_secondId);
		if (m_items.find(m_thirdId) != m_items.end())
			seed.push_back(m_thirdId);
		if (!seed.empty())
		{
			Random random = Random();
			int index = random.nextIntRange(0, seed.size() - 1);
			String uniqueId = seed[index];
			selectItem(uniqueId);
		}
		getParent()->hideTigerLottery();
	}

	bool gui_tigerLottery::onSelectFirst(const EventArgs & events)
	{
		if (m_firstScroll)
			return false;
		selectItem(m_firstId);
		getParent()->hideTigerLottery();
		return true;
	}

	bool gui_tigerLottery::onSelectSecond(const EventArgs & events)
	{
		if (m_secondScroll)
			return false;
		selectItem(m_secondId);
		getParent()->hideTigerLottery();
		return true;
	}

	bool gui_tigerLottery::onSelectThird(const EventArgs & events)
	{
		if (m_thirdScroll)
			return false;
		selectItem(m_thirdId);
		getParent()->hideTigerLottery();
		return true;
	}

	bool gui_tigerLottery::onLotteryAgain(const EventArgs & events)
	{
		if (m_firstScroll || m_secondScroll || m_thirdScroll)
			return false;
		GameClient::CGame::Instance()->getNetwork()->getSender()->sendLotteryAgain();
		return true;
	}

	void gui_tigerLottery::selectItem(String uniqueId)
	{
		if (m_items.find(uniqueId) != m_items.end())
			GameClient::CGame::Instance()->getNetwork()->getSender()->sendSelectLotteryItem(uniqueId);
		m_firstId = "";
		m_secondId = "";
		m_thirdId = "";
	}

}