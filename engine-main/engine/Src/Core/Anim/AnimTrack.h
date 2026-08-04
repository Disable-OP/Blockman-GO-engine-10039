/********************************************************************
filename: 	AnimTrack.h
file path:	dev\engine\Src\Core\Anim\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_ANIMTRACK_H__
#define __LORD_ANIMTRACK_H__

#include "Memory/LordMemory.h"

namespace LORD
{
	class Animation;

	class LORD_CORE_API AnimTrack: public ObjectAlloc
	{	
	public:
		AnimTrack(Dword handle);
		virtual ~AnimTrack();

	public:
		virtual Dword	getHandle() const;

	protected:
		Dword			m_handle;
	};

}
#endif