/********************************************************************
filename: 	GUIRenderImage.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef __GUI_RENDER_IMAGE_H__
#define __GUI_RENDER_IMAGE_H__

#include "Core.h"
#include "GUIString.h"
#include "GUICommonDef.h"
#include "Resource/LordThread.h"
#include "GUIImage.h"

namespace LORD
{
#define CACHEGPUBUFFER
	class GUIImageset;
	class GUIWindow;
	class GUIImage;

	class LORD_CORE_API GUIRenderImage: public ObjectAlloc
	{
		friend class RenderImagePrepareEvent;

	public:

		GUIRenderImage();
		~GUIRenderImage();

		void			SetWholeImageName(const GUIString& strWholeName);
		void			DrawImage(const GUIWindow* window, const ERect& rtArea, unsigned int nColor, float fRotate, 
								float fRotateY = 0.0f, float fRotateX = 0.0f, const ERect* rtClip = NULL);

		void			DrawStretchImage(const GUIWindow* window, const ERect& rtArea, unsigned int nColor, StretchType stretchType, const StretchOffset& offset, float fRotate,
										float fRotateY = 0.0f, float fRotateX = 0.0f, const ERect* rtClip = NULL);
		
		const GUIString& GetWholeImageName() const { return m_strWholeImageName; }
		Vector2			GetImageSize();


		// 为单线程加载用
		void			prepare_io();

		void			prepare_image();

		void			checkImage();

	protected:
		void			drawCatheImage(const GUIWindow* window);

	private:
		GUIString		m_strWholeImageName;
		GUIString		m_strImagesetName;
		GUIString		m_strImageName;
		GUIImageset*	m_pImageset;
		GUIImage*		m_pImage;

		ImageBufferListType	m_actionVertex;
		VertexListType		m_actionVertexLR;
		VertexListType		m_actionVertexTB;
		VertexListType		m_actionVertexNine;

#ifdef CACHEGPUBUFFER
		ERect m_rtArea;
		unsigned int m_nColor;
		float m_fRotate;
		float m_fRotateY;
		float m_fRotateX;
		ERect m_rtClip;
		StretchOffset m_StretchOffset;
		StretchType m_StretchType;

		GUIVertexDeclaration*	m_pVertexSystemMemory;
		Word*					m_pIndexSystemMemory;
		GPUBuffer*				m_pVertexBuffer;
		GPUBuffer*				m_pIndexBuffer;
		int						m_VertexCount;
		int						m_IndexCount;
		bool					m_UpdateData;
#endif
	};

	class RenderImagePrepareEvent : public ThreadEvent
	{
	public:
		RenderImagePrepareEvent(GUIImageset* imageset);
		virtual ~RenderImagePrepareEvent();

		virtual bool ProcessEvent();

		virtual bool RespondEvent();

		virtual bool GetRepeatRespondIfFail()	{ return true; }
		virtual bool IsAutoDestroy()			{ return true; }

	protected:
		GUIImageset*	m_imageset;
	};

	class ImagesetReleaseEvent : public ThreadEvent
	{
	public:
		ImagesetReleaseEvent(GUIImageset* imageset);
		virtual ~ImagesetReleaseEvent();

		virtual bool ProcessEvent();

		virtual bool RespondEvent();

		virtual bool GetRepeatRespondIfFail()	{ return true; }
		virtual bool IsAutoDestroy()			{ return true; }

	protected:
		GUIImageset*	m_imageset;
	};
}

#endif
