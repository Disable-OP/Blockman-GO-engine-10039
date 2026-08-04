#ifndef __PACKEET_HANDLER_REGISTRY_HEADER__
#define __PACKEET_HANDLER_REGISTRY_HEADER__

#include <functional>
#include "Network/protocol/PacketDefine.h"

 
template<typename enumtype ,template<enumtype> class  CPacket>
class PacketHandlerRegistry
{
public:
	template<typename Packet, typename RET=void>
	using Callback = std::function<RET(std::shared_ptr<Packet>&)>;

protected:
	vector<Callback<DataPacket>>::type PacketHandlers;

public:
	template<enumtype type>
	inline void registerHandler(Callback<CPacket<type>> callback);

	void handlePacket(std::shared_ptr<DataPacket> &packet);
};

template<typename enumtype, template<enumtype> class  CPacket>
inline
void PacketHandlerRegistry<enumtype, CPacket>::handlePacket(std::shared_ptr<DataPacket> &packet)
{
	auto index = packet->pid();
	if (PacketHandlers.size() < (size_t)(index + 1))
	{
		LordLogError("packet id exceed handlers size, handler size=%u, recv pid=%u", PacketHandlers.size(), (size_t)index);
		return;
	}
	const auto& callback = PacketHandlers[index];
	if (!callback)
	{
		LordLogError("can not find handler for packet id=%u", (size_t)index);
		return;
	}
	callback(packet);
}

template<typename enumtype, template<enumtype> class  CPacket>
template<enumtype type>
inline 
void PacketHandlerRegistry<enumtype, CPacket>::registerHandler(Callback<CPacket<type>> callback)
{
	auto index = static_cast<int>(type);
	if (PacketHandlers.size() < (size_t)(index + 1))
	{
		PacketHandlers.resize(index + 1);
	}

	PacketHandlers[static_cast<int>(type)] = [callback](std::shared_ptr<DataPacket> packet)
	{
		auto derived = std::dynamic_pointer_cast<CPacket<type>>(packet);
		if (!derived)
		{
			LordLogError("Failed to cast packet to %s", typeid(CPacket<type>).name());
		}
		callback(derived);
	};
};

#endif