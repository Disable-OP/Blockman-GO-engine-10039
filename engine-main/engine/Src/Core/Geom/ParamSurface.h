/********************************************************************
filename: 	ParamSurface.h
file path:	dev\engine\Src\Core\Geom\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_PARAMSURFACE_H__
#define __LORD_PARAMSURFACE_H__

#include "Core.h"
#include "Object/Buffer.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"

namespace LORD
{
	class LORD_CORE_API ParamSurface
	{
	public:
		ParamSurface(const Vector2& vDivision, const Vector2& vUpperBound, const Vector2& vTcrdWeight);
		virtual ~ParamSurface();

	public:
		uint			getVertexCount() const;
		uint			getLineIndexCount() const;
		uint			getTriangleIndexCount() const;

		void			generateVertices(Buffer& buff, bool bNormal = true, bool bTexCoord = true) const;
		void			generateLineIndices(Buffer& buff, uint& stride) const;
		void			generateTriangleIndices(Buffer& buff, uint& stride) const;

	protected:
		virtual Vector3	evaluate(const Vector2& domain) const = 0;
		virtual bool	isNormalInverse(const Vector2& vDomain) const;
		Vector2			computeDomain(Real i, Real j) const;

	protected:
		Vector2	m_vDivision;
		Vector2	m_vUpperBound;
		Vector2	m_vTcrdWeight;
		Vector2	m_vSlice;
	};

	class LORD_CORE_API SphereSurface: public ParamSurface
	{
	public:
		SphereSurface(	Real radius,
			const Vector2& vDivision = Vector2(20, 20),
			const Vector2& vUpperBound = Vector2(Math::PI, Math::PI_2),
			const Vector2& vTcrdWeight = Vector2(20, 35));

	private:
		Vector3		evaluate(const Vector2& vDomain) const;

	private:
		Real		m_radius;
	};
}

#endif