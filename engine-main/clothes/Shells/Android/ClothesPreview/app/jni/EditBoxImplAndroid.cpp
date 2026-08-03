#include "EditBoxImplAndroid.h"
#include "EchoHelper.h"

//#include "CCEditBox.h"
//#include "jni/Java_org_cocos2dx_lib_Cocos2dxBitmap.h"
//#include "jni/Java_org_cocos2dx_lib_Cocos2dxHelper.h"


namespace LORD
{
GUIEditBoxImpl* __createSystemEditBox(GUIEditBox* pEditBox)
{
    return new EditBoxImplAndroid(pEditBox);
}

void __destroySystemEditBox(GUIEditBoxImpl* pEditBoxImpl)
{
	if(pEditBoxImpl)
	{
		delete pEditBoxImpl;
	}
}

EditBoxImplAndroid::EditBoxImplAndroid(GUIEditBox* pEditText)
: GUIEditBoxImpl(pEditText)
//, _label(nullptr)
//, _labelPlaceHolder(nullptr)
, _editBoxInputMode(IM_SINGLE_LINE)
, _editBoxInputFlag(IF_INTIAL_CAPS_ALL_CHARACTERS)
, _keyboardReturnType(KRT_DONE)
, _colText(Color::WHITE)
, _colPlaceHolder(Color::GRAY)
, _maxLength(-1)
{
    
}

EditBoxImplAndroid::~EditBoxImplAndroid()
{
	
}

void EditBoxImplAndroid::doAnimationWhenKeyboardMove(float duration, float distance)
{ // don't need to be implemented on android platform.

}

//static const int CC_EDIT_BOX_PADDING = 5;

bool EditBoxImplAndroid::initWithSize(const Vector2& size)
{
//    int fontSize = getFontSizeAccordingHeightJni(size.height-12);
//    _label = Label::create();
//    _label->setSystemFontSize(size.height-12);
//	// align the text vertically center
//    _label->setAnchorPoint(Point(0, 0.5f));
//    _label->setPosition(Point(CC_EDIT_BOX_PADDING, size.height / 2.0f));
//    _label->setColor(_colText);
//    _editBox->addChild(_label);
//
//    _labelPlaceHolder = Label::create();
//    _labelPlaceHolder->setSystemFontSize(size.height-12);
//	// align the text vertically center
//    _labelPlaceHolder->setAnchorPoint(Point(0, 0.5f));
//    _labelPlaceHolder->setPosition(Point(CC_EDIT_BOX_PADDING, size.height / 2.0f));
//    _labelPlaceHolder->setVisible(false);
//    _labelPlaceHolder->setColor(_colPlaceHolder);
//    _editBox->addChild(_labelPlaceHolder);

    _editSize = size;
    return true;
}

void EditBoxImplAndroid::setFont(const char* pFontName, int fontSize)
{
//	if(_label != NULL) {
//		_label->setSystemFontName(pFontName);
//		_label->setSystemFontSize(fontSize);
//	}
//
//	if(_labelPlaceHolder != NULL) {
//		_labelPlaceHolder->setSystemFontName(pFontName);
//		_labelPlaceHolder->setSystemFontSize(fontSize);
//	}
}

void EditBoxImplAndroid::setFontColor(const Color& color)
{
//    _colText = color;
//    _label->setColor(color);
}

void EditBoxImplAndroid::setPlaceholderFont(const char* pFontName, int fontSize)
{
//	if(_labelPlaceHolder != NULL) {
//		_labelPlaceHolder->setSystemFontName(pFontName);
//		_labelPlaceHolder->setSystemFontSize(fontSize);
//	}
}

void EditBoxImplAndroid::setPlaceholderFontColor(const Color& color)
{
//    _colPlaceHolder = color;
//    _labelPlaceHolder->setColor(color);
}

void EditBoxImplAndroid::setInputMode(InputMode inputMode)
{
    _editBoxInputMode = inputMode;
}

void EditBoxImplAndroid::setMaxLength(int maxLength)
{
    _maxLength = maxLength;
}

int EditBoxImplAndroid::getMaxLength()
{
    return _maxLength;
}

void EditBoxImplAndroid::setInputFlag(InputFlag inputFlag)
{
    _editBoxInputFlag = inputFlag;
}

void EditBoxImplAndroid::setReturnType(KeyboardReturnType returnType)
{
    _keyboardReturnType = returnType;
}

bool EditBoxImplAndroid::isEditing()
{
    return false;
}

void EditBoxImplAndroid::setText(const char* pText)
{
    if (pText != NULL)
    {
        _text = pText;

        _editBox->OnInputText(_text);

//        if (_text.length() > 0)
//        {
//            //_labelPlaceHolder->setVisible(false);
//
//            std::string strToShow;
//
//            if (InputFlag::PASSWORD == _editBoxInputFlag)
//            {
//                long length = cc_utf8_strlen(_text.c_str(), -1);
//                for (long i = 0; i < length; i++)
//                {
//                    strToShow.append("*");
//                }
//            }
//            else
//            {
//                strToShow = _text;
//            }
//
//			_label->setString(strToShow.c_str());
//
//			// Clip the text width to fit to the text box
//
//            float fMaxWidth = _editSize.width - CC_EDIT_BOX_PADDING * 2;
//            auto labelSize = _label->getContentSize();
//            if(labelSize.width > fMaxWidth) {
//                _label->setDimensions(fMaxWidth,labelSize.height);
//            }
//        }
//        else
//        {
//            //_labelPlaceHolder->setVisible(true);
//            //_label->setString("");
//        }

    }
}

const char*  EditBoxImplAndroid::getText(void)
{
    return _text.c_str();
}

void EditBoxImplAndroid::setPlaceHolder(const char* pText)
{
    if (pText != NULL)
    {
        _placeHolder = pText;
        if (_placeHolder.length() > 0 && _text.length() == 0)
        {
            //_labelPlaceHolder->setVisible(true);
        }

        //_labelPlaceHolder->setString(_placeHolder.c_str());
    }
}

void EditBoxImplAndroid::setPosition(const Vector2& pos)
{ // don't need to be implemented on android platform.

}

void EditBoxImplAndroid::setVisible(bool visible)
{ // don't need to be implemented on android platform.

}

void EditBoxImplAndroid::setContentSize(const Vector2& size)
{ // don't need to be implemented on android platform.

}

void EditBoxImplAndroid::setAnchorPoint(const Vector2& anchorPoint)
{ // don't need to be implemented on android platform.

}

void EditBoxImplAndroid::visit(void)
{ // don't need to be implemented on android platform.

}

void EditBoxImplAndroid::onEnter(void)
{ // don't need to be implemented on android platform.

}

static void editBoxCallbackFunc(const char* pText, void* ctx)
{
    EditBoxImplAndroid* thiz = (EditBoxImplAndroid*)ctx;
    thiz->setText(pText);
	
//    if (thiz->getDelegate() != NULL)
//    {
//        thiz->getDelegate()->editBoxTextChanged(thiz->getEditBox(), thiz->getText());
//        thiz->getDelegate()->editBoxEditingDidEnd(thiz->getEditBox());
//        thiz->getDelegate()->editBoxReturn(thiz->getEditBox());
//    }
    
//#if CC_ENABLE_SCRIPT_BINDING
//    EditBox* pEditBox = thiz->getEditBox();
//    if (NULL != pEditBox && 0 != pEditBox->getScriptEditBoxHandler())
//    {
//        CommonScriptData data(pEditBox->getScriptEditBoxHandler(), "changed",pEditBox);
//        ScriptEvent event(kCommonEvent,(void*)&data);
//        ScriptEngineManager::getInstance()->getScriptEngine()->sendEvent(&event);
//        memset(data.eventName, 0, sizeof(data.eventName));
//        strncpy(data.eventName, "ended", sizeof(data.eventName));
//        event.data = (void*)&data;
//        ScriptEngineManager::getInstance()->getScriptEngine()->sendEvent(&event);
//        memset(data.eventName, 0, sizeof(data.eventName));
//        strncpy(data.eventName, "return", sizeof(data.eventName));
//        event.data = (void*)&data;
//        ScriptEngineManager::getInstance()->getScriptEngine()->sendEvent(&event);
//    }
//#endif
}

void EditBoxImplAndroid::openKeyboard()
{
//    if (_delegate != NULL)
//    {
//        _delegate->editBoxEditingDidBegin(_editBox);
//    }
//
//#if CC_ENABLE_SCRIPT_BINDING
//    EditBox* pEditBox = this->getEditBox();
//    if (NULL != pEditBox && 0 != pEditBox->getScriptEditBoxHandler())
//    {
//        CommonScriptData data(pEditBox->getScriptEditBoxHandler(), "began",pEditBox);
//        ScriptEvent event(cocos2d::kCommonEvent,(void*)&data);
//        ScriptEngineManager::getInstance()->getScriptEngine()->sendEvent(&event);
//    }
//#endif
	
    showEditTextDialogJNI(  _placeHolder.c_str(),
						  _text.c_str(),
						  (int)_editBoxInputMode,
						  (int)_editBoxInputFlag,
						  (int)_keyboardReturnType,
						  _maxLength,
						  editBoxCallbackFunc,
						  (void*)this);
	
}

void EditBoxImplAndroid::closeKeyboard()
{
	
}

}




