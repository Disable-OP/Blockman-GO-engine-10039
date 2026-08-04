/********************************************************************
filename: 	IntMath.h
file path:	dev\engine\Src\Core\Math\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/
#ifndef __INT_MATH_H__
#define __INT_MATH_H__

#include <array>
#include "LordMath.h"
#include "Util/AssertX.h"

namespace LORD
{
	const int MXI_FBIT = 16;
	const int MXI_UNIT = 1 << MXI_FBIT;

	class Vector3i;
	class Vector4i;

	class LORD_CORE_API Vector2i
	{
	public:
		union
		{
			struct
			{
				int x, y;
			};

			int m[2];
		};

		static const Vector2i INVALID;
		static const Vector2i ZERO;
		static const Vector2i ONE;
		static const Vector2i NEG_ONE;
		static const Vector2i UNIT_X;
		static const Vector2i UNIT_Y;
		static const Vector2i NEG_UNIT_X;
		static const Vector2i NEG_UNIT_Y;
		static const Vector2i MXI_ONE;

		static const Vector2i MXI_NEG_ONE;
		static const Vector2i MXI_UNIT_X;
		static const Vector2i MXI_UNIT_Y;
		static const Vector2i MXI_NEG_UNIT_X;
		static const Vector2i MXI_NEG_UNIT_Y;

	public:
		inline Vector2i() {}
		inline Vector2i(int _x, int _y) : x(_x), y(_y) {}
		inline Vector2i(const Vector2i& vec) : x(vec.x), y(vec.y) {}

	public:
		inline Vector2i& operator = (const Vector2i& rhs) { x = rhs.x; y = rhs.y; return *this; }

		inline Vector2i operator + (const Vector2i& rhs) const { return Vector2i(x + rhs.x, y + rhs.y); }
		inline Vector2i operator - (const Vector2i& rhs) const { return Vector2i(x - rhs.x, y - rhs.y); }
		inline Vector2i operator * (const Vector2i& rhs) const { return Vector2i(x*rhs.x, y*rhs.y); }
		inline Vector2i operator / (const Vector2i& rhs) const { return Vector2i(x / rhs.x, y / rhs.y); }

		inline Vector2i operator + (int i) const { return Vector2i(x + i, y + i); }
		inline Vector2i operator - (int i) const { return Vector2i(x - i, y - i); }
		inline Vector2i operator * (int i) const { return Vector2i(x*i, y*i); }
		inline Vector2i operator / (int i) const { LordAssertX(i != 0, "Zero divide error"); return Vector2i(x / i, y / i); }

		inline Vector2i& operator += (const Vector2i& rhs) { x += rhs.x; y += rhs.y; return *this; }
		inline Vector2i& operator -= (const Vector2i& rhs) { x -= rhs.x; y -= rhs.y; return *this; }
		inline Vector2i& operator *= (const Vector2i& rhs) { x *= rhs.x; y *= rhs.y; return *this; }
		inline Vector2i& operator /= (const Vector2i& rhs) { LordAssertX(rhs.x != 0 && rhs.y != 0, "Zero divide error"); x /= rhs.x; y /= rhs.y; return *this; }

		inline Vector2i& operator += (int i) { x += i; y += i; return *this; }
		inline Vector2i& operator -= (int i) { x -= i; y -= i; return *this; }
		inline Vector2i& operator *= (int i) { x *= i; y *= i; return *this; }
		inline Vector2i& operator /= (int i) { LordAssertX(i != 0, "Zero divide error"); x /= i; y /= i; return *this; }

		inline const Vector2i& operator + () const { return *this; }
		inline Vector2i operator - () const { return Vector2i(-x, -y); }

		inline bool operator == (const Vector2i& rhs) const { return (x == rhs.x && y == rhs.y); }
		inline bool operator != (const Vector2i& rhs) const { return (x != rhs.x || y != rhs.y); }
		inline int& operator [] (int index) { LordAssertX(index >= 0 && index < 2, "Access out of bounds"); return m[index]; }
		inline const int& operator [] (int index) const { LordAssertX(index >= 0 && index < 2, "Access out of bounds"); return m[index]; }

		inline bool operator <	(const Vector2i& rhs) const { if (x < rhs.x) return true; else if (x == rhs.x) return y < rhs.y; return false; }
		inline bool operator <= (const Vector2i& rhs) const { return !(*this > rhs); }
		inline bool operator >	(const Vector2i& rhs) const { if (x > rhs.x) return true; else if (x == rhs.x) return y > rhs.y; return false; }
		inline bool operator >= (const Vector2i& rhs) const { return !(*this < rhs); }
		inline bool operator <	(int i) const { if (x < i && y < i)   return true; else return false; }
		inline bool operator <= (int i) const { if (x <= i && y <= i) return true; else	return false; }
		inline bool operator >	(int i) const { if (x > i && y > i)   return true; else return false; }
		inline bool operator >= (int i) const { if (x >= i && y >= i) return true; else return false; }

		int					dot(const Vector2i& rhs) const;
		int					cross(const Vector2i& rhs) const;
		inline int*			ptr() { return &x; }
		inline const int*	ptr() const { return &x; }
		inline void			zero() { x = 0; y = 0; }
		inline void			one() { x = 1; y = 1; }
		inline void			set(int x, int y) { this->x = x; this->y = y; }
		inline void			set(int* p) { this->x = p[0]; this->y = p[1]; }
		inline void			flip() { x = -x; y = -y; }
		inline Vector2i&	abs() { x = Math::Abs(x);	y = Math::Abs(y); return *this; }
		inline Vector2i&	neg() { x = -x; y = -y; return *this; }

		/// static class.
		static inline void	Abs(Vector2i& outVec, const Vector2i& a) { outVec = a; outVec.abs(); }
		static inline void	Neg(Vector2i& outVec, const Vector2i& a) { outVec = a; outVec.neg(); }
		static inline int   Dot(const Vector2i& a, const Vector2i& b) { return a.dot(b); }
		static inline int   Cross(const Vector2i& a, const Vector2i& b) { return a.cross(b); }
	};

	class LORD_CORE_API Vector3i
	{
	public:
		union
		{
			struct
			{
				int x, y, z;
			};

			int m[3];
		};

		static const Vector3i ZERO;						//!< Vec3(0, 0, 0)
		static const Vector3i ONE;						//!< Vec3(1, 1, 1)
		static const Vector3i INVALID;					//!< Vec3(inv,inv,inv)
		static const Vector3i UNIT_X;					//!< Vec3(1, 0, 0)
		static const Vector3i UNIT_Y;					//!< Vec3(0, 1, 0)
		static const Vector3i UNIT_Z;					//!< Vec3(0, 0, 1)
		static const Vector3i NEG_UNIT_X;				//!< Vec3(-1, 0, 0)
		static const Vector3i NEG_UNIT_Y;				//!< Vec3(0, -1, 0)
		static const Vector3i NEG_UNIT_Z;				//!< Vec3(0, 0, -1)

		static const Vector3i MXI_ONE;					//!< Vec3(1, 1, 1)
		static const Vector3i MXI_UNIT_X;				//!< Vec3(1, 0, 0)
		static const Vector3i MXI_UNIT_Y;				//!< Vec3(0, 1, 0)
		static const Vector3i MXI_UNIT_Z;				//!< Vec3(0, 0, 1)
		static const Vector3i MXI_NEG_UNIT_X;			//!< Vec3(-1, 0, 0)
		static const Vector3i MXI_NEG_UNIT_Y;			//!< Vec3(0, -1, 0)
		static const Vector3i MXI_NEG_UNIT_Z;			//!< Vec3(0, 0, -1)

	public:
		inline Vector3i() : x(0), y(0), z(0) { }
		inline Vector3i(int _x, int _y, int _z) : x(_x), y(_y), z(_z) { }
		inline Vector3i(const Vector3i& vec) : x(vec.x), y(vec.y), z(vec.z) { }
		inline Vector3i(const std::array<int, 3>& arr) : x(arr[0]), y(arr[1]), z(arr[2]) { }

	public:
		inline Vector3i& operator =  (const Vector3i& rhs) { x = rhs.x;  y = rhs.y;  z = rhs.z;  return *this; }
		inline Vector3i& operator =  (const std::array<int, 3>& arr) { x = arr[0];  y = arr[1];  z = arr[2];  return *this; }
		inline operator std::array<int, 3>() const { return { {x, y, z} }; }

		inline Vector3i operator + (const Vector3i& rhs) const { return Vector3i(x + rhs.x, y + rhs.y, z + rhs.z); }
		inline Vector3i operator - (const Vector3i& rhs) const { return Vector3i(x - rhs.x, y - rhs.y, z - rhs.z); }
		inline Vector3i operator * (const Vector3i& rhs) const { return Vector3i(x * rhs.x, y * rhs.y, z * rhs.z); }
		inline Vector3i operator / (const Vector3i& rhs) const { LordAssertX(rhs.x != 0 && rhs.y != 0 && rhs.z != 0, "Zero divide error"); return Vector3i(x / rhs.x, y / rhs.y, z / rhs.z); }

		inline Vector3i operator + (int i) const { return Vector3i(x + i, y + i, z + i); }
		inline Vector3i operator - (int i) const { return Vector3i(x - i, y - i, z - i); }
		inline Vector3i operator * (int i) const { return Vector3i(x * i, y * i, z * i); }
		inline Vector3i operator / (int i) const { LordAssertX(i != 0, "Zero divide error"); return Vector3i(x / i, y / i, z / i); }

		inline Vector3i& operator += (const Vector3i& rhs) { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
		inline Vector3i& operator -= (const Vector3i& rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }
		inline Vector3i& operator *= (const Vector3i& rhs) { x *= rhs.x; y *= rhs.y; z *= rhs.z; return *this; }
		inline Vector3i& operator /= (const Vector3i& rhs) { LordAssertX(rhs.x != 0 && rhs.y != 0 && rhs.z != 0, "Zero divide error"); x /= rhs.x; y /= rhs.y; z /= rhs.z; return *this; }

		inline Vector3i& operator += (int i) { x += i; y += i; z += i; return *this; }
		inline Vector3i& operator -= (int i) { x -= i; y -= i; z -= i; return *this; }
		inline Vector3i& operator *= (int i) { x *= i; y *= i; z *= i; return *this; }
		inline Vector3i& operator /= (int i) { LordAssertX(i != 0, "Zero divide error"); x /= i; y /= i; z /= i; return *this; }

		inline Vector3i operator >> (int i) const { return Vector3i(x >> i, y >> i, z >> i); }
		inline Vector3i operator << (int i) const { return Vector3i(x << i, y << i, z << i); }
		inline Vector3i& operator >>= (int i) { x >>= i; y >>= i; z >>= i; return *this; }
		inline Vector3i& operator <<= (int i) { x <<= i; y <<= i; z <<= i; return *this; }

		inline Vector3i getSection() const { return Vector3i(x, y & 15, z); }
		inline Vector3i getChunkPos() const { return Vector3i(x & 15, y, z & 15); }
		
		inline Vector3i getEast() const { return Vector3i(x + 1, y, z); }
		inline Vector3i getWest() const { return Vector3i(x - 1, y, z); }
		inline Vector3i getUp() const { return Vector3i(x, y + 1, z); }
		inline Vector3i getDown() const { return Vector3i(x, y - 1, z); }
		inline Vector3i getSouth() const { return Vector3i(x, y, z + 1); }
		inline Vector3i getNorth() const { return Vector3i(x, y, z - 1); }
		
		inline Vector3i getPosX() const { return Vector3i(x + 1, y, z); }
		inline Vector3i getNegX() const { return Vector3i(x - 1, y, z); }
		inline Vector3i getPosY() const { return Vector3i(x, y + 1, z); }
		inline Vector3i getNegY() const { return Vector3i(x, y - 1, z); }
		inline Vector3i getPosZ() const { return Vector3i(x, y, z + 1); }
		inline Vector3i getNegZ() const { return Vector3i(x, y, z - 1); }
		
		inline Vector3i getPosX2() const { return Vector3i(x + 2, y, z); }
		inline Vector3i getNegX2() const { return Vector3i(x - 2, y, z); }
		inline Vector3i getPosY2() const { return Vector3i(x, y + 2, z); }
		inline Vector3i getNegY2() const { return Vector3i(x, y - 2, z); }
		inline Vector3i getPosZ2() const { return Vector3i(x, y, z + 2); }
		inline Vector3i getNegZ2() const { return Vector3i(x, y, z - 2); }
		
		inline Vector3i getNE() const { return Vector3i(x + 1, y, z - 1); }
		inline Vector3i getNW() const { return Vector3i(x - 1, y, z - 1); }
		inline Vector3i getSE() const { return Vector3i(x + 1, y, z + 1); }
		inline Vector3i getSW() const { return Vector3i(x - 1, y, z + 1); }

		inline Vector3i getUE() const { return Vector3i(x + 1, y + 1, z); }
		inline Vector3i getUW() const { return Vector3i(x - 1, y + 1, z); }
		inline Vector3i getUS() const { return Vector3i(x, y + 1, z + 1); }
		inline Vector3i getUN() const { return Vector3i(x, y + 1, z - 1); }

		inline Vector3i getDE() const { return Vector3i(x + 1, y - 1, z); }
		inline Vector3i getDW() const { return Vector3i(x - 1, y - 1, z); }
		inline Vector3i getDS() const { return Vector3i(x, y - 1, z + 1); }
		inline Vector3i getDN() const { return Vector3i(x, y - 1, z - 1); }

		inline Vector3i getUNE() const { return Vector3i(x + 1, y + 1, z - 1); }
		inline Vector3i getUNW() const { return Vector3i(x - 1, y + 1, z - 1); }
		inline Vector3i getUSE() const { return Vector3i(x + 1, y + 1, z + 1); }
		inline Vector3i getUSW() const { return Vector3i(x - 1, y + 1, z + 1); }
		
		inline Vector3i getDNE() const { return Vector3i(x + 1, y - 1, z - 1); }
		inline Vector3i getDNW() const { return Vector3i(x - 1, y - 1, z - 1); }
		inline Vector3i getDSE() const { return Vector3i(x + 1, y - 1, z + 1); }
		inline Vector3i getDSW() const { return Vector3i(x - 1, y - 1, z + 1); }
			

		inline const Vector3i& operator + () const { return *this; }
		inline Vector3i operator - () const { return Vector3i(-x, -y, -z); }

		inline bool operator == (const Vector3i& rhs) const { return (x == rhs.x && y == rhs.y && z == rhs.z); }
		inline bool operator != (const Vector3i& rhs) const { return (x != rhs.x || y != rhs.y || z != rhs.z); }
		inline int& operator [] (int index) { LordAssertX(index >= 0 && index < 3, "Access out of bounds"); return m[index]; }
		inline const int& operator [] (int index) const { LordAssertX(index >= 0 && index < 3, "Access out of bounds"); return m[index]; }

		//inline bool operator <  (const Vector3i& rhs) const { if (x < rhs.x  && y < rhs.y  && z < rhs.z)  return true; else return false; }
		inline bool operator <= (const Vector3i& rhs) const { if (x <= rhs.x && y <= rhs.y && z <= rhs.z) return true; else return false; }
		inline bool operator >  (const Vector3i& rhs) const { if (x > rhs.x  && y > rhs.y  && z > rhs.z)  return true; else return false; }
		inline bool operator >= (const Vector3i& rhs) const { if (x >= rhs.x && y >= rhs.y && z >= rhs.z) return true; else	return false; }
		inline bool operator <	(int i) const { if (x < i  && y < i  && z < i)  return true; else return false; }
		inline bool operator <= (int i) const { if (x <= i && y <= i && z <= i) return true; else return false; }
		inline bool operator >	(int i) const { if (x > i  && y > i  && z > i)  return true; else return false; }
		inline bool operator >= (int i) const { if (x >= i && y >= i && z >= i)  return true; else return false; }

		inline Vector3i&	normalize();
		int					len() const;
		int					lenSqr() const;
		int					dot(const Vector3i& rhs) const;
		Vector3i			cross(const Vector3i& rhs) const;
		inline int*			ptr() { return &x; }
		inline const int*	ptr() const { return &x; }
		inline void			zero() { x = 0;	y = 0; z = 0; }
		inline void			one() { x = 1; y = 1; z = 1; }
		inline void			flip() { x = -x; y = -y; z = -z; }
		inline void			set(int x, int y, int z) { this->x = x; this->y = y; this->z = z; }
		inline void			set(int* p) { this->x = p[0]; this->y = p[1]; this->z = p[2]; }
		inline Vector3i&	abs() { x = Math::Abs(x);	y = Math::Abs(y); z = Math::Abs(z); return *this; }
		inline Vector3i&	neg() { x = -x; y = -y; z = -z; return *this; }

		////static 
		static inline void Abs(Vector3i& outVec, const Vector3i& a) { outVec = a; outVec.abs(); }
		static inline void Neg(Vector3i& outVec, const Vector3i& a) { outVec = a; outVec.neg(); }
		static inline int  Dot(const Vector3i& a, const Vector3i& b) { return a.dot(b); }
		static inline void Cross(Vector3i& outVec, const Vector3i& a, const Vector3i& b) { outVec = a.cross(b); }
	};

	struct CompareVector3i
	{
		bool operator()(const Vector3i& lhs, const Vector3i& rhs) const
		{
			if (lhs.x != rhs.x)
			{
				return lhs.x < rhs.x;
			}
			else if (lhs.y != rhs.y)
			{
				return lhs.y < rhs.y;
			}
			return lhs.z < rhs.z;
		}
	};

	typedef set<Vector3i, CompareVector3i>::type Vector3iSet;
	template<typename T>
	using Vector3iMap = typename map<Vector3i, T, CompareVector3i>::type;

	class LORD_CORE_API Vector4i
	{
	public:
		union
		{
			struct
			{
				int x, y, z, w;
			};

			int m[4];
		};

		static const Vector4i ZERO;					//!< Vec4(0, 0, 0, 0)
		static const Vector4i ONE;					//!< Vec4(1, 1, 1, 1)
		static const Vector4i UNIT_X;				//!< Vec4(1, 0, 0, 0)
		static const Vector4i UNIT_Y;				//!< Vec4(0, 1, 0, 0)
		static const Vector4i UNIT_Z;				//!< Vec4(0, 0, 1, 0)
		static const Vector4i UNIT_W;				//!< Vec4(0, 0, 0, 1)
		static const Vector4i NEG_UNIT_X;			//!< Vec4(-1,0, 0, 0)
		static const Vector4i NEG_UNIT_Y;			//!< Vec4(0,-1, 0, 0)
		static const Vector4i NEG_UNIT_Z;			//!< Vec4(0, 0,-1, 0)
		static const Vector4i NEG_UNIT_W;			//!< Vec4(0, 0, 0,-1

		static const Vector4i MXI_ONE;					//!< Vec4(1, 1, 1, 1)
		static const Vector4i MXI_UNIT_X;				//!< Vec4(1, 0, 0, 0)
		static const Vector4i MXI_UNIT_Y;				//!< Vec4(0, 1, 0, 0)
		static const Vector4i MXI_UNIT_Z;				//!< Vec4(0, 0, 1, 0)
		static const Vector4i MXI_UNIT_W;				//!< Vec4(0, 0, 0, 1)
		static const Vector4i MXI_NEG_UNIT_X;			//!< Vec4(-1,0, 0, 0)
		static const Vector4i MXI_NEG_UNIT_Y;			//!< Vec4(0,-1, 0, 0)
		static const Vector4i MXI_NEG_UNIT_Z;			//!< Vec4(0, 0,-1, 0)
		static const Vector4i MXI_NEG_UNIT_W;			//!< Vec4(0, 0, 0,-1)

	public:
		inline Vector4i() { }
		inline Vector4i(int _x, int _y, int _z, int _w) : x(_x), y(_y), z(_z), w(_w) { }
		inline Vector4i(const Vector3i& vec, int _w) : x(vec.x), y(vec.y), z(vec.z), w(_w) {}
		inline Vector4i(const Vector4i& vec) : x(vec.x), y(vec.y), z(vec.z), w(vec.w) { }

	public:
		inline Vector4i& operator =  (const Vector4i& rhs) { x = rhs.x;  y = rhs.y;  z = rhs.z; w = rhs.w;  return *this; }

		inline Vector4i operator + (const Vector4i& rhs) const { return Vector4i(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w); }
		inline Vector4i operator - (const Vector4i& rhs) const { return Vector4i(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w); }
		inline Vector4i operator * (const Vector4i& rhs) const { return Vector4i(x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w); }
		inline Vector4i operator / (const Vector4i& rhs) const {
			LordAssertX(rhs.x != 0 && rhs.y != 0 && rhs.z != 0 && rhs.w != 0, "Zero divide error");
			return Vector4i(x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w);
		}

		inline Vector4i operator + (int i) const { return Vector4i(x + i, y + i, z + i, w + i); }
		inline Vector4i operator - (int i) const { return Vector4i(x - i, y - i, z - i, w - i); }
		inline Vector4i operator * (int i) const { return Vector4i(x * i, y * i, z * i, w * i); }
		inline Vector4i operator / (int i) const { LordAssertX(i != 0, "Zero divide error"); return Vector4i(x / i, y / i, z / i, w / i); }

		inline Vector4i& operator += (const Vector4i& rhs) { x += rhs.x; y += rhs.y; z += rhs.z; w += rhs.w; return *this; }
		inline Vector4i& operator -= (const Vector4i& rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z; w -= rhs.w; return *this; }
		inline Vector4i& operator *= (const Vector4i& rhs) { x *= rhs.x; y *= rhs.y; z *= rhs.z; w *= rhs.w; return *this; }
		inline Vector4i& operator /= (const Vector4i& rhs) {
			LordAssertX(rhs.x != 0 && rhs.y != 0 && rhs.z != 0 && rhs.w != 0, "Zero divide error");
			x /= rhs.x; y /= rhs.y; z /= rhs.z; w /= rhs.w; return *this;
		}

		inline Vector4i& operator += (int i) { x += i; y += i; z += i; w += i; return *this; }
		inline Vector4i& operator -= (int i) { x -= i; y -= i; z -= i; w -= i; return *this; }
		inline Vector4i& operator *= (int i) { x *= i; y *= i; z *= i; w *= i; return *this; }
		inline Vector4i& operator /= (int i) { LordAssertX(i != 0, "Zero divide error"); x /= i; y /= i; z /= i; w /= i; return *this; }

		inline operator Vector3i() { return Vector3i(x, y, z); }
		inline operator const Vector3i() const { return Vector3i(x, y, z); }
		inline const Vector4i& operator + () const { return *this; }
		inline Vector4i operator - () const { return Vector4i(-x, -y, -z, -w); }

		inline bool operator == (const Vector4i& rhs) const { return (x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w); }
		inline bool operator != (const Vector4i& rhs) const { return (x != rhs.x || y == rhs.y || z != rhs.z || w != rhs.w); }
		inline int& operator [] (int index) { LordAssertX(index >= 0 && index < 3, "Access out of bound"); return m[index]; }
		inline const int& operator [] (int index) const { LordAssertX(index >= 0 && index < 3, "Access out of bound"); return m[index]; }

		inline bool operator <  (const Vector4i& rhs) const { if (x < rhs.x && y < rhs.y && z < rhs.z)	  return true; else return false; }
		inline bool operator <= (const Vector4i& rhs) const { if (x <= rhs.x && y <= rhs.y && z <= rhs.z) return true; else return false; }
		inline bool operator >  (const Vector4i& rhs) const { if (x > rhs.x && y > rhs.y && z > rhs.z)    return true; else return false; }
		inline bool operator >= (const Vector4i& rhs) const { if (x >= rhs.x && y >= rhs.y && z >= rhs.z) return true; else	return false; }
		inline bool operator <	(int i) const { if (x < i && y < i && z < i)    return true; else return false; }
		inline bool operator <= (int i) const { if (x <= i && y <= i && z <= i) return true; else return false; }
		inline bool operator >	(int i) const { if (x > i && y > i && z > i)    return true; else return false; }
		inline bool operator >= (int i) const { if (x >= i && y >= i && z >= i)  return true; else return false; }

		int                 dot(const Vector4i& rhs) const;
		inline int*			ptr() { return &x; }
		inline const int*	ptr() const { return &x; }
		inline void			zero() { x = 0;	y = 0; z = 0; }
		inline void			one() { x = 1; y = 1; z = 1; }
		inline void			flip() { x = -x; y = -y; z = -z; }
		inline void			set(int x, int y, int z) { this->x = x; this->y = y; this->z = z; }
		inline void			set(int* p) { this->x = p[0]; this->y = p[1]; this->z = p[2]; }
		inline Vector4i&	abs() { x = Math::Abs(x);	y = Math::Abs(y); z = Math::Abs(z); return *this; }
		inline Vector4i&	neg() { x = -x; y = -y; z = -z; return *this; }

		////static 
		static inline void Abs(Vector4i& outVec, const Vector4i& a) { outVec = a; outVec.abs(); }
		static inline void Neg(Vector4i& outVec, const Vector4i& a) { outVec = a; outVec.neg(); }
		static inline int  Dot(const Vector4i& a, const Vector4i& b) { return a.dot(b); }
	};

	//class LORD_CORE_API Quaternioni
	//{
	//public:
	//	union
	//	{
	//		struct
	//		{
	//			int x, y, z, w;
	//		};
	//		int m[4];
	//	};

	//	static const Quaternioni IDENTITY;		//!< Quaternioni(1, 0, 0, 0)
	//	static const Quaternioni INVALID;		//!< Quaternioni(Math::MAX_INT, Math::MAX_INT, Math::MAX_INT, Math::MAX_INT)

	//public:
	//	inline Quaternioni() : x(0), y(0), z(0), w(1) {}
	//	inline Quaternioni(const Quaternioni& quan) : x(quan.x), y(quan.y), z(quan.z), w(quan.w) {}
	//	inline Quaternioni(const int* pi) { assert(pi != NULL); x = pi[0]; y = pi[1]; z = pi[2]; w = pi[3]; }
	//	inline Quaternioni(int ix, int iy, int iz, int iw) : x(ix), y(iy), z(iz), w(iw) {}
	//	inline Quaternioni(const Vector3i& viAixs, int iRadian) { rotateAxis(viAixs, iRadian); }

	//	inline Quaternioni& operator= (const Quaternioni& quan) { x = quan.x;	y = quan.y;	z = quan.z;	w = quan.w;	return *this; }

	//	inline int& operator[] (int index) { assert(index >= 0 && index < 4); return m[index]; }
	//	inline const int& operator[] (int index) const { assert(index >= 0 && index < 4); return m[index]; }
	//	inline operator int* () { return m; }
	//	inline operator const int* () const { return m; }
	//	inline int* ptr() { return &x; }
	//	inline const int* ptr() const { return &x; }

	//	inline const Quaternioni& operator + () const { return *this; }
	//	inline Quaternioni operator - () const { return Quaternioni(-x, -y, -z, -w); }

	//	inline Quaternioni operator + (const Quaternioni& b) const { Quaternioni q; q.x = x + b.x; q.y = y + b.y; q.z = z + b.z; q.w = w + b.w; return q; }
	//	inline Quaternioni operator - (const Quaternioni& b) const { Quaternioni q; q.x = x - b.x; q.y = y - b.y; q.z = z - b.z; q.w = w - b.w; return q; }
	//	Quaternioni operator * (const Quaternioni& b) const;

	//	inline Quaternioni operator/ (int i) const { assert(i != 0); Quaternioni q; q.x = x / i; q.y = y / i; q.z = z / i; q.w = w / i; return q; }
	//	Quaternioni operator * (int i) const;

	//	friend Quaternioni operator * (int i, const Quaternioni& q);
	//	inline friend Quaternioni operator / (int i, const Quaternioni& a) { assert(i != 0); Quaternioni q; q.x = i / a.x; q.y = i / a.y; q.z = i / a.z; q.w = i / a.w; return q; }

	//	inline Quaternioni& operator += (const Quaternioni& q) { x += q.x; y += q.y; z += q.z; w += q.w; return *this; }
	//	inline Quaternioni& operator -= (const Quaternioni& q) { x -= q.x; y -= q.y; z -= q.z; w -= q.w; return *this; }
	//	Quaternioni& operator *= (const Quaternioni& q);

	//	inline Quaternioni& operator *= (int i) { x *= i; y *= i; z *= i; w *= i; return *this; }
	//	inline Quaternioni& operator /= (int i) { assert(i != 0); x /= i; y /= i; z /= i; w /= i; return *this; }

	//	inline bool operator == (const Quaternioni& b) const { return (x == b.x && y == b.y && z == b.z && w == b.w); }
	//	inline bool operator != (const Quaternioni& b) const { return !(x == b.x && y == b.y && z == b.z && w == b.w); }

	//	Vector3i operator * (const Vector3i& v) const;
	//	Vector4i operator * (const Vector4i& v) const;

	//	Quaternioni cross(const Quaternioni& rhs);
	//	int			dot(const Quaternioni& rhs) const;
	//	void		inverse();
	//	int			len() const;
	//	int			lenSqr() const;
	//	void		toMat3(Matrix3i& mat) const;
	//	void		toMat4(Matrix4i& mat) const;
	//	void		rotateVec3(Vector3i& outVec, const Vector3i& vec) const;
	//	Vector3i	xAxis() const;
	//	Vector3i	yAxis() const;
	//	Vector3i	zAxis() const;
	//	int			getRadian() const;
	//	void		rotateAxis(const Vector3i& axis, int iRadian);
	//	void		fromEulerAngle(int iPitch, int iYaw, int iRoll);
	//	void		toEulerAngle(int& iPitch, int& iYaw, int& iRoll) const;

	//	inline void zero() { x = 0; y = 0; z = 0; w = 0; }
	//	inline void invalid() { *this = INVALID; }
	//	inline void set(int ix, int iy, int iz, int iw) { x = ix; y = iy; z = iz; w = iw; }
	//	inline void set(int i) { x = y = z = w = i; }
	//	inline void set(int* pi) { x = pi[0]; y = pi[1]; z = pi[2]; w = pi[3]; }
	//	inline void conjugate() { x = -x; y = -y; z = -z; }
	//	inline void identity() { x = 0; y = 0; z = 0; w = MXI_UNIT; }
	//	inline void diff(const Quaternioni& q1, const Quaternioni& q2) { Quaternioni invQ1 = q1; invQ1.inverse(); *this = invQ1 * q2; }
	//	inline Quaternioni& abs() { x = Math::Abs(x); y = Math::Abs(y); z = Math::Abs(z); w = Math::Abs(w); return *this; }
	//	inline Quaternioni& neg() { x = -x; y = -y; z = -z;	w = -w; return *this; }
	//	inline Vector3i getAxis() const { return Vector3i(x, y, z); }

	//};

	//class LORD_CORE_API Matrix3i
	//{
	//public:
	//	union
	//	{
	//		struct
	//		{
	//			int m00, m01, m02;
	//			int m10, m11, m12;
	//			int m20, m21, m22;
	//		};

	//		int m[9];
	//	};

	//	static const Matrix3i ZERO;
	//	static const Matrix3i IDENTITY;
	//	static const Matrix3i INVALID;

	//public:
	//	inline Matrix3i() { }
	//	inline Matrix3i(int i00, int i01, int i02, int i10, int i11, int i12, int i20, int i21, int i22)
	//		: m00(i00), m01(i01), m02(i02), m10(i10), m11(i11), m12(i12), m20(i20), m21(i21), m22(i22) {}
	//	inline Matrix3i(int* arr) { memcpy(m, arr, sizeof(int) * 9); }
	//	inline Matrix3i(const Matrix3i& mat) { memcpy(m, mat.m, sizeof(int) * 9); }

	//public:
	//	inline Matrix3i& operator= (const Matrix3i& rhs) { memcpy(m, rhs.m, sizeof(int) * 9); return *this; }

	//	inline int operator() (int i, int j) const { assert(i >= 0 && i < 3); assert(j >= 0 && j < 3); return getRow(i)[j]; }
	//	inline Vector3i operator[] (int row) const { return getRow(row); }
	//	inline int* ptr() { return m; }
	//	inline const int* ptr() const { return m; }

	//	inline Matrix3i& operator+= (const int i) { m00 += i; m01 += i; m02 += i; m10 += i; m11 += i; m12 += i;	m20 += i; m21 += i; m22 += i; return *this; }
	//	inline Matrix3i& operator-= (const int i) { m00 -= i; m01 -= i; m02 -= i; m10 -= i; m11 -= i; m12 -= i; m20 -= i; m21 -= i; m22 -= i; return *this; }
	//	Matrix3i& operator*= (const int i);
	//	inline Matrix3i& operator/= (const int i) { assert(i != 0); m00 /= i; m01 /= i; m02 /= i; m10 /= i; m11 /= i; m12 /= i; m20 /= i; m21 /= i; m22 /= i; return *this; }

	//	inline Matrix3i& operator+= (const Matrix3i& b) { m00 += b.m00; m01 += b.m01; m02 += b.m02;	m10 += b.m10; m11 += b.m11; m12 += b.m12; m20 += b.m20; m21 += b.m21; m22 += b.m22; return *this; }
	//	inline Matrix3i& operator-= (const Matrix3i& b) { m00 -= b.m00; m01 -= b.m01; m02 -= b.m02; m10 -= b.m10; m11 -= b.m11; m12 -= b.m12; m20 -= b.m20; m21 -= b.m21; m22 -= b.m22;	return *this; }
	//	Matrix3i& operator*= (const Matrix3i& rhs);

	//	inline Matrix3i operator+ (const int i) const { Matrix3i result = *this; result += i; return result; }
	//	inline Matrix3i operator- (const int i) const { Matrix3i result = *this; result -= i; return result; }
	//	Matrix3i operator* (const int i) const;
	//	Matrix3i operator/ (const int i) const;

	//	Matrix3i operator+ (const Matrix3i& b) const;
	//	Matrix3i operator- (const Matrix3i& b) const;
	//	Matrix3i operator* (const Matrix3i& b) const;

	//	inline friend Matrix3i operator+ (const int i, const Matrix3i& a) { Matrix3i result = a; result += i; return result; }
	//	inline friend Matrix3i operator- (const int i, const Matrix3i& a) { Matrix3i result = a; result -= i; return result; }
	//	friend const  Vector3i operator* (const Vector3i& v, const Matrix3i& m);
	//	friend const  Matrix3i operator* (const int i, const Matrix3i& a);

	//	inline bool operator== (const Matrix3i& b) const { return m00 == b.m00 && m01 == b.m01 && m02 == b.m02 && m10 == b.m10 && m11 == b.m11 && m12 == b.m12 && m20 == b.m20 && m21 == b.m21 && m22 == b.m22; }
	//	inline bool operator!= (const Matrix3i& b) const { return m00 != b.m00 || m01 != b.m01 || m02 != b.m02 || m10 != b.m10 || m11 != b.m11 || m12 != b.m12 || m20 != b.m20 || m21 != b.m21 || m22 != b.m22; }

	//	inline void		zero() { memset(m, 0, sizeof(int) * 9); }
	//	inline void		invalid() { *this = INVALID; }
	//	inline void		setRow(int row, Vector3i& vec) { assert(row >= 0 && row < 3); int* p = m + row;	p[0] = vec.x; p[1] = vec.y;	p[2] = vec.z; }
	//	inline void		setColumn(int column, Vector3i& vec) { assert(column >= 0 && column < 3); int* p = m + column; p[0] = vec.x; p[3] = vec.y; p[6] = vec.z; }
	//	inline Vector3i getRow(int row) const { assert(row >= 0 && row < 3); const int* p = m + row * 3; return *(Vector3i*)(p); }
	//	inline Vector3i getColumn(int column) const { assert(column >= 0 && column < 3); const int *p = m + column; return Vector3i(p[0], p[3], p[6]); }
	//	inline void		identity() { *this = IDENTITY; }
	//	void			rotateAxis(const Vector3i& axis, int iRadian);
	//	void			rotateAxisReplace(const Vector3i& axis, int iRadian);
	//	void			rotateX(int iRadian);
	//	void			rotateY(int iRadian);
	//	void			rotateZ(int iRadian);
	//	void			rotateXReplace(int iRadian);
	//	void			rotateYReplace(int iRadian);
	//	void			rotateZReplace(int iRadian);
	//	Vector3i		rotateVec3(const Vector3i& vec);
	//	inline void		scale(const Vector3i& scaleVec) { scale(scaleVec[0], scaleVec[1], scaleVec[2]); }
	//	void		scale(int x, int y, int z);
	//	inline void		scaleReplace(const Vector3i& scaleVec) { scaleReplace(scaleVec[0], scaleVec[1], scaleVec[2]); }
	//	inline void		scaleReplace(int x, int y, int z) { m00 = x; m01 = 0; m02 = 0; m10 = 0; m11 = y; m12 = 0; m20 = 0; m21 = 0; m22 = z; }
	//	Vector3i		transform(const Vector3i& vec) const;
	//	inline Matrix3i& transpose() { Math::Swap(m01, m10); Math::Swap(m02, m20); Math::Swap(m12, m21); return *this; }
	//	inline Matrix3i getTranspose() const { Matrix3i mRes = *this; mRes.transpose(); return mRes; }
	//	inline Matrix3i& inverse() { Math::Swap(m01, m10); Math::Swap(m02, m20); Math::Swap(m12, m21); return *this; }
	//	inline void		makeScaling(int x, int y, int z) { identity(); m00 = x; m11 = y; m22 = z; }
	//	inline void		makeScaling(const Vector3i& vec) { makeScaling(vec.x, vec.y, vec.z); }
	//	inline void		noRotate() { m00 = MXI_UNIT; m01 = 0; m02 = 0; m10 = 0; m11 = MXI_UNIT; m12 = 0; m20 = 0; m21 = 0; m22 = MXI_UNIT; }

	//public:
	//	static void		Transpose(Matrix3i& outMat, const Matrix3i& matrix);
	//	static void		TransformVec3(Vector3i& outVec, const Vector3i& v, const Matrix3i& matrix);
	//	static void		TransformNormal(Vector3i& outVec, const Vector3i& v, const Matrix3i& matrix);
	//	static void		Inverse(Matrix3i& outMat, const Matrix3i& matrix);
	//};

	//class LORD_CORE_API Matrix4i
	//{
	//public:
	//	union
	//	{
	//		struct
	//		{
	//			int m00, m01, m02, m03;
	//			int m10, m11, m12, m13;
	//			int m20, m21, m22, m23;
	//			int m30, m31, m32, m33;
	//		};

	//		int m[12];
	//	};

	//	static const Matrix4i ZERO;
	//	static const Matrix4i IDENTITY;
	//	static const Matrix4i INVALID;

	//	inline Matrix4i() { }

	//	inline Matrix4i(int i00, int i01, int i02, int i03, int i10, int i11, int i12, int i13, int i20, int i21, int i22, int i23, int i30, int i31, int i32, int i33)
	//		: m00(i00), m01(i01), m02(i02), m03(i03), m10(i10), m11(i11), m12(i12), m13(i13), m20(i20), m21(i21), m22(i22), m23(i23), m30(i30), m31(i31), m32(i32), m33(i33) {}
	//	inline Matrix4i(int* arr) { memcpy(m, arr, sizeof(int) * 16); }
	//	inline Matrix4i(const Matrix4i& mat) { memcpy(m, mat.m, sizeof(int) * 16); }

	//	inline int operator() (int i, int j) const { assert(i >= 0 && i < 4);	assert(j >= 0 && j < 4); return getRow(i)[j]; }
	//	inline Vector4i operator[] (int row) const { return getRow(row); }
	//	inline Matrix4i& operator= (const Matrix4i& rhs) { memcpy(m, rhs.m, sizeof(int) * 16); return *this; }

	//	inline Matrix4i& operator+= (int i) { for (int i = 0; i < 16; ++i) m[i] += i; return *this; }
	//	inline Matrix4i& operator-= (int i) { for (int i = 0; i < 16; ++i) m[i] -= i; return *this; }
	//	inline Matrix4i& operator*= (int i) { for (int i = 0; i < 16; ++i) { i64 t = i64(m[i]) * i >> MXI_FBIT; assert(t < Math::MAX_INT); m[i] = int(t); } return *this; }
	//	inline Matrix4i& operator/= (int i) { for (int i = 0; i < 16; ++i) { i64 t = (i64(m[i]) << MXI_FBIT) / i; assert(t < Math::MAX_INT); m[i] = int(t); } return *this; }

	//	inline Matrix4i& operator+= (const Matrix4i& rhs) { for (int i = 0; i < 16; ++i) m[i] += rhs.m[i]; return *this; }
	//	inline Matrix4i& operator-= (const Matrix4i& rhs) { for (int i = 0; i < 16; ++i) m[i] -= rhs.m[i]; return *this; }
	//	Matrix4i& operator*= (const Matrix4i& rhs);

	//	inline int* ptr() { return m; }
	//	inline const int* ptr() const { return m; }
	//	inline bool operator== (const Matrix4i& b) const { bool result = true;  for (int i = 0; i < 16; ++i) if (m[i] != b.m[i]) { result = false; break; } return result; }
	//	inline bool operator!= (const Matrix4i& b) const { bool result = false; for (int i = 0; i < 16; ++i) if (m[i] != b.m[i]) { result = true;  break; } return result; }
	//	inline const Matrix4i operator+ (int i) const { Matrix4i result = *this; result += i; return result; }
	//	inline Matrix4i operator- (int i) const { Matrix4i result = *this; result -= i; return result; }

	//	inline friend Matrix4i operator+ (int i, const Matrix4i& a) { Matrix4i result = a; result += i; return result; }
	//	inline friend Matrix4i operator- (int i, const Matrix4i& a) { Matrix4i result = a; result -= i; return result; }

	//	inline Matrix4i operator + (const Matrix4i& b) const { Matrix4i result; for (int i = 0; i < 16; ++i) result.m[i] = m[i] + b.m[i]; return result; }
	//	inline Matrix4i operator - (const Matrix4i& b) const { Matrix4i result; for (int i = 0; i < 16; ++i) result.m[i] = m[i] - b.m[i]; return result; }
	//	friend const Vector4i operator* (const Vector4i& v, const Matrix4i& m);
	//	friend const Vector3i operator* (const Vector3i& v, const Matrix4i& m);
	//	Matrix4i operator* (const Matrix4i& b) const;
	//	inline Matrix4i operator* (int i) const { Matrix4i r; for (int i = 0; i < 16; ++i) { i64 t = i64(m[i])*i >> MXI_FBIT; assert(t < Math::MAX_INT); r.m[i] = int(t); } return r; }
	//	inline Matrix4i operator/ (int i) const { Matrix4i r; for (int i = 0; i < 16; ++i) { i64 t = (i64(m[i]) << MXI_FBIT) / i; assert(t < Math::MAX_INT); r.m[i] = int(t); } return r; }
	//	inline friend const Matrix4i operator* (int i, const Matrix4i& a) { return a*i; }

	//	inline void		zero() { memset(m, 0, sizeof(int) * 16); }
	//	inline void		invalid() { *this = INVALID; }
	//	inline void		setRow(int row, Vector4i& vec) { assert(row >= 0 && row < 4); int* p = m + row;	p[0] = vec.x; p[1] = vec.y;	p[2] = vec.z; p[3] = vec.w; }
	//	inline void		setColumn(int column, Vector4i& vec) { assert(column >= 0 && column < 4); int* p = m + column; p[0] = vec.x; p[4] = vec.y; p[8] = vec.z; p[12] = vec.w; }
	//	inline Vector4i getRow(int row) const { assert(row >= 0 && row < 4); const int* p = m + row * 4; return *(Vector4i*)(p); }
	//	inline Vector4i getColumn(int column) const { assert(column >= 0 && column < 4); const int *p = m + column; return Vector4i(p[0], p[4], p[8], p[12]); }
	//	inline void		identity() { *this = IDENTITY; }
	//	inline void		translate(int x, int y, int z) { m30 += x; m31 += y; m32 += z; }
	//	inline void		translate(const Vector3i& v) { m30 += v.x; m31 += v.y; m32 += v.z; }
	//	inline void		translateReplace(int x, int y, int z) { identity(); m30 = x; m31 = y; m32 = z; }
	//	inline void		translateReplace(const Vector3i& v) { identity(); m30 = v.x; m31 = v.y; m32 = v.z; }
	//	inline void		translateX(int d) { m30 += d; }
	//	inline void		translateY(int d) { m31 += d; }
	//	inline void		translateZ(int d) { m32 += d; }
	//	void			rotateAxis(const Vector3i& axis, int iRadian);
	//	void			rotateAxisReplace(const Vector3i& axis, int iRadian);
	//	void			rotateX(int iRadian);
	//	void			rotateXReplace(int iRadian);
	//	void			rotateY(int iRadian);
	//	void			rotateYReplace(int iRadian);
	//	void			rotateZ(int iRadian);
	//	void			rotateZReplace(int iRadian);
	//	Vector3i		rotateVec3(const Vector3i& vec);
	//	Vector4i		rotateVec4(const Vector4i& vec);
	//	inline void		scale(const Vector3i& scaleVec) { scale(scaleVec[0], scaleVec[1], scaleVec[2]); }
	//	void			scale(int x, int y, int z);
	//	inline void		scaleReplace(const Vector3i& scaleVec) { scaleReplace(scaleVec[0], scaleVec[1], scaleVec[2]); }
	//	inline void		scaleReplace(int x, int y, int z) { zero(); m00 = x; m11 = y; m22 = z; m33 = MXI_FBIT; }
	//	inline Vector4i	transform(const Vector4i& vec) const { return vec * (*this); }
	//	Vector3i		transform(const Vector3i& vec) const;
	//	Vector3i		transformAffine(const Vector3i& vec) const;
	//	inline Vector4i transformNormal(const Vector4i& vec) const { Matrix4i mat = *this; mat.noTranslate(); return vec * mat; }
	//	inline Vector3i transformNormal(const Vector3i& vec) const { Matrix4i mat = *this; mat.noTranslate(); return mat.transform(vec); }
	//	inline Matrix4i& transpose() { Math::Swap(m01, m10); Math::Swap(m02, m20); Math::Swap(m03, m30); Math::Swap(m12, m21); Math::Swap(m13, m31); Math::Swap(m23, m32); return *this; }
	//	Matrix4i&		inverse();
	//	inline void		makeTranslation(int x, int y, int z) { identity(); m30 = x; m31 = y; m32 = z;	m33 = MXI_UNIT; }
	//	inline void		makeTranslation(const Vector3i& vec) { makeTranslation(vec.x, vec.y, vec.z); }
	//	inline void		makeScaling(int x, int y, int z) { identity(); m00 = x; m11 = y; m22 = z; }
	//	inline void		makeScaling(const Vector3i& vec) { makeScaling(vec.x, vec.y, vec.z); }
	//	inline void		noTranslate() { m30 = 0; m31 = 0; m32 = 0; m33 = MXI_UNIT; }
	//	inline void		noRotate() { m00 = MXI_UNIT; m01 = 0; m02 = 0; m10 = 0; m11 = MXI_UNIT; m12 = 0; m20 = 0; m21 = 0; m22 = MXI_UNIT; }
	//	inline bool		isAffine() const { return m30 == 0 && m31 == 0 && m32 == 0 && m33 == MXI_UNIT; }
	//	void			fromQuan(const Quaternioni& quan);

	//public:
	//	static void		Transpose(Matrix4i& outMat, const Matrix4i& matrix);
	//	static void		TransformVec3(Vector3i& outVec, const Vector3i& v, const Matrix4i& matrix);
	//	static void		TransformVec4(Vector4i& outVec, const Vector4i& v, const Matrix4i& matrix);
	//	static void		TransformNormal(Vector3i& outVec, const Vector3i& v, const Matrix4i& matrix);
	//	static void		Inverse(Matrix4i& outMat, const Matrix4i& matrix);
	//	static void		RotateAxis(Matrix4i& outMat, const Vector3i& axis, int iRadian);
	//	static void		RotateYawPitchRoll(Matrix4i& outMat, int iYaw, int iPitch, int iRoll);
	//};
}


#endif
