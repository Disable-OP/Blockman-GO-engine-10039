
#include "RenderStage.h"
#include "RenderQueue.h"
#include "RenderTarget.h"
#include "Material.h"
#include "ShaderProgramManager.h"
#include "Util/QueryObject.h"
#include "Effect/EffectSystemManager.h"

namespace LORD
{
	RenderStage::RenderStage(RenderStageID stageID)
		: m_rsID(stageID)
		, m_bEnable(false)
		, m_bInitialized(false)
	{

	}

	RenderStage::~RenderStage()
	{
	}

	EditorBackgroundRenderStage::EditorBackgroundRenderStage()
		: RenderStage(RSI_EditorBackground)
	{

	}

	EditorBackgroundRenderStage::~EditorBackgroundRenderStage()
	{
		destroy();
	}

	bool EditorBackgroundRenderStage::initialize()
	{
		return true;
	}

	void EditorBackgroundRenderStage::destroy()
	{
	}

	void EditorBackgroundRenderStage::render()
	{
		return;
	}

	LDRSceneColorRenderStage::LDRSceneColorRenderStage()
		: RenderStage(RSI_LDRSceneColor)
	{

	}

	LDRSceneColorRenderStage::~LDRSceneColorRenderStage()
	{
		destroy();
	}

	bool LDRSceneColorRenderStage::initialize()
	{
		return true;
	}

	void LDRSceneColorRenderStage::destroy()
	{

	}

	void LDRSceneColorRenderStage::render()
	{
		LordAssert(SceneManager::Instance());
		LordAssert(RenderTargetManager::Instance());

		RenderTargetManager::Instance()->beginRenderTarget(RTI_LDRSceneColorMap);

		SceneManager::Instance()->renderScene();

		if (EffectSystemManager::Instance()->getEnableDistortionRender()
			&& RenderTargetManager::Instance()->getRenderTargetByID(RTI_SceneColorMap))
		{

			RenderTargetManager::Instance()->endRenderTarget(RTI_LDRSceneColorMap);

			if (Root::Instance()->getEnableFilterAdditional())
				RenderTargetManager::Instance()->updateRenderTargetWithFilter(RTI_LDRSceneColorMap, RTI_SceneColorMap);
			else
				RenderTargetManager::Instance()->updateRenderTarget(RTI_LDRSceneColorMap, RTI_SceneColorMap);

			RenderTargetManager::Instance()->beginRenderTarget(RTI_LDRSceneColorMap, false, Renderer::BGCOLOR, false, 1.0);

			EffectSystemManager::Instance()->renderDistortionLayer();
			EffectSystemManager::Instance()->renderDistortionedEffectLayer();
		}

		if (QueryObjectManager::Instance())
		{
			QueryObjectManager::Instance()->render();
		}

		RenderTargetManager::Instance()->endRenderTarget(RTI_LDRSceneColorMap);

	}

	PostProcessRenderStage::PostProcessRenderStage()
		: RenderStage(RSI_PostProcess)
		, m_pScreenAlignedQuadIB(0)
		, m_pScreenAlignedQuadVB(0)
	{
	}

	PostProcessRenderStage::~PostProcessRenderStage()
	{
		destroy();
	}

	ui32 PostProcessRenderStage::GaussianBlurringHelper::m_width = 0;
	ui32 PostProcessRenderStage::GaussianBlurringHelper::m_height = 0;
	float PostProcessRenderStage::GaussianBlurringHelper::m_gaussianMean = 0.0f; // -1.0f -- 1.0f
	float PostProcessRenderStage::GaussianBlurringHelper::m_gaussianDev = 0.8f;  // 0.0f -- 1.0f
	float PostProcessRenderStage::GaussianBlurringHelper::m_gaussianMultiplier = 0.43f;  // 0.0f -- 2.0f
	Vector4 PostProcessRenderStage::GaussianBlurringHelper::m_horizonFilter[4];
	Vector4 PostProcessRenderStage::GaussianBlurringHelper::m_verticalFilter[4];
	Vector4 PostProcessRenderStage::GaussianBlurringHelper::m_blurringFilter[4];
	Material* PostProcessRenderStage::GaussianBlurringHelper::m_pMtlGaussianBlurring = 0;
	Renderable* PostProcessRenderStage::GaussianBlurringHelper::m_pRaGaussianBlurring = 0;

	bool PostProcessRenderStage::GaussianBlurringHelper::initialize(PostProcessRenderStage* pprs)
	{
		// init pp_gaussiansample material
		ShaderProgram* pShaderProgram = ShaderProgramManager::Instance()->getShaderProgram("GaussianBlur");
		m_pMtlGaussianBlurring = LordNew Material; LordAssert(m_pMtlGaussianBlurring);
		m_pRaGaussianBlurring = LordNew Renderable();
		m_pMtlGaussianBlurring->setShaderProgram(pShaderProgram);
		m_pRaGaussianBlurring->setMaterial(m_pMtlGaussianBlurring);

		DepthStencilState::DepthStencilDesc depthdesc = m_pMtlGaussianBlurring->getDepthState()->getDesc();
		depthdesc.bWriteDepth = false;
		depthdesc.bDepthEnable = false;

		SamplerState::SamplerDesc samdesc = m_pMtlGaussianBlurring->getSamplerState(0)->getDesc();
		samdesc.mipFilter = SamplerState::FO_NONE;
		samdesc.addrUMode = SamplerState::AM_CLAMP;
		samdesc.addrVMode = SamplerState::AM_CLAMP;

		m_pMtlGaussianBlurring->setDepthState(depthdesc);
		m_pMtlGaussianBlurring->setSamplerState(0, samdesc);

		RenderInput* pRenderInput = Renderer::Instance()->createRenderInput(RenderTargetManager::Instance()->getVertexElement(), m_pMtlGaussianBlurring->getShaderProgram());
		pRenderInput->bindGPUBuffers(pprs->m_pScreenAlignedQuadVB, sizeof(float) * 5, pprs->m_pScreenAlignedQuadIB, sizeof(ui16));
		pRenderInput->setTopologyType(RenderInput::TT_TRIANGLELIST);
		m_pRaGaussianBlurring->setRenderInput(pRenderInput);

		m_pMtlGaussianBlurring->beginShaderParams(2);
		m_pMtlGaussianBlurring->setShaderParam("texSampler", SPT_TEXTURE, (void*)&TEXTURE_UNIT0);
		m_pMtlGaussianBlurring->setShaderParam("gaussianFilter", SPT_VEC4, (void*)&GaussianBlurringHelper::m_blurringFilter[0], 4);
		m_pMtlGaussianBlurring->endShaderParams();
		return true;
	}

	void PostProcessRenderStage::GaussianBlurringHelper::destroy()
	{
		LordSafeDelete(m_pMtlGaussianBlurring);
		RenderInput* pRi = m_pRaGaussianBlurring->getRenderInput();
		LordSafeDelete(pRi);
		LordSafeDelete(m_pRaGaussianBlurring);
	}

	void PostProcessRenderStage::destroy()
	{
		if (m_bInitialized)
		{
			GaussianBlurringHelper::destroy();

			LordSafeDelete(m_pScreenAlignedQuadIB);
			LordSafeDelete(m_pScreenAlignedQuadVB);
		}
	}

	void PostProcessRenderStage::GaussianBlurringHelper::horizonBlurring(RenderTargetID _srcID, RenderTargetID _dstID)
	{
		LordAssert(RenderTargetManager::Instance()->checkRenderTargetIDValid(_srcID));
		LordAssert(RenderTargetManager::Instance()->checkRenderTargetIDValid(_dstID));

		RenderTargetManager::Instance()->beginRenderTarget(_dstID, false, Renderer::BGCOLOR, false);

		RenderTarget* pSrcRt = RenderTargetManager::Instance()->getRenderTargetByID(_srcID);

		Texture* pSrcTex = pSrcRt->getBindTexture();
		pSrcTex->setSamplerState(m_pMtlGaussianBlurring->getSamplerStateByTexStage(0));
		Renderer::Instance()->setTexture(0, pSrcTex);

		calculateBlurringFilter(pSrcRt->width(), pSrcRt->width());

		enableHorizonFiter();

		m_pMtlGaussianBlurring->active();

		Renderer::Instance()->render(m_pRaGaussianBlurring->getRenderInput());

		RenderTargetManager::Instance()->endRenderTarget(_dstID);

	}

	void PostProcessRenderStage::GaussianBlurringHelper::verticalBlurring(RenderTargetID _srcID, RenderTargetID _dstID)
	{
		LordAssert(RenderTargetManager::Instance()->checkRenderTargetIDValid(_srcID));
		LordAssert(RenderTargetManager::Instance()->checkRenderTargetIDValid(_dstID));

		RenderTargetManager::Instance()->beginRenderTarget(_dstID, false, Renderer::BGCOLOR, false);

		RenderTarget* pSrcRt = RenderTargetManager::Instance()->getRenderTargetByID(_srcID);

		Texture* pSrcTex = pSrcRt->getBindTexture();
		pSrcTex->setSamplerState(m_pMtlGaussianBlurring->getSamplerStateByTexStage(0));
		Renderer::Instance()->setTexture(0, pSrcTex);

		calculateBlurringFilter(pSrcRt->width(), pSrcRt->width());

		enableVerticalFiter();

		m_pMtlGaussianBlurring->active();

		Renderer::Instance()->render(m_pRaGaussianBlurring->getRenderInput());

		RenderTargetManager::Instance()->endRenderTarget(_dstID);
	}

	bool PostProcessRenderStage::initialize()
	{
		if (m_bEnable)
		{
			float vertex_buffer[] = { -1.f, -1.f, 0.0f, 0.f, 0.f,
				1.f, -1.f, 0.0f, 1.f, 0.f,
				1.f, 1.f, 0.0f, 1.f, 1.f,
				-1.f, 1.f, 0.0f, 0.f, 1.f };

			ui16 index_buffer[] = { 0, 3, 2, 0, 2, 1 };

			Buffer bufferVertices(sizeof(vertex_buffer), vertex_buffer);
			Buffer bufferIndices(sizeof(index_buffer), index_buffer);
			m_pScreenAlignedQuadVB = Renderer::Instance()->createVertexBuffer(GPUBuffer::GBU_GPU_READ, bufferVertices);
			m_pScreenAlignedQuadIB = Renderer::Instance()->createIndexBuffer(GPUBuffer::GBU_GPU_READ, bufferIndices);

			GaussianBlurringHelper::initialize(this);

			m_bInitialized = true;
		}
		return true;
	}

	void PostProcessRenderStage::render()
	{
		if (m_bEnable)
		{
			if (Root::Instance()->getEnableFilterAdditional())
				RenderTargetManager::Instance()->updateRenderTargetWithFilter(RTI_LDRSceneColorMap, RTI_DefaultBackBuffer);
			else
				RenderTargetManager::Instance()->updateRenderTarget(RTI_LDRSceneColorMap, RTI_DefaultBackBuffer, true, true);
		}
	}

	UIRenderStage::UIRenderStage()
		: RenderStage(RSI_UI)
	{

	}

	UIRenderStage::~UIRenderStage()
	{
		destroy();
	}

	bool UIRenderStage::initialize()
	{
		return true;
	}

	void UIRenderStage::destroy()
	{

	}

	void UIRenderStage::render()
	{
		LordAssert(SceneManager::Instance());

		RenderQueue* pRenderQueue = SceneManager::Instance()->getRenderQueue("UI");

		if (!pRenderQueue) return;

		RenderTargetManager::Instance()->beginRenderTarget(RTI_LDRSceneColorMap, false, Renderer::BGCOLOR, true, 1.0);

		pRenderQueue->renderQueue();

		pRenderQueue->beginRender();

		RenderTargetManager::Instance()->endRenderTarget(RTI_LDRSceneColorMap);
	}


	RenderStageManager::RenderStageManager()
	{
		RenderStage* pRs = 0;

		pRs = LordNew LDRSceneColorRenderStage;
		pRs->setEnable(true);
		m_renderStages.push_back(pRs);

		pRs = LordNew UIRenderStage;
		pRs->setEnable(true);
		m_renderStages.push_back(pRs);

		pRs = LordNew PostProcessRenderStage;
		pRs->setEnable(true);
		m_renderStages.push_back(pRs);

		m_BlurSceneTarget = false;
	}

	RenderStageManager::~RenderStageManager()
	{
		vector<RenderStage*>::type::iterator bit = m_renderStages.begin();

		for (; bit != m_renderStages.end(); )
		{
			LordSafeDelete(*bit);

			bit = m_renderStages.erase(bit);
		}
	}

	bool RenderStageManager::initialize()
	{
		vector<RenderStage*>::type::iterator bit = m_renderStages.begin();

		for (; bit != m_renderStages.end(); ++bit)
		{
			(*bit)->initialize();
		}

		return true;
	}

	void RenderStageManager::processRenderStages()
	{
		for (size_t i = 0; i < m_renderStages.size(); ++i)
		{
			m_renderStages[i]->render();
		}
	}

	Texture* RenderStageManager::blurLDRSceneTarget(bool blur)
	{
		if (blur)
		{
			m_BlurSceneTarget = false;
			Root::Instance()->frameMove(Root::Instance()->getFrameTime());
			m_renderStages[0]->render();
			//m_renderStages[1]->render();
			m_BlurSceneTarget = true;
			RenderTargetManager::Instance()->updateRenderTarget(RTI_LDRSceneColorMap, RTI_LDRSceneColorMap_Half, true, true);
			PostProcessRenderStage::GaussianBlurringHelper::horizonBlurring(RTI_LDRSceneColorMap_Half, RTI_HorizonBlurringMap_Quater);
			PostProcessRenderStage::GaussianBlurringHelper::verticalBlurring(RTI_HorizonBlurringMap_Quater, RTI_VerticalBlurringMap_Quater);
			RenderTargetManager::Instance()->updateRenderTarget(RTI_VerticalBlurringMap_Quater, RTI_LDRSceneColorMap_Half, true, true);
			return RenderTargetManager::Instance()->getRenderTargetByID(RTI_LDRSceneColorMap_Half)->getBindTexture();
		}
		else
		{
			m_BlurSceneTarget = false;
		}
		return NULL;
	}

}