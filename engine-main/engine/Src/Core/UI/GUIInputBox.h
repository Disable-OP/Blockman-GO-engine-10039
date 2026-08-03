/********************************************************************
filename: 	GUIInputBox.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef __GUI_INPUT_BOX_H__
#define __GUI_INPUT_BOX_H__

#include "Core.h"
#include "GUIFont.h"
#include "GUIRenderImage.h"
#include "GUIWindow.h"
#include "GUIRenderText.h"

namespace LORD
{
	class LORD_CORE_API GUIInputBox:public GUIWindow 
	{
	public:
		struct InputBoxType:public ObjectAlloc
		{
			LORD::GUIString name;
			Real x;
			Real y;
		};
		GUIInputBox(GUIWindowType nType, const GUIString& strName);
		virtual ~GUIInputBox();

		virtual bool	TouchDown(const Vector2& position);
		virtual bool	TouchMove(const Vector2& position);
		virtual bool	TouchUp(const Vector2& position);

		virtual bool	SetProperty(const GUIString& strName, const GUIString& strValue);
		virtual bool	GetProperty(const GUIString& strName, GUIString& strValue);	
		virtual void	GetPropertyList(GUIPropertyList& properlist);
		virtual bool	GetPropertyType(const GUIString& strName, GUIPropertyType& type);

		void ADDText(const GUIString& text);

		void DrawSelf();
		void SetNormalImage( const GUIString& strNormalImage );
		Real GetWidth(){return m_width;}////返回字体长度
		Real GetHeight(){return m_height;}///返回字体高度
		void Init();
		void ClearList();

		void	SetMoveAble(bool isMove) { m_isMoveAble = isMove; };
	protected:
		Real m_height;//字体高度
		Real m_width;//字体宽度
		//LORD::Vector2 m_initialPostion;
		LORD::ERect m_Rect;
		Real m_MaxWidth;//最大宽度
		Real m_MaxHeight;//最大高度
		ui32 m_MaxNUMWidth;////横向最多可以存的字符数
		ui32 m_MaxNUMHeight;////纵向最多可以存的字符数
		LORD::list<InputBoxType*>::type InputBoxList;
		Real m_Postion;
		GUIRenderImage BackGround;
		LORD::Color m_fcolor;
		LORD::Color m_bcolor;

		ui32 len;
		ui32 LastLen;
	//	LORD::GUIFont m_font;
		Real ResetLenght;
		Real MoveLenght;
		bool m_Start;
		bool Resetting;
		GUIRenderText  m_RenderText;
		bool m_isMoveAble;
	};
}

#endif
