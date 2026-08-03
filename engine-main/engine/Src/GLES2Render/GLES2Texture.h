/********************************************************************
filename: 	GLES2Texture.h
file path:	dev\engine\Src\GLES2Render\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_GLES2TEXTURE_H__
#define __LORD_GLES2TEXTURE_H__

#include "Render/Texture.h"

namespace LORD
{

	class LORD_GLES2RENDER_API GLES2Texture: public Texture
	{
		friend class GLES2RenderTarget;
	public:
		GLES2Texture(const String& name, ResourceType rt);
		GLES2Texture(TexType texType, PixelFormat pixFmt, Dword usage, uint width, uint height, uint depth,
						uint numMipmaps, const Buffer& buff, bool bBak = true);
		virtual ~GLES2Texture();

	public:
		virtual	bool		updateSubTex2D(uint level, const ERect& rect, void* pData);

		GLuint				getTextureHandle() const;
		void				setTextureHandle( GLuint hTex );
	
		/*void				setEtcSeperateAlpha( bool _alpha )
		{
		m_bEtcSeperateAlpha = _alpha;
		}*/

		/*virtual bool		getEtcSeperateAlpha()
		{
		return m_bEtcSeperateAlpha;
		}*/

		/*GLuint				getAlphaTexture() const
		{
			return m_hAlphaTexture;
		}*/

		virtual unsigned int	getHandle() { return m_hTexture; }

	protected:
		virtual bool		create2D(PixelFormat pixFmt, Dword usage, uint width, uint height, uint numMipmaps, const Buffer& buff);

		// for resource management
		virtual bool		prepareImpl();

		virtual bool		loadImpl();

		virtual void		unloadImpl();

		bool				_data_parser();

		bool				_parser_dds();
		bool				_parser_pvr();
		bool				_parser_common();
		bool				_parser_ktx();

		bool				_upload_dds();
		bool				_upload_pvr();
		bool				_upload_common();
		bool				_upload_ktx();
		bool				_upload_etc_aa_ktx();
		bool				_parser_etc_aa_ktx();
	protected:
		GLuint				m_hTexture;
		//GLuint				m_hAlphaTexture;				
		//bool				m_bEtcSeperateAlpha;
		ui32				m_uiRgbSize;
		//ui32				m_uiAlphaSize;
	};

}

#endif
