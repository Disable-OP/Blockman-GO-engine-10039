#include "TileEntityRenderable.h"
#include "TileEntity/TileEntity.h"
#include "TileEntityRenders.h"
#include "TileEntityRenderMgr.h"
#include "Model/ModelBase.h"

#include "Tessolator/SkinedRenderable.h"
#include "Tessolator/SkinedRenderQueue.h"
#include "Tessolator/TessManager.h"

namespace BLOCKMAN
{

TileEntityRenderable::TileEntityRenderable(TileEntity* pTileEntity)
	: m_tileEntity(pTileEntity)
	, m_skin(NULL)
	, m_texture(NULL)
	, m_texSize(64, 32)
	, m_model(NULL)
	, m_boneMatrix(NULL)
	, m_boneCount(0)
	, m_worldMat(Matrix4::IDENTITY)
{
	LordAssert(pTileEntity);
	m_renderer = TileEntityRenderMgr::Instance()->getSpecialRendererForEntity(pTileEntity);
	LordAssert(m_renderer);
	
	invalid();
}

TileEntityRenderable::~TileEntityRenderable()
{
	release();
}

void TileEntityRenderable::release()
{
	LordSafeDelete(m_skin);
	LordSafeFree(m_boneMatrix);

	m_model = NULL;
	m_tileEntity = NULL;
	m_texture = NULL;
	m_boneCount = 0;
}

void TileEntityRenderable::setTileEntity(TileEntity* pTileEntity)
{
	LordAssert(pTileEntity);

	if (pTileEntity == m_tileEntity)
		return;

	release();
	m_tileEntity = pTileEntity;
	m_renderer = TileEntityRenderMgr::Instance()->getSpecialRendererForEntity(pTileEntity);
	LordAssert(m_renderer);

	invalid();
}

void TileEntityRenderable::compileGeometry()
{
	if (m_skin)
		return;

	if (!m_model)
		m_model = m_renderer->getModel();

	if (!m_model)
		return;

	int boneCount = m_model->getBoneCount();
	LordAssert(boneCount <= SkinedRenderable::s_maxBone);

	if (boneCount != m_boneCount)
	{
		LordSafeFree(m_boneMatrix);
		m_boneCount = boneCount;
	}

	if (!m_boneMatrix)
	{
		m_boneMatrix = (Vector4*)LordMalloc(sizeof(Vector4) * 3 * m_boneCount);
	}

	if (!m_skin)
	{
		m_skin = TessManager::Instance()->createSkinedRenderable(m_texture, SRP_SOLID);
		// fill the m_skin vertices
		m_skin->beginPolygon();
		m_model->compileSkinMesh(m_skin, Vector2i::ZERO, m_texSize);
		m_skin->endPolygon(true);
		m_skin->setBoneMatrices(m_boneMatrix);
		m_skin->setBoneCount(m_boneCount);
		m_skin->setWorldMatrix(&m_worldMat);
	}
}

void TileEntityRenderable::compileTexture()
{
	if (m_texture)
		return;

	// todo. 
	// currently all living use the player's default texture.
	m_texture = m_renderer->getTexture();
	m_texSize.x = m_texture->getWidth();
	m_texSize.y = m_texture->getHeight();
}

void TileEntityRenderable::checkTexutreAndModel(Texture* tex, ModelBase* model)
{
	if (m_texture != tex || m_model != model)
	{
		release();
		invalid();
	}
}

void TileEntityRenderable::setBrightness(const Color& b)
{
	if (m_skin)
		m_skin->setExtraColor(b);
}

void TileEntityRenderable::setColor(const Color& c)
{
	if (m_skin)
		m_skin->setExtraColor(c);
}

void TileEntityRenderable::updateAnimation()
{
	if (m_boneMatrix && m_model)
	{
		m_model->updateBones(m_boneMatrix);
	}
}

void TileEntityRenderable::updateRender()
{
	// post the renderable to tessolator manager.
	TessManager::Instance()->addSkinedRenderable(m_skin);
}

void TileEntityRenderable::invalid()
{
	compileTexture();
	compileGeometry();
}


}
