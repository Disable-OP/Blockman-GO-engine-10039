/********************************************************************
filename: 	GLES2TextureLoaderHelper.h
file path:	dev\engine\Src\GLES2Render\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __GLES2_TexturLoader_H
#define __GLES2_TexturLoader_H

#include "TypeDef.h"
#include "CoreStd.h"

namespace LORD
{

	template<char ch0, char ch1, char ch2, char ch3>
	struct FourCC
	{
		enum { value = ( (ch0 << 0) + (ch1 << 8) + (ch2 << 16) + (ch3 << 24) ) };
	};


#define	TEXTURE_COMPRESSED_PVR3			FourCC<'P', 'V', 'R', 3>::value			
#define	TEXTURE_COMPRESSED_ETC1			FourCC<'E', 'T', 'C', '1'>:value		
#define TEXTURE_COMPRESSED_DDS			FourCC<'D', 'D', 'S', ' '>::value		
#define TEXTURE_COMPRESSED_COMMON		FourCC<'C', 'O', 'M', 'M'>::value


	
	const ui32 c_pvrtex3_ident = TEXTURE_COMPRESSED_PVR3;

	enum EPVRTPixelFormat
	{
		ePVRTPF_PVRTCI_2bpp_RGB,
		ePVRTPF_PVRTCI_2bpp_RGBA,
		ePVRTPF_PVRTCI_4bpp_RGB,
		ePVRTPF_PVRTCI_4bpp_RGBA,
		ePVRTPF_PVRTCII_2bpp,
		ePVRTPF_PVRTCII_4bpp,
		ePVRTPF_ETC1,
		ePVRTPF_DXT1,  
		ePVRTPF_DXT2,
		ePVRTPF_DXT3,
		ePVRTPF_DXT4,
		ePVRTPF_DXT5,

		//These formats are identical to some DXT formats.
		ePVRTPF_BC1 = ePVRTPF_DXT1,
		ePVRTPF_BC2 = ePVRTPF_DXT3,
		ePVRTPF_BC3 = ePVRTPF_DXT5,

		//These are currently unsupported:
		ePVRTPF_BC4,
		ePVRTPF_BC5,
		ePVRTPF_BC6,
		ePVRTPF_BC7,

		//These are supported
		ePVRTPF_UYVY,
		ePVRTPF_YUY2,
		ePVRTPF_BW1bpp,
		ePVRTPF_SharedExponentR9G9B9E5,
		ePVRTPF_RGBG8888,
		ePVRTPF_GRGB8888,
		ePVRTPF_ETC2_RGB,
		ePVRTPF_ETC2_RGBA,
		ePVRTPF_ETC2_RGB_A1,
		ePVRTPF_EAC_R11,
		ePVRTPF_EAC_RG11,

		//Invalid value
		ePVRTPF_NumCompressedPFs
	};

#if (LORD_PLATFORM == LORD_PLATFORM_WINDOWS)
#pragma pack( push, 4 )
#endif	
	struct PVRTextureHeaderV3{
		ui32	u32Version;			//Version of the file header, used to identify it.
		ui32	u32Flags;			//Various format flags.
		ui64	u64PixelFormat;		//The pixel format, 8cc value storing the 4 channel identifiers and their respective sizes.
		ui32	u32ColourSpace;		//The Colour Space of the texture, currently either linear RGB or sRGB.
		ui32	u32ChannelType;		//Variable type that the channel is stored in. Supports signed/unsigned int/short/byte or float for now.
		ui32	u32Height;			//Height of the texture.
		ui32	u32Width;			//Width of the texture.
		ui32	u32Depth;			//Depth of the texture. (Z-slices)
		ui32	u32NumSurfaces;		//Number of members in a Texture Array.
		ui32	u32NumFaces;		//Number of faces in a Cube Map. Maybe be a value other than 6.
		ui32	u32MIPMapCount;		//Number of MIP Maps in the texture - NB: Includes top level.
		ui32	u32MetaDataSize;	//Size of the accompanying meta data.

		//Constructor for the header - used to make sure that the header is initialised usefully. The initial pixel format is an invalid one and must be set.
		PVRTextureHeaderV3()
			: u32Version(c_pvrtex3_ident)
			, u32Flags(0)
			, u64PixelFormat(ePVRTPF_NumCompressedPFs)
			, u32ColourSpace(0),u32ChannelType(0)
			, u32Height(1),u32Width(1),u32Depth(1)
			, u32NumSurfaces(1),u32NumFaces(1)
			, u32MIPMapCount(1),u32MetaDataSize(0)
		{}
	};
#if (LORD_PLATFORM == LORD_PLATFORM_WINDOWS)
#pragma pack( pop )
#endif	

	const ui32 c_pvrtex3_header_size = 52; // sizeof(PVRTextureHeaderV3)

	ui32 pvrInternalformatMapping( ui64& pixelFormat );

	void pvrPixelDimension( ui64 pixelFormat, ui32& xDimension, ui32& yDimension, ui32& zDimension );

	ui32 pvrBitsPerPixel( ui64 pixelFormat );

	struct DDSPixelFormat
	{
		unsigned int dwSize;
		unsigned int dwFlags;
		unsigned int dwFourCC;
		unsigned int dwRGBBitCount;
		unsigned int dwRBitMask;
		unsigned int dwGBitMask;
		unsigned int dwBBitMask;
		unsigned int dwABitMask;
	};

	struct DDSHeader
	{
		unsigned int     dwSize;
		unsigned int     dwFlags;
		unsigned int     dwHeight;
		unsigned int     dwWidth;
		unsigned int     dwPitchOrLinearSize;
		unsigned int     dwDepth;
		unsigned int     dwMipMapCount;
		unsigned int     dwReserved1[11];
		DDSPixelFormat	 ddspf;
		unsigned int     dwCaps;
		unsigned int     dwCaps2;
		unsigned int     dwCaps3;
		unsigned int     dwCaps4;
		unsigned int     dwReserved2;
	};

#ifndef GL_COMPRESSED_RGBA_S3TC_DXT1_EXT
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT 0x83F1
#endif

#ifndef GL_COMPRESSED_RGBA_S3TC_DXT3_EXT
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT 0x83F2
#endif

#ifndef GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT 0x83F3
#endif

#ifndef GL_ETC1_RGB8_OES
#define GL_ETC1_RGB8_OES				0x8D64
#endif
	
#ifndef GL_ATC_RGB_AMD
#define GL_ATC_RGB_AMD					0x8C92
#endif

#ifndef GL_ATC_RGBA_EXPLICIT_ALPHA_AMD
#define GL_ATC_RGBA_EXPLICIT_ALPHA_AMD                          0x8C93
#endif

#ifndef GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD
#define GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD                      0x87EE
#endif

	struct KTXHeader
	{
		ui8					m_identifier[12];
		ui32				m_endianness;
		ui32				m_type;
		ui32				m_typeSize;
		ui32				m_format;
		ui32				m_internalFormat;
		ui32				m_baseInternalFormat;
		ui32				m_pixelWidth;
		ui32				m_pixelHeight;
		ui32				m_pixelDepth;
		ui32				m_numberOfArrayElements;
		ui32				m_numberOfFaces;
		ui32				m_numberOfMipmapLevels;
		ui32				m_bytesOfKeyValueData;


	};

	static const ui8 cs_etc1_identifier[12] = { 0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31, 0x31, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A };
	static const ui32 cs_big_endian = 0x04030201;
	static const ui32 cs_little_endian = 0x01020304;


}

#endif