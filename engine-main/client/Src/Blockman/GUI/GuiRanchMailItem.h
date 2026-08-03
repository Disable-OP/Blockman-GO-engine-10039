#pragma once
#ifndef __GUI_RANCH_MAIL_ITEM_HEADER__
#define __GUI_RANCH_MAIL_ITEM_HEADER__
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUILayout.h"
#include "World/Ranch.h"
#include "UI/GUIButton.h"
#include "Util/FriendManager.h"
#include "Util/MailManager.h"
#include "GUI/GuiUrlImage.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiRanchMailItem : public CustomGuiWindow
	{

	private:
		enum class ViewId {
			BTN_ICON,
			BTN_SUER
		};

	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;
		GUIStaticText* m_tvTitle = nullptr;
		GUIStaticText* m_tvProsperityValue = nullptr;

		GUIStaticText* m_tvContent = nullptr;

		GUIStaticText* m_tvName = nullptr;
		GuiUrlImage* m_ivIcon = nullptr;


		GUIStaticImage* m_ivGiveIcon = nullptr;
		GUIStaticImage* m_ivLevelIcon = nullptr;

		GUIStaticText* m_tvGiveNum = nullptr;
		GUIStaticText* m_tvLevel = nullptr;

		GUIButton* m_btnSure = nullptr;
		GUILayout* m_giveLayout = nullptr;
		GUILayout* m_prosperityLayout = nullptr;

		Friend* m_friend = nullptr;
		i32 m_mailType = 0;
		MailInfo m_mail;
		String m_msg = "";

	private:
		void removeComponents();

	private:
		void refresh(bool hasRanch);

	public:
		GuiRanchMailItem(const GUIWindowType& nType, const GUIString& strName);
		~GuiRanchMailItem() = default;
		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;
		bool onClick(const EventArgs& args, ViewId viewId);
		void setMail(const MailInfo& mailInfo);
		
	private:
		void parseContent(String data);

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};

}
#endif
