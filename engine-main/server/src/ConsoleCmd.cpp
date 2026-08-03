#include "ConsoleCmd.h"
#include "Global.h"
#include <iostream>
#include "Server.h"
ConsoleCmd::ConsoleCmd()
{

}

ConsoleCmd::~ConsoleCmd()
{

}

void ConsoleCmd::start()
{
	m_cmdThread = std::make_shared<UThread>("ConsoleCmd", &ConsoleCmd::parse, this);
	//m_cmdThread->start();
}

void ConsoleCmd::stop()
{
	m_cmdThread->stopSync();
}

void ConsoleCmd::parse(StopFlag shoudStop)
{

	while(std::cin.good())
	{
		String command;
		std::getline(std::cin, command);
		if (!command.empty())
		{
			// todo cmd parse
			if (command == "stop")
			{
				Server::Instance()->setStopEvent();
				break;
			}
		}
	}
	
}