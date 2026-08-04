// #include "GUICoolDownRender.h"
// #include "GUIRenderManager.h"
// #include "Render/Material.h"
// 
// namespace LORD
// {
// 	const int MAX_COOL_DOWN_TRIANGLE_COUNT = 200;
// 	const int MAX_COOL_DOWN_VERTEX_COUNT = MAX_COOL_DOWN_TRIANGLE_COUNT * 3;
// 	const int MAX_COOL_DOWN_INDEX_COUNT = MAX_COOL_DOWN_TRIANGLE_COUNT * 3;
// 	// 5度一份
// 	const float DETLA_ANGLE = Math::DEG2RAD * 5;
// 
// 	GUICoolDownRender::GUICoolDownRender()
// 		: m_pVertexSystemMemory(NULL)
// 		, m_pIndexSystemMemory(NULL)
// 		, m_nVertexCount(0)
// 		, m_nIndexCount(0)
// 		, m_nTriangleCount(0)
// 		, m_pVertexBuffer(NULL)
// 		, m_pIndexBuffer(NULL)
// 		, m_pTexture(NULL)
// 		, m_pRenderInput(NULL)
// 		, m_pBufferOffset(NULL)
// 	{
// 
// 	}
// 
// 	GUICoolDownRender::~GUICoolDownRender()
// 	{
// 
// 	}
// 
// 	void GUICoolDownRender::CreateRenderable()
// 	{
// 		RenderInput::VertexElementList vertElms(3);
// 		vertElms[0].semantic = RenderInput::VS_POSITION;
// 		vertElms[0].pixFmt = PF_RGB32_FLOAT;
// 
// 		vertElms[1].semantic = RenderInput::VS_COLOR;
// 		vertElms[1].pixFmt = PF_RGBA8_UNORM;
// 
// 		vertElms[2].semantic = RenderInput::VS_TEXCOORD;
// 		vertElms[2].pixFmt = PF_RG32_FLOAT;
// 
// 		m_pRenderInput = Renderer::Instance()->createRenderInput(vertElms, GUIRenderManager::Instance()->GetGUIShaderProgram());
// 
// 		// buffer
// 		m_pVertexSystemMemory = (GUIVertexDeclaration*)LordMalloc(MAX_COOL_DOWN_VERTEX_COUNT*sizeof(GUIVertexDeclaration));
// 		m_pIndexSystemMemory = (Word*)LordMalloc(MAX_COOL_DOWN_INDEX_COUNT*sizeof(Word));
// 
// 		Word* pQuadStart = m_pIndexSystemMemory;
// 		// init index 
// 		int nOffset = 0;
// 		for (unsigned int i = 0; i < MAX_COOL_DOWN_TRIANGLE_COUNT; ++i )
// 		{
// 			pQuadStart[0] = nOffset + 0;
// 			pQuadStart[1] = nOffset + 1;
// 			pQuadStart[2] = nOffset + 2;
// 			pQuadStart += 3;
// 			nOffset += 3;
// 		}
// 
// 		Buffer vertBuffer(sizeof(GUIVertexDeclaration)*MAX_COOL_DOWN_VERTEX_COUNT, m_pVertexSystemMemory);
// 		m_pVertexBuffer = Renderer::Instance()->createVertexBuffer(GPUBuffer::GBU_GPU_READ|GPUBuffer::GBU_CPU_WRITE, vertBuffer);
// 
// 		Buffer indexBuffer(sizeof(Word)*MAX_COOL_DOWN_INDEX_COUNT, m_pIndexSystemMemory);
// 		m_pIndexBuffer = Renderer::Instance()->createIndexBuffer(GPUBuffer::GBU_GPU_READ,indexBuffer);
// 
// 		// reset
// 		m_nVertexCount = 0;
// 		m_nIndexCount = 0;
// 		m_nTriangleCount = 0;
// 		m_pBufferOffset = m_pVertexSystemMemory;
// 
// 		// create texture
// 		m_pTexture = TextureManager::Instance()->createTexture("cooldown.png");
// 		if (m_pTexture)
// 		{
// 			m_pTexture->load();
// 			GUIRenderManager::Instance()->GetMaterial()->activeTexture(0, m_pTexture);
// 		}
// 	}
// 
// 	void GUICoolDownRender::DestroyRenderable()
// 	{
// 		if (m_pTexture)
// 		{
// 			TextureManager::Instance()->releaseResource(m_pTexture);
// 			m_pTexture = NULL;
// 		}
// 
// 		LordSafeDelete(m_pIndexBuffer);
// 		LordSafeDelete(m_pVertexBuffer);
// 		LordSafeFree(m_pVertexSystemMemory);
// 		LordSafeFree(m_pIndexSystemMemory);
// 		LordSafeDelete(m_pRenderInput);
// 	}
// 
// 	void GUICoolDownRender::AddCoolDown( const Vector2& centerPoint, float fRadius, float fPercent )
// 	{
// 		float fNowAngle = Math::PI_2 * fPercent;
// 		Vector2 position0;
// 		Vector2 uv0;
// 		Vector2 position1;
// 		Vector2 uv1;
// 		while(fNowAngle < Math::PI_2)
// 		{
// 			float fSin = Math::Sin(fNowAngle);
// 			float fCos = Math::Cos(fNowAngle);
// 			position0.x = centerPoint.x + fRadius * fSin;
// 			position0.y = centerPoint.y - fRadius * fCos;
// 
// 			uv0.x = fSin * 0.5f + 0.5f;
// 			uv0.y = 0.5f - 0.5f * fCos;
// 
// 			// ------------------------------------------
// 			float fAngle2 = fNowAngle+DETLA_ANGLE;
// 			fAngle2 = fAngle2 > (Math::PI_2) ? (Math::PI_2) : fAngle2;
// 
// 			fSin = Math::Sin(fAngle2);
// 			fCos = Math::Cos(fAngle2);
// 			position1.x = centerPoint.x + fRadius * fSin;
// 			position1.y = centerPoint.y - fRadius * fCos;
// 
// 			uv1.x = fSin * 0.5f + 0.5f;
// 			uv1.y = 0.5f - 0.5f * fCos;
// 
// 			//
// 			AddPoint(centerPoint, Vector2(0.5f, 0.5f));
// 			AddPoint(position0, uv0);
// 			AddPoint(position1, uv1);
// 
// 			++m_nTriangleCount;
// 
// 			fNowAngle += DETLA_ANGLE;
// 		}
// 	}
// 
// 	void GUICoolDownRender::AddPoint( const Vector2& position, const Vector2& uv, unsigned int nColor /*= 0xFFFFFFFF*/ )
// 	{
// 		if (m_nTriangleCount < MAX_COOL_DOWN_TRIANGLE_COUNT)
// 		{
// 			m_pBufferOffset[0].position = Vector3(position.x, position.y, 0.0f);
// 			m_pBufferOffset[0].color = nColor;
// 			m_pBufferOffset[0].texCoord = Vector2(uv.x, uv.y);
// 
// 			++m_nVertexCount;
// 			++m_nIndexCount;
// 			++m_pBufferOffset;
// 		}
// 		else if (m_nTriangleCount == MAX_COOL_DOWN_TRIANGLE_COUNT)
// 		{
// 			DrawBuffer();
// 		}
// 	}
// 
// 	void GUICoolDownRender::DrawBuffer()
// 	{
// 		// 先把前面的画了
// 		GUIRenderManager::Instance()->DrawBuffer();
// 
// 		if (m_nVertexCount>0 && m_pTexture)
// 		{
// 			Renderer* pRender = Renderer::Instance();
// 
// 			ShaderProgram* pShader = GUIRenderManager::Instance()->GetGUIShaderProgram();
// 
// 			pShader->bind();
// 			pShader->setMat4(GUIRenderManager::Instance()->GetUniformMatWVPIndex(), &SceneManager::Instance()->getGUICamera()->getViewProjMatrix());
// 			pShader->setSampler(GUIRenderManager::Instance()->GetUniformSamplerIndex(), 0);
// 
// 			pRender->setTexture(0, m_pTexture);
// 			Buffer vertBuffer(m_nVertexCount*sizeof(GUIVertexDeclaration), m_pVertexSystemMemory);
// 			m_pVertexBuffer->updateSubData(0, vertBuffer);
// 
// 			m_pRenderInput->unbind();
// 			m_pRenderInput->bindGPUBuffers(m_pVertexBuffer, sizeof(GUIVertexDeclaration), m_pIndexBuffer, sizeof(Word));
// 			m_pRenderInput->setVertexCount(m_nVertexCount);
// 			m_pRenderInput->setIndexCount(m_nIndexCount);
// 			Renderer::Instance()->render(m_pRenderInput);
// 		}
// 
// 		// reset
// 		m_nVertexCount = 0;
// 		m_nIndexCount = 0;
// 		m_nTriangleCount = 0;
// 		m_pBufferOffset = m_pVertexSystemMemory;
// 	}
// 
// }
