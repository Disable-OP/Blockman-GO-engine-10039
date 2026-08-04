#include "TileEntityRenders.h"
#include "TileEntityRenderMgr.h"
#include "TileEntityRenderable.h"
#include "Model/ModelBox.h"
#include "Model/ModelTileEntity.h"
#include "Render/RenderBlocks.h"
#include "cBlock/cBlock.h"
#include "cBlock/cBlockManager.h"

#include "Block/Block.h"
#include "Block/Blocks.h"
#include "Block/BlockManager.h"
#include "TileEntity/TileEntityInventory.h"
#include "TileEntity/TileEntitys.h"
#include "World/World.h"

#include "UI/GUIFontManager.h"
#include "UI/GUIImage.h"
#include "UI/GUIImageset.h"
#include "UI/GUIFontGlyph.h"
#include "UI/GUIFont.h"
#include "Tessolator/FontRenderable.h"
#include "Tessolator/TessRenderable.h"

namespace BLOCKMAN
{

TEChestRender::TEChestRender()
{
	m_texTrapped_double = TextureManager::Instance()->createTexture("trapped_double.png");
	m_texTrapped_double->load();
	m_texChristmas_double = TextureManager::Instance()->createTexture("christmas_double.png");
	m_texChristmas_double->load();
	m_texNormal_double = TextureManager::Instance()->createTexture("normal_double.png");
	m_texNormal_double->load();
	m_texTrapped = TextureManager::Instance()->createTexture("trapped.png");
	m_texTrapped->load();
	m_texChristmas = TextureManager::Instance()->createTexture("christmas.png");
	m_texChristmas->load();
	m_texNormal = TextureManager::Instance()->createTexture("normal.png");
	m_texNormal->load();

	m_texNormal_double_new = TextureManager::Instance()->createTexture("normal_new_double.png");
	m_texNormal_double_new->load();
	m_texNormal_new = TextureManager::Instance()->createTexture("normal_new.png");
	m_texNormal_new->load();
	m_texTrapped_double_new = TextureManager::Instance()->createTexture("trapped_new_double.png");
	m_texTrapped_double_new->load();
	m_texTrapped_new = TextureManager::Instance()->createTexture("trapped_new.png");
	m_texTrapped_new->load();

	m_texCurrent = NULL;
	m_model = NULL;

	// todo.
	isChristmas = false;
	chestModel = LordNew ModelChest(false);
	largeChestModel = LordNew ModelChest(true);
}

TEChestRender::~TEChestRender()
{
	TextureManager::Instance()->releaseResource(m_texTrapped_double);
	TextureManager::Instance()->releaseResource(m_texChristmas_double);
	TextureManager::Instance()->releaseResource(m_texNormal_double);
	TextureManager::Instance()->releaseResource(m_texTrapped);
	TextureManager::Instance()->releaseResource(m_texChristmas);
	TextureManager::Instance()->releaseResource(m_texNormal);

	TextureManager::Instance()->releaseResource(m_texNormal_double_new);
	TextureManager::Instance()->releaseResource(m_texNormal_new);
	TextureManager::Instance()->releaseResource(m_texTrapped_double_new);
	TextureManager::Instance()->releaseResource(m_texTrapped_new);

	m_texTrapped_double = nullptr;
	m_texChristmas_double = nullptr;
	m_texNormal_double = nullptr;
	m_texTrapped = nullptr;
	m_texChristmas = nullptr;
	m_texNormal = nullptr;

	m_texNormal_double_new = nullptr;
	m_texNormal_new = nullptr;
	m_texTrapped_double_new = nullptr;
	m_texTrapped_new = nullptr;

	LordSafeDelete(chestModel);
	LordSafeDelete(largeChestModel);
}

void TEChestRender::renderTileEntityAt(TileEntity* pTileEntity, const Vector3& pos, int bright, float rdt)
{
	renderTileEntityAt_impl((TileEntityChest*)pTileEntity, pos, bright, rdt);
}

void TEChestRender::renderTileEntityToTessRender(TileEntity* pTileEntity, const Vector3& pos, int bright, TessRenderable* pTarget)
{
	/** render chest in hold item. */
	int meta = 0;
	m_model = chestModel;
	TileEntityChest* pChest = (TileEntityChest*)(pTileEntity);

// 	if (pChest->getChestType() == 1)
// 		m_texCurrent = m_texTrapped;
// 	else if (isChristmas)
// 		m_texCurrent = m_texChristmas;
// 	else
// 		m_texCurrent = m_texNormal;

	if (isChristmas)
	{
		m_texCurrent = m_texChristmas;
	}
	else
	{
		switch (pChest->getChestType())
		{
		case BlockChest::BTT_BLOCK_NOT_TRAPPED:
			m_texCurrent = m_texNormal;
			break;
		case BlockChest::BTT_BLOCK_TRAPPED:
			m_texCurrent = m_texTrapped;
			break;
		case BlockChest::BTT_BLOCK_NOT_TRAPPED_NEW:
			m_texCurrent = m_texNormal_new;
			break;
		case BlockChest::BTT_BLOCK_TRAPPED_NEW:
			m_texCurrent = m_texTrapped_new;
			break;
		default:
			m_texCurrent = m_texNormal;
			break;
		}
	}

	pTarget->setMaterialParam(m_texCurrent, TRP_SOLID);
	m_model->b_lid->m_pitch = m_model->b_knob->m_pitch = 0.f;
	Matrix4 matChest;
	matChest.identity();
	matChest.translate(pos);
	Vector2i texSize;
	texSize.x = m_texCurrent->getWidth();
	texSize.y = m_texCurrent->getHeight();
	m_model->compileTessRender(pTarget, matChest, Vector2i::ZERO, texSize, bright);
}

void TEChestRender::renderTileEntityAt_impl(TileEntityChest* pChest, const Vector3& pos, int bright, float rdt)
{
	int meta;

	if (!pChest->hasWorldObj())
	{
		meta = 0;
	}
	else
	{
		Block* pBlock = pChest->getBlock();
		meta = pChest->getBlockMetadata();

		BlockChest* pBlockChest = dynamic_cast<BlockChest*>(pBlock);
		if (pBlockChest && meta == 0)
		{
			pBlockChest->unifyAdjacentChests(pChest->getWorldObj(), pChest->m_pos);
			meta = pChest->getBlockMetadata();
		}

		if (pChest->isChestOpen() && pChest->numUsingPlayers == 0)
		{
			pChest->openChest();
		}

		if (!pChest->isChestOpen() && pChest->numUsingPlayers == 1)
		{
			pChest->closeChest();
		}

		pChest->checkForAdjacentChests();
	}

	if (pChest->adjacentChestZNeg == nullptr &&
		pChest->adjacentChestXNeg == nullptr)
	{
		if (pChest->adjacentChestXPos == nullptr &&
			pChest->adjacentChestZPos == nullptr)
		{
			m_model = chestModel;

// 			if (pChest->getChestType() == 1)
// 				m_texCurrent = m_texTrapped;
// 			else if (isChristmas)
// 				m_texCurrent = m_texChristmas;
// 			else
// 				m_texCurrent = m_texNormal;

			if (isChristmas)
			{
				m_texCurrent = m_texChristmas;
			}
			else
			{
				switch (pChest->getChestType())
				{
				case BlockChest::BTT_BLOCK_NOT_TRAPPED:
					m_texCurrent = m_texNormal;
					break;
				case BlockChest::BTT_BLOCK_TRAPPED:
					m_texCurrent = m_texTrapped;
					break;
				case BlockChest::BTT_BLOCK_NOT_TRAPPED_NEW:
					m_texCurrent = m_texNormal_new;
					break;
				case BlockChest::BTT_BLOCK_TRAPPED_NEW:
					m_texCurrent = m_texTrapped_new;
					break;
				default:
					m_texCurrent = m_texNormal;
					break;
				}
			}
		}
		else
		{
			m_model = largeChestModel;

// 			if (pChest->getChestType() == 1)
// 				m_texCurrent = m_texTrapped_double;
// 			else if (isChristmas)
// 				m_texCurrent = m_texChristmas_double;
// 			else
// 				m_texCurrent = m_texNormal_double;

			if (isChristmas)
			{
				m_texCurrent = m_texChristmas_double;
			}
			else
			{
				switch (pChest->getChestType())
				{
				case BlockChest::BTT_BLOCK_NOT_TRAPPED:
					m_texCurrent = m_texNormal_double;
					break;
				case BlockChest::BTT_BLOCK_TRAPPED:
					m_texCurrent = m_texTrapped_double;
					break;
				case BlockChest::BTT_BLOCK_NOT_TRAPPED_NEW:
					m_texCurrent = m_texNormal_double_new;
					break;
				case BlockChest::BTT_BLOCK_TRAPPED_NEW:
					m_texCurrent = m_texTrapped_double_new;
					break;
				default:
					m_texCurrent = m_texNormal;
					break;
				}
			}
		}

		TileEntityRenderable* pRenderable = TileEntityRenderMgr::Instance()->getTileEntityRenderable(pChest);
		LordAssert(pRenderable);
		pRenderable->checkTexutreAndModel(m_texCurrent, m_model);
		Matrix4 matWorld, matTemp;
		matWorld.identity(); matWorld.translate(pos + Vector3(0.5f, 0.f, 0.5f));

		float radian = 0;
		if (meta == 2)
			radian = 0.f;
		if (meta == 3)
			radian = Math::PI;
		if (meta == 4)
			radian = Math::PI_DIV2;
		if (meta == 5)
			radian = -Math::PI_DIV2;

		if (m_model == largeChestModel)
		{
			if (meta == 2 || meta == 0)
				matWorld.translate(Vector3(0.f, 0.f, -0.5f + 0.0625f));
			if (meta == 3)
				matWorld.translate(Vector3(1.f, 0.f, 0.5f - 0.0625f));
			if (meta == 4)
				matWorld.translate(Vector3(-0.5f + 0.0625f, 0.f, 1.f));
			if (meta == 5)
				matWorld.translate(Vector3(0.5f - 0.0625f, 0.f, 0.f));
		}
			

		matTemp.identity(); matTemp.rotateY(radian);
		matWorld.joint(matTemp);
		// GL11.glRotatef((float)angle, 0.0F, 1.0F, 0.0F);
		// GL11.glTranslatef(-0.5F, -0.5F, -0.5F);
		float lidAngle = pChest->prevLidAngle + (pChest->lidAngle - pChest->prevLidAngle) * rdt;
		float neighborLidAngle = 0.f;

		if (pChest->adjacentChestZNeg != nullptr)
		{
			TileEntityChest* neighbor = pChest->adjacentChestZNeg;
			neighborLidAngle = neighbor->prevLidAngle + (neighbor->lidAngle - neighbor->prevLidAngle) * rdt;

			if (neighborLidAngle > lidAngle)
				lidAngle = neighborLidAngle;
		}

		if (pChest->adjacentChestXNeg != nullptr)
		{
			TileEntityChest* neighbor = pChest->adjacentChestXNeg;
			neighborLidAngle = neighbor->prevLidAngle + (neighbor->lidAngle - neighbor->prevLidAngle) * rdt;

			if (neighborLidAngle > lidAngle)
				lidAngle = neighborLidAngle;
		}

		lidAngle = 1.0F - lidAngle;
		lidAngle = 1.0F - lidAngle * lidAngle * lidAngle;
		
		m_model->b_lid->m_pitch = m_model->b_knob->m_pitch = lidAngle * Math::PI_DIV2;
		pRenderable->updateAnimation();
		Color b(bright);
		pRenderable->setBrightness(Color(bright));
		pRenderable->setWorldMatrix(matWorld);
		pRenderable->updateRender();
	}
}

TEEnderChestRenderer::TEEnderChestRenderer()
{
	m_texture = TextureManager::Instance()->createTexture("ender.png");
	m_texture->load();
	m_texture_new = TextureManager::Instance()->createTexture("ender_new.png");
	m_texture_new->load();
	m_model = LordNew ModelChest(false);
}

TEEnderChestRenderer::~TEEnderChestRenderer()
{
	TextureManager::Instance()->releaseResource(m_texture);
	TextureManager::Instance()->releaseResource(m_texture_new);
	m_texture = nullptr;
	m_texture_new = nullptr;
	LordSafeDelete(m_model);
}

void TEEnderChestRenderer::renderTileEntityAt(TileEntity* pTileEntity, const Vector3& pos, int bright, float rdt)
{
	renderTileEntityAt_impl((TileEntityEnderChest*)pTileEntity, pos, bright, rdt);
}

void TEEnderChestRenderer::renderTileEntityToTessRender(TileEntity* pTileEntity, const Vector3& pos, int bright, TessRenderable* pTarget)
{
	/** render chest in hold item. */
	int meta = 0;
	TileEntityChest* pChest = (TileEntityChest*)(pTileEntity);

	switch (pChest->getChestType())
	{
	case BlockEnderChest::BECT_OLD:
		m_texCurrent = m_texture;
		break;
	case BlockEnderChest::BECT_NEW:
		m_texCurrent = m_texture_new;
		break;
	default:
		m_texCurrent = m_texture;
		break;
	}

	pTarget->setMaterialParam(m_texCurrent, TRP_SOLID);
	m_model->b_lid->m_pitch = m_model->b_knob->m_pitch = 0.f;
	Matrix4 matChest;
	matChest.identity();
	matChest.translate(pos);
	Vector2i texSize;
	texSize.x = m_texCurrent->getWidth();
	texSize.y = m_texCurrent->getHeight();
	m_model->compileTessRender(pTarget, matChest, Vector2i::ZERO, texSize, bright);
}

void TEEnderChestRenderer::renderTileEntityAt_impl(TileEntityEnderChest* pChest, const Vector3& pos, int bright, float rdt)
{
	int meta = 0;

	if (pChest->hasWorldObj())
	{
		meta = pChest->getBlockMetadata();
	}

	switch (pChest->getChestType())
	{
	case BlockEnderChest::BECT_OLD:
		m_texCurrent = m_texture;
		break;
	case BlockEnderChest::BECT_NEW:
		m_texCurrent = m_texture_new;
		break;
	default:
		m_texCurrent = m_texture;
		break;
	}

	TileEntityRenderable* pRenderable = TileEntityRenderMgr::Instance()->getTileEntityRenderable(pChest);
	LordAssert(pRenderable);
	pRenderable->checkTexutreAndModel(m_texCurrent, m_model);
	Matrix4 matWorld, matTemp;
	matWorld.identity(); matWorld.translate(pos + Vector3(0.5f, 0.f, 0.5f));

	float radian = 0;
	if (meta == 2)
		radian = 0.f;
	if (meta == 3)
		radian = Math::PI;
	if (meta == 4)
		radian = Math::PI_DIV2;
	if (meta == 5)
		radian = -Math::PI_DIV2;

	matTemp.identity(); matTemp.rotateY(radian);
	matWorld.joint(matTemp);

	float lidAngle = pChest->m_prevLidAngle + (pChest->m_lidAngle - pChest->m_prevLidAngle) * rdt;
	lidAngle = 1.0F - lidAngle;
	lidAngle = 1.0F - lidAngle * lidAngle * lidAngle;

	m_model->b_lid->m_pitch = m_model->b_knob->m_pitch = lidAngle * Math::PI_DIV2;
	pRenderable->updateAnimation();
	Color b(bright);
	pRenderable->setBrightness(Color(b.r, b.b, 1.f, 1.f));
	pRenderable->setWorldMatrix(matWorld);
	pRenderable->updateRender();
}

TESignRenderer::TESignRenderer()
{
	m_texture = TextureManager::Instance()->createTexture("sign.png");
	m_texture->load();
	m_modelPost = LordNew ModelSign(true);
	m_modelWall = LordNew ModelSign(false);
	m_model = m_modelPost;
}

TESignRenderer::~TESignRenderer()
{
	TextureManager::Instance()->releaseResource(m_texture);
	m_texture = NULL;
	LordSafeDelete(m_modelPost);
	LordSafeDelete(m_modelWall);
}

void TESignRenderer::renderTileEntityAt(TileEntity* pTileEntity, const Vector3& pos, int bright, float rdt)
{
	renderTileEntityAt_impl((TileEntitySign*)pTileEntity, pos, bright, rdt);
}

void TESignRenderer::renderTileEntityToTessRender(TileEntity* pTileEntity, const Vector3& pos, int bright, TessRenderable* pTarget)
{
	/** render chest in hold item. */
	int meta = 0;
	TileEntitySign* pSign = (TileEntitySign*)(pTileEntity);
	pTarget->setMaterialParam(m_texture, TRP_SOLID);
	Matrix4 matSign;
	matSign.identity();
	matSign.translate(pos);
	Vector2i texSize;
	texSize.x = m_texture->getWidth();
	texSize.y = m_texture->getHeight();
	m_modelWall->compileTessRender(pTarget, matSign, Vector2i::ZERO, texSize, bright);
}

void TESignRenderer::renderTileEntityAt_impl(TileEntitySign* pSign, const Vector3& pos, int bright, float rdt)
{
	Block* pBlock = pSign->getBlock();
	if (pBlock == nullptr)
		return;
	float s = 0.6666667F;

	TileEntityRenderable* pRenderable = TileEntityRenderMgr::Instance()->getTileEntityRenderable(pSign);
	LordAssert(pRenderable);

	Matrix4 matWorld, matTemp;
	matWorld.identity(); matWorld.translate(pos + Vector3(0.5f, 0.f, 0.5f));

	if (pBlock->getBlockID() == BLOCK_ID_SIGN_POST)
	{
		float angle = (float)(pSign->getBlockMetadata() * 360) / 16.0F;
		matTemp.identity(); matTemp.rotateY(-angle * Math::DEG2RAD);
		matWorld.joint(matTemp);
		m_model = m_modelPost;
	}
	else
	{
		int meta = pSign->getBlockMetadata();
		float radian = 0.0F;

		if (meta == 2)
			radian = Math::PI;
		if (meta == 4)
			radian = Math::PI_DIV2;
		if (meta == 5)
			radian = -Math::PI_DIV2;

		matTemp.identity(); matTemp.rotateY(-radian);
		matWorld.joint(matTemp);
		matTemp.identity(); matTemp.translate(0.f, -0.3125f, -0.4375f);
		matWorld.joint(matTemp);
		m_model = m_modelWall;
	}

	matTemp.identity(); matTemp.scale(s, s, s);
	matWorld.joint(matTemp);

	pRenderable->checkTexutreAndModel(m_texture, m_model);
	pRenderable->updateAnimation();
	Color b(bright);
	pRenderable->setBrightness(Color(bright));
	pRenderable->setWorldMatrix(matWorld);
	pRenderable->updateRender();

	renderSignText(pSign, matWorld);
}

void TESignRenderer::renderSignText(TileEntitySign* pSign, const Matrix4& matWorld)
{
	// render text.
	// use the default font.
	struct CharacterInfo
	{
		Vector2 pos;
		Vector2 size;
		ERect	uv;
		Texture* tex;
	};
	GUIFont* pFont = GUIFontManager::Instance()->GetFont("HT28");

	vector<CharacterInfo>::type buffer;

	for (int i = 0; i < TileEntitySign::SIGN_COUNT; ++i)
	{
		GUIString guiStr = pSign->getText(i).c_str();
		if (guiStr.empty())
			continue;
		
		// memset(szbuff, 0, 256);
		// StringUtil::GBKToUTF8(szbuff, len, str.c_str(), str.length());
		// GUIString guiStr(szbuff);
		// b_board->setPosition(Vector3(-12.f, 12.f, -1.F) * scale, Vector3(24.f, 12.f, 2.f) * scale, 0.f);  // 1/24 is 1 metre.
		// height is 0.5 / 4.0 metre. line space is 0.025f.
		float lineSpace = 0.025f;
		float fontHeight = 0.71f / 4.0f - lineSpace;  // 0.1f;
		float heightScale = fontHeight / pFont->GetFontHeight();
		utf16 cCode;
		float totalWidth = 0.f;
		vector<CharacterInfo>::type buffer_line;
		for (size_t c = 0; c < guiStr.length(); ++c)
		{
			cCode = guiStr[c];
			const GUIFontGlyph* glyph = pFont->GetGlyphData(cCode);
			if (!glyph)
				continue;

			float charWidth = glyph->GetWidth() * heightScale;
			float charHeight = glyph->GetHeight() * heightScale;
			if (totalWidth + charWidth > 1.5f)
				break;
			
			CharacterInfo info;
			const GUIImage* const img = glyph->GetImage();
			info.tex = img->GetImageset()->GetTexture();
			info.uv = glyph->GetImage()->GetUV();
			Vector2 offset = img->GetOffset() * heightScale;
			info.pos.x = totalWidth + offset.x;
			info.pos.y = 1.5f - (0.71f / 4.f) * (i+1) - offset.y - charHeight;
			info.size.x = charWidth;
			info.size.y = charHeight;
			buffer_line.push_back(info);

			totalWidth += charWidth;
		}
		// adjust all the xposition of all the characters.
		for (auto& it : buffer_line)
			it.pos.x -= totalWidth / 2.f;
		buffer.insert(buffer.end(), buffer_line.begin(), buffer_line.end());
	}
	FontRenderable* renderable = NULL;
	FontVertexFmt* pVertex = NULL;
	TileEntityRenderMgr* mgr = TileEntityRenderMgr::Instance();
	for (auto& it : buffer)
	{
		renderable = mgr->getFontRenderable(it.tex, true);
		if (!renderable->getLockPtr(1, pVertex))
			continue;

		float z = 0.072f;
		Vector3 v1(it.pos.x, it.pos.y, z);
		Vector3 v2(it.pos.x, it.pos.y + it.size.y, z);
		Vector3 v3(it.pos.x + it.size.x, it.pos.y + it.size.y, z);
		Vector3 v4(it.pos.x + it.size.x, it.pos.y, z);
		pVertex[0].set(v1 * matWorld, 0xFF222222, it.uv.left, it.uv.bottom);
		pVertex[3].set(v2 * matWorld, 0xFF222222, it.uv.left, it.uv.top);
		pVertex[2].set(v3 * matWorld, 0xFF222222, it.uv.right, it.uv.top);
		pVertex[1].set(v4 * matWorld, 0xFF222222, it.uv.right, it.uv.bottom);
	}
}

TEPistonRenderer::TEPistonRenderer()
{
	m_blockRender = nullptr;
	m_texture = TextureManager::Instance()->createTexture("Blocks.png");
	m_texture->load();
}

TEPistonRenderer::~TEPistonRenderer()
{
	LordSafeDelete(m_blockRender);
	TextureManager::Instance()->releaseResource(m_texture);
	m_texture = NULL;
}

void TEPistonRenderer::renderTileEntityAt(TileEntity* pTileEntity, const Vector3& pos, int bright, float rdt)
{
	renderTileEntityAt_impl((TileEntityPiston*)pTileEntity, pos, bright, rdt);
}

void TEPistonRenderer::renderTileEntityAt_impl(TileEntityPiston* piston, const Vector3& pos, int bright, float rdt)
{
	int blockID = piston->getStoredBlockID();
	Block* pBlock = BlockManager::sBlocks[blockID];
	cBlock* pcBlock = cBlockManager::scBlocks[blockID];

	TessRenderable* renderalbe = TileEntityRenderMgr::Instance()->getTessRenderable(m_texture, TRP_SOLID, 14);
	m_blockRender->setSpecialRenderable(renderalbe);

	if (pBlock && piston->getProgress(rdt) < 1.0F)
	{
		m_blockRender->setTranslate(pos - Vector3(piston->m_pos) + piston->getOffset(rdt));

		if (blockID == BLOCK_ID_PISTON_EXTENSION && piston->getProgress(rdt) < 0.5F)
		{
			m_blockRender->renderPistonExtension(pBlock, pcBlock, piston->m_pos, false);
		}
		else if (piston->shouldRenderHead() && !piston->isExtending())
		{
			cBlockPistonExtension* pExtension = (cBlockPistonExtension*)cBlockManager::pistonExtension;
			cBlockPistonBase* pBase = (cBlockPistonBase*)pBlock;
			pExtension->setHeadTexture(pBase->getPistonExtensionTexture());
			m_blockRender->renderPistonExtension(BlockManager::pistonExtension, pExtension, piston->m_pos, piston->getProgress(rdt) < 0.5F);
			pExtension->clearHeadTexture();
			m_blockRender->setTranslate(pos - piston->m_pos);
			m_blockRender->renderPistonBase(pBlock, pBase, piston->m_pos, true);
		}
		else
		{
			m_blockRender->renderBlockAllFaces(pBlock, piston->m_pos);
		}

		m_blockRender->setTranslate(Vector3::ZERO);
	}
}

void TEPistonRenderer::onWorldChange(World* pWorld)
{
	m_blockRender = LordNew RenderBlocks(pWorld);
}

}
