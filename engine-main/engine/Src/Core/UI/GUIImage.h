/********************************************************************
filename: 	GUIImage.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef __GUI_IMAGE_H__
#define __GUI_IMAGE_H__

#include "Core.h"
#include "GUIString.h"
#include "GUICommonDef.h"

namespace LORD
{
	class GUIImageset;

	// font buffer struct
	struct FontBuffer
	{
		// 4个顶点的坐标
		Vector3 lt;
		Vector3 rb;
		Vector3 lb;
		Vector3 rt;
		ERect	uv;
		ColorRect	color;
		utf16	code; // unicode
		GUIImageset*	imageset;

		FontBuffer()
			: lt(Vector3::ZERO)
			, rb(Vector3::ZERO)
			, lb(Vector3::ZERO)
			, rt(Vector3::ZERO)
			, code(0)
			, imageset(NULL)
		{}

	};

	// image buffer struct
	struct ImageBuffer
	{
		// 4个顶点的坐标
		Vector3 lt;
		Vector3 rb;
		Vector3 lb;
		Vector3 rt;
		ERect	uv;
		ColorRect	color;
		GUIImageset*	imageset;

		ImageBuffer()
			: lt(Vector3::ZERO)
			, rb(Vector3::ZERO)
			, lb(Vector3::ZERO)
			, rt(Vector3::ZERO)
			, imageset(NULL)
		{}

	};

	typedef vector<FontBuffer>::type FontBufferListType;
	typedef vector<ImageBuffer>::type ImageBufferListType;

	class LORD_CORE_API GUIImage: public ObjectAlloc
	{
	public:
		enum RenderAreaType
		{
			RAT_LEFT_TOP,
			RAT_MIDDLE_TOP,
			RAT_RIGHT_TOP,
			RAT_LEFT_MIDDLE,
			RAT_MIDDLE_MIDDLE,
			RAT_RIGHT_MIDDLE,
			RAT_LEFT_BOTTOM,
			RAT_MIDDLE_BOTTOM,
			RAT_RIGHT_BOTTOM,
			RAT_MAX_TYPE,
		};

		GUIImage(GUIImageset* owner, const GUIString& name, const ERect& area, 
				const ERect& rtPixelRect, const Vector2& offset = Vector2::ZERO);
		GUIImage(const GUIImage& image);
		~GUIImage(void);

		const GUIImageset*	GetImageset() const { return m_pOwner; }

		float GetWidth() const { return m_rtPixelRect.getWidth(); }

		float GetHeight() const { return m_rtPixelRect.getHeight(); }

		void DrawImage(ImageBufferListType& vertex, const ERect& rtArea, const ColorRect& color, float fRotate,
					float fRotateY, float fRotateX, const ERect* rtClip = NULL, const Vector2& vScale = Vector2::ONE) const;
		
		//height(range is 0.0~1.0)
		void DrawHexagon(ImageBufferListType& vertex, GUIImage* pImageSide, GUIImage* pImageOverlayer, const ERect rtArea, const Color& topColor,
			const Color& sideColor, const Color& overlayerColor, float height, const ERect* rtClip = NULL, const Vector2& vScale = Vector2::ONE) const;

		void DrawFontImage(utf16 code, const ERect& rtArea, const ERect* rtClip, const ColorRect& color, FontBufferListType* fontBuffer, const Vector2& vScale = Vector2::ONE) const;

		void DrawStretchImage(ImageBufferListType& vertex, const ERect& rtArea, const unsigned int color, float fRotate, float fRotateY,
						float fRotateX, const ERect* rtClip, const StretchOffset& offset) const;

		void DrawStretchImageTB(ImageBufferListType& vertex, const ERect& rtArea, const unsigned int color, float fRotate,
							float fRotateY, float fRotateX, const ERect* rtClip, const StretchOffset& offset) const;

		void DrawStretchImageLR(ImageBufferListType& vertex, const ERect& rtArea, const unsigned int color, float fRotate,
							float fRotateY, float fRotateX, const ERect* rtClip, const StretchOffset& offset) const;

		void UpdateImageData(GUIVertexDeclaration*& vertexData, const ERect& rtArea, const ColorRect& color, float fRotate, float fRotateY, float fRotateX,
			const ERect* rtClip = NULL, const Vector2& vScale = Vector2::ONE);

		void UpdateStretchImageData(GUIVertexDeclaration*& vertexData, const ERect& rtArea, const unsigned int color, float fRotate, float fRotateY,
			float fRotateX, const ERect* rtClip, const StretchOffset& offset) const;

		void UpdateStretchImageTBData(GUIVertexDeclaration*& vertexData, const ERect& rtArea, const unsigned int color, float fRotate,
			float fRotateY, float fRotateX, const ERect* rtClip, const StretchOffset& offset) const;

		void UpdateStretchImageLRData(GUIVertexDeclaration*& vertexData, const ERect& rtArea, const unsigned int color, float fRotate,
			float fRotateY, float fRotateX, const ERect* rtClip, const StretchOffset& offset) const;

		const GUIString& GetName() { return m_strName;}

		const Vector2& GetOffset() const { return m_offset; }

		const ERect&		GetUV() const { return m_rtArea; }

		void SetUV(ERect uv)  { m_rtArea = uv; }

		static void	CaculateRotate(const ERect& rtArea, const Vector3& center, const Vector3& angle
			, Vector3& lt, Vector3& rb, Vector3& lb, Vector3& rt);

	private:

		friend class GUIImageset;
		GUIImageset*		m_pOwner;
		GUIString			m_strName;
		ERect				m_rtArea;
		ERect				m_rtPixelRect;
		Vector2				m_offset;
	};
}


#endif
