#include "Core.h"
#include "TerrainMeshObject.h"
#include "TerrainEntity.h"
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
	TerrainMeshObject::TerrainMeshObject()
		: GameObject()
	{
		m_Type = GOT_TERRAIN;
		editor_canMirror = true;
		editor_canRotate = true;
		editor_canScale = true;

		m_pMesh = NULL;
		m_pLayerTexture = LordAlloc(Texture*, 4);
		memset(m_pLayerTexture, 0, sizeof(Texture*)*4);

		m_lightmapSize.x = 128.0f;
		m_lightmapSize.y = 128.0f;

		m_InversMat = Matrix4::IDENTITY;

		ShaderProgram* shaderProgram = ShaderProgramManager::Instance()->getShaderProgram("TerrainMesh");
		Renderer* pRenderer = Renderer::Instance();
		{
			SamplerState::SamplerDesc desc = m_Material->getSamplerState(0)->getDesc();
			desc.minFilter = SamplerState::FO_LINEAR;
			desc.magFilter = SamplerState::FO_LINEAR;
			desc.mipFilter = SamplerState::FO_NONE;
			desc.addrUMode = SamplerState::AM_WRAP;
			desc.addrVMode = SamplerState::AM_WRAP;

			m_Material->setSamplerState(0, desc);
			m_Material->setSamplerState(1, desc);
			m_Material->setSamplerState(2, desc);
			m_Material->setSamplerState(3, desc);
			m_Material->setSamplerState(4, desc);
		}

		m_Material->setShaderProgram(shaderProgram);
	}

	TerrainMeshObject::~TerrainMeshObject()
	{
		_unprepare();
		LordSafeFree(m_pLayerTexture);
	}

	bool TerrainMeshObject::exportData( rapidxml::xml_node<>* parentNode, rapidxml::xml_document<>& doc )
	{
		GameObject::exportData(parentNode, doc);
		if (m_pLayerTexture[0])
		{
			rapidxml::xml_node<>* LayerNode1 = doc.allocate_node(rapidxml::node_element, "Layer1", NULL);
			char* layername1 = doc.allocate_string(m_pLayerTexture[0]->getName().c_str());
			LayerNode1->append_attribute(doc.allocate_attribute("value", layername1));
			parentNode->append_node(LayerNode1);
		}
		if (m_pLayerTexture[1])
		{
			rapidxml::xml_node<>* LayerNode2 = doc.allocate_node(rapidxml::node_element, "Layer2", NULL);
			char* layername2 = doc.allocate_string(m_pLayerTexture[1]->getName().c_str());
			LayerNode2->append_attribute(doc.allocate_attribute("value", layername2));
			parentNode->append_node(LayerNode2);
		}
		if (m_pLayerTexture[2])
		{
			rapidxml::xml_node<>* LayerNode3 = doc.allocate_node(rapidxml::node_element, "Layer3", NULL);
			char* layername3 = doc.allocate_string(m_pLayerTexture[2]->getName().c_str());
			LayerNode3->append_attribute(doc.allocate_attribute("value", layername3));
			parentNode->append_node(LayerNode3);
		}
		if (m_pLayerTexture[3])
		{
			rapidxml::xml_node<>* LayerNode4 = doc.allocate_node(rapidxml::node_element, "Layer4", NULL);
			char* layername4 = doc.allocate_string(m_pLayerTexture[3]->getName().c_str());
			LayerNode4->append_attribute(doc.allocate_attribute("value", layername4));
			parentNode->append_node(LayerNode4);
		}

		return true;
	}

	bool TerrainMeshObject::importData( rapidxml::xml_node<>* parentNode )
	{
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
			else if (String(pPropertyNode->name()) == "Layer1")
			{
				rapidxml::xml_attribute<> *pPropertyName = pPropertyNode->first_attribute();
				m_pLayerTexture[0] = TextureManager::Instance()->createTexture(pPropertyName->value());
				m_pLayerTexture[0]->load();
			}
			else if (String(pPropertyNode->name()) == "Layer2")
			{
				rapidxml::xml_attribute<> *pPropertyName = pPropertyNode->first_attribute();
				m_pLayerTexture[1] = TextureManager::Instance()->createTexture(pPropertyName->value());
				m_pLayerTexture[1]->load();				
			}
			else if (String(pPropertyNode->name()) == "Layer3")
			{
				rapidxml::xml_attribute<> *pPropertyName = pPropertyNode->first_attribute();
				m_pLayerTexture[2] = TextureManager::Instance()->createTexture(pPropertyName->value());
				m_pLayerTexture[2]->load();			
			}
			else if (String(pPropertyNode->name()) == "Layer4")
			{
				rapidxml::xml_attribute<> *pPropertyName = pPropertyNode->first_attribute();
				m_pLayerTexture[3] = TextureManager::Instance()->createTexture(pPropertyName->value());
				m_pLayerTexture[3]->load();			
			}
			pPropertyNode = pPropertyNode->next_sibling();
		}
		
		return true;
	}

	void TerrainMeshObject::_exportData( DataStream& datastream )
	{
		size_t pixelSize = PixelUtil::GetPixelSize(PF_RGBA8_UNORM);
		size_t vertexBias = PixelUtil::GetPixelSize(PF_RGB32_FLOAT) + PixelUtil::GetPixelSize(PF_RGB32_FLOAT);
		for(size_t i=0; i<mEntityArray.size(); ++i)
		{
			TerrainEntity* pEnt = mEntityArray[i];
			ui32 bufSize = (ui32)(sizeof(ui8)*pEnt->getVertexCount()*pixelSize);
			ui8* BlendData = static_cast<ui8*>(LordMalloc(bufSize));
			ui8* pData = BlendData;
			ui8* pVertex = (ui8*)pEnt->getVertices();
			for (ui16 j=0; j<pEnt->getVertexCount(); ++j)
			{
				ui8* p = (ui8*)(pVertex+j*pEnt->getVertexStride()+vertexBias);
				memcpy(pData, p, pixelSize);
				pData += pixelSize;
			}
			datastream.write(&bufSize, sizeof(ui32));
			datastream.write(BlendData, bufSize);
			LordFree(BlendData);
		}
	}

	void TerrainMeshObject::_importData( DataStream& datastream )
	{
		size_t pixelSize = PixelUtil::GetPixelSize(PF_RGBA8_UNORM);
		size_t vertexBias = PixelUtil::GetPixelSize(PF_RGB32_FLOAT) + PixelUtil::GetPixelSize(PF_RGB32_FLOAT);
		for(size_t i=0; i<mEntityArray.size(); ++i)
		{
			TerrainEntity* pEnt = mEntityArray[i];
			const float* vertexPtr = (float*)pEnt->getVertices();
			ui32 bufSize = 0;
			datastream.read(&bufSize, sizeof(ui32));
			ui8* BlendData = static_cast<ui8*>(LordMalloc(bufSize));
			datastream.read(BlendData, bufSize);
			ui8* pData = BlendData;
			ui8* pVertex = (ui8*)pEnt->getVertices();
			for (ui16 j=0; j<pEnt->getVertexCount(); ++j)
			{
				ui8* p = (ui8*)(pVertex+j*pEnt->getVertexStride()+vertexBias);
				memcpy(p, pData, pixelSize);
				pData+=pixelSize;
			}
			LordFree(BlendData);
			pEnt->createRenderable();
		}
	}

	void TerrainMeshObject::_prepare_io()
	{
		m_pMesh->prepare();
	}

	void TerrainMeshObject::_prepare_res()
	{
		m_pMesh = MeshManager::Instance()->createMesh(m_Resource, true);
	}

	void TerrainMeshObject::_prepare_obj()
	{
		Scene* scene = SceneManager::Instance()->getCurrentScene();
		LordAssert(scene);
		String sceneName = scene->getSceneName();
		
		// ----update scene node
		m_ParentNode = SceneManager::Instance()->getRootNode();
		m_SceneNode = m_ParentNode->createChild();
		m_SceneNode->setWorldPosition(m_Position);
		m_SceneNode->setWorldOrientation(m_Oritation);
		m_SceneNode->setLocalScaling(m_Scale);
		m_SceneNode->update();

		m_LocalBox.reset();
		LightmapMgr* pLmMgr = LightmapMgr::Instance();
		m_pMesh->load();
		int subnum = m_pMesh->getSubMeshNum();
		for (int j = 0; j<subnum; ++j)
		{
			SubMesh* pSub = m_pMesh->getSubMesh(j);
			TerrainEntity* pEntity = LordNew TerrainEntity;
			SceneNode* pNode = m_SceneNode->createChild();
			pEntity->attachTo(pNode);
			pEntity->setMaterial(m_Material);
			pEntity->importSubMesh(pSub);
			mEntityArray.push_back(pEntity);	
		}
		m_LocalBox = m_pMesh->getBox();

		MeshManager::Instance()->releaseResource(m_pMesh);
		m_pMesh = NULL;

		for (ui32 i = 0; i<(ui32)mEntityArray.size(); ++i)
		{
			mEntityArray[i]->setTexture(1, m_pLayerTexture[0]);
			mEntityArray[i]->setTexture(2, m_pLayerTexture[1]);
			mEntityArray[i]->setTexture(3, m_pLayerTexture[2]);
			//mEntityArray[i]->setTexture(4, m_pLayerTexture[3]);
			mEntityArray[i]->setDarkRadio(1.0f);
		}
		for (ui32 i = 0; i<(ui32)mEntityArray.size(); ++i)
		{ 
			String uniquename = sceneName + m_Name + "_" + StringUtil::ToString(i);
			const Texture* pTex = NULL;
			Vector4 param( 0.f, 0.f, 1.f, 1.f);
			Vector3 scale( 1.f, 1.f, 1.f);
            pLmMgr->query((uniquename).c_str(), pTex, param, scale);
			
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

		updateWorldAABB();

		scene->registerGameObjectToZoom(this);
			
		String TerrainFile = sceneName + "_" + m_Name + ".TerrainMesh";
		DataStream* pTerrainStream = ResourceGroupManager::Instance()->openResource(TerrainFile);
		if (pTerrainStream)
		{
			_importData(*pTerrainStream);
			LordDelete pTerrainStream;
		}
		else
		{
			LordLogInfo("can't load the TerrianMap file: %s", TerrainFile.c_str());
		}

		m_IOprepared = true;
	}
	
	void TerrainMeshObject::unloadLMTexture()
	{
		for (size_t i=0; i<mEntityArray.size(); ++i)
		{ 
			Texture* pOldTex = mEntityArray[i]->getMaterial()->getTexture(0);
			if (pOldTex)
			{
				TextureManager::Instance()->releaseResource(pOldTex);
			}

			mEntityArray[i]->setTexture(0, NULL);
		}
	}

	void TerrainMeshObject::refleshLMTexture()
	{
		Scene* scene = SceneManager::Instance()->getCurrentScene();
		LordAssert(scene);
		String sceneName = scene->getSceneName();
		LightmapMgr* pLmMgr = LightmapMgr::Instance();
		for (ui32 i=0; i<mEntityArray.size(); ++i)
		{ 
			String uniquename = sceneName + m_Name + "_" + StringUtil::ToString(i);
			const Texture* pTex = NULL;
			Vector4 param( 0.f, 0.f, 1.f, 1.f);
			Vector3 scale( 1.f, 1.f, 1.f);
			pLmMgr->query((uniquename).c_str(), pTex, param, scale);

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

	void TerrainMeshObject::_prepare_editor()
	{
		if (Root::Instance()->isEditorMode())
		{
			Scene* pScene = SceneManager::Instance()->getCurrentScene();
			QueryObjectManager* pManager = pScene->getQueryObjectManager();

			m_pHoverBoxQueryObject = pManager->createQueryObject(m_Name + "hover_box");
			m_pHoverBoxQueryObject->setQueryFlag(0);
			m_pHoverBoxQueryObject->attachTo(m_SceneNode);

			m_pHoverBoxQueryObject->createAABBFrame(m_LocalBox, Color::BLUE);
		}
	}

	void TerrainMeshObject::_unprepare()
	{
		if (m_IOprepared)
		{
			Scene* pScene = SceneManager::Instance()->getCurrentScene();
			pScene->unregisterGameObjectFromZoom(this);
			pScene->registerGameObjectToZoom_unprepare(this);

			if (Root::Instance()->isEditorMode())
			{
				QueryObjectManager* pManager = SceneManager::Instance()->getCurrentScene()->getQueryObjectManager();

				pManager->destroyQueryObject(m_pHoverBoxQueryObject);
				m_pHoverBoxQueryObject = NULL;
			}

			unloadLMTexture();

			for (size_t i=0; i<mEntityArray.size(); ++i)
			{
				LordSafeDelete(mEntityArray[i]);
			}
			
			for (int i=0; i<4; ++i)
			{
				TextureManager::Instance()->releaseResource(m_pLayerTexture[i]);
				m_pLayerTexture[i] = NULL;
			}

			m_ParentNode->destroyChild(m_SceneNode);
			m_SceneNode = NULL;

			m_IOprepared = false;
		}
	}

	void TerrainMeshObject::ChangeBaseTexture( const LORD::String& name )
	{
		for (size_t i=1; i<4; ++i)
		{
			if (m_pLayerTexture[i] == m_pLayerTexture[0])
			{
				m_pLayerTexture[i] = NULL;
			}
		}
		if (m_pLayerTexture[0])
		{
			TextureManager::Instance()->releaseResource(m_pLayerTexture[0]);
		}
		m_pLayerTexture[0] = TextureManager::Instance()->createTexture(name);
		m_pLayerTexture[0]->load();

		for (ui32 i=0; i<(ui32)mEntityArray.size(); ++i)
		{
			mEntityArray[i]->setTexture(1, m_pLayerTexture[0]);
			for (ui32 j=1; j<4; ++j)
			{
				if (m_pLayerTexture[j] == NULL)
				{
					m_pLayerTexture[j] = m_pLayerTexture[0];
					mEntityArray[i]->setTexture(j+1, m_pLayerTexture[0]);
				}
			}
		}		
	}

	void TerrainMeshObject::setLayerTexture( uint id, const String& name )
	{
		if (m_pLayerTexture[id] && m_pLayerTexture[id] != m_pLayerTexture[0])
		{
			TextureManager::Instance()->releaseResource(m_pLayerTexture[id]);
		}
		m_pLayerTexture[id] = TextureManager::Instance()->createTexture(name);
		m_pLayerTexture[id]->load();
		for (size_t i=0; i<mEntityArray.size(); ++i)
		{
			mEntityArray[i]->setTexture(id+1, m_pLayerTexture[id]);
		}
	}

	Texture* TerrainMeshObject::getLayerTexture( uint id ) const
	{
		return m_pLayerTexture[id];
	}

	void TerrainMeshObject::deleteLayer( uint id )
	{
		LordAssert(id<5);
		if (m_pLayerTexture[id] == m_pLayerTexture[0])
		{
			return;
		}

		TextureManager::Instance()->releaseResource(m_pLayerTexture[id]);
		m_pLayerTexture[id] = m_pLayerTexture[0];
		for (size_t i=0; i<mEntityArray.size(); ++i)
		{
			mEntityArray[i]->setTexture(id+1, m_pLayerTexture[0]);
		}
	}

	std::pair<bool, Real> TerrainMeshObject::rayIntersects(const Ray& ray)
	{
		Ray rayObject = ray;
		rayObject.transform(m_InversMat);

		std::pair<bool, Real> result;
		vector<PickInfo>::type PickResults;
		result.first = false;
		result.second = 10000.0f;
		Real		tMin;	

		for (size_t entid=0; entid<mEntityArray.size(); ++entid)
		{
			TerrainEntity* pEnt = mEntityArray[entid];

			const float* vertexPtr = (float*)pEnt->getVertices();
			const ui16*	indexPtr = (ui16*)pEnt->getIndices();
			size_t vertexSize = pEnt->getVertexStride()/sizeof(float);
			ui16 indexNum = pEnt->getIndexCount();

			for(ui16 i=0; i<indexNum; i += 3)
			{
				bool h = rayObject.hitTri(Triangle(*(Vector3*)(vertexPtr+indexPtr[i]*vertexSize),
					*(Vector3*)(vertexPtr+indexPtr[i+1]*vertexSize),
					*(Vector3*)(vertexPtr+indexPtr[i+2]*vertexSize)),
					tMin);

				if(!h)
					continue;

				PickInfo pickInfo;
				pickInfo.info.first = h;
				pickInfo.info.second = tMin;
				PickResults.push_back(pickInfo);
			}
		}

		std::sort(PickResults.begin(), PickResults.end());

		vector<PickInfo>::type::iterator iter = PickResults.begin();
		if (iter!=PickResults.end())
			result = iter->info;

		return result;
	}

	Vector3 TerrainMeshObject::rayTerrain(const Ray& ray, const LayerBrushInfo& brushInfo, bool bBrush )
	{
		LordAssert( brushInfo.layerIndex>=-1 && brushInfo.layerIndex<=2);

		Vector3 hitPos = Vector3::INVALID;
		Real tMin;
		Ray rayObject = ray;
		rayObject.transform(m_InversMat);

		bool bhit = false;
		for(size_t i=0; i<mEntityArray.size(); ++i)
		{
			TerrainEntity* pEnt = mEntityArray[i];

			size_t vertexSize = pEnt->getVertexStride()/sizeof(float);
			const float* vertexPtr = (float*)pEnt->getVertices();
			const ui16*	indexPtr = (ui16*)pEnt->getIndices();
			ui16 indexNum = pEnt->getIndexCount();
			
			map<ui32, Vector3>::type posMap;

			for(ui16 j=0; j<indexNum; j+=3)
			{
				if (rayObject.hitTri(Triangle(*(Vector3*)(vertexPtr+indexPtr[j]*vertexSize),
					*(Vector3*)(vertexPtr+indexPtr[j+1]*vertexSize),
					*(Vector3*)(vertexPtr+indexPtr[j+2]*vertexSize)),
					tMin))
				{
					hitPos = rayObject.getPoint(tMin);
					bhit = true;
					break;
				}			
			}
			if (bhit)
				break;
		}

		if (bhit && bBrush)
		{
			float rSqr = brushInfo.radius*brushInfo.radius;
			ui32 vertexBias = PixelUtil::GetPixelSize(PF_RGB32_FLOAT) + PixelUtil::GetPixelSize(PF_RGB32_FLOAT);
			float PixelStrenth = brushInfo.strength*255;
			int vertexStride = mEntityArray[0]->getVertexStride();
			for(size_t i=0; i<mEntityArray.size(); ++i)
			{
				TerrainEntity* pEnt = mEntityArray[i];
				Byte* pEntVertexBuf = (Byte*)pEnt->getVertices();
				for (ui16 j=0; j<pEnt->getVertexCount(); ++j)
				{
					Vector3 pos = *(Vector3*)(pEntVertexBuf+j*vertexStride);
					float lenSqrt = (hitPos - pos).lenSqr();
					if (lenSqrt <= rSqr)
					{
						ui8 blendines = ui8((1.0f - Math::Sqrt(lenSqrt)/brushInfo.radius)*PixelStrenth);
						ui8* p = (ui8*)(pEntVertexBuf+j*vertexStride+vertexBias);

						// compute temporary alpha
						int alpha = Math::Clamp(255 - p[0] - p[1] - p[2], 0, 255);

						if (brushInfo.eraser)
						{
							if( brushInfo.layerIndex==-1)
								alpha = (ui8)(std::max<i16>(alpha - blendines, 0));
							else
								p[brushInfo.layerIndex] = (ui8)(std::max<i16>(p[brushInfo.layerIndex] - blendines, 0));
						}
						else
						{
							if( brushInfo.layerIndex==-1)
								alpha = (ui8)(std::min<ui16>( alpha + blendines, 255));
							else
								p[brushInfo.layerIndex] = (ui8)(std::min<ui16>(p[brushInfo.layerIndex] + blendines, 255));
						}

						// normalize
						float totalValue = static_cast<float>( alpha + p[0] + p[1] + p[2]) + FLT_EPSILON;
						float invValue   = 255.f / totalValue;
						for ( int weightIdx=0; weightIdx<3; weightIdx++)
						{
							p[weightIdx] = static_cast<ui8>( static_cast<float>(p[weightIdx]) * invValue);
						}
					}
				}				
				pEnt->createRenderable();
			}
		}
		return hitPos;
	}

	void TerrainMeshObject::_render()
	{
		LordAssert(mEntityArray.size()>0);
		if (m_bVisible)
		{
			const Frustum& pFrustum = SceneManager::Instance()->getMainCamera()->getFrustum();
			for (size_t i = 0; i < mEntityArray.size(); ++i)
			{
				TerrainEntity* pEntity = mEntityArray[i];
				if (pFrustum.intersect(pEntity->getWorldAABB()))
				{
					pEntity->update();
					m_RenderQueue->addRenderable(mEntityArray[i]);
				}
			}
		}

		if (Root::Instance()->isEditorMode())
		{
			m_pHoverBoxQueryObject->setVisible(getHovered());
		}
	}

	const Box& TerrainMeshObject::getWorldAABB() const
	{
		LordAssert(mEntityArray.size());
		return m_worldAABB;
	}

	void TerrainMeshObject::updateWorldAABB()
	{
		LordAssert(mEntityArray.size());
		const Matrix4& mat = m_SceneNode->getWorldMatrix();
		m_InversMat = mat;
		m_InversMat.detInverse();
		m_worldAABB = m_LocalBox.transform(mat);
		for(size_t i=0; i<mEntityArray.size(); ++i)
		{
			mEntityArray[i]->updateWorldAABB(mat);
		}

		if (Root::Instance()->isEditorMode())
		{
			if (m_pHoverBoxQueryObject)
			{
				m_pHoverBoxQueryObject->createAABBFrame(m_LocalBox, Color::BLUE);
			}
		}
	}

	Vector3 TerrainMeshObject::editor_modifyTerrainLayer( const Vector2& pos, const LayerBrushInfo& brushInfo, bool bBrush )
	{
		Ray ray;
		Vector2 uv;
		SceneManager::Instance()->getMainCamera()->getCameraRay(ray, pos);
		if (ray.hitBox(m_worldAABB))
		{
			return rayTerrain(ray, brushInfo, bBrush);
		}
		else
		{
			return Vector3::ZERO;
		}
	}

	void TerrainMeshObject::setLMTexture(uint subid, Texture* pTex )
	{
		Texture* pOldTex = mEntityArray[subid]->getMaterial()->getTexture(0);
		if (pOldTex)
		{
			TextureManager::Instance()->releaseResource(pOldTex);
		}

		pTex->addRefrenceCount();
		mEntityArray[subid]->setTexture(0, pTex);
	}

	ui32 TerrainMeshObject::getEntityNum() const
	{
		return (ui32)mEntityArray.size();
	}

	void TerrainMeshObject::getVertexList( size_t subId, vector<LightmapRasterizer::VertexInput>::type& lst )
	{
		lst.clear();
		TerrainEntity* pEnt = mEntityArray[subId];
		if (pEnt)
		{
			ui32 vertexbias = PixelUtil::GetPixelSize(PF_RGB32_FLOAT);
			ui32 uvbias = vertexbias * 2 + PixelUtil::GetPixelSize(PF_RGBA8_UNORM);
			ui32 uniquebias = uvbias + PixelUtil::GetPixelSize(PF_RG32_FLOAT);
			Byte* pVertex = (Byte*)pEnt->getVertices();
			const Matrix4& matWorld = m_SceneNode->getWorldMatrix();
			ui32 vertexStride = pEnt->getVertexStride();
			for (size_t i = 0; i < pEnt->getVertexCount(); ++i)
			{
				LightmapRasterizer::VertexInput input;
				input.m_position = *(Vector3*)(pVertex + i*vertexStride) * matWorld;
				input.m_normal = Vector4(*(Vector3*)(pVertex + i*vertexStride + vertexbias), 0.0f) * matWorld;
				input.m_uv = *(Vector3*)(pVertex + i*vertexStride + uvbias);
				input.m_uniqueUV = *(Vector3*)(pVertex + i*vertexStride + uniquebias);
				lst.push_back(input);
			}
		}
	}

	void TerrainMeshObject::getIndexList( size_t subId, vector<Word>::type& lst )
	{
		lst.clear();
		TerrainEntity* pEnt = mEntityArray[subId];
		Word* pIndex = (Word*)pEnt->getIndices();
		for (ui16 i=0; i<pEnt->getIndexCount(); ++i)
		{
			lst.push_back(*(pIndex+i));
		}
	}

	void TerrainMeshObject::setLightMapParam( size_t subId, const Vector4& param, const Vector3& scale )
	{
		mEntityArray[subId]->setLMParam(param, scale);
	}

	TerrainEntity* TerrainMeshObject::getEntity(ui32 entid) const
	{
		return mEntityArray[entid];
	}

	const Matrix4& TerrainMeshObject::getWorldMatrix() const
	{
		return m_SceneNode->getWorldMatrix();
	}

	void TerrainMeshObject::getPropertyList( PropertyList & lst )
	{
		GameObject::getPropertyList(lst);
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_SM_LMSIZE], true));
	}

	bool TerrainMeshObject::getPropertyType( const String & name, PropertyType &type )
	{
		if (GameObject::getPropertyType(name, type))
		{
			return true;
		}
		if (name == objToken[TOKEN_SM_LMSIZE])
		{
			type = PT_VECTOR2; return true;
		}
		return false;
	}

	bool TerrainMeshObject::getPropertyValue( const String & name, String &value )
	{
		if (GameObject::getPropertyValue(name, value))
		{
			return true;
		}
		if (name == objToken[TOKEN_SM_LMSIZE])
		{
			value = StringUtil::ToString(m_lightmapSize); return true;
		}
		return false;
	}

	bool TerrainMeshObject::setPropertyValue( const String & name,const String & value )
	{
		if (GameObject::setPropertyValue(name, value))
		{
			return true;
		}
		if (name == objToken[TOKEN_SM_LMSIZE])
		{
			m_lightmapSize = StringUtil::ParseVec2(value);
			return true;
		}
		return false;	
	}

	void TerrainMeshObject::getLightMapSize( Vector2& lmsize )
	{
		lmsize = m_lightmapSize;
	}

	void TerrainMeshObject::changeDark(float radio)
	{
		if (m_IOprepared)
		{
			for (size_t i = 0; i < mEntityArray.size(); ++i)
			{
				mEntityArray[i]->setDarkRadio(radio);
			}
		}
	}

	void TerrainMeshObject::revertLight()
	{
		for (size_t i = 0; i < mEntityArray.size(); ++i)
		{
			mEntityArray[i]->setDarkRadio(1.0f);
		}
	}

}
