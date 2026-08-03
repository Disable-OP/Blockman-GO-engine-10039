#include "EntityRender.h"
#include "EntityRenderManager.h"

#include "cBlock/cBlock.h"
#include "cBlock/cBlockManager.h"
#include "Model/ItemModelMgr.h"
#include "Render/TextureAtlas.h"
#include "Actor/ActorObject.h"
#include "Actor/ActorManager.h"
#include "Block/Block.h"
#include "Block/BlockManager.h"
#include "Entity/Entity.h"
#include "Entity/EntityLiving.h"
#include "Entity/EntityPlayer.h"
#include "World/GameSettings.h"
#include "World/World.h"

#include "Tessolator/TessManager.h"
#include "Tessolator/FxRenderable.h"

namespace BLOCKMAN
{

Texture* EntityRender::SHADOW_TEXTURES = NULL;

EntityRender::EntityRender()
	: shadowSize(0.f)
	, shadowOpaque(1.f)
	, renderOutlines(false)
{
	renderManager = EntityRenderManager::Instance();
}

EntityRender::~EntityRender()
{}

void EntityRender::renderEntityOnFire(Entity* pEntity, const Vector3& pos, float rdt)
{
	int frames = TessManager::Instance()->getFrameCount();
	AtlasSprite* fire0 = cBlockManager::scBlocks[BLOCK_ID_FIRE]->getSpriteFromSide(0);
	AtlasSprite* fire1 = cBlockManager::scBlocks[BLOCK_ID_FIRE]->getSpriteFromSide(1);

	// fire is 16 * 512.
	int frame = frames % 32;
	
	Matrix4 matfire, mattemp;
	matfire.identity();
	matfire.translate(pos);
	float scale = pEntity->width * 1.4F;
	mattemp.identity(); mattemp.scale(scale, scale, scale);
	matfire.joint(mattemp);
	float segments = pEntity->height / scale;
	mattemp.identity(); mattemp.rotateY(-renderManager->playerViewY * Math::DEG2RAD);
	matfire.joint(mattemp);
	mattemp.identity(); mattemp.translate(0.f, 0.f, -0.3f + segments * 0.02f);
	matfire.joint(mattemp);

	float x1 = 0.5F;
	float x0 = 0.0F;
	float y0 = pEntity->position.y - pEntity->boundingBox.vMin.y;
	float z0 = 0.0F;
	int quads = 0;

	SkinedVertexFmt* verts = (SkinedVertexFmt*)LordMalloc(sizeof(SkinedVertexFmt) * 4 * int(segments / 0.45f + 1));

	while (segments > 0.0F)
	{
		AtlasSprite* fire = quads % 2 == 0 ? fire0 : fire1;
		
		float minU = fire->getMinU();
		float minV = fire->getMinV();
		float maxU = fire->getMaxU();
		float maxV = fire->getMaxV();
		float vVal = maxV - minV;
		minV += vVal * frame;
		maxV += vVal * frame;

		if (quads / 2 % 2 == 0)
			std::swap(maxU, minU);

		ui32 effectColor = 0xFFFFFFFF;
		verts[quads * 4 + 0].setEx2(Vector3(x1 - x0, -y0, z0),        effectColor, maxU, maxV, 0);
		verts[quads * 4 + 1].setEx2(Vector3(-x1 - x0, -y0, z0),       effectColor, minU, maxV, 0);
		verts[quads * 4 + 2].setEx2(Vector3(-x1 - x0, 1.4f - y0, z0), effectColor, minU, minV, 0);
		verts[quads * 4 + 3].setEx2(Vector3(x1 - x0, 1.4f - y0, z0),  effectColor, maxU, minV, 0);
		segments -= 0.45F;
		y0 -= 0.45F;
		x1 *= 0.9F;
		z0 += 0.03F;
		++quads;
	}

	PackMassRenderable* pPackRenderable = EntityRenderManager::Instance()->getPackRenderable(fire0->getTexture(), SRP_SOLID, quads);
	pPackRenderable->addPackMass(verts, quads, 0xFFFFFFFF, matfire);
	LordSafeFree(verts);
}

void EntityRender::renderShadow(Entity* pEntity, const Vector3& pos, float alpha, float rdt)
{
	World* pWorld = getWorldFromRenderManager();
	float ss = shadowSize;

	if (pEntity->isClass(ENTITY_CLASS_PLAYER))
	{
		EntityPlayer* player = (EntityPlayer*)pEntity;
		if (player->isActorInvisible() || player->m_isCameraLocked)
			return;
	}

	if (pEntity->isClass(ENTITY_CLASS_LIVING))
	{
		EntityLiving* pLiving = (EntityLiving*)pEntity;
		ss *= pLiving->getRenderSizeModifier();

		if (pLiving->isChild())
		{
			ss *= 0.5F;
		}
	}

	Vector3 vpos = pEntity->lastTickPos + (pEntity->position - pEntity->lastTickPos) * rdt;
	vpos.y += pEntity->getShadowSize();
	int x0 = int(Math::Floor(vpos.x - ss));
	int x1 = int(Math::Floor(vpos.x + ss));
	int y0 = int(Math::Floor(vpos.y - ss));
	int y1 = int(Math::Floor(vpos.y));
	int z0 = int(Math::Floor(vpos.z - ss));
	int z1 = int(Math::Floor(vpos.z + ss));

	Vector3 offset(0.f, pEntity->getShadowSize(), 0.f);
	for (int ix = x0; ix <= x1; ++ix)
	{
		for (int iy = y0; iy <= y1; ++iy)
		{
			for (int iz = z0; iz <= z1; ++iz)
			{
				BlockPos ipos(ix, iy, iz);
				int blockID = pWorld->getBlockId(ipos.getNegY());

				if (blockID > 0 && pWorld->getBlockLightValue(ipos) > 3)
				{
					renderShadowOnBlock(
						BlockManager::sBlocks[blockID],
						pos + offset,
						ipos,
						alpha,
						ss,
						pos - vpos + offset);
				}
			}
		}
	}
}

World* EntityRender::getWorldFromRenderManager()
{
	return renderManager->world;
}

void EntityRender::renderShadowOnBlock(Block* pBlock, const Vector3& center, const Vector3i& blockPos, float alpha, float size, const Vector3& offset)
{
	if (!pBlock->renderAsNormalBlock())
		return;

	float bright = (alpha - (center.y - (blockPos.y + offset.y)) / 2.0f) * 0.5f * getWorldFromRenderManager()->getLightBrightness(blockPos);

	if (bright < 0.f)
		return;

	if (bright > 1.f)
		bright = 1.f;

	FxRenderable* renderable = renderManager->getShadowRenderable();
	FxVertexFmt* verts;
	if (!renderable->getLockPtr(1, verts))
		return;

	int color = Color(1.f, 1.f, 1.f, bright).getARGB();

	float x0 = blockPos.x + pBlock->getBlockBoundsMinX() + offset.x;
	float x1 = blockPos.x + pBlock->getBlockBoundsMaxX() + offset.x;
	float y0 = blockPos.y + pBlock->getBlockBoundsMinY() + offset.y + 0.015625f;
	float z0 = blockPos.z + pBlock->getBlockBoundsMinZ() + offset.z;
	float z1 = blockPos.z + pBlock->getBlockBoundsMaxZ() + offset.z;
	float minU = (center.x - x0) / 2.f / size + 0.5f;
	float maxU = (center.x - x1) / 2.f / size + 0.5f;
	float minV = (center.z - z0) / 2.f / size + 0.5f;
	float maxV = (center.z - z1) / 2.f / size + 0.5f;
	verts[0].set(x0, y0, z0, color, 0xFFFFFFFF, minU, minV);
	verts[1].set(x0, y0, z1, color, 0xFFFFFFFF, minU, maxV);
	verts[2].set(x1, y0, z1, color, 0xFFFFFFFF, maxU, maxV);
	verts[3].set(x1, y0, z0, color, 0xFFFFFFFF, maxU, minV);
}

void EntityRender::renderOffsetAABB(const Box& aabb, const Vector3& pos)
{
	/*GL11.glDisable(GL11.GL_TEXTURE_2D);
	Tessellator var7 = Tessellator.instance;
	GL11.glColor4f(1.0F, 1.0F, 1.0F, 1.0F);
	var7.startDrawingQuads();
	var7.setTranslation(par1, par3, par5);
	var7.setNormal(0.0F, 0.0F, -1.0F);
	var7.addVertex(aabb.minX, aabb.maxY, aabb.minZ);
	var7.addVertex(aabb.maxX, aabb.maxY, aabb.minZ);
	var7.addVertex(aabb.maxX, aabb.minY, aabb.minZ);
	var7.addVertex(aabb.minX, aabb.minY, aabb.minZ);
	var7.setNormal(0.0F, 0.0F, 1.0F);
	var7.addVertex(aabb.minX, aabb.minY, aabb.maxZ);
	var7.addVertex(aabb.maxX, aabb.minY, aabb.maxZ);
	var7.addVertex(aabb.maxX, aabb.maxY, aabb.maxZ);
	var7.addVertex(aabb.minX, aabb.maxY, aabb.maxZ);
	var7.setNormal(0.0F, -1.0F, 0.0F);
	var7.addVertex(aabb.minX, aabb.minY, aabb.minZ);
	var7.addVertex(aabb.maxX, aabb.minY, aabb.minZ);
	var7.addVertex(aabb.maxX, aabb.minY, aabb.maxZ);
	var7.addVertex(aabb.minX, aabb.minY, aabb.maxZ);
	var7.setNormal(0.0F, 1.0F, 0.0F);
	var7.addVertex(aabb.minX, aabb.maxY, aabb.maxZ);
	var7.addVertex(aabb.maxX, aabb.maxY, aabb.maxZ);
	var7.addVertex(aabb.maxX, aabb.maxY, aabb.minZ);
	var7.addVertex(aabb.minX, aabb.maxY, aabb.minZ);
	var7.setNormal(-1.0F, 0.0F, 0.0F);
	var7.addVertex(aabb.minX, aabb.minY, aabb.maxZ);
	var7.addVertex(aabb.minX, aabb.maxY, aabb.maxZ);
	var7.addVertex(aabb.minX, aabb.maxY, aabb.minZ);
	var7.addVertex(aabb.minX, aabb.minY, aabb.minZ);
	var7.setNormal(1.0F, 0.0F, 0.0F);
	var7.addVertex(aabb.maxX, aabb.minY, aabb.minZ);
	var7.addVertex(aabb.maxX, aabb.maxY, aabb.minZ);
	var7.addVertex(aabb.maxX, aabb.maxY, aabb.maxZ);
	var7.addVertex(aabb.maxX, aabb.minY, aabb.maxZ);
	var7.setTranslation(0.0D, 0.0D, 0.0D);
	var7.draw();
	GL11.glEnable(GL11.GL_TEXTURE_2D);*/
}

void EntityRender::renderAABB(const Box& aabb)
{
	/*Tessellator var1 = Tessellator.instance;
	var1.startDrawingQuads();
	var1.addVertex(aabb.minX, aabb.maxY, aabb.minZ);
	var1.addVertex(aabb.maxX, aabb.maxY, aabb.minZ);
	var1.addVertex(aabb.maxX, aabb.minY, aabb.minZ);
	var1.addVertex(aabb.minX, aabb.minY, aabb.minZ);
	var1.addVertex(aabb.minX, aabb.minY, aabb.maxZ);
	var1.addVertex(aabb.maxX, aabb.minY, aabb.maxZ);
	var1.addVertex(aabb.maxX, aabb.maxY, aabb.maxZ);
	var1.addVertex(aabb.minX, aabb.maxY, aabb.maxZ);
	var1.addVertex(aabb.minX, aabb.minY, aabb.minZ);
	var1.addVertex(aabb.maxX, aabb.minY, aabb.minZ);
	var1.addVertex(aabb.maxX, aabb.minY, aabb.maxZ);
	var1.addVertex(aabb.minX, aabb.minY, aabb.maxZ);
	var1.addVertex(aabb.minX, aabb.maxY, aabb.maxZ);
	var1.addVertex(aabb.maxX, aabb.maxY, aabb.maxZ);
	var1.addVertex(aabb.maxX, aabb.maxY, aabb.minZ);
	var1.addVertex(aabb.minX, aabb.maxY, aabb.minZ);
	var1.addVertex(aabb.minX, aabb.minY, aabb.maxZ);
	var1.addVertex(aabb.minX, aabb.maxY, aabb.maxZ);
	var1.addVertex(aabb.minX, aabb.maxY, aabb.minZ);
	var1.addVertex(aabb.minX, aabb.minY, aabb.minZ);
	var1.addVertex(aabb.maxX, aabb.minY, aabb.minZ);
	var1.addVertex(aabb.maxX, aabb.maxY, aabb.minZ);
	var1.addVertex(aabb.maxX, aabb.maxY, aabb.maxZ);
	var1.addVertex(aabb.maxX, aabb.minY, aabb.maxZ);
	var1.draw();*/
}

void EntityRender::doRenderShadowAndFire(Entity* pEntity, const Vector3& pos, float yaw, float rdt)
{
	if (/*renderManager->options->fancyGraphics &&*/ shadowSize > 0.0F && !pEntity->isInvisible())
	{
		float dis = renderManager->getDistanceToCamera(pEntity->position);
		float alpha = (1.f - dis / 128.f) * shadowOpaque;

		if (alpha > 0.0F)
		{
			renderShadow(pEntity, pos, alpha, rdt);
		}
	}

	if (pEntity->canRenderOnFire())
	{
		renderEntityOnFire(pEntity, pos, rdt);
	}
}

void EntityRender::doRenderSelectedNode(Entity * pEntity, const Vector3 & pos, float yaw, float rdt)
{
	if (!pEntity->onSelectedChange)
		return;

	ActorObject* actor = EntityRenderManager::Instance()->getEntityActor(pEntity);
	if (!actor || !actor->getHasInited())
		return;

	if (pEntity->isOnSelected())
	{
		String top_mesh_name = "";
		String bottom_mesh_name = "";
		switch (pEntity->selectNodeType)
		{
		case SelectNodeType::NODE_BLUE:
			top_mesh_name = "node_blue_top.mesh";
			bottom_mesh_name = "node_blue_bottom.mesh";
			break;
		case SelectNodeType::NODE_RED:
			top_mesh_name = "node_red_top.mesh";
			bottom_mesh_name = "node_red_bottom.mesh";
			break;
		default:
			break;
		}
		if (top_mesh_name.length() == 0)
			return;
		float width = pEntity->getWidth() + 0.2f;
		float height = pEntity->getHeight() + 0.2f;
		float length = pEntity->getLength() + 0.2f;
		actor->DeleteChildMesh("SelectNode1");
		actor->DeleteChildMesh("SelectNode2");
		actor->DeleteChildMesh("SelectNode3");
		actor->DeleteChildMesh("SelectNode4");
		actor->DeleteChildMesh("SelectNode5");
		actor->DeleteChildMesh("SelectNode6");
		actor->DeleteChildMesh("SelectNode7");
		actor->DeleteChildMesh("SelectNode8");
		actor->AddChildMesh("SelectNode1", "node1", top_mesh_name, Vector3(width / 2, height, -length / 2), Quaternion(Vector3::UNIT_Y, 180 * Math::DEG2RAD));
		actor->AddChildMesh("SelectNode2", "node2", top_mesh_name, Vector3(-width / 2, height, -length / 2), Quaternion(Vector3::UNIT_Y, 270 * Math::DEG2RAD));
		actor->AddChildMesh("SelectNode3", "node3", top_mesh_name, Vector3(-width / 2, height, length / 2), Quaternion(Vector3::UNIT_Y, 360 * Math::DEG2RAD));
		actor->AddChildMesh("SelectNode4", "node4", top_mesh_name, Vector3(width / 2, height, length / 2), Quaternion(Vector3::UNIT_Y, 90 * Math::DEG2RAD));
		actor->AddChildMesh("SelectNode5", "node5", bottom_mesh_name, Vector3(width / 2, 0.f, -length / 2), Quaternion(Vector3::UNIT_Y, 180 * Math::DEG2RAD));
		actor->AddChildMesh("SelectNode6", "node6", bottom_mesh_name, Vector3(-width / 2, 0.f, -length / 2), Quaternion(Vector3::UNIT_Y, 270 * Math::DEG2RAD));
		actor->AddChildMesh("SelectNode7", "node7", bottom_mesh_name, Vector3(-width / 2, 0.f, length / 2), Quaternion(Vector3::UNIT_Y, 360 * Math::DEG2RAD));
		actor->AddChildMesh("SelectNode8", "node8", bottom_mesh_name, Vector3(width / 2, 0.f, length / 2), Quaternion(Vector3::UNIT_Y, 90 * Math::DEG2RAD));
	}
	else
	{
		actor->DeleteChildMesh("SelectNode1");
		actor->DeleteChildMesh("SelectNode2");
		actor->DeleteChildMesh("SelectNode3");
		actor->DeleteChildMesh("SelectNode4");
		actor->DeleteChildMesh("SelectNode5");
		actor->DeleteChildMesh("SelectNode6");
		actor->DeleteChildMesh("SelectNode7");
		actor->DeleteChildMesh("SelectNode8");
	}

	pEntity->onSelectedChange = false;
}


}