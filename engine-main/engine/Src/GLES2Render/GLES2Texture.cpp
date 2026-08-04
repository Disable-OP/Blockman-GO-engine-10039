#include "GLES2RenderStd.h"
#include "GLES2Renderer.h"
#include "GLES2Texture.h"
#include "GLES2Mapping.h"
#include "Image/PixelFormat.h"
#include "Geom/Rect.h"
#include "Image/Image.h"
#include "Object/Root.h"
#include "GLES2TextureLoaderHelper.h"

namespace LORD
{
	GLES2Texture::GLES2Texture(const String& name, ResourceType rt)
		: Texture(name, rt)
		, m_hTexture(0)
		//, m_bEtcSeperateAlpha(0)
		//, m_hAlphaTexture(0)
	{
	}
	
	GLES2Texture::GLES2Texture(TexType texType, PixelFormat pixFmt, Dword usage, uint width, uint height, uint depth,
							   uint numMipmaps, const Buffer &buff, bool bBak)
		: Texture(texType, pixFmt, usage, width, height, depth, numMipmaps, buff)
		, m_hTexture(0)
		//, m_bEtcSeperateAlpha(0)
		//, m_hAlphaTexture(0)
	{
		create2D(pixFmt, usage, width, height, numMipmaps, buff);
	}
	
	GLES2Texture::~GLES2Texture()
	{
		if(m_hTexture)
			glDeleteTextures(1, &m_hTexture);
	}
	
	bool GLES2Texture::updateSubTex2D(uint level, const ERect& rect, void* pData)
	{
		if(level >= m_numMipmaps || !pData)
			return false;
		
		glBindTexture(GL_TEXTURE_2D, m_hTexture);

		GLenum glFmt = GLES2Mapping::MapFormat(m_pixFmt);
		GLenum glType = GLES2Mapping::MapDataType(m_pixFmt);
		glTexSubImage2D(GL_TEXTURE_2D, level, (GLint)rect.left, (GLint)rect.top, (GLsizei)rect.getWidth(), (GLsizei)rect.getHeight(), glFmt, glType, pData);
		
		if(level == 0 && (m_usage & Texture::TU_GPU_MIPMAP))
			glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);

		return true;
	}

	GLuint GLES2Texture::getTextureHandle() const
	{
		return m_hTexture;
	}

	void GLES2Texture::setTextureHandle( GLuint htex )
	{
		m_hTexture = htex;
	}

	bool GLES2Texture::create2D(PixelFormat pixFmt, Dword usage, uint width, uint height, uint numMipmaps, const Buffer& buff)
	{
		glGenTextures(1, &m_hTexture);
		if(!m_hTexture)
		{
			LordLogError("Create GLES2Texture [%s] failed.", PixelUtil::GetPixelFormatName(pixFmt).c_str());
			return false;
		}

		// generate gpu texture
		glBindTexture(GL_TEXTURE_2D, m_hTexture);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		
		// set default sampler state
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		uint texWidth = width;
		uint texHeight = height;
		Byte* pData = buff.getData();

		bool bHasData = (pData != NULL? true : false);

		PixelFormat srcPixFmt = pixFmt;
		PixelFormat dstPixFmt = pixFmt;
		switch (pixFmt)
		{
		case PF_R8_UNORM:
			{
				srcPixFmt = PF_R8_UNORM;
				dstPixFmt = PF_RGBA8_UNORM;
			} break;
		case PF_RGBA8_UNORM:
			{
				srcPixFmt = PF_BGRA8_UNORM;
				dstPixFmt = PF_RGBA8_UNORM;
			} break;
		case PF_RGB8_UNORM:
			{
				srcPixFmt = PF_BGR8_UNORM;
				dstPixFmt = PF_RGB8_UNORM;
			} break;
		default: break;
		}

		GLenum internalFmt = GLES2Mapping::MapInternalFormat(dstPixFmt);
		GLenum glFmt = GLES2Mapping::MapFormat(dstPixFmt);
		GLenum glType = GLES2Mapping::MapDataType(dstPixFmt);

		bool bRequiredConvert = (srcPixFmt == dstPixFmt ? false : true);

		for(uint level = 0; level < m_numMipmaps; ++level)
		{
			Byte* curMipData = 0;

			if(bHasData)
			{
				if(bRequiredConvert)
				{
					PixelBox srcBox(texWidth, texHeight, 1, srcPixFmt, pData);
					PixelBox dstBox(texWidth, texHeight, 1, dstPixFmt);
					dstBox.pData = LORD_ALLOC_T(Byte, dstBox.getConsecutiveSize());

					PixelUtil::BulkPixelConversion(srcBox, dstBox);
					curMipData = (Byte*)dstBox.pData;
					glTexImage2D(GL_TEXTURE_2D, level, internalFmt, texWidth, texHeight, 0, glFmt, glType, curMipData);
					
					LORD_FREE(curMipData);
				}
				else
				{
					curMipData = pData;
					glTexImage2D(GL_TEXTURE_2D, level, internalFmt, texWidth, texHeight, 0, glFmt, glType, curMipData);
				}
			}
			else
			{
				glTexImage2D(GL_TEXTURE_2D, level, internalFmt, texWidth, texHeight, 0, glFmt, glType, curMipData);
			}

			ui32 curMipSize  = PixelUtil::CalcLevelSize(width, height, 1, level, pixFmt);
			//pData += PixelUtil::CalcLevelSize(width, height, 1, level, pixFmt);
			pData += curMipSize;
			m_uploadedSize += curMipSize;

			texWidth /= 2;
			texHeight /= 2;
		}
		
		// generate GPU mipmap
		if (m_numMipmaps < 2)
		{
			if (usage & TU_GPU_MIPMAP)
				glGenerateMipmap(GL_TEXTURE_2D);
		}

		glBindTexture(GL_TEXTURE_2D, 0);
		
		return true;
	}
	
	void GLES2Texture::unloadImpl()
	{
		if(m_hTexture)
			glDeleteTextures(1, &m_hTexture);

		/*if( getEtcSeperateAlpha() )
		{
			glDeleteTextures( 1, &m_hAlphaTexture );
		}*/
		
		if( Root::Instance()->getEnableFrameProfile() )
		{
			Root::Instance()->frameState().decrUploadTextureSizeInBytes( m_uploadedSize );
		}
	}  
	
	bool GLES2Texture::prepareImpl()
	{
		// 1) load into system memory
		if (mResourceType == RESOURCE_TYPE_NORMAL)
		{
			if (!_data_io())
			{
				LordLogError("GLES2Textrue[%s] _data_io Failed !", getName().c_str());
				return false;
			}
		}
		else if (mResourceType == RESOURCE_TYPE_OUTSIDE)
		{
			if (!_data_io_outside())
			{
				LordLogError("Texture[%s] load Into System Memory Failed !", getName().c_str());

				return false;
			}
		}

		//2) parser System memory and fill texture header info
		return _data_parser();
	}

	bool GLES2Texture::loadImpl()
	{ 
		bool no_error = false;
		switch(m_compressType)
		{
		case Texture::CompressType_PVR:
		case Texture::CompressType_ETC1:
			no_error = _upload_pvr();
			break;

		case Texture::CompressType_KTX:
			no_error = _upload_ktx();
			break;

		case Texture::CompressType_ETCA:
			no_error = _upload_etc_aa_ktx();
			break;;

		case Texture::CompressType_ATITC:
		case Texture::CompressType_S3TC:
			no_error = _upload_dds();
			break;

		case Texture::CompressType_Unknown:
			no_error = _upload_common();
			break;
		}

		if( no_error && Root::Instance()->getEnableFrameProfile())
		{
			Root::Instance()->frameState().incrUploadTextureSizeInBytes( m_uploadedSize );
		}

		return no_error;
	}


	bool GLES2Texture::_data_parser()
	{
		LordAssert( m_pPreparedData );

		ui32* pIdentifier = (ui32 *)m_pPreparedData;

		if( *pIdentifier == c_pvrtex3_ident ) // its pvr format texture
		{
			return _parser_pvr();
		}

		else if( *pIdentifier == FourCC<'E', 'T', 'C', 'A'>::value )
		{
			return _parser_etc_aa_ktx();
		}

		else if( *pIdentifier ==  FourCC<'D', 'D', 'S', ' '>::value ) // its dds format texture 
		{
			return _parser_dds();
		}

		else if( !memcmp(m_pPreparedData, cs_etc1_identifier, sizeof(ui8)*12 ) )
		{
			return _parser_ktx();
		}

		else   // its png ,tga or jpg
		{
			return _parser_common();
		}

		return false;
	}

	bool GLES2Texture::_parser_pvr()
	{

		// its pvrtc or etc1 texture

		m_bCompressed = true;

		m_compressType = Texture::CompressType_PVR;

		ui8* pTextureData = const_cast<ui8*>(m_pPreparedData);

		// fill info data;
		PVRTextureHeaderV3* pHeader = (PVRTextureHeaderV3*)pTextureData;

		//		tli.m_pPixelData = pTextureData + PVRTEX3_HEADERSIZE;

		m_width = pHeader->u32Width;

		m_height = pHeader->u32Height;

		m_depth = pHeader->u32Depth;

		m_numMipmaps = pHeader->u32MIPMapCount;

		m_faceNum = pHeader->u32NumSurfaces;

		m_internalFormat = pvrInternalformatMapping( pHeader->u64PixelFormat );

		m_bitsPerPixel = pvrBitsPerPixel(pHeader->u64PixelFormat);			

		pvrPixelDimension( pHeader->u64PixelFormat, m_xDimension, m_yDimension, m_zDimension );

		return true;
	}

	bool GLES2Texture::_parser_dds()
	{
		ui8* pTextureData = m_pPreparedData;

		LordAssert( pTextureData );

		m_bCompressed = true;

		DDSHeader* pDDSHeader = (DDSHeader *)(pTextureData + 4);
		m_headerSize = sizeof(DDSHeader);
		m_width = pDDSHeader->dwWidth;
		m_height = pDDSHeader->dwHeight;
		m_numMipmaps = pDDSHeader->dwMipMapCount;

		//		tli.m_pPixelData = pTextureData + sizeof(DDSHeader) + sizeof(ui32);

		switch( pDDSHeader->ddspf.dwFourCC )
		{
		case FourCC<'A', 'T', 'C', ' '>::value:
			m_internalFormat = GL_ATC_RGB_AMD;
			m_blockSize = 8;
			m_compressType = Texture::CompressType_ATITC;

			return true;

		case FourCC<'A', 'T', 'C', 'A'>::value:
			m_internalFormat = GL_ATC_RGBA_EXPLICIT_ALPHA_AMD;  
			m_blockSize = 16;
			m_compressType = Texture::CompressType_ATITC;
			return true;

		case FourCC<'A', 'T', 'C', 'I'>::value:
			m_internalFormat = GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD; //
			m_blockSize = 16;
			m_compressType = Texture::CompressType_ATITC;
			return true;

		case FourCC<'D', 'X', 'T', '1'>::value:
			m_internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			m_blockSize = 8;
			m_compressType = Texture::CompressType_S3TC;
			return true;


		case FourCC<'D', 'X', 'T', '3'>::value:
			m_internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			m_blockSize = 16;
			m_compressType = Texture::CompressType_S3TC;
			return true;

		case FourCC<'D', 'X', 'T', '5'>::value:
			m_internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			m_blockSize = 16;
			m_compressType = Texture::CompressType_S3TC;
			return true;
		}

		return false;
	}

	bool GLES2Texture::_parser_common()
	{
		LordAssert( m_pPreparedData );

		Buffer commonTextureBuffer(m_size, m_pPreparedData);
		Image* pImage = Image::CreateFromMemory(commonTextureBuffer, Image::GetImageFormat(getName()));

		if(!pImage)
			return false;

		m_bCompressed = false;

		m_compressType = Texture::CompressType_Unknown;

		PixelFormat pixFmt = pImage->getPixelFormat();
#ifdef LORD_ENDIAN_LITTLE
		switch(pixFmt)
		{
		case PF_BGR8_UNORM:		pixFmt = PF_RGB8_UNORM;		break;
		case PF_BGRA8_UNORM:	pixFmt = PF_RGBA8_UNORM;	break;
		default: ;
		}
#endif
		m_width = pImage->getWidth();
		m_height = pImage->getHeight();
		m_depth = pImage->getDepth(); // 1
		m_pixFmt = pixFmt;
		m_numMipmaps = pImage->getNumMipmaps();
		if( m_numMipmaps == 0 )
			m_numMipmaps = 1;

		m_size = PixelUtil::CalcSurfaceSize(m_width, m_height, m_depth, m_numMipmaps, m_pixFmt);
		LordSafeFree(m_pPreparedData);
		m_pPreparedData = LordAlloc(Byte, m_size);
		memcpy(m_pPreparedData,pImage->getData(), m_size);

		LordDelete pImage;

		return true;
	}

	bool GLES2Texture::_parser_etc_aa_ktx()
	{
		LordAssert( m_pPreparedData );

		/*if( !Root::Instance()->deviceFeatures().getSeperateSampleAlpha() )
		{
		LordLogError( "Device dont use seperate sample alpha, but texture[%s] is seperate alpha!", getName().c_str() );
		return false;
		}*/

		m_uiRgbSize = *((ui32*)(m_pPreparedData + 4));
		//m_uiAlphaSize = m_uiRgbSize;

		ui8* pRgbData = m_pPreparedData + 8;

		//setEtcSeperateAlpha(true);

		m_bCompressed = true;

		m_compressType = Texture::CompressType_ETCA;

		KTXHeader* pKtxHeader = (KTXHeader*)pRgbData;


		m_width = pKtxHeader->m_pixelWidth;
		m_height = pKtxHeader->m_pixelHeight;
		m_depth = pKtxHeader->m_pixelDepth;
		m_internalFormat = pKtxHeader->m_internalFormat;
		m_numMipmaps = pKtxHeader->m_numberOfMipmapLevels;
		m_faceNum = pKtxHeader->m_numberOfFaces;
		m_headerSize = sizeof( KTXHeader );

		return true;

	}

	bool GLES2Texture::_upload_pvr()
	{

		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
		glGenTextures(1, &m_hTexture);
		glBindTexture( GL_TEXTURE_2D, m_hTexture );


		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		ui8* pPixelData = m_pPreparedData + c_pvrtex3_header_size; //tli.m_pPixelData;

		const ui32 c_ref = 1;

		for( ui32 curMip = 0; curMip < m_numMipmaps; curMip ++ )
		{
			ui32 curMipWidth = Math::Max( c_ref, m_width >> curMip );
			ui32 curMipHeight = Math::Max( c_ref,m_height >> curMip );
			ui32 curMipDepth = Math::Max( c_ref, m_depth >> curMip );


			ui32 tw = curMipWidth + ( ( -1 * curMipWidth ) % m_xDimension);
			ui32 th = curMipHeight + ( ( -1 * curMipHeight ) % m_yDimension );
			ui32 td = curMipDepth + ( ( -1 * curMipDepth ) % m_zDimension );


			ui32 curMipMapSize = ( m_bitsPerPixel * tw * th * td ) / 8; 


			if( Root::Instance()->startMipmap() != 0 && curMip < Root::Instance()->startMipmap() )
			{
				pPixelData += curMipMapSize;
				continue;
			}

			glCompressedTexImage2D(GL_TEXTURE_2D, Root::Instance()->startMipmap() == 0 ? curMip : curMip - Root::Instance()->startMipmap(), m_internalFormat, curMipWidth, curMipHeight, 0, curMipMapSize, pPixelData );

#if _DEBUG
#if (LORD_PLATFORM == LORD_PLATFORM_ANDROID || LORD_PLATFORM == LORD_PLATFORM_WINDOWS)
			
			GLenum no_error = eglGetError();
			
			if( no_error != GL_NO_ERROR && no_error != EGL_SUCCESS )
			{
				LordLogError( "glCompressedTexImage2D: pvr compressed Texture[%s] Failed in _upload_pvr() error code : %d", getName().c_str(), no_error );
				return false;
			}
#else
			GLenum no_error = glGetError();
			
			if( no_error != GL_NO_ERROR )
			{
				LordLogError( "glCompressedTexImage2D: pvr compressed Texture[%s] Failed in _upload_pvr() error code : %d", getName().c_str(), no_error );
				return false;
			}
#endif
#endif

			pPixelData += curMipMapSize;

			m_uploadedSize += curMipMapSize;

		}


		return true;
	}


	bool GLES2Texture::_upload_dds()
	{
		
		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
		glGenTextures(1, &m_hTexture);
		glBindTexture( GL_TEXTURE_2D, m_hTexture );
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		ui8* pPixelData = m_pPreparedData + sizeof(ui32) + m_headerSize;

		int block_x = ( m_width + 3 ) / 4;
		int block_y = (	m_height + 3 ) / 4;

		int curMipSize = block_x * block_y * m_blockSize ;
		int offset = 0;
		int curW = m_width, curH = m_height;

		for( ui32 level = 0; level < m_numMipmaps; level ++ )
		{

			m_uploadedSize += curMipSize;

			glCompressedTexImage2D( GL_TEXTURE_2D, level, m_internalFormat, curW, curH, 0, curMipSize, pPixelData + offset);

			offset += curMipSize;

#if _DEBUG
#if (LORD_PLATFORM == LORD_PLATFORM_ANDROID || LORD_PLATFORM == LORD_PLATFORM_WINDOWS)
			
			GLenum no_error = eglGetError();
			
			if( no_error != GL_NO_ERROR && no_error != EGL_SUCCESS )
			{
				LordLogError( "glCompressedTexImage2D: dds compressed Texture[%s] Failed in _upload_dds() error code : %d", getName().c_str(), no_error );
				return false;
			}
#else
			GLenum no_error = glGetError();
			
			if( no_error != GL_NO_ERROR )
			{
				LordLogError( "glCompressedTexImage2D: dds compressed Texture[%s] Failed in _upload_dds() error code : %d", getName().c_str(), no_error );
				return false;
			}
#endif
#endif

			if( curW > 1 ) curW /= 2;
			if( curH > 1 ) curH /= 2;

			block_x = ( curW + 3 ) / 4;
			block_y = ( curH + 3 ) / 4;
			
			curMipSize = block_x * block_y * m_blockSize;
		}

		return true;
	}

	bool GLES2Texture::_upload_common()
	{
		Buffer buff( m_size, m_pPreparedData);

		return create2D( m_pixFmt, m_usage, m_width, m_height, m_numMipmaps, buff );
	}


	bool GLES2Texture::_parser_ktx()
	{
		ui8* pTextureData = m_pPreparedData;

		KTXHeader* pKtxHeader = ( KTXHeader * )pTextureData;

		LordAssert( pKtxHeader->m_endianness == cs_big_endian );

		// for compressed texture, glType and glFormat must equal to 'zero'
		LordAssert( pKtxHeader->m_type == 0 && pKtxHeader->m_format == 0 );

		m_compressType = CompressType_KTX;
		m_bCompressed = true;

		m_width = pKtxHeader->m_pixelWidth;
		m_height = pKtxHeader->m_pixelHeight;
		m_depth = pKtxHeader->m_pixelDepth;
		m_internalFormat = pKtxHeader->m_internalFormat;
		m_numMipmaps = pKtxHeader->m_numberOfMipmapLevels;
		m_faceNum = pKtxHeader->m_numberOfFaces;
		m_headerSize = sizeof( KTXHeader );

		return true;
	}

	bool GLES2Texture::_upload_ktx()
	{
		ui8* pPixelData = m_pPreparedData + m_headerSize;

		LordAssert( pPixelData );

		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
		glGenTextures(1, &m_hTexture);
		glBindTexture( GL_TEXTURE_2D, m_hTexture );

		ui32 curWidth = 0;
		ui32 curHeight = 0;
		ui32 offset = 0;
		const ui32 c_ref = 1;
		for( ui32 cur_level = 0; cur_level < m_numMipmaps; cur_level ++ )
		{
			curWidth = Math::Max( c_ref, m_width >> cur_level );
			curHeight = Math::Max( c_ref, m_height >> cur_level );
			ui32* imageLodSize = (ui32*)(pPixelData + offset);
			offset += sizeof(ui32);

			glCompressedTexImage2D( GL_TEXTURE_2D, cur_level, m_internalFormat, curWidth, curHeight, 0, *imageLodSize, pPixelData + offset );

#if _DEBUG
#if (LORD_PLATFORM == LORD_PLATFORM_ANDROID || LORD_PLATFORM == LORD_PLATFORM_WINDOWS) 

			GLenum no_error = eglGetError();

			if( no_error != GL_NO_ERROR && no_error != EGL_SUCCESS )
			{
				LordLogError( "glCompressedTexImage2D: Ktx compressed Texture[%s] Failed in _upload_ktx() error code : %d", getName().c_str(), no_error );
				return false;
			}
#else
			GLenum no_error = glGetError();

			if( no_error != GL_NO_ERROR )
			{
				LordLogError( "glCompressedTexImage2D: Ktx compressed Texture[%s] Failed in _upload_ktx() error code : %d", getName().c_str(), no_error );
				return false;
			}
#endif
#endif

			offset += *imageLodSize;

		}

		return true;
	}

	bool GLES2Texture::_upload_etc_aa_ktx()
	{

// upload alpha part {{
//		ui8* pPixelData = m_pPreparedData + 4 + 4 + m_headerSize;
////		KTXHeader* pktxHeader = (KTXHeader *)(m_pPreparedData + 4 + 4);
//
//		LordAssert( pPixelData );
//
//		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
//		/*glGenTextures(1, &m_hAlphaTexture);
//		glBindTexture( GL_TEXTURE_2D, m_hAlphaTexture );*/
//
//		ui32 curWidth = 0;
//		ui32 curHeight = 0;
//		ui32 offset = 0;
//		const ui32 c_ref = 1;
//		for( ui32 cur_level = 0; cur_level < m_numMipmaps; cur_level ++ )
//		{
//			curWidth = Math::Max( c_ref, m_width >> cur_level );
//			curHeight = Math::Max( c_ref, m_height >> cur_level );
//			ui32* imageLodSize = (ui32*)(pPixelData + offset);
//			offset += sizeof(ui32);
//
//			glCompressedTexImage2D( GL_TEXTURE_2D, cur_level, m_internalFormat, curWidth, curHeight, 0, *imageLodSize, pPixelData + offset );
//
//#if _DEBUG
//#if (LORD_PLATFORM == LORD_PLATFORM_ANDROID || LORD_PLATFORM == LORD_PLATFORM_WINDOWS) 
//
//			GLenum no_error = eglGetError();
//
//			if( no_error != GL_NO_ERROR && no_error != EGL_SUCCESS )
//			{
//				LordLogError( "glCompressedTexImage2D: etc compressed Texture[%s] Failed in _upload_etc_aa_ktx() alpha part! error code : %d", getName().c_str(), no_error );
//				return false;
//			}
//#else
//			GLenum no_error = glGetError();
//
//			if( no_error != GL_NO_ERROR )
//			{
//				LordLogError( "glCompressedTexImage2D: etc compressed Texture[%s] Failed in _upload_etc_aa_ktx() alpha part! error code : %d", getName().c_str(), no_error );
//				return false;
//			}
//#endif
//#endif
//			offset += *imageLodSize;
//			m_uploadedSize += *imageLodSize;
//
//		}
//  }} upload alpha part 

// upload rgb part {{
		{

		ui8* pPixelRgbData = m_pPreparedData + 4 + 4 + m_uiRgbSize + m_headerSize;
//		KTXHeader* pktxRgbHeader = (KTXHeader *)(m_pPreparedData + 4 + 4 + m_uiRgbSize);

		LordAssert( pPixelRgbData );

		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
		glGenTextures(1, &m_hTexture);
		glBindTexture( GL_TEXTURE_2D, m_hTexture );

		ui32 curWidth = 0;
		ui32 curHeight = 0;
		ui32 offset = 0;
		const ui32 c_ref = 1;
		for( ui32 cur_level = 0; cur_level < m_numMipmaps; cur_level ++ )
		{
			curWidth = Math::Max( c_ref, m_width >> cur_level );
			curHeight = Math::Max( c_ref, m_height >> cur_level );
			ui32* imageLodSize = (ui32*)(pPixelRgbData + offset);
			offset += sizeof(ui32);

			glCompressedTexImage2D( GL_TEXTURE_2D, cur_level, m_internalFormat, curWidth, curHeight, 0, *imageLodSize, pPixelRgbData + offset );

#if _DEBUG
#if (LORD_PLATFORM == LORD_PLATFORM_ANDROID || LORD_PLATFORM == LORD_PLATFORM_WINDOWS)

			GLenum no_error = eglGetError();

			if (no_error != GL_NO_ERROR && no_error != EGL_SUCCESS)
			{
				LordLogError("glCompressedTexImage2D: etc compressed Texture[%s] Failed in _upload_etc_aa_ktx() rgb part! error code : %d", getName().c_str(), no_error);
				return false;
			}
#else
			GLenum no_error = glGetError();

			if( no_error != GL_NO_ERROR )
			{
				LordLogError( "glCompressedTexImage2D: etc compressed Texture[%s] Failed in _upload_etc_aa_ktx() rgb part! error code : %d", getName().c_str(), no_error );
				return false;
			}
#endif
#endif
			offset += *imageLodSize;
			m_uploadedSize += *imageLodSize;
		}

		}
//  }} upload rgb part 
		return true;
	}
}
