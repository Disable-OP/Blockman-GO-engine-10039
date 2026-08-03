#include "OceanRender.h"
#include "RenderQueue.h"
#include "Material.h"
#include "Object/Root.h"
#include "Scene/SceneManager.h"
#include "Scene/Scene.h"
#include "Util/QueryObject.h"
#include "ShaderProgramManager.h"

namespace LORD
{
	OceanRender::OceanRender()
		: Renderable()
		, m_GridSize(1.0f)
		, m_CurTime(0.f)
		, m_WaveSpeed(0.25f)
		, m_WaterHeight(0.f)
		, m_PixelParam(400.0f, 1.0f)
		, m_ShoreDark(0.0f, 0.475f, 0.474f)
		, m_ShoreLight(0.25f, 1.0f, 1.0f)
		, m_SeaDark(0.0f, 0.0f, 0.384f)
		, m_SeaLight(0.118f, 0.235f, 0.235f)
		, m_LightPos(250.f, 250.f, 0.0f)
		, m_WaveParam(0.0039f, 0.0039f, 0.1f, 0.1f)
		, m_matVP(Matrix4::IDENTITY)
		, m_VertexBuffer(NULL)
		, m_IndexBuffer(NULL)	
		, m_GpuVertexBuffer(NULL)
		, m_GpuIndexBuffer(NULL)
		, m_NumGird( 240 )				
		, m_VertexCount(0)
		, m_IndexCount(0)
		, m_VertexSize(0)
		, m_isPrepared(false)
		, m_pNormalmap(NULL)
		, m_pFoamMap(NULL)
		, m_NormalmapName("water_normals.png")
		, m_FoamName("foam.png")
	{
	}

	OceanRender::~OceanRender()
	{
		_unprepare();
	}

	void OceanRender::_unprepare()
	{
		if (m_pFoamMap)
		{
			TextureManager::Instance()->releaseResource(m_pFoamMap);
		}
		if (m_pNormalmap)
		{
			TextureManager::Instance()->releaseResource(m_pNormalmap);
		}
		m_pFoamMap = NULL;
		m_pNormalmap = NULL;

		_destroyGeometry();

		LordSafeDelete(m_Material);

		m_isPrepared = false;
	}

	void OceanRender::_prepare_res()
	{
		assert(m_pNormalmap == NULL);
		assert(m_pFoamMap == NULL);
		m_pNormalmap = TextureManager::Instance()->createTexture(m_NormalmapName);
		m_pFoamMap = TextureManager::Instance()->createTexture(m_FoamName);
	}

	void OceanRender::_prepare_io()
	{
		//
		//Texture* normalMap_Render = m_pNormalmap;
		//Texture* formMap_Render = m_pFoamMap;

		if( m_FoamName != StringUtil::BLANK   )
		{
			if (!m_pFoamMap->prepare())
			{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
				char buf[MAX_PATH];
				sprintf_s(buf, "OceanRender::_prepare_io: Texture file isn't exits. [%s]", m_FoamName.c_str());
				MessageBox(0, buf, 0, 0);
#endif
				TextureManager::Instance()->releaseResource(m_pFoamMap);
				m_pFoamMap = NULL;
			}
		}
		if(m_NormalmapName != StringUtil::BLANK)
		{
			if (!m_pNormalmap->prepare())
			{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
				char buf[MAX_PATH];
				sprintf_s(buf, "OceanRender::_prepare_io: Texture file isn't exits. [%s]", m_NormalmapName.c_str());
				MessageBox(0, buf, 0, 0);
#endif
				TextureManager::Instance()->releaseResource(m_pNormalmap);
				m_pNormalmap = NULL;
			}
		}
	}

	void OceanRender::_prepare_obj()
	{
		if (m_pFoamMap)
		{
			if (!m_pFoamMap->load())
			{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
				char buf[MAX_PATH];
				sprintf_s(buf, "OceanRender::_prepare_obj: Texture file is load error. [%s]", m_FoamName.c_str());
				MessageBox(0, buf, 0, 0);
#endif
				TextureManager::Instance()->releaseResource(m_pFoamMap);
				m_pFoamMap = NULL;
				return;
			}
		}
		if (m_pNormalmap)
		{
			if (!m_pNormalmap->load())
			{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
				char buf[MAX_PATH];
				sprintf_s(buf, "OceanRender::_prepare_obj: Texture file is load error. [%s]", m_NormalmapName.c_str());
				MessageBox(0, buf, 0, 0);
#endif
				TextureManager::Instance()->releaseResource(m_pNormalmap);
				m_pNormalmap = NULL;
				return;
			}
		}
	
		ShaderProgram* shaderProgram = ShaderProgramManager::Instance()->getShaderProgram("Water");
		m_Material = LordNew Material();
		BlendState::BlendDesc blenddesc = m_Material->getBlendState()->getDesc();
		blenddesc.bBlendEnable = true;
		blenddesc.blendOP = BlendState::BOP_ADD;
		blenddesc.srcBlend = BlendState::BF_SRC_ALPHA;
		blenddesc.dstBlend = BlendState::BF_INV_SRC_ALPHA;
		blenddesc.alphaBlendOP = BlendState::BOP_ADD;
		blenddesc.srcAlphaBlend = BlendState::BF_ONE;
		blenddesc.dstAlphaBlend = BlendState::BF_ZERO;
		m_Material->setBlendState(blenddesc);

		DepthStencilState::DepthStencilDesc depthdesc = m_Material->getDepthState()->getDesc();
		depthdesc.bWriteDepth = false;
		m_Material->setDepthState(depthdesc);
		
		SamplerState::SamplerDesc samdesc = m_Material->getSamplerState(0)->getDesc();
		samdesc.mipFilter = SamplerState::FO_POINT;
		m_Material->setSamplerState(0, samdesc);
		m_Material->setSamplerState(1, samdesc);

		m_Material->setShaderProgram(shaderProgram);
		m_Material->beginShaderParams(12);
		m_Material->setShaderParam("normal0", SPT_TEXTURE, (void*)&TEXTURE_UNIT0);
		m_Material->setShaderParam("foam", SPT_TEXTURE, (void*)&TEXTURE_UNIT1);
		m_Material->setShaderParam("u_time", SPT_FLOAT, (void*)&(m_CurTime));
		m_Material->setShaderParam("u_param", SPT_VEC2, (void*)&(m_PixelParam));
		m_Material->setShaderParam("SHORE_DARK", SPT_VEC3, (void*)&(m_ShoreDark));
		m_Material->setShaderParam("SHORE_LIGHT", SPT_VEC3, (void*)&(m_ShoreLight));
		m_Material->setShaderParam("SEA_DARK", SPT_VEC3, (void*)&(m_SeaDark));
		m_Material->setShaderParam("SEA_LIGHT", SPT_VEC3, (void*)&(m_SeaLight));
		m_Material->setShaderParam("u_lightPos", SPT_VEC3, (void*)&(m_LightPos));
		m_Material->setShaderParam("u_lightDir", SPT_VEC3, (void*)SceneManager::Instance()->getMainLightDirPtr());
		m_Material->setShaderParam("u_WaveParam", SPT_VEC4, (void*)&(m_WaveParam));
		m_Material->setShaderParam("u_mvp", SPT_MAT4, (void*)&(m_matVP));
		m_Material->endShaderParams();

		m_Material->activeTexture(TEXTURE_UNIT0, m_pNormalmap);
		m_Material->activeTexture(TEXTURE_UNIT1, m_pFoamMap);

		_createGeometry(m_NumGird);

		m_isPrepared = true;

		Scene* scene = SceneManager::Instance()->getCurrentScene();
		LordAssert(scene);
		String OceanFile = scene->getSceneName() + ".ocean";
		DataStream* pTerrainStream = ResourceGroupManager::Instance()->openResource(OceanFile);
		if (pTerrainStream)
		{
			importDepthData(*pTerrainStream);
			LordDelete pTerrainStream;
		}
		else
		{
			LordLogInfo("can't load the ocean file: %s", OceanFile.c_str());
		}
	}

	void OceanRender::_destroyGeometry()
	{
		LordSafeFree(m_VertexBuffer);
		LordSafeFree(m_IndexBuffer);
		LordSafeDelete(m_GpuVertexBuffer);
		LordSafeDelete(m_GpuIndexBuffer);
		LordSafeDelete(m_RenderInput);
	}

	void OceanRender::_createGeometry( ui32 size)
	{
		if( size > 255 || size < 4 || m_GridSize < 0.1f)
			return;

		bool isEditorMode = Root::Instance()->isEditorMode();
		_destroyGeometry();

		uint vertUnitOffset = 0;
		RenderInput::VertexElementList vertElms;
		RenderInput::VertexElement poselemt, colorelemt;
		poselemt.semantic = RenderInput::VS_POSITION;
		poselemt.pixFmt = PF_RGB32_FLOAT;
		vertUnitOffset += PixelUtil::GetPixelSize(PF_RGB32_FLOAT);
		
		colorelemt.semantic = RenderInput::VS_COLOR;
		colorelemt.pixFmt = PF_RGBA32_FLOAT;
		vertUnitOffset += PixelUtil::GetPixelSize(PF_RGBA32_FLOAT);
		
		vertElms.push_back(poselemt);
		vertElms.push_back(colorelemt);

		m_VertexSize = vertUnitOffset;
		m_VertexCount = size * size;
		m_NumGird = size;			
		m_VertexBuffer = (float*)LordMalloc(m_VertexCount * m_VertexSize);

		m_IndexCount = (size - 1)*(size - 1) * 6;
		m_IndexBuffer = (ui16*)LordMalloc(m_IndexCount * sizeof(ui16));

		float* pVertex = m_VertexBuffer;
		float maxDepth = 10.0f;
		float BiasSize = size /  4.0f;
		for( int i =0; i<(int)size; i ++  )
		{
			for( int j=0; j<(int)size; j++ )
			{	
				pVertex[0] = (float)j*m_GridSize - (float)size*0.5f*m_GridSize;
				pVertex[1] = m_WaterHeight;
				pVertex[2] = (float)i*m_GridSize - (float)size*0.5f*m_GridSize;
				
				float rateXMin = Math::Max(Math::Min(((float)(i - 1)) / BiasSize, 1.0f), 0.0f);
				float rateXMax = Math::Max(Math::Min(((float)(size - i - 2)) / BiasSize, 1.0f), 0.0f);

				float rateYMin = Math::Max(Math::Min(((float)(j - 1)) / BiasSize, 1.0f), 0.0f);
				float rateYMax = 1.0f;

				//wave
				pVertex[3] = Math::Min(rateYMin, rateYMax);
				//wind
				pVertex[4] = Math::Min(rateXMin, rateXMax);

				float depth = maxDepth;
				if (isEditorMode)
				{
					//depth
					Ray ray(*(Vector3*)pVertex + Vector3::UNIT_Y* 1000.f, Vector3::NEG_UNIT_Y);
					Vector3 intersectPos; //
					if (SceneManager::Instance()->getCurrentScene()->getRayIntersectPoint(ray, intersectPos, NULL, GOT_SMESH | GOT_SMESH_L | GOT_TERRAIN))
					{
						depth = Math::Min(Math::Abs(m_WaterHeight - intersectPos.y), maxDepth);
					}
				}
				depth = Math::Min(depth / maxDepth, 1.0f);
				depth = 1.0f - depth;
				pVertex[5] =  depth;
				//if (depth > 0.95f)
				//{
					pVertex[6] = (float)i*m_WaveParam.x * depth * maxDepth;
					pVertex[6] *= pVertex[6];
				//}
				//else
				//{
				//	pVertex[6] = 0.0f;
				//}

				pVertex += m_VertexSize/sizeof(float);
			}
		}

		ui16* pIndex =m_IndexBuffer;
		for( ui16 j=0; j< size-1; j++ )
		{
			for (ui16 i = 0; i< size - 1; i++)
			{
				pIndex[0] = i + size * j;
				pIndex[1] = i + size * j + 1;
				pIndex[2] = i + size * (j+1)+1;

				pIndex[3] = i + size * j;
				pIndex[4] = i +  size * (j+1) + 1;
				pIndex[5] = i + size * (j+1);

				pIndex+=6;
			}
		}

		// flush data to Gpu buffer.
		Renderer* pRenderer = Renderer::Instance();
		Buffer vertBuff(m_VertexCount * m_VertexSize, m_VertexBuffer);
		m_GpuVertexBuffer = Renderer::Instance()->createVertexBuffer(GPUBuffer::GBU_CPU_READ, vertBuff);
		Buffer indBuff(m_IndexCount * sizeof(ui16), m_IndexBuffer);
		m_GpuIndexBuffer = Renderer::Instance()->createIndexBuffer(GPUBuffer::GBU_CPU_READ, indBuff);
	
		m_RenderInput = pRenderer->createRenderInput(vertElms, m_Material->getShaderProgram());
		m_RenderInput->bindGPUBuffers(m_GpuVertexBuffer, m_VertexSize, m_GpuIndexBuffer, sizeof(ui16));
	}

	void OceanRender::_updateRenderQueue(RenderQueue* pRenderQueue)
	{
		if (m_isPrepared)
		{
			m_LightPos = SceneManager::Instance()->getMainCamera()->getPosition();
			m_matVP = SceneManager::Instance()->getMainCamera()->getViewProjMatrix();
			m_CurTime += (float)Root::Instance()->getFrameTime() * 0.01f * m_WaveSpeed;

			pRenderQueue->addRenderable(this);
		}
	}
	
	void OceanRender::exportData( DataStream& dataStream)
	{
		dataStream.write(&m_NumGird, sizeof(int));
		dataStream.write(&m_GridSize, sizeof(float));
		dataStream.write(&m_WaveSpeed, sizeof(float));
		dataStream.write(&m_WaterHeight, sizeof(float));
		dataStream.write(&m_ShoreDark, sizeof(Vector3));
		dataStream.write(&m_ShoreLight, sizeof(Vector3));
		dataStream.write(&m_SeaDark, sizeof(Vector3));
		dataStream.write(&m_SeaLight, sizeof(Vector3));
		dataStream.write(&m_WaveParam, sizeof(Vector4));
	}

	void OceanRender::importData(DataStream& dataStream)
	{
		dataStream.read(&m_NumGird, sizeof(int));
		dataStream.read(&m_GridSize, sizeof(float));
		dataStream.read(&m_WaveSpeed, sizeof(float));
		dataStream.read(&m_WaterHeight, sizeof(float));
		dataStream.read(&m_ShoreDark, sizeof(Vector3));
		dataStream.read(&m_ShoreLight, sizeof(Vector3));
		dataStream.read(&m_SeaDark, sizeof(Vector3));
		dataStream.read(&m_SeaLight, sizeof(Vector3));
		dataStream.read(&m_WaveParam, sizeof(Vector4));
	}

	void OceanRender::exportData(rapidjson::Value& node, rapidjson::Document* doc)
	{

		String strNumGrid = StringUtil::ToString(m_NumGird);
		String strGridSize = StringUtil::ToString(m_GridSize);
		String strWaveSpeed = StringUtil::ToString(m_WaveSpeed);
		String strHeight = StringUtil::ToString(m_WaterHeight);
		String strShoreDark = StringUtil::ToString(m_ShoreDark);
		String strShoreLight = StringUtil::ToString(m_ShoreLight);
		String strSeaDark = StringUtil::ToString(m_SeaDark);
		String strSeaLight = StringUtil::ToString(m_SeaLight);
		String strWaveParam = StringUtil::ToString(m_WaveParam);
		String strFadeOutDist = StringUtil::ToString(m_PixelParam);

		rapidjson::Value valNumGrid(strNumGrid.c_str(), doc->GetAllocator());
		rapidjson::Value valGridSize(strGridSize.c_str(), doc->GetAllocator());
		rapidjson::Value valWaveSpeed(strWaveSpeed.c_str(), doc->GetAllocator());
		rapidjson::Value valHeight(strHeight.c_str(), doc->GetAllocator());
		rapidjson::Value valShoreDark(strShoreDark.c_str(), doc->GetAllocator());
		rapidjson::Value valShoreLight(strShoreLight.c_str(), doc->GetAllocator());
		rapidjson::Value valSeaDark(strSeaDark.c_str(), doc->GetAllocator());
		rapidjson::Value valSeaLight(strSeaLight.c_str(), doc->GetAllocator());
		rapidjson::Value valWaveParam(strWaveParam.c_str(), doc->GetAllocator());
		rapidjson::Value valFadeOutDist(strFadeOutDist.c_str(), doc->GetAllocator());

		node.AddMember("GridNum", valNumGrid, doc->GetAllocator());
		node.AddMember("GridSize", valNumGrid, doc->GetAllocator());
		node.AddMember("WaveSpeed", valWaveSpeed, doc->GetAllocator());
		node.AddMember("WaterHeight", valHeight, doc->GetAllocator());
		node.AddMember("ShoreDark", valShoreDark, doc->GetAllocator());
		node.AddMember("ShoreLight", valShoreLight, doc->GetAllocator());
		node.AddMember("SeaDark", valSeaDark, doc->GetAllocator());
		node.AddMember("SeaLight", valSeaDark, doc->GetAllocator());
		node.AddMember("WavaParam", valWaveParam, doc->GetAllocator());
		node.AddMember("FadeOutDistance", valFadeOutDist, doc->GetAllocator());

	}

	void OceanRender::importData(rapidjson::Value& node)
	{
		m_NumGird = StringUtil::ParseInt(node["GridNum"].GetString());
		m_GridSize = StringUtil::ParseFloat(node["GridSize"].GetString());
		m_WaveSpeed = StringUtil::ParseFloat(node["WaveSpeed"].GetString());
		m_WaterHeight = StringUtil::ParseFloat(node["WaterHeight"].GetString());
		m_ShoreDark = StringUtil::ParseVec3(node["ShoreDark"].GetString());
		m_ShoreLight = StringUtil::ParseVec3(node["ShoreLight"].GetString());
		m_SeaDark = StringUtil::ParseVec3(node["SeaDark"].GetString());
		m_SeaLight = StringUtil::ParseVec3(node["SeaLight"].GetString());
		m_WaveParam = StringUtil::ParseVec4(node["WaveParam"].GetString());
		if (node.HasMember("FadeOutDistance"))
		{
			m_PixelParam = StringUtil::ParseVec2(node["FadeOutDistance"].GetString());
		}
	}

	void OceanRender::exportDepthData(DataStream& dataStream)
	{
		if (m_isPrepared)
		{
			ui32 vertexStride = m_VertexSize / sizeof(float);
			float* pVertex = NULL;
			for (ui32 i = 0; i < m_VertexCount; ++i)
			{
				pVertex = m_VertexBuffer + i*vertexStride;
				float depth = pVertex[5];
				dataStream.write(&depth, sizeof(float));
			}
		}
	}

	void OceanRender::importDepthData(DataStream& dataStream)
	{
		if (m_isPrepared)
		{
			float dtSpace = 1.0f / ((float)(m_NumGird - 1));
			float maxDepth = 10.0f;
			ui32 vertexStride = m_VertexSize / sizeof(float);
			float* pVertex = NULL;
			for (ui32 i = 0; i < m_VertexCount; ++i)
			{
				float depth = maxDepth;
				dataStream.read(&depth, sizeof(float));
				pVertex = m_VertexBuffer + i*vertexStride;
				pVertex[5] = depth;
				//if (depth > 0.95f)
				//{
					pVertex[6] = (float)i*dtSpace * depth * maxDepth;
					pVertex[6] *= pVertex[6];
				//}
				//else
				//{
				//	pVertex[6] = 0.0f;
				//}
			}
			Buffer vertBuff(m_VertexCount * m_VertexSize, m_VertexBuffer);
			m_GpuVertexBuffer->updateSubData(0, vertBuff);
		}
	}

	void OceanRender::exportData(rapidxml::xml_node<>* parentNode, rapidxml::xml_document<>& doc)
	{
		rapidxml::xml_node<>* GirdNumNode = doc.allocate_node(rapidxml::node_element, "GirdNum", NULL);
		rapidxml::xml_node<>* GridSizeNode = doc.allocate_node(rapidxml::node_element, "GridSize", NULL);
		rapidxml::xml_node<>* speedNode = doc.allocate_node(rapidxml::node_element, "WaveSpeed", NULL);
		rapidxml::xml_node<>* heightNode = doc.allocate_node(rapidxml::node_element, "WaterHeight", NULL);
		rapidxml::xml_node<>* ShoreDarkNode = doc.allocate_node(rapidxml::node_element, "ShoreDark", NULL);
		rapidxml::xml_node<>* ShoreLightNode = doc.allocate_node(rapidxml::node_element, "ShoreLight", NULL);
		rapidxml::xml_node<>* SeaDarkNode = doc.allocate_node(rapidxml::node_element, "SeaDark", NULL);
		rapidxml::xml_node<>* SeaLightNode = doc.allocate_node(rapidxml::node_element, "SeaLight", NULL);
		rapidxml::xml_node<>* WaveParamNode = doc.allocate_node(rapidxml::node_element, "WaveParam", NULL);
		rapidxml::xml_node<>* FadeDistNode = doc.allocate_node(rapidxml::node_element, "FadeOutDistance", NULL);

		char* strNumGrid = doc.allocate_string(StringUtil::ToString(m_NumGird).c_str());
		char* strGridSize = doc.allocate_string(StringUtil::ToString(m_GridSize).c_str());
		char* strWaveSpeed = doc.allocate_string(StringUtil::ToString(m_WaveSpeed).c_str());
		char* strHeight = doc.allocate_string(StringUtil::ToString(m_WaterHeight).c_str());
		char* strShoreDark = doc.allocate_string(StringUtil::ToString(m_ShoreDark).c_str());
		char* strShoreLight = doc.allocate_string(StringUtil::ToString(m_ShoreLight).c_str());
		char* strSeaDark = doc.allocate_string(StringUtil::ToString(m_SeaDark).c_str());
		char* strSeaLight = doc.allocate_string(StringUtil::ToString(m_SeaLight).c_str());
		char* strWaveParam = doc.allocate_string(StringUtil::ToString(m_WaveParam).c_str());
		char* strFadeOutDist = doc.allocate_string(StringUtil::ToString(m_PixelParam).c_str());

		GirdNumNode->append_attribute(doc.allocate_attribute("value", strNumGrid));
		GridSizeNode->append_attribute(doc.allocate_attribute("value", strGridSize));
		speedNode->append_attribute(doc.allocate_attribute("value", strWaveSpeed));
		heightNode->append_attribute(doc.allocate_attribute("value", strHeight));
		ShoreDarkNode->append_attribute(doc.allocate_attribute("value", strShoreDark));
		ShoreLightNode->append_attribute(doc.allocate_attribute("value", strShoreLight));
		SeaDarkNode->append_attribute(doc.allocate_attribute("value", strSeaDark));
		SeaLightNode->append_attribute(doc.allocate_attribute("value", strSeaLight));
		WaveParamNode->append_attribute(doc.allocate_attribute("value", strWaveParam));
		FadeDistNode->append_attribute(doc.allocate_attribute("value", strFadeOutDist));

		parentNode->append_node(GirdNumNode);
		parentNode->append_node(GridSizeNode);
		parentNode->append_node(speedNode);
		parentNode->append_node(heightNode);
		parentNode->append_node(ShoreDarkNode);
		parentNode->append_node(ShoreLightNode);
		parentNode->append_node(SeaDarkNode);
		parentNode->append_node(SeaLightNode);
		parentNode->append_node(WaveParamNode);
		parentNode->append_node(FadeDistNode);
	}

	void OceanRender::importData(rapidxml::xml_node<>* parentNode)
	{
		rapidxml::xml_node<>* pGridNumNode		= parentNode->first_node();
		rapidxml::xml_node<>* pGridSizeNode		= pGridNumNode->next_sibling();
		rapidxml::xml_node<>* pSpeedNode		= pGridSizeNode->next_sibling();
		rapidxml::xml_node<>* pHeightNode		= pSpeedNode->next_sibling();
		rapidxml::xml_node<>* pShoreDarkNode	= pHeightNode->next_sibling();
		rapidxml::xml_node<>* pShoreLightNode	= pShoreDarkNode->next_sibling();
		rapidxml::xml_node<>* pSeaDarkNode		= pShoreLightNode->next_sibling();
		rapidxml::xml_node<>* pSeaLightNode		= pSeaDarkNode->next_sibling();
		rapidxml::xml_node<>* pWaveParamNode	= pSeaLightNode->next_sibling();
		rapidxml::xml_node<>* pFadeDistNode 	= pWaveParamNode->next_sibling();

		rapidxml::xml_attribute<>* GridNumValue		= pGridNumNode->first_attribute();
		rapidxml::xml_attribute<>* GridSizeValue	= pGridSizeNode->first_attribute();
		rapidxml::xml_attribute<>* SpeedValue		= pSpeedNode->first_attribute();
		rapidxml::xml_attribute<>* HeightValue		= pHeightNode->first_attribute();
		rapidxml::xml_attribute<>* ShoreDarkValue	= pShoreDarkNode->first_attribute();
		rapidxml::xml_attribute<>* ShoreLightValue  = pShoreLightNode->first_attribute();
		rapidxml::xml_attribute<>* SeaDarkValue		= pSeaDarkNode->first_attribute();
		rapidxml::xml_attribute<>* SeaLightValue	= pSeaLightNode->first_attribute();
		rapidxml::xml_attribute<>* WaveParamValue   = pWaveParamNode->first_attribute();
		rapidxml::xml_attribute<>* FadeDistValue	= pFadeDistNode ? pFadeDistNode->first_attribute() : NULL;

		m_NumGird = StringUtil::ParseInt(GridNumValue->value());
		m_GridSize = StringUtil::ParseFloat(GridSizeValue->value());
		m_WaveSpeed = StringUtil::ParseFloat(SpeedValue->value());
		m_WaterHeight = StringUtil::ParseFloat(HeightValue->value());
		m_ShoreDark = StringUtil::ParseVec3(ShoreDarkValue->value());
		m_ShoreLight = StringUtil::ParseVec3(ShoreLightValue->value());
		m_SeaDark = StringUtil::ParseVec3(SeaDarkValue->value());
		m_SeaLight = StringUtil::ParseVec3(SeaLightValue->value());
		m_WaveParam = StringUtil::ParseVec4(WaveParamValue->value());
		if (FadeDistValue)	m_PixelParam = StringUtil::ParseVec2(FadeDistValue->value());
	}

	void OceanRender::setHeight( float wl )
	{
		if (m_WaterHeight != wl)
		{
			m_WaterHeight = wl;
			float* pVertex = m_VertexBuffer;
			float dtSpace = 1.0f / ((float)(m_NumGird - 1));
			float maxDepth = 10.0f;
			//float BiasSize = m_NumGird / 4.0f;
			for (int i = 0; i < (int)m_NumGird; i++)
			{
				for (int j = 0; j < (int)m_NumGird; j++)
				{
					pVertex[1] = wl;

					//depth
					Ray ray(*(Vector3*)pVertex + Vector3::UNIT_Y* 1000.f, Vector3::NEG_UNIT_Y);
					Vector3 intersectPos; //
					float depth = maxDepth;
					if (SceneManager::Instance()->getCurrentScene()->getRayIntersectPoint(ray, intersectPos, NULL, GOT_SMESH | GOT_SMESH_L | GOT_TERRAIN))
					{
						depth = Math::Min(Math::Abs(m_WaterHeight - intersectPos.y), maxDepth);
					}
					depth = Math::Min(depth / maxDepth, 1.0f);
					depth = 1.0f - depth;
					pVertex[5] = depth;
					//if (depth > 0.95f)
					//{
						pVertex[6] = (float)i*dtSpace * depth * maxDepth;
						pVertex[6] *= pVertex[6];
					//}
					//else
					//{
					//	pVertex[6] = 0.0f;
					//}
					//pVertex += 7;
				}
			}

			// Renderer* pRenderer = Renderer::Instance();
			Buffer vertBuff(m_VertexCount * m_VertexSize, m_VertexBuffer);
			m_GpuVertexBuffer->updateSubData(0, vertBuff);
		}
	}

	void OceanRender::setWaveParam(const Vector4& f)
	{
		m_WaveSpeed = f.y;
		m_WaveParam.z = f.z;
		m_WaveParam.w = f.w;
		setHeight(f.x);
	}

	void OceanRender::setGridNum(ui16 _num)
	{
		if (m_NumGird != _num)
		{
			_createGeometry(_num);
		}
	}

	void OceanRender::setGridSize(float _size)
	{
		if (m_GridSize != _size)
		{
			m_GridSize = _size;
			_createGeometry(m_NumGird);
		}
	}

	LORD::Vector3 OceanRender::editor_modify(const Vector2& pos, const OceanBrushInfo& brushInfo, bool bBrush)
	{
		Ray ray;
		Vector2 uv;
		SceneManager::Instance()->getMainCamera()->getCameraRay(ray, pos);

		Vector3 hitPos = Vector3::INVALID;
		Real tMin;
		bool bHit = false;
		ui32 vertexStride = m_VertexSize / sizeof(float);

		for (ui32 i = 0; i < m_IndexCount; i+=3)
		{
			if (ray.hitTri(Triangle(*(Vector3*)(m_VertexBuffer + m_IndexBuffer[i] * vertexStride),
				*(Vector3*)(m_VertexBuffer + m_IndexBuffer[i + 1] * vertexStride),
				*(Vector3*)(m_VertexBuffer + m_IndexBuffer[i + 2] * vertexStride)),
				tMin))
			{
				hitPos = ray.getPoint(tMin);
				hitPos.y = m_WaterHeight;
				bHit = true;
				break;
			}
		}

		if (bHit && bBrush)
		{
			float dtSpace = 1.0f / ((float)(m_NumGird - 1));
			float rSqr = brushInfo.radius*brushInfo.radius;
			float maxDepth = 10.0f;

			for (ui32 i = 0; i < m_VertexCount; ++i)
			{
				Vector3 pos = *(Vector3*)(m_VertexBuffer + i*vertexStride);
				float lenSqrt = (hitPos - pos).lenSqr();
				if (lenSqrt <= rSqr)
				{
					float* pVertex = m_VertexBuffer + i*vertexStride;

					float blendines = Math::Max(1.0f - Math::Sqrt(lenSqrt) / brushInfo.radius, 0.0f) * brushInfo.strength;

					float depth = pVertex[5];
					depth = 1.0f - depth;
					depth *= maxDepth;

					if (brushInfo.depth)
					{
						depth = Math::Max(depth + blendines, 0.0f);
					}
					else
					{
						depth = Math::Max(0.0f, Math::Min(depth - blendines, maxDepth));
					}
					
					depth = Math::Min(depth / maxDepth, 1.0f);
					depth = 1.0f - depth;
					pVertex[5] = depth;
					//if (depth > 0.95f)
					//{
						pVertex[6] = (float)i*dtSpace * depth * maxDepth;
						pVertex[6] *= pVertex[6];
					//}
					//else
					//{
					//	pVertex[6] = 0.0f;
					//}
				}
			}

			Buffer vertBuff(m_VertexCount * m_VertexSize, m_VertexBuffer);
			m_GpuVertexBuffer->updateSubData(0, vertBuff);
		}

		return hitPos;
	}
}
