#include "Core.h"
#include "ItemRenderQueue.h"
#include "ItemRenderable.h"
#include "TessManager.h"

namespace LORD
{

ItemRenderQueue::ItemRenderQueue(ITEM_RENDER_PASS rg)
	: m_renderInput(NULL)
	, m_shaderProgram(NULL)
	//, m_samplerState(NULL)
	, m_blendState(NULL)
	, m_depthState(NULL)
	, m_rasterizerState(NULL)
	, m_sPMatMVPIndex(0)
	, m_spMatWorldIndex(0)
	, m_spVecFogIndex(0)
	, m_spCameraPosIndex(0)
	, m_spExtraColorIndex(0)
	, m_spFogColorIndex(0)
	, m_renderGroup(rg)
{ }

ItemRenderQueue::~ItemRenderQueue()
{
	if (m_renderInput)
		m_renderInput->unbindGPUBuffers();

	LordSafeDelete(m_renderInput);
	// LordSafeDelete(m_samplerState);
	LordSafeDelete(m_blendState);
	LordSafeDelete(m_depthState);
	LordSafeDelete(m_rasterizerState);
}

void ItemRenderQueue::setShaderProgram(ShaderProgram* s)
{
	LordAssert(s);
	m_shaderProgram = s;
	m_sPMatMVPIndex = m_shaderProgram->getParamPhysicsIndex("matWVP");
	m_spMatWorldIndex = m_shaderProgram->getParamPhysicsIndex("matWorld");
	m_spVecFogIndex = m_shaderProgram->getParamPhysicsIndex("fogParam");
	m_spCameraPosIndex = m_shaderProgram->getParamPhysicsIndex("cameraPos");
	m_spExtraColorIndex = m_shaderProgram->getParamPhysicsIndex("extraColor");
	m_spFogColorIndex = m_shaderProgram->getParamPhysicsIndex("fogColor");
}

//void ItemRenderQueue::setSamplerState(const SamplerState::SamplerDesc& desc)
//{
//	Renderer* pRenderer = Renderer::Instance();
//	LordAssert(pRenderer);
//
//	m_samplerState = pRenderer->createSamplerState(desc);
//}

void ItemRenderQueue::setBlendState(const BlendState::BlendDesc& desc)
{
	Renderer* pRenderer = Renderer::Instance();
	LordAssert(pRenderer);

	m_blendState = pRenderer->createBlendState(desc);
}

void ItemRenderQueue::setDepthState(const DepthStencilState::DepthStencilDesc& desc)
{
	Renderer* pRenderer = Renderer::Instance();
	LordAssert(pRenderer);

	m_depthState = pRenderer->createDepthStencilState(desc);
}

void ItemRenderQueue::setRasterizerState(const RasterizerState::RasterizerDesc& desc)
{
	Renderer* pRenderer = Renderer::Instance();
	LordAssert(pRenderer);

	m_rasterizerState = pRenderer->createRasterizerState(desc);
}

void ItemRenderQueue::setRenderInput(const RenderInput::VertexElementList& elements)
{
	Renderer* pRenderer = Renderer::Instance();
	LordAssert(pRenderer);
	LordAssert(m_shaderProgram);

	m_renderInput = pRenderer->createRenderInput(elements, m_shaderProgram);
}

void ItemRenderQueue::addRenderable(ItemRenderable* renderable)
{
	LordAssert(renderable);
	LordAssert(renderable->getMesh());
	m_renderables.push_back(renderable);
}

void ItemRenderQueue::endRender(bool isLine, const Matrix4* viewproj)
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
	
	Vector4 cameraFogParam = Vector4::ZERO;
	const Vector3* camerapos = camera->getPositionPtr();
	const Vector4* fogParam = SceneManager::Instance()->getFogParamVec4Ptr();
	const Vector3& mainLightDir = SceneManager::Instance()->getMainLightDir();
	const Color& mainLightColor = SceneManager::Instance()->getMainLightColor();
	const Color& subLightColor = SceneManager::Instance()->getSubLightColor();
	const Color& fog_color = SceneManager::Instance()->getFogColor();
	Color extraColor(1.f, 1.f, 1.0f, 1.f);

	ItemRenderable* pRenderable = m_renderables.front();
	m_shaderProgram->setUniform(m_sPMatMVPIndex, &viewProjMat, SPT_MAT4, 1);
	m_shaderProgram->setUniform(m_spMatWorldIndex, &worldMat, SPT_MAT4, 1);
	m_shaderProgram->setUniform(m_spVecFogIndex, (void*)fogParam, SPT_VEC4, 1);
	m_shaderProgram->setUniform(m_spCameraPosIndex, (void*)camerapos, SPT_VEC3, 1);
	m_shaderProgram->setUniform(m_spExtraColorIndex, &extraColor, SPT_VEC4, 1);
	m_shaderProgram->setUniform(m_spFogColorIndex, &fog_color, SPT_VEC4, 1);
	m_shaderProgram->bindUniforms();

	pRender->setDepthStencilState(m_depthState);
	pRender->setRasterizerState(m_rasterizerState);
	pRender->setBlendState(m_blendState);


	for (ItemRenderList::iterator it = m_renderables.begin(); it != m_renderables.end(); ++it)
	{
		pRenderable = *it;
		LordAssert(pRenderable);
		if (!pRenderable->getMesh()) // temp fix crash bug	
			continue;

		LordAssert(pRenderable->getMesh());

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
			worldViewMat = (worldMat) * viewProjMat;

		// set the unifroms.
		m_shaderProgram->setUniform(m_sPMatMVPIndex, &worldViewMat, SPT_MAT4, 1);
		m_shaderProgram->setUniform(m_spVecFogIndex, pFogParam, SPT_VEC4, 1);
		m_shaderProgram->setUniform(m_spMatWorldIndex, &worldMat, SPT_MAT4, 1);
		m_shaderProgram->setUniform(m_spExtraColorIndex, &(pRenderable->getBrightness()), SPT_VEC4, 1);

		m_shaderProgram->bindUniforms();

		_render(pRender, pRenderable->getMesh(), isLine);
	}

	pRender->setTexture(0, NULL);
	pRender->setTexture(1, NULL);
}

void ItemRenderQueue::_render(Renderer* pRender, ItemRenderMesh* pItemMesh, bool isLine)
{
	LordAssert(pRender);
	LordAssert(pItemMesh);

	ui16 quadNum = pItemMesh->getQuadNum();
	if (quadNum == 0)
		return;

	m_renderInput->unbindGPUBuffers();
	m_renderInput->bindGPUBuffers(pItemMesh->getVertexBuffer(), sizeof(TessVertexFmt), pItemMesh->getIndexBuffer(), sizeof(ui16));
	m_renderInput->setVertexCount(quadNum * 4);
	m_renderInput->setIndexCount(quadNum * (isLine ? 8 : 6));
	m_renderInput->setTopologyType(isLine ? RenderInput::TT_LINELIST : RenderInput::TT_TRIANGLELIST);
	pRender->render(m_renderInput);
}

}