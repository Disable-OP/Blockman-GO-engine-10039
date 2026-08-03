#include "SubMesh.h"
#include "Object/Root.h"

namespace LORD
{
	// 构造函数
	SubMesh::SubMesh()
		: m_pDiffTex( NULL)
		, m_VertexBuffer( NULL)
		, m_IndexBuffer( NULL)
		, m_vertStride( 0)
		, m_vertCount(0)
		, m_vertices(NULL)
		, m_idxCount(0)
		, m_idxStride(0)
		, m_indices(NULL)
		, m_pMaskTex(NULL)
		, m_bHasMaskTexture(false)
	{

	}

	// 析构函数
	SubMesh::~SubMesh()
	{
		unloadImpl();
	}

	// 获取漫反射纹理
	Texture* SubMesh::getDiffuseTexture() const
	{
		return m_pDiffTex;
	}

	// get mask texture
	Texture* SubMesh::getMasktexture() const
	{
		return m_pMaskTex;
	}

	// 获取顶点缓冲
	GPUBuffer* SubMesh::getVertexBuffer() const
	{
		return m_VertexBuffer;
	}

	// 获取索引缓冲
	GPUBuffer* SubMesh::getIndexBuffer() const
	{
		return m_IndexBuffer;
	}

	// 加载函数
	bool SubMesh::load()
	{
		Buffer vertBuff(m_vertStride*m_vertCount, m_vertices);
		m_VertexBuffer = Renderer::Instance()->createVertexBuffer(GPUBuffer::GBU_GPU_READ, vertBuff);
		Buffer indexBuff(m_idxCount*m_idxStride, m_indices);
		m_IndexBuffer = Renderer::Instance()->createIndexBuffer(GPUBuffer::GBU_GPU_READ, indexBuff);

		return true;
	}

	// 重建
	void SubMesh::rebuildVertexFomat_Remove(RenderInput::VertexSemantic semantic)
	{
		if(!hasVertexUsage(semantic))
			return;

		LordSafeDelete(m_VertexBuffer);
//		LordSafeDelete(m_IndexBuffer);

		RenderInput::VertexElementList mlastElements = m_VertexElements;
		m_VertexElements.clear();
		size_t removeStrite;
		size_t oldElementsSize = mlastElements.size();

		ui32 vertOffFront = 0;
		ui32 allStrite = 0;
		ui32 vertOffBack = 0;
		size_t j;
		for(j=0; j<oldElementsSize; ++j)
		{
			if(mlastElements[j].semantic == semantic)
			{
				removeStrite = PixelUtil::GetPixelSize(mlastElements[j].pixFmt);
				break;
			}
			else
			{
				m_VertexElements.push_back(mlastElements[j]);
				allStrite += PixelUtil::GetPixelSize(mlastElements[j].pixFmt);
			}
		}
		vertOffFront = allStrite;
		j++;
		for(; j<oldElementsSize; ++j)
		{
			m_VertexElements.push_back(mlastElements[j]);
			allStrite += PixelUtil::GetPixelSize(mlastElements[j].pixFmt);
			vertOffBack += PixelUtil::GetPixelSize(mlastElements[j].pixFmt);
		}

		m_vertStride = allStrite;

		Byte* pNewVertexArray = LordAlloc(Byte, allStrite*m_vertCount);
		Byte* pcopyDest = pNewVertexArray;
		Byte* pcopySrc = m_vertices;
		for(size_t vertIdx = 0; vertIdx<m_vertCount; ++vertIdx)
		{
			memcpy(pcopyDest, pcopySrc, vertOffFront);
			pcopyDest += vertOffFront;
			pcopySrc += vertOffFront+removeStrite;
			memcpy(pcopyDest, pcopySrc, vertOffBack);
			pcopyDest += vertOffBack;
			pcopySrc += vertOffBack;
		}

		LordFree(m_vertices);
		m_vertices = pNewVertexArray;

		Buffer vertBuff(m_vertStride*m_vertCount, m_vertices);
		m_VertexBuffer = Renderer::Instance()->createVertexBuffer(GPUBuffer::GBU_GPU_READ, vertBuff);
//		Buffer indexBuff(m_idxCount*m_idxStride, m_indices);
//		m_IndexBuffer = Renderer::Instance()->createIndexBuffer(GPUBuffer::GBU_GPU_READ, indexBuff);
	}

	// 重建
	void SubMesh::rebuildVertexFomat_Insert(const RenderInput::VertexElement& element, void* templateData)
	{
		if(hasVertexUsage(element.semantic))
			return;

		LordSafeDelete(m_VertexBuffer);
//		LordSafeDelete(m_IndexBuffer);

		RenderInput::VertexElementList mlastElements = m_VertexElements;
		m_VertexElements.clear();

		// first get the vertex semantic index to insert.
		ui32 insertIndex = 0;
		size_t oldElementsSize = mlastElements.size();
		for(size_t j=0; j<oldElementsSize; ++j)
		{
			if(mlastElements[j].semantic<element.semantic)
				insertIndex++;
			else
				break;
		}

		// position.
		ui32 vertOffFront = 0;
		ui32 allStrite = 0;
		ui32 vertOffBack = 0;
		size_t backindex = insertIndex;

		/// front
		for(size_t j=0; j<backindex; ++j)
		{
			m_VertexElements.push_back(mlastElements[j]);
			vertOffFront += PixelUtil::GetPixelSize(mlastElements[j].pixFmt);
		}

		m_VertexElements.push_back(element);

		/// back
		for(; backindex < oldElementsSize; ++backindex)
		{
			m_VertexElements.push_back(mlastElements[backindex]);
			vertOffBack += PixelUtil::GetPixelSize(mlastElements[backindex].pixFmt);
		}

		uint insertStrite = PixelUtil::GetPixelSize(element.pixFmt);
		allStrite = vertOffFront + insertStrite + vertOffBack;
		m_vertStride = allStrite;

		Byte* pNewVertexArray = LordAlloc(Byte, allStrite*m_vertCount);
		Byte* pcopyDest = pNewVertexArray;
		Byte* pcopySrc = m_vertices;
		for(size_t vertIdx = 0; vertIdx<m_vertCount; ++vertIdx)
		{
			memcpy(pcopyDest, pcopySrc, vertOffFront);
			pcopyDest += vertOffFront;
			pcopySrc += vertOffFront;
			memcpy(pcopyDest, templateData, insertStrite);
			pcopyDest += insertStrite;
			memcpy(pcopyDest, pcopySrc, vertOffBack);
			pcopyDest += vertOffBack;
			pcopySrc += vertOffBack;
		}

		LordFree(m_vertices);
		m_vertices = pNewVertexArray;

		Buffer vertBuff(m_vertStride*m_vertCount, m_vertices);
		m_VertexBuffer = Renderer::Instance()->createVertexBuffer(GPUBuffer::GBU_GPU_READ, vertBuff);
//		Buffer indexBuff(m_idxCount*m_idxStride, m_indices);
//		m_IndexBuffer = Renderer::Instance()->createIndexBuffer(GPUBuffer::GBU_GPU_READ, indexBuff);
	}

	// 卸载
	void SubMesh::unloadImpl()
	{
		LordSafeFree(m_vertices);
		LordSafeFree(m_indices);

		LordSafeDelete(m_VertexBuffer);
		LordSafeDelete(m_IndexBuffer);

		if(m_pDiffTex)
		{
			TextureManager::Instance()->releaseResource(m_pDiffTex);
			m_pDiffTex = NULL;
		}

		if (m_pMaskTex)
		{
			TextureManager::Instance()->releaseResource(m_pMaskTex);
			m_pMaskTex = NULL;
		}
	}

	uint SubMesh::getVertexStride() const
	{
		return m_vertStride;
	}

	uint SubMesh::getVertexCount() const
	{
		return m_vertCount;
	}

	Byte* SubMesh::getVertices() const
	{
		return m_vertices;
	}

	uint SubMesh::getIndexCount() const
	{
		return m_idxCount;
	}

	// 获取面数量
	uint SubMesh::getFaceCount() const
	{
		// triangle strip
		// return m_idxCount - 2;

		// triangle list
		return m_idxCount / 3;
	}

	uint SubMesh::getIndexStride() const
	{
		return m_idxStride;
	}

	Byte* SubMesh::getIndices() const
	{
		return m_indices;
	}

	// 计算占用内存大小
	size_t SubMesh::calculateSize() const
	{
		return m_vertCount*m_vertStride + m_idxCount*m_idxStride;
	}

	// ...
	void SubMesh::unprepareImpl()
	{
		if (!Root::Instance()->isEditorMode())
		{
			//LordSafeFree(m_vertices);
			//LordSafeFree(m_indices)
		}
	}

	// 判断顶点格式中是否含有指定类型的数据
	bool SubMesh::hasVertexUsage(RenderInput::VertexSemantic semantic) const
	{
		size_t num = m_VertexElements.size();
		for(size_t i=0; i<num; ++i)
		{
			if(m_VertexElements[i].semantic == semantic)
				return true;
		}

		return false;
	}

	// 获取顶点格式
	const RenderInput::VertexElementList& SubMesh::getVertexElements() const 
	{
		return m_VertexElements;
	}

	const Box& SubMesh::getLocalBox() const
	{
		return m_box;
	}
	
	void SubMesh::setTexture(ui32 uint, Texture* pTexture)
	{
		if (uint == 0)
		{
			if (m_pDiffTex)
			{
				TextureManager::Instance()->releaseResource(m_pDiffTex);
			}
			m_pDiffTex = pTexture;
		}
		else if (uint == 1)
		{
			if (m_pMaskTex)
			{
				TextureManager::Instance()->releaseResource(m_pMaskTex);
			}
			m_pMaskTex = pTexture;
		}
	}

}