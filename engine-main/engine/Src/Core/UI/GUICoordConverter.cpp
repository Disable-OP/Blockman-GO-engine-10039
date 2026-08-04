#include "GUICoordConverter.h"
#include "GUIRenderManager.h"

namespace LORD
{
float CoordConverter::windowToScreenX(const GUIWindow& window, const UDim& x)
{
    return getBaseXValue(window) + x.asAbsolute(window.GetPixelSize().x);
}

//----------------------------------------------------------------------------//

float CoordConverter::windowToScreenY(const GUIWindow& window, const UDim& y)
{
    return getBaseYValue(window) + y.asAbsolute(window.GetPixelSize().y);
}

//----------------------------------------------------------------------------//

Vector2 CoordConverter::windowToScreen(const GUIWindow& window, const UVector2& vec)
{
    return getBaseValue(window) + vec.asAbsolute(window.GetPixelSize());
}

//----------------------------------------------------------------------------//

ERect CoordConverter::windowToScreen(const GUIWindow& window, const URect& rect)
{
    ERect tmp(rect.asAbsolute(window.GetPixelSize()));
    return tmp.offset(getBaseValue(window));
}

//----------------------------------------------------------------------------//

float CoordConverter::screenToWindowX(const GUIWindow& window, const UDim& x)
{
    return x.asAbsolute((float)GUIRenderManager::Instance()->GetWidth()) -
           getBaseXValue(window);
}

//----------------------------------------------------------------------------//

float CoordConverter::screenToWindowY(const GUIWindow& window, const UDim& y)
{
    return y.asAbsolute((float)GUIRenderManager::Instance()->GetHeight()) -
           getBaseYValue(window);
}

//----------------------------------------------------------------------------//

Vector2 CoordConverter::screenToWindow(const GUIWindow& window, const UVector2& vec)
{
    return vec.asAbsolute(GUIRenderManager::Instance()->GetDisplaySize()) -
           getBaseValue(window);
}

//----------------------------------------------------------------------------//

ERect CoordConverter::screenToWindow(const GUIWindow& window, const URect& rect)
{
    Vector2 base(getBaseValue(window));
    ERect pixel(
        rect.asAbsolute(GUIRenderManager::Instance()->GetDisplaySize()));

    // negate base position
    base.x = -base.x;
    base.y = -base.y;

    return pixel.offset(base);
}

//----------------------------------------------------------------------------//

float CoordConverter::windowToScreenX(const GUIWindow& window, const float x)
{
    return getBaseXValue(window) + x;
}

//----------------------------------------------------------------------------//

float CoordConverter::windowToScreenY(const GUIWindow& window, const float y)
{
    return getBaseYValue(window) + y;
}

//----------------------------------------------------------------------------//

Vector2 CoordConverter::windowToScreen(const GUIWindow& window, const Vector2& vec)
{
    return getBaseValue(window) + vec;
}

//----------------------------------------------------------------------------//

ERect CoordConverter::windowToScreen(const GUIWindow& window, const ERect& rect)
{
    ERect tmp(rect);
    return tmp.offset(getBaseValue(window));
}

//----------------------------------------------------------------------------//

float CoordConverter::screenToWindowX(const GUIWindow& window, const float x)
{
    return x - getBaseXValue(window);
}

//----------------------------------------------------------------------------//

float CoordConverter::screenToWindowY(const GUIWindow& window, const float y)
{
    return y - getBaseYValue(window);
}

//----------------------------------------------------------------------------//

Vector2 CoordConverter::screenToWindow(const GUIWindow& window, const Vector2& vec)
{
    return vec - getBaseValue(window);
}

//----------------------------------------------------------------------------//

ERect CoordConverter::screenToWindow(const GUIWindow& window, const ERect& rect)
{
    Vector2 base(getBaseValue(window));

    // negate base position
    base.x = -base.x;
    base.y = -base.y;

    ERect tmp(rect);
    return tmp.offset(base);
}

//----------------------------------------------------------------------------//

float CoordConverter::getBaseXValue(const GUIWindow& window)
{
    const GUIWindow* parent = window.GetParent();

    const ERect parent_rect(parent ?
        parent->GetUnclippedOuterRect() :
        ERect(0, 0,(float)GUIRenderManager::Instance()->GetWidth(), (float)GUIRenderManager::Instance()->GetHeight()));

    const float parent_width = parent_rect.getWidth();
    float baseX = parent_rect.left;

    baseX += window.GetArea().d_min.d_x.asAbsolute(parent_width);

    switch(window.GetHorizontalAlignment())
    {
        case HA_CENTRE:
            baseX += (parent_width - window.GetPixelSize().x) * 0.5f;
            break;
        case HA_RIGHT:
            baseX += parent_width - window.GetPixelSize().x;
            break;
        default:
            break;
    }

    return PixelAligned(baseX);
}

//----------------------------------------------------------------------------//

float CoordConverter::getBaseYValue(const GUIWindow& window)
{
    const GUIWindow* parent = window.GetParent();

    const ERect parent_rect(parent ?
        parent->GetUnclippedOuterRect() :
        ERect(0, 0, (float)GUIRenderManager::Instance()->GetWidth(), (float)GUIRenderManager::Instance()->GetHeight())
    );

    const float parent_height = parent_rect.getHeight();
    float baseY = parent_rect.top;

    baseY += window.GetArea().d_min.d_y.asAbsolute(parent_height);

    switch(window.GetVerticalAlignment())
    {
        case VA_CENTRE:
            baseY += (parent_height - window.GetPixelSize().y) * 0.5f;
            break;
        case VA_BOTTOM:
            baseY += parent_height - window.GetPixelSize().y;
            break;
        default:
            break;
    }

    return PixelAligned(baseY);
}

//----------------------------------------------------------------------------//

Vector2 CoordConverter::getBaseValue(const GUIWindow& window)
{
    return Vector2(getBaseXValue(window), getBaseYValue(window));
}

}
