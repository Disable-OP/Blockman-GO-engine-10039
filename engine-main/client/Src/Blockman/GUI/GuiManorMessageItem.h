#pragma once
#ifndef __GUI_MANOR_MESSAGE_ITEM_HEADER__
#define __GUI_MANOR_MESSAGE_ITEM_HEADER__
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUILayout.h"
#include "UI/GUIButton.h"
#include "World/Manor.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiManorMessageItem : public CustomGuiWindow
	{
	public:
		enum class ViewId
		{
			BTN_GIVE,
			BTN_CALL_ON,
			BTN_DELETE
		};

	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;
		GUILayout* m_vegetable = nullptr;
		GUIButton* m_btnGive = nullptr;
		GUIButton* m_btnCallOn = nullptr;
		GUIButton* m_btnDelete = nullptr;
		GUIStaticText* m_messageTip = nullptr;
		GUIStaticText* m_vegetableValue = nullptr;
		GUIStaticImage* m_vegetableIcon = nullptr;
		ManorMessage* m_manorMessage = nullptr;
		bool m_isDelete = false;
		bool m_isChange = false;

	private:
		void removeComponents();

	public:
		GuiManorMessageItem(const GUIWindowType& nType, const GUIString& strName);
		~GuiManorMessageItem() = default;
		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;
		bool onClick(const EventArgs & events, ViewId viewId);
		void setManorMessage(ManorMessage* message)
		{
			this->m_manorMessage = message;
			this->m_isChange = true;
		}

	private:
		void updateIcon(int itemId, int meta);

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};

}
#endif
