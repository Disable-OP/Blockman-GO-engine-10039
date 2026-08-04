#include "Core.h"
#include "PixelFormat.h"
#include "Util/AssertX.h"
#include "PixelConversions.h"
#include "Color.h"

namespace LORD
{
	PixelBox::PixelBox()
		: left(0)
		, top(0)
		, right(1)
		, bottom(1)
		, front(0)
		, back(1)
		, pixFmt(PF_UNKNOWN)
	{
	}

	PixelBox::PixelBox(uint width, uint height, uint depth, PixelFormat pixFormat, void *pPixData)
		: left(0)
		, top(0)
		, right(width)
		, bottom(height)
		, front(0)
		, back(depth)
		, pData(pPixData)
		, pixFmt(pixFormat)
	{
		setConsecutive();
	}

	PixelBox::~PixelBox()
	{
	}

	void PixelBox::setConsecutive()
	{
		rowPitch = getWidth();
		slicePitch = getWidth()*getHeight();
	}

	uint PixelBox::getRowSkip() const
	{
		return (rowPitch - getWidth());
	}

	uint PixelBox::getSliceSkip() const
	{
		return (slicePitch - getHeight() * rowPitch);
	}

	bool PixelBox::isConsecutive() const 
	{ 
		return (rowPitch == getWidth() && slicePitch == getWidth()*getHeight()); 
	}

	uint PixelBox::getConsecutiveSize() const
	{
		return PixelUtil::GetMemorySize(getWidth(), getHeight(), getDepth(), pixFmt);
	}

	bool PixelBox::getSubVolume(PixelBox& out, const PixelBox& def) const
	{
		if(PixelUtil::IsCompressed(pixFmt))
		{
			if(def.left == left && def.top == top && def.front == front &&
				def.right == right && def.bottom == bottom && def.back == back)
			{
				// Entire buffer is being queried
				out = *this;
				return true;
			}

			LordLogError("Cannot return subvolume of compressed PixelBuffer");
			return false;
		}

		if(!contains(def))
		{
			LordLogError("Bounds out of range");
			return false;
		}

		const uint pixSize = PixelUtil::GetPixelSize(pixFmt);

		// Calculate new data origin
		// Notice how we do not propagate left/top/front from the incoming box, since
		// the returned pointer is already offset
		PixelBox rval(def.getWidth(), def.getHeight(), def.getDepth(), pixFmt, 
			((ui8*)pData) + ((def.left-left)*pixSize) + ((def.top-top)*rowPitch*pixSize) + ((def.front-front)*slicePitch*pixSize));

		rval.rowPitch = rowPitch;
		rval.slicePitch = slicePitch;
		rval.pixFmt = pixFmt;

		out = rval;

		return true;
	}

	Color PixelBox::getColor(uint x, uint y, uint z) const
	{
		Color color;

		Byte pixelSize = PixelUtil::GetPixelSize(pixFmt);
		size_t pixelOffset = pixelSize * (z * slicePitch + y * rowPitch + x);
		PixelUtil::UnpackColor(color, pixFmt, (Byte*)pData + pixelOffset);

		return color;
	}

	void PixelBox::setColor(const Color& color, uint x, uint y, uint z)
	{
		Byte pixelSize = PixelUtil::GetPixelSize(pixFmt);
		size_t pixelOffset = pixelSize * (z * slicePitch + y * rowPitch + x);
		PixelUtil::PackColor(color, pixFmt, (Byte*)pData + pixelOffset);
	}

	//////////////////////////////////////////////////////////////////////////

	/**
	* A record that describes a pixel format in detail.
	*/
	struct PixelFormatDesc
	{
		// Name of the format, as in the enum
		String				name;
		// Pixel format flags, see enum PixelFormatFlags for the bit field definitions
		ui32				flags;
		// Channel count
		Byte				chCount;
		// Channel format
		PixelChannelFormat	ch0, ch1, ch2, ch3;
		// Channel type
		PixelChannelType	ch0Type, ch1Type, ch2Type, ch3Type;
		// Number of bits for channel0, channel1, channel2, channel3
		Byte				ch0Bits, ch1Bits, ch2Bits, ch3Bits;
		// Masks and shifts as used by packers/unpackers
		ui32				ch0Mask, ch1Mask, ch2Mask, ch3Mask;
		Byte				ch0Shift, ch1Shift, ch2Shift, ch3Shift;
	};


	/** Pixel format database */
	PixelFormatDesc g_pixFmtDescArr[PF_COUNT] =
	{
		//-----------------------------------------------------------------------
		{
			"PF_UNKNOWN", 
			/* Flags */
			0, 
			/* Channel count */
			0, 
			/* Channel format */
			PCF_NONE, PCF_NONE, PCF_NONE, PCF_NONE, 
			/* Channel type */
			PCT_NONE, PCT_NONE, PCT_NONE, PCT_NONE, 
			/* Channel bits */
			0, 0, 0, 0, 
			/* Masks and shifts */
			0, 0, 0, 0, 
			0, 0, 0, 0
		}, 
		//-----------------------------------------------------------------------
		{
			"PF_R8_UNORM", 
			/* Flags */
			PFF_NORMALIZED | PFF_LUMINANCE | PFF_NATIVEENDIAN,
			/* Channel count */
			1, 
			/* Channel format */
			PCF_R, PCF_NONE, PCF_NONE, PCF_NONE, 
			/* Channel type */
			PCT_UNORM, PCT_NONE, PCT_NONE, PCT_NONE, 
			/* Channel bits */
			8, 0, 0, 0, 
			/* Masks and shifts */
			0xFF, 0, 0, 0, 
			0, 0, 0, 0
		}, 
		//-----------------------------------------------------------------------
		{
			"PF_R8_SNORM", 
			/* Flags */
			PFF_NORMALIZED | PFF_LUMINANCE | PFF_NATIVEENDIAN, 
			/* Channel count */
			1, 
			/* Channel format */
			PCF_R, PCF_NONE, PCF_NONE, PCF_NONE, 
			/* Channel type */
			PCT_SNORM, PCT_NONE, PCT_NONE, PCT_NONE, 
			/* Channel bits */
			8, 0, 0, 0, 
			/* Masks and shifts */
			0xFF, 0, 0, 0, 
			0, 0, 0, 0
		}, 
		//-----------------------------------------------------------------------
		{
			"PF_R8_UINT", 
			/* Flags */
			PFF_LUMINANCE | PFF_NATIVEENDIAN, 
			/* Channel count */
			1, 
			/* Channel format */
			PCF_R, PCF_NONE, PCF_NONE, PCF_NONE, 
			/* Channel type */
			PCT_UINT, PCT_NONE, PCT_NONE, PCT_NONE, 
			/* Channel bits */
			8, 0, 0, 0, 
			/* Masks and shifts */
			0xFF, 0, 0, 0, 
			0, 0, 0, 0
		}, 
		//-----------------------------------------------------------------------
		{
			"PF_R8_SINT", 
			/* Flags */
			PFF_LUMINANCE | PFF_NATIVEENDIAN, 
			/* Channel count */
			1, 
			/* Channel format */
			PCF_R, PCF_NONE, PCF_NONE, PCF_NONE, 
			/* Channel type */
			PCT_SINT, PCT_NONE, PCT_NONE, PCT_NONE, 
			/* Channel bits */
			8, 0, 0, 0, 
			/* Masks and shifts */
			0xFF, 0, 0, 0, 
			0, 0, 0, 0
		},
		//-----------------------------------------------------------------------
		{
			"PF_A8_UNORM",
			/* Flags */
			PFF_NORMALIZED | PFF_ALPHA | PFF_NATIVEENDIAN,
			/* Channel count */
			1,
			/* Channel format */
			PCF_NONE, PCF_NONE, PCF_NONE, PCF_A,
			/* Channel type */
			PCT_NONE, PCT_NONE, PCT_NONE, PCT_UNORM,
			/* Channel bits */
			0, 0, 0, 8,
			/* Masks and shifts */
			0, 0, 0, 0xFF,
			0, 0, 0, 0
		},
		//-----------------------------------------------------------------------
		{
			"PF_RG8_UNORM", 
			/* Flags */
			PFF_NORMALIZED | PFF_NATIVEENDIAN, 
			/* Channel count */
			2, 
			/* Channel format */
			PCF_R, PCF_G, PCF_NONE, PCF_NONE, 
			/* Channel type */
			PCT_UNORM, PCT_UNORM, PCT_NONE, PCT_NONE, 
			/* Channel bits */
			8, 8, 0, 0, 
			/* Masks and shifts */
			0xFF00, 0x00FF, 0, 0, 
			8, 0, 0, 0
		}, 
		//-----------------------------------------------------------------------
		{
			"PF_RG8_SNORM", 
			/* Flags */
			PFF_NORMALIZED | PFF_NATIVEENDIAN, 
			/* Channel count */
			2, 
			/* Channel format */
			PCF_R, PCF_G, PCF_NONE, PCF_NONE, 
			/* Channel type */
			PCT_SNORM, PCT_SNORM, PCT_NONE, PCT_NONE, 
			/* Channel bits */
			8, 8, 0, 0, 
			/* Masks and shifts */
			0xFF00, 0x00FF, 0, 0, 
			8, 0, 0, 0
		}, 
		//-----------------------------------------------------------------------
		{
			"PF_RG8_UINT", 
			/* Flags */
			PFF_NATIVEENDIAN, 
			/* Channel count */
			2, 
			/* Channel format */
			PCF_R, PCF_G, PCF_NONE, PCF_NONE, 
			/* Channel type */
			PCT_UINT, PCT_UINT, PCT_NONE, PCT_NONE, 
			/* Channel bits */
			8, 8, 0, 0, 
			/* Masks and shifts */
			0xFF00, 0x00FF, 0, 0, 
			8, 0, 0, 0
		}, 
		//-----------------------------------------------------------------------
		{
			"PF_RG8_SINT", 
			/* Flags */
			PFF_NATIVEENDIAN, 
			/* Channel count */
			2, 
			/* Channel format */
			PCF_R, PCF_G, PCF_NONE, PCF_NONE, 
			/* Channel type */
			PCT_SINT, PCT_SINT, PCT_NONE, PCT_NONE, 
			/* Channel bits */
			8, 8, 0, 0, 
			/* Masks and shifts */
			0xFF00, 0x00FF, 0, 0, 
			8, 0, 0, 0
		}, 
		//-----------------------------------------------------------------------
		{
			"PF_RGB8_UNORM", 
			/* Flags */
			PFF_NORMALIZED | PFF_NATIVEENDIAN, 
			/* Channel count */
			3, 
			/* Channel format */
			PCF_R, PCF_G, PCF_B, PCF_NONE, 
			/* Channel type */
			PCT_UNORM, PCT_UNORM, PCT_UNORM, PCT_NONE, 
			/* Channel bits */
			8, 8, 8, 0, 
			/* Masks and shifts */
			0xFF0000, 0x00FF00, 0x0000FF, 0, 
			16, 8, 0, 0
		}, 
	//-----------------------------------------------------------------------
		{
			"PF_RGB8_SNORM", 
			/* Flags */
			PFF_NORMALIZED | PFF_NATIVEENDIAN, 
			/* Channel count */
			3, 
			/* Channel format */
			PCF_R, PCF_G, PCF_B, PCF_NONE, 
			/* Channel type */
			PCT_SNORM, PCT_SNORM, PCT_SNORM, PCT_NONE, 
			/* Channel bits */
			8, 8, 8, 0, 
			/* Masks and shifts */
			0xFF0000, 0x00FF00, 0x0000FF, 0, 
			16, 8, 0, 0
		}, 
		//-----------------------------------------------------------------------
		{
			"PF_RGB8_UINT", 
			/* Flags */
			PFF_NATIVEENDIAN, 
			/* Channel count */
			3, 
			/* Channel format */
			PCF_R, PCF_G, PCF_B, PCF_NONE, 
			/* Channel type */
			PCT_UINT, PCT_UINT, PCT_UINT, PCT_NONE, 
			/* Channel bits */
			8, 8, 8, 0, 
			/* Masks and shifts */
			0xFF0000, 0x00FF00, 0x0000FF, 0, 
			16, 8, 0, 0
		}, 
		//-----------------------------------------------------------------------
		{
			"PF_RGB8_SINT", 
			/* Flags */
			PFF_NATIVEENDIAN, 
			/* Channel count */
			3, 
			/* Channel format */
			PCF_R, PCF_G, PCF_B, PCF_NONE, 
			/* Channel type */
			PCT_SINT, PCT_SINT, PCT_SINT, PCT_NONE, 
			/* Channel bits */
			8, 8, 8, 0, 
			/* Masks and shifts */
			0xFF0000, 0x00FF00, 0x0000FF, 0, 
			16, 8, 0, 0
		}, 
		//-----------------------------------------------------------------------
		{
			"PF_BGR8_UNORM", 
			/* Flags */
			PFF_NORMALIZED | PFF_NATIVEENDIAN, 
			/* Channel count */
			3, 
			/* Channel format */
			PCF_R, PCF_G, PCF_B, PCF_NONE, 
			/* Channel type */
			PCT_UNORM, PCT_UNORM, PCT_UNORM, PCT_NONE, 
			/* Channel bits */
			8, 8, 8, 0, 
			/* Masks and shifts */
			0x0000FF, 0x00FF00, 0xFF0000, 0, 
			0, 8, 16, 0
		}, 
		//-----------------------------------------------------------------------
		{
			"PF_RGBA8_UNORM", 
			/* Flags */
			PFF_NORMALIZED | PFF_ALPHA | PFF_NATIVEENDIAN, 
			/* Channel count */
			4, 
			/* Channel format */
			PCF_R, PCF_G, PCF_B, PCF_A, 
			/* Channel type */
			PCT_UNORM, PCT_UNORM, PCT_UNORM, PCT_UNORM, 
			/* Channel bits */
			8, 8, 8, 8, 
			/* Masks and shifts */
			0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF, 
			24, 16, 8, 0
		}, 
		//-----------------------------------------------------------------------
		{
			"PF_RGBA8_SNORM", 
			/* Flags */
			PFF_NORMALIZED | PFF_ALPHA | PFF_NATIVEENDIAN, 
			/* Channel count */
			4, 
			/* Channel format */
			PCF_R, PCF_G, PCF_B, PCF_A, 
			/* Channel type */
			PCT_SNORM, PCT_SNORM, PCT_SNORM, PCT_SNORM, 
			/* Channel bits */
			8, 8, 8, 8, 
			/* Masks and shifts */
			0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF, 
			24, 16, 8, 0
		}, 
		//-----------------------------------------------------------------------
		{
			"PF_RGBA8_UINT", 
			/* Flags */
			PFF_ALPHA | PFF_NATIVEENDIAN, 
			/* Channel count */
			4, 
			/* Channel format */
			PCF_R, PCF_G, PCF_B, PCF_A, 
			/* Channel type */
			PCT_UINT, PCT_UINT, PCT_UINT, PCT_UINT, 
			/* Channel bits */
			8, 8, 8, 8, 
			/* Masks and shifts */
			0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF, 
			24, 16, 8, 0
		}, 
		//-----------------------------------------------------------------------
		{
			"PF_RGBA8_SINT", 
			/* Flags */
			PFF_ALPHA | PFF_NATIVEENDIAN, 
			/* Channel count */
			4, 
			/* Channel format */
			PCF_R, PCF_G, PCF_B, PCF_A, 
			/* Channel type */
			PCT_SINT, PCT_SINT, PCT_SINT, PCT_SINT, 
			/* Channel bits */
			8, 8, 8, 8, 
			/* Masks and shifts */
			0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF, 
			24, 16, 8, 0
		}, 
		//-----------------------------------------------------------------------
		{
			"PF_BGRA8_UNORM", 
			/* Flags */
			PFF_NORMALIZED | PFF_ALPHA | PFF_NATIVEENDIAN, 
			/* Channel count */
			4, 
			/* Channel format */
			PCF_R, PCF_G, PCF_B, PCF_A, 
			/* Channel type */
			PCT_UNORM, PCT_UNORM, PCT_UNORM, PCT_UNORM, 
			/* Channel bits */
			8, 8, 8, 8, 
			/* Masks and shifts */
			0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000,
			16, 8, 0, 24,
		}, 
		//-----------------------------------------------------------------------
		{
			"PF_R16_UNORM", 
			/* Flags */
			PFF_NORMALIZED | PFF_LUMINANCE, 
			/* Channel count */
			1, 
			/* Channel format */
			PCF_R, PCF_NONE, PCF_NONE, PCF_NONE, 
			/* Channel type */
			PCT_UNORM, PCT_NONE, PCT_NONE, PCT_NONE, 
			/* Channel bits */
			16, 0, 0, 0, 
			/* Masks and shifts */
			0xFFFF, 0, 0, 0,
			0, 0, 0, 0
		}, 
		//-----------------------------------------------------------------------
		{
			"PF_R16_SNORM", 
			/* Flags */
			PFF_NORMALIZED | PFF_LUMINANCE, 
			/* Channel count */
			1, 
			/* Channel format */
			PCF_R, PCF_NONE, PCF_NONE, PCF_NONE, 
			/* Channel type */
			PCT_SNORM, PCT_NONE, PCT_NONE, PCT_NONE, 
			/* Channel bits */
			16, 0, 0, 0, 
			/* Masks and shifts */
			0xFFFF, 0, 0, 0,
			0, 0, 0, 0
		},
		//-----------------------------------------------------------------------
		{
			"PF_R16_UINT",
			/* Flags */
			PFF_LUMINANCE,
			/* Channel count */
			1,
			/* Channel format */
			PCF_R, PCF_NONE, PCF_NONE, PCF_NONE,
			/* Channel type */
			PCT_UINT, PCT_NONE, PCT_NONE, PCT_NONE,
			/* Channel bits */
			16, 0, 0, 0,
			/* Masks and shifts */
			0xFFFF, 0, 0, 0,
			0, 0, 0, 0
		},
		//-----------------------------------------------------------------------
		{
			"PF_R16_SINT", 
			/* Flags */
			PFF_LUMINANCE, 
			/* Channel count */
			1, 
			/* Channel format */
			PCF_R, PCF_NONE, PCF_NONE, PCF_NONE, 
			/* Channel type */
			PCT_SINT, PCT_NONE, PCT_NONE, PCT_NONE, 
			/* Channel bits */
			16, 0, 0, 0, 
			/* Masks and shifts */
			0xFFFF, 0, 0, 0,
			0, 0, 0, 0
		}, 
		//-----------------------------------------------------------------------
		{
			"PF_R16_FLOAT", 
			/* Flags */
			PFF_LUMINANCE | PFF_FLOAT, 
			/* Channel count */
			1, 
			/* Channel format */
			PCF_R, PCF_NONE, PCF_NONE, PCF_NONE, 
			/* Channel type */
			PCT_FLOAT, PCT_NONE, PCT_NONE, PCT_NONE, 
			/* Channel bits */
			16, 0, 0, 0, 
			/* Masks and shifts */
			0, 0, 0, 0,
			0, 0, 0, 0
		}, 
		//-----------------------------------------------------------------------
		{
			"PF_RG16_UNORM", 
			/* Flags */
			PFF_NORMALIZED, 
			/* Channel count */
			2, 
			/* Channel format */
			PCF_R, PCF_G, PCF_NONE, PCF_NONE, 
			/* Channel type */
			PCT_UNORM, PCT_UNORM, PCT_NONE, PCT_NONE, 
			/* Channel bits */
			16, 16, 0, 0, 
			/* Masks and shifts */
			0xFFFF0000, 0x0000FFFF, 0, 0,
			16, 0, 0, 0
		}, 
		//-----------------------------------------------------------------------
		{
			"PF_RG16_SNORM", 
			/* Flags */
			PFF_NORMALIZED, 
			/* Channel count */
			2, 
			/* Channel format */
			PCF_R, PCF_G, PCF_NONE, PCF_NONE, 
			/* Channel type */
			PCT_SNORM, PCT_SNORM, PCT_NONE, PCT_NONE, 
			/* Channel bits */
			16, 16, 0, 0, 
			/* Masks and shifts */
			0xFFFF0000, 0x0000FFFF, 0, 0,
			16, 0, 0, 0
		}, 
		//-----------------------------------------------------------------------
		{
			"PF_RG16_UINT", 
			/* Flags */
			0,
			/* Channel count */
			2, 
			/* Channel format */
			PCF_R, PCF_G, PCF_NONE, PCF_NONE, 
			/* Channel type */
			PCT_UINT, PCT_UINT, PCT_NONE, PCT_NONE, 
			/* Channel bits */
			16, 16, 0, 0, 
			/* Masks and shifts */
			0xFFFF0000, 0x0000FFFF, 0, 0,
			16, 0, 0, 0
		}, 
		//-----------------------------------------------------------------------
		{
			"PF_RG16_SINT", 
			/* Flags */
			PFF_NATIVEENDIAN, 
			/* Channel count */
			2, 
			/* Channel format */
			PCF_R, PCF_G, PCF_NONE, PCF_NONE, 
			/* Channel type */
			PCT_SINT, PCT_SINT, PCT_NONE, PCT_NONE, 
			/* Channel bits */
			16, 16, 0, 0, 
			/* Masks and shifts */
			0xFFFF0000, 0x0000FFFF, 0, 0,
			16, 0, 0, 0
		}, 
		//-----------------------------------------------------------------------
		{
			"PF_RG16_FLOAT", 
			/* Flags */
			PFF_FLOAT, 
			/* Channel count */
			2, 
			/* Channel format */
			PCF_R, PCF_G, PCF_NONE, PCF_NONE, 
			/* Channel type */
			PCT_FLOAT, PCT_FLOAT, PCT_NONE, PCT_NONE, 
			/* Channel bits */
			16, 16, 0, 0, 
			/* Masks and shifts */
			0, 0, 0, 0,
			0, 0, 0, 0
		}, 
		//-----------------------------------------------------------------------
		{
			"PF_RGB16_UNORM", 
			/* Flags */
			PFF_NORMALIZED, 
			/* Channel count */
			3, 
			/* Channel format */
			PCF_R, PCF_G, PCF_B, PCF_NONE, 
			/* Channel type */
			PCT_UNORM, PCT_UNORM, PCT_UNORM, PCT_NONE, 
			/* Channel bits */
			16, 16, 16, 0, 
			/* Masks and shifts */
			0, 0, 0, 0,
			0, 0, 0, 0
		}, 
		//-----------------------------------------------------------------------
		{
			"PF_RGB16_SNORM", 
			/* Flags */
			PFF_NORMALIZED, 
			/* Channel count */
			3, 
			PCF_R, PCF_G, PCF_B, PCF_NONE, 
			/* Channel type */
			PCT_SNORM, PCT_SNORM, PCT_SNORM, PCT_NONE, 
			/* Channel bits */
			16, 16, 16, 0, 
			/* Masks and shifts */
			0, 0, 0, 0,
			0, 0, 0, 0
		}, 
		//-----------------------------------------------------------------------
		{
			"PF_RGB16_UINT", 
			/* Flags */
			0,
			/* Channel count */
			3, 
			/* Channel format */
			PCF_R, PCF_G, PCF_B, PCF_NONE, 
			/* Channel type */
			PCT_UINT, PCT_UINT, PCT_UINT, PCT_NONE, 
			/* Channel bits */
			16, 16, 16, 0, 
			/* Masks and shifts */
			0, 0, 0, 0,
			0, 0, 0, 0
		}, 
		//-----------------------------------------------------------------------
		{
			"PF_RGB16_SINT", 
			/* Flags */
			PFF_NATIVEENDIAN,
			/* Channel count */
			3, 
			/* Channel format */
			PCF_R, PCF_G, PCF_B, PCF_NONE, 
			/* Channel type */
			PCT_SINT, PCT_SINT, PCT_SINT, PCT_NONE, 
			/* Channel bits */
			16, 16, 16, 0, 
			/* Masks and shifts */
			0, 0, 0, 0,
			0, 0, 0, 0
		}, 
		//-----------------------------------------------------------------------
		{
			"PF_RGB16_FLOAT", 
			/* Flags */
			PFF_FLOAT, 
			/* Channel count */
			3, 
			/* Channel format */
			PCF_R, PCF_G, PCF_B, PCF_NONE, 
			/* Channel type */
			PCT_FLOAT, PCT_FLOAT, PCT_FLOAT, PCT_NONE, 
			/* Channel bits */
			16, 16, 16, 0, 
			/* Masks and shifts */
			0, 0, 0, 0,
			0, 0, 0, 0
		}, 
		//-----------------------------------------------------------------------
		{
			"PF_RGBA16_UNORM", 
			/* Flags */
			PFF_NORMALIZED, 
			/* Channel count */
			4, 
			/* Channel format */
			PCF_R, PCF_G, PCF_B, PCF_A, 
			/* Channel type */
			PCT_UNORM, PCT_UNORM, PCT_UNORM, PCT_UNORM, 
			/* Channel bits */
			16, 16, 16, 16, 
			/* Masks and shifts */
			0, 0, 0, 0,
			0, 0, 0, 0
		}, 
		//-----------------------------------------------------------------------
		{
			"PF_RGBA16_SNORM", 
			/* Flags */
			PFF_NORMALIZED, 
			/* Channel count */
			4, 
			/* Channel format */
			PCF_R, PCF_G, PCF_B, PCF_A, 
			/* Channel type */
			PCT_SNORM, PCT_SNORM, PCT_SNORM, PCT_SNORM, 
			/* Channel bits */
			16, 16, 16, 16, 
			/* Masks and shifts */
			0, 0, 0, 0,
			0, 0, 0, 0
		}, 
		//-----------------------------------------------------------------------
		{
			"PF_RGBA16_UINT", 
			/* Flags */
			PFF_NATIVEENDIAN, 
			/* Channel count */
			4, 
			/* Channel format */
			PCF_R, PCF_G, PCF_B, PCF_A, 
			/* Channel type */
			PCT_UINT, PCT_UINT, PCT_UINT, PCT_UINT, 
			/* Channel bits */
			16, 16, 16, 16, 
			/* Masks and shifts */
			0, 0, 0, 0,
			0, 0, 0, 0
		}, 
		//-----------------------------------------------------------------------
		{
			"PF_RGBA16_SINT", 
			/* Flags */
			PFF_NATIVEENDIAN, 
			/* Channel count */
			4, 
			/* Channel format */
			PCF_R, PCF_G, PCF_B, PCF_A, 
			/* Channel type */
			PCT_SINT, PCT_SINT, PCT_SINT, PCT_SINT, 
			/* Channel bits */
			16, 16, 16, 16, 
			/* Masks and shifts */
			0, 0, 0, 0,
			0, 0, 0, 0
		}, 
		//-----------------------------------------------------------------------
		{
			"PF_RGBA16_FLOAT", 
			/* Flags */
			PFF_FLOAT, 
			/* Channel count */
			4, 
			/* Channel format */
			PCF_R, PCF_G, PCF_B, PCF_A, 
			/* Channel type */
			PCT_FLOAT, PCT_FLOAT, PCT_FLOAT, PCT_FLOAT, 
			/* Channel bits */
			16, 16, 16, 16, 
			/* Masks and shifts */
			0, 0, 0, 0,
			0, 0, 0, 0
		}, 
		//-----------------------------------------------------------------------
		{
			"PF_R32_UNORM", 
			/* Flags */
			PFF_NORMALIZED, 
			/* Channel count */
			1, 
			/* Channel format */
			PCF_R, PCF_NONE, PCF_NONE, PCF_NONE, 
			/* Channel type */
			PCT_UNORM, PCT_NONE, PCT_NONE, PCT_NONE, 
			/* Channel bits */
			32, 0, 0, 0, 
			/* Masks and shifts */
			0, 0, 0, 0,
			0, 0, 0, 0
		}, 
		//-----------------------------------------------------------------------
		{
			"PF_R32_SNORM", 
			/* Flags */
			PFF_NORMALIZED, 
			/* Channel count */
			1, 
			/* Channel format */
			PCF_R, PCF_NONE, PCF_NONE, PCF_NONE, 
			/* Channel type */
			PCT_SNORM, PCT_NONE, PCT_NONE, PCT_NONE, 
			/* Channel bits */
			32, 0, 0, 0, 
			/* Masks and shifts */
			0, 0, 0, 0,
			0, 0, 0, 0
		}, 
		//---------------------------------------------------------------------
		{
			"PF_R32_UINT", 
			/* Flags */
			0, 
			/* Channel count */
			1, 
			/* Channel format */
			PCF_R, PCF_NONE, PCF_NONE, PCF_NONE, 
			/* Channel type */
			PCT_UINT, PCT_NONE, PCT_NONE, PCT_NONE, 
			/* Channel bits */
			32, 0, 0, 0, 
			/* Masks and shifts */
			0, 0, 0, 0,
			0, 0, 0, 0
		}, 
		//---------------------------------------------------------------------
		{
			"PF_R32_SINT", 
			/* Flags */
			0, 
			/* Channel count */
			1, 
			/* Channel format */
			PCF_R, PCF_NONE, PCF_NONE, PCF_NONE, 
			/* Channel type */
			PCT_SINT, PCT_NONE, PCT_NONE, PCT_NONE, 
			/* Channel bits */
			32, 0, 0, 0, 
			/* Masks and shifts */
			0, 0, 0, 0,
			0, 0, 0, 0
		}, 
		//---------------------------------------------------------------------
		{
			"PF_R32_FLOAT", 
			/* Flags */
			PFF_FLOAT, 
			/* Channel count */
			1, 
			/* Channel format */
			PCF_R, PCF_NONE, PCF_NONE, PCF_NONE, 
			/* Channel type */
			PCT_FLOAT, PCT_NONE, PCT_NONE, PCT_NONE, 
			/* Channel bits */
			32, 0, 0, 0, 
			/* Masks and shifts */
			0, 0, 0, 0,
			0, 0, 0, 0
		}, 
		//---------------------------------------------------------------------
		{
			"PF_RG32_UNORM", 
			/* Flags */
			PFF_NORMALIZED, 
			/* Channel count */
			2, 
			/* Channel format */
			PCF_R, PCF_G, PCF_NONE, PCF_NONE, 
			/* Channel type */
			PCT_UNORM, PCT_UNORM, PCT_NONE, PCT_NONE, 
			/* Channel bits */
			32, 32, 0, 0, 
			/* Masks and shifts */
			0, 0, 0, 0,
			0, 0, 0, 0
		}, 
		//---------------------------------------------------------------------
		{
			"PF_RG32_SNORM", 
			/* Flags */
			PFF_NORMALIZED, 
			/* Channel count */
			2, 
			/* Channel format */
			PCF_R, PCF_G, PCF_NONE, PCF_NONE, 
			/* Channel type */
			PCT_SNORM, PCT_SNORM, PCT_NONE, PCT_NONE, 
			/* Channel bits */
			32, 32, 0, 0, 
			/* Masks and shifts */
			0, 0, 0, 0,
			0, 0, 0, 0
		}, 
		//---------------------------------------------------------------------
		{
			"PF_RG32_UINT", 
			/* Flags */
			0, 
			/* Channel count */
			2, 
			/* Channel format */
			PCF_R, PCF_G, PCF_NONE, PCF_NONE, 
			/* Channel type */
			PCT_UINT, PCT_UINT, PCT_NONE, PCT_NONE, 
			/* Channel bits */
			32, 32, 0, 0, 
			/* Masks and shifts */
			0, 0, 0, 0,
			0, 0, 0, 0
		}, 
		//---------------------------------------------------------------------
		{
			"PF_RG32_SINT", 
			/* Flags */
			0, 
			/* Channel count */
			2, 
			/* Channel format */
			PCF_R, PCF_G, PCF_NONE, PCF_NONE, 
			/* Channel type */
			PCT_SINT, PCT_SINT, PCT_NONE, PCT_NONE, 
			/* Channel bits */
			32, 32, 0, 0, 
			/* Masks and shifts */
			0, 0, 0, 0,
			0, 0, 0, 0
		}, 
		//---------------------------------------------------------------------
		{
			"PF_RG32_FLOAT", 
			/* Flags */
			PFF_FLOAT, 
			/* Channel count */
			2, 
			/* Channel format */
			PCF_R, PCF_G, PCF_NONE, PCF_NONE, 
			/* Channel type */
			PCT_FLOAT, PCT_FLOAT, PCT_NONE, PCT_NONE, 
			/* Channel bits */
			32, 32, 0, 0, 
			/* Masks and shifts */
			0, 0, 0, 0,
			0, 0, 0, 0
		}, 
		//---------------------------------------------------------------------
		{
			"PF_RGB32_UNORM", 
			/* Flags */
			PFF_NORMALIZED, 
			/* Channel count */
			3, 
			/* Channel format */
			PCF_R, PCF_G, PCF_B, PCF_NONE, 
			/* Channel type */
			PCT_UNORM, PCT_UNORM, PCT_UNORM, PCT_NONE, 
			/* Channel bits */
			32, 32, 32, 0, 
			/* Masks and shifts */
			0, 0, 0, 0,
			0, 0, 0, 0
		}, 
		//---------------------------------------------------------------------
		{
			"PF_RGB32_SNORM", 
			/* Flags */
			PFF_NORMALIZED, 
			/* Channel count */
			3, 
			/* Channel format */
			PCF_R, PCF_G, PCF_B, PCF_NONE, 
			/* Channel type */
			PCT_SNORM, PCT_SNORM, PCT_SNORM, PCT_NONE, 
			/* Channel bits */
			32, 32, 32, 0, 
			/* Masks and shifts */
			0, 0, 0, 0,
			0, 0, 0, 0
		}, 
		//---------------------------------------------------------------------
		{
			"PF_RGB32_UINT", 
			/* Flags */
			0, 
			/* Channel count */
			3, 
			/* Channel format */
			PCF_R, PCF_G, PCF_B, PCF_NONE, 
			/* Channel type */
			PCT_UINT, PCT_UINT, PCT_UINT, PCT_NONE, 
			/* Channel bits */
			32, 32, 32, 0, 
			/* Masks and shifts */
			0, 0, 0, 0,
			0, 0, 0, 0
		}, 
		//---------------------------------------------------------------------
		{
			"PF_RGB32_SINT", 
			/* Flags */
			0, 
			/* Channel count */
			3, 
			/* Channel format */
			PCF_R, PCF_G, PCF_B, PCF_NONE, 
			/* Channel type */
			PCT_SINT, PCT_SINT, PCT_SINT, PCT_NONE, 
			/* Channel bits */
			32, 32, 32, 0, 
			/* Masks and shifts */
			0, 0, 0, 0,
			0, 0, 0, 0
		}, 
		//---------------------------------------------------------------------
		{
			"PF_RGB32_FLOAT", 
			/* Flags */
			PFF_FLOAT, 
			/* Channel count */
			3, 
			/* Channel format */
			PCF_R, PCF_G, PCF_B, PCF_NONE, 
			/* Channel type */
			PCT_FLOAT, PCT_FLOAT, PCT_FLOAT, PCT_NONE, 
			/* Channel bits */
			32, 32, 32, 0, 
			/* Masks and shifts */
			0, 0, 0, 0,
			0, 0, 0, 0
		}, 
		//---------------------------------------------------------------------
		{
			"PF_RGBA32_UNORM", 
			/* Flags */
			PFF_NORMALIZED | PFF_ALPHA,
			/* Channel count */
			4, 
			/* Channel format */
			PCF_R, PCF_G, PCF_B, PCF_A, 
			/* Channel type */
			PCT_UNORM, PCT_UNORM, PCT_UNORM, PCT_UNORM, 
			/* Channel bits */
			32, 32, 32, 32, 
			/* Masks and shifts */
			0, 0, 0, 0,
			0, 0, 0, 0
		}, 
		//--------------------------------------------------------------------
		{
			"PF_RGBA32_SNORM", 
				/* Flags */
				PFF_NORMALIZED | PFF_ALPHA,
				/* Channel count */
				4, 
				/* Channel format */
				PCF_R, PCF_G, PCF_B, PCF_A, 
				/* Channel type */
				PCT_SNORM, PCT_SNORM, PCT_SNORM, PCT_SNORM, 
				/* Channel bits */
				32, 32, 32, 32, 
				/* Masks and shifts */
				0, 0, 0, 0,
				0, 0, 0, 0
		}, 
		//--------------------------------------------------------------------
		{
		"PF_RGBA32_UINT", 
			/* Flags */
			PFF_ALPHA, 
			/* Channel count */
			4, 
			/* Channel format */
			PCF_R, PCF_G, PCF_B, PCF_A, 
			/* Channel type */
			PCT_UINT, PCT_UINT, PCT_UINT, PCT_UINT, 
			/* Channel bits */
			32, 32, 32, 32, 
			/* Masks and shifts */
			0, 0, 0, 0,
			0, 0, 0, 0
		}, 
		//--------------------------------------------------------------------
		{
			"PF_RGBA32_SINT", 
			/* Flags */
			PFF_ALPHA, 
			/* Channel count */
			4, 
			/* Channel format */
			PCF_R, PCF_G, PCF_B, PCF_A, 
			/* Channel type */
			PCT_SINT, PCT_SINT, PCT_SINT, PCT_SINT, 
			/* Channel bits */
			32, 32, 32, 32, 
			/* Masks and shifts */
			0, 0, 0, 0,
			0, 0, 0, 0
		}, 
		//--------------------------------------------------------------------
		{
			"PF_RGBA32_FLOAT", 
			/* Flags */
			PFF_ALPHA | PFF_FLOAT, 
			/* Channel count */
			4, 
			/* Channel format */
			PCF_R, PCF_G, PCF_B, PCF_A, 
			/* Channel type */
			PCT_FLOAT, PCT_FLOAT, PCT_FLOAT, PCT_FLOAT, 
			/* Channel bits */
			32, 32, 32, 32, 
			/* Masks and shifts */
			0, 0, 0, 0,
			0, 0, 0, 0
		}, 
		//--------------------------------------------------------------------
		{
			"PF_D16_UNORM", 
			/* Flags */
			PFF_NORMALIZED | PFF_DEPTH | PFF_NATIVEENDIAN,
			/* Channel count */
			1, 
			/* Channel format */
			PCF_D, PCF_NONE, PCF_NONE, PCF_NONE, 
			/* Channel type */
			PCT_UNORM, PCT_NONE, PCT_NONE, PCT_NONE, 
			/* Channel bits */
			16, 0, 0, 0, 
			/* Masks and shifts */
			0xFF, 0, 0, 0,
			0, 0, 0, 0
		}, 
		//--------------------------------------------------------------------
		{
			"PF_D24_UNORM_S8_UINT", 
			/* Flags */
			PFF_NORMALIZED | PFF_DEPTH | PFF_STENCIL,
			/* Channel count */
			2, 
			/* Channel format */
			PCF_D, PCF_S, PCF_NONE, PCF_NONE, 
			/* Channel type */
			PCT_UNORM, PCT_UINT, PCT_NONE, PCT_NONE, 
			/* Channel bits */
			16, 8, 0, 0, 
			/* Masks and shifts */
			0, 0, 0, 0,
			0, 0, 0, 0
		}, 
		//--------------------------------------------------------------------
		{
			"PF_D32_FLOAT", 
			/* Flags */
			PFF_DEPTH | PFF_FLOAT, 
			/* Channel count */
			1, 
			/* Channel format */
			PCF_D, PCF_NONE, PCF_NONE, PCF_NONE, 
			/* Channel type */
			PCT_FLOAT, PCT_NONE, PCT_NONE, PCT_NONE, 
			/* Channel bits */
			32, 0, 0, 0, 
			/* Masks and shifts */
			0, 0, 0, 0,
			0, 0, 0, 0
		}, 
		//--------------------------------------------------------------------
		{
			"PF_BC1_UNORM", 
			/* Flags */
			PFF_NORMALIZED | PFF_COMPRESSED | PFF_ALPHA,
			/* Channel count */
			1, 
			/* Channel format */
			PCF_BC, PCF_NONE, PCF_NONE, PCF_NONE, 
			/* Channel type */
			PCT_UNORM, PCT_NONE, PCT_NONE, PCT_NONE, 
			/* Channel bits */
			4, 0, 0, 0, 
			/* Masks and shifts */
			0, 0, 0, 0,
			0, 0, 0, 0
		}, 
		//--------------------------------------------------------------------
		{
			"PF_BC1_UNORM_SRGB", 
			/* Flags */
			PFF_NORMALIZED | PFF_COMPRESSED | PFF_ALPHA, 
			/* Channel count */
			1, 
			/* Channel format */
			PCF_BC, PCF_NONE, PCF_NONE, PCF_NONE, 
			/* Channel type */
			PCT_UNORM_SRGB, PCT_NONE, PCT_NONE, PCT_NONE, 
			/* Channel bits */
			4, 0, 0, 0, 
			/* Masks and shifts */
			0, 0, 0, 0,
			0, 0, 0, 0
		}, 
		//--------------------------------------------------------------------
		{
			"PF_BC2_UNORM", 
			/* Flags */
			PFF_NORMALIZED | PFF_COMPRESSED | PFF_ALPHA,
			/* Channel count */
			1, 
			/* Channel format */
			PCF_BC, PCF_NONE, PCF_NONE, PCF_NONE, 
			/* Channel type */
			PCT_UNORM, PCT_NONE, PCT_NONE, PCT_NONE, 
			/* Channel bits */
			8, 0, 0, 0, 
			/* Masks and shifts */
			0, 0, 0, 0,
			0, 0, 0, 0
		}, 
		//--------------------------------------------------------------------
		{
			"PF_BC2_UNORM_SRGB", 
			/* Flags */
			PFF_NORMALIZED | PFF_COMPRESSED | PFF_ALPHA,
			/* Channel count */
			1, 
			/* Channel format */
			PCF_BC, PCF_NONE, PCF_NONE, PCF_NONE, 
			/* Channel type */
			PCT_UNORM_SRGB, PCT_NONE, PCT_NONE, PCT_NONE, 
			/* Channel bits */
			8, 0, 0, 0, 
			/* Masks and shifts */
			0, 0, 0, 0,
			0, 0, 0, 0
		}, 
		//--------------------------------------------------------------------
		{
			"PF_BC3_UNORM", 
			/* Flags */
			PFF_NORMALIZED | PFF_COMPRESSED | PFF_ALPHA,
			/* Channel count */
			1, 
			/* Channel format */
			PCF_BC, PCF_NONE, PCF_NONE, PCF_NONE, 
			/* Channel type */
			PCT_UNORM, PCT_NONE, PCT_NONE, PCT_NONE, 
			/* Channel bits */
			8, 0, 0, 0, 
			/* Masks and shifts */
			0, 0, 0, 0,
			0, 0, 0, 0
		}, 
		//--------------------------------------------------------------------
		{
			"PF_BC3_UNORM_SRGB", 
			/* Flags */
			PFF_NORMALIZED | PFF_COMPRESSED | PFF_ALPHA,
			/* Channel count */
			1, 
			/* Channel format */
			PCF_BC, PCF_NONE, PCF_NONE, PCF_NONE, 
			/* Channel type */
			PCT_UNORM_SRGB, PCT_NONE, PCT_NONE, PCT_NONE, 
			/* Channel bits */
			8, 0, 0, 0, 
			/* Masks and shifts */
			0, 0, 0, 0,
			0, 0, 0, 0
		}, 
		//--------------------------------------------------------------------
		{
			"PF_BC4_UNORM", 
			/* Flags */
			PFF_NORMALIZED | PFF_COMPRESSED | PFF_ALPHA,
			/* Channel count */
			1, 
			/* Channel format */
			PCF_BC, PCF_NONE, PCF_NONE, PCF_NONE, 
			/* Channel type */
			PCT_UNORM, PCT_NONE, PCT_NONE, PCT_NONE, 
			/* Channel bits */
			8, 0, 0, 0, 
			/* Masks and shifts */
			0, 0, 0, 0,
			0, 0, 0, 0
		}, 
		//--------------------------------------------------------------------
		{
			"PF_BC4_UNORM_SRGB", 
			/* Flags */
			PFF_NORMALIZED | PFF_COMPRESSED | PFF_ALPHA,
			/* Channel count */
			1, 
			/* Channel format */
			PCF_BC, PCF_NONE, PCF_NONE, PCF_NONE, 
			/* Channel type */
			PCT_UNORM_SRGB, PCT_NONE, PCT_NONE, PCT_NONE, 
			/* Channel bits */
			8, 0, 0, 0, 
			/* Masks and shifts */
			0, 0, 0, 0,
			0, 0, 0, 0
		}, 
		//--------------------------------------------------------------------
		{
			"PF_BC5_UNORM", 
			/* Flags */
			PFF_NORMALIZED | PFF_COMPRESSED | PFF_ALPHA,
			/* Channel count */
			1, 
			/* Channel format */
			PCF_BC, PCF_NONE, PCF_NONE, PCF_NONE, 
			/* Channel type */
			PCT_UNORM, PCT_NONE, PCT_NONE, PCT_NONE, 
			/* Channel bits */
			16, 0, 0, 0, 
			/* Masks and shifts */
			0, 0, 0, 0,
			0, 0, 0, 0
		}, 
		//--------------------------------------------------------------------
		{
			"PF_BC5_UNORM_SRGB", 
			/* Flags */
			PFF_NORMALIZED | PFF_COMPRESSED | PFF_ALPHA,
			/* Channel count */
			1, 
			/* Channel format */
			PCF_BC, PCF_NONE, PCF_NONE, PCF_NONE, 
			/* Channel type */
			PCT_UNORM_SRGB, PCT_NONE, PCT_NONE, PCT_NONE, 
			/* Channel bits */
			16, 0, 0, 0, 
			/* Masks and shifts */
			0, 0, 0, 0,
			0, 0, 0, 0
		}, 
		//--------------------------------------------------------------------
	};

	/**
	* Directly get the description record for provided pixel format. For debug builds,
	* this checks the bounds of fmt with an assertion.
	*/
	static inline const PixelFormatDesc& GetPixelFormatDesc(PixelFormat pixFmt)
	{
		const int ord = (int)pixFmt;
		LordAssert(ord >= 0 && ord < PF_COUNT);

		return g_pixFmtDescArr[ord];
	}

	//////////////////////////////////////////////////////////////////////////

	String PixelUtil::GetPixelFormatName(PixelFormat pixFmt)
	{
		return GetPixelFormatDesc(pixFmt).name;
	}

	uint PixelUtil::GetPixelBits(PixelFormat pixFmt)
	{
		PixelFormatDesc pixFmtDesc = GetPixelFormatDesc(pixFmt);
		return ((uint)pixFmtDesc.ch0Bits + (uint)pixFmtDesc.ch1Bits + (uint)pixFmtDesc.ch2Bits + (uint)pixFmtDesc.ch3Bits);
	}

	uint PixelUtil::GetPixelSize(PixelFormat pixFmt)
	{
		return (GetPixelBits(pixFmt)/8);
	}

	Dword PixelUtil::GetFlags(PixelFormat pixFmt)
	{
		return GetPixelFormatDesc(pixFmt).flags;
	}

	bool PixelUtil::IsNormalized(PixelFormat pixFmt)
	{
		return (GetFlags(pixFmt) & PFF_NORMALIZED) > 0;
	}

	bool PixelUtil::HasAlpha(PixelFormat pixFmt)
	{
		return (GetFlags(pixFmt) & PFF_ALPHA) > 0;
	}

	bool PixelUtil::IsFloatingPoint(PixelFormat pixFmt)
	{
		return (GetFlags(pixFmt) & PFF_FLOAT) > 0;
	}

	bool PixelUtil::IsCompressed(PixelFormat pixFmt)
	{
		return (GetFlags(pixFmt) & PFF_COMPRESSED) > 0;
	}

	bool PixelUtil::IsDepth(PixelFormat pixFmt)
	{
		return (GetFlags(pixFmt) & PFF_DEPTH) > 0;
	}

	bool PixelUtil::IsStencil(PixelFormat pixFmt)
	{
		return (GetFlags(pixFmt) & PFF_STENCIL) > 0;
	}

	bool PixelUtil::IsLuminance(PixelFormat pixFmt)
	{
		return (GetFlags(pixFmt) & PFF_LUMINANCE) > 0;
	}

	bool PixelUtil::IsNativeEndian(PixelFormat pixFmt)
	{
		return (GetFlags(pixFmt) & PFF_NATIVEENDIAN) > 0;
	}

	bool PixelUtil::IsAccessible(PixelFormat pixFmt)
	{
		if(pixFmt == PF_UNKNOWN)
			return false;

		Dword flags = GetFlags(pixFmt);
		return !((flags & PFF_COMPRESSED) || (flags & PFF_DEPTH));
	}

	PixelChannelType PixelUtil::GetChannelType(PixelFormat pixFmt, uint chIdx)
	{
		switch(chIdx)
		{
		case 0:		return GetPixelFormatDesc(pixFmt).ch0Type;
		case 1:		return GetPixelFormatDesc(pixFmt).ch1Type;
		case 2:		return GetPixelFormatDesc(pixFmt).ch2Type;
		case 3:		return GetPixelFormatDesc(pixFmt).ch3Type;
		default:	return PCT_NONE;
		}
	}

	uint PixelUtil::GetChannelCount(PixelFormat pixFmt)
	{
		return GetPixelFormatDesc(pixFmt).chCount;
	}

	uint PixelUtil::GetMemorySize(uint width, uint height, uint depth, PixelFormat pixFmt)
	{
		if(IsCompressed(pixFmt))
		{
			switch(pixFmt)
			{
				// DXT formats work by dividing the image into 4x4 blocks, then encoding each
				// 4x4 block with a certain number of bytes. 
			case PF_BC1_UNORM:
			case PF_BC1_UNORM_SRGB:
				return ((width+3)/4)*((height+3)/4)*8 * depth;

			case PF_BC2_UNORM:
			case PF_BC2_UNORM_SRGB:
			case PF_BC3_UNORM:
			case PF_BC3_UNORM_SRGB:
			case PF_BC4_UNORM:
			case PF_BC4_SNORM:
				return ((width+3)/4)*((height+3)/4)*16 * depth;

			case PF_BC5_UNORM:
			case PF_BC5_SNORM:
				return ((width+3)/4)*((height+3)/4)*32 * depth;

			default:
				return 0;
			}
		}
		else
		{
			return (width*height*depth*GetPixelBits(pixFmt)/8);
		}
	}

	uint PixelUtil::CalcLevelSize(uint width, uint height, uint depth, uint level, PixelFormat pixFmt)
	{
		uint memSize = GetMemorySize(width, height, depth, pixFmt);
		memSize >>= level;

		return memSize;
	}

	uint PixelUtil::CalcSurfaceSize(uint width, uint height, uint depth, uint mipmaps, PixelFormat pixFmt)
	{
		uint mips1 = mipmaps > 0? mipmaps : 1;
		uint memSize = GetMemorySize(width, height, depth, pixFmt);
		uint surSize = 0;
		for(uint i = 0; i < mips1; ++i)
		{
			memSize >>= i;
			surSize += memSize;
		}

		return surSize;
	}

	void PixelUtil::UnpackColor(Color& color, PixelFormat pixFmt, const void* pSrc)
	{
		float r, g, b, a;
		UnpackColor(r, g, b, a, pixFmt, pSrc);
		color.set(r, g, b, a);
	}

	void PixelUtil::UnpackColor(float& r, float& g, float& b, float& a, PixelFormat pixFmt, const void* pSrc)
	{
		const PixelFormatDesc &des = GetPixelFormatDesc(pixFmt);

		if(des.flags & PFF_NATIVEENDIAN)
		{
			// Shortcut for integer formats unpacking
			const uint value = Math::IntRead(pSrc, GetPixelSize(pixFmt));
			if(des.flags & PFF_LUMINANCE)
			{
				// Luminance format -- only rbits used
				r = g = b = Math::FixedToFloat((value & des.ch0Mask)>>des.ch0Shift, des.ch0Bits);
			}
			else
			{
				r = Math::FixedToFloat((value & des.ch0Mask)>>des.ch0Shift, des.ch0Bits);
				g = Math::FixedToFloat((value & des.ch1Mask)>>des.ch1Shift, des.ch1Bits);
				b = Math::FixedToFloat((value & des.ch2Mask)>>des.ch2Shift, des.ch2Bits);
			}
			if(des.flags & PFF_ALPHA)
			{
				a = Math::FixedToFloat((value & des.ch3Mask)>>des.ch3Shift, des.ch3Bits);
			}
			else
			{
				a = 1.0f; // No alpha, default a component to full
			}
		}
		else
		{
			switch(pixFmt)
			{
			case PF_R32_FLOAT:
				{
					r = g = b = ((float*)pSrc)[0];
					a = 1.0f;
				} break;
			case PF_RG32_FLOAT:
				{
					r = ((float*)pSrc)[0];
					g = b = ((float*)pSrc)[1];
					a = 1.0f;
				} break;
			case PF_RGB32_FLOAT:
				{
					r = ((float*)pSrc)[0];
					g = ((float*)pSrc)[1];
					b = ((float*)pSrc)[2];
					a = 1.0f;
				} break;
			case PF_RGBA32_FLOAT:
				{
					r = ((float*)pSrc)[0];
					g = ((float*)pSrc)[1];
					b = ((float*)pSrc)[2];
					a = ((float*)pSrc)[3];
				} break;
			case PF_R16_FLOAT:
				{
					r = g = b = Math::HalfToFloat(((ui16*)pSrc)[0]);
					a = 1.0f;
				} break;
			case PF_RG16_FLOAT:
				{
					g = Math::HalfToFloat(((ui16*)pSrc)[0]);
					r = b = Math::HalfToFloat(((ui16*)pSrc)[1]);
					a = 1.0f;
				} break;
			case PF_RGB16_FLOAT:
				{
					r = Math::HalfToFloat(((ui16*)pSrc)[0]);
					g = Math::HalfToFloat(((ui16*)pSrc)[1]);
					b = Math::HalfToFloat(((ui16*)pSrc)[2]);
					a = 1.0f;
				} break;
			case PF_RGBA16_FLOAT:
				{
					r = Math::HalfToFloat(((ui16*)pSrc)[0]);
					g = Math::HalfToFloat(((ui16*)pSrc)[1]);
					b = Math::HalfToFloat(((ui16*)pSrc)[2]);
					a = Math::HalfToFloat(((ui16*)pSrc)[3]);
				} break;
			case PF_RGB32_UNORM:
				{
					r = Math::FixedToFloat(((ui32*)pSrc)[0], 32);
					g = Math::FixedToFloat(((ui32*)pSrc)[1], 32);
					b = Math::FixedToFloat(((ui32*)pSrc)[2], 32);
					a = 1.0f;
				} break;
			case PF_RGB32_SNORM:
				{
					r = Math::FixedToFloat(((i32*)pSrc)[0], 32);
					g = Math::FixedToFloat(((i32*)pSrc)[1], 32);
					b = Math::FixedToFloat(((i32*)pSrc)[2], 32);
					a = 1.0f;
				} break;
			case PF_RGB32_UINT:
				{
					r = (float)(((ui32*)pSrc)[0]);
					g = (float)(((ui32*)pSrc)[1]);
					b = (float)(((ui32*)pSrc)[2]);
					a = 1.0f;
				} break;
			case PF_RGB32_SINT:
				{
					r = (float)(((i32*)pSrc)[0]);
					g = (float)(((i32*)pSrc)[1]);
					b = (float)(((i32*)pSrc)[2]);
					a = 1.0f;
				} break;
			case PF_RGBA32_UNORM:
				{
					r = Math::FixedToFloat(((ui32*)pSrc)[0], 32);
					g = Math::FixedToFloat(((ui32*)pSrc)[1], 32);
					b = Math::FixedToFloat(((ui32*)pSrc)[2], 32);
					a = Math::FixedToFloat(((ui32*)pSrc)[3], 32);
				} break;
			case PF_RGBA32_SNORM:
				{
					r = Math::FixedToFloat(((i32*)pSrc)[0], 32);
					g = Math::FixedToFloat(((i32*)pSrc)[1], 32);
					b = Math::FixedToFloat(((i32*)pSrc)[2], 32);
					a = Math::FixedToFloat(((i32*)pSrc)[3], 32);
				} break;
			case PF_RGBA32_UINT:
				{
					r = (float)(((ui32*)pSrc)[0]);
					g = (float)(((ui32*)pSrc)[1]);
					b = (float)(((ui32*)pSrc)[2]);
					a = (float)(((ui32*)pSrc)[3]);
				} break;
			case PF_RGBA32_SINT:
				{
					r = (float)(((i32*)pSrc)[0]);
					g = (float)(((i32*)pSrc)[1]);
					b = (float)(((i32*)pSrc)[2]);
					a = (float)(((i32*)pSrc)[3]);
				} break;
			case PF_RGB16_UNORM:
				{
					r = Math::FixedToFloat(((ui16*)pSrc)[0], 16);
					g = Math::FixedToFloat(((ui16*)pSrc)[1], 16);
					b = Math::FixedToFloat(((ui16*)pSrc)[2], 16);
					a = 1.0f;
				} break;
			case PF_RGB16_SNORM:
				{
					r = Math::FixedToFloat(((i16*)pSrc)[0], 16);
					g = Math::FixedToFloat(((i16*)pSrc)[1], 16);
					b = Math::FixedToFloat(((i16*)pSrc)[2], 16);
					a = 1.0f;
				} break;
			case PF_RGB16_UINT:
				{
					r = (float)(((ui16*)pSrc)[0]);
					g = (float)(((ui16*)pSrc)[1]);
					b = (float)(((ui16*)pSrc)[2]);
					a = 1.0f;
				} break;
			case PF_RGB16_SINT:
				{
					r = (float)(((i16*)pSrc)[0]);
					g = (float)(((i16*)pSrc)[1]);
					b = (float)(((i16*)pSrc)[2]);
					a = 1.0f;
				} break;
			case PF_RGBA16_UNORM:
				{
					r = Math::FixedToFloat(((ui16*)pSrc)[0], 16);
					g = Math::FixedToFloat(((ui16*)pSrc)[1], 16);
					b = Math::FixedToFloat(((ui16*)pSrc)[2], 16);
					a = Math::FixedToFloat(((ui16*)pSrc)[3], 16);
				} break;
			case PF_RGBA16_SNORM:
				{
					r = Math::FixedToFloat(((i16*)pSrc)[0], 16);
					g = Math::FixedToFloat(((i16*)pSrc)[1], 16);
					b = Math::FixedToFloat(((i16*)pSrc)[2], 16);
					a = Math::FixedToFloat(((i16*)pSrc)[3], 16);
				} break;
			case PF_RGBA16_UINT:
				{
					r = (float)(((ui16*)pSrc)[0]);
					g = (float)(((ui16*)pSrc)[1]);
					b = (float)(((ui16*)pSrc)[2]);
					a = (float)(((ui16*)pSrc)[3]);
				} break;
			case PF_RGBA16_SINT:
				{
					r = (float)(((i16*)pSrc)[0]);
					g = (float)(((i16*)pSrc)[1]);
					b = (float)(((i16*)pSrc)[2]);
					a = (float)(((i16*)pSrc)[3]);
				} break;
			default:
				{
					// Not yet supported
					LordLogError("unpack from PixelFormat [%s] not implemented", GetPixelFormatName(pixFmt).c_str());
				} break;
			}
		}
	}

	void PixelUtil::PackColor(const Color& color, PixelFormat pixFmt, void* pDest)
	{
		PackColor((float)color.r, (float)color.g, (float)color.b, (float)color.a, pixFmt, pDest);
	}

	void PixelUtil::PackColor(float r, float g, float b, float a, PixelFormat pixFmt, void* pDest)
	{
		// Catch-it-all here
		const PixelFormatDesc &des = GetPixelFormatDesc(pixFmt);

		if(des.flags & PFF_NATIVEENDIAN)
		{
			// Do the packing
			const uint value =	((Math::FloatToFixed(r, des.ch0Bits)<<des.ch0Shift) & des.ch0Mask) |
				((Math::FloatToFixed(g, des.ch1Bits)<<des.ch1Shift) & des.ch1Mask) |
				((Math::FloatToFixed(b, des.ch2Bits)<<des.ch2Shift) & des.ch2Mask) |
				((Math::FloatToFixed(a, des.ch3Bits)<<des.ch3Shift) & des.ch3Mask);

			// And write to memory
			Math::IntWrite(pDest, GetPixelSize(pixFmt), value);
		}
		else
		{
			switch(pixFmt)
			{
			case PF_R32_FLOAT:
				{
					((float*)pDest)[0] = r;
				} break;
			case PF_RG32_FLOAT:
				{
					((float*)pDest)[0] = g;
					((float*)pDest)[1] = r;
				} break;
			case PF_RGB32_FLOAT:
				{
					((float*)pDest)[0] = r;
					((float*)pDest)[1] = g;
					((float*)pDest)[2] = b;
				} break;
			case PF_RGBA32_FLOAT:
				{
					((float*)pDest)[0] = r;
					((float*)pDest)[1] = g;
					((float*)pDest)[2] = b;
					((float*)pDest)[3] = a;
				} break;
			case PF_R16_FLOAT:
				{
					((ui16*)pDest)[0] = Math::FloatToHalf(r);
				} break;
			case PF_RG16_FLOAT:
				{
					((ui16*)pDest)[0] = Math::FloatToHalf(g);
					((ui16*)pDest)[1] = Math::FloatToHalf(r);
				} break;
			case PF_RGB16_FLOAT:
				{
					((ui16*)pDest)[0] = Math::FloatToHalf(r);
					((ui16*)pDest)[1] = Math::FloatToHalf(g);
					((ui16*)pDest)[2] = Math::FloatToHalf(b);
				} break;
			case PF_RGBA16_FLOAT:
				{
					((ui16*)pDest)[0] = Math::FloatToHalf(r);
					((ui16*)pDest)[1] = Math::FloatToHalf(g);
					((ui16*)pDest)[2] = Math::FloatToHalf(b);
					((ui16*)pDest)[3] = Math::FloatToHalf(a);
				} break;
			case PF_RGB32_UNORM:
				{
					((ui32*)pDest)[0] = (ui32)Math::FloatToFixed(r, 32);
					((ui32*)pDest)[1] = (ui32)Math::FloatToFixed(g, 32);
					((ui32*)pDest)[2] = (ui32)Math::FloatToFixed(b, 32);
				} break;
			case PF_RGB32_SNORM:
				{
					((i32*)pDest)[0] = (i32)Math::FloatToFixed(r, 32);
					((i32*)pDest)[1] = (i32)Math::FloatToFixed(g, 32);
					((i32*)pDest)[2] = (i32)Math::FloatToFixed(b, 32);
				} break;
			case PF_RGB32_UINT:
				{
					((ui32*)pDest)[0] = (ui32)r;
					((ui32*)pDest)[1] = (ui32)g;
					((ui32*)pDest)[2] = (ui32)b;
				} break;
			case PF_RGB32_SINT:
				{
					((i32*)pDest)[0] = (i32)r;
					((i32*)pDest)[1] = (i32)g;
					((i32*)pDest)[2] = (i32)b;
				} break;
			case PF_RGBA32_UNORM:
				{
					((ui32*)pDest)[0] = (ui32)Math::FloatToFixed(r, 32);
					((ui32*)pDest)[1] = (ui32)Math::FloatToFixed(g, 32);
					((ui32*)pDest)[2] = (ui32)Math::FloatToFixed(b, 32);
					((ui32*)pDest)[3] = (ui32)Math::FloatToFixed(a, 32);
				} break;
			case PF_RGBA32_SNORM:
				{
					((i32*)pDest)[0] = (i32)Math::FloatToFixed(r, 32);
					((i32*)pDest)[1] = (i32)Math::FloatToFixed(g, 32);
					((i32*)pDest)[2] = (i32)Math::FloatToFixed(b, 32);
					((i32*)pDest)[3] = (i32)Math::FloatToFixed(a, 32);
				} break;
			case PF_RGBA32_UINT:
				{
					((ui32*)pDest)[0] = (ui32)r;
					((ui32*)pDest)[1] = (ui32)g;
					((ui32*)pDest)[2] = (ui32)b;
					((ui32*)pDest)[3] = (ui32)a;
				} break;
			case PF_RGBA32_SINT:
				{
					((i32*)pDest)[0] = (i32)r;
					((i32*)pDest)[1] = (i32)g;
					((i32*)pDest)[2] = (i32)b;
					((i32*)pDest)[3] = (i32)a;
				} break;
			case PF_RGB16_UNORM:
				{
					((ui16*)pDest)[0] = (ui16)Math::FloatToFixed(r, 16);
					((ui16*)pDest)[1] = (ui16)Math::FloatToFixed(g, 16);
					((ui16*)pDest)[2] = (ui16)Math::FloatToFixed(b, 16);
				} break;
			case PF_RGB16_SNORM:
				{
					((ui16*)pDest)[0] = (ui16)Math::FloatToFixed(r, 16);
					((ui16*)pDest)[1] = (ui16)Math::FloatToFixed(g, 16);
					((ui16*)pDest)[2] = (ui16)Math::FloatToFixed(b, 16);
				} break;
			case PF_RGB16_UINT:
				{
					((ui16*)pDest)[0] = (ui16)r;
					((ui16*)pDest)[1] = (ui16)g;
					((ui16*)pDest)[2] = (ui16)b;
				} break;
			case PF_RGB16_SINT:
				{
					((i16*)pDest)[0] = (i16)r;
					((i16*)pDest)[1] = (i16)g;
					((i16*)pDest)[2] = (i16)b;
				} break;
			case PF_RGBA16_UNORM:
				{
					((ui16*)pDest)[0] = (ui16)Math::FloatToFixed(r, 16);
					((ui16*)pDest)[1] = (ui16)Math::FloatToFixed(g, 16);
					((ui16*)pDest)[2] = (ui16)Math::FloatToFixed(b, 16);
					((ui16*)pDest)[3] = (ui16)Math::FloatToFixed(a, 16);
				} break;
			case PF_RGBA16_SNORM:
				{
					((i16*)pDest)[0] = (i16)Math::FloatToFixed(r, 16);
					((i16*)pDest)[1] = (i16)Math::FloatToFixed(g, 16);
					((i16*)pDest)[2] = (i16)Math::FloatToFixed(b, 16);
					((i16*)pDest)[3] = (i16)Math::FloatToFixed(a, 16);
				} break;
			case PF_RGBA16_UINT:
				{
					((ui16*)pDest)[0] = (ui16)r;
					((ui16*)pDest)[1] = (ui16)g;
					((ui16*)pDest)[2] = (ui16)b;
					((ui16*)pDest)[3] = (ui16)a;
				} break;
			case PF_RGBA16_SINT:
				{
					((i16*)pDest)[0] = (i16)r;
					((i16*)pDest)[1] = (i16)g;
					((i16*)pDest)[2] = (i16)b;
					((i16*)pDest)[3] = (i16)a;
				} break;
			default:
				// Not yet supported
				LordLogError("unpack from PixelFormat [%s] not implemented", GetPixelFormatName(pixFmt).c_str());
				break;
			}
		}
	}

	Color PixelUtil::ConvertColor(const Color& color, PixelFormat pixFmt)
	{
		Color retColor;

#if(FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR)
		switch(pixFmt)
		{
		case PF_BGR8_UNORM:	
		case PF_BGRA8_UNORM:	
			{
				retColor.r = color.b;
				retColor.g = color.g;
				retColor.b = color.r;
				retColor.a = color.a;
			} break;
		default:
			retColor = color;
		}
#else
		retColor = color;
#endif

		return retColor;
	}

	int PixelUtil::optionalPO2(int n)
	{
		--n;
		n|=n>>16;
		n|=n>>8;
		n|=n>>4;
		n|=n>>2;
		n|=n>>1;
		++n;
		return n;
	}

	bool PixelUtil::isOptionalPO2(int n)
	{
		int n2 = optionalPO2(n);
		return n2 == n;
	}

	void PixelUtil::BulkPixelConversion(void* pSrcData, PixelFormat srcPixFmt, void* pDestData, PixelFormat dstPixFmt, uint count)
	{
		PixelBox src(count, 1, 1, srcPixFmt, pSrcData);
		PixelBox dst(count, 1, 1, dstPixFmt, pDestData);

		BulkPixelConversion(src, dst);
	}

	void PixelUtil::BulkPixelConversion(const PixelBox& src, const PixelBox& dst)
	{
		assert(	src.getWidth() == dst.getWidth() &&
			src.getHeight() == dst.getHeight() &&
			src.getDepth() == dst.getDepth());

		// Check for compressed formats, we don't support decompression, compression or recoding
		if(PixelUtil::IsCompressed(src.pixFmt) || PixelUtil::IsCompressed(dst.pixFmt))
		{
			if(src.pixFmt == dst.pixFmt)
			{
				memcpy(dst.pData, src.pData, src.getConsecutiveSize());
				return ;
			}
			else
			{
				LordLogError("This method can not be used to compress or decompress images");
				return ;
			}
		}

		// The easy case
		if(src.pixFmt == dst.pixFmt)
		{
			// Everything consecutive?
			if(src.isConsecutive() && dst.isConsecutive())
			{
				memcpy(dst.pData, src.pData, src.getConsecutiveSize());
				return ;
			}

			const uint srcPixelSize = PixelUtil::GetPixelSize(src.pixFmt);
			const uint dstPixelSize = PixelUtil::GetPixelSize(dst.pixFmt);
			ui8* srcptr = static_cast<ui8*>(src.pData) + (src.left + src.top * src.rowPitch + src.front * src.slicePitch) * srcPixelSize;
			ui8* dstptr = static_cast<ui8*>(dst.pData) + (dst.left + dst.top * dst.rowPitch + dst.front * dst.slicePitch) * dstPixelSize;

			// Calculate pitches+skips in bytes
			const uint srcRowPitchBytes = src.rowPitch*srcPixelSize;
			//const uint srcRowSkipBytes = src.getRowSkip()*srcPixelSize;
			const uint srcSliceSkipBytes = src.getSliceSkip()*srcPixelSize;

			const uint dstRowPitchBytes = dst.rowPitch*dstPixelSize;
			//const uint dstRowSkipBytes = dst.getRowSkip()*dstPixelSize;
			const uint dstSliceSkipBytes = dst.getSliceSkip()*dstPixelSize;

			// Otherwise, copy per row
			const uint rowSize = src.getWidth() * srcPixelSize;
			for(uint z = src.front; z < src.back; ++z)
			{
				for(uint y = src.top; y < src.bottom; ++y)
				{
					memcpy(dstptr, srcptr, rowSize);
					srcptr += srcRowPitchBytes;
					dstptr += dstRowPitchBytes;
				}

				srcptr += srcSliceSkipBytes;
				dstptr += dstSliceSkipBytes;
			}

			return ;
		}

		// Is there a specialized, inlined, conversion?
		if(DoOptimizedConversion(src, dst))
		{
			// If so, good
			return ;
		}

		const uint srcPixelSize = PixelUtil::GetPixelSize(src.pixFmt);
		const uint dstPixelSize = PixelUtil::GetPixelSize(dst.pixFmt);
		ui8* srcptr = static_cast<ui8*>(src.pData) + (src.left + src.top * src.rowPitch + src.front * src.slicePitch) * srcPixelSize;
		ui8* dstptr = static_cast<ui8*>(dst.pData) + (dst.left + dst.top * dst.rowPitch + dst.front * dst.slicePitch) * dstPixelSize;

		// Calculate pitches+skips in bytes
		const uint srcRowSkipBytes = src.getRowSkip()*srcPixelSize;
		const uint srcSliceSkipBytes = src.getSliceSkip()*srcPixelSize;
		const uint dstRowSkipBytes = dst.getRowSkip()*dstPixelSize;
		const uint dstSliceSkipBytes = dst.getSliceSkip()*dstPixelSize;

		// The brute force fallback
		float r = 0, g = 0, b = 0, a = 1;
		for(uint z = src.front; z < src.back; ++z)
		{
			for(uint y = src.top; y < src.bottom; ++y)
			{
				for(uint x = src.left; x < src.right; ++x)
				{
					UnpackColor(r, g, b, a, src.pixFmt, srcptr);
					PackColor(r, g, b, a, dst.pixFmt, dstptr);
					srcptr += srcPixelSize;
					dstptr += dstPixelSize;
				}
				srcptr += srcRowSkipBytes;
				dstptr += dstRowSkipBytes;
			}
			srcptr += srcSliceSkipBytes;
			dstptr += dstSliceSkipBytes;
		}
	}

}
