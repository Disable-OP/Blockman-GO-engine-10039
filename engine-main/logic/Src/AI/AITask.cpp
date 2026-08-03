#include "AITask.h"
#include "AIBase.h"

namespace BLOCKMAN
{

AITasks::AITasks()
: tickCount(0)
, tickRate(3)
{}

AITasks::~AITasks()
{
	for (auto it : taskEntries)
	{
		LordDeleteT(it, AITaskEntry);
	}
}

void AITasks::addTask(int priority, AIBase* ai)
{
	AITaskEntry* entry = LordNewT(AITaskEntry);
	entry->set(this, priority, ai);
	taskEntries.insert(entry);
}

void AITasks::removeTask(AIBase* ai)
{
	for (AITaskEntrices::iterator it = taskEntries.begin(); it != taskEntries.end(); ++it)
	{
		AITaskEntry* entry = *it;
		if (entry->action == ai)
		{
			auto jt = executingTaskEntries.find(entry);
			if (jt != executingTaskEntries.end())
			{
				ai->resetTask();
				executingTaskEntries.erase(jt);
			}
			taskEntries.erase(it);
			break; // todo. need break?
		}
	}
}

void AITasks::onUpdateTasks()
{
	AITaskEntrices entrices;
	AITaskEntry* pEntry = NULL;

	if (tickCount++ % tickRate == 0)
	{
		for (auto it : taskEntries)
		{
			pEntry = it;
			auto jt = executingTaskEntries.find(pEntry);
			if (jt != executingTaskEntries.end())
			{
				if (canUse(pEntry) && canContinue(pEntry))
					continue;

				pEntry->action->resetTask();
				executingTaskEntries.erase(jt);
			}
			if (canUse(pEntry) && pEntry->action->shouldExecute())
			{
				entrices.insert(pEntry);
				executingTaskEntries.insert(pEntry);
			}
		}
	}
	else
	{
		for (auto it = executingTaskEntries.begin(); it != executingTaskEntries.end(); )
		{
			pEntry = *it;

			if (!pEntry->action->continueExecuting())
			{
				pEntry->action->resetTask();
				it = executingTaskEntries.erase(it);
			}
			else
				++it;
		}
	}

	for (auto it : entrices)
	{
		pEntry = it;
		pEntry->action->startExecuting();
	}

	for (auto it : executingTaskEntries)
	{
		pEntry = it;
		pEntry->action->updateTask();
	}
}

bool AITasks::canContinue(AITaskEntry* pEntry)
{
	LordAssert(pEntry);
	LordAssert(pEntry->action);
	return pEntry->action->continueExecuting();
}

bool AITasks::canUse(AITaskEntry* pEntry)
{
	for (auto it : taskEntries)
	{
		if (it != pEntry)
		{
			if (pEntry->priority >= it->priority)
			{
				if (executingTaskEntries.find(it) != executingTaskEntries.end() &&
					!areTasksCompatible(pEntry, it))
					return false;
			}
			else if (executingTaskEntries.find(it) != executingTaskEntries.end() && 
				!it->action->isInterruptible())
				return false;
		}
	}

	return true;
}

bool AITasks::areTasksCompatible(AITaskEntry* pEntry1, AITaskEntry* pEntry2)
{
	return (pEntry1->action->getMutexBits() & pEntry2->action->getMutexBits()) == 0;
}

}
