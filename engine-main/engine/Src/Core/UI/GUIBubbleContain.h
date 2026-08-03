/********************************************************************
filename: 	GUIBubbleContain.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef __GUI_BUBBLE_CONTAIN_H__
#define __GUI_BUBBLE_CONTAIN_H__

#include "Core.h"
#include "GUIWindow.h"

namespace LORD
{
	enum ElemStatus
	{
		MOVE,
		STAY_MOVE,
		STAY_STOP
	};
	struct ElemInfo
	{
		GUIWindow*   pWnd;
		float          slidLen;
		float          maxSlidLen;
		i32          stayTime;			
		ElemStatus   status;
		ElemInfo(GUIWindow* wnd=NULL,ElemStatus es=MOVE,i32 stayt=0,float st=0,float mst=0.0f)
		{
			pWnd=wnd; status=es; stayTime=stayt;slidLen=st; maxSlidLen=mst;
		}
	};
	class LORD_CORE_API GUIBubbleContain: public GUIWindow
	{
	public:
		GUIBubbleContain(GUIWindowType nType, const GUIString& strName);
		~GUIBubbleContain(void);

		virtual bool	SetProperty(const GUIString& strName, const GUIString& strValue);
		virtual bool	GetProperty(const GUIString& strName, GUIString& strValue);	
		virtual void	GetPropertyList(GUIPropertyList& properlist);
		virtual bool	GetPropertyType(const GUIString& strName, GUIPropertyType& type);

		void   setOverlap(bool overlap) { m_bOverlap = overlap;}

		void   addItem(GUIString strValue, bool bStretched = false);
		void   setItemBackImageName(GUIString strValue);	
		void   setSpeedAndStayTime(float speed=7.0f, i32 stayTime=500);

		virtual void	DrawSelf();
		virtual void	UpdateSelf(ui32 nTimeElapse);		
		void AddChild_impl( GUIWindow* wnd );

	protected:

		void     MoveElemPosition(GUIWindow * wnd,float slidLen);
		void     OnImageChanged();
		
		vector<ElemInfo>::type m_vectElemInfo;

		i32     m_stayTime;
		float   m_goStep;

		GUIString m_strItemBackImageName;
		HorizontalAlignment m_hAlign;
		VerticalAlignment m_VAlign;
		bool    m_wordWrap;

		bool    m_bOverlap;
	};

	class LORD_CORE_API GUIBubbleContainEx: public GUIWindow
	{
	public:
		GUIBubbleContainEx(GUIWindowType nType, const GUIString& strName);
		~GUIBubbleContainEx(void);

		virtual bool	SetProperty(const GUIString& strName, const GUIString& strValue);
		virtual bool	GetProperty(const GUIString& strName, GUIString& strValue);	
		virtual void	GetPropertyList(GUIPropertyList& properlist);
		virtual bool	GetPropertyType(const GUIString& strName, GUIPropertyType& type);

		void   setOverlap(bool overlap) { m_bOverlap = overlap;}

		void   addItem(GUIString strValue,GUIString& strTemplate,GUIString& strText, bool bStretched = false);
		void   setItemBackImageName(GUIString strValue);	
		void   setSpeedAndStayTime(float speed=7.0f, i32 stayTime=500);

		virtual void	DrawSelf();
		virtual void	UpdateSelf(ui32 nTimeElapse);		
		void AddChild_impl( GUIWindow* wnd );

	protected:
		void     MoveElemPosition(GUIWindow * wnd,float slidLen);
		void     OnImageChanged();

		vector<ElemInfo>::type m_vectElemInfo;

		i32     m_stayTime;
		float   m_goStep;

		GUIString m_strItemBackImageName;
		HorizontalAlignment m_hAlign;
		VerticalAlignment m_VAlign;
		bool    m_wordWrap;

		bool    m_bOverlap;
	};
}
#endif

