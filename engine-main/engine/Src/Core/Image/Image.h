/********************************************************************
filename: 	Image.h
file path:	dev\engine\Src\Core\Image\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_IMAGE_H__
#define __LORD_IMAGE_H__

#include "Core.h"
#include "Object/Buffer.h"
#include "Color.h"

namespace LORD
{
	class DataStream;
	class PixelBox;
	class LORD_CORE_API Image: public ObjectAlloc
	{
	protected:
		Image();

	public:
		Image(Byte* pData, uint width, uint height, uint depth, 
			PixelFormat pixFmt, uint numFaces = 1, uint numMipMaps = 0);

		virtual ~Image();


		static const uint	MAX_MINMAPS = 16;

		enum ImageFlags
		{
			IMGFLAG_COMPRESSED	= 0x00000001, 
			IMGFLAG_CUBEMAP		= 0x00000002, 
			IMGFLAG_3DTEX		= 0x00000004, 
		};

		enum ImageFilter
		{
			IMGFILTER_NEAREST, 
			IMGFILTER_LINEAR, 
			IMGFILTER_BILINEAR, 
			IMGFILTER_BOX, 
			IMGFILTER_TRIANGLE, 
			IMGFILTER_BICUBIC, 
		};

		struct ImageInfo 
		{
			uint					width;
			uint					height;
			uint					depth;
			uint					size;
			Word					numMipmaps;			//!< used for dds.
			uint					flags;
			PixelFormat				pixFmt;
		};

	public:

		static String				GetImageFormatName(ImageFormat imgFmt);
		static Image*				CreateFromMemory(const Buffer &inBuff, ImageFormat imgFmt);
		static Image*				loadFromDataStream(DataStream* stream, const String& name);
		static Image*				loadFromFile(const String& fileName);

		virtual bool				saveToFile(const String &filename, ImageFormat imgFmt = IF_UNKNOWN);
		virtual void				destroy();

		virtual bool				hasFlag(ImageFlags imgFlag) const;
		virtual PixelFormat			getPixelFormat() const;
		virtual uint				getWidth() const;
		virtual uint				getHeight() const;
		virtual uint				getDepth() const;
		virtual uint				getNumMipmaps() const;
		virtual uint				getNumFaces() const;
		virtual bool				hasAlpha() const;
		virtual Byte				getBPP() const;
		virtual PixelBox			getPixelBox(uint face = 0, uint mipmap = 0) const;
		virtual Byte*				getData() const;

		/**
		* Get color value from a certain location in the image. The z coordinate
		* is only valid for cubemap and volume textures. This uses the first (largest) mipmap.
		*/
		virtual Color				getColor(int x, int y, int z) const;

		// Resize a 2D image, applying the appropriate filter.
		virtual bool				scale(uint width, uint height, ImageFilter filter = IMGFILTER_BILINEAR);

		static String				GetImageFormatExt(ImageFormat imgFmt);
		static ImageFormat			GetImageFormat(const String &filename);
		static ImageFormat			GetImageFormatByExt(const String &imgExt);

		// Static function to calculate size in bytes from the number of mipmaps, faces and the dimensions
		static uint					CalculateSize(uint mipmaps, uint faces, uint width, uint height, uint depth, PixelFormat pixFmt);

		/** Scale a 1D, 2D or 3D image volume. 
		@param 	src			PixelBox containing the source pointer, dimensions and format
		@param 	dst			PixelBox containing the destination pointer, dimensions and format
		@param 	filter		Which filter to use
		@remarks 	This function can do pixel format conversion in the process.
		@note	dst and src can point to the same PixelBox object without any problem
		*/
		static bool					Scale(const PixelBox &src, const PixelBox &dst, ImageFilter filter = IMGFILTER_BILINEAR);

	protected:
		PixelFormat			m_pixFmt;			// The pixel format of the image
		uint				m_width;			// The width of the image in pixels
		uint				m_height;			// The height of the image in pixels
		uint				m_depth;			// The depth of the image
		uint				m_numMipmaps;		// The number of mipmaps the image contains
		int					m_flags;			// Image specific flags.
		uint				m_pixelSize;		// The number of bytes per pixel
		uint				m_size;
		Byte*				m_pData;
	};
}

#endif