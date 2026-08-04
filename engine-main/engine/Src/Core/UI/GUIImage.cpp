#include "GUIImage.h"
#include "GUIRenderManager.h"
#include "GUIImageset.h"

namespace LORD
{

	//const int PIXEL_OFFSET = 10;

	GUIImage::GUIImage( GUIImageset* owner, const GUIString& name, const ERect& area, const ERect& rtPixelRect, const Vector2& offset)
		: m_pOwner(owner)
		, m_strName(name)
		, m_rtArea(area)
		, m_rtPixelRect(rtPixelRect)
		, m_offset(offset)
	{

	}

	GUIImage::GUIImage( const GUIImage& image )
		: m_pOwner(image.m_pOwner)
		, m_strName(image.m_strName)
		, m_rtArea(image.m_rtArea)
		, m_rtPixelRect(image.m_rtPixelRect)
		, m_offset(image.m_offset)
	{
	}

	GUIImage::~GUIImage( void )
	{

	}

	void GUIImage::UpdateImageData(GUIVertexDeclaration*& vertexData, const ERect& rtArea, const ColorRect& color, float fRotate, float fRotateY, float fRotateX, const ERect* rtClip /*= NULL*/, const Vector2& vScale /*= Vector2::ONE*/)
	{
		ERect rtDest = rtArea;
		rtDest.top = PixelAligned(rtArea.top);
		rtDest.left = PixelAligned(rtArea.left);
		rtDest.right = PixelAligned(rtDest.left + rtArea.getWidth() * vScale.x);
		rtDest.bottom = PixelAligned(rtDest.top + rtArea.getHeight() * vScale.y);

		ERect rtTexUV = m_rtArea;

		// 如果有裁剪
		if (rtClip)
		{
			rtDest = rtArea.intersect(*rtClip);
			if (rtDest.getHeight() == 0 || rtDest.getWidth() == 0)
			{
				vertexData[0].position = Vector3::ZERO;
				vertexData[0].color = 0;
				vertexData[0].texCoord.x = 0;
				vertexData[0].texCoord.y = 0;

				vertexData[1].position = Vector3::ZERO;
				vertexData[1].color = 0;
				vertexData[1].texCoord.x = 0;
				vertexData[1].texCoord.y = 0;

				vertexData[2].position = Vector3::ZERO;
				vertexData[2].color = 0;
				vertexData[2].texCoord.x = 0;
				vertexData[2].texCoord.y = 0;

				vertexData[3].position = Vector3::ZERO;
				vertexData[3].color = 0;
				vertexData[3].texCoord.x = 0;
				vertexData[3].texCoord.y = 0;
				return;
			}

			float fInverseWidth = m_rtArea.getWidth() / rtArea.getWidth();
			float fInverseHeight = m_rtArea.getHeight() / rtArea.getHeight();

			rtTexUV.left = m_rtArea.left + (rtDest.left - rtArea.left) * fInverseWidth;
			rtTexUV.top = m_rtArea.top + (rtDest.top - rtArea.top) * fInverseHeight;
			rtTexUV.right = m_rtArea.left + (rtDest.right - rtArea.left) * fInverseWidth;
			rtTexUV.bottom = m_rtArea.top + (rtDest.bottom - rtArea.top) * fInverseHeight;
		}

		Vector2 newOffset;
		newOffset.x = PixelAligned(m_offset.x * vScale.x);
		newOffset.y = PixelAligned(m_offset.y * vScale.y);
		rtDest.offset(newOffset);

		float halfWidth = rtDest.getWidth() * 0.5f;
		float halfHeight = rtDest.getHeight() * 0.5f;
		Vector3 center = Vector3(rtDest.left + halfWidth, rtDest.top + halfHeight, 0.0f);

		Vector3 lt, rb, lb, rt;
		// 计算旋转
		CaculateRotate(rtDest, center, Vector3(fRotateX, fRotateY, fRotate), lt, rb, lb, rt);

		vertexData[0].position = lt;
		vertexData[0].color = color.cLeftTop.getABGR();
		vertexData[0].texCoord.x = rtTexUV.left;
		vertexData[0].texCoord.y = rtTexUV.top;

		vertexData[1].position = rb;
		vertexData[1].color = color.cRightBottom.getABGR();
		vertexData[1].texCoord.x = rtTexUV.right;
		vertexData[1].texCoord.y = rtTexUV.bottom;

		vertexData[2].position = lb;
		vertexData[2].color = color.cLeftBottom.getABGR();
		vertexData[2].texCoord.x = rtTexUV.left;
		vertexData[2].texCoord.y = rtTexUV.bottom;

		vertexData[3].position = rt;
		vertexData[3].color = color.cRightTop.getABGR();
		vertexData[3].texCoord.x = rtTexUV.right;
		vertexData[3].texCoord.y = rtTexUV.top;
	}
	
	void GUIImage::UpdateStretchImageData(GUIVertexDeclaration*& vertexData, const ERect& rtArea, const unsigned int color, float fRotate, float fRotateY, float fRotateX, const ERect* rtClip, const StretchOffset& offset) const
	{
		ERect rtDestRect[RAT_MAX_TYPE];
		ERect rtUV[RAT_MAX_TYPE];

		for (ui32 i = 0; i < RAT_MAX_TYPE; ++i)
		{
			rtDestRect[i] = rtArea;
			rtUV[i] = m_rtArea;
		}

		//	-----------------
		//|		0 			|
		//|		---------	|
		//|		|		|	|
		//|		|		|	|
		//|		|		|	|
		//|		---------1	|
		//|					|
		//	-----------------

		Vector2 helperAddress0;
		Vector2 helperAddress1;
		helperAddress0.x = (m_rtPixelRect.left + offset.fLeft)*m_pOwner->m_fInverseNativeHorzRes;
		helperAddress0.y = (m_rtPixelRect.top + offset.fTop)*m_pOwner->m_fInverseNativeVertRes;
		helperAddress1.x = (m_rtPixelRect.right - offset.fRight)*m_pOwner->m_fInverseNativeHorzRes;
		helperAddress1.y = (m_rtPixelRect.bottom - offset.fBottom)*m_pOwner->m_fInverseNativeVertRes;

		Vector2 helperRect0;
		Vector2 helperRect1;
		helperRect0.x = rtArea.left + offset.fLeft;
		helperRect0.y = rtArea.top + offset.fTop;
		helperRect1.x = rtArea.right - offset.fRight;
		helperRect1.y = rtArea.bottom - offset.fBottom;

		// lt
		rtDestRect[RAT_LEFT_TOP].right = helperRect0.x;
		rtDestRect[RAT_LEFT_TOP].bottom = helperRect0.y;
		rtUV[RAT_LEFT_TOP].right = helperAddress0.x;
		rtUV[RAT_LEFT_TOP].bottom = helperAddress0.y;

		// mt
		rtDestRect[RAT_MIDDLE_TOP].left = helperRect0.x;
		rtDestRect[RAT_MIDDLE_TOP].right = helperRect1.x;
		rtDestRect[RAT_MIDDLE_TOP].bottom = helperRect0.y;
		rtUV[RAT_MIDDLE_TOP].left = helperAddress0.x;
		rtUV[RAT_MIDDLE_TOP].right = helperAddress1.x;
		rtUV[RAT_MIDDLE_TOP].bottom = helperAddress0.y;

		// rt
		rtDestRect[RAT_RIGHT_TOP].left = helperRect1.x;
		rtDestRect[RAT_RIGHT_TOP].bottom = helperRect0.y;
		rtUV[RAT_RIGHT_TOP].left = helperAddress1.x;
		rtUV[RAT_RIGHT_TOP].bottom = helperAddress0.y;

		// lm
		rtDestRect[RAT_LEFT_MIDDLE].top = helperRect0.y;
		rtDestRect[RAT_LEFT_MIDDLE].right = helperRect0.x;
		rtDestRect[RAT_LEFT_MIDDLE].bottom = helperRect1.y;
		rtUV[RAT_LEFT_MIDDLE].top = helperAddress0.y;
		rtUV[RAT_LEFT_MIDDLE].right = helperAddress0.x;
		rtUV[RAT_LEFT_MIDDLE].bottom = helperAddress1.y;

		// mm
		rtDestRect[RAT_MIDDLE_MIDDLE].left = helperRect0.x;
		rtDestRect[RAT_MIDDLE_MIDDLE].top = helperRect0.y;
		rtDestRect[RAT_MIDDLE_MIDDLE].right = helperRect1.x;
		rtDestRect[RAT_MIDDLE_MIDDLE].bottom = helperRect1.y;
		rtUV[RAT_MIDDLE_MIDDLE].left = helperAddress0.x;
		rtUV[RAT_MIDDLE_MIDDLE].top = helperAddress0.y;
		rtUV[RAT_MIDDLE_MIDDLE].right = helperAddress1.x;
		rtUV[RAT_MIDDLE_MIDDLE].bottom = helperAddress1.y;

		// rm
		rtDestRect[RAT_RIGHT_MIDDLE].left = helperRect1.x;
		rtDestRect[RAT_RIGHT_MIDDLE].top = helperRect0.y;
		rtDestRect[RAT_RIGHT_MIDDLE].bottom = helperRect1.y;
		rtUV[RAT_RIGHT_MIDDLE].left = helperAddress1.x;
		rtUV[RAT_RIGHT_MIDDLE].top = helperAddress0.y;
		rtUV[RAT_RIGHT_MIDDLE].bottom = helperAddress1.y;

		// lb
		rtDestRect[RAT_LEFT_BOTTOM].top = helperRect1.y;
		rtDestRect[RAT_LEFT_BOTTOM].right = helperRect0.x;
		rtUV[RAT_LEFT_BOTTOM].top = helperAddress1.y;
		rtUV[RAT_LEFT_BOTTOM].right = helperAddress0.x;

		// mb
		rtDestRect[RAT_MIDDLE_BOTTOM].left = helperRect0.x;
		rtDestRect[RAT_MIDDLE_BOTTOM].top = helperRect1.y;
		rtDestRect[RAT_MIDDLE_BOTTOM].right = helperRect1.x;
		rtUV[RAT_MIDDLE_BOTTOM].left = helperAddress0.x;
		rtUV[RAT_MIDDLE_BOTTOM].top = helperAddress1.y;
		rtUV[RAT_MIDDLE_BOTTOM].right = helperAddress1.x;

		// rb
		rtDestRect[RAT_RIGHT_BOTTOM].left = helperRect1.x;
		rtDestRect[RAT_RIGHT_BOTTOM].top = helperRect1.y;
		rtUV[RAT_RIGHT_BOTTOM].left = helperAddress1.x;
		rtUV[RAT_RIGHT_BOTTOM].top = helperAddress1.y;

		float halfWidth = rtArea.getWidth() * 0.5f;
		float halfHeight = rtArea.getHeight() * 0.5f;
		Vector3 center = Vector3(rtArea.left + halfWidth, rtArea.top + halfHeight, 0.0f);

		GUIVertexDeclaration* pVertex = vertexData;
		for (ui32 i = 0; i < RAT_MAX_TYPE; ++i)
		{
			// 如果有裁剪
			if (rtClip)
			{
				ERect oldRect = rtDestRect[i];
				ERect oldUV = rtUV[i];
				rtDestRect[i] = rtDestRect[i].intersect(*rtClip);
				if (rtDestRect[i].getHeight() == 0 || rtDestRect[i].getWidth() == 0)
				{
					pVertex[0].position = Vector3::ZERO;
					pVertex[0].color = 0;
					pVertex[0].texCoord.x = 0;
					pVertex[0].texCoord.y = 0;

					pVertex[1].position = Vector3::ZERO;
					pVertex[1].color = 0;
					pVertex[1].texCoord.x = 0;
					pVertex[1].texCoord.y = 0;

					pVertex[2].position = Vector3::ZERO;
					pVertex[2].color = 0;
					pVertex[2].texCoord.x = 0;
					pVertex[2].texCoord.y = 0;

					pVertex[3].position = Vector3::ZERO;
					pVertex[3].color = 0;
					pVertex[3].texCoord.x = 0;
					pVertex[3].texCoord.y = 0;
					pVertex += 4;
					continue;
				}

				float fInverseWidth = rtUV[i].getWidth() / oldRect.getWidth();
				float fInverseHeight = rtUV[i].getHeight() / oldRect.getHeight();

				rtUV[i].left = oldUV.left + (rtDestRect[i].left - oldRect.left) * fInverseWidth;
				rtUV[i].top = oldUV.top + (rtDestRect[i].top - oldRect.top) * fInverseHeight;
				rtUV[i].right = oldUV.left + (rtDestRect[i].right - oldRect.left) * fInverseWidth;
				rtUV[i].bottom = oldUV.top + (rtDestRect[i].bottom - oldRect.top) * fInverseHeight;
			}

			Vector2 newOffset;
			newOffset.x = PixelAligned(m_offset.x);
			newOffset.y = PixelAligned(m_offset.y);
			rtDestRect[i].offset(newOffset);

			Color cTemp;
			cTemp.setABGR(color);
			ColorRect cRect(cTemp, cTemp, cTemp, cTemp);

			Vector3 lt, rb, lb, rt;
			// 计算旋转
			CaculateRotate(rtDestRect[i], center, Vector3(fRotateX, fRotateY, fRotate), lt, rb, lb, rt);

			pVertex[0].position = lt;
			pVertex[0].color = cRect.cLeftTop.getABGR();
			pVertex[0].texCoord.x = rtUV[i].left;
			pVertex[0].texCoord.y = rtUV[i].top;

			pVertex[1].position = rb;
			pVertex[1].color = cRect.cRightBottom.getABGR();
			pVertex[1].texCoord.x = rtUV[i].right;
			pVertex[1].texCoord.y = rtUV[i].bottom;

			pVertex[2].position = lb;
			pVertex[2].color = cRect.cLeftBottom.getABGR();
			pVertex[2].texCoord.x = rtUV[i].left;
			pVertex[2].texCoord.y = rtUV[i].bottom;

			pVertex[3].position = rt;
			pVertex[3].color = cRect.cRightTop.getABGR();
			pVertex[3].texCoord.x = rtUV[i].right;
			pVertex[3].texCoord.y = rtUV[i].top;
			pVertex += 4;
		}
	}

	void GUIImage::UpdateStretchImageTBData(GUIVertexDeclaration*& vertexData, const ERect& rtArea, const unsigned int color, float fRotate, float fRotateY, float fRotateX, const ERect* rtClip, const StretchOffset& offset) const
	{
		ERect rtDestRect[3];
		ERect rtUV[3];

		for (ui32 i = 0; i < 3; ++i)
		{
			rtDestRect[i] = rtArea;
			rtUV[i] = m_rtArea;
		}
		float helperAddress0 = (m_rtPixelRect.top + offset.fTop)*m_pOwner->m_fInverseNativeVertRes;
		float helperAddress1 = (m_rtPixelRect.bottom - offset.fBottom)*m_pOwner->m_fInverseNativeVertRes;

		float helper0 = rtArea.top + offset.fTop;
		float helper1 = rtArea.bottom - offset.fBottom;
		//Top		
		rtDestRect[0].bottom = helper0;
		rtUV[0].bottom = helperAddress0;
		//Center
		rtDestRect[1].top = helper0;
		rtDestRect[1].bottom = helper1;
		rtUV[1].top = helperAddress0;
		rtUV[1].bottom = helperAddress1;
		//Bottom
		rtDestRect[2].top = helper1;
		rtUV[2].top = helperAddress1;

		float halfWidth = rtArea.getWidth() * 0.5f;
		float halfHeight = rtArea.getHeight() * 0.5f;
		Vector3 center = Vector3(rtArea.left + halfWidth, rtArea.top + halfHeight, 0.0f);

		GUIVertexDeclaration* pVertex = vertexData;
		for (ui32 i = 0; i < 3; ++i)
		{
			// 如果有裁剪
			if (rtClip)
			{
				ERect oldRect = rtDestRect[i];
				ERect oldUV = rtUV[i];
				rtDestRect[i] = rtDestRect[i].intersect(*rtClip);
				if (rtDestRect[i].getHeight() == 0 || rtDestRect[i].getWidth() == 0)
				{
					pVertex[0].position = Vector3::ZERO;
					pVertex[0].color = 0;
					pVertex[0].texCoord.x = 0;
					pVertex[0].texCoord.y = 0;

					pVertex[1].position = Vector3::ZERO;
					pVertex[1].color = 0;
					pVertex[1].texCoord.x = 0;
					pVertex[1].texCoord.y = 0;

					pVertex[2].position = Vector3::ZERO;
					pVertex[2].color = 0;
					pVertex[2].texCoord.x = 0;
					pVertex[2].texCoord.y = 0;

					pVertex[3].position = Vector3::ZERO;
					pVertex[3].color = 0;
					pVertex[3].texCoord.x = 0;
					pVertex[3].texCoord.y = 0;
					pVertex += 4;
					continue;
				}

				float fInverseWidth = rtUV[i].getWidth() / oldRect.getWidth();
				float fInverseHeight = rtUV[i].getHeight() / oldRect.getHeight();

				rtUV[i].left = oldUV.left + (rtDestRect[i].left - oldRect.left) * fInverseWidth;
				rtUV[i].top = oldUV.top + (rtDestRect[i].top - oldRect.top) * fInverseHeight;
				rtUV[i].right = oldUV.left + (rtDestRect[i].right - oldRect.left) * fInverseWidth;
				rtUV[i].bottom = oldUV.top + (rtDestRect[i].bottom - oldRect.top) * fInverseHeight;
			}

			Vector2 newOffset;
			newOffset.x = PixelAligned(m_offset.x);
			newOffset.y = PixelAligned(m_offset.y);
			rtDestRect[i].offset(newOffset);

			Color cTemp;
			cTemp.setABGR(color);
			ColorRect cRect(cTemp, cTemp, cTemp, cTemp);

			Vector3 lt, rb, lb, rt;
			// 计算旋转
			CaculateRotate(rtDestRect[i], center, Vector3(fRotateX, fRotateY, fRotate), lt, rb, lb, rt);

			pVertex[0].position = lt;
			pVertex[0].color = cRect.cLeftTop.getABGR();
			pVertex[0].texCoord.x = rtUV[i].left;
			pVertex[0].texCoord.y = rtUV[i].top;

			pVertex[1].position = rb;
			pVertex[1].color = cRect.cRightBottom.getABGR();
			pVertex[1].texCoord.x = rtUV[i].right;
			pVertex[1].texCoord.y = rtUV[i].bottom;

			pVertex[2].position = lb;
			pVertex[2].color = cRect.cLeftBottom.getABGR();
			pVertex[2].texCoord.x = rtUV[i].left;
			pVertex[2].texCoord.y = rtUV[i].bottom;

			pVertex[3].position = rt;
			pVertex[3].color = cRect.cRightTop.getABGR();
			pVertex[3].texCoord.x = rtUV[i].right;
			pVertex[3].texCoord.y = rtUV[i].top;

			pVertex += 4;
		}
	}

	void GUIImage::UpdateStretchImageLRData(GUIVertexDeclaration*& vertexData, const ERect& rtArea, const unsigned int color, float fRotate, float fRotateY, float fRotateX, const ERect* rtClip, const StretchOffset& offset) const
	{
		ERect rtDestRect[3];
		ERect rtUV[3];

		for (ui32 i = 0; i < 3; ++i)
		{
			rtDestRect[i] = rtArea;
			rtUV[i] = m_rtArea;
		}
		float helperAddress0 = (m_rtPixelRect.left + offset.fLeft)*m_pOwner->m_fInverseNativeHorzRes;
		float helperAddress1 = (m_rtPixelRect.right - offset.fRight)*m_pOwner->m_fInverseNativeHorzRes;

		float helper0 = rtArea.left + offset.fLeft;
		float helper1 = rtArea.right - offset.fRight;
		//left		
		rtDestRect[0].right = helper0;
		rtUV[0].right = helperAddress0;
		//Center
		rtDestRect[1].left = helper0;
		rtDestRect[1].right = helper1;
		rtUV[1].left = helperAddress0;
		rtUV[1].right = helperAddress1;
		//Right
		rtDestRect[2].left = helper1;
		rtUV[2].left = helperAddress1;

		float halfWidth = rtArea.getWidth() * 0.5f;
		float halfHeight = rtArea.getHeight() * 0.5f;
		Vector3 center = Vector3(rtArea.left + halfWidth, rtArea.top + halfHeight, 0.0f);

		GUIVertexDeclaration* pVertex = vertexData;
		for (ui32 i = 0; i < 3; ++i)
		{
			// 如果有裁剪
			if (rtClip)
			{
				ERect oldRect = rtDestRect[i];
				ERect oldUV = rtUV[i];
				rtDestRect[i] = rtDestRect[i].intersect(*rtClip);
				if (rtDestRect[i].getHeight() == 0 || rtDestRect[i].getWidth() == 0)
				{
					pVertex[0].position = Vector3::ZERO;
					pVertex[0].color = 0;
					pVertex[0].texCoord.x = 0;
					pVertex[0].texCoord.y = 0;

					pVertex[1].position = Vector3::ZERO;
					pVertex[1].color = 0;
					pVertex[1].texCoord.x = 0;
					pVertex[1].texCoord.y = 0;

					pVertex[2].position = Vector3::ZERO;
					pVertex[2].color = 0;
					pVertex[2].texCoord.x = 0;
					pVertex[2].texCoord.y = 0;

					pVertex[3].position = Vector3::ZERO;
					pVertex[3].color = 0;
					pVertex[3].texCoord.x = 0;
					pVertex[3].texCoord.y = 0;
					pVertex += 4;
					continue;
				}

				float fInverseWidth = rtUV[i].getWidth() / oldRect.getWidth();
				float fInverseHeight = rtUV[i].getHeight() / oldRect.getHeight();

				rtUV[i].left = oldUV.left + (rtDestRect[i].left - oldRect.left) * fInverseWidth;
				rtUV[i].top = oldUV.top + (rtDestRect[i].top - oldRect.top) * fInverseHeight;
				rtUV[i].right = oldUV.left + (rtDestRect[i].right - oldRect.left) * fInverseWidth;
				rtUV[i].bottom = oldUV.top + (rtDestRect[i].bottom - oldRect.top) * fInverseHeight;
			}

			Vector2 newOffset;
			newOffset.x = PixelAligned(m_offset.x);
			newOffset.y = PixelAligned(m_offset.y);
			rtDestRect[i].offset(newOffset);

			Color cTemp;
			cTemp.setABGR(color);
			ColorRect cRect(cTemp, cTemp, cTemp, cTemp);

			Vector3 lt, rb, lb, rt;
			// 计算旋转
			CaculateRotate(rtDestRect[i], center, Vector3(fRotateX, fRotateY, fRotate), lt, rb, lb, rt);

			pVertex[0].position = lt;
			pVertex[0].color = cRect.cLeftTop.getABGR();
			pVertex[0].texCoord.x = rtUV[i].left;
			pVertex[0].texCoord.y = rtUV[i].top;

			pVertex[1].position = rb;
			pVertex[1].color = cRect.cRightBottom.getABGR();
			pVertex[1].texCoord.x = rtUV[i].right;
			pVertex[1].texCoord.y = rtUV[i].bottom;

			pVertex[2].position = lb;
			pVertex[2].color = cRect.cLeftBottom.getABGR();
			pVertex[2].texCoord.x = rtUV[i].left;
			pVertex[2].texCoord.y = rtUV[i].bottom;

			pVertex[3].position = rt;
			pVertex[3].color = cRect.cRightTop.getABGR();
			pVertex[3].texCoord.x = rtUV[i].right;
			pVertex[3].texCoord.y = rtUV[i].top;

			pVertex += 4;
		}
	}

	void GUIImage::DrawHexagon(ImageBufferListType& vertex, GUIImage* pImageSide, GUIImage* pImageOverlayer, const ERect rtArea,
		const Color& topColor, const Color& sideColor, const Color& overlayerColor,float height, const ERect* rtClip, const Vector2& vScale) const
	{
		ERect rtDest = rtArea;
		rtDest.top = PixelAligned(rtArea.top);
		rtDest.left = PixelAligned(rtArea.left);
		rtDest.right = PixelAligned(rtDest.left + rtArea.getWidth() * vScale.x);
		rtDest.bottom = PixelAligned(rtDest.top + rtArea.getHeight() * vScale.y);

		// make square
		rtDest = rtDest.makeSquare();

		ERect rtTexUV = m_rtArea;
		ERect rtTexUV_side = m_rtArea;
		ERect rtTexUV_overlayer = m_rtArea;
		if (pImageSide)
		{
			//modify by maxicheng (set side uv)
			ERect uvSide = pImageSide->m_rtArea;			
			uvSide.top += (1.0f - height) * 0.03125f;
			rtTexUV_side = uvSide;

			//rtTexUV_side = pImageSide->m_rtArea;
		}
			
		if (pImageOverlayer)
			rtTexUV_overlayer = pImageOverlayer->m_rtArea;

		float w = rtDest.getWidth();
		float h = rtDest.getHeight();
		float x2 = rtDest.left + w / 2.f;
		float y2 = rtDest.top + w / 2.f;
		float f0 = 0.441941738f * w;
		float f1 = 0.270632952f * h;
		float f2 = 0.491603792f * h;
		float f3 = 0.0496620759f * h;
		
		float offsetHeight = (1.0f - height) * (f2);
		
		Vector3 t[7];
		t[0] = Vector3(-f0 + x2, -f1 + y2 + offsetHeight, 0); //
		t[1] = Vector3(0.f + x2, -f2 + y2 + offsetHeight, 0);
		t[2] = Vector3(f0 + x2, -f1 + y2 + offsetHeight, 0.f);
		t[3] = Vector3(0.f + x2, -f3 + y2 + offsetHeight, 0.f);
		t[4] = Vector3(0.f + x2, f2 + y2, 0.f);
		t[5] = Vector3(f0 + x2, f1 + y2, 0.f);
		t[6] = Vector3(-f0 + x2, f1 + y2, 0.f);
					
		float brightLeft = 0.5f;
		float brightRight = 0.7f;
		// 顶点缓存起来
		if (vertex.empty())
		{
			ImageBuffer temp;
			temp.lt = t[0];
			temp.rb = t[2];
			temp.lb = t[3];
			temp.rt = t[1];
			temp.uv = rtTexUV;
			temp.color = ColorRect(topColor);
			temp.imageset = m_pOwner;
			vertex.push_back(temp);
			
			temp.lt = t[3];
			temp.rb = t[5];
			temp.lb = t[4];
			temp.rt = t[2];
			temp.color = ColorRect(sideColor * brightRight);
			temp.uv = rtTexUV_side;
			vertex.push_back(temp);
			const ImageBuffer& temp1 = vertex.back();
		
			temp.lt = t[0];
			temp.rb = t[4];
			temp.lb = t[6];
			temp.rt = t[3];
			temp.color = ColorRect(sideColor * brightLeft);
			vertex.push_back(temp);
			const ImageBuffer& temp2 = vertex.back();

			if (pImageOverlayer)
			{
				temp = temp1;
				temp.color = ColorRect(overlayerColor * brightRight);
				temp.uv = rtTexUV_overlayer;
				vertex.push_back(temp);

				temp = temp2;
				temp.color = ColorRect(overlayerColor * brightLeft);
				temp.uv = rtTexUV_overlayer;
				vertex.push_back(temp);
			}
		}
	}

	void GUIImage::DrawImage(ImageBufferListType& vertex, const ERect& rtArea, const ColorRect& color, float fRotate,
							float fRotateY, float fRotateX, const ERect* rtClip, const Vector2& vScale) const
	{
		ERect rtDest = rtArea;
		rtDest.top = PixelAligned(rtArea.top);
		rtDest.left = PixelAligned(rtArea.left);
		rtDest.right = PixelAligned(rtDest.left + rtArea.getWidth() * vScale.x);
		rtDest.bottom = PixelAligned(rtDest.top + rtArea.getHeight() * vScale.y);

		ERect rtTexUV = m_rtArea;

		// 如果有裁剪
		if (rtClip)
		{
			rtDest = rtArea.intersect(*rtClip);
			if (rtDest.getHeight() == 0 || rtDest.getWidth() == 0)
			{
				return;
			}

			float fInverseWidth = m_rtArea.getWidth() / rtArea.getWidth();
			float fInverseHeight = m_rtArea.getHeight() / rtArea.getHeight();

			rtTexUV.left = m_rtArea.left + (rtDest.left - rtArea.left) * fInverseWidth;
			rtTexUV.top = m_rtArea.top + (rtDest.top - rtArea.top) * fInverseHeight;
			rtTexUV.right = m_rtArea.left + (rtDest.right - rtArea.left) * fInverseWidth;
			rtTexUV.bottom = m_rtArea.top + (rtDest.bottom - rtArea.top) * fInverseHeight;
		}

		Vector2 newOffset;
		newOffset.x = PixelAligned(m_offset.x * vScale.x);
		newOffset.y = PixelAligned(m_offset.y * vScale.y);
		rtDest.offset(newOffset);

		float halfWidth = rtDest.getWidth() * 0.5f;
		float halfHeight = rtDest.getHeight() * 0.5f;
		Vector3 center = Vector3(rtDest.left + halfWidth, rtDest.top + halfHeight, 0.0f);

		Vector3 lt, rb, lb, rt;
		// 计算旋转
		CaculateRotate(rtDest, center, Vector3(fRotateX, fRotateY, fRotate), lt, rb, lb, rt);

// 		if (vertex.empty())
// 		{
// 			vertex.push_back(lt);
// 			vertex.push_back(rb);
// 			vertex.push_back(lb);
// 			vertex.push_back(rt);
// 			m_pOwner->DrawImage(lt * actionMatrix, rb * actionMatrix, lb * actionMatrix, rt * actionMatrix, rtTexUV, color);
// 		} 
// 		else
// 		{
// 			if (vertex.size() == 4)
// 			{
// 				m_pOwner->DrawImage(vertex[0] * actionMatrix, vertex[1] * actionMatrix, vertex[2] * actionMatrix, vertex[3] * actionMatrix, rtTexUV, color);
// 			}
// 		}

		// 顶点缓存起来
		if (vertex.empty())
		{
			ImageBuffer temp;
			temp.lt = lt;
			temp.rb = rb;
			temp.lb = lb;
			temp.rt = rt;
			temp.uv = rtTexUV;
			temp.color = color;
			temp.imageset = m_pOwner;
			vertex.push_back(temp);
		}
	}

	void GUIImage::DrawStretchImageTB(ImageBufferListType& vertex, const ERect& rtArea, const unsigned int color,
									float fRotate, float fRotateY, float fRotateX, 
									const ERect* rtClip, const StretchOffset& offset) const
	{
		ERect rtDestRect[3];
		ERect rtUV[3];

		for (size_t i = 0; i < 3; ++i)
		{
			rtDestRect[i] = rtArea;
			rtUV[i] = m_rtArea;
		}
		float helperAddress0 = (m_rtPixelRect.top+offset.fTop)*m_pOwner->m_fInverseNativeVertRes;
		float helperAddress1 = (m_rtPixelRect.bottom - offset.fBottom)*m_pOwner->m_fInverseNativeVertRes;

		float helper0 = rtArea.top + offset.fTop;
		float helper1 = rtArea.bottom - offset.fBottom;
		//Top		
		rtDestRect[0].bottom = helper0;	
		rtUV[0].bottom = helperAddress0;
		//Center
		rtDestRect[1].top = helper0;	
		rtDestRect[1].bottom = helper1;	
		rtUV[1].top = helperAddress0;
		rtUV[1].bottom = helperAddress1;
		//Bottom
		rtDestRect[2].top = helper1;	
		rtUV[2].top = helperAddress1;

		float halfWidth = rtArea.getWidth() * 0.5f;
		float halfHeight = rtArea.getHeight() * 0.5f;
		Vector3 center = Vector3(rtArea.left + halfWidth, rtArea.top + halfHeight, 0.0f);

		// bool insertVertex = vertex.empty();

		for (uint i = 0; i < 3; ++i)
		{
			// 如果有裁剪
			if (rtClip)
			{
				ERect oldRect = rtDestRect[i];
				ERect oldUV = rtUV[i];
				rtDestRect[i] = rtDestRect[i].intersect(*rtClip);
				if (rtDestRect[i].getHeight() == 0 || rtDestRect[i].getWidth() == 0)
				{
					continue;
				}

				float fInverseWidth = rtUV[i].getWidth() / oldRect.getWidth();
				float fInverseHeight = rtUV[i].getHeight() / oldRect.getHeight();

				rtUV[i].left = oldUV.left + (rtDestRect[i].left - oldRect.left) * fInverseWidth;
				rtUV[i].top = oldUV.top + (rtDestRect[i].top - oldRect.top) * fInverseHeight;
				rtUV[i].right = oldUV.left + (rtDestRect[i].right - oldRect.left) * fInverseWidth;
				rtUV[i].bottom = oldUV.top + (rtDestRect[i].bottom - oldRect.top) * fInverseHeight;
			}

			Vector2 newOffset;
			newOffset.x = PixelAligned(m_offset.x);
			newOffset.y = PixelAligned(m_offset.y);
			rtDestRect[i].offset(newOffset);

			Color cTemp;
			cTemp.setABGR(color);

			Vector3 lt, rb, lb, rt;
			// 计算旋转
			CaculateRotate(rtDestRect[i], center, Vector3(fRotateX, fRotateY, fRotate), lt, rb, lb, rt);

			ImageBuffer temp;
			temp.lt = lt;
			temp.rb = rb;
			temp.lb = lb;
			temp.rt = rt;
			temp.uv = rtUV[i];
			temp.color = ColorRect(cTemp, cTemp, cTemp, cTemp);
			temp.imageset = m_pOwner;
			vertex.push_back(temp);

// 			if (insertVertex)
// 			{
// 				vertex.push_back(lt);
// 				vertex.push_back(rb);
// 				vertex.push_back(lb);
// 				vertex.push_back(rt);
// 				m_pOwner->DrawImage(lt*actionMatrix, rb*actionMatrix, lb*actionMatrix, rt*actionMatrix, rtUV[i], ColorRect(cTemp, cTemp, cTemp, cTemp));
// 			}
// 			else
// 			{
// 				if (vertex.size() == 12)
// 				{
// 					int offset = i * 4;
// 					m_pOwner->DrawImage(vertex[offset] * actionMatrix, vertex[offset + 1] * actionMatrix, vertex[offset + 2] * actionMatrix, vertex[offset + 3] * actionMatrix, rtUV[i], ColorRect(cTemp, cTemp, cTemp, cTemp));
// 				}
// 			}
		}		
	}

	void GUIImage::DrawStretchImageLR(ImageBufferListType& vertex, const ERect& rtArea, const unsigned int color, float fRotate,
										float fRotateY, float fRotateX, const ERect* rtClip, const StretchOffset& offset) const
	{
		ERect rtDestRect[3];
		ERect rtUV[3];

		for (size_t i = 0; i < 3; ++i)
		{
			rtDestRect[i] = rtArea;
			rtUV[i] = m_rtArea;
		}
		float helperAddress0 = (m_rtPixelRect.left+offset.fLeft)*m_pOwner->m_fInverseNativeHorzRes;
		float helperAddress1 = (m_rtPixelRect.right - offset.fRight)*m_pOwner->m_fInverseNativeHorzRes;

		float helper0 = rtArea.left + offset.fLeft;
		float helper1 = rtArea.right - offset.fRight;
		//left		
		rtDestRect[0].right = helper0;	
		rtUV[0].right = helperAddress0;
		//Center
		rtDestRect[1].left = helper0;	
		rtDestRect[1].right = helper1;	
		rtUV[1].left = helperAddress0;
		rtUV[1].right = helperAddress1;
		//Right
		rtDestRect[2].left = helper1;	
		rtUV[2].left = helperAddress1;

		float halfWidth = rtArea.getWidth() * 0.5f;
		float halfHeight = rtArea.getHeight() * 0.5f;
		Vector3 center = Vector3(rtArea.left + halfWidth, rtArea.top + halfHeight, 0.0f);

		// bool insertVertex = vertex.empty();

		for (int i = 0; i < 3; ++i)
		{
			// 如果有裁剪
			if (rtClip)
			{
				ERect oldRect = rtDestRect[i];
				ERect oldUV = rtUV[i];
				rtDestRect[i] = rtDestRect[i].intersect(*rtClip);
				if (rtDestRect[i].getHeight() == 0 || rtDestRect[i].getWidth() == 0)
				{
					continue;
				}

				float fInverseWidth = rtUV[i].getWidth() / oldRect.getWidth();
				float fInverseHeight = rtUV[i].getHeight() / oldRect.getHeight();

				rtUV[i].left = oldUV.left + (rtDestRect[i].left - oldRect.left) * fInverseWidth;
				rtUV[i].top = oldUV.top + (rtDestRect[i].top - oldRect.top) * fInverseHeight;
				rtUV[i].right = oldUV.left + (rtDestRect[i].right - oldRect.left) * fInverseWidth;
				rtUV[i].bottom = oldUV.top + (rtDestRect[i].bottom - oldRect.top) * fInverseHeight;
			}

			Vector2 newOffset;
			newOffset.x = PixelAligned(m_offset.x);
			newOffset.y = PixelAligned(m_offset.y);
			rtDestRect[i].offset(newOffset);

			Color cTemp;
			cTemp.setABGR(color);

			Vector3 lt, rb, lb, rt;
			// 计算旋转
			CaculateRotate(rtDestRect[i], center, Vector3(fRotateX, fRotateY, fRotate), lt, rb, lb, rt);

// 			if (insertVertex)
// 			{
// 				vertex.push_back(lt);
// 				vertex.push_back(rb);
// 				vertex.push_back(lb);
// 				vertex.push_back(rt);
// 				m_pOwner->DrawImage(lt*actionMatrix, rb*actionMatrix, lb*actionMatrix, rt*actionMatrix, rtUV[i], ColorRect(cTemp, cTemp, cTemp, cTemp));
// 			} 
// 			else
// 			{
// 				if (vertex.size() == 12)
// 				{
// 					int offset = i * 4;
// 					m_pOwner->DrawImage(vertex[offset] * actionMatrix, vertex[offset + 1] * actionMatrix, vertex[offset + 2] * actionMatrix, vertex[offset + 3] * actionMatrix, rtUV[i], ColorRect(cTemp, cTemp, cTemp, cTemp));
// 				}
// 			}

			ImageBuffer temp;
			temp.lt = lt;
			temp.rb = rb;
			temp.lb = lb;
			temp.rt = rt;
			temp.uv = rtUV[i];
			temp.color = ColorRect(cTemp, cTemp, cTemp, cTemp);
			temp.imageset = m_pOwner;
			vertex.push_back(temp);
		}		
	}

	void GUIImage::DrawStretchImage(ImageBufferListType& vertex, const ERect& rtArea, const unsigned int color, float fRotate,
									float fRotateY, float fRotateX, const ERect* rtClip, const StretchOffset& offset) const
	{
		ERect rtDestRect[RAT_MAX_TYPE];
		ERect rtUV[RAT_MAX_TYPE];

		for (size_t i = 0; i < RAT_MAX_TYPE; ++i)
		{
			rtDestRect[i] = rtArea;
			rtUV[i] = m_rtArea;
		}
		
			//	-----------------
			//|		0 			|
			//|		---------	|
			//|		|		|	|
			//|		|		|	|
			//|		|		|	|
			//|		---------1	|
			//|					|
			//	-----------------

		Vector2 helperAddress0;
		Vector2 helperAddress1;
		helperAddress0.x = (m_rtPixelRect.left+offset.fLeft)*m_pOwner->m_fInverseNativeHorzRes;
		helperAddress0.y = (m_rtPixelRect.top+offset.fTop)*m_pOwner->m_fInverseNativeVertRes;
		helperAddress1.x = (m_rtPixelRect.right - offset.fRight)*m_pOwner->m_fInverseNativeHorzRes;
		helperAddress1.y = (m_rtPixelRect.bottom - offset.fBottom)*m_pOwner->m_fInverseNativeVertRes;

		Vector2 helperRect0;
		Vector2 helperRect1;
		helperRect0.x = rtArea.left + offset.fLeft;
		helperRect0.y = rtArea.top + offset.fTop;
		helperRect1.x = rtArea.right - offset.fRight;
		helperRect1.y = rtArea.bottom - offset.fBottom;

		// lt
		rtDestRect[RAT_LEFT_TOP].right = helperRect0.x;
		rtDestRect[RAT_LEFT_TOP].bottom = helperRect0.y;
		rtUV[RAT_LEFT_TOP].right = helperAddress0.x;
		rtUV[RAT_LEFT_TOP].bottom = helperAddress0.y;

		// mt
		rtDestRect[RAT_MIDDLE_TOP].left = helperRect0.x;
		rtDestRect[RAT_MIDDLE_TOP].right = helperRect1.x;
		rtDestRect[RAT_MIDDLE_TOP].bottom = helperRect0.y;
		rtUV[RAT_MIDDLE_TOP].left = helperAddress0.x;
		rtUV[RAT_MIDDLE_TOP].right = helperAddress1.x;
		rtUV[RAT_MIDDLE_TOP].bottom = helperAddress0.y;

		// rt
		rtDestRect[RAT_RIGHT_TOP].left = helperRect1.x;
		rtDestRect[RAT_RIGHT_TOP].bottom = helperRect0.y;
		rtUV[RAT_RIGHT_TOP].left = helperAddress1.x;
		rtUV[RAT_RIGHT_TOP].bottom = helperAddress0.y;

		// lm
		rtDestRect[RAT_LEFT_MIDDLE].top = helperRect0.y;
		rtDestRect[RAT_LEFT_MIDDLE].right = helperRect0.x;
		rtDestRect[RAT_LEFT_MIDDLE].bottom = helperRect1.y;
		rtUV[RAT_LEFT_MIDDLE].top = helperAddress0.y;
		rtUV[RAT_LEFT_MIDDLE].right = helperAddress0.x;
		rtUV[RAT_LEFT_MIDDLE].bottom = helperAddress1.y;

		// mm
		rtDestRect[RAT_MIDDLE_MIDDLE].left = helperRect0.x;
		rtDestRect[RAT_MIDDLE_MIDDLE].top = helperRect0.y;
		rtDestRect[RAT_MIDDLE_MIDDLE].right = helperRect1.x;
		rtDestRect[RAT_MIDDLE_MIDDLE].bottom = helperRect1.y;
		rtUV[RAT_MIDDLE_MIDDLE].left = helperAddress0.x;
		rtUV[RAT_MIDDLE_MIDDLE].top = helperAddress0.y;
		rtUV[RAT_MIDDLE_MIDDLE].right = helperAddress1.x;
		rtUV[RAT_MIDDLE_MIDDLE].bottom = helperAddress1.y;

		// rm
		rtDestRect[RAT_RIGHT_MIDDLE].left = helperRect1.x;
		rtDestRect[RAT_RIGHT_MIDDLE].top = helperRect0.y;
		rtDestRect[RAT_RIGHT_MIDDLE].bottom = helperRect1.y;
		rtUV[RAT_RIGHT_MIDDLE].left = helperAddress1.x;
		rtUV[RAT_RIGHT_MIDDLE].top = helperAddress0.y;
		rtUV[RAT_RIGHT_MIDDLE].bottom = helperAddress1.y;

		// lb
		rtDestRect[RAT_LEFT_BOTTOM].top = helperRect1.y;
		rtDestRect[RAT_LEFT_BOTTOM].right = helperRect0.x;
		rtUV[RAT_LEFT_BOTTOM].top = helperAddress1.y;
		rtUV[RAT_LEFT_BOTTOM].right = helperAddress0.x;

		// mb
		rtDestRect[RAT_MIDDLE_BOTTOM].left = helperRect0.x;
		rtDestRect[RAT_MIDDLE_BOTTOM].top = helperRect1.y;
		rtDestRect[RAT_MIDDLE_BOTTOM].right = helperRect1.x;
		rtUV[RAT_MIDDLE_BOTTOM].left = helperAddress0.x;
		rtUV[RAT_MIDDLE_BOTTOM].top = helperAddress1.y;
		rtUV[RAT_MIDDLE_BOTTOM].right = helperAddress1.x;

		// rb
		rtDestRect[RAT_RIGHT_BOTTOM].left = helperRect1.x;
		rtDestRect[RAT_RIGHT_BOTTOM].top = helperRect1.y;
		rtUV[RAT_RIGHT_BOTTOM].left = helperAddress1.x;
		rtUV[RAT_RIGHT_BOTTOM].top = helperAddress1.y;
		
		float halfWidth = rtArea.getWidth() * 0.5f;
		float halfHeight = rtArea.getHeight() * 0.5f;
		Vector3 center = Vector3(rtArea.left + halfWidth, rtArea.top + halfHeight, 0.0f);

		// bool insertVertex = vertex.empty();

		for (int i = 0; i < RAT_MAX_TYPE; ++i)
		{
			// 如果有裁剪
			if (rtClip)
			{
				ERect oldRect = rtDestRect[i];
				ERect oldUV = rtUV[i];
				rtDestRect[i] = rtDestRect[i].intersect(*rtClip);
				if (rtDestRect[i].getHeight() == 0 || rtDestRect[i].getWidth() == 0)
				{
					continue;
				}

				float fInverseWidth = rtUV[i].getWidth() / oldRect.getWidth();
				float fInverseHeight = rtUV[i].getHeight() / oldRect.getHeight();

				rtUV[i].left = oldUV.left + (rtDestRect[i].left - oldRect.left) * fInverseWidth;
				rtUV[i].top = oldUV.top + (rtDestRect[i].top - oldRect.top) * fInverseHeight;
				rtUV[i].right = oldUV.left + (rtDestRect[i].right - oldRect.left) * fInverseWidth;
				rtUV[i].bottom = oldUV.top + (rtDestRect[i].bottom - oldRect.top) * fInverseHeight;
			}

			Vector2 newOffset;
			newOffset.x = PixelAligned(m_offset.x);
			newOffset.y = PixelAligned(m_offset.y);
			rtDestRect[i].offset(newOffset);

			Color cTemp;
			cTemp.setABGR(color);

			Vector3 lt, rb, lb, rt;
			// 计算旋转
			CaculateRotate(rtDestRect[i], center, Vector3(fRotateX, fRotateY, fRotate), lt, rb, lb, rt);
// 
// 			if (insertVertex)
// 			{
// 				vertex.push_back(lt);
// 				vertex.push_back(rb);
// 				vertex.push_back(lb);
// 				vertex.push_back(rt);
// 				m_pOwner->DrawImage(lt*actionMatrix, rb*actionMatrix, lb*actionMatrix, rt*actionMatrix, rtUV[i], ColorRect(cTemp, cTemp, cTemp, cTemp));
// 			} 
// 			else
// 			{
// 				if (vertex.size() == 4 * RAT_MAX_TYPE)
// 				{
// 					int offset = i * 4;
// 					m_pOwner->DrawImage(vertex[offset] * actionMatrix, vertex[offset + 1] * actionMatrix, vertex[offset + 2] * actionMatrix, vertex[offset + 3] * actionMatrix, rtUV[i], ColorRect(cTemp, cTemp, cTemp, cTemp));
// 				}
// 			}

			ImageBuffer temp;
			temp.lt = lt;
			temp.rb = rb;
			temp.lb = lb;
			temp.rt = rt;
			temp.uv = rtUV[i];
			temp.color = ColorRect(cTemp, cTemp, cTemp, cTemp);
			temp.imageset = m_pOwner;
			vertex.push_back(temp);
		}		
	}	

	void GUIImage::CaculateRotate( const ERect& rtArea, const Vector3& center, const Vector3& angle
									, Vector3& lt, Vector3& rb, Vector3& lb, Vector3& rt)
	{
		lt = Vector3(rtArea.left, rtArea.top, 0.0f);
		rb = Vector3(rtArea.right, rtArea.bottom, 0.0f);
		lb = Vector3(rtArea.left, rtArea.bottom, 0.0f);
		rt = Vector3(rtArea.right, rtArea.top, 0.0f);

		// 计算旋转
		if (angle.x != 0 || angle.y != 0 || angle.z != 0)
		{
			lt -= center;
			rb -= center;
			lb -= center;
			rt -= center;

			Quaternion qz(Vector3::UNIT_Z, Math::DEG2RAD*angle.z);
			Quaternion qy(Vector3::UNIT_Y, Math::DEG2RAD*angle.y);
			Quaternion qx(Vector3::UNIT_X, Math::DEG2RAD*angle.x);

			Quaternion q = qx * qy * qz;

			lt = q * lt;
			rb = q * rb;
			lb = q * lb;
			rt = q * rt;

			lt += center;
			rb += center;
			lb += center;
			rt += center;
		}
	}

	void GUIImage::DrawFontImage(utf16 code, const ERect& rtArea, const ERect* rtClip, const ColorRect& color, FontBufferListType* fontBuffer, const Vector2& vScale /*= Vector2::ONE*/) const
	{
		ERect rtDest = rtArea;
		rtDest.top = PixelAligned(rtArea.top);
		rtDest.left = PixelAligned(rtArea.left);
		rtDest.right = PixelAligned(rtDest.left + rtArea.getWidth() * vScale.x);
		rtDest.bottom = PixelAligned(rtDest.top + rtArea.getHeight() * vScale.y);

		Vector2 newOffset;
		newOffset.x = PixelAligned(m_offset.x * vScale.x);
		newOffset.y = PixelAligned(m_offset.y * vScale.y);
		rtDest.offset(newOffset);

		ERect afterClipDest = rtDest;

		ERect rtTexUV = m_rtArea;
		// 如果有裁剪
		if (rtClip)
		{
			afterClipDest = rtDest.intersect(*rtClip);
			if (afterClipDest.getHeight() == 0 || afterClipDest.getWidth() == 0)
			{
				return;
			}

			float fInverseWidth = m_rtArea.getWidth() / rtDest.getWidth();
			float fInverseHeight = m_rtArea.getHeight() / rtDest.getHeight();

			rtTexUV.left = m_rtArea.left + (afterClipDest.left - rtDest.left) * fInverseWidth;
			rtTexUV.top = m_rtArea.top + (afterClipDest.top - rtDest.top) * fInverseHeight;
			rtTexUV.right = m_rtArea.left + (afterClipDest.right - rtDest.left) * fInverseWidth;
			rtTexUV.bottom = m_rtArea.top + (afterClipDest.bottom - rtDest.top) * fInverseHeight;
		}

		float halfWidth = rtDest.getWidth() * 0.5f;
		float halfHeight = rtDest.getHeight() * 0.5f;
		Vector3 center = Vector3(rtDest.left + halfWidth, rtDest.top + halfHeight, 0.0f);

		Vector3 lt, rb, lb, rt;
		// 计算旋转
		CaculateRotate(afterClipDest, center, Vector3(0, 0, 0), lt, rb, lb, rt);

		// 如果传入了buffer，就是先收集起来
		// 然后最后再渲染, 目前只有rendertext会传入buffer
		if (fontBuffer)
		{
			FontBuffer temp;
			temp.lt = lt;
			temp.rb = rb;
			temp.lb = lb;
			temp.rt = rt;
			temp.uv = rtTexUV;
			temp.color = color;
			temp.code = code;
			temp.imageset = m_pOwner;
			fontBuffer->push_back(temp);
		} 
		else
		{
			if (m_pOwner->IsFreeTypeFontBuffer())
			{
				m_pOwner->DrawImage(lt, rb, lb, rt, rtTexUV, color, GUIMaterialType_FONT);
			} 
			else
			{
				m_pOwner->DrawImage(lt, rb, lb, rt, rtTexUV, color, GUIMaterialType_CULL_BACK);
			}
		}

	}

}
