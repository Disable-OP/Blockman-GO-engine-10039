/********************************************************************
filename: 	Texture.h
file path:	dev\engine\Src\Core\Render\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_TEXTURE_H__
#define __LORD_TEXTURE_H__

#include "Core.h"
#include "Resource/Resource.h"
#include "Object/Buffer.h"

namespace LORD
{

	class LORD_CORE_API Texture: public Resource
	{
		friend struct commonTextureUploadFunc;
		friend class RenderTarget;
	public:
		enum TexType
		{
			TT_1D,
			TT_2D,
			TT_3D,
			TT_CUBE,
		};

		enum TexUsage
		{
			TU_CPU_READ			= 0x00000001,
			TU_CPU_WRITE		= 0x00000002,
			// texture is used as shader resource
			TU_GPU_READ			= 0x00000004,
			// texture is used as depth or render target (depend on pixel format)
			TU_GPU_WRITE		= 0x00000008,
			TU_GPU_UNORDERED	= 0x00000010,

			TU_GPU_MIPMAP		= 0x00000020,

			// default to automatic mipmap generation static textures
			TU_DEFAULT			= TU_GPU_READ | TU_GPU_WRITE | TU_GPU_MIPMAP,
			TU_STATIC			= TU_GPU_READ | TU_GPU_MIPMAP,
			TU_DYNAMIC			= TU_GPU_READ | TU_CPU_WRITE | TU_GPU_MIPMAP,
		};

		enum CompressType
		{
			CompressType_ETC1,
			CompressType_ETCA,
			CompressType_PVR,
			CompressType_S3TC,
			CompressType_ATITC,
			CompressType_KTX,
			CompressType_Unknown
		};

		static const uint	MAX_MINMAPS = 16;

		typedef vector<Buffer>::type		TexArray;

	public:
		Texture(const String& name, ResourceType rt = RESOURCE_TYPE_NORMAL);
		Texture(TexType texType, PixelFormat pixFmt, Dword usage, uint width, uint height, uint depth, 
			uint numMipmaps, const Buffer& buff, bool bBak = true);
		virtual ~Texture();

		friend class TextureManager;
		friend struct TexturePrepareFunc;

	public:
		inline TexType getType() const { return m_texType; }
		inline PixelFormat getPixelFormat() const { return m_pixFmt; }
		inline uint getWidth() const { return m_width; }
		inline uint getHeight() const { return m_height; }
		inline uint getDepth() const { return m_depth; }
		inline uint getNumMipmaps() const { return m_numMipmaps; }

		void setUserData(int data) { m_userData = data; }
		int getUserData() const { return m_userData; }

		virtual bool			updateSubTex2D(uint level, const ERect& rect, void* pData) = 0;

		/// for editor.
		virtual bool			getData(Byte* data);

		virtual void			setSamplerState( class SamplerState* pSs )
		{
			if (!pSs) return;

			if(m_pSamplerState!=pSs )
            {
                m_pSamplerState = pSs;
                pSs->bind2Texture(this);
            }
		}
                
		class SamplerState*		getSamplerState() const
		{
			return m_pSamplerState;
		}
		virtual bool			reCreate2D(PixelFormat pixFmt, Dword usage, uint width, uint height, uint numMipmaps, const Buffer& buff);

		virtual unsigned int	getHandle() = 0;
        
	protected:
		virtual bool			create2D(PixelFormat pixFmt, Dword usage, uint width, uint height, uint numMipmaps, const Buffer& buff);

		virtual size_t			calculateSize() const;
		virtual bool			prepareImpl();
		virtual void			unprepareImpl();
		virtual bool			loadImpl();
		virtual void			unloadImpl();

		bool					_data_io(); // internal function
		bool					_data_io_outside();


	protected:
		TexType							m_texType;
		PixelFormat						m_pixFmt;
		Dword							m_usage;
		uint							m_width;
		uint							m_height;
		uint							m_depth;
		uint							m_numMipmaps;
		uint							m_size;
		Byte*							m_pPreparedData;
		bool							m_bCompressed;
		ui32							m_faceNum;

		CompressType					m_compressType;

		int								m_userData;
		ui32							m_internalFormat;
		ui32							m_endian;
		ui32							m_bitsPerPixel;
		ui32							m_blockSize;
		ui32							m_xDimension;
		ui32							m_yDimension;
		ui32							m_zDimension;
		ui32							m_headerSize;
		ui32							m_uploadedSize;  // its actually uploaded size. uploadSize + headerSize + identifier size = m_size
		ui32							m_surfaceNum;
		class SamplerState*				m_pSamplerState;
	};
}

#endif
