/********************************************************************
filename: 	RenderInput.h
file path:	dev\engine\Src\Core\Render\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_RENDERINPUT_H__
#define __LORD_RENDERINPUT_H__

#include "Core.h"

namespace LORD
{

	class LORD_CORE_API RenderInput: public ObjectAlloc
	{
	public:
		enum VertexSemantic
		{
			// Position
			VS_POSITION, 
			// Normal
			VS_NORMAL, 
			// Diffuse & specular color
			VS_COLOR, 
			// Texture coordinates
			VS_TEXCOORD, 
			VS_TEXCOORD1, 
			// Blending indices
			VS_BLENDINDICES, 
			// Blending weights
			VS_BLENDWEIGHTS, 
			/// Tangent (X axis if normal is Z)
			VS_TANGENT, 
			/// Binormal (Y axis if normal is Z)
			VS_BINORMAL, 
			VS_MAX
		};

		enum TopologyType
		{
			// A list of points, 1 vertex per point
			TT_POINTLIST, 
			// A list of lines, 2 vertices per line
			TT_LINELIST, 
			// A strip of connected lines, 1 vertex per line plus 1 start vertex
			TT_LINESTRIP, 
			// A list of triangles, 3 vertices per triangle
			TT_TRIANGLELIST, 
			// A strip of triangles, 3 vertices for the first triangle, and 1 per triangle after that 
			TT_TRIANGLESTRIP, 
		};

		struct VertexElement
		{
			VertexSemantic		semantic;		// Vertex Semantic
			PixelFormat			pixFmt;			// Vertex pixel format			
		};
		typedef vector<VertexElement>::type	VertexElementList;

		struct VertexDeclaration
		{
			ui32	index;
			ui32	count;
			ui32	type;
			bool	bNormalize;
			ui32	elementOffset;
		};
		typedef vector<VertexDeclaration>::type	VertexDeclarationList;

		RenderInput(const VertexElementList &vertElms, ShaderProgram *pProgram);
		virtual ~RenderInput();

	public:
		virtual void					bindGPUBuffers(GPUBuffer *pVertGPUBuff, uint vertStride);
		virtual void					bindGPUBuffers(GPUBuffer *pVertGPUBuff, uint vertStride, GPUBuffer *pIdxGPUBuff, uint idxStride);
		virtual void					unbindGPUBuffers();
		virtual void					bind() = 0;
		virtual void					unbind() = 0;
		
		/*virtual void					setTopologyType(TopologyType type);
		virtual void					setStartVertex(uint startVert);
		virtual void					setBaseVertex(uint baseVert);
		virtual void					setVertexCount(uint vertCount);
		virtual void					setStartIndex(uint startIdx);
		virtual void					setIndexCount(uint idxCount);*/

		inline void setTopologyType(TopologyType type)
		{
			m_topologyType = type;
		}

		inline void setStartVertex(uint startVert)
		{
			m_startVert = startVert;
		}

		inline void setBaseVertex(uint baseVert)
		{
			m_baseVert = baseVert;
		}

		inline void setVertexCount(uint vertCount)
		{
			m_vertCount = vertCount;
		}

		inline void setStartIndex(uint startIdx)
		{
			m_startIdx = startIdx;
		}

		inline void setIndexCount(uint idxCount)
		{
			m_idxCount = idxCount;
		}

		inline TopologyType getTopologyType() const
		{
			return m_topologyType;
		}
		inline uint getStartVertex() const
		{
			return m_startVert;
		}

		inline uint getBaseVertex() const
		{
			return m_baseVert;
		}

		inline uint getStartIndex() const
		{
			return m_startIdx;
		}

		inline GPUBuffer* getVertexBuffer() const
		{
			return m_pVertBuff;
		}

		inline uint getVertexStride() const
		{
			return m_vertStride;
		}

		inline uint getVertexCount() const
		{
			return m_vertCount;
		}

		inline GPUBuffer* getIndexBuffer() const
		{
			return m_pIdxBuff;
		}

		inline uint getIndexStride() const
		{
			return m_idxStride;
		}

		inline uint getIndexCount() const
		{
			return m_idxCount;
		}
		/*virtual TopologyType			getTopologyType() const;
		virtual uint					getStartVertex() const;
		virtual uint					getBaseVertex() const;
		virtual uint					getStartIndex() const;
		virtual GPUBuffer*				getVertexBuffer() const;
		virtual uint					getVertexStride() const;
		virtual uint					getVertexCount() const;
		virtual GPUBuffer*				getIndexBuffer() const;
		virtual uint					getIndexStride() const;
		virtual uint					getIndexCount() const;*/
		
	protected:
		VertexDeclarationList	m_vertDeclaration;
		TopologyType			m_topologyType;
		uint					m_startVert;
		uint					m_baseVert;
		uint					m_startIdx;
		GPUBuffer*				m_pVertBuff;
		uint					m_vertStride;
		uint					m_vertCount;
		GPUBuffer*				m_pIdxBuff;
		uint					m_idxStride;
		uint					m_idxCount;
	};
}

#endif
