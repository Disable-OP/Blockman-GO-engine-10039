#include "Core.h"
#include "AnimState.h"
#include "Animation.h"
#include "AnimTrack.h"

namespace LORD
{
	AnimState::AnimState(Animation *pAnim)
		:m_pAnim(pAnim)
		, m_bEnable(false)
		, m_bLoop(false)
		, m_length(0)
		, m_timePos(0)
		, m_nFrame(0)
		, m_timeScale(1.f)
	{
		LordAssert(pAnim);
		m_length = pAnim->getLength();
	}

	AnimState::~AnimState()
	{
	}

	void AnimState::enable(bool bEnable)
	{
		m_bEnable = bEnable;
	}

	void AnimState::enableLoop(bool bEnable)
	{
		m_bLoop = bEnable;
	}

	void AnimState::setLength(ui32 length)
	{
		m_length = length;
	}

	void AnimState::setTimePosition(ui32 timePos)
	{
		m_timePos = timePos;
		m_nFrame = m_pAnim->getFrameIndex(m_timePos);
	}

	Animation* AnimState::getAnim() const
	{
		return m_pAnim;
	}

	bool AnimState::isEnable() const
	{
		return m_bEnable;
	}

	ui32 AnimState::getLength() const
	{
		return m_length;
	}

	bool AnimState::isLoop() const
	{
		return m_bLoop;
	}

	ui32 AnimState::getTimePosition() const
	{
		return m_timePos;
	}

	uint AnimState::getCurrentFrameIndex() const
	{
		return m_nFrame;
	}

	LORD::uint AnimState::getNextFrameIndex() const
	{
		return std::min<uint>(m_nFrame + 1, m_pAnim->getFrameCount() - 1);
	}

	AnimKeyFrame* AnimState::getCurrentAnimKeyFrame() const
	{
		return m_pAnim->getAnimKeyFrameByIdx(m_nFrame);
	}

	AnimKeyFrame* AnimState::getAnimKeyFrame(uint index) const
	{
		return m_pAnim->getAnimKeyFrameByIdx(index);
	}

	void AnimState::frameMove(ui32 deltaTime)
	{
		if (!m_bEnable)
			return;

		// calculate current time
		m_timePos += int(deltaTime * m_timeScale);

		if (m_bLoop)
		{
			m_timePos = (m_timePos) % (m_length + 1);
		}
		else
		{
			if (m_timePos > m_length)
			{
				m_timePos = m_length;
				m_bEnable = false;
			}
		}

		m_nFrame = m_pAnim->getFrameIndex(m_timePos);
	}
}