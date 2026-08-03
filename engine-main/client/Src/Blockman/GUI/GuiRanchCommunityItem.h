#pragma once
#ifndef __GUI_RANCH_COMMUNITY_ITEM_HEADER__
#define __GUI_RANCH_COMMUNITY_ITEM_HEADER__
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUILayout.h"
#include "World/Ranch.h"
#include "UI/GUIButton.h"
#include "Util/FriendManager.h"
#include "GUI/GuiUrlImage.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiRanchCommunityItem : public CustomGuiWindow
	{

	private:
		enum class ViewId {
			BTN_ICON,
			BTN_SUER
		};

	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;
		GUIStaticText* m_tvDesc = nullptr;
		GUIStaticText* m_tvProsperityValue = nullptr;

		GUIStaticText* m_tvName = nullptr;
		GUILayout* m_ivIcon = nullptr;
		GUIStaticImage* m_ivHelp = nullptr;

		GuiUrlImage* m_ivBackground = nullptr;
		GUIStaticImage* m_ivGiveIcon = nullptr;
		GUIStaticImage* m_ivLevelIcon = nullptr;

		GUIStaticText* m_tvGiveNum = nullptr;
		GUIStaticText* m_tvLevel = nullptr;

		GUIButton* m_btnSure = nullptr;
		GUILayout* m_giveLayout = nullptr;
		GUILayout* m_prosperityLayout = nullptr;

		i64 m_friendId = 0;

	private:
		void removeComponents();

	private:
		void refresh(bool hasRanch);

	public:
		GuiRanchCommunityItem(const GUIWindowType& nType, const GUIString& strName);
		~GuiRanchCommunityItem() = default;
		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;
		bool onClick(const EventArgs& args, ViewId viewId);
		void setFriendId(i64 userId);

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};

}
#endif
