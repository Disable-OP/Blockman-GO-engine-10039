/********************************************************************
filename: 	S2CPacketDispatch
file path:	H:\sandboxol\client\blockmango-client\dev\client\Src\Network\S2CPacketDispatch.h

version:	1
author:		qutianxiang
company:    sandboxol
date:		2017/07/26
*********************************************************************/
#ifndef __S2CPACKETDISPATCH_H__
#define __S2CPACKETDISPATCH_H__

#include "cWorld/Blockman.h"
#include "cWorld/WorldClient.h"

#include "Core.h"
#include "Network/protocol/S2CPackets.h"
#include "Network/PacketHandlerRegistry.h"
#include "Network/ClientNetwork.h"

template<typename T, S2CPacketType type>
struct HasS2CPacketHandle
{
	typedef void(*handleFucPtr)(std::shared_ptr< S2CPacket<type> >&);

	template<typename U, handleFucPtr ptr> struct HELPS;
	template<typename U> static char Test(HELPS<U, &U::handlePacket>*);
	template<typename U> static int Test(...);
	const static bool value = sizeof(Test<T>(0)) == sizeof(char);
};

class S2CPacketDispatch : public PacketHandlerRegistry<S2CPacketType, S2CPacket>, public ObjectAlloc
{
public:
	template<typename Packet, typename T>
	using MethodCallback = void (T::*)(std::shared_ptr<Packet>);

public:
	S2CPacketDispatch() {}
	~S2CPacketDispatch() {}

	template<typename T, S2CPacketType type>
	void registerHandle(MethodCallback<S2CPacket<type>, T>methodCallback,  T* thisPtr);

	template<typename T>
	void registerClass() {
		registerClassHelp< T, S2CPacketType::ProtocolEnd, false>();
	}

private:

	template<typename T, S2CPacketType type, bool has>
	typename std::enable_if<type != S2CPacketType::ProtocolBegin, void>::type registerClassHelp();

	template<typename T, S2CPacketType type, bool has, 
		typename = typename std::enable_if<type == S2CPacketType::ProtocolBegin, bool>::type >
	void registerClassHelp() {};

	template<typename T, S2CPacketType type, bool has>
	typename std::enable_if<has, void>::type registerClassMember();

	template<typename T, S2CPacketType type, bool has,
		typename = typename std::enable_if<!has, void>::type>
	void registerClassMember() {};

};


template<typename T, S2CPacketType type>
inline
void S2CPacketDispatch::registerHandle(MethodCallback<S2CPacket<type>, T>methodCallback, T* thisPtr)
{
	registerHandler<type>(std::bind(methodCallback, thisPtr));
}


template<typename T, S2CPacketType type, bool has>
inline
typename std::enable_if<type != S2CPacketType::ProtocolBegin, void>::type S2CPacketDispatch::registerClassHelp()
{
	
	registerClassHelp<T, (type - 1),
							HasS2CPacketHandle<T, (type - 1)>::value >();

	registerClassMember<T, type, has>();
	
}

template<typename T, S2CPacketType type, bool has>
inline 
typename std::enable_if<has, void>::type  S2CPacketDispatch::registerClassMember()
{
	typedef void(*handleFuncPtr)(std::shared_ptr < S2CPacket<type> > &);
	registerHandler<type>((handleFuncPtr)&T::handlePacket);
}

#endif // !__ClientNetPlayer_H__

