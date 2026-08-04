/********************************************************************
filename: 	ItemTesselation.h
file path:	dev\client\Src\Blockman\Model

version:	1
author:		ajohn
company:	supernano
date:		2017-5-4
*********************************************************************/
#ifndef __ITEM_TESSELATION_HEADER__
#define __ITEM_TESSELATION_HEADER__

#include <stdint.h>

#include "Item/Item.h"
#include "Core.h"

using namespace LORD;

namespace BLOCKMAN
{

class ItemStack;
class EntityPlayer;
class TextureAtlas;
class MemTextureAtlas;

/* fetch mesh data from the renderBlocks. */
class ItemBlockMesh : public ObjectAlloc
{
public:
	ItemBlockMesh(int blockID, int meta);
	~ItemBlockMesh();
	
	int		getKey() const;
	bool	buildMesh();
	int		getQuadNum() const { return m_quadNum; }
	bool	isValid() const;
	bool	renderToRenderable(SkinedRenderable* pRenderable, int brightness, int boneIdx);
	int		btyesInMemory() const;
	Texture* getTexture() { return m_texture; }
	const Box& getAABB() const { return m_aabb; }

protected:
	int					m_blockID = 0;
	Texture*			m_texture = nullptr;
	int					m_meta = 0;
	SkinedVertexFmt*	m_vertices = nullptr;
	int*				m_colorCached = nullptr;
	int					m_quadNum = 0;
	Box					m_aabb;
};

/** for ItemBlock render. */
class PackMassRenderable : public ObjectAlloc
{
public:
	PackMassRenderable(Texture* tex, SKINED_RENDER_PASS rp);
	~PackMassRenderable();
	
	int getFreeQuadNum() const;
	int getFreeBoneCount() const;
	Texture* getTexture() const;
	SKINED_RENDER_PASS getRenderGroup() const;
	void setMaterialParam(Texture* tex, SKINED_RENDER_PASS rq);
	bool addItemBlock(ItemBlockMesh* pBlockMesh, int bright, const Matrix4& worldMat);
	bool addPackMass(SkinedVertexFmt* pVertices, int quadCount, int bright, const Matrix4& worldMat);
	void beginPolygon();
	void endPolygon();
	void updateRenderQueue();
	size_t  getLastBlockKey() const { return m_lastBlockKey; }
	int  getBoneCount() const { return m_blockCount; }
	void setViewSpace(bool viewspace);
	void setBoneMatrix(int boneIdx, const Matrix4& worldMat);
	void setTextureAtlas(const Vector4& atlas);

protected:
	bool addBoneMatrix(const Matrix4& matWorld);

protected:
	size_t				m_lastBlockKey = 0;
	int					m_blockCount = 0;
	SkinedRenderable*	m_pRenderable = nullptr;
	Vector4*			m_pBoneMats = nullptr;
	Matrix4				m_worldMat;
};

typedef map<int, ItemBlockMesh*>::type ItemBlockMeshMap;
typedef map<uint32_t, ItemRenderMesh *>::type ItemRenderMeshMap;

/** for ItemModel render. */
class ItemModelManager : public Singleton<ItemModelManager>, public ObjectAlloc
{
public:
	ItemModelManager();
	~ItemModelManager();

	// void setAtlasFor32PixBlock(const String & atlasname);
	void setImage(int tessID, const String& imageName);
	void initBlockImage(MemTextureAtlas* atlas);

	ItemRenderMesh* getItemTesselation(ItemStackPtr pStack, EntityPlayer* pPlayer);
	ItemBlockMesh* getItemBlockMesh(int blockID, int meta);

protected:
	bool tesselateModel(int tessID, ItemRenderMesh* pOut, const Vector2i pos1, const Vector2i size1, const Color& c1,
		const Vector2i& pos2, const Vector2i& size2, const Color& c2);

protected:
	static const int ITEM_TESS_COUNT = 4;
	ItemRenderMesh** m_ItemMeshs = nullptr;
	ItemRenderMeshMap m_mapItemMeshs;
	ItemBlockMeshMap m_BlockMeshs;

	ui32*	m_colorBuffer[ITEM_TESS_COUNT] = { nullptr };

	int		m_imageWidth[ITEM_TESS_COUNT] = { 0 };
	int		m_imageHeight[ITEM_TESS_COUNT] = { 0 };
	
	int		m_blockMip = 2;
	MemTextureAtlas* m_pBlockAtlas = nullptr;
	// 由于block使用了64位的贴图，但是64位贴图细分不出模型，面数太多了。所以
	// ItemMesh 还是沿用原来32位的贴图。 于是，需要记录16位的atlas。
	// TextureAtlas* m_pAtlasBlock = nullptr;
};

}

#endif