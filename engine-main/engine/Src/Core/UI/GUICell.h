/********************************************************************
filename: 	GUICell.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		maxicheng
company:	supernano
*********************************************************************/



#ifndef __GUI_CELL_H__
#define __GUI_CELL_H__

#include "Core.h"
#include "GUIWindow.h"
#include "GUIImage.h"

namespace LORD
{


class LORD_CORE_API GUICell : public GUIWindow
{
public:
	GUICell(GUIWindowType nType, const GUIString& strName);
	virtual ~GUICell();

	virtual void	DrawSelf();

	void			SetVertexData(const ImageBufferListType& vertex);

protected:
	ImageBufferListType m_vertex;
	ImageBufferListType m_actionVertex;

};

}



#endif