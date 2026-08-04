#include "GUIAction.h"

namespace LORD
{


	GUIAction::GUIAction()
		:m_actionTimeStamp(0)
	{
		clear();
	}

	GUIAction::~GUIAction()
	{
		clear();
	}

	void GUIAction::clear()
	{
		m_actionTimeStamp = 0;

		vector<GUIActionKeyFrame*>::type::iterator iter = m_keyFrameList.begin();
		for (; iter != m_keyFrameList.end(); ++iter)
		{
			if (*iter)
			{
				LordSafeDelete(*iter);
			}
		}

		m_keyFrameList.clear();
	}

	void GUIAction::addKeyFrame(const Vector3& translate, const Vector3& rotate, const Vector3& scale, float alpha, ui32 timeStamp)
	{
		GUIActionKeyFrame* pKeyFrame = LordNew GUIActionKeyFrame;
		if (!pKeyFrame)
		{
			return;
		}

		pKeyFrame->m_translate = translate;
		//转成弧度
		pKeyFrame->m_rotate = rotate * Math::DEG2RAD;
		pKeyFrame->m_scale = scale;
		pKeyFrame->m_alpha = alpha;
		pKeyFrame->m_timestamp = timeStamp;

		m_keyFrameList.push_back(pKeyFrame);
	}

	bool GUIAction::update(ui32 dt)
	{
		// 返回是不是结束了
		vector<GUIActionKeyFrame*>::type::iterator iter = m_keyFrameList.begin();

		for (; iter != m_keyFrameList.end(); ++iter)
		{
			if (m_actionTimeStamp <= (*iter)->m_timestamp)
			{
				// 找到当前处于哪个关键帧
				break;
			}
		}

		// 时间超过最后一帧了
		if (iter == m_keyFrameList.end())
		{
			if (m_keyFrameList.size() > 0 )
			{
				//设置成最后一帧
				m_currentFrameData = *(*(m_keyFrameList.end() - 1));
			}

			clear();
			return true;
		}

		// 第一帧不是0开始的，再等等
		if (iter == m_keyFrameList.begin())
		{
			m_currentFrameData = *(*iter);

			m_actionTimeStamp += dt;
			return false;
		}


		// 在两帧之间差值
		vector<GUIActionKeyFrame*>::type::iterator preIter = iter - 1;

		float t = (m_actionTimeStamp - (*preIter)->m_timestamp) * 1.0f / ((*iter)->m_timestamp - (*preIter)->m_timestamp );

		m_currentFrameData.m_translate = (*preIter)->m_translate + t * ((*iter)->m_translate - (*preIter)->m_translate);
		m_currentFrameData.m_rotate = (*preIter)->m_rotate + t * ((*iter)->m_rotate - (*preIter)->m_rotate);
		m_currentFrameData.m_scale = (*preIter)->m_scale + t * ((*iter)->m_scale - (*preIter)->m_scale);
		m_currentFrameData.m_alpha = (*preIter)->m_alpha + t * ((*iter)->m_alpha - (*preIter)->m_alpha);

		m_actionTimeStamp += dt;
		return false;
	}

}
