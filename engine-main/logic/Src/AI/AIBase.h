/********************************************************************
filename: 	AIBase.h
file path:	dev\client\Src\Blockman\AI

version:	1
author:		ajohn
company:	supernano
date:		2017-07-05
*********************************************************************/
#ifndef __ENTITY_AI_BASE_HEADER__
#define __ENTITY_AI_BASE_HEADER__

#include "Core.h"
using namespace LORD;

namespace BLOCKMAN
{

class AIBase : public ObjectAlloc
{
protected:
	/** A bitmask telling which other tasks may not run concurrently. The test is a simple bitwise AND - if it yields
	* zero, the two tasks may run concurrently, if not - they must run exclusively from each other. */
	int mutexBits;

public:
	/** Returns whether the EntityAIBase should begin execution. */
	virtual bool shouldExecute() = 0;

	/** Returns whether an in-progress EntityAIBase should continue executing */
	virtual bool continueExecuting() { return shouldExecute(); }

	/** Determine if this AI Task is interruptible by a higher (= lower value) priority task. */
	virtual bool isInterruptible() { return true; }

	/** Execute a one shot task or start executing a continuous task */
	virtual void startExecuting() {}

	/** Resets the task */
	virtual void resetTask() {}

	/** Updates the task */
	virtual void updateTask() {}

	/** Sets a bitmask telling which other tasks may not run concurrently. */
	void setMutexBits(int bits) { mutexBits = bits; }

	/** Get a bitmask telling which other tasks may not run concurrently. */
	int getMutexBits() { return mutexBits; }
};

}

#endif