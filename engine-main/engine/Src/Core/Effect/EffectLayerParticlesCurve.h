/********************************************************************
filename: 	EffectLayerParticlesCurve.h
file path:	dev\engine\Src\Core\Effect\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __EFFECT_LAYER_CURVEPARTICLE_H__
#define __EFFECT_LAYER_CURVEPARTICLE_H__

#include "EffectLayerParticles.h"

namespace LORD
{
	class LORD_CORE_API EffectLayerParticlesCurve : public EffectLayerParticles
	{
	public:
		EffectLayerParticlesCurve();
		virtual ~EffectLayerParticlesCurve();

		virtual void emitterParticle(EffectParticle* particle);
	};
}

#endif