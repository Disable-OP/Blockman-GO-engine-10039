/********************************************************************
filename: 	ConsoleCmd
file path:	H:\sandboxol\client\blockmango-client\dev\server\src\ConsoleCmd.h

version:	1
author:		qutianxiang
company:    sandboxol
date:		2017/07/05
*********************************************************************/

#ifndef __CONSOLECMD_H__
#define __CONSOLECMD_H__
#include "Util/UThread.h"
class ConsoleCmd
{
public:
	ConsoleCmd();
	~ConsoleCmd();
	
	void start();
	void stop();

	void parse(StopFlag shouldStop);
private:

	std::shared_ptr<UThread>  m_cmdThread;
};


#endif // !__CONSOLECMD_H__
