#ifndef __EDITBOXIMPLANDROID_H__
#define __EDITBOXIMPLANDROID_H__

#include "UI/GUIEditBoxImpl.h"
#include "Core.h"
#include "UI/GUICommonDef.h"

namespace LORD
{
	class EditBoxImplAndroid : public GUIEditBoxImpl
	{
	public:
		/**
		 * @js NA
		 */
		EditBoxImplAndroid(GUIEditBox* pEditText);
		/**
		 * @js NA
		 * @lua NA
		 */
		virtual ~EditBoxImplAndroid();

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
		/**
		 * @js NA
		 * @lua NA
		 */
		virtual void onEnter(void);
		virtual void doAnimationWhenKeyboardMove(float duration, float distance);
		virtual void openKeyboard();
		virtual void closeKeyboard();

	private:
//		Label* _label;
//		Label* _labelPlaceHolder;
		InputMode    _editBoxInputMode;
		InputFlag    _editBoxInputFlag;
		KeyboardReturnType  _keyboardReturnType;

		GUIString 	_text;
		std::string _placeHolder;

		Color _colText;
		Color _colPlaceHolder;

		int   _maxLength;
		Vector2 _editSize;
	};

}

#endif


