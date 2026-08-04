/********************************************************************
filename: 	Common.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-11-28
*********************************************************************/
#ifndef __COMMON_HEADER__
#define __COMMON_HEADER__
#include <memory>

#include "Core.h"
#include "Network/CommonPacketSender.h"
using namespace LORD;

namespace BLOCKMAN
{

#define ERROR_BREAK(condition) \
if(!(condition)) \
	break;

#define SUCCESS_RESULT_BREAK(condition, result) \
if((condition)) \
{ \
	result = true; \
	break; \
}

#define ERROR_BREAK_CODE(condition, result) \
if(!(condition)) \
{ \
	result = false; \
	break; \
}

#define ERROR_ID 0
#define ERROR_INDEX -1

class GameCommon:public Singleton<GameCommon>, public ObjectAlloc
{
public:
	GameCommon();
	~GameCommon();

	void setCommonPacketSender(std::shared_ptr<CommonPacketSender> & sender);
	std::shared_ptr<CommonPacketSender> getCommonPacketSender(){return m_commonPacketSender;}

public:
	static void logIfExpired(ui32 startTime, ui32 endTime, ui32 threshold, std::string msg);

private:
	std::shared_ptr<CommonPacketSender> m_commonPacketSender;
};



}

#endif

