/********************************************************************
filename: 	SubMesh.h
file path:	dev\engine\Src\Core\Model\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef _SUB_MESH_
#define _SUB_MESH_

#include "Core.h"
#include "Render/RenderInput.h"

namespace LORD
{
	/** SubMesh 2013-11-6 刘亮 */
	class Mesh;
	class LORD_CORE_API SubMesh : public ObjectAlloc
	{
		friend class Mesh;
		friend class SkinnedMesh;
	public:
		// 获取名称
		inline const String& getName() const { return m_name; }

		void setTexture(ui32 uint, Texture* pTexture);

		// 获取漫反射纹理
		Texture* getDiffuseTexture() const;
		
		// get mask texture
		Texture* getMasktexture() const;

		// 获取顶点缓冲
		GPUBuffer* getVertexBuffer() const;

		// 获取索引缓冲
		GPUBuffer* getIndexBuffer() const;

		// 获取顶点格式大小
		uint getVertexStride() const;

		// 获取顶点数量
		uint getVertexCount() const;

		// 获取顶点数据
		Byte* getVertices() const;

		// 获取面数量
		uint getFaceCount() const;

		// 获取索引数量
		uint getIndexCount() const;

		// 获取索引格式大小
		uint getIndexStride() const;

		// 获取索引数据
		Byte* getIndices() const;

		// 判断顶点格式中是否含有指定类型的数据
		bool hasVertexUsage(RenderInput::VertexSemantic semantic) const;

		// 获取顶点格式
		const RenderInput::VertexElementList& getVertexElements() const;

		// 获取本地包围盒
		const Box& getLocalBox() const;

		// 获取受影响的骨骼数量
		inline uint getBoneNum() const { return static_cast<uint>(m_boneIdxs.size()); }

		// 获取受影响的骨骼索引
		inline uint getBoneIdx( int idx) { return m_boneIdxs[idx]; }

		// 获取光照图大小
		inline int getLightmapResolution() const { return m_lightmapResolution; }

		// 设置光照图大小
		inline void setLightmapResolution( int resolution) { m_lightmapResolution = resolution; }

	private:
		SubMesh();
		~SubMesh();

		// 加载
		bool load();

		// 卸载
		void unloadImpl();

		// ...
		void unprepareImpl();

		// 重建
		void rebuildVertexFomat_Insert(const RenderInput::VertexElement& element, void* templateData);
		void rebuildVertexFomat_Remove(RenderInput::VertexSemantic semantic);

		// 计算占用内存大小
		size_t calculateSize() const;

	private:
		String				m_name;					// 名称
		Box					m_box;					// 包围盒
		bool				m_isUseNormal;			// 是否使用法线
		bool				m_isUseVertexColor;		// 是否使用顶点色
		bool				m_isUseLightmapUV;		// 是否使用光照图UV
		bool				m_isUseTangentBinormal; // 是否使用切线与副线
		int					m_lightmapResolution;	// 光照图分辨率
		uint				m_vertCount;			// 顶点数量
		uint				m_vertStride;			// 顶点格式大小
		Byte*				m_vertices;				// 顶点数据
		uint				m_idxCount;				// 索引数量
		uint				m_idxStride;			// 索引格式大小
		Byte*				m_indices;				// 索引数据
		GPUBuffer*			m_VertexBuffer;			// 顶点缓冲
		GPUBuffer*			m_IndexBuffer;			// 索引缓冲
		Texture*			m_pDiffTex;				// 漫反射纹理
		Texture*			m_pMaskTex;				// mask
		bool				m_bHasMaskTexture;
		vector<uint>::type  m_boneIdxs;				// 骨骼索引

		RenderInput::VertexElementList m_VertexElements; // 顶点格式
	};
}
#endif