/********************************************************************
filename: 	GUIRenderHexagon.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef __GUI_RENDER_HEXAGON_H__
#define __GUI_RENDER_HEXAGON_H__

#include "Core.h"
#include "GUIString.h"
#include "GUICommonDef.h"
#include "Resource/LordThread.h"
#include "GUIImage.h"

namespace LORD
{
#define CACHEGPUBUFFER
class GUIImageset;
class GUIWindow;
class GUIImage;

class LORD_CORE_API GUIRenderHexagon : public ObjectAlloc
{
	friend class RenderImagePrepareEvent;

public:

	GUIRenderHexagon();
	~GUIRenderHexagon();

	bool			isActive() const { return m_bActive; }
	void			SetImageTopName(const GUIString& strName);
	void			SetImageSideName(const GUIString& strName);
	void			SetImageOverlayerName(const GUIString& strName);	

	void			setTopColor(const Color& color) { m_colorTop = color; }
	void			setSideColor(const Color& color) { m_colorSide = color; }
	void			setOverlayerColor(const Color& color) { m_colorOverlayer = color; }	

	void			SetRenderHeight(float height) { m_height = height; }

	const GUIString& GetImageTopName() const { return m_imageTopWholeName; }
	const GUIString& GetImageSideName() const { return m_imageSideWholeName; }
	const GUIString& GetImageOverlayerName() const { return m_imageOverlayerWholeName; }

	const Color&	getTopColor() const { return m_colorTop; }
	const Color&	getSideColor() const { return m_colorSide; }
	const Color&	getOverlayerColor() const { return m_colorOverlayer; }

	void			DrawHexagon(const GUIWindow* window, const ERect& rtArea, const ERect* rtClip = NULL);	
	Vector2			GetImageSize();
	
	//
	void			SetVertexData(ImageBufferListType &vertex);
	void			DrawHexagon2(GUIWindow* window, const ERect& rtArea, const ERect& rtClip);

	// 为单线程加载用
	void			prepare_io();
	void			prepare_image();
	void			checkImage();

protected:
	void			drawCacheHexagon(const GUIWindow* window);

private:
	GUIString		m_imageTopWholeName;
	GUIString		m_imageSideWholeName;
	GUIString		m_imageOverlayerWholeName;

	GUIString		m_strImagesetName;
	GUIImageset*	m_pImagesetBlock;

	GUIString		m_strImageTopName;
	GUIString		m_strImageSideName;
	GUIString		m_strImageOverlayerName;
	GUIImage*		m_pImageTop;
	GUIImage*		m_pImageSide;
	GUIImage*		m_pImageOverlayer;
	Color			m_colorTop;
	Color			m_colorSide;
	Color			m_colorOverlayer;

	//渲染方块的高度 0.0~1.0
	float			m_height;

	bool			m_bActive;
	
	ImageBufferListType m_blockVertex;
	ImageBufferListType	m_actionVertex;

#ifdef CACHEGPUBUFFER
	ERect m_rtArea;
	unsigned int m_nColor;
	float m_fRotate;
	float m_fRotateY;
	float m_fRotateX;
	ERect m_rtClip;
	StretchOffset m_StretchOffset;
	StretchType m_StretchType;

	GUIVertexDeclaration*	m_pVertexSystemMemory;
	Word*					m_pIndexSystemMemory;
	GPUBuffer*				m_pVertexBuffer;
	GPUBuffer*				m_pIndexBuffer;
	int						m_VertexCount;
	int						m_IndexCount;
	bool					m_UpdateData;
#endif

};

}

#endif
