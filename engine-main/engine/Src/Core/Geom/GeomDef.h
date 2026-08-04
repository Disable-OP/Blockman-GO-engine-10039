/********************************************************************
filename: 	GeomDef.h
file path:	dev\engine\Src\Core\Geom\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_GEOMDEF_H__
#define __LORD_GEOMDEF_H__

#include "Math/MathLib.h"

namespace LORD
{
	enum CullState
	{
		CS_INSIDE,
		CS_OUTSIDE,
		CS_INTERSECT,
	};

	struct HitInfo
	{
	public:
		bool				bHit;			// Did the ray hit an object?
		Vector3	 			hitPos;		// World coordinates of intersection
		Vector3				normal;			// Normal at hit point

		Real				beta;			// being used to get weight of hitting triangle
		Real				gamma;			// being used to get weight of hitting triangle

	public:
		HitInfo()
			: bHit(false)
			, hitPos(Vector3::INVALID)
			, normal(Vector3::INVALID)
			, beta(0.0)
			, gamma(0.0)
		{
		}
	};

}

#endif