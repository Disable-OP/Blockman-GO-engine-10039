#ifndef __INPUT_GEOMETRY_DATA__
#define __INPUT_GEOMETRY_DATA__

#include "Memory/LordMemory.h"
#include "Geom/Box.h"

namespace LORD
{
	class LORD_CORE_API InputGeometryData: public ObjectAlloc
	{
	public:
		InputGeometryData();

		~InputGeometryData();

		inline const float* getVerts() const { return m_verts; }
		inline const int* getTris() const { return m_tris; }
		inline int getVertCount() const { return m_vertCount; }
		inline int getTriCount() const { return m_triCount; }

		void addVertex(float x, float y, float z, int& cap);
		void addTriangle(int a, int b, int c, int& cap);

		void destroyAll();

		Box&		getAABB() { return m_box; }

	private:

		// vertex buffer and index buffer
		float*			m_verts;
		int*			m_tris;
		int				m_vertCount;
		int				m_triCount;
		Box				m_box;

	};

}

#endif