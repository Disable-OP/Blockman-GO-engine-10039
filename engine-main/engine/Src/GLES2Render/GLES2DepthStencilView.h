/********************************************************************
filename: 	GLES22DDepthStencilView.h
file path:	dev\engine\Src\GLES2Render\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_GLES2DEPTHSTENCILVIEW_H__
#define __LORD_GLES2DEPTHSTENCILVIEW_H__

#include "Render/DepthStencilView.h"

namespace LORD
{
	class LORD_GLES2RENDER_API GLES2DepthStencilView: public DepthStencilView
	{
	public:
		GLES2DepthStencilView(PixelFormat pixFmt, uint width, uint height);
		~GLES2DepthStencilView();

	public:
		void		onAttached();
		void		onDetached();

		GLuint		getRBOHandle() const;
	
	private:
		GLuint		m_hRBO;
	};
}

#endif
