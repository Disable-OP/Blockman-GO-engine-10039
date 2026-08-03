#include "Core.h"
#include "EffectRender.h"
#include "Render/Renderer.h"
#include "Render/GPUBuffer.h"
#include "EffectMaterial.h"

namespace LORD
{
	const size_t EffectRenderable::s_VertexSize = 6;

	EffectRenderable::EffectRenderable()
		: m_VertexSysMemory(NULL)
		, m_VertexBuffer(NULL)
		, m_VertexPoolSize(0)
		, m_VertexNum(0)
		, m_IndexSysMemory(NULL)
		, m_IndexBuffer(NULL)
		, m_IndexPoolSize(0)
		, m_IndexNum(0)
		, m_Texture(NULL)
		, m_RenderGroup(ERG_GROUPUP)
		, m_BlendMode(AlphaLightMode)
		, m_distortion( false )
		, m_distortionAlphaThreshold(0.5f)
		, m_distortionTexPtr(0)
		, m_onlyDistortionLayer(false)
		, m_pMtlEffect( 0 )
		, m_SpecailRender(false)
		, m_RenderableId(0)
		, m_viewSpace(false)
	{

	}

	EffectRenderable::~EffectRenderable()
	{
		destroyVertexPool();
		destroyIndexPool();
	}

	void EffectRenderable::createVertexPool(ui16 verticesNum)
	{
		m_VertexPoolSize = verticesNum;
		m_VertexSysMemory = (float*)(LordMalloc(verticesNum*sizeof(EffectVertexFmt)));
		Buffer vertBuff(verticesNum*sizeof(EffectVertexFmt), m_VertexSysMemory);
		m_VertexBuffer = Renderer::Instance()->createVertexBuffer(GPUBuffer::GBU_GPU_READ|GPUBuffer::GBU_CPU_WRITE, vertBuff);
		m_VertexNum = 0;
	}

	void EffectRenderable::createVertexPoolEx(void* vertexdata, ui16 verticesNum, ui32 id)
	{
		m_VertexPoolSize = verticesNum;
		m_VertexSysMemory = (float*)((EffectVertexFmt*)vertexdata + id*verticesNum);
		m_RenderableId = id;
		m_VertexNum = 0;
	}

	void EffectRenderable::createIndexPool(ui16 indicesNum)
	{
		m_IndexPoolSize = indicesNum;
		m_IndexSysMemory = (ui16*)(LordMalloc(indicesNum*sizeof(ui16)));
		Buffer idxBuff(indicesNum*sizeof(ui16), m_IndexSysMemory);
		m_IndexBuffer = Renderer::Instance()->createIndexBuffer(GPUBuffer::GBU_GPU_READ|GPUBuffer::GBU_CPU_WRITE, idxBuff);
		m_IndexNum = 0;

	}

	void EffectRenderable::createIndexPoolEx(ui16 indicesNum)
	{
		m_IndexPoolSize = indicesNum;
		m_IndexSysMemory = (ui16*)(LordMalloc(indicesNum*sizeof(ui16)));
		m_IndexNum = 0;
	}

	void EffectRenderable::destroyVertexPool()
	{
		if(!m_VertexSysMemory)
			return;

		if (m_VertexBuffer)
		{
			LordSafeDelete(m_VertexBuffer);
			LordSafeFree(m_VertexSysMemory);
		}
		else
		{
			m_VertexSysMemory = NULL;
		}
		m_VertexNum = 0;
		m_VertexPoolSize = 0;
	}

	void EffectRenderable::destroyIndexPool()
	{
		if(!m_IndexSysMemory)
			return;

		LordSafeDelete(m_IndexBuffer);
		LordSafeFree(m_IndexSysMemory);
		m_IndexNum = 0;
		m_IndexPoolSize = 0;
	}

	/*void EffectRenderable::beginPolygon()
	{
		m_VertexNum = 0;
		m_IndexNum = 0;
	}*/

	/*bool EffectRenderable::getLockPtr(size_t vertexNum, size_t indexNum, ui16& vertexBegin, EffectVertexFmt*& vertexAddr, ui16*& indexAddr)
	{
	vertexBegin = m_VertexNum;
	if(m_VertexNum + vertexNum < m_VertexPoolSize)
	{
	vertexAddr = (EffectVertexFmt*)(m_VertexSysMemory + m_VertexNum*s_VertexSize);
	m_VertexNum += vertexNum;
	} 
	else
	{
	return false;
	}

	if(m_IndexNum + indexNum < m_IndexPoolSize)
	{
	indexAddr = m_IndexSysMemory + m_IndexNum;
	m_IndexNum += indexNum;
	}
	else 
	{
	return false;
	}
	return true;
	}*/

	/*void EffectRenderable::reversePtr(size_t vertexNum, size_t indexNum)
	{
		m_VertexNum -= vertexNum;
		m_IndexNum -= indexNum;
	}*/

	void EffectRenderable::endPolygon()
	{
		if (m_VertexBuffer)
		{
			Buffer vbuff(m_VertexNum*s_VertexSize*sizeof(float), m_VertexSysMemory);
			m_VertexBuffer->updateSubData(0, vbuff);
		}
		if (m_IndexBuffer)
		{
			Buffer iBuff(m_IndexNum*sizeof(ui16), m_IndexSysMemory);
			m_IndexBuffer->updateSubData(0, iBuff);
		}
	}

	void EffectRenderable::setMaterialParam(Texture* texture, EFFECT_RENDER_GROUP group, EFFECT_BLEND_MODE blendmode, EffectMaterial* pEm )
	{
		m_Texture = texture;
		m_RenderGroup = group;
		m_BlendMode = blendmode;
		m_pMtlEffect = pEm;
	}

	/*void EffectRenderable::setDistortion( bool _val )
	{
		m_distortion = _val;
	}*/

	bool EffectRenderable::getDistortion() const
	{
		return m_pMtlEffect->getDistortionEnable();
	}

	const Vector4& EffectRenderable::getDistortionUVScaleOffset() const
	{
		m_pMtlEffect->getDistortionUVScaleOffset(m_distortionUVScaleOffset);
		return m_distortionUVScaleOffset;
	}

	const float& EffectRenderable::getDistortionAlphaThreshold() const
	{
		m_distortionAlphaThreshold = m_pMtlEffect->getDistortionAlphaThreshold();
		return m_distortionAlphaThreshold;
	}

	bool EffectRenderable::getOnyDistortionLayer() const
	{
		return m_pMtlEffect->getOnlyDistortionLayer();
	}

	Texture* EffectRenderable::getDistortionTexturePtr() const
	{
		return m_pMtlEffect->getDistortionTexturePtr();
	}
}