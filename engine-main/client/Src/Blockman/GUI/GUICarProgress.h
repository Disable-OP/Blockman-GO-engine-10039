#ifndef __GUI_CAR_PROGRESS_HEADER__
#define __GUI_CAR_PROGRESS_HEADER__

#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "GUI/CustomGuiWindow.h"

using namespace LORD;

namespace BLOCKMAN
{
	enum CarProgressStyle
	{
		Yellow = 0,
		Green,
		Red
	};

	enum CarProgressDirection
	{
		Right,
		Left
	};

	class GUICarProgress : public CustomGuiWindow
	{
	private:
		GUIWindow* m_window = nullptr;
		GUIStaticImage* m_thumb = nullptr;
		CarProgressStyle m_style = Yellow;
		CarProgressDirection m_direction = Right;

	private:
		using BaseClass = CustomGuiWindow;
		void removeComponents();

	public:
		GUICarProgress(const GUIWindowType& nType, const GUIString& strName);
		~GUICarProgress() = default;

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;
		
		void InitTeamIcon();
		void SetStyle(CarProgressStyle style);
		void SetDirection(CarProgressDirection direction);
		void SetProgress(float progress);

	};
}

#endif