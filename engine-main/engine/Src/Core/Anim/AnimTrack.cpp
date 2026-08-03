#include "Core.h"
#include "AnimTrack.h"
#include "Animation.h"

namespace LORD
{
	AnimTrack::AnimTrack(Dword handle)
		:m_handle(handle)
	{
	}

	AnimTrack::~AnimTrack()
	{
	}

	Dword AnimTrack::getHandle() const
	{
		return m_handle;
	}

	/*
	AnimKeyFrame* AnimTrack::getInterpolatedAnimKeyFrame(float time) const
	{

	if(time > m_pAnim->getLength())
	return NULL;

	SRTKeyFrame* pKeyFrame = LordNew SRTKeyFrame(time);

	uint nFrame = (uint)Math::Floor(time * m_pAnim->getFPS());
	uint nNextFrame = (nFrame > (uint)m_keyFrames.size()? nFrame : nFrame + 1);

	// calculate the difference in time between key frames
	// and calculate a scalar value to use for adjusting the transformations
	SRTKeyFrame* pCurKeyFrame = (SRTKeyFrame*)getAnimKeyFrame(nFrame);
	SRTKeyFrame* pNextKeyFrame = (SRTKeyFrame*)getAnimKeyFrame(nNextFrame);

	float diffTime = pNextKeyFrame->getTime() - pCurKeyFrame->getTime();
	LordAssert(diffTime >= 0);

	float t = (time - pCurKeyFrame->getTime()) / diffTime;

	if(m_pAnim->hasScaling())
	{
		Vec3 scaling = pCurKeyFrame->getScaling() + (pNextKeyFrame->getScaling() - pCurKeyFrame->getScaling()) * t;
		pKeyFrame->setScaling(scaling);
	}

	Quan rotation;
	Quan::Lerp(rotation, pCurKeyFrame->getRotation(), pNextKeyFrame->getRotation(), t, true);
	pKeyFrame->setRotation(rotation);

	Vec3 translation = pCurKeyFrame->getTranslation() + (pNextKeyFrame->getTranslation() - pCurKeyFrame->getTranslation()) * t;
	pKeyFrame->setTranslation(translation);

	return pKeyFrame;
	}
	*/
}