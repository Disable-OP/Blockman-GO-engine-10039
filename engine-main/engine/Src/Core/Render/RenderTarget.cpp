#include "RenderTarget.h"
#include "Render/Material.h"
#include "Effect/EffectSystemManager.h"
#include "RenderStage.h"
#include "ShaderProgramManager.h"

namespace LORD
{

	RenderTarget::RenderTarget(RenderTargetID _id, RenderTargetCreateFlags _rtcf, float _width, float _height, PixelFormat _format, bool _hasDepth /* = false */, bool _msaa /* = false */, bool _multi_resolution)
		: m_id(_id)
		, m_pixelFormat(_format)
		, m_bHasDepth(_hasDepth)
		, m_bHasMSAA(_msaa)
		, m_width(0)
		, m_height(0)
		, m_bFrameBufferChange(0)
		, m_bViewportChange(0)
		, m_bindTexture(0)
		, m_rtcf(_rtcf)
		, m_rtcf_width(_width)
		, m_rtcf_height(_height)
		, m_bLogicResolution(_multi_resolution)
	{
		switch (_rtcf)
		{
		case RTCF_Scale:
			m_width = static_cast<ui32>(m_bLogicResolution ? RenderTargetManager::Instance()->logicWidth() : RenderTargetManager::Instance()->hardwareWidth() * _width);

			if (m_width == 0) m_width = 1;

			m_height = static_cast<ui32>(m_bLogicResolution ? RenderTargetManager::Instance()->logicHeight() : RenderTargetManager::Instance()->hardwareHeight() * _height);

			if (m_height == 0) m_height = 1;

			break;

		case RTCF_Fixed:
			m_width = static_cast<ui32>(_width);
			m_height = static_cast<ui32>(_height);
			break;
		default:
			LordLogError("Unknown Render Target Creation flags ");
			break;
		}
	}

	RenderTarget::~RenderTarget()
	{
		LordSafeDelete(m_bindTexture);
	}


	ui32 RenderTargetManager::m_autoSizeMinWidth(1280);
	ui32 RenderTargetManager::m_autoSizeMinHeight(720);
	ui32 RenderTargetManager::m_autoSizeMaxWidth(1920);
	ui32 RenderTargetManager::m_autoSizeMaxHeight(1080);

	RenderTargetManager::RenderTargetManager()
		: m_inUsingRenderTarget(RTI_End)
		, m_bEnableMultiResolution(false)
		, m_bEnableFilter(false)
		, m_logicWidth(0)
		, m_logicHeight(0)
		, m_hardwareWidth(0)
		, m_hardwareHeight(0)
		, m_logicWidthScale(1.0f)
		, m_logicHeightScale(1.0f)
		, m_pScreenAlignedQuad(NULL)
		, m_pScreenAlignedQuadFilter(NULL)
		, m_pMaterialBaseUpdate(NULL)
		, m_pMaterialFilterUpdate(NULL)
		, m_pFilterBlendmap(NULL)
	{

	}

	RenderTargetManager::~RenderTargetManager()
	{
		m_VertexElement.clear();
		for (RenderTargetMap::iterator bit = m_mapRenderTargets.begin(); bit != m_mapRenderTargets.end(); )
		{
			LordSafeDelete(bit->second);
			m_mapRenderTargets.erase(bit++);
		}

		LordSafeDelete(m_pMaterialBaseUpdate);
		RenderInput* ri = m_pScreenAlignedQuad->getRenderInput();

		GPUBuffer* vertBuffer = ri->getVertexBuffer();

		LordSafeDelete(vertBuffer);

		GPUBuffer* indexBuffer = ri->getIndexBuffer();

		LordSafeDelete(indexBuffer);

		LordSafeDelete(ri);

		LordSafeDelete(m_pScreenAlignedQuad);

		if (m_bEnableFilter)
		{
			LordSafeDelete(m_pMaterialFilterUpdate);
			TextureManager::Instance()->releaseResource(m_pFilterBlendmap);
			m_pFilterBlendmap = NULL;
			RenderInput* _ri = m_pScreenAlignedQuadFilter->getRenderInput();
			GPUBuffer* _vertBuffer = _ri->getVertexBuffer();
			LordSafeDelete(_vertBuffer);
			GPUBuffer* _indexBuffer = _ri->getIndexBuffer();
			LordSafeDelete(_indexBuffer);
			LordSafeDelete(_ri);
			LordSafeDelete(m_pScreenAlignedQuadFilter);
		}
	}

	bool RenderTargetManager::initialize()
	{
		m_hardwareWidth = Renderer::Instance()->getScreenWidth();
		m_hardwareHeight = Renderer::Instance()->getScreenHeight();

		//#if LORD_PLATFORM != LORD_PLATFORM_WINDOWS
		//		m_bEnableMultiResolution = true;
		//
		//		if( m_bEnableMultiResolution )
		//		{
		//			AdaptSize(m_hardwareWidth, m_hardwareHeight, m_logicWidth, m_logicHeight);
		//		
		//			m_logicWidthScale = static_cast<float>(m_logicWidth) / static_cast<float>(m_hardwareWidth);
		//			m_logicHeightScale = static_cast<float>(m_logicHeight) / static_cast<float>(m_hardwareHeight);
		//        }
		//#endif

		RenderInput::VertexElement posElm, uvElm;
		posElm.semantic = RenderInput::VS_POSITION;
		posElm.pixFmt = PF_RGB32_FLOAT;
		uvElm.semantic = RenderInput::VS_TEXCOORD;
		uvElm.pixFmt = PF_RG32_FLOAT;
		m_VertexElement.push_back(posElm);
		m_VertexElement.push_back(uvElm);

		m_pMaterialBaseUpdate = LordNew Material;

		if (storeDefaultRenderTarget())
		{
			LordLogInfo("RenderTargetManager::storeDefaultRenderTarget() ... succeeded");
		}
		else
		{
			LordLogError("storeDefaultRenderTarget() ... Failed ! ");
			return false;
		}

		if (EffectSystemManager::Instance()->getEnableDistortionRender())
		{
			if (NULL != createRenderTarget(RTI_SceneColorMap, RTCF_Scale, 1.f, 1.f, PF_RGBA8_UNORM, true, false, m_bEnableMultiResolution))
			{
				LordLogInfo("RenderTargetManager::createRenderTarget( RTI_SceneColorMap ) ... succeeded");
			}
			else
			{
				LordLogError("RenderTargetManager::createRenderTarget( RTI_SceneColorMap ) ... Failed");
				return false;
			}
		}

		if (NULL != createRenderTarget(RTI_LDRSceneColorMap, RTCF_Scale, 1.0f, 1.0f, PF_RGB8_UNORM, true, false, m_bEnableMultiResolution))
		{
			LordLogInfo("RenderTargetManager::createRenderTarget( RTI_LDRSceneColor ) ... succeeded");
		}
		else
		{
			LordLogError("RenderTargetManager::createRenderTarget( RTI_LDRSceneColor ) ... Failed");
			return false;
		}

		if (NULL != createRenderTarget(RTI_LDRSceneColorMap_Half, RTCF_Scale, 0.5f, 0.5f, PF_RGB8_UNORM, true, false, m_bEnableMultiResolution))
		{
			LordLogInfo("RenderTargetManager::createRenderTarget( RTI_LDRSceneColor ) ... succeeded");
		}
		else
		{
			LordLogError("RenderTargetManager::createRenderTarget( RTI_LDRSceneColor ) ... Failed");
			return false;
		}

		if (NULL != createRenderTarget(RTI_HorizonBlurringMap_Quater, RTCF_Scale, 0.25f, 0.25f, PF_RGB8_UNORM, true, false, m_bEnableMultiResolution))
		{
			LordLogInfo("RenderTargetManager::createRenderTarget( RTI_HorizonBlurringMap_Quater ) ... succeeded");
		}
		else
		{
			LordLogError("RenderTargetManager::createRenderTarget( RTI_HorizonBlurringMap_Quater ) ... Failed");
			return false;
		}

		if (NULL != createRenderTarget(RTI_VerticalBlurringMap_Quater, RTCF_Scale, 0.25f, 0.25f, PF_RGB8_UNORM, true, false, m_bEnableMultiResolution))
		{
			LordLogInfo("RenderTargetManager::createRenderTarget( RTI_VerticalBlurringMap_Quater ) ... succeeded");
		}
		else
		{
			LordLogError("RenderTargetManager::createRenderTarget( RTI_VerticalBlurringMap_Quater ) ... Failed");
			return false;
		}

		if (Root::Instance()->getEnableFilterAdditional())
		{
			m_bEnableFilter = true;

			m_pMaterialFilterUpdate = LordNew Material;

			changeFilterBlendmapName("FilterAdditional.tga");
		}

		if (restoreDefaultRenderTarget())
		{
			LordLogInfo("RenderTargetManager::restoreDefaultRenderTarget() succeeded");
		}
		else
		{
			LordLogInfo("RenderTargetManager::restoreDefaultRenderTarget() Failed !");
			return false;
		}

		if (!createScreenAlignedQuad_ext())
		{
			LordLogInfo("createScreenAlignedQuad_ext() Failed !");
			return false;
		}

		return true;
	}

	void RenderTargetManager::destroyRenderTarget(RenderTargetID _id)
	{
		RenderTargetMap::iterator fit = m_mapRenderTargets.find(_id);

		if (fit == m_mapRenderTargets.end())
		{
			LordLogError("Could not found RenderTarget[%d]", _id);
			return;
		}

		LordSafeDelete(fit->second);
		m_mapRenderTargets.erase(fit);
	}

	bool RenderTargetManager::updateRenderTarget(RenderTargetID _srcID, RenderTargetID _dstID, bool _clear_color /* =false */, bool _clear_depth /* = false */, bool _clear_stencil /* =false */)
	{
		LordAssert(_srcID != RTI_End && _dstID != RTI_End && m_pMaterialBaseUpdate);

		if (_srcID == RTI_DefaultBackBuffer)
		{
			LordLogError("Could not support copy from default back buffer!");
			return false;
		}

		if (!checkRenderTargetIDValid(_srcID) || !checkRenderTargetIDValid(_dstID))
		{
			return false;
		}

		beginRenderTarget(_dstID, _clear_color, Renderer::BGCOLOR, _clear_depth, 1.0f, _clear_stencil, 0);

		Texture* pTexture = getRenderTargetByID(_srcID)->getBindTexture();
		SamplerState* pState = m_pMaterialBaseUpdate->getSamplerStateByTexStage(0);
		pTexture->setSamplerState(pState);

		m_pMaterialBaseUpdate->active();
		Renderer::Instance()->setTexture(0, pTexture);
		Renderer::Instance()->render(m_pScreenAlignedQuad->getRenderInput());

		endRenderTarget(_dstID);
		return true;
	}

	bool RenderTargetManager::updateRenderTargetWithFilter(RenderTargetID _srcID, RenderTargetID _dstID, bool _clear_color /* =false */, bool _clear_depth /* = false */, bool _clear_stencil /* =false */)
	{
		LordAssert(_srcID != RTI_End && _dstID != RTI_End && m_pMaterialBaseUpdate);

		if (_srcID == RTI_DefaultBackBuffer)
		{
			LordLogError("Could not support copy from default back buffer!");
			return false;
		}

		if (!checkRenderTargetIDValid(_srcID) || !checkRenderTargetIDValid(_dstID))
		{
			return false;
		}

		beginRenderTarget(_dstID, _clear_color, Renderer::BGCOLOR, _clear_depth, 1.0f, _clear_stencil, 0);

		SamplerState* pState = m_pMaterialFilterUpdate->getSamplerStateByTexStage(0);
		getRenderTargetByID(_srcID)->getBindTexture()->setSamplerState(pState);
		m_pMaterialFilterUpdate->activeTexture(1, m_pFilterBlendmap);
		m_pMaterialFilterUpdate->active();
		Renderer::Instance()->render(m_pScreenAlignedQuadFilter->getRenderInput());

		endRenderTarget(_dstID);
		return true;
	}

	bool RenderTargetManager::createScreenAlignedQuad_ext()
	{
		float vertex_buffer[] = { -1.f, -1.f, 0.0f, 0.f,  0.f,
			1.f, -1.f, 0.0f, 1.f,  0.f,
			1.f,  1.f, 0.0f, 1.f,  1.f,
			-1.f,  1.f,0.0f, 0.f,  1.f };


		ui16 index_buffer[] = { 0, 3, 2, 0, 2, 1 };

		ShaderProgram* pShaderProgram = ShaderProgramManager::Instance()->getShaderProgram("UpdateRenderTarget");
		m_pScreenAlignedQuad = LordNew Renderable();
		m_pScreenAlignedQuad->setMaterial(m_pMaterialBaseUpdate);
		m_pMaterialBaseUpdate->setShaderProgram(pShaderProgram);
		m_pMaterialBaseUpdate->beginShaderParams(1);
		m_pMaterialBaseUpdate->setShaderParam("texSampler", SPT_TEXTURE, (void*)&TEXTURE_UNIT0);
		m_pMaterialBaseUpdate->endShaderParams();

		{
			BlendState::BlendDesc blenddesc = m_pMaterialBaseUpdate->getBlendState()->getDesc();
			blenddesc.bBlendEnable = false;
			blenddesc.srcBlend = BlendState::BF_SRC_ALPHA;
			blenddesc.dstBlend = BlendState::BF_INV_DST_ALPHA;

			DepthStencilState::DepthStencilDesc depthdesc = m_pMaterialBaseUpdate->getDepthState()->getDesc();
			depthdesc.bWriteDepth = false;
			depthdesc.bDepthEnable = false;

			SamplerState::SamplerDesc samdesc = m_pMaterialBaseUpdate->getSamplerState(0)->getDesc();
			samdesc.minFilter = SamplerState::FO_LINEAR;
			samdesc.magFilter = SamplerState::FO_LINEAR;
			samdesc.mipFilter = SamplerState::FO_NONE;
			samdesc.addrUMode = SamplerState::AM_CLAMP;
			samdesc.addrVMode = SamplerState::AM_CLAMP;

			m_pMaterialBaseUpdate->setBlendState(blenddesc);
			m_pMaterialBaseUpdate->setDepthState(depthdesc);
			m_pMaterialBaseUpdate->setSamplerState(0, samdesc);
		}


		Buffer bufferVertices(sizeof(vertex_buffer), vertex_buffer);
		Buffer bufferIndices(sizeof(index_buffer), index_buffer);
		GPUBuffer* vertexGPUBuffer = Renderer::Instance()->createVertexBuffer(GPUBuffer::GBU_GPU_READ, bufferVertices);
		GPUBuffer* indexGPUBuffer = Renderer::Instance()->createIndexBuffer(GPUBuffer::GBU_GPU_READ, bufferIndices);

		RenderInput* pRenderInput = Renderer::Instance()->createRenderInput(RenderTargetManager::Instance()->getVertexElement(), m_pMaterialBaseUpdate->getShaderProgram());
		pRenderInput->bindGPUBuffers(vertexGPUBuffer, sizeof(float) * 5, indexGPUBuffer, sizeof(ui16));
		pRenderInput->setTopologyType(RenderInput::TT_TRIANGLELIST);
		m_pScreenAlignedQuad->setRenderInput(pRenderInput);

		if (m_bEnableFilter)
		{
			m_pScreenAlignedQuadFilter = LordNew Renderable();
			GPUBuffer* vertexGPUBuffer_filter = Renderer::Instance()->createVertexBuffer(GPUBuffer::GBU_GPU_READ, bufferVertices);
			GPUBuffer* indexGPUBuffer_filter = Renderer::Instance()->createIndexBuffer(GPUBuffer::GBU_GPU_READ, bufferIndices);

			ShaderProgram* pShaderProgram_filter = ShaderProgramManager::Instance()->getShaderProgram("FilterAdditional");
			m_pMaterialFilterUpdate->setShaderProgram(pShaderProgram_filter);

			RenderInput* pRenderInput_filter = Renderer::Instance()->createRenderInput(RenderTargetManager::Instance()->getVertexElement(), m_pMaterialFilterUpdate->getShaderProgram());
			pRenderInput_filter->bindGPUBuffers(vertexGPUBuffer_filter, sizeof(float) * 5, indexGPUBuffer_filter, sizeof(ui16));
			pRenderInput_filter->setTopologyType(RenderInput::TT_TRIANGLELIST);

			m_pScreenAlignedQuadFilter->setRenderInput(pRenderInput_filter);

			m_pMaterialFilterUpdate->beginShaderParams(2);
			m_pMaterialFilterUpdate->setShaderParam("sceneColorSampler", SPT_TEXTURE, (void*)&TEXTURE_UNIT0);
			m_pMaterialFilterUpdate->setShaderParam("FilterAdditionalSampler", SPT_TEXTURE, (void*)&TEXTURE_UNIT1);
			m_pMaterialFilterUpdate->endShaderParams();
		}

		return true;

	}

	void RenderTargetManager::changeFilterBlendmapName(const String& texName)
	{
		if (!m_bEnableFilter)
			return;

		LordAssert(m_pMaterialFilterUpdate);

		if (texName != m_pFilterBlendmapName)
		{
			m_pFilterBlendmapName = texName;

			if (m_pFilterBlendmap)
			{
				TextureManager::Instance()->releaseResource(m_pFilterBlendmap);
				m_pFilterBlendmap = NULL;
			}
			m_pFilterBlendmap = TextureManager::Instance()->createTexture(m_pFilterBlendmapName);
			m_pFilterBlendmap->load();
			m_pMaterialFilterUpdate->activeTexture(1, m_pFilterBlendmap);
		}
	}

}