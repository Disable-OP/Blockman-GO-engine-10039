/********************************************************************
filename: 	Ray.h
file path:	dev\engine\Src\Core\Geom\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_RAY_H__
#define __LORD_RAY_H__

#include "Math/MathLib.h"
#include "Triangle.h"
#include "Plane.h"
#include "Rect.h"
#include "Box.h"
#include "Sphere.h"

namespace LORD
{
	class LORD_CORE_API Ray
	{
	public:
		Vector3	o;			//!< origin
		Vector3	dir;		//!< direction

	public:
		Ray();
		Ray(const Vector3& o, const Vector3& dir);
		Ray(const Ray& src);

	public:
		bool hitTri(const Triangle& tri, Real& tmin) const;
		bool hitTri(const Triangle& tri, Real& tmin, HitInfo& si) const;
		bool hitPlane(const Plane& plane, Real& tmin) const;
		bool hitPlane(const Plane& plane, Real& tmin, HitInfo& si) const;
		bool hitBox(const Box& box) const;
		bool hitBox(const Box& box, Real& tmin, HitInfo& si) const;
		bool hitSphere(const Sphere& sphere, Real& tmin, HitInfo& si) const;
		bool hitSphere(const Sphere& sphere, Real& tmin) const;
		void transform(const Matrix4& mat);
		Vector3 getPoint(Real dis) const;
	};
}

#endif
