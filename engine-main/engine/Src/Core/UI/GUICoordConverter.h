/********************************************************************
filename: 	GUICoordConverter.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef __GUI_COORD_CONVERTER__
#define __GUI_COORD_CONVERTER__

#include "GUIUDim.h"
#include "GUIWindow.h"

namespace LORD
{

class LORD_CORE_API CoordConverter
{
public:

    static float windowToScreenX(const GUIWindow& window, const UDim& x);

    static float windowToScreenX(const GUIWindow& window, const float x);

    static float windowToScreenY(const GUIWindow& window, const UDim& y);

    static float windowToScreenY(const GUIWindow& window, const float y);

    static Vector2 windowToScreen(const GUIWindow& window, const UVector2& vec);

    static Vector2 windowToScreen(const GUIWindow& window, const Vector2& vec);

    static ERect windowToScreen(const GUIWindow& window, const URect& rect);

    static ERect windowToScreen(const GUIWindow& window, const ERect& rect);

    static float screenToWindowX(const GUIWindow& window, const UDim& x);

    static float screenToWindowX(const GUIWindow& window, const float x);

    static float screenToWindowY(const GUIWindow& window, const UDim& y);

    static float screenToWindowY(const GUIWindow& window, const float y);

    static Vector2 screenToWindow(const GUIWindow& window, const UVector2& vec);

    static Vector2 screenToWindow(const GUIWindow& window, const Vector2& vec);

    static ERect screenToWindow(const GUIWindow& window, const URect& rect);

    static ERect screenToWindow(const GUIWindow& window, const ERect& rect);

private:
    static float getBaseXValue(const GUIWindow& window);

    static float getBaseYValue(const GUIWindow& window);

    static Vector2 getBaseValue(const GUIWindow& window);
};

}
#endif 
