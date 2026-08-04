/********************************************************************
filename: 	RenderView.h
file path:	dev\engine\Src\Core\Render\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_RENDERVIEW_H__
#define __LORD_RENDERVIEW_H__

#include "Core.h"

namespace LORD
{
	class LORD_CORE_API RenderView: public ObjectAlloc
	{
	public:
		enum RenderViewType
		{
			RVT_RENDERTARGET, 
			RVT_DEPTHSTENCIL, 
		};

	public:
		RenderView(RenderViewType type, PixelFormat pixFmt);
		virtual ~RenderView();

	public:
		virtual RenderViewType		getRenderViewType() const;
		virtual PixelFormat			getPixelFormat() const;

	protected:
		RenderViewType		m_type;
		PixelFormat			m_pixFmt;
	};
}

#endif