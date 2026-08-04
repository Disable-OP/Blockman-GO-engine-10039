#include "GUIRenderManager.h"
#include "Scene/SceneManager.h"
#include "Render/Material.h"
#include "Render/ShaderProgramManager.h"
#include "GUICoolDownRender.h"
#include "GUISystem.h"

namespace LORD
{

	const int MAX_GUI_RENDER_QUAD_COUNT = 4000;
	const int MAX_GUI_RENDER_VERTEXT_COUNT = MAX_GUI_RENDER_QUAD_COUNT * 4;
	const int MAX_GUI_RENDER_INDEX_COUNT = MAX_GUI_RENDER_QUAD_COUNT * 6;

	//---------------------------------------------------------------------
	GUIRenderManager* GUIRenderManager::Instance()
	{
		return ms_pSingleton;
	}

	GUIRenderManager::GUIRenderManager(uint nWidth, uint nHeight)
		: m_nDisplayWidth(nWidth)
		, m_nDisplayHeight(nHeight)
		, m_bIsInScissor(false)
		, m_pGUI2DCamera(NULL)
		, m_pActorCamera(NULL)
		, m_matViewProject(Matrix4::IDENTITY)
		, m_pVertexSystemMemory(NULL)
		, m_pIndexSystemMemory(NULL)
		, m_nVertexCount(0)
		, m_nIndexCount(0)
		, m_nQuadCount(0)
		, m_pVertexBuffer(NULL)
		, m_pIndexBuffer(NULL)
		, m_pActiveTexture(NULL)
		, m_pRenderInput(NULL)
		, m_pBufferOffset(NULL)
		, m_depth(0.0f)
		, m_CurrentProgram(NULL)
		, m_CurrentTexture(NULL)
		, m_is3DCamera(true)
		, m_currentMaterial(GUIMaterialType_INVALID)
		, m_currentTexture1(NULL)
		, m_currentTexture2(NULL)
		, m_quadNumber(0)
	{
		Initialize();
	}

	GUIRenderManager::~GUIRenderManager()
	{
		Destroy();
	}

	void GUIRenderManager::CreateRenderable()
	{
		RenderInput::VertexElementList vertElms(3);
		vertElms[0].semantic = RenderInput::VS_POSITION;
		vertElms[0].pixFmt = PF_RGB32_FLOAT;

		vertElms[1].semantic = RenderInput::VS_COLOR;
		vertElms[1].pixFmt = PF_RGBA8_UNORM;

		vertElms[2].semantic = RenderInput::VS_TEXCOORD;
		vertElms[2].pixFmt = PF_RG32_FLOAT;

		// 默认先用通用的renderinput，如果材质有不同的需求，只能做新的renderinput了
		LordAssert(m_materialMap[GUIMaterialType_CULL_BACK]);
		m_pRenderInput = Renderer::Instance()->createRenderInput(vertElms, m_materialMap[GUIMaterialType_CULL_BACK]->getShaderProgram());

		// buffer
		m_pVertexSystemMemory = (GUIVertexDeclaration*)LordMalloc(MAX_GUI_RENDER_VERTEXT_COUNT*sizeof(GUIVertexDeclaration));
		m_pIndexSystemMemory = (Word*)LordMalloc(MAX_GUI_RENDER_INDEX_COUNT*sizeof(Word));

		Word* pQuadStart = m_pIndexSystemMemory;
		// init index 
		for (unsigned int i = 0; i < MAX_GUI_RENDER_QUAD_COUNT; ++i )
		{
			int nOffset = i * 4;
			pQuadStart[0] = nOffset + 0;
			pQuadStart[1] = nOffset + 1;
			pQuadStart[2] = nOffset + 2;
			pQuadStart[3] = nOffset + 0;
			pQuadStart[4] = nOffset + 3;
			pQuadStart[5] = nOffset + 1;
			pQuadStart += 6;
		}

		Buffer vertBuffer(sizeof(GUIVertexDeclaration)*MAX_GUI_RENDER_VERTEXT_COUNT, m_pVertexSystemMemory);
		m_pVertexBuffer = Renderer::Instance()->createVertexBuffer(GPUBuffer::GBU_GPU_READ|GPUBuffer::GBU_CPU_WRITE, vertBuffer);

		Buffer indexBuffer(sizeof(Word)*MAX_GUI_RENDER_INDEX_COUNT, m_pIndexSystemMemory);
		m_pIndexBuffer = Renderer::Instance()->createIndexBuffer(GPUBuffer::GBU_GPU_READ,indexBuffer);

		// reset
		m_nVertexCount = 0;
		m_nIndexCount = 0;
		m_nQuadCount = 0;
		m_pBufferOffset = m_pVertexSystemMemory;
	}

	void GUIRenderManager::DestroyRenderable()
	{
		for (size_t i = 0; i < m_RenderEvent.size(); ++i)
		{
			RenderEvent& evt = m_RenderEvent[i];
			LordSafeFree(evt.pIndices);
		}
		m_RenderEvent.clear();

		LordSafeDelete(m_pIndexBuffer);
		LordSafeDelete(m_pVertexBuffer);
		LordSafeFree(m_pVertexSystemMemory);
		LordSafeFree(m_pIndexSystemMemory);
		LordSafeDelete(m_pRenderInput);
	}

	void GUIRenderManager::Initialize()
	{
		// matrix
		m_pGUI2DCamera = LordNew Camera(m_is3DCamera ? Camera::PM_PERSPECTIVE : Camera::PM_ORTHO, false);
		m_pActorCamera = LordNew Camera(Camera::PM_ORTHO, false);
		UpdateProjMatrix();

		// shader
		// set render Queue
		SceneManager* pSceneManager = SceneManager::Instance();
		LordAssert(pSceneManager);
		pSceneManager->setGUICamera(m_pActorCamera);

		ShaderProgramManager* pShaderMgr = ShaderProgramManager::Instance();
		LordAssert(pShaderMgr);
		ShaderProgram* program = pShaderMgr->getShaderProgram("UI");
		Material* pMaterial = m_materialMap[GUIMaterialType_CULL_BACK] = LordNew Material;
		pMaterial->setShaderProgram(program);
		{	// UI
			BlendState::BlendDesc blenddesc = pMaterial->getBlendState()->getDesc();
			blenddesc.bBlendEnable = true;
			blenddesc.srcBlend = BlendState::BF_SRC_ALPHA;
			blenddesc.dstBlend = BlendState::BF_INV_SRC_ALPHA;
			blenddesc.srcAlphaBlend = BlendState::BF_INV_DST_ALPHA;
			blenddesc.dstAlphaBlend = BlendState::BF_ONE;

			DepthStencilState::DepthStencilDesc depthdesc = pMaterial->getDepthState()->getDesc();
			depthdesc.bDepthEnable = false;
			depthdesc.bWriteDepth = false;

			SamplerState::SamplerDesc samdesc = pMaterial->getSamplerState(0)->getDesc();
			samdesc.minFilter = SamplerState::FO_POINT;
			samdesc.magFilter = SamplerState::FO_POINT;
			samdesc.mipFilter = SamplerState::FO_NONE;
			samdesc.addrUMode = SamplerState::AM_CLAMP;
			samdesc.addrVMode = SamplerState::AM_CLAMP;

			pMaterial->setBlendState(blenddesc);
			pMaterial->setDepthState(depthdesc);
			pMaterial->setSamplerState(0, samdesc);
			_handleProgram(GUIMaterialType_CULL_BACK);
		}

		pMaterial = m_materialMap[GUIMaterialType_CULL_NONE] = LordNew Material;
		pMaterial->setShaderProgram(program);
		{	// UI CullNone
			BlendState::BlendDesc blenddesc = pMaterial->getBlendState()->getDesc();
			blenddesc.bBlendEnable = true;
			blenddesc.srcBlend = BlendState::BF_SRC_ALPHA;
			blenddesc.dstBlend = BlendState::BF_INV_SRC_ALPHA;
			blenddesc.srcAlphaBlend = BlendState::BF_INV_DST_ALPHA;
			blenddesc.dstAlphaBlend = BlendState::BF_ONE;

			RasterizerState::RasterizerDesc rasterdesc = pMaterial->getRasterizerState()->getDesc();
			rasterdesc.cullMode = RasterizerState::CULL_NONE;

			DepthStencilState::DepthStencilDesc depthdesc = pMaterial->getDepthState()->getDesc();
			depthdesc.bDepthEnable = false;
			depthdesc.bWriteDepth = false;

			SamplerState::SamplerDesc samdesc = pMaterial->getSamplerState(0)->getDesc();
			samdesc.minFilter = SamplerState::FO_POINT;
			samdesc.magFilter = SamplerState::FO_POINT;
			samdesc.mipFilter = SamplerState::FO_NONE;
			samdesc.addrUMode = SamplerState::AM_CLAMP;
			samdesc.addrVMode = SamplerState::AM_CLAMP;

			pMaterial->setBlendState(blenddesc);
			pMaterial->setRasterizerState(rasterdesc);
			pMaterial->setDepthState(depthdesc);
			pMaterial->setSamplerState(0, samdesc);
			_handleProgram(GUIMaterialType_CULL_NONE);
		}

		// font shader
		// set render Queue
		program = pShaderMgr->getShaderProgram("Font");
		pMaterial = m_materialMap[GUIMaterialType_FONT] = LordNew Material;
		pMaterial->setShaderProgram(program);
		{	// Font
			BlendState::BlendDesc blenddesc = pMaterial->getBlendState()->getDesc();
			blenddesc.bBlendEnable = true;
			blenddesc.srcBlend = BlendState::BF_SRC_ALPHA;
			blenddesc.dstBlend = BlendState::BF_INV_SRC_ALPHA;
			blenddesc.srcAlphaBlend = BlendState::BF_INV_DST_ALPHA;
			blenddesc.dstAlphaBlend = BlendState::BF_ONE;

			DepthStencilState::DepthStencilDesc depthdesc = pMaterial->getDepthState()->getDesc();
			depthdesc.bDepthEnable = false;
			depthdesc.bWriteDepth = false;

			SamplerState::SamplerDesc samdesc = pMaterial->getSamplerState(0)->getDesc();
			samdesc.minFilter = SamplerState::FO_LINEAR;
			samdesc.magFilter = SamplerState::FO_LINEAR;
			samdesc.mipFilter = SamplerState::FO_NONE;
			samdesc.addrUMode = SamplerState::AM_CLAMP;
			samdesc.addrVMode = SamplerState::AM_CLAMP;

			pMaterial->setBlendState(blenddesc);
			pMaterial->setDepthState(depthdesc);
			pMaterial->setSamplerState(0, samdesc);
			_handleProgram(GUIMaterialType_FONT);
		}

		program = pShaderMgr->getShaderProgram("UIMask");
		pMaterial = m_materialMap[GUIMaterialType_MASK] = LordNew Material;
		pMaterial->setShaderProgram(program);
		{	// UI Mask
			BlendState::BlendDesc blenddesc = pMaterial->getBlendState()->getDesc();
			blenddesc.bBlendEnable = true;
			blenddesc.srcBlend = BlendState::BF_SRC_ALPHA;
			blenddesc.dstBlend = BlendState::BF_INV_SRC_ALPHA;
			blenddesc.srcAlphaBlend = BlendState::BF_INV_DST_ALPHA;
			blenddesc.dstAlphaBlend = BlendState::BF_ONE;

			DepthStencilState::DepthStencilDesc depthdesc = pMaterial->getDepthState()->getDesc();
			depthdesc.bDepthEnable = false;
			depthdesc.bWriteDepth = false;

			SamplerState::SamplerDesc samdesc = pMaterial->getSamplerState(0)->getDesc();
			samdesc.minFilter = SamplerState::FO_POINT;
			samdesc.magFilter = SamplerState::FO_POINT;
			samdesc.mipFilter = SamplerState::FO_NONE;
			samdesc.addrUMode = SamplerState::AM_CLAMP;
			samdesc.addrVMode = SamplerState::AM_CLAMP;

			pMaterial->setBlendState(blenddesc);
			pMaterial->setDepthState(depthdesc);
			pMaterial->setSamplerState(0, samdesc);
			pMaterial->setSamplerState(1, samdesc);
			_handleProgram(GUIMaterialType_MASK);
		}

		pMaterial = m_materialMap[GUIMaterialType_MASK_CULL_NONE] = LordNew Material;
		pMaterial->setShaderProgram(program);
		{	// UI Mask CullNone
			BlendState::BlendDesc blenddesc = pMaterial->getBlendState()->getDesc();
			blenddesc.bBlendEnable = true;
			blenddesc.srcBlend = BlendState::BF_SRC_ALPHA;
			blenddesc.dstBlend = BlendState::BF_INV_SRC_ALPHA;
			blenddesc.srcAlphaBlend = BlendState::BF_INV_DST_ALPHA;
			blenddesc.dstAlphaBlend = BlendState::BF_ONE;

			RasterizerState::RasterizerDesc rasterdesc = pMaterial->getRasterizerState()->getDesc();
			rasterdesc.cullMode = RasterizerState::CULL_NONE;

			DepthStencilState::DepthStencilDesc depthdesc = pMaterial->getDepthState()->getDesc();
			depthdesc.bDepthEnable = false;
			depthdesc.bWriteDepth = false;

			SamplerState::SamplerDesc samdesc = pMaterial->getSamplerState(0)->getDesc();
			samdesc.minFilter = SamplerState::FO_POINT;
			samdesc.magFilter = SamplerState::FO_POINT;
			samdesc.mipFilter = SamplerState::FO_NONE;
			samdesc.addrUMode = SamplerState::AM_CLAMP;
			samdesc.addrVMode = SamplerState::AM_CLAMP;

			pMaterial->setBlendState(blenddesc);
			pMaterial->setRasterizerState(rasterdesc);
			pMaterial->setDepthState(depthdesc);
			pMaterial->setSamplerState(0, samdesc);
			pMaterial->setSamplerState(1, samdesc);
			_handleProgram(GUIMaterialType_MASK_CULL_NONE);
		}

		// disable material
		program = pShaderMgr->getShaderProgram("UIDisable");
		pMaterial = m_materialMap[GUIMaterialType_CULL_BACK_GRAY] = LordNew Material;
		pMaterial->setShaderProgram(program);
		{	// UI Disable
			BlendState::BlendDesc blenddesc = pMaterial->getBlendState()->getDesc();
			blenddesc.bBlendEnable = true;
			blenddesc.srcBlend = BlendState::BF_SRC_ALPHA;
			blenddesc.dstBlend = BlendState::BF_INV_SRC_ALPHA;
			blenddesc.srcAlphaBlend = BlendState::BF_INV_DST_ALPHA;
			blenddesc.dstAlphaBlend = BlendState::BF_ONE;
			
			DepthStencilState::DepthStencilDesc depthdesc = pMaterial->getDepthState()->getDesc();
			depthdesc.bDepthEnable = false;
			depthdesc.bWriteDepth = false;

			SamplerState::SamplerDesc samdesc = pMaterial->getSamplerState(0)->getDesc();
			samdesc.minFilter = SamplerState::FO_POINT;
			samdesc.magFilter = SamplerState::FO_POINT;
			samdesc.mipFilter = SamplerState::FO_NONE;
			samdesc.addrUMode = SamplerState::AM_CLAMP;
			samdesc.addrVMode = SamplerState::AM_CLAMP;

			pMaterial->setBlendState(blenddesc);
			pMaterial->setDepthState(depthdesc);
			pMaterial->setSamplerState(0, samdesc);
			_handleProgram(GUIMaterialType_CULL_BACK_GRAY);
		}

		pMaterial = m_materialMap[GUIMaterialType_CULL_NONE_GRAY] = LordNew Material;
		pMaterial->setShaderProgram(program);
		{	// UI Disable CullNone
			BlendState::BlendDesc blenddesc = pMaterial->getBlendState()->getDesc();
			blenddesc.bBlendEnable = true;
			blenddesc.srcBlend = BlendState::BF_SRC_ALPHA;
			blenddesc.dstBlend = BlendState::BF_INV_SRC_ALPHA;
			blenddesc.srcAlphaBlend = BlendState::BF_INV_DST_ALPHA;
			blenddesc.dstAlphaBlend = BlendState::BF_ONE;

			RasterizerState::RasterizerDesc rasterdesc = pMaterial->getRasterizerState()->getDesc();
			rasterdesc.cullMode = RasterizerState::CULL_NONE;

			DepthStencilState::DepthStencilDesc depthdesc = pMaterial->getDepthState()->getDesc();
			depthdesc.bDepthEnable = false;
			depthdesc.bWriteDepth = false;

			SamplerState::SamplerDesc samdesc = pMaterial->getSamplerState(0)->getDesc();
			samdesc.minFilter = SamplerState::FO_POINT;
			samdesc.magFilter = SamplerState::FO_POINT;
			samdesc.mipFilter = SamplerState::FO_NONE;
			samdesc.addrUMode = SamplerState::AM_CLAMP;
			samdesc.addrVMode = SamplerState::AM_CLAMP;

			pMaterial->setBlendState(blenddesc);
			pMaterial->setRasterizerState(rasterdesc);
			pMaterial->setDepthState(depthdesc);
			pMaterial->setSamplerState(0, samdesc);
			_handleProgram(GUIMaterialType_CULL_NONE_GRAY);
		}

		program = pShaderMgr->getShaderProgram("UIMaskDisable");
		pMaterial = m_materialMap[GUIMaterialType_MASK_GRAY] = LordNew Material;
		pMaterial->setShaderProgram(program);
		{	// UI Mask Disable
			BlendState::BlendDesc blenddesc = pMaterial->getBlendState()->getDesc();
			blenddesc.bBlendEnable = true;
			blenddesc.srcBlend = BlendState::BF_SRC_ALPHA;
			blenddesc.dstBlend = BlendState::BF_INV_SRC_ALPHA;
			blenddesc.srcAlphaBlend = BlendState::BF_INV_DST_ALPHA;
			blenddesc.dstAlphaBlend = BlendState::BF_ONE;

			DepthStencilState::DepthStencilDesc depthdesc = pMaterial->getDepthState()->getDesc();
			depthdesc.bDepthEnable = false;
			depthdesc.bWriteDepth = false;

			SamplerState::SamplerDesc samdesc = pMaterial->getSamplerState(0)->getDesc();
			samdesc.minFilter = SamplerState::FO_POINT;
			samdesc.magFilter = SamplerState::FO_POINT;
			samdesc.mipFilter = SamplerState::FO_NONE;
			samdesc.addrUMode = SamplerState::AM_CLAMP;
			samdesc.addrVMode = SamplerState::AM_CLAMP;

			pMaterial->setBlendState(blenddesc);
			pMaterial->setDepthState(depthdesc);
			pMaterial->setSamplerState(0, samdesc);
			pMaterial->setSamplerState(1, samdesc);
			_handleProgram(GUIMaterialType_MASK_GRAY);
		}

		pMaterial = m_materialMap[GUIMaterialType_MASK_CULL_NONE_GRAY] = LordNew Material;
		pMaterial->setShaderProgram(program);
		{	// UI Mask Disable
			BlendState::BlendDesc blenddesc = pMaterial->getBlendState()->getDesc();
			blenddesc.bBlendEnable = true;
			blenddesc.srcBlend = BlendState::BF_SRC_ALPHA;
			blenddesc.dstBlend = BlendState::BF_INV_SRC_ALPHA;
			blenddesc.srcAlphaBlend = BlendState::BF_INV_DST_ALPHA;
			blenddesc.dstAlphaBlend = BlendState::BF_ONE;

			RasterizerState::RasterizerDesc rasterdesc = pMaterial->getRasterizerState()->getDesc();
			rasterdesc.cullMode = RasterizerState::CULL_NONE;

			DepthStencilState::DepthStencilDesc depthdesc = pMaterial->getDepthState()->getDesc();
			depthdesc.bDepthEnable = false;
			depthdesc.bWriteDepth = false;

			SamplerState::SamplerDesc samdesc = pMaterial->getSamplerState(0)->getDesc();
			samdesc.minFilter = SamplerState::FO_POINT;
			samdesc.magFilter = SamplerState::FO_POINT;
			samdesc.mipFilter = SamplerState::FO_NONE;
			samdesc.addrUMode = SamplerState::AM_CLAMP;
			samdesc.addrVMode = SamplerState::AM_CLAMP;

			pMaterial->setBlendState(blenddesc);
			pMaterial->setRasterizerState(rasterdesc);
			pMaterial->setDepthState(depthdesc);
			pMaterial->setSamplerState(0, samdesc);
			pMaterial->setSamplerState(1, samdesc);
			_handleProgram(GUIMaterialType_MASK_CULL_NONE_GRAY);
		}

		m_RenderEvent.reserve(MAX_GUI_RENDER_QUAD_COUNT);

		CreateRenderable();
	}

	void GUIRenderManager::Destroy()
	{
		DestroyRenderable();

		LordSafeDelete(m_pGUI2DCamera);
		LordSafeDelete(m_pActorCamera);

		map<GUIMaterialType, Material*>::type::iterator it = m_materialMap.begin();
		for (; it != m_materialMap.end(); ++it)
		{
			LordDelete it->second;
		}
		m_materialMap.clear();
	}

	void GUIRenderManager::OnSized( uint nWidth, uint nHeight )
	{
		m_nDisplayWidth = nWidth;
		m_nDisplayHeight = nHeight;

		UpdateProjMatrix();
	}

	void GUIRenderManager::UpdateProjMatrix()
	{
		// setup matrix
		const float w = (float)m_nDisplayWidth;
		const float h = (float)m_nDisplayHeight;
		const float aspect = w / h;
		const float midx = w * 0.5f;
		const float midy = h * 0.5f;

		float zEye = 0.0f;

		if (!m_is3DCamera)
		{
			zEye = midx / (aspect * 0.267949192431123f);
		} 
		else
		{
			zEye = m_nDisplayHeight / 1.1566f;
		}

		Vector3 vEye(midx, midy, -zEye);
		Vector3 vAt(midx, midy, 0.0f);
		Vector3 vUp(0, -1.0f, 0);

		m_pGUI2DCamera->setPosition(vEye);
		Vector3 dir = (vAt - vEye).normalizedCopy();
		m_pGUI2DCamera->setDirection(dir);
		m_pGUI2DCamera->setUp(vUp);
		
		if (!m_is3DCamera)
		{
			m_pGUI2DCamera->setNearClip(-zEye*0.5f);
			m_pGUI2DCamera->setFarClip(zEye*2.0f);
			m_pGUI2DCamera->setFov(Math::PI_DIV6);
		}
		else
		{
			m_pGUI2DCamera->setNearClip(50.0f);
			m_pGUI2DCamera->setFarClip(zEye + h / 2);
			m_pGUI2DCamera->setFov(Math::PI_DIV3);
		}

		m_pGUI2DCamera->setWidth((uint)w);
		m_pGUI2DCamera->setHeight((uint)h);
		m_pGUI2DCamera->update();

		m_matViewProject = m_pGUI2DCamera->getViewProjMatrix();

		// actor camera--------------------------------------------------------
		float actorEyeDis = midx / (aspect * 0.267949192431123f);
		Vector3 actorEye(midx, midy, -actorEyeDis);
		m_pActorCamera->setPosition(actorEye);
		Vector3 actorDir = (vAt - actorEye).normalizedCopy();
		m_pActorCamera->setDirection(actorDir);
		m_pActorCamera->setUp(vUp);

		m_pActorCamera->setNearClip(-actorEyeDis*0.5f);
		m_pActorCamera->setFarClip(actorEyeDis*2.0f);
		m_pActorCamera->setFov(Math::PI_DIV6);

		m_pActorCamera->setWidth((uint)w);
		m_pActorCamera->setHeight((uint)h);
		m_pActorCamera->update();

	}
	
	void GUIRenderManager::AddQuadToBuffer(const Vector3& lt, const Vector3& rb, const Vector3& lb, const Vector3& rt, 
											const ERect& rtTexAddress, const ColorRect& color, Texture* pTexture1, Texture* pTexture2, GUIMaterialType material)
	{
		if (!m_materialMap[material])
		{
			return;
		}

		// 如果第二张贴图不存在，不能用mask材质
		if ((material == GUIMaterialType_MASK || material == GUIMaterialType_MASK_CULL_NONE ) && pTexture2 == NULL)
		{
			return;
		}

		// check material
		// 如果材质切换了，肯定要把缓存先draw，
		// 材质没变，贴图变了，也要清缓存
		if (!(material == m_currentMaterial && pTexture1 == m_currentTexture1 && pTexture2 == m_currentTexture2))
		{
			DrawBuffer();
		}

		m_currentTexture1 = pTexture1;
		m_currentTexture2 = pTexture2;
		m_currentMaterial = material;


		unsigned long nQuadNum = (((unsigned long)m_pBufferOffset - (unsigned long)m_pVertexSystemMemory)) / (sizeof(GUIVertexDeclaration) * 4);

		if (nQuadNum > m_quadNumber)
		{
			m_quadNumber = nQuadNum;
		}

		if (nQuadNum >= MAX_GUI_RENDER_QUAD_COUNT)
		{
			LordLogError("ui renderable num is full!");
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS 
			MessageBox(0, "ui renderable num is full!", 0, 0);
#endif
			return;
		}

		if (m_nQuadCount < MAX_GUI_RENDER_QUAD_COUNT)
		{
			m_pBufferOffset[0].position = lt;
			m_pBufferOffset[0].position.x = PixelAligned(m_pBufferOffset[0].position.x);
			m_pBufferOffset[0].position.y = PixelAligned(m_pBufferOffset[0].position.y);

			m_pBufferOffset[0].color = color.cLeftTop.getABGR();
			m_pBufferOffset[0].texCoord.x = rtTexAddress.left;
			m_pBufferOffset[0].texCoord.y = rtTexAddress.top;

			m_pBufferOffset[1].position = rb;
			m_pBufferOffset[1].position.x = PixelAligned(m_pBufferOffset[1].position.x);
			m_pBufferOffset[1].position.y = PixelAligned(m_pBufferOffset[1].position.y);
			m_pBufferOffset[1].color = color.cRightBottom.getABGR();
			m_pBufferOffset[1].texCoord.x = rtTexAddress.right;
			m_pBufferOffset[1].texCoord.y = rtTexAddress.bottom;

			m_pBufferOffset[2].position = lb;
			m_pBufferOffset[2].position.x = PixelAligned(m_pBufferOffset[2].position.x);
			m_pBufferOffset[2].position.y = PixelAligned(m_pBufferOffset[2].position.y);
			m_pBufferOffset[2].color = color.cLeftBottom.getABGR();
			m_pBufferOffset[2].texCoord.x = rtTexAddress.left;
			m_pBufferOffset[2].texCoord.y = rtTexAddress.bottom;

			m_pBufferOffset[3].position = rt;
			m_pBufferOffset[3].position.x = PixelAligned(m_pBufferOffset[3].position.x);
			m_pBufferOffset[3].position.y = PixelAligned(m_pBufferOffset[3].position.y);
			m_pBufferOffset[3].color = color.cRightTop.getABGR();
			m_pBufferOffset[3].texCoord.x = rtTexAddress.right;
			m_pBufferOffset[3].texCoord.y = rtTexAddress.top;

			++m_nQuadCount;
			m_nVertexCount += 4;
			m_nIndexCount += 6;
			m_pBufferOffset += 4;
		}

		if (m_nQuadCount == MAX_GUI_RENDER_QUAD_COUNT)
		{
			DrawBuffer();
		}
	}

	void GUIRenderManager::DrawBuffer()
	{
		if (!m_materialMap[m_currentMaterial])
		{
			//LordLogError("GUIRenderManager::DrawBuffer() material do not exist!");
			return;
		}

		// 如果第二张贴图不存在，不能用mask材质
		if ((m_currentMaterial == GUIMaterialType_MASK || m_currentMaterial == GUIMaterialType_MASK_CULL_NONE) && m_currentTexture2 == NULL)
		{
			LordLogError("GUIRenderManager::DrawBuffer() material is set mask but not has second texture!");
			return;
		}
		
		if (m_nQuadCount > 0)
		{
			RenderEvent evt;
			evt.matType = m_currentMaterial;
			evt.pTexture[0] = m_currentTexture1;
			evt.pTexture[1] = m_currentTexture2;
			evt.pIndices = (Word*)LordMalloc(sizeof(Word) * m_nIndexCount);
			evt.indicesNum = m_nIndexCount;
			if (m_bIsInScissor)
			{
				evt.scissorRect = m_rtCurrentClipRect;
				evt.hasScissor = true;
			}
			else
			{
				evt.hasScissor = false;
			}
			unsigned long nQuadNum = (((unsigned long)m_pBufferOffset - (unsigned long)m_pVertexSystemMemory)) / (sizeof(GUIVertexDeclaration) * 4);
			LordAssert(nQuadNum < MAX_GUI_RENDER_QUAD_COUNT);
			ui32 nCurrentIndex = nQuadNum - m_nQuadCount;
			memcpy(evt.pIndices, m_pIndexSystemMemory + nCurrentIndex * 6, m_nIndexCount*sizeof(Word));
			m_RenderEvent.push_back(evt);

			m_nQuadCount = 0;
			m_nIndexCount = 0;
		}
	}

	void GUIRenderManager::BeginRender()
	{
	}

	LORD::Vector2 GUIRenderManager::GetDisplaySize()
	{
		return Vector2((float)m_nDisplayWidth, (float)m_nDisplayHeight);
	}

	void GUIRenderManager::EndRender()
	{
		DrawBuffer();
		RenderEvents();
		//DrawBuffer();
		m_CurrentProgram = NULL;
		m_CurrentTexture = NULL;
	}
	
	void GUIRenderManager::BeginScissorRect( const ERect& rtClip )
	{
		DrawBuffer();

		m_rtCurrentClipRect = rtClip;
		m_bIsInScissor = true;
	}

	void GUIRenderManager::EndScissorRect()
	{
		DrawBuffer();
		m_bIsInScissor = false;
	}

	void GUIRenderManager::BeginGUIViewport()
	{
		// set viewport
		Viewport* pViewport = Renderer::Instance()->getMainViewport();
		LordAssert(pViewport);
		pViewport->resize(0, 0, GUISystem::Instance()->GetScreenWidth(), GUISystem::Instance()->GetScreenHeight());
		Renderer::Instance()->setViewport(pViewport);
	}

	void GUIRenderManager::EndGUIViewport()
	{
		// set viewport
		Viewport* pViewport = Renderer::Instance()->getMainViewport();
		LordAssert(pViewport);
		pViewport->resize(0, 0, GUISystem::Instance()->GetScreenWidth(), GUISystem::Instance()->GetScreenHeight());
		Renderer::Instance()->setViewport(pViewport);
	}

	void GUIRenderManager::_handleMaterial()
	{
		Renderer* pRender = Renderer::Instance();

		// handle material
		m_materialMap[m_currentMaterial]->active();
	}

	void GUIRenderManager::_handleMaterial(GUIMaterialType _type)
	{
		// handle material
		m_materialMap[_type]->active();
	}

	void GUIRenderManager::_handleProgram(GUIMaterialType _type)
	{
		// handle uniform
		// 需要根据材质区分
		switch (_type)
		{
		case GUIMaterialType_CULL_BACK:
		case GUIMaterialType_CULL_NONE:
		case GUIMaterialType_FONT:
		case GUIMaterialType_CULL_BACK_GRAY:
		case GUIMaterialType_CULL_NONE_GRAY:
		case GUIMaterialType_FONT_GRAY:
		{
			ShaderProgram* shader = m_materialMap[_type]->getShaderProgram();
			if (shader)
			{
				m_materialMap[_type]->beginShaderParams(2);
				m_materialMap[_type]->setShaderParam("matWVP", SPT_MAT4, &m_matViewProject);
				m_materialMap[_type]->setShaderParam("texSampler", SPT_TEXTURE, (void*)&TEXTURE_UNIT0);
				m_materialMap[_type]->endShaderParams();
			}
		}
			break;
		case GUIMaterialType_MASK:
		case GUIMaterialType_MASK_CULL_NONE:
		case GUIMaterialType_MASK_GRAY:
		case GUIMaterialType_MASK_CULL_NONE_GRAY:
		{
			ShaderProgram* shader = m_materialMap[_type]->getShaderProgram();
			if (shader)
			{
				m_materialMap[_type]->beginShaderParams(3);
				m_materialMap[_type]->setShaderParam("matWVP", SPT_MAT4, &m_matViewProject);
				m_materialMap[_type]->setShaderParam("texSampler", SPT_TEXTURE, (void*)&TEXTURE_UNIT0);
				m_materialMap[_type]->setShaderParam("texMaskSampler", SPT_TEXTURE, (void*)&TEXTURE_UNIT1);
				m_materialMap[_type]->endShaderParams();
			}
		}
			break;
		default:
			break;
		}
	}

	void GUIRenderManager::_reset()
	{
		// reset
		m_nVertexCount = 0;
		m_nIndexCount = 0;
		m_nQuadCount = 0;
		m_pBufferOffset = m_pVertexSystemMemory;
		m_currentMaterial = GUIMaterialType_INVALID;
		m_currentTexture1 = NULL;
		m_currentTexture2 = NULL;
	}

	void GUIRenderManager::RenderEvents()
	{
		if (!m_RenderEvent.empty())
		{
			Renderer* pRender = Renderer::Instance();
			// handle renderinput
			Buffer vertBuffer(m_nVertexCount*sizeof(GUIVertexDeclaration), m_pVertexSystemMemory);
			m_pVertexBuffer->updateSubData(0, vertBuffer);

			m_pRenderInput->unbindGPUBuffers();
			m_pRenderInput->bindGPUBuffers(m_pVertexBuffer, sizeof(GUIVertexDeclaration), NULL, 0);
			m_pRenderInput->setVertexCount(m_nVertexCount);

			m_pRenderInput->bind();
			for (size_t i = 0; i < m_RenderEvent.size(); ++i)
			{
				RenderEvent& evt = m_RenderEvent[i];
				_handleMaterial(evt.matType);
				
				// handle texture
				if (evt.pTexture[0])
				{
					pRender->setTexture(0, evt.pTexture[0]);
					evt.pTexture[0]->setSamplerState(m_materialMap[evt.matType]->getSamplerState(0));
				}

				if (evt.pTexture[1])
				{
					pRender->setTexture(1, evt.pTexture[1]);
				}

				if (evt.hasScissor)
				{
					pRender->scissor(
						(uint)PixelAligned(evt.scissorRect.left), 
						(uint)PixelAligned(evt.scissorRect.top), 
						(uint)PixelAligned(evt.scissorRect.getWidth()), 
						(uint)PixelAligned(evt.scissorRect.getHeight()));
				}
				
				pRender->render(m_pRenderInput, evt.pIndices, evt.indicesNum);
				pRender->endScissor();
				LordFree(evt.pIndices);
			}
			m_pRenderInput->unbind();
			m_RenderEvent.clear();

			_reset();
		}
	}

}
