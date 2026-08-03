/********************************************************************
filename: 	GUIImageset.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef __GUI_IMAGESET_H__
#define __GUI_IMAGESET_H__

#include "Core.h"
#include "Resource/Resource.h"
#include "GUIImage.h"
#include "Resource/LordThread.h"

namespace LORD
{
// 	class ImagesetPrepareEvent : public ThreadEvent
// 	{
// 	public:
// 		ImagesetPrepareEvent(GUIImageset* imageset);
// 		virtual ~ImagesetPrepareEvent();
// 
// 		virtual bool ProcessEvent();
// 
// 		virtual bool RespondEvent();
// 
// 		virtual bool GetRepeatRespondIfFail()	{ return true; }
// 		virtual bool IsAutoDestroy()			{ return true; }
// 
// 	protected:
// 		GUIImageset*	m_imageset;
// 	};

	class LORD_CORE_API GUIImageset: public Resource
	{
	public:
		struct ImageSetUnit
		{
			String name;
			Vector2i pos;
			Vector2i size;
		};

		typedef vector<ImageSetUnit>::type ImageSetUnitArr;

	public:

		friend class GUIImagesetManager;
		friend class GUIImage;
		//friend class ImagesetPrepareEvent;

		typedef	map<GUIString, GUIImage*, GUIString::FastLessCompare>::type	ImageRegistry;

		GUIImageset(const String& strName, ResourceType rt);

		~GUIImageset(void);

		GUIImage* DefineImage(const GUIString& strName, const ERect& rtArea, const Vector2& offset = Vector2::ZERO);

		bool IsImageDefined(const GUIString& strName) const	{return m_Images.find(strName) != m_Images.end();}

		void UndefineImage(const GUIString& strName);

		void UndefineAllImage();

		Texture*	GetTexture() const { return m_pTexture; }

		// for test.
		void ReplaceTexture(Texture* tex);

		bool		IsFreeTypeFontBuffer() { return m_bIsFontBuffer; }

		void		SetFontBuffer(bool bIsFontBuffer) { m_bIsFontBuffer = bIsFontBuffer;}

		const GUIString&	GetName() { return m_strName; }

		GUIImage*	GetImage(const GUIString& strName) const;

		const ImageRegistry& GetImages() { return m_Images; }

// 		void		DrawImage(const ERect& rtDest, const ERect& rtTexAddress, const ColorRect& color, 
// 							float fRotate = 0.0f, float fRotateY = 0.0f, float fRotateX = 0.0f);

		void		DrawImage(const Vector3& lt, const Vector3& rb, const Vector3& lb, const Vector3& rt, 
								const ERect& rtTexAddress, const ColorRect& color, GUIMaterialType material, Texture* maskTexture = NULL);
		
		void		setPreparing(bool preparing) { m_preparing = preparing; }

		bool		isPreparing() { return m_preparing;  }

		bool		loadOutSide(Texture* texture, const ImageSetUnitArr& defines);

	protected:
		virtual size_t			calculateSize() const;
		virtual bool			prepareImpl();
		virtual void			unprepareImpl();
		virtual bool			loadImpl();
		virtual void			unloadImpl();

	protected:
		GUIString		m_strName;
		ImageRegistry	m_Images;
		Texture*		m_pTexture;
		GUIString       m_strTextureFileName;

		float			m_fNativeHorzRes;
		float			m_fNativeVertRes;
		float			m_fInverseNativeHorzRes;
		float			m_fInverseNativeVertRes;
		bool			m_bIsFontBuffer;
		// 正在多线程请求中
		bool			m_preparing;
	};
}


#endif
