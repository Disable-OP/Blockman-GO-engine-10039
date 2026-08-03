/********************************************************************
filename: 	GLES2RenderState.h
file path:	dev\engine\Src\GLES2Render\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_GLES2RENDERSTATE_H__
#define __LORD_GLES2RENDERSTATE_H__

#include "Render/RenderState.h"

namespace LORD
{
	class LORD_GLES2RENDER_API GLES2BlendState: public BlendState
	{
	public:
		GLES2BlendState();
		GLES2BlendState(const BlendDesc &desc);
		~GLES2BlendState();

	public:
		void						active();
		void						setDesc(const BlendDesc& desc);
	private:
		void						create();

	private:
		GLenum						m_glBlendOP;
		GLenum						m_glAlphaBlendOP;
		GLenum						m_glSrcBlend;
		GLenum						m_glDstBlend;
		GLenum						m_glSrcAlphaBlend;
		GLenum						m_glDstAlphaBlend;
		GLboolean					m_glRedMask;
		GLboolean					m_glGreenMask;
		GLboolean					m_glBlueMask;
		GLboolean					m_glAlphaMask;
	};
	
	class LORD_GLES2RENDER_API GLES2DepthStencilState: public DepthStencilState
	{
	public:
		GLES2DepthStencilState();
		GLES2DepthStencilState(const DepthStencilDesc& desc);
		~GLES2DepthStencilState();

	public:
		void						active();
		void						setDesc(const DepthStencilDesc& desc);

	private:
		void						create();

	private:
		GLboolean					m_glDepthMask;
		GLenum						m_glDepthFunc;
		GLenum						m_glFrontStencilFunc;
		GLenum						m_glFrontStencilFailOP;
		GLenum						m_glFrontStencilDepthFailOP;
		GLenum						m_glFrontStencilPassOP;
		GLenum						m_glBackStencilFunc;
		GLenum						m_glBackStencilFailOP;
		GLenum						m_glBackStencilDepthFailOP;
		GLenum						m_glBackStencilPassOP;
	};
	
		class LORD_GLES2RENDER_API GLES2RasterizerState: public RasterizerState
	{
	public:
		GLES2RasterizerState();
		GLES2RasterizerState(const RasterizerDesc& desc);
		~GLES2RasterizerState();

	public:
		void						active();
		void						setDesc(const RasterizerDesc& desc);

	private:
		void						create();

	private:
		//GLenum					m_glPolygonMode;
		//GLenum					m_glShadeModel;
		GLenum						m_glFrontFace;
	};
	
		class LORD_GLES2RENDER_API GLES2SamplerState: public SamplerState
	{
	public:
		GLES2SamplerState();
		GLES2SamplerState(const SamplerDesc& desc);
		~GLES2SamplerState();

	public:
		GLint			getGLMinFilter() const;
		GLint			getGLMagFilter() const;
		GLint			getGLAddrModeU() const;
		GLint			getGLAddrModeV() const;
		GLint			getGLAddrModeW() const;
		void			bind2Texture(Texture* pTexture);
		void			setDesc(const SamplerDesc& desc);
	private:
		void			create();

	private:
		GLint			m_glMinFilter;
		GLint			m_glMagFilter;
		GLint			m_glAddrModeU;
		GLint			m_glAddrModeV;
		GLint			m_glAddrModeW;
	};
}

#endif
