#include "gui_buildwarGuess.h"
#include "Util/LanguageKey.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "game.h"
#include "External/cjson.h"
#include "GUIBuildWarGuessItem.h"
#include "GUI/GuiDef.h"

using namespace LORD;

namespace BLOCKMAN
{

	gui_buildwarGuess::gui_buildwarGuess()
		: gui_layout("BuildWarGuess.json")
	{
	}

	gui_buildwarGuess::~gui_buildwarGuess()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_buildwarGuess::onLoad()
	{
		m_List = getWindow<GUIListBox>("BuildWarGuess-List");
		m_subscriptionGuard.add(ShowBuildWarGuessUi::subscribe(std::bind(&gui_buildwarGuess::RefreshAllInfo, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(SetBuildWarGuessShow::subscribe(std::bind(&gui_buildwarGuess::setIsNeedShow, this, std::placeholders::_1)));
		getWindow<GUIStaticText>("BuildWarGuess-Title")->SetText(getString(LanguageKey::GUI_BUILD_WAR_GUESS_TITLE).c_str());
	}

	void gui_buildwarGuess::onUpdate(ui32 nTimeElapse)
	{
		if (!isShown() && m_isNeedShow)
		{
			if (!getParent()->isChatShown() && !getParent()->isMenuShown() && !getParent()->isAppShopShown() && !getParent()->isFinalSummaryShown() && !getParent()->isLoading())
			{
				getParent()->showBuildWarGuessUi(true);
			}
		}
	}

	bool gui_buildwarGuess::RefreshAllInfo(String info, int guess_room_id)
	{
		setIsNeedShow(true);
		std::vector<PlayerInfo> player_list;
		std::vector<int> grade_room;

		if (getGuessInfo(info, player_list, grade_room))
		{
			m_List->ClearAllItem();
			m_List->SetItemHeight(55.0f);

			for (auto player : player_list)
			{
				bool is_heart = false;
				for (auto grade : grade_room)
				{
					if (grade == player.room_id)
					{
						is_heart = true;
						break;
					}
				}
				GuiBuildWarGuessItem* pBuildWarGuessItem = dynamic_cast<GuiBuildWarGuessItem*>(GUIWindowManager::Instance()->CreateGUIWindow(GWT_BUILDWAR_GUESS_ITEM, GUIString("BuildWarGuessItem") + StringUtil::ToString(player.room_id).c_str()));
				pBuildWarGuessItem->RefreshItem(is_heart, player.room_id, player.name, guess_room_id, player.user_id);
				pBuildWarGuessItem->SetPosition(UVector2(UDim(0.0f, 0.0f), UDim(0.0f, 0.0f)));
				pBuildWarGuessItem->SetSize(UVector2(UDim(1.0f, 0.0f), UDim(0.0f, 55.0f)));
				m_List->AddItem(pBuildWarGuessItem, true);
			}

			m_List->SetAllowScroll(true);
			m_List->SetTouchable(true);
		}
		
		return true;
	}

	bool gui_buildwarGuess::getGuessInfo(const String& info, std::vector<PlayerInfo>& player_list, std::vector<int>& grade_room)
	{
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(info.c_str());

		if (doc->HasParseError())
		{
			LordLogError("gui_buildwarGuess::getGuessInfo HasParseError.");
			LordDelete(doc);
			return false;
		}

		if (!doc->HasMember("player_info") || !doc->HasMember("grade_room"))
		{
			LordLogError("gui_buildwarGuess::getGuessInfo The info result content missed some field.");
			LordDelete(doc);
			return false;
		}

		rapidjson::Value arr = doc->FindMember("player_info")->value.GetArray();
		for (size_t i = 0; i < arr.Size(); ++i)
		{
			if (!arr[i].HasMember("room_id") || !arr[i].HasMember("name") || !arr[i].HasMember("platformUid"))
			{
				LordLogError("gui_buildwarGuess::getGuessInfo The game result content player_info missed some field. %d", i);
				LordDelete(doc);
				return false;
			}

			String playerName = arr[i].FindMember("name")->value.GetString();
			int roomId = arr[i].FindMember("room_id")->value.GetInt();
			ui64 user_id = arr[i].FindMember("platformUid")->value.GetInt64();

			PlayerInfo playerInfo;
			playerInfo.name = playerName;
			playerInfo.room_id = roomId;
			// playerInfo.user_id = StringUtil::ParseI64(user_id, 0);
			playerInfo.user_id = user_id;
			player_list.push_back(playerInfo);
		}

		rapidjson::Value arrG = doc->FindMember("grade_room")->value.GetArray();
		for (size_t i = 0; i < arrG.Size(); ++i)
		{
			if (!arrG[i].HasMember("grade"))
			{
				LordLogError("gui_buildwarGuess::getGuessInfo The game result content grade_room missed some field. %d", i);
				LordDelete(doc);
				return false;
			}

			int grade = arrG[i].FindMember("room_id")->value.GetInt();
			grade_room.push_back(grade);
		}


		LordDelete(doc);
		return true;
	}

}
