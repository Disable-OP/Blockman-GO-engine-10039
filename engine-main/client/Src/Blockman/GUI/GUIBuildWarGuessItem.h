#pragma once
#include "UI/GUIWindow.h"
#include "UI/GUIButton.h"
#include "UI/GUIStaticImage.h"
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIStaticText.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiBuildWarGuessItem : public CustomGuiWindow
	{
	private:
		using BaseClass = CustomGuiWindow;
		GUIWindow* m_window = nullptr;
		GUIStaticImage* m_HeartImage = nullptr;
		GUIStaticText* m_nameTxt = nullptr;
		GUIStaticText* m_roomTxt = nullptr;
		GUIButton* m_visitBtn = nullptr;
		GUIButton* m_guessBtn = nullptr;

		void removeComponents();

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;

	public:
		GuiBuildWarGuessItem(const GUIWindowType& nType, const GUIString& strName);
		~GuiBuildWarGuessItem() = default;

		void Destroy() override;
		void UpdateSelf(ui32 nTimeElapse) override;
		void RefreshItem(bool is_heart, int room, String name, int guess_room_id, ui64 user_id);
		bool onClickVisit(const EventArgs&);
		bool onClickGuess(const EventArgs&);

	private:
		String m_name = "";
		int m_roomId = 0;
		ui64 m_userId = 0;
	};
}
#pragma once
