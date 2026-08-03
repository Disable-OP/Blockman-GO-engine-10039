#include "AnimKeyFrame.h"
#include "Animation.h"
#include "SRTTrack.h"

namespace LORD
{
	AnimKeyFrame::AnimKeyFrame(Animation* pAnim, ui32 time)
		:m_pAnim(pAnim)
		,m_time(time)
		,m_KeyOrientation(NULL)
		,m_KeyPosition(NULL)
	{
	}

	AnimKeyFrame::~AnimKeyFrame()
	{
		if (m_KeyOrientation)
		{
			LordFree(m_KeyOrientation);
			m_KeyOrientation = NULL;
		}
		if (m_KeyPosition)
		{
			LordFree(m_KeyPosition);
			m_KeyPosition = NULL;
		}
	}

	void AnimKeyFrame::createAnimData( Dword boneCount )
	{
		assert(m_KeyOrientation==NULL && m_KeyPosition==NULL);
		m_KeyOrientation = static_cast<Quaternion*>(LordMalloc(boneCount*sizeof(Quaternion)));
		m_KeyPosition = static_cast<Vector3*>(LordMalloc(boneCount*sizeof(Vector3)));
	}
}