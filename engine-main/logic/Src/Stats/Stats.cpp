#include "Stats.h"
#include "StatList.h"

namespace BLOCKMAN
{

IStatType* StatBase::simpleStatType = new StatTypeSimple();
IStatType* StatBase::timeStatType = new StatTypeTime();
IStatType* StatBase::distanceStatType = new StatTypeDistance();
IStatType* StatBase::field_111202_k = new StatTypeFloat();

String StatTypeDistance::format(int v)
{
	float v_m = (float)v / 100.0f;
	float v_km = v_m / 1000.0f;
	if (v_km > 0.5f)
		return StringUtil::Format("%.2f km", v_km);
	else if (v_m > 0.5f)
		return StringUtil::Format("%.2f m", v_m);
	else
		return StringUtil::Format("%.2f cm", v);
}

String StatTypeFloat::format(int v)
{
	return StringUtil::Format("%.2f", v*0.1f);
}

String StatTypeSimple::format(int v)
{
	return StringUtil::Format("%d", v);
}

String StatTypeTime::format(int v)
{
	float second = (float)v / 20.0f;
	float minute = second / 60.0f;
	float hour = minute / 60.0f;
	float day = hour / 24.0f;
	float year = day / 365.0f;
	
	if (year > 0.5f)
		return StringUtil::Format("%.2f y", year);
	else if (day > 0.5f)
		return StringUtil::Format("%.2f d", day);
	else if (hour > 0.5f)
		return StringUtil::Format("%.2f h", hour);
	else if (minute > 0.5f)
		return StringUtil::Format("%.2f m", minute);
	return StringUtil::Format("%.2f s", second);
}


StatBase::StatBase(int id, const String& name, IStatType* statType)
	: statId(id)
	, statName(name)
	, type(statType)
	, m_isIndependent(false)
{
}

StatBase::StatBase(int id, const String& name)
	: statId(id)
	, statName(name)
	, type(simpleStatType)
	, m_isIndependent(false)
{
}

void StatBase::registerStat()
{
	LordAssert(StatList::oneShotStats.find(statId) == StatList::oneShotStats.end());

	StatList::allStats.push_back(this); 
	StatList::oneShotStats.insert(std::make_pair(statId, this));
	// todo.
	// statGuid = AchievementMap::getGuid(statId);
}

String StatBase::func_75968_a(int v) 
{
	return type->format(v);
}

String StatBase::toString()
{
	// todo.
	// return StatCollector.translateToLocal(statName);
	return statName;
}

StatBasic::StatBasic(int id, const String& name, IStatType* statType)
	: StatBase(id, name, statType)
{}

StatBasic::StatBasic(int id, const String& name)
	: StatBase(id, name)
{}

void StatBasic::registerStat()
{
	StatBase::registerStat();
	StatList::generalStats.push_back(this);
}

StatCrafting::StatCrafting(int id, const String& name, int itemid)
	: StatBase(id, name)
	, itemID(itemid)
{
}


}
