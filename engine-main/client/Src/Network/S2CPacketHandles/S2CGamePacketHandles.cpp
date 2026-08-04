#include "S2CGamePacketHandles.h"
#include "Util/ClientEvents.h"
#include "Setting/LogicSetting.h"

using namespace BLOCKMAN;

void S2CGamePacketHandles::handlePacket(std::shared_ptr<S2CPacketCarDirection>& packet)
{
	UpdateCarDirectionEvent::emit(packet->type, packet->direction);
}

void S2CGamePacketHandles::handlePacket(std::shared_ptr<S2CPacketCarProgress>& packet)
{
	UpdateCarProgressEvent::emit(packet->type, packet->progress);
}

void S2CGamePacketHandles::handlePacket(std::shared_ptr<S2CPacketEnableAutoShoot>& packet)
{
	LogicSetting::Instance()->setAutoShootEnable(packet->enable);
}
