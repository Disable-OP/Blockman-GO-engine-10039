/********************************************************************
filename: 	ItemRenderable.h
file path:	dev\engine\Src\Core\Tessolator\

version:	1
author:		ajohn
company:	supernano
datetime:   2017-05-04
*********************************************************************/

#ifndef __LORD_ITEM_RENDERABLE_H__
#define __LORD_ITEM_RENDERABLE_H__

#include "Core.h"
#include "Tessolator/TessHeader.h"
#include "Render/RenderInput.h"

namespace LORD
{

class LORD_CORE_API ItemRenderMesh : public ObjectAlloc
{
public:
	ItemRenderMesh();
	~ItemRenderMesh();

	void setQuadPoolSize(ui16 quadSize);
	void destroyVertexPool();

	inline GPUBuffer* getVertexBuffer() const { return m_VertexBuffer; }
	inline GPUBuffer* getIndexBuffer() const { return m_IndexBuffer; }
	void setIndexBuffer(GPUBuffer* indexBuffer) { m_IndexBuffer = indexBuffer; }
	void beginPolygon();
	inline bool getLockPtr(ui16 quadNum, ItemVertexFmt*& vertexAddr)
	{
		if (m_quadNum + quadNum <= m_quadPoolSize)
		{
			vertexAddr = (ItemVertexFmt*)(m_VertexSysMemory + m_quadNum * 4 * sizeof(ItemVertexFmt) / sizeof(float));
			m_quadNum += quadNum;
		}
		else
		{
			return false;
		}

		return true;
	}

	inline void reversePtr(ui16 vertexNum)
	{
		m_quadNum -= vertexNum;
	}

	void endPolygon();
	ui16 getQuadNum() { return m_quadNum; }
	ui16 getFreeQuadNum() { return m_quadPoolSize - m_quadNum; }


protected:
	ui16			m_quadPoolSize;
	ui16			m_quadNum;

	float*			m_VertexSysMemory;
	GPUBuffer*		m_VertexBuffer;
	GPUBuffer*		m_IndexBuffer;
};

class LORD_CORE_API ItemRenderable : public ObjectAlloc
{
	friend class TessMaterial;

public:
	ItemRenderable(ITEM_RENDER_PASS pass = IRP_SOLID);
	~ItemRenderable();

	void setMesh(ItemRenderMesh* pMesh) { m_pItemMesh = pMesh; }
	void setBrightness(const Color& b) { m_brightness = b; }
	void setWorldMatrix(const Matrix4& worldMat) { m_worldMat = worldMat; }
	void setViewSpace(bool viewer) { m_viewSpace = viewer; }
	void setItemKey(int key) { m_key = key; }
	
	void setMaterialParam(Texture* tex, ITEM_RENDER_PASS group) { m_texture = tex; m_RenderGroup = group; }
	inline Texture* getTexture() const { return m_texture; }
	inline ITEM_RENDER_PASS getRenderGroup() const { return m_RenderGroup; }
	ItemRenderMesh* getMesh() { return m_pItemMesh; }
	const Matrix4& getWorldMatrix() const { return m_worldMat; }
	const Color& getBrightness() const { return m_brightness; }
	bool isViewSpace() const { return m_viewSpace; }
	int getItemKey() const { return m_key; }

protected:
	ItemRenderMesh* m_pItemMesh;

	Texture*		m_texture;
	ITEM_RENDER_PASS m_RenderGroup;
	Color			m_brightness;
	Matrix4			m_worldMat;
	bool			m_viewSpace;
	int				m_key;
};

}

#endif