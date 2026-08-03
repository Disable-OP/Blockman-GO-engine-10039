#pragma once
#ifndef __GUI_BIRD_LOTTERY_ITEM_HEADER__
#define __GUI_BIRD_LOTTERY_ITEM_HEADER__
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUILayout.h"
#include "World/BirdSimulator.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiBirdLotteryItem : public CustomGuiWindow
	{
	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;
		GUIStaticImage* m_image = nullptr;
		GUIStaticText* m_pro = nullptr;
		GUIStaticText* m_quality = nullptr;

	private:
		void removeComponents();

	public:
		GuiBirdLotteryItem(const GUIWindowType& nType, const GUIString& strName);
		~GuiBirdLotteryItem() = default;

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;
		void setBirdEgg(const BirdEgg& birdEgg);
		void setNameColor(i32 quality);
		void setAtlasItem(const BirdEgg& birdEgg);

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};

}
#endif
