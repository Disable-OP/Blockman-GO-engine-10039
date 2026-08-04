/********************************************************************
filename: 	GUIActionKeyFrame.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef __GUI_ACTION_KEY_FRAME_H__
#define __GUI_ACTION_KEY_FRAME_H__

#include "Core.h"

namespace LORD
{
	class LORD_CORE_API GUIActionKeyFrame: public ObjectAlloc
	{
	public:
		GUIActionKeyFrame();
		~GUIActionKeyFrame();

		// 平移
		Vector3			m_translate;

		// 旋转分别是绕x，y，z轴的转角
		Vector3			m_rotate;

		// 缩放
		Vector3			m_scale;

		// alpha
		float			m_alpha;

		// time stamp
		ui32			m_timestamp;
	};
}

#endif