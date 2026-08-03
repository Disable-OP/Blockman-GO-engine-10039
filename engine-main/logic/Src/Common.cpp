#include "Common.h"
#include "Log/Log.h"

namespace BLOCKMAN
{
	GameCommon::GameCommon()
	{
		m_commonPacketSender = LORD::make_shared<CommonPacketSender>();
	}

	GameCommon::~GameCommon()
	{

	}

	void GameCommon::setCommonPacketSender(std::shared_ptr<CommonPacketSender> & sender)
	{
		m_commonPacketSender = sender;
	}

	void GameCommon::logIfExpired(ui32 startTime, ui32 endTime, ui32 threshold, std::string msg)
	{
#if LORD_PLATFORM == LORD_PLATFORM_LINUX
		if (endTime - startTime >= threshold) {
			LordLogWarning("===runtime=== %s slow, threshold %u ms, cost %u ms", msg.c_str(), threshold, endTime - startTime);
		}
#endif
	}

}
