#include "GUIActionKeyFrame.h"

namespace LORD
{
	GUIActionKeyFrame::GUIActionKeyFrame()
		: m_translate(Vector3::ZERO)
		, m_rotate(Vector3::ZERO)
		, m_scale(Vector3::ONE)
		, m_alpha(1.0f)
		, m_timestamp(0)
	{

	}

	GUIActionKeyFrame::~GUIActionKeyFrame()
	{

	}

}
