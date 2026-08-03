#include "GUICell.h"
#include "GUIImageset.h"
#include "GUICoordConverter.h"


namespace LORD
{

GUICell::GUICell(GUIWindowType nType, const GUIString& strName)
	: GUIWindow(nType, strName)
{

}

GUICell::~GUICell()
{

}


void GUICell::DrawSelf()
{

	m_actionVertex = m_vertex;

	//×ø±ê±ä»»
	for (ImageBufferListType::iterator it = m_actionVertex.begin(); it != m_actionVertex.end(); ++it)
	{
		it->lt.x = CoordConverter::windowToScreenX(*this, it->lt.x);
		it->lt.y = CoordConverter::windowToScreenY(*this, it->lt.y);

		it->lb.x = CoordConverter::windowToScreenX(*this, it->lb.x);
		it->lb.y = CoordConverter::windowToScreenY(*this, it->lb.y);

		it->rb.x = CoordConverter::windowToScreenX(*this, it->rb.x);
		it->rb.y = CoordConverter::windowToScreenY(*this, it->rb.y);

		it->rt.x = CoordConverter::windowToScreenX(*this, it->rt.x);
		it->rt.y = CoordConverter::windowToScreenY(*this, it->rt.y);
	}

	//²Ã¼ô todo.


	for (ImageBufferListType::iterator iter = m_actionVertex.begin(); iter != m_actionVertex.end(); ++iter)
	{
		if (iter->imageset)
		{
			// Òª³ËÒÔ±ä»Ã¾ØÕó
			iter->imageset->DrawImage(iter->lt * this->getActionMatrix(), iter->rb * this->getActionMatrix(),
				iter->lb * this->getActionMatrix(), iter->rt * this->getActionMatrix(), iter->uv, iter->color, this->getMaterial(), this->getMaskTexture());
		}
	}



}

void GUICell::SetVertexData(const ImageBufferListType& vertex)
{
	m_vertex = vertex;
}

}