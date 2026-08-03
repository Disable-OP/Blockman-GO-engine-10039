#include "Core.h"
#include "SkinedRenderQueue.h"
#include "SkinedRenderable.h"
#include "BatchRenderable.h"
#include "BatchRenderQueue.h"
#include "TessManager.h"

namespace LORD
{

SkinedRenderQueue::SkinedRenderQueue(SKINED_RENDER_PASS rg)
	: m_renderInput(NULL)
	, m_shaderProgram(NULL)
	, m_samplerState(NULL)
	, m_blendState(NULL)
	, m_depthState(NULL)
	, m_rasterizerState(NULL)
	, m_sPMatMVPIndex(0)
	, m_spMatWorldIndex(0)
	, m_spVecFogIndex(0)
	, m_spFogColorIndex(0)
	, m_spCameraPosIndex(0)
	, m_spExtraColorIndex(0)
	, m_spBoneMatricesIndex(0)
	, m_sPTexTextureIndex(0)
	, m_renderGroup(rg)
	, m_renderTexture(NULL)
{ 
	m_renderables.clear();
}

SkinedRenderQueue::~SkinedRenderQueue()
{
	if (m_renderInput)
		m_renderInput->unbindGPUBuffers();

	LordSafeDelete(m_renderInput);
	LordSafeDelete(m_samplerState);
	LordSafeDelete(m_blendState);
	LordSafeDelete(m_depthState);
	LordSafeDelete(m_rasterizerState);
    
    m_renderables.clear();
}

void SkinedRenderQueue::setShaderProgram(ShaderProgram* s)
{
	LordAssert(s);
	m_shaderProgram = s;
	m_sPMatMVPIndex = m_shaderProgram->getParamPhysicsIndex("matWVP");
	m_spMatWorldIndex = m_shaderProgram->getParamPhysicsIndex("matWorld");
	m_spVecFogIndex = m_shaderProgram->getParamPhysicsIndex("fogParam");
	m_spFogColorIndex = m_shaderProgram->getParamPhysicsIndex("fogColor");
	m_spCameraPosIndex = m_shaderProgram->getParamPhysicsIndex("cameraPos");
	m_spExtraColorIndex = m_shaderProgram->getParamPhysicsIndex("extraColor");
	m_spBoneMatricesIndex = m_shaderProgram->getParamPhysicsIndex("boneMatRows");
	m_sPTexTextureIndex = m_shaderProgram->getParamPhysicsIndex("texSampler");
}

void SkinedRenderQueue::setSamplerState(const SamplerState::SamplerDesc& desc)
{
	Renderer* pRenderer = Renderer::Instance();
	LordAssert(pRenderer);

	m_samplerState = pRenderer->createSamplerState(desc);
}

void SkinedRenderQueue::setBlendState(const BlendState::BlendDesc& desc)
{
	Renderer* pRenderer = Renderer::Instance();
	LordAssert(pRenderer);

	m_blendState = pRenderer->createBlendState(desc);
}

void SkinedRenderQueue::setDepthState(const DepthStencilState::DepthStencilDesc& desc)
{
	Renderer* pRenderer = Renderer::Instance();
	LordAssert(pRenderer);

	m_depthState = pRenderer->createDepthStencilState(desc);
}

void SkinedRenderQueue::setRasterizerState(const RasterizerState::RasterizerDesc& desc)
{
	Renderer* pRenderer = Renderer::Instance();
	LordAssert(pRenderer);

	m_rasterizerState = pRenderer->createRasterizerState(desc);
}

void SkinedRenderQueue::setRenderInput(const RenderInput::VertexElementList& elements)
{
	Renderer* pRenderer = Renderer::Instance();
	LordAssert(pRenderer);
	LordAssert(m_shaderProgram);

	m_renderInput = pRenderer->createRenderInput(elements, m_shaderProgram);
}

void SkinedRenderQueue::addRenderable(SkinedRenderable* renderable)
{
	LordAssert(renderable);
	if (renderable->getTexture()->getType() == 0) {
		LordAssert(false);
	}
	m_renderables.push_back(renderable);
}

void SkinedRenderQueue::endRender(bool isLine, const Matrix4* viewproj)
{
	//flush all renderalbes to draw.
	if (m_renderables.empty())
		return;

	LordAssert(m_shaderProgram);
	LordAssert(m_renderInput);
	m_renderTexture = NULL;
	Renderer* pRender = Renderer::Instance();
	SceneManager* pSceneManager = SceneManager::Instance();
	m_shaderProgram->bind();

	Camera* camera = SceneManager::Instance()->getMainCamera();
	camera->update();
	Matrix4 viewProjMat = viewproj ? *viewproj : camera->getViewProjMatrix();
	Matrix4 projMat = camera->getProjMatrix();
	Matrix4 worldMat = Matrix4::IDENTITY;
		
	const Vector4 cameraFogParam = Vector4::ZERO;
	const Vector4& fog_params = SceneManager::Instance()->getFogParam();
	const Color& fog_color = SceneManager::Instance()->getFogColor();
	const Vector3* camerapos = camera->getPositionPtr();
	const Color extraColor(1.f, 1.f, 1.f, 1.f);

	SkinedRenderable* pRenderable = m_renderables.front();
	m_shaderProgram->setUniform(m_sPMatMVPIndex, &viewProjMat, SPT_MAT4, 1);
	m_shaderProgram->setUniform(m_spMatWorldIndex, &worldMat, SPT_MAT4, 1);
	m_shaderProgram->setUniform(m_spVecFogIndex, &fog_params, SPT_VEC4, 1);
	m_shaderProgram->setUniform(m_spFogColorIndex, &fog_color, SPT_VEC4, 1);
	m_shaderProgram->setUniform(m_spCameraPosIndex, (void*)camerapos, SPT_VEC3, 1);
	m_shaderProgram->setUniform(m_spExtraColorIndex, &extraColor, SPT_VEC4, 1);
	m_shaderProgram->setUniform(m_sPTexTextureIndex, (void*)&TEXTURE_UNIT0, SPT_TEXTURE, 1);
	m_shaderProgram->setUniform(m_spBoneMatricesIndex, pRenderable->getBoneMatrices(), SPT_VEC4, pRenderable->getBoneCount() * 3);
	m_shaderProgram->bindUniforms();

	pRender->setDepthStencilState(m_depthState);
	pRender->setRasterizerState(m_rasterizerState);
	pRender->setBlendState(m_blendState);

	// batch for all the same texture's skinned. max 60 bones.
	if (m_renderGroup == SRP_SOLID)
		batch();
	
	for (EntityRenderList::iterator it = m_renderables.begin(); it != m_renderables.end(); ++it)
	{
		pRenderable = *it;
		LordAssert(pRenderable);

		Vector4* boneMats = pRenderable->getBoneMatrices();
		int boneCount = pRenderable->getBoneCount();
		Matrix4* worldMat = pRenderable->getWorldMatrix();
		Matrix4 worldViewMat;
		const Vector4* pFogParam = &fog_params;
		if (pRenderable->isViewSpace())
		{
			const Matrix4& viewBobing = TessManager::Instance()->getViewBobing();
			projMat = camera->getProjMatrix2();
			worldViewMat = viewBobing * (*worldMat) * projMat;
			pFogParam = &cameraFogParam;
		}
		else
			worldViewMat = (*worldMat) * viewProjMat;
		
		// set the unifroms.
		m_shaderProgram->setUniform(m_sPMatMVPIndex, &worldViewMat, SPT_MAT4, 1);
		m_shaderProgram->setUniform(m_spVecFogIndex, pFogParam, SPT_VEC4, 1);
		m_shaderProgram->setUniform(m_spMatWorldIndex, worldMat, SPT_MAT4, 1);
		m_shaderProgram->setUniform(m_spExtraColorIndex, &pRenderable->getExtraColor(), SPT_VEC4, 1);
		m_shaderProgram->setUniform(m_spBoneMatricesIndex, boneMats, SPT_VEC4, boneCount * 3);

		m_shaderProgram->bindUniforms();

		_render(pRender, pRenderable, isLine);
	}

	pRender->setTexture(0, NULL);
	pRender->setTexture(1, NULL);
}

BatchRenderable* SkinedRenderQueue::batchPass(Texture* tex)
{
	BatchRenderable* pBatchRenderable = TessManager::Instance()->createBatchRenderable(tex, STRP_SOLID);
	TessManager::Instance()->getBatchRenderQueue(STRP_SOLID)->addRenderable(pBatchRenderable);
	pBatchRenderable->beginPolygon();
	return pBatchRenderable;
}

void SkinedRenderQueue::batch()
{
	EntityRenderList unBatchs;
	EntityRenderBatch batchs;
	SkinedRenderable* pRenderable = nullptr;

	// first, Statistics how many batch needed.
	for (auto it : m_renderables)
	{
		pRenderable = it;
		if (pRenderable->needBatch() && (!pRenderable->isViewSpace()))
		{
			Texture* pTex = pRenderable->getTexture();
			LordAssert(pTex);
			batchs[pTex].push_back(pRenderable);
		}
		else
		{
			unBatchs.push_back(pRenderable);
		}
	}
	// add to m_renderables.
	m_renderables.clear();
	m_renderables.insert(m_renderables.end(), unBatchs.begin(), unBatchs.end());

	BatchRenderable* pBatchRenderable = nullptr;
	// begin to batch!
	for (auto it : batchs)
	{
		pBatchRenderable = batchPass(it.first);

		int boneRaime = 60;
		for (auto jt : it.second)
		{
			pRenderable = jt;
			int quadNumNeeded = pRenderable->getQuadNum();
			int boneCnt = pRenderable->getBoneCount();
			if (pBatchRenderable->getFreeQuadNum() < quadNumNeeded)
			{
				pBatchRenderable->endPolygon();
				pBatchRenderable = batchPass(it.first);
			}

			// dump all the skinedRenderables's vertices to batchRenderable
			BatchVertexFmt* pVertDst = NULL;
			pBatchRenderable->getLockPtr(quadNumNeeded, pVertDst);
			pRenderable->batch(pVertDst);
		}
		pBatchRenderable->endPolygon();
	}
}

void SkinedRenderQueue::_render(Renderer* pRender, SkinedRenderable* pRenderable, bool isLine)
{
	LordAssert(pRender);
	LordAssert(pRenderable);
    
	ui16 quadNum = pRenderable->getQuadNum();
	if (quadNum == 0)
		return;

	Texture* texture = pRenderable->getTexture();
	if (texture != m_renderTexture)
	{
		pRender->setTexture(0, texture);
		texture->setSamplerState(m_samplerState);
		m_renderTexture = texture;
	}

	m_renderInput->unbindGPUBuffers();
	m_renderInput->bindGPUBuffers(pRenderable->getVertexBuffer(), sizeof(TessVertexFmt), pRenderable->getIndexBuffer(), sizeof(ui16));
	m_renderInput->setVertexCount(quadNum * 4);
	m_renderInput->setIndexCount(quadNum * (isLine ? 8 : 6));
	m_renderInput->setTopologyType(isLine ? RenderInput::TT_LINELIST : RenderInput::TT_TRIANGLELIST);
	pRender->render(m_renderInput);
}


}
