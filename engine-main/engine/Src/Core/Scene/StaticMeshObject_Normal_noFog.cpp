#include "Core.h"
#include "StaticMeshObject_Normal_noFog.h"
#include "ModelEntity.h"
#include "Model/MeshManager.h"
#include "SceneManager.h"
#include "Object/Root.h"
#include "Util/QueryObject.h"
#include "Scene.h"
#include "Model/Mesh.h"
#include "SceneNode.h"
#include "Render/ShaderProgramManager.h"

namespace LORD
{
	StaticMeshObject_Normal_noFog::StaticMeshObject_Normal_noFog()
		: GameObject()
		, m_modelEntity(NULL)
		, m_pMesh(NULL)
		, m_UVScale(Vector2::ZERO)
		, m_bAlphaTest(false)
		, m_bGray(false)
	{
		m_Type = GOT_SMESH_L_NOFOG;
		editor_canMirror = true;
		editor_canRotate = true;
		editor_canScale = true;

		SamplerState::SamplerDesc desc = m_Material->getSamplerState(0)->getDesc();
		desc.minFilter = SamplerState::FO_LINEAR;
		desc.magFilter = SamplerState::FO_LINEAR;
		desc.mipFilter = SamplerState::FO_POINT;
		desc.addrUMode = SamplerState::AM_WRAP;
		desc.addrVMode = SamplerState::AM_WRAP;
		m_Material->setSamplerState(0, desc);

		m_ShaderDefine = "StaticMeshNormalForSceneNoFog";
	}

	StaticMeshObject_Normal_noFog::~StaticMeshObject_Normal_noFog()
	{
		_unprepare();
	}
	
	bool StaticMeshObject_Normal_noFog::exportData( rapidxml::xml_node<>* parentNode, rapidxml::xml_document<>& doc )
	{
		GameObject::exportData(parentNode, doc);
		return true;
	}

	bool StaticMeshObject_Normal_noFog::importData( rapidxml::xml_node<>* parentNode )
	{
		GameObject::importData(parentNode);
		return true;
	}

	void StaticMeshObject_Normal_noFog::_prepare_io()
	{
		m_pMesh->prepare();
	}

	void StaticMeshObject_Normal_noFog::_prepare_res()
	{
		ShaderProgram* shaderProgram = ShaderProgramManager::Instance()->getShaderProgram(m_ShaderDefine);
		m_Material->setShaderProgram(shaderProgram);

		m_pMesh = MeshManager::Instance()->createMesh(m_Resource, true);
	}

	void StaticMeshObject_Normal_noFog::_prepare_obj()
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

		m_pMesh->rebuildVertexFomat_Remove(RenderInput::VS_TEXCOORD1);

		m_ParentNode = SceneManager::Instance()->getRootNode();
		m_SceneNode = m_ParentNode->createChild();

		m_modelEntity = LordNew ModelEntity;
		m_modelEntity->setMaterial(m_Material);
		m_modelEntity->setMesh(m_pMesh);
		m_modelEntity->attachTo(m_SceneNode);
		m_modelEntity->setRenderType(ModelEntity::RT_FOR_SCENE_L_NOFOG);
		m_modelEntity->EnableGrayRender(m_bGray);
		m_modelEntity->createRenderable();
		m_modelEntity->EnableAlphaTest(m_bAlphaTest);
		
		// ¼ÓÔØÄ£ÐÍÌùÍ¼
		m_modelEntity->prepareTexture(Root::Instance()->isEditorMode(), __FUNCTION__);

		// ----update scene node
		m_SceneNode->setWorldPosition(m_Position);
		m_SceneNode->setWorldOrientation(m_Oritation);
		m_SceneNode->setLocalScaling(m_Scale);
		m_SceneNode->update();
		updateWorldAABB();

		Scene* scene = SceneManager::Instance()->getCurrentScene();
		LordAssert(scene);
		scene->registerGameObjectToZoom(this);
		String sceneName = scene->getSceneName();
		
		m_IOprepared = true;
	}

	void StaticMeshObject_Normal_noFog::_prepare_editor()
	{
		if (Root::Instance()->isEditorMode())
		{
			Scene* pScene = SceneManager::Instance()->getCurrentScene();
			QueryObjectManager* pManager = pScene->getQueryObjectManager();

			m_pHoverBoxQueryObject = pManager->createQueryObject(m_Name + "hover_box");
			m_pHoverBoxQueryObject->setQueryFlag(0);
			m_pHoverBoxQueryObject->attachTo(m_SceneNode);

			m_pHoverBoxQueryObject->createAABBFrame(m_modelEntity->getLocalAABB(), Color::BLUE);
			m_pHoverBoxQueryObject->setTestDepth(true);
			m_pHoverBoxQueryObject->setWriteDepth(true);

			m_pSelectBoxQueryObject = pManager->createQueryObject(m_Name + "select_box");
			m_pSelectBoxQueryObject->setQueryFlag(0);
			m_pSelectBoxQueryObject->attachTo(m_SceneNode);

			m_pSelectBoxQueryObject->createAABBFrame(m_modelEntity->getLocalAABB(), Color::YELLOW);
			m_pSelectBoxQueryObject->setTestDepth(true);
			m_pSelectBoxQueryObject->setWriteDepth(true);
		}
	}

	void StaticMeshObject_Normal_noFog::_unprepare()
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

	std::pair<bool, Real> StaticMeshObject_Normal_noFog::rayIntersects(const Ray& ray)
	{
		if (!m_bVisible)
			return std::pair<bool, Real>(false, 0.0f);

		std::pair<bool, Real> result = GameObject::rayIntersects(ray);
		if (!result.first)
			return result;

		Mesh* pMesh = m_pMesh;
		Ray rayObject = ray;
		rayObject.transform(m_InversMat);

		return rayIntersectsMesh(rayObject, pMesh);
	}

	void StaticMeshObject_Normal_noFog::_render()
	{
		if (m_bVisible)
		{
			LordAssert(m_modelEntity);
			m_modelEntity->update();
			m_modelEntity->updateRenderQueue(m_RenderQueue);
		}

		if (Root::Instance()->isEditorMode())
		{
			m_pHoverBoxQueryObject->setVisible(getHovered());
			m_pSelectBoxQueryObject->setVisible(getSelected());
		}
	}

	const Box& StaticMeshObject_Normal_noFog::getWorldAABB() const
	{
		LordAssert(m_modelEntity);
		return m_worldAABB;
	}

	void StaticMeshObject_Normal_noFog::updateWorldAABB()
	{
		LordAssert(m_modelEntity);
		m_worldAABB = m_modelEntity->getLocalAABB().transform(m_SceneNode->getWorldMatrix());
		m_InversMat = m_SceneNode->getWorldMatrix();
		m_InversMat.detInverse();
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

	const Matrix4& StaticMeshObject_Normal_noFog::getWorldMatrix() const
	{
		return m_SceneNode->getWorldMatrix();
	}

	void StaticMeshObject_Normal_noFog::setUVScale( float u, float v )
	{
		m_UVScale.x = u;
		m_UVScale.y = v;
	}

	void StaticMeshObject_Normal_noFog::frameMove( i32 deltaTime )
	{
		m_modelEntity->updateUVanimation(m_UVScale*(float)deltaTime*0.001f);
	}

	void StaticMeshObject_Normal_noFog::getPropertyList( PropertyList & lst )
	{
		GameObject::getPropertyList(lst);
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_SM_UVSCALE], true));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_SM_VISIBLE], true));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_SM_ALPHATEST], true));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_SM_ISGRAY], true));
	}

	bool StaticMeshObject_Normal_noFog::getPropertyType( const String & name, PropertyType &type )
	{
		if (GameObject::getPropertyType(name, type))
		{
			return true;
		}
		if (name == objToken[TOKEN_SM_UVSCALE])
		{
			type = PT_VECTOR2; return true;
		}
		if (name == objToken[TOKEN_SM_VISIBLE])
		{
			type = PT_BOOL; return true;
		}
		if (name == objToken[TOKEN_SM_ALPHATEST])
		{
			type = PT_BOOL; return true;
		}
		if (name == objToken[TOKEN_SM_ISGRAY])
		{
			type = PT_BOOL; return true;
		}
		return false;		
	}

	bool StaticMeshObject_Normal_noFog::getPropertyValue( const String & name, String &value )
	{
		if (GameObject::getPropertyValue(name, value))
		{
			return true;
		}
		if (name == objToken[TOKEN_SM_UVSCALE])
		{
			value = StringUtil::ToString(m_UVScale); return true;
		}
		if (name == objToken[TOKEN_SM_VISIBLE])
		{
			value = StringUtil::ToString(m_bVisible); return true;
		}
		if (name == objToken[TOKEN_SM_ALPHATEST])
		{
			value = StringUtil::ToString(m_bAlphaTest); return true;
		}
		if (name == objToken[TOKEN_SM_ISGRAY])
		{
			value = StringUtil::ToString(m_bGray); return true;
		}
		return false;
	}

	bool StaticMeshObject_Normal_noFog::setPropertyValue( const String & name,const String & value )
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
		if (name == objToken[TOKEN_SM_VISIBLE])
		{
			m_bVisible = StringUtil::ParseBool(value);
			return true;
		}
		if (name == objToken[TOKEN_SM_ALPHATEST])
		{
			m_bAlphaTest = StringUtil::ParseBool(value);
			if (m_bAlphaTest)
			{
				m_ShaderDefine = "StaticMeshNormalForSceneNoFog";
			}
			else
			{
				m_ShaderDefine = "StaticMeshNormalForSceneNoFogAlpha";
			}
			if (m_IOprepared)
			{
				m_Material->setShaderProgram(ShaderProgramManager::Instance()->getShaderProgram(m_ShaderDefine));
				m_modelEntity->EnableAlphaTest(m_bAlphaTest);
			}
			return true;
		}
		if (name == objToken[TOKEN_SM_ISGRAY])
		{
			m_bGray = StringUtil::ParseBool(value);
			return true;
		}
		return false;	
	}

	LORD::ui32 StaticMeshObject_Normal_noFog::editor_getSubMeshNum() const
	{
		assert(m_pMesh);
		return m_pMesh->getSubMeshNum();
	}

	void StaticMeshObject_Normal_noFog::changeDark(float radio)
	{
		if (m_IOprepared)
		{
			m_modelEntity->setDarkRadio(radio);
		}
	}

	void StaticMeshObject_Normal_noFog::revertLight()
	{
		if (m_IOprepared)
		{
			m_modelEntity->setDarkRadio(1.0f);
		}
	}

	void StaticMeshObject_Normal_noFog::EnableGrayRender(bool gray)
	{
		m_bGray = gray;
	}

}
