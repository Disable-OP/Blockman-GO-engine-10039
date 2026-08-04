/********************************************************************
filename: 	AutoRegisterPacketC2S
file path:	H:\sandboxol\client\blockmango-client\dev\common\Src\Network\protocol\AutoRegisterPacketC2S.h

version:	1
author:		qutianxiang
company:    sandboxol
date:		2017/07/27
*********************************************************************/
#ifndef __AUTOREGISTERPACKETC2S_H__
#define __AUTOREGISTERPACKETC2S_H__
#include "Network/protocol/DataPacketFactory.h"

class AutoRegisterC2S
{
public:
	template< C2SPacketType pkType >
	typename std::enable_if<pkType != C2SPacketType::ProtocolBegin, void>::type autoRegister()
	{
		autoRegister<pkType - 1>();
		realRegister<pkType, C2SPacket<pkType> >();
	};

	template<C2SPacketType pkType, typename = typename std::enable_if<pkType == C2SPacketType::ProtocolBegin, void>::type>
	void autoRegister()
	{

	};

	template<C2SPacketType pkType, typename T>
	typename std::enable_if< IsClassDefined<T>::value, void>::type  realRegister()
	{
		C2SPacket<pkType> packet(BLOCKMAN::void_value);
		C2SDataPacketFactory::getInstance()->registerCreator<T>(static_cast<ui16>(pkType));
	}

	template<C2SPacketType pkType, typename T,
		typename = typename std::enable_if< !IsClassDefined<T>::value, void>::type >
		void realRegister()
	{
	};
};

#endif
