#include "backgroundRenderable.h"
#include "Render/Material.h"
#include "Render/ShaderProgramManager.h"

#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include "rapidxml_print.hpp"

namespace LORD
{
	BackGroundRenderable::BackGroundRenderable(const String& texName)
		:Renderable()
	{
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

		m_Material = LordNew Material;
		{
			DepthStencilState::DepthStencilDesc depthdesc = m_Material->getDepthState()->getDesc();
			depthdesc.bDepthEnable = false;
			depthdesc.bWriteDepth = false;
			depthdesc.depthFunc = RenderState::CF_ALWAYS;

			SamplerState::SamplerDesc samdesc = m_Material->getSamplerState(0)->getDesc();
			samdesc.minFilter = SamplerState::FO_LINEAR;
			samdesc.magFilter = SamplerState::FO_LINEAR;
			samdesc.mipFilter = SamplerState::FO_NONE;

			RasterizerState::RasterizerDesc rasterdesc = m_Material->getRasterizerState()->getDesc();
			rasterdesc.cullMode = RasterizerState::CULL_NONE;

			m_Material->setDepthState(depthdesc);
			m_Material->setSamplerState(0, samdesc);
			m_Material->setRasterizerState(rasterdesc);
		}

		Renderer* pRenderer = Renderer::Instance();
		ShaderProgram* shaderProgram = ShaderProgramManager::Instance()->getShaderProgram("BackgroundForEditor");
		m_Material->setShaderProgram(shaderProgram);
		m_pRenderQueue = SceneManager::Instance()->getRenderQueue("Background");
		m_RenderInput = pRenderer->createRenderInput(m_VertexElement, shaderProgram);

		m_defaultBG = texName;
		m_pTexture = TextureManager::Instance()->createTexture(m_defaultBG);
		m_pTexture->load();
		m_Material->activeTexture(0, m_pTexture);

		unsigned int nVertexCount = 4;
		unsigned int nIndexCount = 6;
		float texHeight = float(m_pTexture->getHeight());
		float texWidth = float(m_pTexture->getWidth());
		BackGroundVertexType* vertices = NULL;
		vertices = LordNewArrayT(BackGroundVertexType, nVertexCount);
		// init vertices
		vertices[0].position = Vector3(0, 0, 0);
		vertices[0].color = 0xFFFFFFFF;
		vertices[0].texCoord = Vector2(0, 0);
		vertices[1].position = Vector3(texWidth, -texHeight, 0);
		vertices[1].color = 0xFFFFFFFF;
		vertices[1].texCoord = Vector2(1, 1);
		vertices[2].position = Vector3(0, -texHeight, 0);
		vertices[2].color = 0xFFFFFFFF;
		vertices[2].texCoord = Vector2(0, 1);
		vertices[3].position = Vector3(texWidth, 0, 0);
		vertices[3].color = 0xFFFFFFFF;
		vertices[3].texCoord = Vector2(1, 0);

		Buffer vertBuffer(nVertexCount*sizeof(BackGroundVertexType), vertices);
		m_vetexBuffer = Renderer::Instance()->createVertexBuffer(GPUBuffer::GBU_DEFAULT, vertBuffer);

		Word* indices = NULL;
		indices = LordNewArrayT(Word, nIndexCount);

		Word* quadStart = indices;

		quadStart[0] = 0;
		quadStart[1] = 1;
		quadStart[2] = 2;
		quadStart[3] = 0;
		quadStart[4] = 3;
		quadStart[5] = 1;
		
		Buffer idxBuffer(nIndexCount*sizeof(Word), indices);
		m_indexBuffer = Renderer::Instance()->createIndexBuffer(GPUBuffer::GBU_DEFAULT, idxBuffer);

		m_RenderInput->bindGPUBuffers(m_vetexBuffer, sizeof(BackGroundVertexType), m_indexBuffer, sizeof(Word));
		LordDeleteArrayT(vertices, BackGroundVertexType, nVertexCount);
		LordDeleteArrayT(indices, Word, nIndexCount );

		m_Material->beginShaderParams(2);
		m_Material->setShaderParam("matWVP", SPT_MAT4, (void*)&m_matWVP);
		m_Material->setShaderParam("texSampler", SPT_TEXTURE, (void*)&TEXTURE_UNIT0);
		m_Material->endShaderParams();
	}

	BackGroundRenderable::~BackGroundRenderable()
	{
		TextureManager::Instance()->releaseResource(m_pTexture);
		LordSafeDelete(m_Material);
		LordSafeDelete(m_RenderInput);
		LordSafeDelete(m_vetexBuffer);
		LordSafeDelete(m_indexBuffer);
	}

	void BackGroundRenderable::changeTexture(const String& texName)
	{
		TextureManager::Instance()->releaseResource(m_pTexture);
		m_pTexture = TextureManager::Instance()->createTexture(texName);
		if (!m_pTexture->load())
		{
			TextureManager::Instance()->releaseResource(m_pTexture);
			m_pTexture = TextureManager::Instance()->createTexture(m_defaultBG);
			m_pTexture->load();
		}

		m_Material->activeTexture(0, m_pTexture);

		LordSafeDelete(m_vetexBuffer);

		unsigned int nVertexCount = 4;
		float texHeight = float(m_pTexture->getHeight());
		float texWidth = float(m_pTexture->getWidth());
		BackGroundVertexType* vertices = NULL;
		vertices = LordNewArrayT(BackGroundVertexType, nVertexCount);
		// init vertices
		vertices[0].position = Vector3(0, 0, 0);
		vertices[0].color = 0xFFFFFFFF;
		vertices[0].texCoord = Vector2(0, 0);
		vertices[1].position = Vector3(texWidth, -texHeight, 0);
		vertices[1].color = 0xFFFFFFFF;
		vertices[1].texCoord = Vector2(1, 1);
		vertices[2].position = Vector3(0, -texHeight, 0);
		vertices[2].color = 0xFFFFFFFF;
		vertices[2].texCoord = Vector2(0, 1);
		vertices[3].position = Vector3(texWidth, 0, 0);
		vertices[3].color = 0xFFFFFFFF;
		vertices[3].texCoord = Vector2(1, 0);


		Buffer vertBuffer(nVertexCount*sizeof(BackGroundVertexType), vertices);
		m_vetexBuffer = Renderer::Instance()->createVertexBuffer(GPUBuffer::GBU_DEFAULT, vertBuffer);
		m_RenderInput->bindGPUBuffers(m_vetexBuffer, sizeof(BackGroundVertexType), m_indexBuffer, sizeof(Word));
	}

	void BackGroundRenderable::update()
	{
		Viewport* pViewport = Renderer::Instance()->getMainViewport(); //getFrameBuffer()->getViewport();
		Real xOffset = -(Real)pViewport->getWidth() / 2.0f;
		Real yOffset = (Real)pViewport->getHeight() / 2.0f;
		float texW = float(m_pTexture->getWidth());
		float texH = float(m_pTexture->getHeight());

		const Matrix4& matVP = pViewport->getViewProjMatrix();
		
		Matrix4 matWorld;
		matWorld.rotateAxisReplace(Vector3::UNIT_Z, 0);
		matWorld.scale(pViewport->getWidth()/texW, pViewport->getHeight()/texH, 1.f);
		matWorld.translate(xOffset, yOffset, 0);
		m_matWVP = matWorld * matVP;
	}

	void BackGroundRenderable::render()
	{
		if (m_pRenderQueue)
		{
			m_pRenderQueue->addRenderable(this);
		}
	}
}