/********************************************************************
filename: 	GUIAction.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef __GUI_ACTION_H__
#define __GUI_ACTION_H__

#include "Core.h"
#include "GUIActionKeyFrame.h"

namespace LORD
{
	class LORD_CORE_API GUIAction: public ObjectAlloc
	{
	public:
		GUIAction();
		~GUIAction();

		void	clear();

		void	addKeyFrame(const Vector3& translate, const Vector3& rotate, const Vector3& scale, float alpha, ui32 timeStamp);

		bool	update(ui32 dt);

		const GUIActionKeyFrame& getCurrentFrameData() const { return m_currentFrameData; }
		
	private:

		vector<GUIActionKeyFrame*>::type	m_keyFrameList;
		GUIActionKeyFrame					m_currentFrameData;
		ui32								m_actionTimeStamp;
	};
}

#endif