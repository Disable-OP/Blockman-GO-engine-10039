/********************************************************************
filename: 	TessHeader.h
file path:	dev\engine\Src\Core\Tessellator\

version:	1
author:		ajohn
company:	supernano
datetime:   2016-11-29
*********************************************************************/

#ifndef __TESSELLATOR_HEADER_H__
#define __TESSELLATOR_HEADER_H__

#include "Math/MathLib.h"
#include "Image/Color.h"

namespace LORD
{

class TessSystem;						
class TessMaterial;					
class TessRenderable;
class TessRenderQueue;

enum TESS_RENDER_PASS
{
	TRP_INVALID = -1,
	
	TRP_SOLID,
	TRP_SOLID_NOMIP,
	TRP_SOLID_UV,
	TRP_TRANSPARENT_3D,		// need write Z first.
	TRP_TRANSPARENT_3DUV,   // need write Z first. for moving water.
	TRP_TRANSPARENT_2D,
	TRP_TRANSPARENT_UV,
	TRP_TRANSPARENT_HL,		// high lightting.

	TRP_COUNT,
};

enum FX_RENDER_PASS
{
	FRP_INVALID = -1,

	FRP_DEFAULT,
	FRP_ALPHA_CHANEL,

	FRP_COUNT,
};

enum SKINED_RENDER_PASS
{
	SRP_INVALID = -1,

	SRP_SOLID,
	SRP_SOLID_LIGHT,
	SRP_SOLID_DFACE,
	SRP_TRANSPARENT,

	SRP_SOLID_NODEPTH,
	SRP_ENCHANT,
	SRP_ENCHANT_NODEPTH,

	SRP_COUNT,
};

enum ITEM_RENDER_PASS
{
	IRP_INVALID = -1,

	IRP_SOLID,
	IRP_TRANSPARENT,

	IRP_SOLID_NODEPTH,
	IRP_ENCHANT,
	IRP_ENCHANT_NODEPTH,

	IRP_COUNT,
};

enum BATCH_RENDER_PASS
{
	STRP_INVALID = -1,

	STRP_SOLID,
	STRP_TRANSPARENT,

	STRP_COUNT,
};

enum LINE_RENDER_PASS
{
	LRP_INVALID = -1,

	LRP_SELECTED,
	LRP_VIRSUALHELP,

	LRP_COUNT,
};

struct TessRasterize
{
	TessRasterize()
	{
		position[0] = 0;
		position[1] = 0;
		position[2] = 0;
	}

	TessRasterize(const Vector3& v)
	{
		position[0] = (char)(v.x + 0.5f);
		position[1] = (char)(v.y + 0.5f);
		position[2] = (char)(v.z + 0.5f);
	}

	char position[3];
	void SetPosition(float x, float y, float z)
	{
		position[0] = (char)(x + 0.5f);
		position[1] = (char)(y + 0.5f);
		position[2] = (char)(z + 0.5f);
	}

	Vector3 GetPosition() const
	{
		Vector3 pos;
		pos.x = position[0];
		pos.y = position[1];
		pos.z = position[2];
		return pos;
	}
};

struct TessVertexFmt
{
	inline void setEx(const Vector3& start, float x, float y, float z, ui32 c, int luv, float u, float v)
	{
		SetPosition(x - start.x, y - start.y, z - start.z);
		// SetNormal(normal.x, normal.y, normal.z);
		// SetNormal(0.0f, 1.0f, 0.0f);
		SetLightColor(c);
		SetLightMapUV(luv);
		SetTexcoord(u, v);
	}
	
	// Position(xyz) vec4
	static const int PX = 0;
	static const int PY = 1;
	static const int PZ = 2;
	static const int LSKY = 3;

	// Normal(xyz) vec4
	//static const int NX = 4;
	//static const int NY = 5;
	//static const int NZ = 6;
	//static const int LBLOCK = 7;

	// Texcoord0(xy) vec2
	static const int TX0 = 4;
	static const int TY0 = 6;

	// Light(xyz) vec4
	static const int LR = 8;
	static const int LG = 9;
	static const int LB = 10;
	static const int LBLOCK = 11;
	static const int IDX = 11;

	unsigned char packs[12];

	// [-1, 1]
	void SetPosition(float x, float y, float z)
	{
		int nx = (int)(x * 15.0f + 0.5f);
		int ny = (int)(y * 15.0f + 0.5f);
		int nz = (int)(z * 15.0f + 0.5f);

		if (nx < 0x00) nx = 0x00;
		if (nx > 0xF0) nx = 0xF0;

		if (ny < 0x00) ny = 0x00;
		if (ny > 0xF0) ny = 0xF0;

		if (nz < 0x00) nz = 0x00;
		if (nz > 0xF0) nz = 0xF0;

		packs[PX] = nx;
		packs[PY] = ny;
		packs[PZ] = nz;
	}

	void setPositionEx(float x, float y, float z)
	{
		int nx = (int)((x / 8.0f + 1.0f) * 127.0f + 0.5f);
		int ny = (int)((y / 8.0f + 1.0f) * 127.0f + 0.5f);
		int nz = (int)((z / 8.0f + 1.0f) * 127.0f + 0.5f);

		if (nx < 0x00) nx = 0x00;
		if (nx > 0xff) nx = 0xff;

		if (ny < 0x00) ny = 0x00;
		if (ny > 0xff) ny = 0xff;

		if (nz < 0x00) nz = 0x00;
		if (nz > 0xff) nz = 0xff;

		packs[PX] = nx;
		packs[PY] = ny;
		packs[PZ] = nz;
	}

	void setPositionEx2(float x, float y, float z)
	{
		int nx = (int)((x / 2.0f + 1.0f) * 127.0f + 0.5f);
		int ny = (int)((y / 2.0f + 1.0f) * 127.0f + 0.5f);
		int nz = (int)((z / 2.0f + 1.0f) * 127.0f + 0.5f);

		if (nx < 0x00) nx = 0x00;
		if (nx > 0xff) nx = 0xff;

		if (ny < 0x00) ny = 0x00;
		if (ny > 0xff) ny = 0xff;

		if (nz < 0x00) nz = 0x00;
		if (nz > 0xff) nz = 0xff;

		packs[PX] = nx;
		packs[PY] = ny;
		packs[PZ] = nz;
	}

	void SetPosition(const Vector3& v)
	{
		SetPosition(v.x, v.y, v.z);
	}

	Vector3 GetPosition()
	{
		float x = packs[PX] / 15.f;
		float y = packs[PY] / 15.f;
		float z = packs[PZ] / 15.f;
		return Vector3(x, y, z);
	}

	Vector3 GetPositionEx()
	{
		float x = (packs[PX] / 127.0f - 1.0f) * 8.f;
		float y = (packs[PY] / 127.0f - 1.0f) * 8.f;
		float z = (packs[PZ] / 127.0f - 1.0f) * 8.f;
		return Vector3(x, y, z);
	}

	// [-1, 1]
	void SetTexcoord(float u, float v)
	{
		int nu = (int)(u * 2048.0f + 0.5f);
		int nv = (int)(v * 2048.0f + 0.5f);

		if (nu < 0x000) nu = 0x000;
		if (nu > 0x800) nu = 0x800;

		if (nv < 0x000) nv = 0x000;
		if (nv > 0x800) nv = 0x800;

		*(unsigned short *)(&packs[TX0]) = nu;
		*(unsigned short *)(&packs[TY0]) = nv;
	}

	Vector2 GetTexcoord()
	{
		int u = *(unsigned short *)(&packs[TX0]);
		int v = *(unsigned short *)(&packs[TY0]);

		return Vector2(u / 2048.f, v / 2048.f);
	}

	void SetLightColor(unsigned int c)
	{
		*(ui32*)(packs + LR) = c;
	}

	ui32 GetLightColor()
	{
		return *(ui32*)(packs + LR);
	}

	Color GetLightColorEx()
	{
		Color ret;
		ret.r = packs[LR] / 255.f;
		ret.g = packs[LB] / 255.f;
		ret.b = packs[LB] / 255.f;
		ret.a = 1.f;
		return ret;
	}

	void SetLightMapUV(unsigned int uv)
	{
		int u = (uv & 0x000000ff);
		int v = (uv & 0x00ff0000) >> 16;
		packs[LSKY] = v;
		packs[LBLOCK] = u;
	}

	ui32 GetLightMapUV()
	{
		return (ui32(packs[LBLOCK])) | (ui32(packs[LSKY]) << 16);
	}

	float GetSkyLight()
	{
		return float(packs[LSKY]) / 255.f;
	}

	float GetBlockLight()
	{
		return float(packs[LBLOCK]) / 255.f;
	}
};

static void AutoCaculateQuadNormal(TessVertexFmt* vertex)
{
	// not needed
}

struct SkinedVertexFmt : public TessVertexFmt
{
	inline void setEx2(const Vector3& pos, ui32 color, float u, float v, int idx)
	{
		setPositionEx(pos.x, pos.y, pos.z);
		SetLightColor(color);
		SetTexcoord(u, v);
		setIndex(idx);
	}

	void setIndex(int idx)
	{
		packs[IDX] = idx;
	}

	int getIndex()
	{
		return (int)packs[IDX];
	}
};

struct FxVertexFmt
{
	inline void set(float x, float y, float z, ui32 c, int luv, float u, float v)
	{
		pos.x = x;
		pos.y = y;
		pos.z = z;
		color = c;
		lightmapUV = luv;
		uv.x = u;
		uv.y = v;
	}

	Vector3 pos;
	ui32 color;
	Vector2 uv;
	int lightmapUV;
};

struct LineVertexFmt
{
	inline void set(float x, float y, float z, ui32 c)
	{
		pos.x = x;
		pos.y = y;
		pos.z = z;
		color = c;
	}
	Vector3 pos;
	ui32 color;
};


struct ItemVertexFmt : public TessVertexFmt
{
	inline void setEx3(const Vector3& pos, ui32 color, float u, float v)
	{
		setPositionEx2(pos.x, pos.y, pos.z);
		// SetNormal(normal.x, normal.y, normal.z);
		// SetNormal(normal.x, normal.y, normal.z);
		SetLightColor(color);
		SetTexcoord(u, v);
	}
};

struct BatchVertexFmt
{
	Vector3 pos;
	Vector2 uv;
	ui32    color;
};

struct FontVertexFmt
{
	inline void set(const Vector3& p, ui32 c, float u, float v)
	{
		pos = p;
		color = c;
		uv.x = u;
		uv.y = v;
	}
	Vector3 pos;
	ui32    color;
	Vector2 uv;
};

}

#endif