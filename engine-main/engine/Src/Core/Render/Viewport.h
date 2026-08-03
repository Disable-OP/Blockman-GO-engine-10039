/********************************************************************
filename: 	Viewport.h
file path:	dev\engine\Src\Core\Render\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_VIEWPORT_H__
#define __LORD_VIEWPORT_H__

#include "Core.h"

namespace LORD
{
	class LORD_CORE_API Viewport: public ObjectAlloc
	{	
	public:
		Viewport(int left, int top, uint width, uint height);
		virtual ~Viewport();

	public:
		int			getLeft() const;
		int			getTop() const;
		uint		getWidth() const;
		uint		getHeight() const;

		void		resize(int left, int top, uint width, uint height);
		void		setViewProjMatrix(const Matrix4& mat);

		const Matrix4&	getViewProjMatrix() const;

	protected:
		int			m_left;
		int			m_top;
		uint		m_width;
		uint		m_height;
		Matrix4		m_matVP;
	};
}
#endif