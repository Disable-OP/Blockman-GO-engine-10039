/********************************************************************
filename: 	GLES2FrameBuffer.h
file path:	dev\engine\Src\GLES2Render\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_GLES2FRAMEBUFFER_H__
#define __LORD_GLES2FRAMEBUFFER_H__

#include "GLES2RenderTargetView.h"
#include "GLES2DepthStencilView.h"
#include "Render/FrameBuffer.h"

namespace LORD
{
	class LORD_GLES2RENDER_API GLES2FrameBuffer: public FrameBuffer
	{
	public:
		GLES2FrameBuffer();
		GLES2FrameBuffer(GLuint hFBO);
		~GLES2FrameBuffer();
	
	public:
		void			clear(Dword clearFlags, const Color& color, float depth, int stencil);
		inline void		swap();
		void			bind();

		GLuint			getFBOHandle() const;

	private:
		GLuint			m_hFBO;

		Color m_Color;
		float m_Depth;
		int m_Stencil;
	};
}

#endif
