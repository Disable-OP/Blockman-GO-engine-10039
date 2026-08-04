/********************************************************************
filename: 	PixelFormat.h
file path:	dev\engine\Src\Core\Image\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_PIXELFORMAT_H__
#define __LORD_PIXELFORMAT_H__

#include "Core.h"

namespace LORD
{
	//////////////////////////////////////////////////////////////////////////

	class LORD_CORE_API PixelBox
	{
	public:
		PixelBox();

		/** Constructor providing width, height and depth. This constructor
		assumes the pixel data is laid out consecutively in memory. (this
		means row after row, slice after slice, with no space in between)
		@param width	    Width of the region
		@param height	    Height of the region
		@param depth	    Depth of the region
		@param pixelFormat	Format of this buffer
		@param pixelData    Pointer to the actual data
		*/
		PixelBox(uint width, uint height, uint depth, PixelFormat pixFormat, void* pPixData = NULL);

		~PixelBox();

	public:
		uint			left;
		uint			top;
		uint			right;
		uint			bottom;
		uint			front;
		uint			back;

		// The data pointer 
		void*			pData;

		// The pixel format 
		PixelFormat		pixFmt;

		/** Number of elements between the leftmost pixel of one row and the left
		pixel of the next. This value must always be equal to getWidth() (consecutive) 
		for compressed formats.
		*/
		uint			rowPitch;

		/** Number of elements between the top left pixel of one (depth) slice and 
		the top left pixel of the next. This can be a negative value. Must be a multiple of
		rowPitch. This value must always be equal to getWidth()*getHeight() (consecutive) 
		for compressed formats.
		*/
		uint			slicePitch;

	public:

		// Get the width of this box
		inline uint			getWidth() const { return right-left; }
		// Get the height of this box
		inline uint			getHeight() const { return bottom-top; }
		// Get the depth of this box
		inline uint			getDepth() const { return back-front; }

		// Return true if the other box is a part of this one
		inline bool			contains(const PixelBox &def) const
		{
			return (def.left >= left && def.top >= top && def.front >= front &&
				def.right <= right && def.bottom <= bottom && def.back <= back);
		}

		/** Set the rowPitch and slicePitch so that the buffer is laid out consecutive 
		in memory.
		*/
		void				setConsecutive();

		/**	Get the number of elements between one past the rightmost pixel of 
		one row and the leftmost pixel of the next row. (IE this is zero if rows
		are consecutive).
		*/
		uint				getRowSkip() const;

		/** Get the number of elements between one past the right bottom pixel of
		one slice and the left top pixel of the next slice. (IE this is zero if slices
		are consecutive).
		*/
		uint				getSliceSkip() const;

		/** Return whether this buffer is laid out consecutive in memory (ie the pitches
		are equal to the dimensions)
		*/
		bool				isConsecutive() const;

		/** Return the size (in bytes) this image would take if it was
		laid out consecutive in memory
		*/
		uint				getConsecutiveSize() const;

		/** Return a subvolume of this PixelBox.
		@param def	Defines the bounds of the subregion to return
		@return	A pixel box describing the region and the data in it
		@remarks	This function does not copy any data, it just returns
		a PixelBox object with a data pointer pointing somewhere inside 
		the data of object.
		@throws	Exception(ERR_INVALIDPARAMS) if def is not fully contained
		*/
		bool				getSubVolume(PixelBox &out, const PixelBox &def) const;

		/**
		* Get colour value from a certain location in the PixelBox. The z coordinate
		* is only valid for cubemaps and volume textures. This uses the first (largest)
		* mipmap.
		*/
		Color				getColor(uint x, uint y, uint z) const;

		/**
		* Set colour value at a certain location in the PixelBox. The z coordinate
		* is only valid for cubemaps and volume textures. This uses the first (largest)
		* mipmap.
		*/
		void				setColor(const Color &color, uint x, uint y, uint z);
	};

	//////////////////////////////////////////////////////////////////////////

	class LORD_CORE_API PixelUtil
	{
	public:
		static String				GetPixelFormatName(PixelFormat pixFmt);
		static uint					GetPixelBits(PixelFormat pixFmt);
		static uint					GetPixelSize(PixelFormat pixFmt);
		static Dword				GetFlags(PixelFormat pixFmt);
		static bool					IsNormalized(PixelFormat pixFmt);
		static bool					HasAlpha(PixelFormat pixFmt);
		static bool					IsFloatingPoint(PixelFormat pixFmt);
		static bool					IsCompressed(PixelFormat pixFmt);
		static bool					IsDepth(PixelFormat pixFmt);
		static bool					IsStencil(PixelFormat pixFmt);
		static bool					IsLuminance(PixelFormat pixFmt);
		static bool					IsNativeEndian(PixelFormat pixFmt);
		static bool					IsAccessible(PixelFormat pixFmt);
		static PixelChannelType		GetChannelType(PixelFormat pixFmt, uint chIdx);
		static uint					GetChannelCount(PixelFormat pixFmt);
		static uint					GetMemorySize(uint width, uint height, uint depth, PixelFormat pixFmt);
		static uint					CalcLevelSize(uint width, uint height, uint depth, uint level, PixelFormat pixFmt);
		static uint					CalcSurfaceSize(uint width, uint height, uint depth, uint mipmaps, PixelFormat pixFmt);
		static void					UnpackColor(Color& color, PixelFormat pixFmt, const void* pSrc);
		static void					UnpackColor(float& r, float& g, float& b, float& a, PixelFormat pixFmt, const void* pSrc);
		static void					PackColor(const Color& color, PixelFormat pixFmt, void* pDest);
		static void					PackColor(float r, float g, float b, float a, PixelFormat pixFmt, void* pDest);
		static Color				ConvertColor(const Color& color, PixelFormat pixFmt);
		static int					optionalPO2(int n);
		static bool					isOptionalPO2(int n);

		// Convert pixels from one format to another
		static void					BulkPixelConversion(void* pSrcData, PixelFormat srcPixFmt, void* pDestData, PixelFormat dstPixFmt, uint count);
		static void					BulkPixelConversion(const PixelBox& src, const PixelBox& dst);
	};

}

#endif
