#include "Core.h"
#include "Viewport.h"

namespace LORD
{
	Viewport::Viewport(int _left, int _top, uint _width, uint _height)
		: m_left(_left)
		, m_top(_top)
		, m_width(_width)
		, m_height(_height)
	{
	}

	Viewport::~Viewport()
	{
	}

	int Viewport::getLeft() const
	{
		return m_left;
	}

	int Viewport::getTop() const
	{
		return m_top;
	}

	uint Viewport::getWidth() const
	{
		return m_width;
	}

	uint Viewport::getHeight() const
	{
		return m_height;
	}

	void Viewport::resize(int left, int top, uint width, uint height)
	{
		m_left = left;
		m_top = top;
		m_width = width;
		m_height = height;
	}

	void Viewport::setViewProjMatrix(const Matrix4& mat)
	{
		m_matVP = mat;
	}

	const Matrix4& Viewport::getViewProjMatrix() const
	{
		return m_matVP;
	}

}