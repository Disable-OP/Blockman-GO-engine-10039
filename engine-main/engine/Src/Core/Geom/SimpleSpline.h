/********************************************************************
filename: 	SimpleSpline.h
file path:	dev\engine\Src\Core\Geom\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_SPLINE_H__
#define __LORD_SPLINE_H__

#include "Core.h"
#include "Math/Matrix4.h"

namespace LORD
{
	class LORD_CORE_API SimpleSpline : public ObjectAlloc
	{
	public:
		SimpleSpline();
		~SimpleSpline();

		void addPoint(const Vector3& p);
		void delPoint(int index);
		const Vector3& getPoint(unsigned short index) const;
		unsigned short getNumPoints(void) const;
		void clear(void);
		void updatePoint(unsigned short index, const Vector3& value);
		Vector3 interpolate(Real t) const;
		Vector3 interpolate(unsigned int fromIndex, Real t) const;
		void setAutoCalculate(bool autoCalc);
		void recalcTangents(void);

	protected:
		bool mAutoCalc;
		vector<Vector3>::type mPoints;
		vector<Vector3>::type mTangents;
		Matrix4 mCoeffs;
	};
}
#endif

