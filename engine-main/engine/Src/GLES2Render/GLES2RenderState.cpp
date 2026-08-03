#include "GLES2RenderStd.h"
#include "GLES2RenderState.h"
#include "GLES2Mapping.h"
#include "GLES2Texture.h"
#include "Render/Renderer.h"
#include "Object/Exception.h"
#include "Object/Root.h"

namespace LORD
{
	/////////////////////////////////////////////////////////////////////////////////////////
	GLES2BlendState::GLES2BlendState()
	{
		create();
	}

	GLES2BlendState::GLES2BlendState(const BlendDesc& desc)
		: BlendState(desc)
	{
		create();
	}

	GLES2BlendState::~GLES2BlendState()
	{
	}

	void GLES2BlendState::active()
	{
		BlendState::BlendDesc* pCurState = Renderer::Instance()->getBlendState();
		if(pCurState)
		{
			const BlendDesc& curDesc = *pCurState;
			if(m_desc.bA2CEnable != curDesc.bA2CEnable)
			{
				if(m_desc.bA2CEnable)
					glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
				else
					glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
			}
			if(m_desc.bBlendEnable != curDesc.bBlendEnable)
			{
				if(m_desc.bBlendEnable)
					glEnable(GL_BLEND);
				else
					glDisable(GL_BLEND);
			}

			if(m_desc.blendOP != curDesc.blendOP)
				glBlendEquationSeparate(m_glBlendOP, m_glAlphaBlendOP);

			if((m_desc.srcBlend != curDesc.srcBlend) ||
				(m_desc.dstBlend != curDesc.dstBlend) ||
				(m_desc.srcAlphaBlend != curDesc.srcAlphaBlend) ||
				(m_desc.dstAlphaBlend != curDesc.dstAlphaBlend))
			{
				glBlendFuncSeparate(m_glSrcBlend, m_glDstBlend, m_glSrcAlphaBlend, m_glDstAlphaBlend);
			}

			if(m_desc.colorWriteMask != curDesc.colorWriteMask)
				glColorMask(m_glRedMask, m_glGreenMask, m_glBlueMask, m_glAlphaMask);

			if(m_desc.blendFactor != curDesc.blendFactor)
				glBlendColor((GLclampf)m_desc.blendFactor.r, (GLclampf)m_desc.blendFactor.g, (GLclampf)m_desc.blendFactor.b, (GLclampf)m_desc.blendFactor.a);
		}
		else
		{
			if(m_desc.bA2CEnable)
				glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
			else
				glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);

			if(m_desc.bBlendEnable)
				glEnable(GL_BLEND);
			else
				glDisable(GL_BLEND);

			glBlendEquationSeparate(m_glBlendOP, m_glAlphaBlendOP);
			glBlendFuncSeparate(m_glSrcBlend, m_glDstBlend, m_glSrcAlphaBlend, m_glDstAlphaBlend);
			glColorMask(m_glRedMask, m_glGreenMask, m_glBlueMask, m_glAlphaMask);
			glBlendColor((GLclampf)m_desc.blendFactor.r, (GLclampf)m_desc.blendFactor.g, (GLclampf)m_desc.blendFactor.b, (GLclampf)m_desc.blendFactor.a);
		}
	}

	void GLES2BlendState::create()
	{
		m_glBlendOP = GLES2Mapping::MapBlendOperation(m_desc.blendOP);
		m_glAlphaBlendOP = GLES2Mapping::MapBlendOperation(m_desc.alphaBlendOP);
		m_glSrcBlend = GLES2Mapping::MapBlendFactor(m_desc.srcBlend);
		m_glDstBlend = GLES2Mapping::MapBlendFactor(m_desc.dstBlend);
		m_glSrcAlphaBlend = GLES2Mapping::MapBlendFactor(m_desc.srcAlphaBlend);
		m_glDstAlphaBlend = GLES2Mapping::MapBlendFactor(m_desc.dstAlphaBlend);
		m_glRedMask = (m_desc.colorWriteMask & CMASK_RED) != 0;
		m_glGreenMask = (m_desc.colorWriteMask & CMASK_GREEN) != 0;
		m_glBlueMask = (m_desc.colorWriteMask & CMASK_BLUE) != 0;
		m_glAlphaMask = (m_desc.colorWriteMask & CMASK_ALPHA) != 0;
	}

	void GLES2BlendState::setDesc(const BlendDesc& desc)
	{
		memcpy(&m_desc, &desc, sizeof(BlendDesc));
		create();
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	GLES2DepthStencilState::GLES2DepthStencilState()
	{
		create();
	}

	GLES2DepthStencilState::GLES2DepthStencilState(const DepthStencilDesc &desc)
		: DepthStencilState(desc)
	{
		create();
	}

	GLES2DepthStencilState::~GLES2DepthStencilState()
	{
	}

	void GLES2DepthStencilState::active()
	{
		DepthStencilState::DepthStencilDesc* pCurState = Renderer::Instance()->getDepthStencilState();
		if(pCurState)
		{
			const DepthStencilDesc& curDesc = *pCurState;
			if(m_desc.bDepthEnable != curDesc.bDepthEnable)
			{
				if(m_desc.bDepthEnable)
					glEnable(GL_DEPTH_TEST);
				else
					glDisable(GL_DEPTH_TEST);
			}

			if(m_desc.bWriteDepth != curDesc.bWriteDepth)
				glDepthMask(m_glDepthMask);

			if(m_desc.depthFunc != curDesc.depthFunc)
				glDepthFunc(m_glDepthFunc);

			if((m_desc.frontStencilFunc != curDesc.frontStencilFunc) ||
				(m_desc.frontStencilRef != curDesc.frontStencilRef) ||
				(m_desc.frontStencilReadMask != curDesc.frontStencilReadMask))
			{
				glStencilFuncSeparate(GL_FRONT, m_glFrontStencilFunc, m_desc.frontStencilRef, m_desc.frontStencilReadMask);
			}

			if((m_desc.frontStencilFailOP != curDesc.frontStencilFailOP) ||
				(m_desc.frontStencilDepthFailOP != curDesc.frontStencilDepthFailOP) ||
				(m_desc.frontStencilPassOP != curDesc.frontStencilPassOP))
			{
				glStencilOpSeparate(GL_FRONT, m_glFrontStencilFailOP, m_glFrontStencilDepthFailOP, m_glFrontStencilPassOP);
			}

			if(m_desc.frontStencilWriteMask != curDesc.frontStencilWriteMask)
			{
				glStencilMaskSeparate(GL_FRONT, m_desc.frontStencilWriteMask);
			}

			if((m_desc.backStencilFunc != curDesc.backStencilFunc) ||
				(m_desc.backStencilRef != curDesc.backStencilRef) ||
				(m_desc.backStencilReadMask != curDesc.backStencilReadMask))
			{
				glStencilFuncSeparate(GL_BACK, m_glBackStencilFunc, m_desc.backStencilRef, m_desc.backStencilReadMask);
			}

			if((m_desc.backStencilFailOP != curDesc.backStencilFailOP) ||
				(m_desc.backStencilDepthFailOP != curDesc.backStencilDepthFailOP) ||
				(m_desc.backStencilPassOP != curDesc.backStencilPassOP))
			{
				glStencilOpSeparate(GL_BACK, m_glBackStencilFailOP, m_glBackStencilDepthFailOP, m_glBackStencilPassOP);
			}

			if(m_desc.backStencilWriteMask != curDesc.backStencilWriteMask)
			{
				glStencilMaskSeparate(GL_BACK, m_desc.backStencilWriteMask);
			}

			if((m_desc.bFrontStencilEnable != curDesc.bFrontStencilEnable) ||
				(m_desc.bBackStencilEnable != curDesc.bBackStencilEnable))
			{
				if(m_desc.bFrontStencilEnable || m_desc.bBackStencilEnable)
					glEnable(GL_STENCIL_TEST);
				else
					glDisable(GL_STENCIL_TEST);
			}
		}
		else
		{
			if(m_desc.bDepthEnable)
				glEnable(GL_DEPTH_TEST);
			else
				glDisable(GL_DEPTH_TEST);

			glDepthMask(m_glDepthMask);
			glDepthFunc(m_glDepthFunc);
			glStencilFuncSeparate(GL_FRONT, m_glFrontStencilFunc, m_desc.frontStencilRef, m_desc.frontStencilReadMask);
			glStencilOpSeparate(GL_FRONT, m_glFrontStencilFailOP, m_glFrontStencilDepthFailOP, m_glFrontStencilPassOP);
			glStencilMaskSeparate(GL_FRONT, m_desc.frontStencilWriteMask);
			glStencilFuncSeparate(GL_BACK, m_glBackStencilFunc, m_desc.backStencilRef, m_desc.backStencilReadMask);
			glStencilOpSeparate(GL_BACK, m_glBackStencilFailOP, m_glBackStencilDepthFailOP, m_glBackStencilPassOP);
			glStencilMaskSeparate(GL_BACK, m_desc.backStencilWriteMask);

			if(m_desc.bFrontStencilEnable || m_desc.bBackStencilEnable)
				glEnable(GL_STENCIL_TEST);
			else
				glDisable(GL_STENCIL_TEST);
		}
	}

	void GLES2DepthStencilState::create()
	{
		m_glDepthMask = m_desc.bWriteDepth? GL_TRUE : GL_FALSE;
		m_glDepthFunc = GLES2Mapping::MapComparisonFunc(m_desc.depthFunc);
		m_glFrontStencilFunc = GLES2Mapping::MapComparisonFunc(m_desc.frontStencilFunc);
		m_glFrontStencilFailOP = GLES2Mapping::MapStencilOperation(m_desc.frontStencilFailOP);
		m_glFrontStencilDepthFailOP = GLES2Mapping::MapStencilOperation(m_desc.frontStencilDepthFailOP);
		m_glFrontStencilPassOP = GLES2Mapping::MapStencilOperation(m_desc.frontStencilPassOP);
		m_glBackStencilFunc = GLES2Mapping::MapComparisonFunc(m_desc.backStencilFunc);
		m_glBackStencilFailOP = GLES2Mapping::MapStencilOperation(m_desc.backStencilFailOP);
		m_glBackStencilDepthFailOP = GLES2Mapping::MapStencilOperation(m_desc.backStencilDepthFailOP);
		m_glBackStencilPassOP = GLES2Mapping::MapStencilOperation(m_desc.backStencilPassOP);
	}

	void GLES2DepthStencilState::setDesc(const DepthStencilDesc& desc)
	{
		memcpy(&m_desc, &desc, sizeof(DepthStencilDesc));
		create();
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	GLES2RasterizerState::GLES2RasterizerState()
	{
		create();
	}

	GLES2RasterizerState::GLES2RasterizerState(const RasterizerDesc& desc)
		: RasterizerState(desc)
	{
		create();
	}

	GLES2RasterizerState::~GLES2RasterizerState()
	{
	}

	void GLES2RasterizerState::active()
	{
		RasterizerState::RasterizerDesc* pCurState = Renderer::Instance()->getRasterizerState();
		if(pCurState)
		{
			const RasterizerDesc& curDesc = *pCurState;
			if(m_desc.cullMode != curDesc.cullMode)
			{
				switch (m_desc.cullMode)
				{
					case RasterizerState::CULL_NONE:
					{
						glDisable(GL_CULL_FACE);
					} break;
					case RasterizerState::CULL_FRONT:
					{
						glEnable(GL_CULL_FACE);
						glCullFace(GL_FRONT);
					} break;
					case RasterizerState::CULL_BACK:
					{
						glEnable(GL_CULL_FACE);
						glCullFace(GL_BACK);
					} break;
				}
			}

			if (m_desc.lineWidth != curDesc.lineWidth)
				glLineWidth(float(m_desc.lineWidth));

			if(m_desc.bFrontFaceCCW != curDesc.bFrontFaceCCW)
				glFrontFace(m_glFrontFace);

			if((m_desc.depthBiasFactor != curDesc.depthBiasFactor) ||
			   (m_desc.depthBias != curDesc.depthBias))
			{
				if(m_desc.depthBias != 0.0f && m_desc.depthBiasFactor != 0.0f)
					glEnable(GL_POLYGON_OFFSET_FILL);
				else
					glDisable(GL_POLYGON_OFFSET_FILL);

				glPolygonOffset(m_desc.depthBiasFactor, m_desc.depthBias);
			}

			if (m_desc.bScissor != curDesc.bScissor)
			{
				if (m_desc.bScissor)
				{
					glEnable(GL_SCISSOR_TEST);
				}
				else
				{
					glDisable(GL_SCISSOR_TEST);
				}
			}
		}
		else
		{

			// set polygon mode
			// mark: only support polygon fill mode
			//glPolygonMode(GL_FRONT_AND_BACK, m_polygonMode);

			// set shade model
			// mark: flat shading is not supported, only support smooth shading
			//glShadeModel(m_shadeModel);

			// set cull mode
			switch (m_desc.cullMode)
			{
				case RasterizerState::CULL_NONE:
				{
					glDisable(GL_CULL_FACE);
				} break;
				case RasterizerState::CULL_FRONT:
				{
					glEnable(GL_CULL_FACE);
					glCullFace(GL_FRONT);
				} break;
				case RasterizerState::CULL_BACK:
				{
					glEnable(GL_CULL_FACE);
					glCullFace(GL_BACK);
				} break;
			}

			// set front face
			glFrontFace(m_glFrontFace);

			// Bias is in {0, 16}, scale the unit addition appropriately
			if(m_desc.depthBias != 0.0f || m_desc.depthBiasFactor != 0.0f)
				glEnable(GL_POLYGON_OFFSET_FILL);
			else
				glDisable(GL_POLYGON_OFFSET_FILL);
			
			glPolygonOffset(m_desc.depthBiasFactor, m_desc.depthBias);

			// depth clip
			/*
			if (m_desc.bDepthClip)
			{
				glDisable(GL_DEPTH_CLAMP);
			}
			else
			{
				glEnable(GL_DEPTH_CLAMP);
			}
			*/

			// scissor test
			if (m_desc.bScissor)
			{
				glEnable(GL_SCISSOR_TEST);
			}
			else
			{
				glDisable(GL_SCISSOR_TEST);
			}

			// multi-sample
			// mark: ios uses glResolveMultisampleFramebufferAPPLE function to multi sample.
			/*
			if (m_desc.bMultisample)
			{
				glEnable(GL_MULTISAMPLE);
				//glResolveMultisampleFramebufferAPPLE
			}
			else
			{
				glDisable(GL_MULTISAMPLE);
			}
			*/
		}
	}

	void GLES2RasterizerState::create()
	{
		if(m_desc.polygonMode != PM_FILL)
			LordException("GLES2Renderer only support polygon fill mode [PM_FILL]. Check polygonMode property.");
		
		if(m_desc.shadeModel != SM_GOURAND)
			LordException("GLES2Renderer only support smooth shading [SM_SMOOTH]. Check shadeModel property.");
		
		m_glFrontFace = m_desc.bFrontFaceCCW? GL_CCW : GL_CW;
	}

	void GLES2RasterizerState::setDesc(const RasterizerDesc& desc)
	{
		memcpy(&m_desc, &desc, sizeof(RasterizerDesc));
		create();
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	GLES2SamplerState::GLES2SamplerState()
	{
		create();
	}

	GLES2SamplerState::GLES2SamplerState(const SamplerDesc& desc)
		: SamplerState(desc)
	{
		create();
	}

	GLES2SamplerState::~GLES2SamplerState()
	{
	}

	void GLES2SamplerState::bind2Texture(Texture* pTexture)
	{
		if (pTexture)
		{
			GLuint hTexture = ((GLES2Texture*)pTexture)->getTextureHandle();
			GLenum glTarget = GLES2Mapping::MapTextureType(pTexture->getType());

			glBindTexture(glTarget, hTexture);

			// only 2D texture type.
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_glMinFilter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_glMagFilter);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_glAddrModeU);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_glAddrModeV);
			//glTexParameteri(glTarget, GL_TEXTURE_WRAP_R, m_glAddrModeW);
		}
	}

	void GLES2SamplerState::create()
	{
		if(m_desc.minFilter == FO_NONE)
		{
			m_glMinFilter = 0;
		}
		else if(m_desc.minFilter == FO_LINEAR || m_desc.minFilter == FO_ANISOTROPIC)
		{
			if(m_desc.mipFilter == FO_NONE)
				m_glMinFilter = GL_LINEAR;
			else if(m_desc.mipFilter == FO_LINEAR || m_desc.mipFilter == FO_ANISOTROPIC)
				m_glMinFilter = GL_LINEAR_MIPMAP_LINEAR;
			else
				m_glMinFilter = GL_LINEAR_MIPMAP_NEAREST;
		}
		else
		{
			if(m_desc.mipFilter == FO_NONE)
				m_glMinFilter = GL_NEAREST;
			else if(m_desc.mipFilter == FO_LINEAR || m_desc.mipFilter == FO_ANISOTROPIC)
				m_glMinFilter = GL_NEAREST_MIPMAP_LINEAR;
			else
				m_glMinFilter = GL_NEAREST_MIPMAP_NEAREST;
		}

		if(m_desc.magFilter == FO_NONE)
			m_glMagFilter = 0;
		else if(m_desc.magFilter == FO_LINEAR || m_desc.magFilter == FO_ANISOTROPIC)
			m_glMagFilter = GL_LINEAR;
		else
			m_glMagFilter = GL_NEAREST;

		m_glAddrModeU = GLES2Mapping::MapAddressMode(m_desc.addrUMode);
		m_glAddrModeV = GLES2Mapping::MapAddressMode(m_desc.addrVMode);
		m_glAddrModeW = GLES2Mapping::MapAddressMode(m_desc.addrWMode);
	}

	GLint GLES2SamplerState::getGLMinFilter() const
	{
		return m_glMinFilter;
	}

	GLint GLES2SamplerState::getGLMagFilter() const
	{
		return m_glMagFilter;
	}

	GLint GLES2SamplerState::getGLAddrModeU() const
	{
		return m_glAddrModeU;
	}

	GLint GLES2SamplerState::getGLAddrModeV() const
	{
		return m_glAddrModeV;
	}

	GLint GLES2SamplerState::getGLAddrModeW() const
	{
		return m_glAddrModeW;
	}

	void GLES2SamplerState::setDesc(const SamplerDesc& desc)
	{
		memcpy(&m_desc, &desc, sizeof(SamplerDesc));
		create();
	}

}
