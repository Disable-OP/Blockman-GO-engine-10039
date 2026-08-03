#include "Core.h"
#include "EnchantRenderQueue.h"
#include "ItemRenderable.h"
#include "SkinedRenderable.h"
#include "TessManager.h"
#include "Scene/SceneManager.h"

namespace LORD
{

EnchantItemRenderQueue::EnchantItemRenderQueue()
	: ItemRenderQueue(IRP_SOLID)
	, m_spMatTextureIndex1(-1)
	, m_spMatTextureIndex2(-1)
	, m_spTexTextureIndex(-1)
	, m_renderTexture(NULL)
{}

EnchantItemRenderQueue::~EnchantItemRenderQueue()
{
	LordSafeDelete(m_samplerState);
	LordSafeDelete(m_depthState_noDepth);
}

void EnchantItemRenderQueue::setSamplerState(const SamplerState::SamplerDesc& desc)
{
	Renderer* pRenderer = Renderer::Instance();
	LordAssert(pRenderer);

	m_samplerState = pRenderer->createSamplerState(desc);
}

void EnchantItemRenderQueue::setDepthState_noDepth(const DepthStencilState::DepthStencilDesc& desc)
{
	Renderer* pRenderer = Renderer::Instance();
	LordAssert(pRenderer);

	m_depthState_noDepth = pRenderer->createDepthStencilState(desc);
}

void EnchantItemRenderQueue::setShaderProgram(ShaderProgram* s)
{
	ItemRenderQueue::setShaderProgram(s);
	m_spMatTextureIndex1 = m_shaderProgram->getParamPhysicsIndex("matTexture1");
	m_spMatTextureIndex2 = m_shaderProgram->getParamPhysicsIndex("matTexture2");
	m_spTexTextureIndex = m_shaderProgram->getParamPhysicsIndex("texSampler");
}

void EnchantItemRenderQueue::endRender(bool isLine, const Matrix4* viewproj)
{
	//flush all renderalbes to draw.
	if (m_renderables.empty())
		return;

	LordAssert(m_shaderProgram);
	LordAssert(m_renderInput);
	Renderer* pRender = Renderer::Instance();
	SceneManager* pSceneManager = SceneManager::Instance();
	m_shaderProgram->bind();

	Camera* camera = SceneManager::Instance()->getMainCamera();
	camera->update();
	Matrix4 viewProjMat = viewproj ? *viewproj : camera->getViewProjMatrix();
	Matrix4 projMat = camera->getProjMatrix();
	Matrix4 worldMat = Matrix4::IDENTITY;
	const Vector4* fogParam = SceneManager::Instance()->getFogParamVec4Ptr();
	Vector4 cameraFogParam = Vector4::ZERO;
	const Vector3* camerapos = camera->getPositionPtr();
	const Vector3& mainLightDir = SceneManager::Instance()->getMainLightDir();
	const Color& mainLightColor = SceneManager::Instance()->getMainLightColor();
	const Vector3& subLightDir = SceneManager::Instance()->getSubLightDir();
	const Color& subLightColor = SceneManager::Instance()->getSubLightColor();
	const Color& amibentColor = SceneManager::Instance()->getAmbientColor();
	const Matrix4& enchantTex1 = TessManager::Instance()->getEnchantTexMat(0);
	const Matrix4& enchantTex2 = TessManager::Instance()->getEnchantTexMat(1);
	const Color& fog_color = SceneManager::Instance()->getFogColor();
	Color extraColor(1.f, 1.f, 1.f, 1.f);
	
	m_shaderProgram->setUniform(m_sPMatMVPIndex, &viewProjMat, SPT_MAT4, 1);
	m_shaderProgram->setUniform(m_spMatWorldIndex, &worldMat, SPT_MAT4, 1);
	m_shaderProgram->setUniform(m_spMatTextureIndex1, &enchantTex1, SPT_MAT4, 1);
	m_shaderProgram->setUniform(m_spMatTextureIndex2, &enchantTex2, SPT_MAT4, 1);
	m_shaderProgram->setUniform(m_spVecFogIndex, (void*)fogParam, SPT_VEC4, 1);
	m_shaderProgram->setUniform(m_spCameraPosIndex, (void*)camerapos, SPT_VEC3, 1);
	m_shaderProgram->setUniform(m_spExtraColorIndex, &extraColor, SPT_VEC4, 1);
	m_shaderProgram->setUniform(m_spFogColorIndex, &fog_color, SPT_VEC4, 1);
	m_shaderProgram->setUniform(m_spTexTextureIndex, (void*)&TEXTURE_UNIT0, SPT_TEXTURE, 1);
	m_shaderProgram->bindUniforms();

	pRender->setDepthStencilState(m_depthState);
	pRender->setRasterizerState(m_rasterizerState);
	pRender->setBlendState(m_blendState);

	ItemRenderable* pRenderable = nullptr;
	for (ItemRenderList::iterator it = m_renderables.begin(); it != m_renderables.end(); ++it)
	{
		pRenderable = *it;
		LordAssert(pRenderable);
		LordAssert(pRenderable->getMesh());

		if (pRenderable->getRenderGroup() == IRP_ENCHANT_NODEPTH)
			pRender->setDepthStencilState(m_depthState_noDepth);
		else
			pRender->setDepthStencilState(m_depthState);

		const Matrix4& worldMat = pRenderable->getWorldMatrix();
		Matrix4 worldViewMat;
		const Vector4* pFogParam = fogParam;
		if (pRenderable->isViewSpace())
		{
			const Matrix4& viewBobing = TessManager::Instance()->getViewBobing();
			projMat = camera->getProjMatrix2();
			worldViewMat = viewBobing * worldMat * projMat;
			pFogParam = &cameraFogParam;
		}
		else
			worldViewMat = (worldMat)* viewProjMat;

		// set the unifroms.
		m_shaderProgram->setUniform(m_sPMatMVPIndex, &worldViewMat, SPT_MAT4, 1);
		m_shaderProgram->setUniform(m_spVecFogIndex, pFogParam, SPT_VEC4, 1);
		m_shaderProgram->setUniform(m_spMatWorldIndex, &worldMat, SPT_MAT4, 1);
		m_shaderProgram->setUniform(m_spExtraColorIndex, &(pRenderable->getBrightness()), SPT_VEC4, 1);

		m_shaderProgram->bindUniforms();

		_render(pRender, pRenderable, isLine);
	}

	pRender->setTexture(0, NULL);
	pRender->setTexture(1, NULL);
	m_renderTexture = NULL;
}

void EnchantItemRenderQueue::_render(Renderer* pRender, ItemRenderable* pRenderable, bool isLine)
{
	LordAssert(pRender);
	LordAssert(pRenderable);

	ItemRenderMesh* pMesh = pRenderable->getMesh();
	if (!pMesh)
		return;

	ui16 quadNum = pMesh->getQuadNum();
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
	m_renderInput->bindGPUBuffers(pMesh->getVertexBuffer(), sizeof(TessVertexFmt), pMesh->getIndexBuffer(), sizeof(ui16));
	m_renderInput->setVertexCount(quadNum * 4);
	m_renderInput->setIndexCount(quadNum * (isLine ? 8 : 6));
	m_renderInput->setTopologyType(isLine ? RenderInput::TT_LINELIST : RenderInput::TT_TRIANGLELIST);
	pRender->render(m_renderInput);
}


EnchantSkinedRenderQueue::EnchantSkinedRenderQueue()
	: SkinedRenderQueue(SRP_SOLID)
	, m_spMatTextureIndex1(-1)
	, m_spMatTextureIndex2(-1)
	, m_spVecTextureAtlas(-1)
	, m_depthState_noDepth(NULL)
{}

EnchantSkinedRenderQueue::~EnchantSkinedRenderQueue()
{
	LordSafeDelete(m_depthState_noDepth);
}

void EnchantSkinedRenderQueue::setDepthState_noDepth(const DepthStencilState::DepthStencilDesc& desc)
{
	Renderer* pRenderer = Renderer::Instance();
	LordAssert(pRenderer);

	m_depthState_noDepth = pRenderer->createDepthStencilState(desc);
}

void EnchantSkinedRenderQueue::setShaderProgram(ShaderProgram* s)
{
	SkinedRenderQueue::setShaderProgram(s);
	m_spMatTextureIndex1 = m_shaderProgram->getParamPhysicsIndex("matTexture1");
	m_spMatTextureIndex2 = m_shaderProgram->getParamPhysicsIndex("matTexture2");
	m_spVecTextureAtlas = m_shaderProgram->getParamPhysicsIndex("texAtlas");
}

void EnchantSkinedRenderQueue::endRender(bool isLine, const Matrix4* viewproj)
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
	const Vector4 texAtlas = Vector4::ONE;
	
	const Vector4 cameraFogParam = Vector4::ZERO;
	const Vector4& fog_params = SceneManager::Instance()->getFogParam();
	const Color& fog_color = SceneManager::Instance()->getFogColor();
	const Vector3* camerapos = camera->getPositionPtr();
	const Color extraColor(1.f, 1.f, 1.f, 1.f);

	const Matrix4& enchantTex1 = TessManager::Instance()->getEnchantTexMat(0);
	const Matrix4& enchantTex2 = TessManager::Instance()->getEnchantTexMat(1);

	EnchantSkinedRenderable* pRenderable = (EnchantSkinedRenderable*)(m_renderables.front());
	m_shaderProgram->setUniform(m_sPMatMVPIndex, &viewProjMat, SPT_MAT4, 1);
	m_shaderProgram->setUniform(m_spMatWorldIndex, &worldMat, SPT_MAT4, 1);
	m_shaderProgram->setUniform(m_spMatTextureIndex1, &enchantTex1, SPT_MAT4, 1);
	m_shaderProgram->setUniform(m_spMatTextureIndex2, &enchantTex2, SPT_MAT4, 1);
	m_shaderProgram->setUniform(m_spVecTextureAtlas, &texAtlas, SPT_VEC4, 1);

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


	for (EntityRenderList::iterator it = m_renderables.begin(); it != m_renderables.end(); ++it)
	{
		pRenderable = (EnchantSkinedRenderable*)(*it);
		LordAssert(pRenderable);

		if (pRenderable->getRenderGroup() == SRP_ENCHANT_NODEPTH)
			pRender->setDepthStencilState(m_depthState_noDepth);
		else
			pRender->setDepthStencilState(m_depthState);

		Vector4* boneMats = pRenderable->getBoneMatrices();
		int boneCount = pRenderable->getBoneCount();
		Matrix4* worldMat = pRenderable->getWorldMatrix();
		Matrix4 worldViewMat;
		const Vector4& atlas = pRenderable->getTextureAtlas();
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
		m_shaderProgram->setUniform(m_spVecTextureAtlas, &atlas, SPT_VEC4, 1);
		m_shaderProgram->setUniform(m_spExtraColorIndex, &pRenderable->getExtraColor(), SPT_VEC4, 1);
		m_shaderProgram->setUniform(m_spBoneMatricesIndex, boneMats, SPT_VEC4, boneCount * 3);

		m_shaderProgram->bindUniforms();

		_render(pRender, pRenderable, isLine);
	}

	pRender->setTexture(0, NULL);
	pRender->setTexture(1, NULL);
}

}