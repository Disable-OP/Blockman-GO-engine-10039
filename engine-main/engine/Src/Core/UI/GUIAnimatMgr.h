/********************************************************************
filename: 	GUIAnimatMgr.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef __GUI_ANIMATMGR__H__
#define __GUI_ANIMATMGR__H__

#include "Core.h"
#include "Object/Singleton.h"
#include "GUIString.h"

namespace LORD
{
	class GUIAnimate;

	class LORD_CORE_API GUIAnimatMgr : public Singleton<GUIAnimatMgr>,public ObjectAlloc
	{
	public:	

		GUIAnimatMgr();
		virtual ~GUIAnimatMgr();

		static GUIAnimatMgr* Instance();
	
		void lordConfig(String cfgFileName);
		GUIAnimate* GetAnimatById(i32 id);

	protected:
		
		typedef map<i32, GUIAnimate*>::type GUIAnimatMap;

		GUIAnimatMap	m_mapAnimations;			
		String m_configFileName;
	};
}


#endif