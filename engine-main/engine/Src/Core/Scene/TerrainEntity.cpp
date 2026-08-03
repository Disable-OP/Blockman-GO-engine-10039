#include "Core.h"
#include "TerrainEntity.h"
#include "SceneManager.h"
#include "Render/RenderQueue.h"
#include "Render/RenderInput.h"
#include "Render/Renderer.h"
#include "Render/Material.h"
#include "Render/ShaderProgramManager.h"
#include "Model/Mesh.h"
#include "Scene/SceneNode.h"
#include "Scene/Camera.h"

namespace LORD
{
	TerrainEntity::TerrainEntity()
		: Renderable()
		, m_pSceneNode(NULL)
		, m_isVisible(true)
		, m_LMTexture(NULL)
		, m_GpuVertexBuffer(NULL)
		, m_GpuIndexBuffer(NULL)
		, m_VertexBuffer(NULL)
		, m_IndexBuffer(NULL)
		, m_VertexCount(0)
	    , m_IndexCount(0)
		, m_VertexStride(0)
	{
		memset(m_LMParam, 0, sizeof(Vector4)*2);
		m_TextureStage[0] = 0;
		m_TextureStage[1] = 1;
		m_TextureStage[2] = 2;
		m_TextureStage[3] = 3;
		m_TextureStage[4] = 4;
	}

	TerrainEntity::~TerrainEntity()
	{
		m_VertexElements.clear();
		LordSafeDelete(m_RenderInput);
		LordSafeDelete(m_GpuVertexBuffer);
		LordSafeDelete(m_GpuIndexBuffer);
		LordSafeFree(m_VertexBuffer);
		LordSafeFree(m_IndexBuffer);
		if (m_pSceneNode)
		{
			m_pSceneNode->getParent()->destroyChild(m_pSceneNode);
			m_pSceneNode = NULL;
		}
	}

	void TerrainEntity::attachTo(SceneNode* pNode)
	{
		m_pSceneNode = pNode;
	}

	SceneNode* TerrainEntity::getSceneNode() const
	{
		return m_pSceneNode;
	}

	void TerrainEntity::importSubMesh( SubMesh* pSubMesh )
	{
		LordSafeFree(m_VertexBuffer);
		LordSafeFree(m_IndexBuffer);

		m_VertexStride = pSubMesh->getVertexStride();
		m_VertexCount = pSubMesh->getVertexCount();
		m_IndexCount = pSubMesh->getIndexCount();

		size_t bufSize = m_VertexCount*m_VertexStride;
		m_VertexBuffer = LordMalloc(bufSize);
		memcpy(m_VertexBuffer, pSubMesh->getVertices(), bufSize);

		bufSize = m_IndexCount*sizeof(Word);
		m_IndexBuffer = LordMalloc(bufSize);
		memcpy(m_IndexBuffer, pSubMesh->getIndices(), bufSize);

		m_VertexElements.assign(pSubMesh->getVertexElements().begin(), pSubMesh->getVertexElements().end());

		m_LocalBox = pSubMesh->getLocalBox();
		updateWorldAABB(m_pSceneNode->getWorldMatrix());

		createRenderable();
}

	void TerrainEntity::createRenderable()
	{
		if (m_RenderInput)
		{
			LordSafeDelete(m_RenderInput);
			LordSafeDelete(m_GpuVertexBuffer);
			LordSafeDelete(m_GpuIndexBuffer);
		}

		Buffer vertBuff(m_VertexCount*m_VertexStride, m_VertexBuffer);
		m_GpuVertexBuffer = Renderer::Instance()->createVertexBuffer(GPUBuffer::GBU_GPU_READ, vertBuff);
		Buffer indexBuff(m_IndexCount*sizeof(Word), m_IndexBuffer);
		m_GpuIndexBuffer = Renderer::Instance()->createIndexBuffer(GPUBuffer::GBU_GPU_READ, indexBuff);

		SceneManager* pSceneManager = SceneManager::Instance();
		Renderer* pRenderer = Renderer::Instance();
		m_RenderInput = pRenderer->createRenderInput(m_VertexElements, m_Material->getShaderProgram());
		m_RenderInput->bindGPUBuffers(m_GpuVertexBuffer, m_VertexStride, m_GpuIndexBuffer, sizeof(Word));
		m_Material->beginShaderParams(8);
		m_Material->setShaderParam("LMSampler", SPT_TEXTURE, &m_TextureStage[0]);
		m_Material->setShaderParam("texSampler", SPT_TEXTURE, &m_TextureStage[1]);
		m_Material->setShaderParam("texSampler1", SPT_TEXTURE, &m_TextureStage[2]);
		m_Material->setShaderParam("texSampler2", SPT_TEXTURE, &m_TextureStage[3]);
		//m_Material->setShaderParam("texSampler3", SPT_TEXTURE, &m_TextureStage[4]);
		m_Material->setShaderParam("LMParam", SPT_VEC4, (void*)m_LMParam, 2);
		m_Material->setShaderParam("fogParam", SPT_VEC4, (void*)pSceneManager->getFogParamVec4Ptr(), 3);
		m_Material->setShaderParam("matWorld", SPT_MAT4, (void*)&(m_pSceneNode->getWorldMatrix()));
		m_Material->setShaderParam("matWVP", SPT_MAT4, (void*)&m_matWVP);
		m_Material->endShaderParams();
	}

	void TerrainEntity::update()
	{
		LordAssert(m_pSceneNode);
		
		m_matWVP = m_pSceneNode->getWorldMatrix() * SceneManager::Instance()->getMainCamera()->getViewProjMatrix();
	}

	Real TerrainEntity::getViewDepth(Camera* pCamera) const
	{
		LordAssert(pCamera);

		const Matrix4& matView = pCamera->getViewMatrix();

		// optimazed the depth calculation
		const Vector3 vPosWorld = m_pSceneNode->getWorldPosition();
		return (vPosWorld.x * matView.m02 + vPosWorld.y * matView.m12 + vPosWorld.z * matView.m22 + matView.m32);
	}
	
	void TerrainEntity::setVisible( const bool& visible )
	{
		m_isVisible = visible;
	}

	const bool& TerrainEntity::getVisible() const
	{
		return m_isVisible;
	}

	void TerrainEntity::setTexture( uint index, Texture* pTex )
	{
		if (m_Material)
			m_Material->activeTexture(index, pTex);
	}

	void* TerrainEntity::getVertices()
	{
		return m_VertexBuffer;
	}

	void* TerrainEntity::getIndices()
	{
		return m_IndexBuffer;
	}

	LORD::ui16 TerrainEntity::getVertexCount() const
	{
		return m_VertexCount;
	}

	LORD::ui16 TerrainEntity::getIndexCount() const
	{
		return m_IndexCount;
	}

	LORD::ui16 TerrainEntity::getVertexStride() const
	{
		return m_VertexStride;
	}

}
