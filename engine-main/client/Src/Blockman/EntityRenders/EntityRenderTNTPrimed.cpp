#include "EntityRenderTNTPrimed.h"
#include "EntityRenderManager.h"

#include "Render/RenderBlocks.h"
#include "Render/RenderEntity.h"
#include "cWorld/Blockman.h"
#include "cBlock/cBlock.h"
#include "cBlock/cBlockManager.h"
#include "cWorld/Blockman.h"
#include "Model/ItemModelMgr.h"

#include "Block/Block.h"
#include "Block/BlockManager.h"
#include "Entity/EntityTNTPrimed.h"
#include "World/World.h"
#include "Common.h"

#include "Tessolator/TessRenderable.h"
#include "Tessolator/TessManager.h"

namespace BLOCKMAN
{

EntityRenderTNTPrimed::EntityRenderTNTPrimed()
{
	shadowSize = 0.5f;
	m_model = NULL;
	m_quadNum = 0;
}

EntityRenderTNTPrimed::~EntityRenderTNTPrimed()
{
	LordSafeFree(m_model);
}

void EntityRenderTNTPrimed::doRender(Entity* pEntity, const Vector3& pos, float yaw, float rdt)
{
	if (m_model == NULL)
		createModel();
	render_impl((EntityTNTPrimed*)pEntity, pos, yaw, rdt);
}

bool EntityRenderTNTPrimed::createModel()
{
	RenderBlocks renderBlocks(Blockman::Instance()->getWorld());
	Block* pBlock = BlockManager::sBlocks[BLOCK_ID_TNT];
	cBlock* pcBlock = cBlockManager::scBlocks[BLOCK_ID_TNT];
	Texture* blockTex = EntityRenderManager::Instance()->m_BlockTex;

	TessRenderable* tessRenderable = TessManager::Instance()->createTessRenderable(blockTex, TRP_SOLID);
	renderBlocks.setSpecialRenderable(tessRenderable);
	tessRenderable->beginPolygon();
	renderBlocks.renderBlockAsItem(pBlock, pcBlock, 0);
	m_quadNum = tessRenderable->getQuadNum();
	bool result = false;
	do
	{
		ERROR_BREAK((m_quadNum > 0));
		// copy the vertex data from tessRenderable's vertex.

		m_model = (SkinedVertexFmt*)LordMalloc(m_quadNum * 4 * sizeof(SkinedVertexFmt));
		TessVertexFmt* tessVertices = (TessVertexFmt*)tessRenderable->getVertexMenory();
		Vector3 vecTrans(-0.5f, 0.f, -0.5f);
		for (int i = 0; i < m_quadNum; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				int idx = i * 4 + j;

				memcpy(m_model + idx, tessVertices + idx, sizeof(SkinedVertexFmt));
				Vector3 newPos = tessVertices[idx].GetPosition() + vecTrans;
				m_model[idx].setPositionEx(newPos.x, newPos.y, newPos.z);
				m_model[idx].setIndex(0);
			}
		}

		result = true;
	} while (0);

	TessManager::Instance()->destroyTessRenderable(tessRenderable);
	return result;
}

void EntityRenderTNTPrimed::render_impl(EntityTNTPrimed* tnt, const Vector3& pos, float yaw, float rdt)
{
	float f, s = 1.f;
	float fuse = float(tnt->getFuse());
	if (fuse - rdt + 1.f < 10.f)
	{
		f = 1.f - (fuse - rdt + 1.f) / 10.f;
		f = Math::Clamp(f, 0.f, 1.f);
		f *= f;
		f *= f;
		s = 1.f + f * 0.3f;
	}
	f = (1.f - (fuse - rdt + 1.f) / 100.f) * 0.8f;

	//LordLogInfo("Tnt scale %f  fuse:%f, rdt:%f", s, fuse, rdt);
	Texture* tex = EntityRenderManager::Instance()->m_BlockTex;
	Matrix4 matTnt;
	matTnt.makeTranslation(pos);
	matTnt.scale(s, s, s);

	int brightUV = tnt->getBrightnessForRender(0.f);
	int brightness = Blockman::Instance()->m_entityRender->getLightMapColor(brightUV);

	PackMassRenderable* pPackRenderable = EntityRenderManager::Instance()->getPackRenderable(tex, SRP_SOLID, m_quadNum);
	pPackRenderable->addPackMass(m_model, m_quadNum, brightness, matTnt);
	if (tnt->getFuse() / 5 % 2 == 0)
	{
		// high-lighting mode.
		PackMassRenderable* pPackRenderable = EntityRenderManager::Instance()->getPackRenderable(tex, SRP_SOLID_LIGHT, m_quadNum);
		pPackRenderable->addPackMass(m_model, m_quadNum, brightness, matTnt);
	}
}

}