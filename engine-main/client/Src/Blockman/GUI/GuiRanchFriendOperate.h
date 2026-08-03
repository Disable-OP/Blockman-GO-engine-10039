#pragma once
#ifndef __GUI_RANCH_FRIEND_OPERATE_HEADER__
#define __GUI_RANCH_FRIEND_OPERATE_HEADER__
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "UI/GUILayout.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiRanchFriendOperate : public CustomGuiWindow
	{

	private:
		enum class ViewId {
			BTN_HELP,
			BTN_VISIT
		};

	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;

		i64 m_userId = 0;
		bool m_needHelp = false;

	private:
		void removeComponents();

	public:
		GuiRanchFriendOperate(const GUIWindowType& nType, const GUIString& strName);
		~GuiRanchFriendOperate() = default;
		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;
		void setUserId(i64 userId) { m_userId = userId; }
		void setNeedHelp(bool needHelp) { m_needHelp = needHelp; }

	private:
		bool onClick(const EventArgs& args, ViewId viewId);

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};

}
#endif
