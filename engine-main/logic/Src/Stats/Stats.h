/********************************************************************
filename: 	StatBase.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2017-02-28
*********************************************************************/
#ifndef __STAT_BASE_HEADER__
#define __STAT_BASE_HEADER__

#include "Core.h"
using namespace LORD;

namespace BLOCKMAN
{

class IStatType
{
public:
	/** Formats a given stat for human consumption. 	*/
	virtual String format(int v) = 0;
};

class StatTypeDistance : public IStatType
{
public:
	virtual String format(int v);
};

class StatTypeFloat : public IStatType
{
public:
	virtual String format(int v);
};

class StatTypeSimple : public IStatType
{
public:
	virtual String format(int v);
};	

class StatTypeTime : public IStatType
{
public:
	virtual String format(int v);
};

class StatBase : public ObjectAlloc
{
protected:
	/** The Stat name */
	String statName;
	bool m_isIndependent = false;
	IStatType* type = nullptr;

public:
	/** The Stat ID */
	int statId = 0;
	/** Holds the GUID of the stat. */
	String statGuid;

	static IStatType* simpleStatType;	// = new StatTypeSimple();
	static IStatType* timeStatType;		// = new StatTypeTime();
	static IStatType* distanceStatType;	// = new StatTypeDistance();
	static IStatType* field_111202_k;	// = new StatTypeFloat();

public:
	StatBase(int id, const String& name, IStatType* statType);
	StatBase(int id, const String& name);

	/** Initializes the current stat as independent (i.e., lacking prerequisites for being updated) and returns the current instance. */
	void initIndependentStat() { m_isIndependent = true; }
	/** Register the stat into StatList. */
	virtual void registerStat();
	/** Returns whether or not the StatBase-derived class is a statistic (running counter) or an achievement (one-shot). */
	bool isAchievement() { return false; }
	String func_75968_a(int v);
	const String& getName() const { return statName; }
	String toString();
	bool isIndependent() { return m_isIndependent; }
};

class StatBasic : public StatBase
{
public:
	StatBasic(int id, const String& name, IStatType* statType);
	StatBasic(int id, const String& name);
	
	/** Implement override function from StatBase */
	virtual void registerStat();
};

class StatCrafting : public StatBase
{
protected:
	int itemID = 0;

public:
	StatCrafting(int id, const String& name, int itemid);
		
	int getItemID() { return itemID; }
};



}

#endif