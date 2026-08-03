#include "gui_playerOperation.h"
#include "Util/LanguageKey.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "World/Manor.h"
#include "ShellInterface.h"
#include "game.h"
#include "GUI/RootGuiLayout.h"
#include "Setting/GameTypeSetting.h"

using namespace LORD;

namespace BLOCKMAN
{

	gui_playerOperation::gui_playerOperation()
		: gui_layout("PlayerOperation.json")
	{

	}

	gui_playerOperation::~gui_playerOperation()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_playerOperation::onLoad()
	{
		getWindow<GUIButton>("PlayerOperation-Add-Friend")->subscribeEvent(EventButtonClick, std::bind(&gui_playerOperation::onClick, this,std::placeholders::_1, ViewId::BTN_ADD_FRIEND ));
		getWindow<GUIButton>("PlayerOperation-Give-Rose")->subscribeEvent(EventButtonClick, std::bind(&gui_playerOperation::onClick, this, std::placeholders::_1, ViewId::BTN_GIVE_ROSE));
		getWindow<GUIButton>("PlayerOperation-Call-On")->subscribeEvent(EventButtonClick, std::bind(&gui_playerOperation::onClick, this, std::placeholders::_1, ViewId::BTN_CALL_ON));
		getWindow<GUIButton>("PlayerOperation-Greetings")->subscribeEvent(EventButtonClick, std::bind(&gui_playerOperation::onClick, this, std::placeholders::_1, ViewId::BTN_GREETINGS));

		getWindow("PlayerOperation-Add-Friend-Name")->SetText(getString(LanguageKey::GUI_PLAYER_LIST_ITEM_ADD_FRIEND));
		getWindow("PlayerOperation-Give-Rose-Name")->SetText(getString(LanguageKey::GUI_MANOR_GIVE_ROSE));
		getWindow("PlayerOperation-Call-On-Name")->SetText(getString(LanguageKey::GUI_MANOR_CALL_ON));
		getWindow("PlayerOperation-Greetings-Name")->SetText(getString(LanguageKey::GUI_PLAYER_OPERATION_GREETINGS));
		m_subscriptionGuard.add(ServerSyncGameTypeEvent::subscribe(std::bind(&gui_playerOperation::onGameTypeUpdate, this)));
		onGameTypeUpdate();
	}

	void gui_playerOperation::onShow()
	{
	}

	void gui_playerOperation::onUpdate(ui32 nTimeElapse)
	{
		
	}

	bool gui_playerOperation::onClick(const EventArgs & events, ViewId viewId)
	{
		switch (viewId)
		{
		case ViewId::BTN_ADD_FRIEND:
			GameClient::CGame::Instance()->getShellInterface()->onFriendOperation(2, m_targetUserId);
			break;
		case ViewId::BTN_GIVE_ROSE:
			ClientNetwork::Instance()->getSender()->sendGiveRose(m_targetUserId);
			break;
		case ViewId::BTN_CALL_ON:
			ClientNetwork::Instance()->getSender()->sendCallOnManor(m_targetUserId);
			break;
		case ViewId::BTN_GREETINGS:
			Blockman::Instance()->m_pPlayer->setSkillAnimation(ActionState::AS_GREETINGS);
			Blockman::Instance()->m_pPlayer->setSkillAnimationTime(1000);
			LORD::vector<String>::type args;
			args.push_back(Blockman::Instance()->m_pPlayer->getEntityName());
			ClientNetwork::Instance()->getSender()->sendPlayerInteractionActionMesage(m_targetUserId, LanguageKey::GUI_PLAYER_OPERATION_GREETINGS_MESSAGE, args);
			String msg = StringUtil::Format(getString("gui_player_operation_greetings_message_for_me").c_str(), m_targetUserName.c_str());
			ChatMessageReceiveEvent::emit(msg);
			break;
		}
		RootGuiLayout::Instance()->hidePlayerOperation();
		return true;
	}

	bool gui_playerOperation::onGameTypeUpdate()
	{
		switch (GameClient::CGame::Instance()->GetGameType())
		{
		case ClientGameType::Tiny_Town:
			getWindow<GUIButton>("PlayerOperation-Greetings")->SetVisible(false);
			break;
		case ClientGameType::Jewel_Knight:
			getWindow<GUIButton>("PlayerOperation-Give-Rose")->SetVisible(false);
			getWindow<GUIButton>("PlayerOperation-Call-On")->SetVisible(false);
			break;
		default:
			getWindow<GUIButton>("PlayerOperation-Give-Rose")->SetVisible(false);
			getWindow<GUIButton>("PlayerOperation-Call-On")->SetVisible(false);
			getWindow<GUIButton>("PlayerOperation-Greetings")->SetVisible(false);
			break;
		}
		return true;
	}

}
