/********************************************************************
filename: 	EffectLayerParticlesPoint.h
file path:	dev\engine\Src\Core\Effect\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __EFFECT_LAYER_POINTPARTICLE_H__
#define __EFFECT_LAYER_POINTPARTICLE_H__

#include "EffectLayerParticles.h"

namespace LORD
{
	class LORD_CORE_API EffectLayerParticlesPoint : public EffectLayerParticles
	{
	public:
		EffectLayerParticlesPoint();
		virtual ~EffectLayerParticlesPoint();

		virtual void emitterParticle(EffectParticle* particle);
	};
}

#endif