/********************************************************************
filename: 	GUIEditBoxImpl.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef __GUI_EDITBOX_IMPL_H__
#define __GUI_EDITBOX_IMPL_H__

#include "GUIEditBox.h"

namespace LORD
{
class GUIEditBoxImpl
{
public:
    /**
     * @js NA
     */
	GUIEditBoxImpl(GUIEditBox* pEditBox) : _editBox(pEditBox) {}
    /**
     * @js NA
     * @lua NA
     */
    virtual ~GUIEditBoxImpl() {}
    
    virtual bool initWithSize(const Vector2& size) = 0;
    virtual void setFont(const char* pFontName, int fontSize) = 0;
    virtual void setFontColor(const Color& color) = 0;
    virtual void setPlaceholderFont(const char* pFontName, int fontSize) = 0;
    virtual void setPlaceholderFontColor(const Color& color) = 0;
    virtual void setInputMode(InputMode inputMode) = 0;
    virtual void setInputFlag(InputFlag inputFlag) = 0;
    virtual void setMaxLength(int maxLength) = 0;
    virtual int  getMaxLength() = 0;
    virtual void setReturnType(KeyboardReturnType returnType) = 0;
    virtual bool isEditing() = 0;
    
    virtual void setText(const char* pText) = 0;
    virtual const char* getText(void) = 0;
    virtual void setPlaceHolder(const char* pText) = 0;
    virtual void doAnimationWhenKeyboardMove(float duration, float distance) = 0;
    
    virtual void openKeyboard() = 0;
    virtual void closeKeyboard() = 0;
    
    virtual void setPosition(const Vector2& pos) = 0;
    virtual void setVisible(bool visible) = 0;
    virtual void setContentSize(const Vector2& size) = 0;
	virtual void setAnchorPoint(const Vector2& anchorPoint) = 0;
    
    /**
     * check the editbox's position, update it when needed
     */
    virtual void updatePosition(float dt){};
    /**
     * @js NA
     * @lua NA
     */
    virtual void visit(void) = 0;
    /**
     * @js NA
     * @lua NA
     */
    virtual void onEnter(void) = 0;
    
    GUIEditBox* getEditBox() { return _editBox; };
protected:

    GUIEditBox* _editBox;
};

extern GUIEditBoxImpl* __createSystemEditBox(GUIEditBox* pEditBox);
extern void __destroySystemEditBox(GUIEditBoxImpl* pEditBoxImpl);

}

#endif
