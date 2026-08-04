/********************************************************************
filename: 	AITask.h
file path:	dev\client\Src\Blockman\AI

version:	1
author:		ajohn
company:	supernano
date:		2017-07-06
*********************************************************************/
#ifndef __ENTITY_AI_TASK_HEADER__
#define __ENTITY_AI_TASK_HEADER__

#include "Core.h"
using namespace LORD;

namespace BLOCKMAN
{

class AIBase;
class AITasks;

struct AITaskEntry
{
	/** The AIBase object. */
	AIBase* action;

	/** Priority of the AIBase */
	int priority;

	/** The EntityAITasks object of which this is an entry. */
	AITasks* tasks;

	void set(AITasks* t, int p, AIBase* ai)
	{ 
		tasks = t;
		priority = p;
		action = ai;
	}
};

typedef set<AITaskEntry*>::type AITaskEntrices;

class AITasks : public ObjectAlloc
{
public:
	AITasks();
	~AITasks();
	
	void addTask(int priority, AIBase* ai);

	/** removes the indicated task from the entity's AI tasks. */
	void removeTask(AIBase* ai);

	/** update all the task. */
	void onUpdateTasks();

protected:
	/** Determine if a specific AI Task should continue being executed. */
	bool canContinue(AITaskEntry* pEntry);

	/** Determine if a specific AI Task can be executed, which means that all running higher (= lower int value) priority
	* tasks are compatible with it or all lower priority tasks can be interrupted. */
	bool canUse(AITaskEntry* pEntry);

	/** Returns whether two EntityAITaskEntries can be executed concurrently */
	bool areTasksCompatible(AITaskEntry* pEntry1, AITaskEntry* pEntry2);

protected:
	/** A list of EntityAITaskEntrys in EntityAITasks. */
	AITaskEntrices taskEntries; // = new ArrayList();

	/** A list of EntityAITaskEntrys that are currently being executed. */
	AITaskEntrices executingTaskEntries; // = new ArrayList();

	int tickCount;
	int tickRate;
};

}

#endif