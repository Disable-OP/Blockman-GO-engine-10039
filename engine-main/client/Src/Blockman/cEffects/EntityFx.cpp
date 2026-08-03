#include "EntityFx.h"

#include "World/World.h"
#include "Util/Random.h"
#include "Render/TextureAtlas.h"
#include "EntityRenders/EntityRenderManager.h"

#include "Tessolator/TessRenderable.h"

namespace BLOCKMAN
{

Vector3 EntityFx::interpPos(0.f, 0.f, 0.f);

void EntityFx::init(const Vector3& pos)
{
	setSize(0.2F, 0.2F);
	yOffset = height / 2.0F;
	setPosition(pos);
	lastTickPos = pos;
	m_color = Color(1.f, 1.f, 1.f, 0.f);
	particleTextureIndexX = 0;
	particleTextureIndexY = 0;
	particleIcon = nullptr;
	particleTextureJitterX = rand->nextFloat() * 3.0F;
	particleTextureJitterY = rand->nextFloat() * 3.0F;
	particleScale = (rand->nextFloat() * 0.5F + 0.5F) * 2.0F;
	particleMaxAge = (int)(4.0F / (rand->nextFloat() * 0.9F + 0.1F));
	particleAge = 0;
}

EntityFx::EntityFx(World* pWorld, const Vector3& pos)
	: Entity(pWorld)
{
	init(pos);
}

EntityFx::EntityFx(World* pWorld, const Vector3& pos, const Vector3& pos2)
	: Entity(pWorld)
{
	init(pos);
	motion = pos2 + (rand->nextVector() * 2.f - 1.f) * 0.4f;
	float size = (Math::UnitRandom() + Math::UnitRandom() + 1.0f) * 0.15F;
	float len = motion.len();
	motion = motion / len * size * 0.4f;
	motion.y += 0.f;
}

void EntityFx::multiplyVelocity(float vel)
{
	motion.x *= vel;
	motion.y = (motion.y - 0.1f) * vel + 0.1f;
	motion.z *= vel;
}

void EntityFx::multipleParticleScaleBy(float scale)
{
	setSize(0.2F * scale, 0.2F * scale);
	particleScale *= scale;
}

void EntityFx::onUpdate()
{
	prevPos = position;

	if (particleAge++ >= particleMaxAge)
	{
		setDead();
	}

	motion.y -= 0.04f * particleGravity;
	moveEntity(motion);
	motion *= 0.98f;

	if (onGround)
	{
		motion.x *= 0.7f;
		motion.z *= 0.7f;
	}
}

void EntityFx::renderParticle(FxVertexFmt* vertices, int brightness, float rdt, float rx, float rxz, float rz, float ryz, float rxy)
{
	float u1 = particleTextureIndexX / 16.0F;
	float u2 = u1 + 0.0624375F;
	float v1 = particleTextureIndexY / 16.0F;
	float v2 = v1 + 0.0624375F;
	float s = 0.1F * particleScale;

	if (particleIcon != nullptr)
	{
		u1 = particleIcon->getMinU();
		u2 = particleIcon->getMaxU();
		v1 = particleIcon->getMinV();
		v2 = particleIcon->getMaxV();
	}

	Vector3 v = prevPos + (position - prevPos) * rdt - interpPos;
	ui32 color = m_color.getARGB();
	vertices[0].set(v.x - (rx + ryz) * s, v.y - rxz * s, v.z - (rz + rxy) * s, color, brightness, u2, v2);
	vertices[1].set(v.x - (rx - ryz) * s, v.y + rxz * s, v.z - (rz - rxy) * s, color, brightness, u2, v1);
	vertices[2].set(v.x + (rx + ryz) * s, v.y + rxz * s, v.z + (rz + rxy) * s, color, brightness, u1, v1);
	vertices[3].set(v.x + (rx - ryz) * s, v.y - rxz * s, v.z + (rz - rxy) * s, color, brightness, u1, v2);
}

void EntityFx::setSprite(AtlasSprite* sprite)
{
	if (getFXLayer() == 1)
	{
		particleIcon = sprite;
	}
	else
	{
		if (getFXLayer() != 2)
		{
			LordException("Invalid call to EntityFx::setSprite, use coordinate methods");
		}

		particleIcon = sprite;
	}
}

void EntityFx::setParticleTextureIndex(int idx)
{
	if (getFXLayer() != 0)
	{
		LordException("Invalid call to EntityFx::setParticleTextureIndex");
	}
	else
	{
		particleTextureIndexX = idx % 16;
		particleTextureIndexY = idx / 16;
	}
}

String EntityFx::toString()
{
	return StringUtil::BLANK;
}

}