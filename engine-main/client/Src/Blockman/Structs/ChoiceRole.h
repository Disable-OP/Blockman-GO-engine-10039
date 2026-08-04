#pragma once
#ifndef __CHOICE_ROLE_HEADER__
#define __CHOICE_ROLE_HEADER__
#include "Core.h"
#include <list>

using namespace LORD;
namespace BLOCKMAN {

	class ResInfo : public ObjectAlloc
	{
	public:
		String resName = "";
		i32		resId = 0;
	};

	class ActorInfo : public ObjectAlloc
	{
	public:
		i32		lv = 1;
		i32		merit = 0;
		i32		threat = 0;
		i64		money = 0;
		bool	canSelect = false;
		String  name = "";
		String  title = "";
		String  model;
		list<ResInfo>::type actors;
	};

	class ChoiceRole : public ObjectAlloc
	{
	public:
		bool      forceOpen = true;
		i32		  type = 0;
		ActorInfo criminal;
		ActorInfo police;

	};


}
#endif

