/********************************************************************
filename: 	AutoRegisterPacketS2C
file path:	H:\sandboxol\client\blockmango-client\dev\common\Src\Network\protocol\AutoRegisterPacketS2C.h

version:	1
author:		qutianxiang
company:    sandboxol
date:		2017/07/27
*********************************************************************/
#ifndef __AUTOREGISTERPACKETS2C_H__
#define __AUTOREGISTERPACKETS2C_H__
#include "Network/protocol/DataPacketFactory.h"
class AutoRegisterS2C
{
public:
	template< S2CPacketType pkType >
	typename std::enable_if<pkType != S2CPacketType::ProtocolBegin, void>::type autoRegister();

	template<S2CPacketType pkType, typename = typename std::enable_if<pkType == S2CPacketType::ProtocolBegin, void>::type>
	void autoRegister() {};

	template<S2CPacketType pkType, typename T>
	typename std::enable_if< IsClassDefined<T>::value, void>::type  realRegister();

	template<S2CPacketType pkType, typename T,
		typename = typename std::enable_if< !IsClassDefined<T>::value, void>::type >
		void realRegister() {};
};

template< S2CPacketType pkType >
typename std::enable_if<pkType != S2CPacketType::ProtocolBegin, void>::type
AutoRegisterS2C::autoRegister()
{
	autoRegister<pkType - 1>();
	realRegister<pkType, S2CPacket<pkType> >();
};

template<S2CPacketType pkType, typename T>
typename std::enable_if< IsClassDefined<T>::value, void>::type
AutoRegisterS2C::realRegister()
{
	S2CPacket<pkType> packet(BLOCKMAN::void_value);
	S2CDataPacketFactory::getInstance()->registerCreator<T>(static_cast<ui16>(pkType));
}
#endif
