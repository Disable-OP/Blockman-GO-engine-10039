/********************************************************************
filename: 	FontRenderable.h
file path:	dev\engine\Src\Core\Tessolator\

version:	1
author:		ajohn
company:	supernano
datetime:   2017-11-01
*********************************************************************/

#ifndef __LORD_FONT_RENDER_H__
#define __LORD_FONT_RENDER_H__

#include "Core.h"
#include "Tessolator/TessHeader.h"
#include "Render/RenderInput.h"

namespace LORD
{

class LORD_CORE_API FontRenderable : public ObjectAlloc
{
	friend class TessMaterial;

public:
	FontRenderable();
	~FontRenderable();

	void setQuadPoolSize(ui16 charCount);
	void destroyVertexPool();

	void setMaterialParam(Texture* texture);
	inline Texture* getTexture() const { return m_Texture; }
	inline GPUBuffer* getVertexBuffer() const { return m_VertexBuffer; }
	inline GPUBuffer* getIndexBuffer() const { return m_IndexBuffer; }
	void setIndexBuffer(GPUBuffer* indexBuffer) { m_IndexBuffer = indexBuffer; }
	void beginPolygon();
	float* getVertexMenory() { return m_VertexSysMemory; }


	inline bool getLockPtr(ui16 charCount, FontVertexFmt*& vertexAddr)
	{
		if (m_charCount + charCount <= m_charPoolSize)
		{
			vertexAddr = (FontVertexFmt*)(m_VertexSysMemory + m_charCount * 4 * sizeof(FontVertexFmt) / sizeof(float));
			m_charCount += charCount;
		}
		else
		{
			return false;
		}

		return true;
	}

	inline bool getLockPtr(ui16 charCount, SkinedVertexFmt*& vertexAddr)
	{
		return false;
	}

	inline void reversePtr(ui16 charCount)
	{
		m_charCount -= charCount;
	}

	inline bool isZTestEnabled() const
	{
		return m_zTestEnabled;
	}

	inline void setZTestEnabled(bool zTestEnabled)
	{
		m_zTestEnabled = zTestEnabled;
	}

	void endPolygon();
	bool empty() { return m_charCount == 0; }
	ui16 getCharCount() { return m_charCount; }
	ui16 getFreeCharNum() { return m_charPoolSize - m_charCount; }
	void setFreeCharNum(ui16 freeNum) { m_charCount = m_charPoolSize - freeNum; }

protected:
	ui16			m_charPoolSize;
	ui16			m_charCount;

	float*			m_VertexSysMemory;  // no need to release in running
	GPUBuffer*		m_VertexBuffer;
	GPUBuffer*		m_IndexBuffer;

	Texture*		m_Texture;

	bool			m_created;
	bool			m_zTestEnabled;
};

}

#endif
