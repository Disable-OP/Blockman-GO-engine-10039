#include "GUIWindowMoveAnimator.h"

namespace BLOCKMAN
{
	void GUIWindowMoveAnimator::update(ui64 nTimeElapse)
	{
		m_currentTime += nTimeElapse;

		if (m_finishCallback)
		{
			for (auto pWindow : m_finishList)
			{
				m_finishCallback(pWindow);
			}
		}
		m_finishList.clear();

		while (!m_movingList.empty())
		{
			const auto& info = m_movingList.front();
			if (m_currentTime < info.startTime + DURATION)
			{
				break;
			}
			info.target->SetPosition(info.dstPos);
			m_finishList.push_back(info.target);
			m_movingList.pop_front();
		}

		for (const auto& info : m_movingList)
		{
			auto pastTime = m_currentTime - info.startTime;
			LordAssert(m_currentTime < info.startTime + DURATION);
			auto newPos = info.srcPos;
			scalePositionFromRoot(newPos, static_cast<float>(DURATION - pastTime) / DURATION);
			auto temp = info.dstPos;
			scalePositionFromRoot(temp, static_cast<float>(pastTime) / DURATION);
			newPos += temp;
			info.target->SetPosition(newPos);
		}
	}
}
