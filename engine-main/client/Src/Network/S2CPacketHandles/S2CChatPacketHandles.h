/********************************************************************
filename: 	S2CInitPacketHandles
file path:	H:\sandboxol\client\blockmango-client\dev\client\Src\Network\S2CPacketHandles\S2CInitPacketHandles.h

version:	1
author:		qutianxiang
company:    sandboxol
date:		2017/07/25
*********************************************************************/
#ifndef __S2CCHATPACKETHANDLES_H__
#define __S2CCHATPACKETHANDLES_H__
#include <memory>
#include "Network/protocol/S2CPackets.h"

class S2CChatPacketHandles
{
public:
	static void handlePacket(std::shared_ptr<S2CPacketChatString>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketChatTranslation>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketChatNotification>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketGameTipNotification>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketKillMsg>& packet);
};


#endif
