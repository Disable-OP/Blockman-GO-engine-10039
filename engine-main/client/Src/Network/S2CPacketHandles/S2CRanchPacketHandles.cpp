#include "S2CRanchPacketHandles.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "cWorld/Blockman.h"
#include "World/World.h"
#include "Util/ClientEvents.h"
#include "World/Ranch.h"
#include "Setting/GuideSetting.h"
#include "game.h"
#include "ShellInterface.h"


using namespace BLOCKMAN;

void S2CRanchPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncRanchInfo>& packet)
{
	auto player = BLOCKMAN::Blockman::Instance()->m_pPlayer;
	if (player && player->m_ranch)
	{
		String plantKey = StringUtil::Format("isFirstPlayPlant_%lld", GameClient::CGame::Instance()->getPlatformUserId());
		if (!player->m_ranch->getInfo() && packet->m_info.level == 1&& GameClient::CGame::Instance()->getShellInterface()->getBoolForKey(plantKey, true))
		{
			auto guideSetting = GuideSetting::getGuideVideoItem(1);
			if (guideSetting)
			{
				GameClient::CGame::Instance()->getShellInterface()->playVideo(guideSetting->type, GuideSetting::getGuideVideoPath(1));
				GameClient::CGame::Instance()->getShellInterface()->putBoolForKey(plantKey, false);
			}
		}
		String orderKey = StringUtil::Format("isFirstPlayOrder_%lld", GameClient::CGame::Instance()->getPlatformUserId());
		if (player->m_ranch->getInfo() && player->m_ranch->getInfo()->level < 5  && packet->m_info.level == 5 && GameClient::CGame::Instance()->getShellInterface()->getBoolForKey(orderKey, true))
		{
			auto guideSetting = GuideSetting::getGuideVideoItem(3);
			if (guideSetting)
			{
				GameClient::CGame::Instance()->getShellInterface()->playVideo(guideSetting->type, GuideSetting::getGuideVideoPath(3));
				GameClient::CGame::Instance()->getShellInterface()->putBoolForKey(orderKey, false);
			}
		}

		String friendKey = StringUtil::Format("isFirstPlayFriend_%lld", GameClient::CGame::Instance()->getPlatformUserId());
		if (player->m_ranch->getInfo() && player->m_ranch->getInfo()->level < 6 && packet->m_info.level == 6 && GameClient::CGame::Instance()->getShellInterface()->getBoolForKey(friendKey, true))
		{
			auto guideSetting = GuideSetting::getGuideVideoItem(4);
			if (guideSetting)
			{
				GameClient::CGame::Instance()->getShellInterface()->playVideo(guideSetting->type, GuideSetting::getGuideVideoPath(4));
				GameClient::CGame::Instance()->getShellInterface()->putBoolForKey(friendKey, false);
			}
		}

		player->m_ranch->setInfo(&packet->m_info);
		RanchDataChangeEvent::emit();
	}
}

void S2CRanchPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncRanchHouse>& packet)
{
	auto player = BLOCKMAN::Blockman::Instance()->m_pPlayer;
	if (player && player->m_ranch)
	{
		player->m_ranch->setHouse(&packet->m_house);
		RanchHouseChangeEvent::emit();
	}
}

void S2CRanchPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncRanchStorage>& packet)
{
	auto player = BLOCKMAN::Blockman::Instance()->m_pPlayer;
	if (player && player->m_ranch)
	{
		player->m_ranch->setStorage(&packet->m_storage);
		RanchStorageChangeEvent::emit();
	}
}

void S2CRanchPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncRanchRank>& packet)
{
	auto player = BLOCKMAN::Blockman::Instance()->m_pPlayer;
	if (player && player->m_ranch)
	{
		player->m_ranch->setRankByType(packet->m_rankType, packet->m_ranks);
		RanchRankChangeEvent::emit();
	}
}

void S2CRanchPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncRanchBuild>& packet)
{
	auto player = BLOCKMAN::Blockman::Instance()->m_pPlayer;
	if (player && player->m_ranch)
	{
		player->m_ranch->setBuildByType(packet->m_buildType, packet->m_builds);
		switch ((RanchBuildType)packet->m_buildType) {
		case RanchBuildType::PLANT:
		case RanchBuildType::FARMING:
			RanchFarmingBuildChangeEvent::emit(packet->m_buildType);
			break;
		case RanchBuildType::FACTORY:
			RanchFactoryBuildChangeEvent::emit();
			break;
		}
		
	}
}

void S2CRanchPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncRanchOrder>& packet)
{
	auto player = BLOCKMAN::Blockman::Instance()->m_pPlayer;
	if (player && player->m_ranch)
	{
		player->m_ranch->setOrders(packet->m_orders);
		RanchOrderChangeEvent::emit();
	}
}

void S2CRanchPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncRanchAchievement>& packet)
{
	auto player = BLOCKMAN::Blockman::Instance()->m_pPlayer;
	if (player && player->m_ranch)
	{
		player->m_ranch->setAchievements(packet->m_achievements);
		RanchAchievementChangeEvent::emit();
	}
}

void S2CRanchPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncRanchTimePrice>& packet)
{
	auto player = BLOCKMAN::Blockman::Instance()->m_pPlayer;
	if (player && player->m_ranch)
	{
		player->m_ranch->setTimePrices(packet->m_timePrices);
	}
}

void S2CRanchPacketHandles::handlePacket(std::shared_ptr<S2CPacketRanchGain>& packet)
{
	BLOCKMAN::Blockman::Instance()->m_pPlayer->playSoundByType(ST_GetItem);
	ShowRanchGainTipEvent::emit(packet->items);
}

void S2CRanchPacketHandles::handlePacket(std::shared_ptr<S2CPacketRanchUnlockItem>& packet)
{
	ShowRanchUnlockItemsTipEvent::emit(packet->items);
}

void S2CRanchPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncRanchShortcutFreeTimes>& packet)
{
	auto player = BLOCKMAN::Blockman::Instance()->m_pPlayer;
	if (player && player->m_ranch)
	{
		map<i32, i32>::type items = map<i32, i32>::type();
		for (const auto& item : packet->m_shortcutFreeTimes)
		{
			items[item[0]] = item[1];
		}
		player->m_ranch->setShortcutFreeTimes(items);
	}
}


