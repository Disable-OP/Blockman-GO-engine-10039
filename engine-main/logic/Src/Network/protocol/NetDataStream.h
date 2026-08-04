/********************************************************************
filename: 	NetDataStream
file path:	H:\sandboxol\client\blockmango-client\dev\common\Src\network\packet\NetDataStream.h

version:	1
author:		qutianxiang
company:    sandboxol
date:		2017/06/20
*********************************************************************/
#ifndef __NETDATASTREAM_H__
#define __NETDATASTREAM_H__
#include "raknet/include/BitStream.h"

class NetDataStream :public RakNet::BitStream
{

public:
	NetDataStream(void){};
	~NetDataStream(void){};
private:
	
};
#endif