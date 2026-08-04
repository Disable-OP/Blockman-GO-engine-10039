/********************************************************************
filename: 	EditBoxImplWin.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#include "Core.h"
#if (LORD_PLATFORM == LORD_PLATFORM_WINDOWS)
#ifndef __EditBoxIMPLWIN_H__
#define __EditBoxIMPLWIN_H__

#include "UI/GUIEditBoxImpl.h"

namespace LORD
{
	class GUIEditBox;
	class EditBoxImplWin : public GUIEditBoxImpl
	{
	public:
		/**
		* @js NA
		*/
		EditBoxImplWin(GUIEditBox* pEditText);
		/**
		* @js NA
		* @lua NA
		*/
		virtual ~EditBoxImplWin();

		virtual bool initWithSize(const Vector2& size);
		virtual void setFont(const char* pFontName, int fontSize);
		virtual void setFontColor(const Color& color);
		virtual void setPlaceholderFont(const char* pFontName, int fontSize);
		virtual void setPlaceholderFontColor(const Color& color);
		virtual void setInputMode(InputMode inputMode);
		virtual void setInputFlag(InputFlag inputFlag);
		virtual void setMaxLength(int maxLength);
		virtual int  getMaxLength();
		virtual void setReturnType(KeyboardReturnType returnType);
		virtual bool isEditing();

		virtual void setText(const char* pText);
		virtual const char* getText(void);
		virtual void setPlaceHolder(const char* pText);
		virtual void setPosition(const Vector2& pos);
		virtual void setVisible(bool visible);
		virtual void setContentSize(const Vector2& size);
		virtual void setAnchorPoint(const Vector2& anchorPoint);
		/**
		* @js NA
		* @lua NA
		*/
		virtual void visit(void);
		virtual void doAnimationWhenKeyboardMove(float duration, float distance);
		virtual void openKeyboard();
		virtual void closeKeyboard();
		/**
		* @js NA
		* @lua NA
		*/
		virtual void onEnter(void);
	private:

		//Label* _label;
		//Label* _labelPlaceHolder;
		InputMode    _editBoxInputMode;
		InputFlag    _editBoxInputFlag;
		KeyboardReturnType  _keyboardReturnType;

		GUIString _text;
		std::string _placeHolder;

		Color _colText;
		Color _colPlaceHolder;

		int   _maxLength;
		Vector2 _editSize;

		/*
		Size     _contentSize;
		HWND       _sysEdit;
		int        _maxTextLength;
		*/
	};
}

#endif /* __CCEditBoxIMPLWIN_H__ */


#endif
