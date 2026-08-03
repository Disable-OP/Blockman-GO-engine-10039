/********************************************************************
filename: 	GLES2RenderTargetView.h
file path:	dev\engine\Src\GLES2Render\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_GLES2RENDERTARGETVIEW_H__
#define __LORD_GLES2RENDERTARGETVIEW_H__

#include "Render/RenderTargetView.h"

namespace LORD
{
	class LORD_GLES2RENDER_API GLES2RenderTargetView: public RenderTargetView
	{
	public:
		GLES2RenderTargetView(PixelFormat pixFmt, uint width, uint height);

		// for creating default RTV
		GLES2RenderTargetView(uint hRBO, uint width, uint height);

		~GLES2RenderTargetView();

	public:
		void		onAttached(uint idx = 0);
		void		onDetached();
	
		uint		getRBOHandle() const;
	
	private:
		uint		m_hRBO;
	};
}

#endif
