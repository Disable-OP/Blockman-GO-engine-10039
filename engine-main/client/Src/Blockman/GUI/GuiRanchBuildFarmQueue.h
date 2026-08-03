#pragma once
#ifndef __GUI_RANCH_BUILD_FARM_QUEUE_HEADER__
#define __GUI_RANCH_BUILD_FARM_QUEUE_HEADER__
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIButton.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIProgressBar.h"
#include "UI/GUILayout.h"
#include "Entity/EntityBuildNpc.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiRanchBuildFarmQueue : public CustomGuiWindow
	{
	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;
		GUILayout* m_leftTimeLayout = nullptr;
		GUILayout* m_buyTimeLayout = nullptr;
		GUIStaticText* m_tvTimePrice = nullptr;
		GUIProgressBar* m_pbTimeBar = nullptr;

		GUIStaticImage* m_ivIconbg = nullptr;
		GUIStaticImage* m_ivIcon = nullptr;
		ProductQueue m_productQueue;
		i32 m_entityId = 0;

	private:
		void removeComponents();

	public:
		GuiRanchBuildFarmQueue(const GUIWindowType& nType, const GUIString& strName);
		~GuiRanchBuildFarmQueue() = default;
		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;

		void setProductQueue(ProductQueue productQueue);
		void setEntityId(i32 entityId) { m_entityId = entityId; }

	private:
		bool onClick(const EventArgs& args);

		String timeFormat();

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};

}
#endif
