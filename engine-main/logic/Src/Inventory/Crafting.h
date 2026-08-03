/********************************************************************
filename: 	Slot.h
file path:	dev\client\Src\Blockman\Inventory

version:	1
author:		ajohn
company:	supernano
date:		2017-02-27
*********************************************************************/
#ifndef __CRAFTING_HEADER__
#define __CRAFTING_HEADER__

#include "BM_Container_def.h"
#include "Core.h"
using namespace LORD;

namespace BLOCKMAN
{

class Container;

class ICrafting
{
public:
	virtual void sendContainerAndContentsToPlayer(Container* pContainer, ItemStackArr& stacks) = 0;

	/**
	* Sends the contents of an inventory slot to the client-side Container. This doesn't have to match the actual
	* contents of that slot. Args: Container, slot number, slot contents
	*/
	virtual void sendSlotContents(Container* pContainer, int slot, ItemStackPtr pStack) = 0;

	/**
	* Sends two ints to the client-side Container. Used for furnace burning time, smelting progress, brewing progress,
	* and enchanting level. Normally the first int identifies which variable to update, and the second contains the new
	* value. Both are truncated to shorts in non-local SMP.
	*/
	virtual void sendProgressBarUpdate(Container* pContainer, int var2, int var3) = 0;
};


}

#endif