/********************************************************************
filename: 	C2SPacketDispatcher
file path:	H:\sandboxol\client\blockmango-client\dev\server\src\Network\C2SPacketDispatcher.h

version:	1
author:		qutianxiang
company:    sandboxol
date:		2017/07/26
*********************************************************************/
#ifndef __C2SPACKETDISPATCHER_H__
#define __C2SPACKETDISPATCHER_H__

#include "Network/PacketHandlerRegistry.h"
#include "Network/ClientPeer.h"
#include "ServerNetwork.h"


template<typename T, C2SPacketType type>
struct HasC2SPacketHandle
{
	typedef void(*handleFucPtr)(std::shared_ptr<ClientPeer>&, std::shared_ptr< C2SPacket<type> >&);

	template<typename U, handleFucPtr ptr> struct HELPS;
	template<typename U> static char Test(HELPS<U, &U::handlePacket>*);
	template<typename U> static int Test(...);
	const static bool value = sizeof(Test<T>(0)) == sizeof(char);
};

class C2SPacketDispatcher :public PacketHandlerRegistry<C2SPacketType, C2SPacket>, public ObjectAlloc
{
public:
	template<typename Packet, typename T>
	using C2SMethodCallBack = void (T::*)(std::shared_ptr<ClientPeer>&, std::shared_ptr<Packet>&);

	template<typename Packet, typename RET = void>
	using C2SCallBack = RET(*)(std::shared_ptr<ClientPeer>&, std::shared_ptr<Packet>&);

	using PreHandle = std::function<bool(std::shared_ptr<ClientPeer>&, std::shared_ptr<DataPacket>&)>;

	C2SPacketDispatcher() {}
	virtual ~C2SPacketDispatcher() {}

	///////

private:
	PreHandle m_preHandle;

public:
	template<C2SPacketType type, typename T>
	void registerHandler(C2SMethodCallBack<C2SPacket<type>, T> callback, T* thisPtr);

	template<C2SPacketType type>
	void registerHandler(C2SCallBack<C2SPacket<type>> callback);

	template<typename T>
	void registerClass() {
		registerClassHelp< T, C2SPacketType::ProtocolEnd, false>();
	}

	void setPreHandle(PreHandle handle) { m_preHandle = handle; };


private:
	template<typename T, C2SPacketType type, bool has>
	typename std::enable_if<type != C2SPacketType::ProtocolBegin, void>::type registerClassHelp();

	template<typename T, C2SPacketType type, bool has,
		typename = typename std::enable_if<type == C2SPacketType::ProtocolBegin, bool>::type >
		void registerClassHelp() {};

	template<typename T, C2SPacketType type, bool has>
	typename std::enable_if<has, void>::type registerClassMember();

	template<typename T, C2SPacketType type, bool has,
		typename = typename std::enable_if<!has, void>::type>
		void registerClassMember() {};
};


template<C2SPacketType type, typename T>
inline void C2SPacketDispatcher::registerHandler(C2SMethodCallBack<C2SPacket<type>, T> callback, T* thisPtr)
{
	auto index = static_cast<int>(type);
	if (PacketHandlers.size() < index + 1)
	{
		PacketHandlers.resize(index + 1);
	}

	PacketHandlers[index] = [callback, thisPtr, this](std::shared_ptr<DataPacket> packet)
	{
		auto c2sPacket = std::dynamic_pointer_cast<C2SPacket<type>>(packet);
		if (!c2sPacket)
		{
			LordLogError("Failed to cast packet to %s", typeid(C2SPacket<type>).name());
		}

		std::shared_ptr<ClientPeer> clientPeer =
			ServerNetwork::Instance()->findPlayerByRakssid(c2sPacket->getSender());

		if (m_preHandle && !m_preHandle(clientPeer, packet))
			return;

		thisPtr->callback(clientPeer, c2sPacket);
	};

}


template<C2SPacketType type>
inline void C2SPacketDispatcher::registerHandler(C2SCallBack<C2SPacket<type>> callback)
{
	auto index = static_cast<int>(type);
	if (PacketHandlers.size() < (size_t)index + 1)
	{
		PacketHandlers.resize(index + 1);
	}

	PacketHandlers[index] = [callback, this](std::shared_ptr<DataPacket> packet)
	{
		auto c2sPacket = std::dynamic_pointer_cast<C2SPacket<type>>(packet);
		if (!c2sPacket)
		{
			LordLogError("Failed to cast packet to %s", typeid(C2SPacket<type>).name());
		}

		std::shared_ptr<ClientPeer> clientPeer =
			ServerNetwork::Instance()->findPlayerByRakssid(c2sPacket->getSender());
		if (m_preHandle && !m_preHandle(clientPeer, packet))
			return;

		callback(clientPeer, c2sPacket);
	};
}

template<typename T, C2SPacketType type, bool has>
inline
typename std::enable_if<type != C2SPacketType::ProtocolBegin, void>::type C2SPacketDispatcher::registerClassHelp()
{

	registerClassHelp<T, (type - 1),
		HasC2SPacketHandle<T, (type - 1)>::value >();

	registerClassMember<T, type, has>();

}

template<typename T, C2SPacketType type, bool has>
inline
typename std::enable_if<has, void>::type  C2SPacketDispatcher::registerClassMember()
{
	typedef void(*handleFuncPtr)(std::shared_ptr<ClientPeer>&, std::shared_ptr < C2SPacket<type> > &);
	registerHandler<type>((handleFuncPtr)&T::handlePacket);
}
#endif
