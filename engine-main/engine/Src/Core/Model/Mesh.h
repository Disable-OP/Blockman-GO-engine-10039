/********************************************************************
filename: 	Mesh.h
file path:	dev\engine\Src\Core\Model\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_MESH_H__
#define __LORD_MESH_H__

#include "Core.h"
#include "SubMesh.h"
#include "Render/RenderInput.h"
#include "Resource/Resource.h"
#include "Geom/Box.h"

namespace LORD
{
	class LORD_CORE_API Mesh: public Resource
	{
	public:
		friend class MeshManager;
		friend class MergeMeshManager;
		Mesh(const String& name);
		virtual ~Mesh();

	public:
		const Box&				getBox() const;

		// some function to re build
		virtual void			rebuildVertexFomat_Insert(const RenderInput::VertexElement& element, void* templateData);
		virtual void			rebuildVertexFomat_Remove(RenderInput::VertexSemantic semantic);

		// 获取版本号
		int getVersion() const { return m_version; }

		// 获取子模型数量
		int getSubMeshNum() const { return static_cast<int>( m_subMeshs.size()); }

		// 获取子模型
		SubMesh* getSubMesh( int idx);

		// 保存
		bool save( const char* filePath);

		void setTextureName(int submeshid, const String& name);
		String getTextureName(int submeshid, ui32 uint);

		bool isSkinned() { return m_isSkinned; }
	protected:
		virtual size_t			calculateSize() const;
		virtual bool			prepareImpl();
		bool					prepareImplInteral( bool isSkinned);
		virtual void			unprepareImpl();
		virtual bool			loadImpl();
		virtual void			unloadImpl();

	protected:
		int						m_version;			// 版本号
		bool					m_bNeedBackUp;
		DataStream*				m_pMeshStream;		// used for meshStream
		Box						m_box;
		vector<SubMesh*>::type	m_subMeshs;			// 子模型列表
		bool					m_bHasMaskTexture;
		bool					m_isSkinned;		// 是否为带动画模型
		vector<String>::type	m_DiffuseVector;
		//MeshCollisionData		m_collisionData;	// 碰撞数据
	};
}

#endif