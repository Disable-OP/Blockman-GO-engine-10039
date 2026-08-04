/********************************************************************
filename: 	GUIGridView.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef __GUI_GRID_VIEW_H__
#define __GUI_GRID_VIEW_H__

#include "GUIWindow.h"
#include "GUIScroll.h"
#include "GUIMutualNonoverlappingGroup.h"

namespace LORD
{	
	class LORD_CORE_API GUIGridView: public GUIWindow
	{
	public:		

		GUIGridView(GUIWindowType nType, const GUIString& strName);
		virtual ~GUIGridView();

		void   InitConfig(float hInterval,float vInterval,i32 rowSize);
		void   AddItem(GUIWindow* pItem);
		size_t GetItemCount() { return m_container ? m_container->GetChildCount() : 0; }
		GUIWindow* GetItem(size_t index) { return m_container ? m_container->GetChildByIndex(index) : nullptr; }
		void   RemoveItem( GUIWindow* pItem );
		void   RemoveAllItems()
		{ 
			if (m_container)
			{
				m_container->CleanupChildren();
			}
		}
		
		void   HasItemHidden(bool bparam) { m_bQuickFind = bparam ;}

		virtual bool	SetProperty(const GUIString& strName, const GUIString& strValue);
		virtual bool	GetProperty(const GUIString& strName, GUIString& strValue);	
		virtual void	GetPropertyList(GUIPropertyList& properlist);
		virtual bool	GetPropertyType(const GUIString& strName, GUIPropertyType& type);

		virtual bool	TouchDown(const Vector2& position);
		virtual bool	TouchMove(const Vector2& position);
		virtual bool	TouchUp(const Vector2& position);
		virtual bool	MotionRelease();
		virtual void	UpdateSelf(ui32 nTimeElapse);		
		virtual	void	ResetPos( );

		void			SetMoveAble(bool moveAble) { m_moveAble = moveAble; };
		void			SetItemAlignment(HorizontalAlignment align);

		virtual void	OnEndDraw();

		GUIWindow* getChildWindow(int index){ return m_container ? m_container->GetChildByIndex(index) : nullptr; }
		int GetChildIndex(const GUIString & strName);
		void			RestChildVisible();
		float			GetScrollOffset();
		void			SetScrollOffset(float offSet);
		float			GetContainerHeight() { return m_fHeight; }
		GUIWindow* GetContainer() { return m_container; }

	protected:
		void   LayoutItems();

	protected:		
		GUIMutualNonoverlappingGroup* m_container;
		float			m_fHeight;
		float			m_fYOffset;
		GUIScroll		m_vertScroll;
		//定位辅助参数
		bool            m_bQuickFind;
		//表格参数
		float           m_fHInterval;
		float           m_fVInterval;
		size_t          m_rowSize;        
		bool			m_moveAble;
		HorizontalAlignment m_itemAlignment;
	};
}

#endif

