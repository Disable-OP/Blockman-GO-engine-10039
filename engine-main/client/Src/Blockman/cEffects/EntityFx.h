/********************************************************************
filename: 	EntityFx.h
file path:	dev\client\Src\Blockman\Effects

version:	1
author:		ajohn
company:	supernano
date:		2017-07-31
*********************************************************************/
#ifndef __ENTITY_FX_HEADER__
#define __ENTITY_FX_HEADER__

#include "Entity/Entity.h"

namespace BLOCKMAN
{

class World;
class ItemStack;
class AtlasSprite;

class EntityFx : public Entity
{
public:
	static Vector3 interpPos;
	bool m_isFirstPerspective = false;

protected:
	int particleTextureIndexX = 0;
	int particleTextureIndexY = 0;
	float particleTextureJitterX = 0.f;
	float particleTextureJitterY = 0.f;
	int particleAge = 0;
	int particleMaxAge = 0;
	float particleScale = 0.f;
	float particleGravity = 0.f;
	AtlasSprite* particleIcon = nullptr;
	Color m_color;
	
protected:
	EntityFx(World* pWorld, const Vector3& pos);
	void init(const Vector3& pos);

	/** implement override functions from Entity. */
	virtual bool canTriggerWalking() { return false; }
	virtual void entityInit() {}

public:
	EntityFx(World* pWorld, const Vector3& pos, const Vector3& pos2);

	void multiplyVelocity(float vel);
	void multipleParticleScaleBy(float scale);
	void setSprite(AtlasSprite* sprite);
	void setParticleTextureIndex(int idx);
	void nextTextureIndexX() { ++particleTextureIndexX; }

	void setColor(float r, float g, float b) { m_color.r = r; m_color.g = g; m_color.b = b; }
	void setAlpha(float a) { m_color.a = a; }
	void setColor(const Color& color) { m_color = color; }

	const Color& getColor() const { return m_color; }
	float getRed() { return m_color.r; }
	float getGreen() { return m_color.g; }
	float getBlue() { return m_color.b; }
	
	/** implement override functions from Entity. */
	virtual void onUpdate();
	virtual void writeEntityToNBT(NBTTagCompound* pNbt) {}
	virtual void readEntityFromNBT(NBTTagCompound* pNbt) {}
	virtual bool canAttackWithItem() { return false; }
	virtual String toString();

	/** get the render layer for this particle. */
	virtual int getFXLayer() { return 0; }

	/** render this particle. */
	virtual void renderParticle(FxVertexFmt* vertices, int brightness, float rdt, float rx, float rxz, float rz, float ryz, float rxy);

};

}

#endif
