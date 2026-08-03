#pragma once
#ifndef __GUI_BIRD_FUSE_HEADER__
#define __GUI_BIRD_FUSE_HEADER__
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUILayout.h"
#include "UI/GUIButton.h"
#include "Util/SubscriptionGuard.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiBirdFuse : public CustomGuiWindow
	{
	private:
		enum class ViewId {
			BTN_FIRST,
			BTN_SECOND,
			BTN_THIRD,
			BTN_SURE,
			BTN_CANCEL
		};

	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;

		GUIButton* m_btnSure = nullptr;
		GUIButton* m_btnCancel = nullptr;
		GUIStaticImage* m_firstBird = nullptr;
		GUIStaticImage* m_secondBird = nullptr;
		GUIStaticImage* m_thirdBird = nullptr;

		SubscriptionGuard m_subscriptionGuard;
		i64 m_firstBirdId = 0;
		i64 m_secondBirdId = 0;
		i64 m_thirdBirdId = 0;

	private:
		void removeComponents();

	public:
		GuiBirdFuse(const GUIWindowType& nType, const GUIString& strName);
		~GuiBirdFuse() = default;

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;
		bool onClick(const EventArgs& args, ViewId viewId);
		void itemRemove(i64 birdId);
		bool onPutBird(i64 birdId, const String & icon);
		void removeBirdFuse(ViewId viewId);
		void removeAllItems();
		bool removeFuseItem(i64 birdId);

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};

}
#endif
