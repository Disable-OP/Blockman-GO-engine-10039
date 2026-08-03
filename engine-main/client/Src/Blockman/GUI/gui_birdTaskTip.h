#pragma once
#ifndef __GUI_BIRD_TASK_TIP_HEADER__
#define __GUI_BIRD_TASK_TIP_HEADER__
#include "GUI/gui_layout.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_birdTaskTip : public gui_layout
	{
		enum class SessionType
		{
			DEFAULT,
			BRID_TASK = 104101,
		};


	public:
		gui_birdTaskTip();
		~gui_birdTaskTip();

		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool onClick(const EventArgs);
		bool open(const String& tips);

	private:
		void parseData(const String& tips);


	private:
		vector<String>::type m_tips = vector<String>::type();
		SessionType m_sessionType = SessionType::DEFAULT;
		i32 m_page = 1;
		i32 m_entityId = 0;
		i32 m_taskId = 0;
		i32 m_taskStatus = 0;

	private:
		GUIStaticText*		m_titleText = nullptr;
		GUIStaticText*		m_messageText = nullptr;
	};
}
#endif
