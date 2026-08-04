/********************************************************************
filename: 	S2CInitPacketHandles
file path:	H:\sandboxol\client\blockmango-client\dev\client\Src\Network\S2CPacketHandles\S2CInitPacketHandles.h

version:	1
author:		qutianxiang
company:    sandboxol
date:		2017/07/25
*********************************************************************/
#ifndef __S2CINITPACKETHANDLES_H__
#define __S2CINITPACKETHANDLES_H__
#include <memory>
#include "Network/protocol/S2CPackets.h"


#include <stdexcept>
#include <Std/iterator.h>

class S2CInitPacketHandles
{
public:
	static void handlePacket(std::shared_ptr<S2CPacketGameInfo>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketLoginResult>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketCheckCSVersionResult>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketEnterOtherGame>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketAppExpResult>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketLogicSettingInfo>& packet);
};
#endif
