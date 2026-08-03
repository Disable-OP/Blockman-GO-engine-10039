/********************************************************************
filename: 	AABBKeyFrame.h
file path:	dev\engine\Src\Core\Anim\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_AABBKEYFRAME_H__
#define __LORD_AABBKEYFRAME_H__

#include "AnimKeyFrame.h"
#include "Geom/GeomLib.h"

namespace LORD
{
	class LORD_CORE_API AABBKeyFrame: public AnimKeyFrame
	{	
	public:
		AABBKeyFrame(Animation* pAnim, ui32 time);
		virtual ~AABBKeyFrame();

	public:

		virtual void		setBox(const Box &box);

		virtual const Box&	getBox() const;

	protected:
		Box		m_box;
	};

}

#endif