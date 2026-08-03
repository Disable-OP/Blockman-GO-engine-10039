/********************************************************************
filename: 	DepthStencilView.h
file path:	dev\engine\Src\Core\Render\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_DEPTHSTENCILVIEW_H__
#define __LORD_DEPTHSTENCILVIEW_H__

#include "RenderView.h"

namespace LORD
{
	class LORD_CORE_API DepthStencilView: public RenderView
	{
	public:
		DepthStencilView(PixelFormat pixFmt, uint width, uint height);
		virtual ~DepthStencilView();

	public:
		virtual void		onAttached() = 0;
		virtual void		onDetached() = 0;

		virtual uint		getWidth() const;
		virtual uint		getHeight() const;

	protected:
		uint				m_width;
		uint				m_height;
	};
}

#endif