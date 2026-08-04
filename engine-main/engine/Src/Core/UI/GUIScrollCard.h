/********************************************************************
filename: 	GUIScrollCard.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef __GUI_SCROLL_CARD_H__
#define __GUI_SCROLL_CARD_H__

#include "Core.h"
#include "GUIWindow.h"

namespace LORD
{
	class LORD_CORE_API GUIScrollCard: public GUIWindow
	{
	public:
		struct CardData
		{
			float fStartPosition;
			float fEndPosition;
			float fStartWidth;
			float fEndWidth;
			float fStartHeight;
			float fEndHeight;
			GUIWindow*	pWindow;
		};

		GUIScrollCard(GUIWindowType nType, const GUIString& strName);
		virtual ~GUIScrollCard();

		virtual bool	SetProperty(const GUIString& strName, const GUIString& strValue);
		virtual bool	GetProperty(const GUIString& strName, GUIString& strValue);	
		virtual void	GetPropertyList(GUIPropertyList& properlist);
		virtual bool	GetPropertyType(const GUIString& strName, GUIPropertyType& type);		
	
		virtual bool	TouchDown(const Vector2& position);
		virtual bool	TouchMove(const Vector2& position);
		virtual bool	TouchUp(const Vector2& position);
		virtual bool	MotionRelease();
		virtual void	UpdateSelf(ui32 nTimeElapse);

		void			AddCard(GUIWindow* pWindow);
		void			ClearAllCard();

		size_t			GetSelectCardIndex() { return m_nSelectCardIndex; }
		void			ScrollToCard(size_t nCardIndex, ui32 nSpeed = 300);
		void			ResetCardPos(size_t nCardIndex);

		bool			handleCardClicked(const EventArgs& e);

		size_t			GetTargetCardIndex() { return m_nTargetCardIndex; }

	protected:	
		void			ScaleChildText(GUIWindow* pWindow, float fScale);
		
		size_t					m_nSelectCardIndex;
		size_t					m_nTargetCardIndex;
		bool					m_bIsScrolling;
		ui32					m_nTimePassed;
		Vector2					m_cardSize;

		typedef vector<CardData>::type	CardList;
		CardList				m_cardList;
		Vector2					m_TouchDownPosition;
        
        bool                    m_bIsDragging;
        ui32                    m_nTouchDownTime;
        float                   m_fDragXPosition;
        ui32                    m_nScrollSpeed;
	};
}

#endif