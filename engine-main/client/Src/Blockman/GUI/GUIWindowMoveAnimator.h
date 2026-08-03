#ifndef __GUI_WINDOW_MOVE_ANIMATOR_HEADER__
#define __GUI_WINDOW_MOVE_ANIMATOR_HEADER__

#include "Core.h"
#include "UI/GUIWindow.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUIWindowMoveAnimator
	{
	public:
		using FinishCallback = std::function<void(GUIWindow*)>;

	private:
		static constexpr ui64 DURATION = 300;

		struct MotionInfo
		{
			GUIWindow* target;
			UVector2 srcPos;
			UVector2 dstPos;
			ui64 startTime;
		};

		ui64 m_currentTime = 0;
		deque<MotionInfo>::type m_movingList;
		vector<GUIWindow*>::type m_finishList;
		FinishCallback m_finishCallback;

		static void scalePositionFromRoot(UVector2& pos, float scale)
		{
			pos.d_x.d_scale *= scale;
			pos.d_x.d_offset *= scale;
			pos.d_y.d_scale *= scale;
			pos.d_y.d_offset *= scale;
		}
		
	public:
		void startMove(GUIWindow* target, const UVector2& to)
		{
			m_movingList.push_back({ target, target->GetPosition(), to, m_currentTime });
		}

		void onFinish(FinishCallback callback)
		{
			m_finishCallback = callback;
		}

		void update(ui64 nTimeElapse);
	};
}

#endif // !__GUI_WINDOW_MOVE_ANIMATOR_HEADER__
