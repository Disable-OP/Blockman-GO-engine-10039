#include "Core.h"
#if (LORD_PLATFORM == LORD_PLATFORM_WINDOWS)
#include "EditBoxImplWin.h"
#include "UI/GUIEditBoxImpl.h"
#include "Win32InputBox.h"

namespace LORD
{
	GUIEditBoxImpl* __createSystemEditBox(GUIEditBox* pEditBox)
	{
		return new EditBoxImplWin(pEditBox);
	}

	void __destroySystemEditBox(GUIEditBoxImpl* pEditBoxImpl)
	{
		if (pEditBoxImpl)
		{
			delete pEditBoxImpl;
		}
	}

	EditBoxImplWin::EditBoxImplWin(GUIEditBox* pEditText)
		: GUIEditBoxImpl(pEditText)
		//, _label(nullptr)
		//, _labelPlaceHolder(nullptr)
		, _editBoxInputMode(InputMode::IM_SINGLE_LINE)
		, _editBoxInputFlag(InputFlag::IF_INTIAL_CAPS_ALL_CHARACTERS)
		, _keyboardReturnType(KeyboardReturnType::KRT_DEFAULT)
		, _colText(Color::WHITE)
		, _colPlaceHolder(Color::GRAY)
		, _maxLength(-1)
	{

	}

	EditBoxImplWin::~EditBoxImplWin()
	{
	}

	void EditBoxImplWin::doAnimationWhenKeyboardMove(float duration, float distance)
	{
	}

	bool EditBoxImplWin::initWithSize(const Vector2& size)
	{
		//! int fontSize = getFontSizeAccordingHeightJni(size.height-12);
		// 		_label = Label::create();
		// 		_label->setSystemFontSize(size.height - 12);
		// 		// align the text vertically center
		// 		_label->setAnchorPoint(Point(0, 0.5f));
		// 		_label->setPosition(Point(5, size.height / 2.0f));
		// 		_label->setColor(_colText);
		// 		_editBox->addChild(_label);
		// 
		// 		_labelPlaceHolder = Label::create();
		// 		_labelPlaceHolder->setSystemFontSize(size.height - 12);
		// 		// align the text vertically center
		// 		_labelPlaceHolder->setAnchorPoint(Point(0, 0.5f));
		// 		_labelPlaceHolder->setPosition(Point(5, size.height / 2.0f));
		// 		_labelPlaceHolder->setVisible(false);
		// 		_labelPlaceHolder->setColor(_colPlaceHolder);
		// 		_editBox->addChild(_labelPlaceHolder);

		_editSize = size;
		return true;
	}

	void EditBoxImplWin::setFont(const char* pFontName, int fontSize)
	{
		// 		if (_label != NULL) {
		// 			_label->setSystemFontName(pFontName);
		// 			_label->setSystemFontSize(fontSize);
		// 		}
		// 
		// 		if (_labelPlaceHolder != NULL) {
		// 			_labelPlaceHolder->setSystemFontName(pFontName);
		// 			_labelPlaceHolder->setSystemFontSize(fontSize);
		// 		}
	}

	void EditBoxImplWin::setFontColor(const Color& color)
	{
		// 		_colText = color;
		// 		_label->setColor(color);
	}

	void EditBoxImplWin::setPlaceholderFont(const char* pFontName, int fontSize)
	{
		// 		if (_labelPlaceHolder != NULL) {
		// 			_labelPlaceHolder->setSystemFontName(pFontName);
		// 			_labelPlaceHolder->setSystemFontSize(fontSize);
		// 		}
	}

	void EditBoxImplWin::setPlaceholderFontColor(const Color& color)
	{
		// 		_colPlaceHolder = color;
		// 		_labelPlaceHolder->setColor(color);
	}

	void EditBoxImplWin::setInputMode(InputMode inputMode)
	{
		_editBoxInputMode = inputMode;
	}

	void EditBoxImplWin::setMaxLength(int maxLength)
	{
		_maxLength = maxLength;
	}

	int EditBoxImplWin::getMaxLength()
	{
		return _maxLength;
	}

	void EditBoxImplWin::setInputFlag(InputFlag inputFlag)
	{
		_editBoxInputFlag = inputFlag;
	}

	void EditBoxImplWin::setReturnType(KeyboardReturnType returnType)
	{
		_keyboardReturnType = returnType;
	}

	bool EditBoxImplWin::isEditing()
	{
		return false;
	}

	void EditBoxImplWin::setText(const char* pText)
	{
		if (pText != NULL)
		{
			_text = pText;

			if (_editBox)
			{
				_editBox->OnInputText(_text);
			}

			// 			if (_text.length() > 0)
			// 			{
			// 				_labelPlaceHolder->setVisible(false);
			// 
			// 				std::string strToShow;
			// 
			// 				if (EditBox::InputFlag::PASSWORD == _editBoxInputFlag)
			// 				{
			// 					long length = cc_utf8_strlen(_text.c_str(), -1);
			// 					for (long i = 0; i < length; i++)
			// 					{
			// 						strToShow.append("*");
			// 					}
			// 				}
			// 				else
			// 				{
			// 					strToShow = _text;
			// 				}
			// 
			// 				//! std::string strWithEllipsis = getStringWithEllipsisJni(strToShow.c_str(), _editSize.width, _editSize.height-12);
			// 				//! _label->setString(strWithEllipsis.c_str());
			// 				_label->setString(strToShow.c_str());
			// 			}
			// 			else
			// 			{
			// 				_labelPlaceHolder->setVisible(true);
			// 				_label->setString("");
			// 			}

		}
	}

	const char*  EditBoxImplWin::getText(void)
	{
		return _text.c_str();
	}

	void EditBoxImplWin::setPlaceHolder(const char* pText)
	{
		// 		if (pText != NULL)
		// 		{
		// 			_placeHolder = pText;
		// 			if (_placeHolder.length() > 0 && _text.length() == 0)
		// 			{
		// 				_labelPlaceHolder->setVisible(true);
		// 			}
		// 
		// 			_labelPlaceHolder->setString(_placeHolder.c_str());
		// 		}
	}

	void EditBoxImplWin::setPosition(const Vector2& pos)
	{
		//_label->setPosition(pos);
		//_labelPlaceHolder->setPosition(pos);
	}

	void EditBoxImplWin::setVisible(bool visible)
	{ // don't need to be implemented on win32 platform.
	}

	void EditBoxImplWin::setContentSize(const Vector2& size)
	{
	}

	void EditBoxImplWin::setAnchorPoint(const Vector2& anchorPoint)
	{ // don't need to be implemented on win32 platform.

	}

	void EditBoxImplWin::visit(void)
	{
	}

	void EditBoxImplWin::openKeyboard()
	{
		// 		if (_delegate != NULL)
		// 		{
		// 			_delegate->editBoxEditingDidBegin(_editBox);
		// 		}

		// 		EditBox* pEditBox = this->getEditBox();
		// 		if (NULL != pEditBox && 0 != pEditBox->getScriptEditBoxHandler())
		// 		{
		// 			CommonScriptData data(pEditBox->getScriptEditBoxHandler(), "began", pEditBox);
		// 			ScriptEvent event(kCommonEvent, (void*)&data);
		// 			ScriptEngineManager::getInstance()->getScriptEngine()->sendEvent(&event);
		// 		}

		// 		std::string placeHolder = _labelPlaceHolder->getString();
		// 		if (placeHolder.length() == 0)
		// 			placeHolder = "Enter value";

		char pText[100] = { 0 };
		GUIString text = getText();
		if (text.length())
			strncpy(pText, text.c_str(), 100);

		// 		GLView *glView = Director::getInstance()->getOpenGLView();
		// 		GLFWwindow *glfwWindow = glView->getWindow();
		// 		HWND hwnd = glfwGetWin32Window(glfwWindow);

// 		HINSTANCE hInst = GetModuleHandle(0);
// 		HWND hWnd = CreateWindow("GenericAppClass",     ///<name of the window class
// 			"",         ///<window name
// 			WS_OVERLAPPEDWINDOW | WS_HSCROLL |
// 			WS_VSCROLL | WS_CLIPSIBLINGS,   ///<specifies the window style
// 			0,                  ///<set the horizontal position of the window
// 			0,                                  ///<set the vertical position of the window
// 			CW_USEDEFAULT,                      ///<set the width of the window
// 			CW_USEDEFAULT,            ///<set the height of the window
// 			NULL,                               ///<contains valid pointer for child window 
// 			NULL,                        ///<class menu is used
// 			hInst,              ///<Handle to the instance associated with the window
// 			NULL);

		HDC dc = GetDC(NULL);
		HWND hWnd = WindowFromDC(dc);

		bool didChange = CWin32InputBox::InputBox("Input", "", pText, 100, false, hWnd) == IDOK;

		if (didChange)
			setText(pText);

		// 		if (_delegate != NULL) {
		// 			if (didChange)
		// 				_delegate->editBoxTextChanged(_editBox, getText());
		// 			_delegate->editBoxEditingDidEnd(_editBox);
		// 			_delegate->editBoxReturn(_editBox);
		// 		}

		// #if CC_ENABLE_SCRIPT_BINDING
		// 		if (nullptr != _editBox && 0 != _editBox->getScriptEditBoxHandler())
		// 		{
		// 			CommonScriptData data(_editBox->getScriptEditBoxHandler(), "changed", _editBox);
		// 			ScriptEvent event(kCommonEvent, (void*)&data);
		// 			if (didChange)
		// 			{
		// 				ScriptEngineManager::getInstance()->getScriptEngine()->sendEvent(&event);
		// 			}
		// 			memset(data.eventName, 0, sizeof(data.eventName));
		// 			strncpy(data.eventName, "ended", sizeof(data.eventName));
		// 			event.data = (void*)&data;
		// 			ScriptEngineManager::getInstance()->getScriptEngine()->sendEvent(&event);
		// 			memset(data.eventName, 0, sizeof(data.eventName));
		// 			strncpy(data.eventName, "return", sizeof(data.eventName));
		// 			event.data = (void*)&data;
		// 			ScriptEngineManager::getInstance()->getScriptEngine()->sendEvent(&event);
		// 		}
		// #endif // #if CC_ENABLE_SCRIPT_BINDING
	}

	void EditBoxImplWin::closeKeyboard()
	{

	}

	void EditBoxImplWin::onEnter(void)
	{

	}

}
#endif


