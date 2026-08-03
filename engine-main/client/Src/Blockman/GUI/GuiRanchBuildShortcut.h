#pragma once
#ifndef __GUI_RANCH_BUILD_SHORTCUT_HEADER__
#define __GUI_RANCH_BUILD_SHORTCUT_HEADER__
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUILayout.h"
#include "Entity/EntityBuildNpc.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiRanchBuildShortcut : public CustomGuiWindow
	{
	public:
		enum class VIewId
		{
			SURE
		};

	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;
		GUIStaticText* m_tvTimeValue = nullptr;
		GUIStaticText* m_tvPrice = nullptr;
		ProductQueue m_productQueue;

	private:
		void removeComponents();

	private:
		i32 m_entityId = 0;
		i32 m_price = 0;

	public:
		GuiRanchBuildShortcut(const GUIWindowType& nType, const GUIString& strName);
		~GuiRanchBuildShortcut() = default;
		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;
		bool onClick(const EventArgs& args, VIewId viewId);
		void setQueue(const ProductQueue& queue) { m_productQueue = queue; }
		void setEntityId(i32 entityId) { m_entityId = entityId; }
		
	private:
		String timeFormat();
		

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};

}
#endif
