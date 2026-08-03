#include "Core.h"
#include "InputGeometryData.h"

namespace LORD
{
	InputGeometryData::InputGeometryData()
		: m_verts(NULL)
		, m_tris(NULL)
		, m_vertCount(0)
		, m_triCount(0)
	{
		m_box.vMin = Vector3::ZERO;
		m_box.vMax = Vector3::ZERO;
	}

	InputGeometryData::~InputGeometryData()
	{
		destroyAll();
	}

	void InputGeometryData::addVertex( float x, float y, float z, int& cap )
	{
		if (m_vertCount+1 > cap)
		{
			cap = !cap ? 8 : cap*2;
			float* nv = (float*)LordMalloc(sizeof(float)*cap*3);
			if (m_vertCount)
				memcpy(nv, m_verts, m_vertCount*3*sizeof(float));
			LordFree(m_verts);
			m_verts = nv;
		}
		float* dst = &m_verts[m_vertCount*3];
		*dst++ = x;
		*dst++ = y;
		*dst++ = z;
		m_vertCount++;

		m_box.addPoint(Vector3(x, y, z));
	}

	void InputGeometryData::addTriangle( int a, int b, int c, int& cap )
	{
		if (m_triCount+1 > cap)
		{
			cap = !cap ? 8 : cap*2;
			int* nv = (int*)LordMalloc(sizeof(int)*cap*3);
			if (m_triCount)
				memcpy(nv, m_tris, m_triCount*3*sizeof(int));
			LordFree(m_tris);
			m_tris = nv;
		}
		int* dst = &m_tris[m_triCount*3];
		*dst++ = a;
		*dst++ = b;
		*dst++ = c;
		m_triCount++;
	}

	void InputGeometryData::destroyAll()
	{
		LordFree(m_verts);
		LordFree(m_tris);
		m_verts = NULL;
		m_tris = NULL;
		m_vertCount = 0;
		m_triCount = 0;
	}

}
