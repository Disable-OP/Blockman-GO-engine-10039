/********************************************************************
filename: 	GUIStaticImage.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef __GUI_STATIC_IMAGE_H__
#define __GUI_STATIC_IMAGE_H__

#include "Core.h"
#include "GUIWindow.h"
#include "GUIRenderImage.h"
#include "GUIRenderHexagon.h"

namespace LORD
{
	class GUIImage;
	class GUIImageset;
	class LORD_CORE_API GUIStaticImage: public GUIWindow
	{
	public:
		GUIStaticImage(GUIWindowType nType, const GUIString& strName);
		virtual ~GUIStaticImage();

		virtual bool	SetProperty(const GUIString& strName, const GUIString& strValue);
		virtual bool	GetProperty(const GUIString& strName, GUIString& strValue);	
		virtual void	GetPropertyList(GUIPropertyList& properlist);
		virtual bool	GetPropertyType(const GUIString& strName, GUIPropertyType& type);

		void			SetImage(const GUIString& strImageName);
		void			SetOverlayImage(const GUIString& strImageName);
		void			SetOverlayColor(const Color& color) { m_overlayColor = color; }
		void			SetHexagonImage(const GUIString& strTopImage, const GUIString& strSideImage);
		void			SetHexagonTopColor(const Color& color);
		void			SetHexagonSideColor(const Color& color);
		void			SetHexagonRenderHeight(float height);
		void			SetRenderAsBlock(bool renderAsBlock) { m_rederAsBlock = renderAsBlock; }
		void			SetVertexData(ImageBufferListType &vertex);

		virtual void	DrawSelf();

		// 用当前framebuff的texture模糊渲染
		void			setBlur(bool blur);

//		virtual void	UpdateSelf(ui32 nTimeElapse);

// 		void			SetCoolDownTime(ui32 nPassTime, ui32 nCoolDownTime, bool bPlay = true);
// 		bool			GetCoolDownStatus( ){ return m_bCoolDown; }

		void			changeTexture(const GUIString& strImageName);

	protected:
/*		void			SetCoolDown(float fCoolDownPercent);*/
		void			drawFromTexture(Texture* texture, bool isblur = false);

	protected:
		
		GUIRenderImage	m_RenderImage;

		/** add by zhouyou for render itemPotion. */
		GUIRenderImage  m_overlayImage;
		GUIString		m_strOverlayFile;
		Color			m_overlayColor;

		//renderHexagon
		GUIRenderHexagon m_hexagon;
		bool m_rederAsBlock;

// 		// cool down
// 		bool			m_bCoolDown;
// 		float			m_fMaskPercent;
// 		ui32			m_nCoolDownTime;
// 		ui32			m_nPassedTime;
// 		bool			m_bPlayCoolDown;

		// 是否是从文件创建的
		bool			m_isImageFromFile;
		GUIString		m_strTextureFile;
		Texture*		m_imageTexture;

		// 模糊纹理
		Texture*		m_blurTexture;
	};
}


#endif