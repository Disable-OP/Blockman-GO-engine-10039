#include "EntityRenderArrow.h"
#include "EntityRenderManager.h"
#include "bm_clientDef.h"

#include "Model/ItemModelMgr.h"
#include "Entity/EntityArrow.h"
#include "cWorld/Blockman.h"
#include "Render/RenderEntity.h"

#include "Tessolator/BatchRenderable.h"
#include "Tessolator/TessManager.h"

namespace BLOCKMAN
{
	
EntityRenderArrow::EntityRenderArrow()
{
	m_texArrow = TextureManager::Instance()->createTexture("arrow.png");
	m_texArrow->load();
	initArrowModel();
}

EntityRenderArrow::~EntityRenderArrow()
{
	LordSafeFree(m_model);
	TextureManager::Instance()->releaseResource(m_texArrow);
}

void EntityRenderArrow::doRender(Entity* pEntity, const Vector3& pos, float yaw, float rdt)
{
	doRenderArrow((EntityArrow*)pEntity, pos, yaw, rdt);
}

void EntityRenderArrow::initArrowModel()
{
	SkinedVertexFmt* vertices = (SkinedVertexFmt*)LordMalloc(sizeof(SkinedVertexFmt)* 24);
	m_model = vertices;
	int yBegin = 0;
	float u0 = 0.0F;
	float u1 = 0.5F;
	float v0 = (0.f + yBegin * 10.f) / 32.0F;
	float v1 = (5.f + yBegin * 10.f) / 32.0F;
	float uf0 = 0.0F;
	float uf1 = 0.15625F;
	float vf0 = (5.f + yBegin * 10.f) / 32.0F;
	float vf1 = (10.f + yBegin * 10.f) / 32.0F;
	float s = 0.05625f;
	float f1 = 7.f * s;
	float f2 = 8.f * s;
	float f3 = 2.f * s;

	ui32 effectColor1 = 0xFFFFFFFF;
	ui32 effectColor2 = 0xAAAAAAAA;

	// arrow bunch
	vertices->setEx2(Vector3(-f1, -f3, -f3), effectColor1, uf0, vf0, 0); vertices++;
	vertices->setEx2(Vector3(-f1, -f3,  f3), effectColor1, uf1, vf0, 0); vertices++;
	vertices->setEx2(Vector3(-f1,  f3,  f3), effectColor1, uf1, vf1, 0); vertices++;
	vertices->setEx2(Vector3(-f1,  f3, -f3), effectColor1, uf0, vf1, 0); vertices++;

	vertices->setEx2(Vector3(-f1,  f3, -f3), effectColor2, uf0, vf0, 0); vertices++;
	vertices->setEx2(Vector3(-f1,  f3,  f3), effectColor2, uf1, vf0, 0); vertices++;
	vertices->setEx2(Vector3(-f1, -f3,  f3), effectColor2, uf1, vf1, 0); vertices++;
	vertices->setEx2(Vector3(-f1, -f3, -f3), effectColor2, uf0, vf1, 0); vertices++;

	// arrow 
	vertices->setEx2(Vector3(-f2, -f3, 0.f), effectColor1, u0, v1, 0); vertices++;
	vertices->setEx2(Vector3( f2, -f3, 0.f), effectColor1, u1, v1, 0); vertices++;
	vertices->setEx2(Vector3( f2,  f3, 0.f), effectColor1, u1, v0, 0); vertices++;
	vertices->setEx2(Vector3(-f2,  f3, 0.f), effectColor1, u0, v0, 0); vertices++;

	vertices->setEx2(Vector3(-f2, -f3, 0.f), effectColor2, u0, v1, 0); vertices++;
	vertices->setEx2(Vector3(-f2,  f3, 0.f), effectColor2, u0, v0, 0); vertices++;
	vertices->setEx2(Vector3( f2,  f3, 0.f), effectColor2, u1, v0, 0); vertices++;
	vertices->setEx2(Vector3( f2, -f3, 0.f), effectColor2, u1, v1, 0); vertices++;

	vertices->setEx2(Vector3(-f2, 0.f, -f3), effectColor1, u0, v1, 0); vertices++;
	vertices->setEx2(Vector3( f2, 0.f, -f3), effectColor1, u1, v1, 0); vertices++;
	vertices->setEx2(Vector3( f2, 0.f,  f3), effectColor1, u1, v0, 0); vertices++;
	vertices->setEx2(Vector3(-f2, 0.f,  f3), effectColor1, u0, v0, 0); vertices++;

	vertices->setEx2(Vector3(-f2, 0.f, -f3), effectColor2, u0, v1, 0); vertices++;
	vertices->setEx2(Vector3(-f2, 0.f,  f3), effectColor2, u0, v0, 0); vertices++;
	vertices->setEx2(Vector3( f2, 0.f,  f3), effectColor2, u1, v0, 0); vertices++;
	vertices->setEx2(Vector3( f2, 0.f, -f3), effectColor2, u1, v1, 0); vertices++;
}

void EntityRenderArrow::doRenderArrow(EntityArrow* pArrow, const Vector3& pos, float yaw, float rdt)
{
	Matrix4 matArrow = Matrix4::IDENTITY;
	Matrix4 temp = Matrix4::IDENTITY;
	matArrow.translate(pos);
	float fyaw = (pArrow->prevRotationYaw + (pArrow->rotationYaw - pArrow->prevRotationYaw) * rdt) * Math::DEG2RAD - Math::PI_DIV2;
	float pitch = (pArrow->prevRotationPitch + (pArrow->rotationPitch - pArrow->prevRotationPitch)* rdt) * Math::DEG2RAD;
	float shake = (float)pArrow->arrowShake - rdt;
	if (shake > 0.0F)
	{
		shake = Math::Sin(shake * 3.0F) * shake * Math::DEG2RAD;
		pitch += shake;
	}
	temp.rotateY(fyaw);
	matArrow.joint(temp);
	temp.identity(); temp.rotateZ(pitch);
	matArrow.joint(temp);
	temp.identity(); temp.rotateX(Math::PI_DIV4);
	matArrow.joint(temp);
	// Matrix4::RotateYawPitchRoll(temp, fyaw, pitch, 0.f);
	// matArrow.joint(temp);
	
	// get the renderable for skin from the EntityRenderManager and inject the data.
	PackMassRenderable* pPackRenderable = EntityRenderManager::Instance()->getPackRenderable(m_texArrow, SRP_SOLID, 6);
	int brightUV = pArrow->getBrightnessForRender(0.f);
	int brightness = Blockman::Instance()->m_entityRender->getLightMapColor(brightUV);
	pPackRenderable->addPackMass(m_model, 6, brightness, matArrow);
}


}