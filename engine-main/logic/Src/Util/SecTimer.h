#pragma once
#include "Core.h"
#include "Object/Singleton.h"
#include "TypeDef.h"
#include <functional>
#include "Object/Root.h"
using namespace LORD;

struct SecTimer
{
	using TickCallBack = std::function<void(int)>;

	struct Timer
	{
		int id;
		ui32 lastUpdateTime;
		ui32 ticks;
		bool hasStart;
		bool hasStop;
		TickCallBack tickCallBack;
	};
	

	static ui16 createTimer(TickCallBack tickCallback)
	{
		auto timer = LORD::make_shared<Timer>();
		timer->id = m_idacc++;
		timer->lastUpdateTime = 0;
		timer->ticks = 0;
		timer->hasStart = false;
		timer->hasStop = false;
		timer->tickCallBack = tickCallback;

		m_TimerList[timer->id] = timer;

		return timer->id;
	}

	static void startTimer(int id)
	{
		auto timer = m_TimerList.find(id);
		if (timer != m_TimerList.end())
		{
			timer->second->hasStart = true;
			timer->second->lastUpdateTime = Root::Instance()->getCurrentTime();
		}
	}

	static void stopTimer(int id)
	{
		auto timer = m_TimerList.find(id);
		if (timer != m_TimerList.end())
			timer->second->hasStop = true;
	}

	static void update()
	{
		auto currentTick = Root::Instance()->getCurrentTime();
		for (auto timer : m_TimerList)
		{
			if (timer.second->hasStart && !timer.second->hasStop)
			{
				if (currentTick >= timer.second->lastUpdateTime + 1000)
				{
					timer.second->ticks += 1;
					timer.second->tickCallBack(timer.second->ticks);
					timer.second->lastUpdateTime = timer.second->lastUpdateTime + 1000;
				}
			}
		}
	}
	
private:
	static ui16 m_idacc;
	static map<int, std::shared_ptr<Timer>>::type m_TimerList;
};

