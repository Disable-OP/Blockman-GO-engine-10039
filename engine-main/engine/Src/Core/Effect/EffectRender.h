/********************************************************************
filename: 	EffectRender.h
file path:	dev\engine\Src\Core\Effect\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_EFFECT_RENDER__
#define __LORD_EFFECT_RENDER__

#include "EffectHeader.h"
#include "Render/RenderInput.h"
#include "Render/Texture.h"
#include "Scene/Camera.h"

namespace LORD
{
	class LORD_CORE_API EffectRenderable
	{
		friend class EffectMaterial;

	public:
		EffectRenderable();
		~EffectRenderable();

		void createVertexPool(ui16 verticesNum);
		void createVertexPoolEx(void* vertexdata, ui16 verticesNum, ui32 id);
		void destroyVertexPool();
		void createIndexPool(ui16 indicesNum);
		void createIndexPoolEx(ui16 indicesNum);
		void destroyIndexPool();

		void setMaterialParam(Texture* texture, EFFECT_RENDER_GROUP group, EFFECT_BLEND_MODE blendmode, EffectMaterial* pEm = 0);
		inline Texture* getTexture() const { return m_Texture; }
		inline EFFECT_BLEND_MODE getBlendMode() const { return m_BlendMode; }
		inline EFFECT_RENDER_GROUP getRenderGroup() const { return m_RenderGroup; }
		inline GPUBuffer* getVertexBuffer() const { return m_VertexBuffer; }
		inline GPUBuffer* getIndexBuffer() const { return m_IndexBuffer; }
		inline ui16*	  getIndexSysMemory() const { return m_IndexSysMemory; }

		inline void beginPolygon()
		{
			m_VertexNum = 0;
			m_IndexNum = 0;
		}

		inline bool getLockPtr(ui16 vertexNum, ui16 indexNum, ui16& vertexBegin, EffectVertexFmt*& vertexAddr, ui16*& indexAddr)
		{
			vertexBegin = m_RenderableId*m_VertexPoolSize + m_VertexNum; // m_VertexNum
			if(m_VertexNum + vertexNum < m_VertexPoolSize)
			{
				vertexAddr = (EffectVertexFmt*)(m_VertexSysMemory + m_VertexNum*sizeof(EffectVertexFmt) / sizeof(float));
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
		}

		inline void reversePtr(ui16 vertexNum, ui16 indexNum)
		{
			m_VertexNum -= vertexNum;
			m_IndexNum -= indexNum;
		}

		void endPolygon();

		ui16   getVertexNum() { return m_VertexNum; }
		ui16   getVertexPoolSize() { return m_VertexPoolSize; }
		ui16   getIndexNum() { return m_IndexNum; }
		ui16   getIndexPoolSize() { return m_IndexPoolSize; }

		/*void setDistortion( bool _val );*/
		bool getDistortion() const;
		inline void setDistortion( bool _val )
		{
			m_distortion = _val;
		}

		const Vector4& getDistortionUVScaleOffset() const;

		const float& getDistortionAlphaThreshold() const;

		bool getOnyDistortionLayer() const;

		Texture* getDistortionTexturePtr() const;

		void setSpecialRender(bool bSpecial) { m_SpecailRender = bSpecial; }
		bool isSpecialRender() const { return m_SpecailRender; }

		void setViewSpace(bool viewer) { m_viewSpace = viewer; }
		bool isViewSpace() const { return m_viewSpace; }

	protected:
		static const size_t  s_VertexSize;

		EFFECT_RENDER_GROUP m_RenderGroup;
		EFFECT_BLEND_MODE   m_BlendMode;

		bool			m_distortion;
		bool			m_onlyDistortionLayer;
		bool			m_SpecailRender;

		ui16			m_VertexPoolSize;
		ui16			m_VertexNum;
		ui16			m_IndexPoolSize;
		ui16			m_IndexNum;

		ui32			m_RenderableId;

		mutable float	m_distortionAlphaThreshold;
		float*			m_VertexSysMemory;
		GPUBuffer*		m_VertexBuffer;
		ui16*			m_IndexSysMemory;
		GPUBuffer*		m_IndexBuffer;

		Texture*		m_Texture;
		Texture*		m_distortionTexPtr;
		EffectMaterial* m_pMtlEffect;

		mutable Vector4	m_distortionUVScaleOffset;
		String			m_distortionTexName;
		bool			m_viewSpace;
	};
}

#endif