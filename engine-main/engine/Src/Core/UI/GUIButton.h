/********************************************************************
filename: 	GUIButton.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef __GUI_BUTTON_H__
#define __GUI_BUTTON_H__

#include "Core.h"
#include "GUIWindow.h"
#include "GUIRenderImage.h"
#include "GUIRenderHexagon.h"
#include "GUIRenderText.h"

namespace LORD
{
	class GUIImageset;
	class LORD_CORE_API GUIButton: public GUIWindow
	{
	public:
		enum ButtonState
		{
			BS_NORMAL,
			BS_PUSHED,
		};

		enum PlayState
		{
			PS_NONE,
			PS_DOWN,
			PS_UP,
			PS_STAY,
		};

		GUIButton(GUIWindowType nType, const GUIString& strName);
		virtual ~GUIButton();

		virtual bool	SetProperty(const GUIString& strName, const GUIString& strValue);
		virtual bool	GetProperty(const GUIString& strName, GUIString& strValue);	
		virtual void	GetPropertyList(GUIPropertyList& properlist);
		virtual bool	GetPropertyType(const GUIString& strName, GUIPropertyType& type);		

		virtual void	DrawSelf();

		virtual	void	UpdateSelf(ui32 nTimeElapse);

		void			SetTextColor(const Color& cColor);
		const Color&	GetTextColor() { return m_RenderText.GetTextColor(); }
		
		virtual bool	TouchDown(const Vector2& position);
		virtual bool	TouchUp(const Vector2& position);

		virtual bool	MotionRelease();

		void			SetNormalImage(const GUIString& strNormalImage);
		void			SetPushedImage(const GUIString& strPushedImage);
		void			SetHexagonImage(const GUIString& strTopImage, const GUIString& strSideImage);
		void			SetHexagonRenderHeight(float height);
		void			SetTextScale(float fScale) { m_fScale = fScale; }
		void			SetRenderOffset(Vector2 offset) { m_RenderText.setRenderOffset(offset); }

		virtual void	OnTextChanged();
		virtual void	OnSized();
		virtual void	OnFontChanged();

	protected:

		GUIRenderText	m_RenderText;
		ButtonState		m_eState;

		// renderData
		GUIRenderImage	m_NormalImage;
		GUIRenderImage	m_PushedImage;	

		// renderHenagon
		GUIRenderHexagon m_hexagon;

		// 点击效果
		ui32			m_nPlayedTime;
		// 原始大小
		URect			m_OrignRect;
		
		PlayState		m_ePlayState;

		//播放声音
		int				m_nSoundType;

		float				m_fScale;
	};
}

#endif