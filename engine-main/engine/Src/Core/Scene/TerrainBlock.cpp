#include "Core.h"
#include "Object/Root.h"
#include "TerrainBlock.h"
#include "Scene.h"
#include "Render/RenderInput.h"
#include "Render/TextureManager.h"
#include "Render/Renderer.h"
#include "Camera.h"
#include "SceneManager.h"
#include "Render/GPUBuffer.h"
#include "Render/Material.h"
#include "Render/ShaderProgramManager.h"
#include "Util/QueryObject.h"

namespace LORD
{
	unsigned int nBatchSize = 32;
	unsigned int nBatchCount = 512 / nBatchSize;

	TerrainBlock::TerrainBlock(Scene* scene, ui16 xUnitOffset, ui16 zUnitOffset, bool bSphere)
		: Renderable()
		, m_VertexNum(0)
		, m_IndexNum(0)
		, m_xUnitOffset(xUnitOffset)
		, m_zUnitOffset(zUnitOffset)
		, m_isTransparent(false)
		, m_bPrepareed(false)
		, m_isNeedToBeDraw(true)
		, m_bSphere(bSphere)
		, m_ownerScene(scene)
		, m_vetexBuffer(NULL)
		, m_indexBuffer(NULL)
		, m_terrainTexture(NULL)
		, m_Vertices(NULL)
		, m_Indices(NULL)
		, m_MeshObject(NULL)
	{
		m_RenderQueue = SceneManager::Instance()->getRenderQueue("Scene");
	}

	TerrainBlock::~TerrainBlock()
	{
		unprepare();
	}

	void TerrainBlock::prepare()
	{
		if(!m_bPrepareed && (m_isNeedToBeDraw || Root::Instance()->isEditorMode()))
		{
			// prepare texture
			// use etc texture for terrain blocks
			//String texName = StringUtil::Format("%s_%d_%d.ktx", m_ownerScene->getSceneName().c_str(), m_xUnitOffset, m_zUnitOffset);
			String texName = StringUtil::Format("%s_%d_%d.tga", m_ownerScene->getSceneName().c_str(), m_xUnitOffset, m_zUnitOffset);
//			String texName = StringUtil::Format("%s_%d_%d.pvr", m_ownerScene->getSceneName().c_str(), m_xUnitOffset, m_zUnitOffset);
			
			m_terrainTexture = TextureManager::Instance()->createTexture(texName);

			if (!m_terrainTexture->load())
			{
				TextureManager::Instance()->releaseResource(m_terrainTexture);
				texName = StringUtil::Format("%s_%d_%d.jpg", m_ownerScene->getSceneName().c_str(), m_xUnitOffset, m_zUnitOffset);
				m_terrainTexture = TextureManager::Instance()->createTexture(texName);
				m_terrainTexture->load();
			}

			if (m_bSphere)
			{
				createSphereRenderble();
			}
			else
			{
				createRenderable();
				// update box
				updateBox();
			}


			m_bPrepareed = true;
		}
	}

	void TerrainBlock::updateBox()
	{
		Vector3 minVec;
		float wpb = m_ownerScene->getWidthPerBlock();
		float hpb = m_ownerScene->getHeightPerBlock();
		minVec.x = m_xUnitOffset*wpb;
		minVec.y = -MAX_TERRAIN_BLOCK_HEIGHT;
		minVec.z = m_zUnitOffset*hpb;

		Vector3 maxVec;
		maxVec.x = minVec.x + wpb;
		maxVec.y = 0.0f;
		maxVec.z = minVec.z + hpb;

		m_box = Box(minVec, maxVec);
	}

	void TerrainBlock::unprepare()
	{
		if (m_bPrepareed)
		{
			LordSafeFree(m_Vertices);
			LordSafeFree(m_Indices);
			LordSafeDelete(m_vetexBuffer);
			LordSafeDelete(m_indexBuffer);
			LordSafeDelete(m_RenderInput);
			LordSafeDelete(m_Material);

			if(m_terrainTexture)
			{
				TextureManager::Instance()->releaseResource(m_terrainTexture);
				m_terrainTexture = NULL;
			}

			m_bPrepareed = false;
		}
		if (m_MeshObject)
		{
			SceneNode* node = m_MeshObject->getParent();
			node->getParent()->destroyChild(node);
			QueryObjectManager::Instance()->destroyQueryObject(m_MeshObject);
			m_MeshObject = NULL;
		}
	}
	 
	void TerrainBlock::createRenderable()
	{
		ShaderProgram* shaderProgram = ShaderProgramManager::Instance()->getShaderProgram("TerrainBlock");
		m_Material = LordNew Material();
		{
			BlendState::BlendDesc blenddesc = m_Material->getBlendState()->getDesc();
			blenddesc.bBlendEnable = false;

			SamplerState::SamplerDesc samdesc = m_Material->getSamplerState(0)->getDesc();
			samdesc.minFilter = SamplerState::FO_LINEAR;
			samdesc.magFilter = SamplerState::FO_LINEAR;
			samdesc.mipFilter = SamplerState::FO_NONE;
			samdesc.addrUMode = SamplerState::AM_MIRROR;
			samdesc.addrVMode = SamplerState::AM_MIRROR;

			m_Material->setBlendState(blenddesc);
			m_Material->setSamplerState(0, samdesc);
		}

		m_Material->setShaderProgram(shaderProgram);

		m_VertexElement.clear();
		RenderInput::VertexElement posElm, colorElm, uvElm;
		posElm.semantic = RenderInput::VS_POSITION;
		posElm.pixFmt = PF_RGB32_FLOAT;
		colorElm.semantic = RenderInput::VS_COLOR;
		colorElm.pixFmt = PF_RGBA8_UNORM;
		uvElm.semantic = RenderInput::VS_TEXCOORD;
		uvElm.pixFmt = PF_RG32_FLOAT;
		m_VertexElement.push_back(posElm);
		m_VertexElement.push_back(colorElm);
		m_VertexElement.push_back(uvElm);
		m_RenderInput = Renderer::Instance()->createRenderInput(m_VertexElement, shaderProgram);

		// create vertex buffer and index buffer
		unsigned int nVertexCount = 4;
		unsigned int nIndexCount = 6;

		TerrainVertexType* vertices = NULL;
		vertices = LordAlloc(TerrainVertexType, nVertexCount);

		float wpb = m_ownerScene->getWidthPerBlock();
		float hpb = m_ownerScene->getHeightPerBlock();
		Vector2 leftTop;
		leftTop.x = m_xUnitOffset*wpb;
		leftTop.y = m_zUnitOffset*hpb;

		Vector2 rightbottom;
		rightbottom.x = leftTop.x + wpb;
		rightbottom.y = leftTop.y + hpb;

		float border = 8.f/512.f;
		// init vertices
		vertices[0].position.x = leftTop.x;  // Position.
		vertices[0].position.y = 0.0f;
		vertices[0].position.z = leftTop.y ;
		vertices[0].color = 0xFFFFFFFF;
		vertices[0].texCoord.x = border;
		vertices[0].texCoord.y = border;//0.002f;//0.0f;

		vertices[1].position.x = rightbottom.x;  // Position.
		vertices[1].position.y = 0.0f;
		vertices[1].position.z = rightbottom.y;
		vertices[1].color = 0xFFFFFFFF;
		vertices[1].texCoord.x = 1.0f-border;
		vertices[1].texCoord.y = 1.0f-border;//0.9988f;//1.0f;

		vertices[2].position.x =  leftTop.x;  // Position.
		vertices[2].position.y =  0.0f;
		vertices[2].position.z =  rightbottom.y ;
		vertices[2].color = 0xFFFFFFFF;
		vertices[2].texCoord.x = border;
		vertices[2].texCoord.y = 1.0f-border; //0.9988f; //1.0f;

		vertices[3].position.x =  rightbottom.x;  // Position.
		vertices[3].position.y =  0.0f;
		vertices[3].position.z =  leftTop.y;
		vertices[3].color = 0xFFFFFFFF;
		vertices[3].texCoord.x = 1.0f-border;
		vertices[3].texCoord.y = border; //0.002f; //0.0f;


		Buffer vertBuffer(nVertexCount*sizeof(TerrainVertexType), vertices);
		m_vetexBuffer = Renderer::Instance()->createVertexBuffer(GPUBuffer::GBU_DEFAULT, vertBuffer);

		Word* indices = NULL;
		indices = LordAlloc(Word, nIndexCount);

		Word* quadStart = indices;

		quadStart[0] = 0;
		quadStart[1] = 1;
		quadStart[2] = 2;
		quadStart[3] = 0;
		quadStart[4] = 3;
		quadStart[5] = 1;


		Buffer idxBuffer(nIndexCount*sizeof(Word), indices);
		m_indexBuffer = Renderer::Instance()->createIndexBuffer(GPUBuffer::GBU_DEFAULT, idxBuffer);

		m_RenderInput->bindGPUBuffers(m_vetexBuffer, sizeof(TerrainVertexType), m_indexBuffer, sizeof(Word));
		LordFree(vertices);
		LordFree(indices);

		m_Material->activeTexture(0, m_terrainTexture);
		m_Material->beginShaderParams(2);
		m_Material->setShaderParam("matWVP", SPT_MAT4, (void*)&SceneManager::Instance()->getMainCamera()->getViewProjMatrix());
		m_Material->setShaderParam("texSampler", SPT_TEXTURE, (void*)&TEXTURE_UNIT0);
		m_Material->endShaderParams();
	}

	void TerrainBlock::createSphereRenderble()
	{
		bool isEditorMode = Root::Instance()->isEditorMode();

		ShaderProgram* shaderProgram = ShaderProgramManager::Instance()->getShaderProgram("TerrainSphere");
		m_Material = LordNew Material();
		{
			SamplerState::SamplerDesc samdesc = m_Material->getSamplerState(0)->getDesc();
			samdesc.minFilter = SamplerState::FO_LINEAR;
			samdesc.magFilter = SamplerState::FO_LINEAR;
			samdesc.mipFilter = SamplerState::FO_NONE;
			samdesc.addrUMode = SamplerState::AM_MIRROR;
			samdesc.addrVMode = SamplerState::AM_MIRROR;
			m_Material->setSamplerState(0, samdesc);
		}
		m_Material->setShaderProgram(shaderProgram);

		m_VertexElement.clear();
		RenderInput::VertexElement posElm, uvElm;
		posElm.semantic = RenderInput::VS_POSITION;
		posElm.pixFmt = PF_RGB32_FLOAT;
		uvElm.semantic = RenderInput::VS_TEXCOORD;
		uvElm.pixFmt = PF_RG32_FLOAT;
		m_VertexElement.push_back(posElm);
		m_VertexElement.push_back(uvElm);

		m_RenderInput = Renderer::Instance()->createRenderInput(m_VertexElement, shaderProgram);

		m_VertexNum = (nBatchCount + 1)*(nBatchCount + 1);
		m_IndexNum = nBatchCount*nBatchCount * 6;
		if (m_HeightArray.empty())
		{
			m_HeightArray.reserve(m_VertexNum);
			m_HeightArray.resize(m_VertexNum, 0.0f);
		}
		// create vertex buffer and index buffer
		LordSafeFree(m_Vertices);
		LordSafeFree(m_Indices);
		m_Vertices = LordAlloc(TerrainSphereVertexType, m_VertexNum);
		m_Indices = LordAlloc(Word, m_IndexNum);
		
		ui16 xcount, ycount;
		m_ownerScene->getBlockCount(xcount, ycount);

		float border = 8.f / 512.f;
		float wpb = m_ownerScene->getWidthPerBlock();
		float hpb = m_ownerScene->getHeightPerBlock();
		float Radius = m_ownerScene->getSphereRadius();

		const Vector3& center = m_ownerScene->getSphereTerrainCenter();
		Vector2 leftTop(m_xUnitOffset*wpb, m_zUnitOffset*hpb);
		Vector2 rightbottom(leftTop.x + wpb, leftTop.y + hpb);

		m_box.reset();
		for (unsigned int i = 0; i < m_VertexNum; ++i)
		{
			int col = i % (nBatchCount + 1);
			int row = i / (nBatchCount + 1);
			m_Vertices[i].position.x = leftTop.x + (float)col / (float)nBatchCount * wpb;
			m_Vertices[i].position.y = 0.0f;
			m_Vertices[i].position.z = leftTop.y + (float)row / (float)nBatchCount * hpb;
			m_Vertices[i].texCoord.x = border + (1.0f - border*2.0f) * (float)col / (float)nBatchCount;
			m_Vertices[i].texCoord.y = border + (1.0f - border*2.0f) * (float)row / (float)nBatchCount;//0.002f;//0.0f;
			Vector3 dir = (center - m_Vertices[i].position).normalizedCopy();
			m_Vertices[i].position = center - dir*(Radius + m_HeightArray[i]);
			m_box.addPoint(m_Vertices[i].position);
		}

		Word* quadStart = m_Indices;
		for (unsigned int i = 0; i < nBatchCount; ++i)
		{
			for (unsigned int j = 0; j < nBatchCount; ++j)
			{
				quadStart[0] = i	   * (nBatchCount + 1) + j;
				quadStart[1] = i	   * (nBatchCount + 1) + j + 1;
				quadStart[2] = (i + 1) * (nBatchCount + 1) + j + 1;
				quadStart[3] = i	   * (nBatchCount + 1) + j;
				quadStart[4] = (i + 1) * (nBatchCount + 1) + j + 1;
				quadStart[5] = (i + 1) * (nBatchCount + 1) + j;
				quadStart += 6;
			}
		}

		Buffer vertBuffer(m_VertexNum*sizeof(TerrainSphereVertexType), m_Vertices);
		Buffer idxBuffer(m_IndexNum*sizeof(Word), m_Indices);
		if (isEditorMode)
		{
			m_vetexBuffer = Renderer::Instance()->createVertexBuffer(GPUBuffer::GBU_DEFAULT, vertBuffer);
			m_indexBuffer = Renderer::Instance()->createIndexBuffer(GPUBuffer::GBU_DEFAULT, idxBuffer);
			m_RenderInput->bindGPUBuffers(m_vetexBuffer, sizeof(TerrainSphereVertexType), m_indexBuffer, sizeof(Word));
		}
		else
		{
			m_vetexBuffer = Renderer::Instance()->createVertexBuffer(GPUBuffer::GBU_GPU_READ, vertBuffer);
			m_indexBuffer = Renderer::Instance()->createIndexBuffer(GPUBuffer::GBU_GPU_READ, idxBuffer);
			m_RenderInput->bindGPUBuffers(m_vetexBuffer, sizeof(TerrainSphereVertexType), m_indexBuffer, sizeof(Word));
			LordSafeFree(m_Vertices);
			LordSafeFree(m_Indices);
		}
		m_Material->activeTexture(0, m_terrainTexture);
		m_Material->beginShaderParams(2);
		m_Material->setShaderParam("matWVP", SPT_MAT4, (void*)&SceneManager::Instance()->getMainCamera()->getViewProjMatrix());
		m_Material->setShaderParam("texSampler", SPT_TEXTURE, (void*)&TEXTURE_UNIT0);
		m_Material->endShaderParams();
	}

	void TerrainBlock::reBuildGeometry()
	{
		float border = 8.f / 512.f;
		ui16 xcount, ycount;
		m_ownerScene->getBlockCount(xcount, ycount);
		float wpb = m_ownerScene->getWidthPerBlock();
		float hpb = m_ownerScene->getHeightPerBlock();
		// float scenewidht = (float)xcount*wpb;
		// float sceneheight = (float)ycount*hpb;
		float radius = m_ownerScene->getSphereRadius();
		const Vector3& center = m_ownerScene->getSphereTerrainCenter();
		Vector2 leftTop(m_xUnitOffset*wpb, m_zUnitOffset*hpb);
		Vector2 rightbottom(leftTop.x + wpb, leftTop.y + hpb);

		m_box.reset();
		for (unsigned int i = 0; i < m_VertexNum; ++i)
		{
			int col = i % (nBatchCount + 1);
			int row = i / (nBatchCount + 1);
			m_Vertices[i].position.x = leftTop.x + (float)col / (float)nBatchCount * wpb;
			m_Vertices[i].position.y = 0.0f;
			m_Vertices[i].position.z = leftTop.y + (float)row / (float)nBatchCount * hpb;
			m_Vertices[i].texCoord.x = border + (1.0f - border*2.0f) * (float)col / (float)nBatchCount;
			m_Vertices[i].texCoord.y = border + (1.0f - border*2.0f) * (float)row / (float)nBatchCount;//0.002f;//0.0f;
			Vector3 dir = (center - m_Vertices[i].position).normalizedCopy();
			m_Vertices[i].position = center - dir*(radius + m_HeightArray[i]);
			m_box.addPoint(m_Vertices[i].position);
		}
		Buffer vertBuffer(m_VertexNum*sizeof(TerrainSphereVertexType), m_Vertices);
		m_vetexBuffer->updateSubData(0, vertBuffer);

		editor_refleshMesh();
	}

	std::pair<bool, Real> TerrainBlock::editor_rayIntersect(const Ray& ray)
	{
		std::pair<bool, Real> result;
		result.first = false;
		result.second = Math::MAX_FLOAT;
				
		if (m_Vertices)
		{
			Real tMin;
			const float* vertexPtr = (float*)m_Vertices;
			const Word* indexPtr = m_Indices;
			size_t vertexSize = sizeof(TerrainSphereVertexType) / sizeof(float);
			ui16 indexNum = m_IndexNum;
			float lastlen = Math::MAX_FLOAT;
			
			for (ui16 i = 0; i < indexNum; i += 3)
			{
				if (ray.hitTri(Triangle(*(Vector3*)(vertexPtr + indexPtr[i] * vertexSize),
					*(Vector3*)(vertexPtr + indexPtr[i + 1] * vertexSize),
					*(Vector3*)(vertexPtr + indexPtr[i + 2] * vertexSize)),
					tMin))
				{
					if (tMin < lastlen)
						lastlen = tMin;

					result.first = true;
				}
			}

			result.second = lastlen;
		}

		return result;
	}

	void TerrainBlock::editor_modifyTerrainHeight(const Vector3& pos, const TerrainBrushInfo& info, float escapeTime)
	{
		if (m_Vertices)
		{
			/*Box brushBox;
			brushBox.fromBSphere(Sphere(pos, info.radius));
			if (m_box.isIntersected(brushBox))*/
			{
				Vector3 brushPos(pos.x, 0.0, pos.z);
				for (ui16 i = 0; i < m_VertexNum; ++i)
				{
					Vector3 vertexPos = m_Vertices[i].position;
					vertexPos.y = 0.0f;
					float innerStrength = (vertexPos - brushPos).len();
					if (innerStrength <= info.radius)
					{
						if (info.fill)
						{
							m_HeightArray[i] = 0.0f;
						}
						else
						{
							innerStrength = Math::Min((info.down ? -1.0f : 1.0f) * (info.radius - innerStrength) / info.radius * info.strength, 0.5f)*escapeTime;
							m_HeightArray[i] += innerStrength;
						}
					}
				}

				reBuildGeometry();
			}
		}

	}

	void TerrainBlock::editor_createEditMesh()
	{
		if (Root::Instance()->isEditorMode())
		{
			if (m_MeshObject == NULL)
			{
				m_MeshObject = QueryObjectManager::Instance()->createQueryObject("TerrainBlock_" + StringUtil::ToString(m_xUnitOffset) + "_" + StringUtil::ToString(m_zUnitOffset));
				m_MeshObject->attachTo(SceneManager::Instance()->getRootNode()->createChild());
				m_MeshObject->setTestDepth(false);
				m_MeshObject->setWriteDepth(false);
				m_MeshObject->setQueryFlag(0);
				m_MeshObject->setVisible(true);
				editor_refleshMesh();
			}
		}
	}

	void TerrainBlock::editor_refleshMesh()
	{
		if (m_MeshObject)
		{
			m_MeshObject->clear();
			m_MeshObject->beginPolygon(QueryObject::QO_WIREFRAME);
			for (unsigned int i = 0; i < m_VertexNum; ++i)
			{
				m_MeshObject->setPosition(m_Vertices[i].position);
				m_MeshObject->setColor(Color::GREEN);
			}
			
			for (unsigned int i = 0; i < nBatchCount; ++i)
			{
				for (unsigned int j = 0; j < nBatchCount; ++j)
				{
					m_MeshObject->setIndex(i*(nBatchCount + 1) + j);
					m_MeshObject->setIndex(i*(nBatchCount + 1) + j + 1);
					m_MeshObject->setIndex(i*(nBatchCount + 1) + j);
					m_MeshObject->setIndex((i + 1)*(nBatchCount + 1) + j);
				}
			}
			m_MeshObject->endPolygon();
		}
	}

	void TerrainBlock::editor_setMeshVisible(bool bVisible)
	{
		if (m_MeshObject)
		{
			m_MeshObject->setVisible(bVisible);
		}
		else
		{
			editor_createEditMesh();
		}
	}

	void TerrainBlock::exportData(DataStream* stream)
	{
		ui32 bufsize = m_HeightArray.size();
		float* data = m_HeightArray.data();
		stream->write(&bufsize, sizeof(ui32));
		stream->write(data, bufsize*sizeof(float));
	}

	void TerrainBlock::importData(DataStream* stream)
	{
		ui32 readsize = 0;
		stream->read(&readsize, sizeof(ui32));
		void* buf = LordMalloc(readsize*sizeof(float));
		stream->read(buf, readsize*sizeof(float));
		m_HeightArray.reserve(readsize);
		m_HeightArray.resize(readsize, 0);
		float* p = (float*)buf;
		for (ui32 i = 0; i < readsize; ++i)
		{
			m_HeightArray[i] = (*(p+i));
		}
		LordFree(buf);
	}

	void TerrainBlock::updateRenderQueue()
	{
		m_RenderQueue->addRenderable(this);
	}

	const String& TerrainBlock::editor_getTextureName()
	{
		if (m_terrainTexture)
		{
			return m_terrainTexture->getName();
		}
		return StringUtil::BLANK;
	}

}
