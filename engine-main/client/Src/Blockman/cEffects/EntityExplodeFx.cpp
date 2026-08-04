#include "EntityExplodeFx.h"

#include "World/World.h"

#include "Tessolator/TessRenderable.h"

namespace BLOCKMAN
{

EntityHugeExplodeFX::EntityHugeExplodeFX(World* pWorld, const Vector3& pos, const Vector3& vel)
	: EntityFx(pWorld, pos, Vector3::ZERO)
{
	m_timeSinceStart = 0;
	m_maximumTime = 8;
}

void EntityHugeExplodeFX::onUpdate()
{
	for (int i = 0; i < 6; ++i)
	{
		Vector3 p = position + (rand->nextVector() - rand->nextVector()) * 4.f;
		world->spawnParticle("largeexplode", p, Vector3((float)m_timeSinceStart / (float)m_maximumTime, 0.f, 0.f));
	}

	++m_timeSinceStart;
	if (m_timeSinceStart == m_maximumTime)
		setDead();
}

EntityLargeExplodeFX::EntityLargeExplodeFX(World* pWorld, const Vector3& pos, const Vector3& vel)
	: EntityFx(pWorld, pos, Vector3::ZERO)
	, m_explodeAge(0)
{
	m_maxExplodeAge = 6 + rand->nextInt(4);
	m_color.r = m_color.g = m_color.b = rand->nextFloat() * 0.6F + 0.4F;
	m_color.a = 1.f;
	m_scales = 1.0F - (float)vel.x * 0.5F;
}

void EntityLargeExplodeFX::renderParticle(FxVertexFmt* vertices, int brightness, float rdt, float rx, float rxz, float rz, float ryz, float rxy)
{
	int frame = (int)((m_explodeAge + rdt) * 15.0F / (float)m_maxExplodeAge);

	if (frame <= 15)
	{
		float u1 = (float)(frame % 4) / 4.0F;
		float u2 = u1 + 0.24975F;
		float v1 = (float)(frame / 4) / 4.0F;
		float v2 = v1 + 0.24975F;
		float s = 2.0F * m_scales;
		Vector3 v = prevPos + (position - prevPos) * rdt - interpPos;
		ui32 color = m_color.getARGB();
		brightness = 240;
		vertices[0].set(v.x - (rx + ryz) * s, v.y - rxz * s, v.z - (rz + rxy) * s, color, brightness, u2, v2);
		vertices[1].set(v.x - (rx - ryz) * s, v.y + rxz * s, v.z - (rz - rxy) * s, color, brightness, u2, v1);
		vertices[2].set(v.x + (rx + ryz) * s, v.y + rxz * s, v.z + (rz + rxy) * s, color, brightness, u1, v1);
		vertices[3].set(v.x + (rx - ryz) * s, v.y - rxz * s, v.z + (rz - rxy) * s, color, brightness, u1, v2);
	}
}

void EntityLargeExplodeFX::onUpdate()
{
	prevPos = position;
	++m_explodeAge;

	if (m_explodeAge == m_maxExplodeAge)
	{
		setDead();
	}
}

}