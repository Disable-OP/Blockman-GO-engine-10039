#include "Core.h"
#include "StaticMeshObject.h"
#include "Scene/ModelEntity.h"
#include "Model/MeshManager.h"
#include "Scene/SceneManager.h"
#include "Object/Root.h"
#include "Util/QueryObject.h"
#include "Scene/Scene.h"
#include "Model/Mesh.h"
#include "Scene/SceneNode.h"
#include "Render/ShaderProgramManager.h"

namespace LORD
{

	StaticMeshObject::StaticMeshObject()
		: GameObject()
		, m_modelEntity(NULL)
		, m_pMesh(NULL)
		, m_UVScale(Vector2::ZERO)
	{
		m_Type = GOT_SMESH;
		editor_canMirror = true;
		editor_canRotate = true;
		editor_canScale = true;
		
		m_ShaderDefine = "StaticMeshForScene";
	}

	StaticMeshObject::~StaticMeshObject()
	{
		_unprepare();
	}

	void StaticMeshObject::_prepare_io()
	{
		m_pMesh->prepare();
	}

	void StaticMeshObject::_prepare_res()
	{
		m_Material->setShaderProgram(ShaderProgramManager::Instance()->getShaderProgram(m_ShaderDefine));
		m_pMesh = MeshManager::Instance()->createMesh(m_Resource, true);
	}

	void StaticMeshObject::_prepare_obj()
	{
		if (!m_pMesh->load())
		{
			LordLogError("Mesh: Mesh file isn't exist. [%s]", m_Resource.c_str());
#if ( LORD_PLATFORM == LORD_PLATFORM_WINDOWS )
			char buf[MAX_PATH];
			sprintf_s(buf, MAX_PATH, "Mesh file isn't exist. [%s]", m_Resource.c_str());
			MessageBox(0, buf, "Error", 0);
#endif
			return;
		}

		// test code. for the resource.
		RenderInput::VertexElement ve;
		ve.semantic = RenderInput::VS_COLOR;
		ve.pixFmt = PF_RGBA8_UNORM;
		Dword color = 0xFFFFFFFF;
		m_pMesh->rebuildVertexFomat_Insert(ve, &color);

		m_ParentNode = SceneManager::Instance()->getRootNode();
		m_SceneNode = m_ParentNode->createChild();

		m_modelEntity = LordNew ModelEntity;
		m_modelEntity->setMesh(m_pMesh);
		m_modelEntity->attachTo(m_SceneNode);
		m_modelEntity->setMaterial(m_Material);
		m_modelEntity->setRenderType(ModelEntity::RT_FOR_SCENE);
		m_modelEntity->createRenderable();

		// ----update scene node
		m_SceneNode->setWorldPosition(m_Position);
		m_SceneNode->setWorldOrientation(m_Oritation);
		m_SceneNode->setLocalScaling(m_Scale);
		m_SceneNode->update();
		updateWorldAABB();

		Scene* scene = SceneManager::Instance()->getCurrentScene();
		LordAssert(scene);
		scene->registerGameObjectToZoom(this);


		m_IOprepared = true;
	}

	void StaticMeshObject::_prepare_editor()
	{
		if (Root::Instance()->isEditorMode())
		{
			Scene* pScene = SceneManager::Instance()->getCurrentScene();
			QueryObjectManager* pManager = pScene->getQueryObjectManager();

			m_pHoverBoxQueryObject = pManager->createQueryObject(m_Name + "hover_box");
			m_pHoverBoxQueryObject->setQueryFlag(0);
			m_pHoverBoxQueryObject->attachTo(m_SceneNode);

			m_pHoverBoxQueryObject->createAABBFrame(m_modelEntity->getLocalAABB(), Color::BLUE);

			m_pSelectBoxQueryObject = pManager->createQueryObject(m_Name + "select_box");
			m_pSelectBoxQueryObject->setQueryFlag(0);
			m_pSelectBoxQueryObject->attachTo(m_SceneNode);

			m_pSelectBoxQueryObject->createAABBFrame(m_modelEntity->getLocalAABB(), Color::YELLOW);
		}
	}

	void StaticMeshObject::_unprepare()
	{
		if (m_IOprepared)
		{
			Scene* pScene = SceneManager::Instance()->getCurrentScene();
			pScene->unregisterGameObjectFromZoom(this);
			pScene->registerGameObjectToZoom_unprepare(this);

			if (Root::Instance()->isEditorMode())
			{
				QueryObjectManager* pManager = pScene->getQueryObjectManager();

				pManager->destroyQueryObject(m_pHoverBoxQueryObject);
				pManager->destroyQueryObject(m_pSelectBoxQueryObject);
				m_pHoverBoxQueryObject = NULL;
				m_pSelectBoxQueryObject = NULL;
			}
			m_ParentNode->destroyChild(m_SceneNode);
			m_SceneNode = NULL;

			LordSafeDelete(m_modelEntity);

			m_IOprepared = false;
		}

		if (m_pMesh)
		{
			MeshManager::Instance()->releaseResource(m_pMesh);
			m_pMesh = NULL;
		}
	}

	std::pair<bool, Real> StaticMeshObject::rayIntersects(const Ray& ray)
	{
		Mesh* pMesh = m_pMesh;
		Matrix4 m_InversMat = m_SceneNode->getWorldMatrix();
		m_InversMat = m_InversMat.detInverse();
		Ray rayObject = ray;
		rayObject.transform(m_InversMat);

		return rayIntersectsMesh(rayObject, pMesh);
	}

	void StaticMeshObject::_render()
	{
		LordAssert(m_modelEntity);
	
		if (m_bVisible)
		{
			m_modelEntity->update();
			m_modelEntity->updateRenderQueue(m_RenderQueue);
		}
		if (Root::Instance()->isEditorMode())
		{
			m_pHoverBoxQueryObject->setVisible(getHovered());
			m_pSelectBoxQueryObject->setVisible(getSelected());
		}
	}

	const Box& StaticMeshObject::getWorldAABB() const
	{
		LordAssert(m_modelEntity);
		return m_worldAABB;
	}

	void StaticMeshObject::updateWorldAABB()
	{
		LordAssert(m_modelEntity);
		m_worldAABB = m_modelEntity->getLocalAABB().transform(m_SceneNode->getWorldMatrix());
		//m_Center = m_worldAABB.getCenter();
		if (Root::Instance()->isEditorMode())
		{
			if (m_pHoverBoxQueryObject)
			{
				m_pHoverBoxQueryObject->createAABBFrame(m_modelEntity->getLocalAABB(), Color::BLUE);
			}
			if (m_pSelectBoxQueryObject)
			{
				m_pSelectBoxQueryObject->createAABBFrame(m_modelEntity->getLocalAABB(), Color::YELLOW);
			}
		}
	}

	void StaticMeshObject::setUVScale( float u, float v )
	{
		m_UVScale.x = u;
		m_UVScale.y = v;
	}

	void StaticMeshObject::frameMove( i32 deltaTime )
	{
		m_modelEntity->updateUVanimation(m_UVScale*(float)deltaTime*0.001f);
	}

	void StaticMeshObject::getPropertyList( PropertyList & lst )
	{
		GameObject::getPropertyList(lst);
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_SM_UVSCALE], true));
	}

	bool StaticMeshObject::getPropertyType( const String & name, PropertyType &type )
	{
		if (GameObject::getPropertyType(name, type))
		{
			return true;
		}
		if (name == objToken[TOKEN_SM_UVSCALE])
		{
			type = PT_VECTOR2; return true;
		}

		return false;
	}

	bool StaticMeshObject::getPropertyValue( const String & name, String &value )
	{
		if (GameObject::getPropertyValue(name, value))
		{
			return true;
		}
		if (name == objToken[TOKEN_SM_UVSCALE])
		{
			value = StringUtil::ToString(m_UVScale); return true;
		}
		return false;
	}

	bool StaticMeshObject::setPropertyValue( const String & name,const String & value )
	{
		if (GameObject::setPropertyValue(name, value))
		{
			return true;
		}
		if (name == objToken[TOKEN_SM_UVSCALE])
		{
			m_UVScale = StringUtil::ParseVec2(value);
			if (m_IOprepared && m_UVScale == Vector2::ZERO)
			{
				m_modelEntity->resetUVanimation();
			}
			return true;
		}
		return false;
	}

}
