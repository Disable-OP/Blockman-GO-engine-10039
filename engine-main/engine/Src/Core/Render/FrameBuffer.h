/********************************************************************
filename: 	FrameBuffer.h
file path:	dev\engine\Src\Core\Render\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_FRAMEBUFFER_H__
#define __LORD_FRAMEBUFFER_H__

#include "Core.h"

namespace LORD
{
	class LORD_CORE_API FrameBuffer: public ObjectAlloc
	{
	public:
		static const uint MAX_RENDERTARGETS = 4;

	public:
		FrameBuffer();
		virtual ~FrameBuffer();

		enum ClearMask
		{
			CM_COLOR	= 0x00000001, 
			CM_DEPTH	= 0x00000002, 
			CM_STENCIL	= 0x00000004, 
		};

	public:
		virtual void					clear(Dword clearFlags, const Color& color, float depth, int stencil) = 0;
		virtual void					swap() = 0;
		virtual void					bind() = 0;

		virtual void					attachRTV(uint idx, RenderTargetView* pRTV);
		virtual void					detachRTV(uint idx);
		virtual void					attachDSV(DepthStencilView* pDSV);
		virtual void					detachDSV();
		virtual RenderTargetView*		getRenderTargetView(uint idx) const;
		virtual DepthStencilView*		getDepthStencilView() const;
		virtual void					setViewport(Viewport* pViewport);
		virtual Viewport*				getViewport() const; 

	protected:
		RenderTargetView*		m_pRTVs[MAX_RENDERTARGETS];
		DepthStencilView*		m_pDSV;
		Viewport*				m_pViewport;
	};
}

#endif