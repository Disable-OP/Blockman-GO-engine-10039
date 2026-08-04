/********************************************************************
filename: 	EntityFx.h
file path:	dev\client\Src\Blockman\Effects

version:	1
author:		ajohn
company:	supernano
date:		2017-07-31
*********************************************************************/
#ifndef __EFFECT_RENDER_HEADER__
#define __EFFECT_RENDER_HEADER__

#include "EntityFx.h"

namespace BLOCKMAN
{

class World;

class EffectRenderer : public Singleton<EffectRenderer>, public ObjectAlloc
{
	typedef list<EntityFx*>::type EffectLayer;
	typedef list<FxRenderable*>::type EffectRenderableArr;
	static const int MAX_FX_LAYER = 5;
	
public:
	Texture*			m_explodeTexture = nullptr; //"textures/entity/explosion.png"

protected:
	Texture*			m_mainTexture = nullptr; //"textures/particle/particles.png"
	World*				m_pWorld = nullptr;
	EffectLayer			m_fxLayers[MAX_FX_LAYER];
	Random*				m_rand = nullptr;

	EffectRenderableArr	m_effectRenderables;		// for effect
	EffectRenderableArr	m_fullEffectRenderables;	// for effect
	EffectRenderableArr	m_freeEffectRenderables;	// for effect

	FxRenderable*		m_gunFlameRenderable = nullptr;

public:
	EffectRenderer(World* pWorld);
	~EffectRenderer();

	FxRenderable* getEffectRenderable(Texture* tex, FX_RENDER_PASS rq, int quadNum);
	void givebackEffectRenderable(FxRenderable* renderable);
	void giveheadEffectRenderable(FxRenderable* renderable);

	void addEffect(EntityFx* pEffect);
	void updateEffects();

	void beginRender();
	/** Renders all current particles. Args player, partialTickTime */
	void renderParticles(Entity* pEntity, float rdt);
	void renderLitParticles(Entity* pEntity, float rdt);
	void endRender();

	void clearEffects(World* pWorld);
	
	/** Adds block destroy particles for the specified block. */
	void addBlockDestroyEffects(const BlockPos& pos, int blockID, int side);

	/** Adds block hit particles for the specified block. */
	void addBlockHitEffects(const BlockPos& pos, int side);

	int getFxCount();
};

}

#endif