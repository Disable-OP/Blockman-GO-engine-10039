/********************************************************************
filename: 	Triangle.h
file path:	dev\engine\Src\Core\Geom\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_TRI_H__
#define __LORD_TRI_H__

#include "Math/MathLib.h"

namespace LORD
{
	class LORD_CORE_API Triangle
	{
	public:
		Vector3	v0;		//!< Vertex 0
		Vector3	v1;		//!< Vertex 1
		Vector3	v2;		//!< Vertex 2

	public:
		inline Triangle()
		{
		}

		inline Triangle(const Vector3& vert0, const Vector3& vert1, const Vector3& vert2):
		v0(vert0),
			v1(vert1),
			v2(vert2)
		{
		}
	public:
		void set(const Vector3& vert0, const Vector3& vert1, const Vector3& vert2);

		Vector3 GetNormal() const;
	};
}

#endif