/********************************************************************
filename: 	Rect.h
file path:	dev\engine\Src\Core\Geom\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_RECT_H__
#define __LORD_RECT_H__

#include "GeomDef.h"

namespace LORD
{
    class LORD_CORE_API ERect
	{
	public:
		Real	left;
		Real	top;
		Real	right;
		Real	bottom;

	public:
		inline ERect()
			:left(0)
			,top(0)
			,right(0)
			,bottom(0)
		{
		}

		inline ERect(Real l, Real t, Real r, Real b)
			:left(l)
			,top(t)
			,right(r)
			,bottom(b)
		{
		}

		inline ERect(const ERect& rect)
			:left(rect.left)
			,top(rect.top)
			,right(rect.right)
			,bottom(rect.bottom)
		{
		}

		inline ERect& operator = (const ERect& rect)
		{
			memcpy(&left, &rect.left, sizeof(ERect));
			return *this;
		}

		inline bool operator != (const ERect& rect)
		{
			return(left!=rect.left || top!=rect.top || right!=rect.right || bottom!=rect.bottom);
		}

		inline Real getWidth() const
		{
			return (right - left);
		}

		inline Real getHeight() const
		{
			return (bottom - top);
		}

		inline Vector2 getSize() const
		{
			return Vector2(getWidth(), getHeight());
		}

		inline bool isNull() const
		{
			return (getWidth() == 0 || getHeight() == 0);
		}

		inline void setNull()
		{
			left = right = top = bottom = 0;
		}

		inline ERect& offset(const Vector2& pt )
		{
			left		+= pt.x;
			right		+= pt.x;
			top			+= pt.y;
			bottom		+= pt.y;
			return *this;
		}

		ERect&	merge(const ERect& rhs);
		ERect	intersect(const ERect& rhs) const;
		bool	isPointInRect(const Vector2& position) const;
		ERect&	makeSquare();
		
		// 获取面积
		Real    getArea() const { return getWidth() * getHeight(); }
	};

	// ERect
	template< typename T> 
	struct TRect
	{
		T left,	top, width, height;

		// 构造函数
		TRect() 
			: left(0), top(0), width(0), height(0)
		{}

		// 构造函数
		TRect( T const& _left, T const& _top, T const& _width, T const& _height)
			: left(_left), top(_top), width(_width), height(_height) 
		{}

		// 获取面积
		T getArea() const { return width * height; }
	};
}

#endif
