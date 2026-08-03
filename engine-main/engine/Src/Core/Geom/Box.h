/********************************************************************
filename: 	Box.h
file path:	dev\engine\Src\Core\Geom\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_BOX_H__
#define __LORD_BOX_H__

#include "Sphere.h"

namespace LORD
{
	/**
	\brief AABB
	*/
	class LORD_CORE_API Box
	{
	public:
		enum Extent
		{
			EXTENT_NULL,
			EXTENT_FINITE,
			EXTENT_INFINITE
		};

		/*
		   4--------7
		  /|       /|
		 / |      / |
		0--------3  |
		|  6-----|--5
		| /      | /
		|/       |/
		2--------1

		vMin (6)
		vMax (3)
		*/

		enum Corner
		{
			CORNER_NLT		= 0,		//!< near left top
			CORNER_NRB		= 1,		//!< near right bottom
			CORNER_NLB		= 2,		//!< near left bottom
			CORNER_NRT		= 3,		//!< near right top

			CORNER_FLT		= 4,		//!< far left top
			CORNER_FRB		= 5,		//!< far right bottom
			CORNER_FLB		= 6,		//!< far left bottom
			CORNER_FRT		= 7,		//!< far right top
		};

	public:
		Extent		mExtent;
		Vector3		vMin;		//!< Min Point
		Vector3		vMax;		//!< Max Point

	public:
		inline Box()
		{
			reset();
		}

		inline Box(const Vector3&_vMin, const Vector3&_vMax)
		{
			setExtents(_vMin, _vMax);
		}

		inline Box(const Box &box)
		{
			if (box.isNull())
				reset();
			else
				setExtents(box.vMin, box.vMax);
		}

		inline Box(Real minX, Real minY, Real minZ, Real maxX, Real maxY, Real maxZ)
		{
			setExtents(minX, minY, minZ, maxX, maxY, maxZ);
		}

	public:
		inline const Box& operator = (const Box& rhs)
		{
			if (rhs.isNull())
			{
				reset();
			}
			else
			{
				vMin = rhs.vMin;
				vMax = rhs.vMax;
				mExtent = EXTENT_FINITE;
			}
			return *this;
		}

		inline bool operator == (const Box& rhs) const
		{
			return (this->vMin == rhs.vMin) && (this->vMax == rhs.vMax);
		}

		inline bool operator != (const Box& rhs) const
		{
			return !(*this == rhs);
		}

	public:
		inline void reset()
		{
			vMin.x = vMin.y = vMin.z = Math::MAX_REAL;
			vMax.x = vMax.y = vMax.z = Math::MIN_REAL;
			mExtent = EXTENT_NULL;
		}

		inline void setExtents(const Vector3& min, const Vector3& max)
		{
			mExtent = EXTENT_FINITE;
			vMin = min;
			vMax = max;
		}

		inline void setExtents(
			Real mx, Real my, Real mz,
			Real Mx, Real My, Real Mz)
		{
			mExtent = EXTENT_FINITE;

			vMin.x = mx;
			vMin.y = my;
			vMin.z = mz;

			vMax.x = Mx;
			vMax.y = My;
			vMax.z = Mz;

			if (mx >= Mx || my >= My || mz >= Mz)
				mExtent = EXTENT_NULL;
		}

		inline void addPoint(float x, float y, float z)
		{
			addPoint(Vector3(x, y, z));
		}

		inline void addPoint(const Vector3 &point)
		{
			switch (mExtent)
			{
			case EXTENT_NULL: // if null, use this point
				setExtents(point, point);
				return;

			case EXTENT_FINITE:
				vMax.makeCeil(point);
				vMin.makeFloor(point);
				return;
			}
		}

		inline Box addCoord(float x, float y, float z) const
		{
			Box result = *this;
			if (x < 0.f)
				result.vMin.x += x;
			if (x > 0.f)
				result.vMax.x += x;
			if (y < 0.f)
				result.vMin.y += y;
			if (y > 0.f)
				result.vMax.y += y;
			if (z < 0.f)
				result.vMin.z += z;
			if (z > 0.f)
				result.vMax.z += z;
			return result;
		}

		inline Box addCoord(const Vector3& v) const
		{
			Box result = *this;
			if (v.x < 0.f)
				result.vMin.x += v.x;
			if (v.x > 0.f)
				result.vMax.x += v.x;
			if (v.y < 0.f)
				result.vMin.y += v.y;
			if (v.y > 0.f)
				result.vMax.y += v.y;
			if (v.z < 0.f)
				result.vMin.z += v.z;
			if (v.z > 0.f)
				result.vMax.z += v.z;
			return result;
		}

		inline void unionBox(const Box &box)
		{
			if ((box.mExtent == EXTENT_NULL))
			{
				return;
			}
			else if (mExtent == EXTENT_NULL)
			{
				setExtents(box.vMin, box.vMax);
			}
			else
			{
				Vector3 min = vMin;
				Vector3 max = vMax;
				max.makeCeil(box.vMax);
				min.makeFloor(box.vMin);

				setExtents(min, max);
			}
		}
		//void			reset();
		//void			addPoint(const Vector3 &point);
		//void			unionBox(const Box& box);

		inline bool isIntersected(const Box& box) const
		{
			if ((mExtent == EXTENT_NULL) || (box.mExtent == EXTENT_NULL))
				return false;

			// Use up to 6 separating planes
			if (vMax.x <= box.vMin.x)
				return false;
			if (vMax.y <= box.vMin.y)
				return false;
			if (vMax.z <= box.vMin.z)
				return false;

			if (vMin.x >= box.vMax.x)
				return false;
			if (vMin.y >= box.vMax.y)
				return false;
			if (vMin.z >= box.vMax.z)
				return false;

			return true;
		}

		inline bool intersects(const Vector3& vec1, const Vector3& vec2) const
		{
			if (mExtent == EXTENT_NULL)
				return false;
			
			float minx = Math::Min(vec1.x, vec2.x);
			float miny = Math::Min(vec1.y, vec2.y);
			float minz = Math::Min(vec1.z, vec2.z);
			float maxx = Math::Max(vec1.x, vec2.x);
			float maxy = Math::Max(vec1.y, vec2.y);
			float maxz = Math::Max(vec1.z, vec2.z);

			return
				vMin.x < maxx && vMax.x > minx &&
				vMin.y < maxy && vMax.y > miny &&
				vMin.z < maxz && vMax.z > minz;
		}

		inline void fromBSphere(const Sphere &bs)
		{
			mExtent = EXTENT_FINITE;

			vMin.x = bs.c.x - bs.r;
			vMin.y = bs.c.y - bs.r;
			vMin.z = bs.c.z - bs.r;

			vMax.x = bs.c.x + bs.r;
			vMax.y = bs.c.y + bs.r;
			vMax.z = bs.c.z + bs.r;
		}

		inline Vector3 getCorner(Corner corner) const
		{
			switch(corner)
			{
			case CORNER_NLT:
				return Vector3(vMin.x, vMax.y, vMax.z);
			case CORNER_NRB:
				return Vector3(vMax.x, vMin.y, vMax.z);
			case CORNER_NLB:
				return Vector3(vMin.x, vMin.y, vMax.z);
			case CORNER_NRT:
				return vMax;
			case CORNER_FLT:
				return Vector3(vMin.x, vMax.y, vMin.z);
			case CORNER_FRB:
				return Vector3(vMax.x, vMin.y, vMin.z);
			case CORNER_FLB:
				return vMin;
			case CORNER_FRT:
				return Vector3(vMax.x, vMax.y, vMin.z);
			default:
				return Vector3::INVALID;
			}
		}
		//bool			isIntersected(const Box& box) const;
		//void			fromBSphere(const Sphere &bs);
		//Vector3		getCorner(Corner corner) const;

		inline Vector3 getSize() const
		{
			return (vMax - vMin);
		}

		inline bool isNull() const
		{
			return mExtent == EXTENT_NULL;
		}

		float getAverageEdgeLength() const
		{
			Vector3 dis = vMax - vMin;
			return (dis.x + dis.y + dis.z) / 3.f;
		}

		inline bool isPointInside(const Vector3& v) const
		{
			switch (mExtent)
			{
			case EXTENT_NULL:
				return false;
			case EXTENT_FINITE:
				return v.x > vMin.x && v.x < vMax.x
					&& v.y > vMin.y && v.y < vMax.y
					&& v.z > vMin.z && v.z < vMax.z;
			}

			return false;
		}

		inline void move(const Vector3& offset)
		{
			vMin += offset;
			vMax += offset;
		}

		inline void move(float x, float y, float z)
		{
			vMin.x += x;
			vMax.x += x;
			vMin.y += y;
			vMax.y += y;
			vMin.z += z;
			vMax.z += z;
		}

		inline void getPoints(Vector3* points) const 
		{
			points[0].set(vMin.x, vMin.y, vMin.z);
			points[1].set(vMax.x, vMin.y, vMin.z);
			points[2].set(vMin.x, vMax.y, vMin.z);
			points[3].set(vMax.x, vMax.y, vMin.z);
			points[4].set(vMin.x, vMin.y, vMax.z);
			points[5].set(vMax.x, vMin.y, vMax.z);
			points[6].set(vMin.x, vMax.y, vMax.z);
			points[7].set(vMax.x, vMax.y, vMax.z);
		}

		inline Box getOffset(const Vector3& offset) const
		{
			Box result = *this;
			result.vMin += offset;
			result.vMax += offset;

			return result;
		}

		inline Box getOffset(float x, float y, float z) const
		{
			Box result = *this;
			result.vMin.x += x;
			result.vMin.y += y;
			result.vMin.z += z;
			result.vMax.x += x;
			result.vMax.y += y;
			result.vMax.z += z;

			return result;
		}

		Real getXOffset(const Box& bb, float d) const
		{
			if (bb.vMax.y > vMin.y && bb.vMin.y < vMax.y && bb.vMax.z > vMin.z && bb.vMin.z < vMax.z)
			{
				if (d > 0.0f && bb.vMax.x <= vMin.x)
				{
					float d1 = vMin.x - bb.vMax.x;

					if (d1 < d)
						d = d1;
				}
				else if (d < 0.0f && bb.vMin.x >= vMax.x)
				{
					float d0 = vMax.x - bb.vMin.x;

					if (d0 > d)
						d = d0;
				}

				return d;
			}
			else
			{
				return d;
			}
		}

		Real getYOffset(const Box& bb, float d) const
		{
			if (bb.vMax.x > vMin.x && bb.vMin.x < vMax.x && bb.vMax.z > vMin.z && bb.vMin.z < vMax.z)
			{
				if (d > 0.0f && bb.vMax.y <= vMin.y)
				{
					float d1 = vMin.y - bb.vMax.y;

					if (d1 < d)
						d = d1;
				}
				else if (d < 0.0f && bb.vMin.y >= vMax.y)
				{
					float d0 = vMax.y - bb.vMin.y;

					if (d0 > d)
						d = d0;
				}

				return d;
			}
			else
			{
				return d;
			}
		}

		Real getZOffset(const Box& bb, float d) const
		{
			if (bb.vMax.x > vMin.x && bb.vMin.x < vMax.x && bb.vMax.y > vMin.y && bb.vMin.y < vMax.y)
			{
				if (d > 0.0f && bb.vMax.z <= vMin.z)
				{
					float d1 = vMin.z - bb.vMax.z;

					if (d1 < d)
						d = d1;
				}
				else if (d < 0.0f && bb.vMin.z >= vMax.z)
				{
					float d0 = vMax.z - bb.vMin.z;

					if (d0 > d)
						d = d0;
				}

				return d;
			}
			else
			{
				return d;
			}
		}

		inline Box expland(Real x, Real y, Real z) const
		{
			Real minx = vMin.x - x;
			Real miny = vMin.y - y;
			Real minz = vMin.z - z;
			Real maxx = vMax.x + x;
			Real maxy = vMax.y + y;
			Real maxz = vMax.z + z;
			return Box(minx, miny, minz, maxx, maxy, maxz);
		}

		inline Box expand(Real extend) const
		{
			Real minx = vMin.x - extend;
			Real miny = vMin.y - extend;
			Real minz = vMin.z - extend;
			Real maxx = vMax.x + extend;
			Real maxy = vMax.y + extend;
			Real maxz = vMax.z + extend;
			return Box(minx, miny, minz, maxx, maxy, maxz);
		}

		inline Box unionCopy(const Box& other) const
		{
			Real minx = Math::Min(vMin.x, other.vMin.x);
			Real miny = Math::Min(vMin.y, other.vMin.y);
			Real minz = Math::Min(vMin.z, other.vMin.z);
			Real maxx = Math::Max(vMax.x, other.vMax.x);
			Real maxy = Math::Max(vMax.y, other.vMax.y);
			Real maxz = Math::Max(vMax.z, other.vMax.z);
			return Box(minx, miny, minz, maxx, maxy, maxz);
		}
		
		inline bool isVecInYZ(const Vector3& vec) const
		{
			return vec.y >= vMin.y && vec.y <= vMax.y && vec.z >= vMin.z && vec.z <= vMax.z;
		}

		inline bool isVecInXZ(const Vector3& vec) const
		{
			return vec.x >= vMin.x && vec.x <= vMax.x && vec.z >= vMin.z && vec.z <= vMax.z;
		}

		inline bool isVecInXY(const Vector3& vec) const
		{
			return vec.x >= vMin.x && vec.x <= vMax.x && vec.y >= vMin.y && vec.y <= vMax.y;
		}

		inline Box contract(Real x, Real y, Real z) const
		{
			Real minx = vMin.x + x;
			Real miny = vMin.y + y;
			Real minz = vMin.z + z;
			Real maxx = vMax.x - x;
			Real maxy = vMax.y - y;
			Real maxz = vMax.z - z;
			return Box(minx, miny, minz, maxx, maxy, maxz);
		}

		inline Box contract(Real b) const
		{
			Real minx = vMin.x + b;
			Real miny = vMin.y + b;
			Real minz = vMin.z + b;
			Real maxx = vMax.x - b;
			Real maxy = vMax.y - b;
			Real maxz = vMax.z - b;
			return Box(minx, miny, minz, maxx, maxy, maxz);
		}

		inline Real getDX() const 
		{
			return vMax.x - vMin.x;
		}

		inline Real getDY() const
		{
			return vMax.y - vMin.y;
		}

		inline Real getDZ() const
		{
			return vMax.z - vMin.z;
		}

		inline Vector3 getCenter() const
		{
			return (vMin + vMax)* 0.5f;
		}

		inline Real getMaxEdgeLen() const
		{
			return Math::Max3(getDX(), getDY(), getDZ());
		}

		inline Box transform(const Matrix4& mat) const
		{
			// Do nothing if current null or infinite
			if (mExtent != EXTENT_FINITE)
				return *this;

			Box box;
			box.reset();

			Vector3 oldMin, oldMax, currentCorner;

			// Getting the old values so that we can use the existing merge method.
			oldMin = vMin;
			oldMax = vMax;
			
			// min min min
			currentCorner = oldMin;
			box.addPoint(currentCorner * mat);
			// min,min,max
			currentCorner.z = oldMax.z;
			box.addPoint(currentCorner * mat);
			// min max max
			currentCorner.y = oldMax.y;
			box.addPoint(currentCorner * mat);
			// min max min
			currentCorner.z = oldMin.z;
			box.addPoint(currentCorner * mat);
			// max max min
			currentCorner.x = oldMax.x;
			box.addPoint(currentCorner * mat);
			// max max max
			currentCorner.z = oldMax.z;
			box.addPoint(currentCorner * mat);
			// max min max
			currentCorner.y = oldMin.y;
			box.addPoint(currentCorner * mat);
			// max min min
			currentCorner.z = oldMin.z;
			box.addPoint(currentCorner * mat);

			return box;
		}

		inline Real getDiagonalLenSqr() const
		{
			Real dx = getDX();
			Real dy = getDY();
			Real dz = getDZ();
			return dx*dx + dy*dy + dz*dz;
		}

		inline Real getDiagonalLen() const 
		{
			return Math::Sqrt(getDiagonalLenSqr());
		}

		inline void inflate(Real x, Real y, Real z) 
		{
			vMin -= Vector3(x, y, z);
			vMax += Vector3(x, y, z);
		}

		inline void deflate(Real x, Real y, Real z)
		{
			vMin += Vector3(x, y, z);
			vMax -= Vector3(x, y, z);
		}

		inline CullState	cullAABB(const Box& box) const
		{
			if (!isIntersected(box))
				return CS_OUTSIDE;
			else if (isPointInside(box.vMin) && isPointInside(box.vMax))
				return CS_INSIDE;
			else
				return CS_INTERSECT;
		}
	};
}

#endif
