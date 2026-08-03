#include "Core.h"
#include "Rect.h"

namespace LORD
{

	ERect& ERect::merge(const ERect& rhs)
	{
		if(isNull())
		{
			*this = rhs;
		}
		else if(!rhs.isNull())
		{
			left = Math::Min(left, rhs.left);
			right = Math::Max(right, rhs.right);
			top = Math::Min(top, rhs.top);
			bottom = Math::Max(bottom, rhs.bottom);
		}

		return *this;
	}

	ERect ERect::intersect(const ERect &rhs) const
	{
		ERect rect;

		if(isNull() || rhs.isNull())
		{
			// empty
			return rect;
		}
		else
		{
			rect.left = Math::Max(left, rhs.left);
			rect.right = Math::Min(right, rhs.right);
			rect.top = Math::Max(top, rhs.top);
			rect.bottom = Math::Min(bottom, rhs.bottom);
		}

		if(rect.left > rect.right || rect.top > rect.bottom)
		{
			// no intersection, return empty
			rect.left = rect.top = rect.right = rect.bottom = 0;
		}

		return rect;
	}

	bool ERect::isPointInRect(const Vector2& position) const
	{
		if(left > position.x ||
			right <= position.x ||
			top > position.y ||
			bottom <= position.y
			)
		{
			return false;
		}
		return true;
	}

	ERect& ERect::makeSquare()
	{
		Real width = getWidth();
		Real height = getHeight();
		if (width > height)
		{
			Real f = (width - height) / 2;
			left += f;
			right -= f;
		}
		else if (height > width)
		{
			Real f = (height - width) / 2;
			top += f;
			bottom -= f;
		}
		return *this;
	}
}
