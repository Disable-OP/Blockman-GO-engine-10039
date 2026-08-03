#include "Box3.h"

namespace LORD
{
	// initialized
	Box3::Box3( const Vector3& center, const Vector3& axis0, const Vector3& axis1, const Vector3& axis2, float extent0, float extent1, float extent2)
	{
		Set( center, axis0, axis1, axis2, extent0, extent1, extent2);
	}

	// Set
	void Box3::Set( const Vector3& center, const Vector3& axis0, const Vector3& axis1, const Vector3& axis2, float extent0, float extent1, float extent2)
	{
		m_center = center;

		m_axis[0] = axis0;
		m_axis[1] = axis1;
		m_axis[2] = axis2;

		m_axis[0].normalize();
		m_axis[1].normalize();
		m_axis[2].normalize();

		m_extent[0] = std::abs(extent0);
		m_extent[1] = std::abs(extent1);
		m_extent[2] = std::abs(extent2);
	}
}