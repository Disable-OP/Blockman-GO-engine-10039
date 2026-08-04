#pragma once
#ifndef __GUI_RANCH_BUILD_FACTORY_QUEUE_HEADER__
#define __GUI_RANCH_BUILD_FACTORY_QUEUE_HEADER__
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIButton.h"
#include "UI/GUIStaticText.h"
#include "UI/GUILayout.h"
#include "Entity/EntityBuildNpc.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiRanchBuildFactoryQueue : public CustomGuiWindow
	{
	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;
		GUIStaticImage* m_ivIconbg = nullptr;
		GUIStaticImage* m_ivIcon = nullptr;
		ProductQueue m_productQueue;
		

	private:
		void removeComponents();

	public:
		GuiRanchBuildFactoryQueue(const GUIWindowType& nType, const GUIString& strName);
		~GuiRanchBuildFactoryQueue() = default;
		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;

		void setProductQueue(ProductQueue productQueue);

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};

}
#endif
