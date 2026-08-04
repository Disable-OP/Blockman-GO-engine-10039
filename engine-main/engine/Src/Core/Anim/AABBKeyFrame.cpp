#include "Core.h"
#include "AABBKeyFrame.h"
#include "Animation.h"

namespace LORD
{
	AABBKeyFrame::AABBKeyFrame(Animation* pAnim, ui32 time)
		:AnimKeyFrame(pAnim, time)
	{
	}

	AABBKeyFrame::~AABBKeyFrame()
	{
	}

	void AABBKeyFrame::setBox(const Box &box)
	{
		m_box = box;
	}

	const Box& AABBKeyFrame::getBox() const
	{
		return m_box;
	}
}
