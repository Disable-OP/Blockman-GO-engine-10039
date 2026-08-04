#ifndef __GUI_SEASON_HONOR_ITEM_HEADER__
#define __GUI_SEASON_HONOR_ITEM_HEADER__

#include "UI/GUIWindow.h"
#include "GUI/CustomGuiWindow.h"

using namespace LORD;

namespace BLOCKMAN
{
	enum HonorItemType
	{
		LAST_SEASON,
		CURRENT_SEASON
	};

	class GUISeasonHonorItem : public CustomGuiWindow
	{
	private:
		GUIWindow* m_window = nullptr;

	private:
		using BaseClass = CustomGuiWindow;
		void removeComponents();

	public:
		GUISeasonHonorItem(const GUIWindowType& nType, const GUIString& strName);
		~GUISeasonHonorItem() = default;

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;

		void SetHonorInfo(HonorItemType type, int honorId, int rank, int honor);

	};
}

#endif