/********************************************************************
filename: 	RenderTargetView.h
file path:	dev\engine\Src\Core\Render\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_RENDERTARGETVIEW_H__
#define __LORD_RENDERTARGETVIEW_H__

#include "RenderView.h"

namespace LORD
{
	class LORD_CORE_API RenderTargetView: public RenderView
	{
	public:
		RenderTargetView(PixelFormat pixFmt, uint width, uint height);
		virtual ~RenderTargetView();

	public:
		virtual void		onAttached(uint idx) = 0;
		virtual void		onDetached() = 0;

		virtual int			getAttachedIndex() const;
		virtual uint		getWidth() const;
		virtual uint		getHeight() const;

	protected:
		int					m_attIdx;
		uint				m_width;
		uint				m_height;
	};

}

#endif