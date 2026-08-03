#include "Core.h"
#include "HazeObject.h"
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
	HazeObejct::HazeObejct()
		: GameObject()
		, m_modelEntity(NULL)
		, m_pMesh(NULL)
		, m_Alpha(1.0f)
	{
		m_Type = GOT_HAZE;
		editor_canMirror = true;
		editor_canRotate = true;
		editor_canScale = true;

		BlendState::BlendDesc blenddesc = m_Material->getBlendState()->getDesc();
		blenddesc.bBlendEnable = true;
		blenddesc.srcBlend = BlendState::BF_SRC_ALPHA;
		blenddesc.dstBlend = BlendState::BF_INV_SRC_ALPHA;
		blenddesc.srcAlphaBlend = BlendState::BF_SRC_ALPHA;
		blenddesc.dstAlphaBlend = BlendState::BF_INV_SRC_ALPHA;
		m_Material->setBlendState(blenddesc);
		
		SamplerState::SamplerDesc desc = m_Material->getSamplerState(0)->getDesc();
		desc.minFilter = SamplerState::FO_LINEAR;
		desc.magFilter = SamplerState::FO_LINEAR;
		desc.mipFilter = SamplerState::FO_POINT;
		desc.addrUMode = SamplerState::AM_WRAP;
		desc.addrVMode = SamplerState::AM_WRAP;
		m_Material->setSamplerState(0, desc);

		DepthStencilState::DepthStencilDesc depthdesc = m_Material->getDepthState()->getDesc();
		depthdesc.bDepthEnable = false;
		depthdesc.bWriteDepth = false;
		m_Material->setDepthState(depthdesc);

		m_ShaderDefine = "Haze";

		m_RenderQueue = SceneManager::Instance()->getRenderQueue("Effect");
	}

	HazeObejct::~HazeObejct()
	{
		_unprepare();
	}

	bool HazeObejct::exportData(rapidxml::xml_node<>* parentNode, rapidxml::xml_document<>& doc)
	{
		GameObject::exportData(parentNode, doc);
		return true;
	}

	bool HazeObejct::importData(rapidxml::xml_node<>* parentNode)
	{
		GameObject::importData(parentNode);
		return true;
	}

	void HazeObejct::_prepare_io()
	{
		m_pMesh->prepare();
	}

	void HazeObejct::_prepare_res()
	{
		ShaderProgram* shaderProgram = ShaderProgramManager::Instance()->getShaderProgram(m_ShaderDefine);
		m_Material->setShaderProgram(shaderProgram);

		m_pMesh = MeshManager::Instance()->createMesh(m_Resource, true);
	}

	void HazeObejct::_prepare_obj()
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

		m_ParentNode = SceneManager::Instance()->getRootNode();
		m_SceneNode = m_ParentNode->createChild();

		m_modelEntity = LordNew ModelEntity;
		m_modelEntity->setMaterial(m_Material);
		m_modelEntity->setMesh(m_pMesh);
		m_modelEntity->setNaturalColor(Color(0, 0, 0, m_Alpha));
		m_modelEntity->attachTo(m_SceneNode);
		m_modelEntity->setRenderType(ModelEntity::RT_FOR_HAZE);
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
		String sceneName = scene->getSceneName();

		m_IOprepared = true;
	}

	void HazeObejct::_prepare_editor()
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

	void HazeObejct::_unprepare()
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

	std::pair<bool, Real> HazeObejct::rayIntersects(const Ray& ray)
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

	void HazeObejct::_render()
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

	const Box& HazeObejct::getWorldAABB() const
	{
		LordAssert(m_modelEntity);
		return m_worldAABB;
	}

	void HazeObejct::updateWorldAABB()
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

	const Matrix4& HazeObejct::getWorldMatrix() const
	{
		return m_SceneNode->getWorldMatrix();
	}

	void HazeObejct::getPropertyList(PropertyList & lst)
	{
		GameObject::getPropertyList(lst);
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_SM_VISIBLE], true));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_HZ_ALPHA], true));
	}

	bool HazeObejct::getPropertyType(const String & name, PropertyType &type)
	{
		if (GameObject::getPropertyType(name, type))
		{
			return true;
		}
		if (name == objToken[TOKEN_SM_VISIBLE])
		{
			type = PT_BOOL; return true;
		}
		if (name == objToken[TOKEN_HZ_ALPHA])
		{
			type = PT_FLOAT; return true;
		}
		return false;
	}

	bool HazeObejct::getPropertyValue(const String & name, String &value)
	{
		if (GameObject::getPropertyValue(name, value))
		{
			return true;
		}
		if (name == objToken[TOKEN_SM_VISIBLE])
		{
			value = StringUtil::ToString(m_bVisible); return true;
		}
		if (name == objToken[TOKEN_HZ_ALPHA])
		{
			value = StringUtil::ToString(m_Alpha); return true;
		}
		return false;
	}

	bool HazeObejct::setPropertyValue(const String & name, const String & value)
	{
		if (GameObject::setPropertyValue(name, value))
		{
			return true;
		}
		if (name == objToken[TOKEN_SM_VISIBLE])
		{
			m_bVisible = StringUtil::ParseBool(value);
			return true;
		}
		if (name == objToken[TOKEN_HZ_ALPHA])
		{
			m_Alpha = StringUtil::ParseFloat(value);
			if (m_IOprepared)
			{
				m_modelEntity->setNaturalColor(Color(0, 0, 0, m_Alpha));
			}
			return true;
		}
		return false;
	}

	LORD::ui32 HazeObejct::editor_getSubMeshNum() const
	{
		assert(m_pMesh);
		return m_pMesh->getSubMeshNum();
	}

	void HazeObejct::setAlpha(float a)
	{
		m_Alpha = a;
		if (m_IOprepared)
		{
			m_modelEntity->setNaturalColor(Color(0, 0, 0, m_Alpha));
		}
	}

}
