/********************************************************************
filename: 	Effect2DGrid.h
file path:	dev\engine\Src\Core\Effect\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_EFFECT_LAYER_2DGRID_H__
#define __LORD_EFFECT_LAYER_2DGRID_H__

#include "EffectLayer3DGrid.h"

namespace LORD
{
	class LORD_CORE_API EffectLayer2DGrid : public EffectLayer3DGrid
	{
	public:
		EffectLayer2DGrid();
		virtual ~EffectLayer2DGrid();

		virtual void getMatrixWorld(Matrix4& mtxWorld);

		virtual const Quaternion& getSurfaceOritation()const;
	protected:
		Quaternion mCurrentCamOritation;
	};
}

#endif