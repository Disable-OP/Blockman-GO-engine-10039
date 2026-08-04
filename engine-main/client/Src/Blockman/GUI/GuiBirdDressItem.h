#pragma once
#ifndef __GUI_BIRD_DRESS_ITEM_HEADER__
#define __GUI_BIRD_DRESS_ITEM_HEADER__
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUILayout.h"
#include "World/BirdSimulator.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiBirdDressItem : public CustomGuiWindow
	{
	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;

		GUIStaticImage* m_image = nullptr;
		GUIStaticImage* m_greenDot = nullptr;
		BirdDress m_birdDress;
		i64 m_birdId = 0;

	private:
		void removeComponents();

	public:
		GuiBirdDressItem(const GUIWindowType& nType, const GUIString& strName);
		~GuiBirdDressItem() = default;

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;


		void setDress(const BirdDress& birdDress);
		void setDressImage(const String& imageName);
		void setUse(bool isUse);
		void setBirdId(i64 birdId) { m_birdId = birdId; }
		bool onClick(const EventArgs& args);

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};

}
#endif
