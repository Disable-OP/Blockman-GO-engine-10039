#pragma once
#ifndef __GUI_BIRD_TASK_ITEM_HEADER__
#define __GUI_BIRD_TASK_ITEM_HEADER__
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticText.h"
#include "UI/GUILayout.h"
#include "UI/GUIProgressBar.h"

using namespace LORD;

namespace BLOCKMAN
{

	class GuiBirdTaskItem : public CustomGuiWindow
	{
	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;
		GUILayout* m_title = nullptr;
		GUILayout* m_content = nullptr;
		GUIProgressBar* m_progressBar = nullptr;
		GUIStaticText* m_tvTitleName = nullptr;
		GUIStaticText* m_tvTaskInfo = nullptr;
		i32 m_taskId = 0;
		i32 m_taskItemId = 0;

	private:
		void removeComponents();

	public:
		GuiBirdTaskItem(const GUIWindowType& nType, const GUIString& strName);
		~GuiBirdTaskItem() = default;

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;
		void setTask(i32 taskId, i32 itemId) { m_taskId = taskId; m_taskItemId = itemId; }
		void setTitle(const String& title);
		void setTaskInfo(const String& info);
		void setProgress(float progress);

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};

}
#endif
