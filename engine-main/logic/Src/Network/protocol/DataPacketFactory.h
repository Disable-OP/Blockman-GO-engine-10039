/********************************************************************
filename: 	DataPacketFactory
file path:	H:\sandboxol\client\blockmango-client\dev\common\Src\network\packet\DataPacketFactory.h

version:	1
author:		qutianxiang
company:    sandboxol
date:		2017/07/05
*********************************************************************/
#ifndef __DATAPACEKTFACTORY_H__
#define __DATAPACEKTFACTORY_H__
#include "Util/UFactory.h"
#include "TypeDef.h"
#include "Network/protocol/PacketDefine.h"


class C2SDataPacketFactory :public UFactory<DataPacket, ui16>
{
public:
	static C2SDataPacketFactory *getInstance() {
		static C2SDataPacketFactory object;
		return &object;
	}

private:
	C2SDataPacketFactory(void) {};
	virtual ~C2SDataPacketFactory(void) {};
};

class S2CDataPacketFactory :public UFactory<DataPacket, ui16>
{
public:
	static S2CDataPacketFactory *getInstance() {
		static S2CDataPacketFactory object;
		return &object;
	}

private:
	S2CDataPacketFactory(void) {};
	virtual ~S2CDataPacketFactory(void) {};
};

struct DataPacketFactory
{
	static UFactory<DataPacket, ui16> * getInstance(bool isServer)
	{
		if (isServer)
			return C2SDataPacketFactory::getInstance();
		else
			return S2CDataPacketFactory::getInstance();
	}
};

template<typename T>
struct IsClassDefined
{	 
	template<typename U, typename  =  decltype(U()) >
    static char test(int);

	template<typename U>
	static int test(...);

	const static bool value = sizeof(test<T>(0)) == sizeof(char);
};


#endif // !__PACKETFACTORY_H__
