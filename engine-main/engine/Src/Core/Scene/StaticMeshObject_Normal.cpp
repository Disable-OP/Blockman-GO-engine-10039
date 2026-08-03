#include "Core.h"
#include "StaticMeshObject_Normal.h"
#include "ModelEntity.h"
#include "Model/MeshManager.h"
#include "SceneManager.h"
#include "Object/Root.h"
#include "Util/QueryObject.h"
#include "Scene.h"
#include "Model/Mesh.h"
#include "SceneNode.h"
#include "LightmapMgr.h"
#include "Render/ShaderProgramManager.h"

namespace LORD
{
	static const String s_DefaultLMName = "LMDefault.tga";
	static const Vector2 s_DefaultLMSize = Vector2(32.0f, 32.0f);

	StaticMeshObject_Normal::StaticMeshObject_Normal()
		: GameObject()
		, m_modelEntity(NULL)
		, m_pMesh(NULL)
		, m_UVScale(Vector2::ZERO)
		, editor_selSubId(-1)
		, m_bAlphaTest(false)
		, m_bGray(false)
	{
		m_Type = GOT_SMESH_L;
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

		desc.addrUMode = SamplerState::AM_BORDER;
		desc.addrVMode = SamplerState::AM_BORDER;
		m_Material->setSamplerState(1, desc);

		m_ShaderDefine = "StaticMeshNormalForSceneNoAlpha";
	}

	StaticMeshObject_Normal::~StaticMeshObject_Normal()
	{
		_unprepare();
	}
	
	bool StaticMeshObject_Normal::exportData( rapidxml::xml_node<>* parentNode, rapidxml::xml_document<>& doc )
	{
		editor_selSubId = -1;
		GameObject::exportData(parentNode, doc);
		assert(m_IOprepared);
		int subNum = m_pMesh->getSubMeshNum();
		char* strSubNum = doc.allocate_string(StringUtil::ToString(subNum).c_str());
		rapidxml::xml_node<>* SubNumNode = doc.allocate_node(rapidxml::node_element,"SubNum",NULL);
		SubNumNode->append_attribute(doc.allocate_attribute("value", strSubNum));
		parentNode->append_node(SubNumNode);
		for (int i=0; i<subNum; ++i)
		{
			char* strSubId = doc.allocate_string(StringUtil::ToString(i).c_str());
			char* strlmSize = doc.allocate_string(StringUtil::ToString(m_lightmapSize[i]).c_str());
			rapidxml::xml_node<>* LMSizeNode = doc.allocate_node(rapidxml::node_element,"LMSize",NULL);
			LMSizeNode->append_attribute(doc.allocate_attribute("id", strSubId));
			LMSizeNode->append_attribute(doc.allocate_attribute("lmsize", strlmSize));
			parentNode->append_node(LMSizeNode);
		}

		return true;
	}

	bool StaticMeshObject_Normal::importData( rapidxml::xml_node<>* parentNode )
	{
		// parse property nodes
		rapidxml::xml_node<> *pPropertyNode = parentNode->first_node();
		while(pPropertyNode)
		{
			if (String(pPropertyNode->name()) == "Property")
			{
				rapidxml::xml_attribute<> *pPropertyName = pPropertyNode->first_attribute();
				String propertyName = pPropertyName->value();
				rapidxml::xml_attribute<> *pPropertyValue = pPropertyName->next_attribute();
				String propertyValue = pPropertyValue->value();

				setPropertyValue(propertyName, propertyValue);
			}
			else if (String(pPropertyNode->name()) == "SubNum")
			{
				rapidxml::xml_attribute<> *pPropertyName = pPropertyNode->first_attribute();
				int subNum = StringUtil::ParseI32(pPropertyName->value());
				m_lightmapSize.reserve(subNum);
				m_lightmapSize.resize(subNum, s_DefaultLMSize);
			}
			else if (String(pPropertyNode->name()) == "LMSize")
			{
				rapidxml::xml_attribute<> *pPropertyName = pPropertyNode->first_attribute();
				int subId = StringUtil::ParseI32(pPropertyName->value());
				rapidxml::xml_attribute<> *pPropertyValue = pPropertyName->next_attribute();
				Vector2 lmsize = StringUtil::ParseVec2(pPropertyValue->value());
				m_lightmapSize[subId] = lmsize;
			}
			pPropertyNode = pPropertyNode->next_sibling();
		}

		return true;
	}

	void StaticMeshObject_Normal::_prepare_io()
	{
		m_pMesh->prepare();
	}

	void StaticMeshObject_Normal::_prepare_res()
	{
		ShaderProgram* shaderProgram = ShaderProgramManager::Instance()->getShaderProgram(m_ShaderDefine);
		m_Material->setShaderProgram(shaderProgram);

		m_pMesh = MeshManager::Instance()->createMesh(m_Resource, true);
	}

	void StaticMeshObject_Normal::_prepare_obj()
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
		m_modelEntity->attachTo(m_SceneNode);
		m_modelEntity->setRenderType(ModelEntity::RT_FOR_SCENE_L);
		m_modelEntity->EnableGrayRender(m_bGray ? 1.0f : 0.0f);
		m_modelEntity->createRenderable();
		m_modelEntity->EnableAlphaTest(m_bAlphaTest);
		// 加载模型贴图
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
		LightmapMgr* pLmMgr = LightmapMgr::Instance();
		int subNum = m_pMesh->getSubMeshNum();
		for (int i=0; i<subNum; ++i)
		{
			SubMesh* pSub = m_pMesh->getSubMesh(i);
			const Texture* pTex = NULL;
			Vector4 param( 0.f, 0.f, 1.f, 1.f);
			Vector3 scale( 1.f, 1.f, 1.f);
			pLmMgr->query((sceneName + m_Name + "_" + StringUtil::ToString(i)).c_str(), pTex, param, scale);
			if (pTex)
			{
				setLMTexture(i, (Texture*)pTex);
				setLightMapParam(i, param, scale);
			}
			else
			{
				Texture* pDefault = TextureManager::Instance()->createTexture(s_DefaultLMName);
				pDefault->load();
				setLMTexture(i, pDefault);
				setLightMapParam(i, param, scale);
			}

			m_lightmapSize.push_back(Vector2(Real(pSub->getLightmapResolution()), Real(pSub->getLightmapResolution())));
		}
		if (m_lightmapSize.empty())
		{
			m_lightmapSize.reserve(subNum);
			m_lightmapSize.resize(subNum, s_DefaultLMSize);
		}

		m_IOprepared = true;
	}

	void StaticMeshObject_Normal::_prepare_editor()
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

	void StaticMeshObject_Normal::_unprepare()
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

			unloadLMTexture();

			LordSafeDelete(m_modelEntity);

			m_IOprepared = false;
		}

		if (m_pMesh)
		{
			MeshManager::Instance()->releaseResource(m_pMesh);
			m_pMesh = NULL;
		}
	}

	std::pair<bool, Real> StaticMeshObject_Normal::rayIntersects(const Ray& ray)
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

	void StaticMeshObject_Normal::_render()
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

	const Box& StaticMeshObject_Normal::getWorldAABB() const
	{
		LordAssert(m_modelEntity);
		return m_worldAABB;
	}

	void StaticMeshObject_Normal::updateWorldAABB()
	{
		LordAssert(m_modelEntity);
		m_worldAABB = m_modelEntity->getLocalAABB().transform(m_SceneNode->getWorldMatrix());
		m_Extent = m_worldAABB.getSize().len() * 0.5f;
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

	void StaticMeshObject_Normal::getVertexList(ui32 subId, vector<LightmapRasterizer::VertexInput>::type& lst )
	{
		// 清空
		lst.clear();

		uint vertexbias = PixelUtil::GetPixelSize(PF_RGB32_FLOAT);
		uint texcoordbias = PixelUtil::GetPixelSize(PF_RG32_FLOAT);
		const Matrix4& matWorld = m_SceneNode->getWorldMatrix();
		LightmapRasterizer::VertexInput Vinput;
		SubMesh* pSubMesh = m_pMesh->getSubMesh(subId);
		Byte* pVertex = pSubMesh->getVertices();
		uint vertexStride = pSubMesh->getVertexStride();
		for (uint j=0; j<pSubMesh->getVertexCount(); ++j)
		{
			Vector3 tPostion = (*(Vector3*)(pVertex+j*vertexStride));
			Vector3 tNormal  = (*(Vector3*)(pVertex+j*vertexStride+vertexbias));

			Vinput.m_position = tPostion * matWorld;
			Vinput.m_normal   = Vector4( tNormal, 0.f) * matWorld;
			Vinput.m_uv		  = *(Vector2*)(pVertex+j*vertexStride+vertexbias*2);
			Vinput.m_uniqueUV = *(Vector2*)(pVertex+j*vertexStride+vertexbias*2+texcoordbias);
			lst.push_back(Vinput);
		}
	}

	void StaticMeshObject_Normal::getIndexList(ui32 subId, vector<Word>::type& lst )
	{
		// 清空
		lst.clear();

		SubMesh* pSubMesh = m_pMesh->getSubMesh(subId);
		Word* pIndex = (Word*)pSubMesh->getIndices();
		for (uint j=0; j<pSubMesh->getIndexCount(); ++j)
		{
			lst.push_back(*(pIndex+j));
		}
	}

	const Matrix4& StaticMeshObject_Normal::getWorldMatrix() const
	{
		return m_SceneNode->getWorldMatrix();
	}

	void StaticMeshObject_Normal::setLMTexture( ui32 subId, Texture* pTexture )
	{
		assert(pTexture);
		m_modelEntity->setLMTexture(subId, pTexture);
	}

	void StaticMeshObject_Normal::setLightMapParam( ui32 subId, const Vector4& param, const Vector3& scale )
	{
		m_modelEntity->setLightMapUVBias(subId, param, scale);
	}

	void StaticMeshObject_Normal::unloadLMTexture()
	{
		m_modelEntity->unloadLMTexture();
	}

	void StaticMeshObject_Normal::refleshLMTexture()
	{
		Scene* scene = SceneManager::Instance()->getCurrentScene();
		LordAssert(scene);
		String sceneName = scene->getSceneName();
		LightmapMgr* pLmMgr = LightmapMgr::Instance();
		for (int i=0; i<m_pMesh->getSubMeshNum(); ++i)
		{
			const Texture* pTex = NULL;
			Vector4 param( 0.f, 0.f, 1.f, 1.f);
			Vector3 scale( 1.f, 1.f, 1.f);
			pLmMgr->query((sceneName + m_Name + "_" + StringUtil::ToString(i)).c_str(), pTex, param, scale);
			if (pTex)
			{
				setLMTexture(i, (Texture*)pTex);
				setLightMapParam(i, param, scale);
			}
			else
			{
				Texture* pDefault = TextureManager::Instance()->createTexture(s_DefaultLMName);
				pDefault->load();
				setLMTexture(i, pDefault);
				setLightMapParam(i, param, scale);
			}
		}		
	}
	
	void StaticMeshObject_Normal::setUVScale( float u, float v )
	{
		m_UVScale.x = u;
		m_UVScale.y = v;
	}

	void StaticMeshObject_Normal::frameMove( i32 deltaTime )
	{
		m_modelEntity->updateUVanimation(m_UVScale*(float)deltaTime*0.001f);
	}

	void StaticMeshObject_Normal::getPropertyList( PropertyList & lst )
	{
		GameObject::getPropertyList(lst);
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_SM_UVSCALE], true));
		if (editor_selSubId != -1) lst.push_back(std::pair<String, bool>(objToken[TOKEN_SM_LMSIZE], true));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_SM_VISIBLE], true));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_SM_ALPHATEST], true));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_SM_ISGRAY], true));
	}

	bool StaticMeshObject_Normal::getPropertyType( const String & name, PropertyType &type )
	{
		if (GameObject::getPropertyType(name, type))
		{
			return true;
		}
		if (name == objToken[TOKEN_SM_UVSCALE])
		{
			type = PT_VECTOR2; return true;
		}
		if (name == objToken[TOKEN_SM_LMSIZE])
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

	bool StaticMeshObject_Normal::getPropertyValue( const String & name, String &value )
	{
		if (GameObject::getPropertyValue(name, value))
		{
			return true;
		}
		if (name == objToken[TOKEN_SM_UVSCALE])
		{
			value = StringUtil::ToString(m_UVScale); return true;
		}
		if (name == objToken[TOKEN_SM_LMSIZE])
		{
			value = StringUtil::ToString(m_lightmapSize[editor_selSubId]); return true;
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

	bool StaticMeshObject_Normal::setPropertyValue( const String & name,const String & value )
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
		if (name == objToken[TOKEN_SM_LMSIZE])
		{
			m_lightmapSize[editor_selSubId] = StringUtil::ParseVec2(value);
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
				m_ShaderDefine = "StaticMeshNormalForScene";
			}
			else
			{
				m_ShaderDefine = "StaticMeshNormalForSceneNoAlpha";
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
			if (m_IOprepared)
			{
				m_modelEntity->EnableGrayRender(m_bGray ? 1.0f : 0.0f);
			}
			return true;
		}
		return false;	
	}

	LORD::ui32 StaticMeshObject_Normal::editor_getSubMeshNum() const
	{
		assert(m_pMesh);
		return m_pMesh->getSubMeshNum();
	}

	void StaticMeshObject_Normal::setSelSubMesh( i32 subId )
	{
		editor_selSubId = subId;
		if (editor_selSubId == -1)
		{
			m_modelEntity->resetEditorColor();
		}
		else
		{
			m_modelEntity->setEditorColor(subId);
		}
	}
	
	void StaticMeshObject_Normal::setLightMapSize( int subId, const Vector2& lmSize )
	{
		m_lightmapSize[subId] = lmSize;
	}

	void StaticMeshObject_Normal::getLightMapSize( int subId, Vector2& lmSize ) const
	{
		lmSize = m_lightmapSize[subId];
	}

	void StaticMeshObject_Normal::changeDark(float radio)
	{
		if (m_IOprepared)
		{
			m_modelEntity->setDarkRadio(radio);
		}		
	}

	void StaticMeshObject_Normal::revertLight()
	{
		if (m_IOprepared)
		{
			m_modelEntity->setDarkRadio(1.0f);
		}
	}

	void StaticMeshObject_Normal::EnableGrayRender(bool gray)
	{
		m_bGray = gray;
		if (m_IOprepared)
		{
			m_modelEntity->EnableGrayRender(gray ? 1.0f : 0.0f);
		}
	}

}
