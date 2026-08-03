#pragma once
#include "UI/GUIWindow.h"
#include "UI/GUIButton.h"
#include "UI/GUIStaticImage.h"
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIStaticText.h"
#include <vector>

using namespace LORD;

namespace BLOCKMAN
{
	class GUIPixelGunHallModeSelectMapItem : public CustomGuiWindow
	{
	public:
		enum MapType
		{
			MT_INVALID = 0,
			MT_RANDOM = 1,
			MT_FRAGMENT = 2,
		};
		
		enum MapOpenType
		{
			MOT_INVALID = 0,
			MOT_LV = 1,
			MOT_DIAMOND = 2,
			MOT_NOTOPEN = 3,
		};

		struct MapItemInfo
		{
			MapItemInfo() {}
			String map_name = "";
			String map_show_name = "";
			String map_img = "";
			String small_info_img = "";
			int map_id = 0;
			bool unlock = false;
			String map_size = "";
			int openMoneyType = 0;
			int openMoneyNum = 0;
			int map_type = 0;
		};

	private:

		void removeComponents();

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;

	public:
		GUIPixelGunHallModeSelectMapItem(const GUIWindowType& nType, const GUIString& strName);
		~GUIPixelGunHallModeSelectMapItem() = default;

		

		void Destroy() override;
		void UpdateSelf(ui32 nTimeElapse) override;
		void RefreshItem(MapItemInfo& map_info, int index);
		bool onMapImgClick();
		void updateSelect(bool select);
		

	private:
 		int m_select_index = 0;

		MapItemInfo m_map_info;

		using BaseClass = CustomGuiWindow;
		GUIWindow* m_window = nullptr;

		GUIStaticImage* m_map_di = nullptr;
		GUIStaticText* m_map_name = nullptr;
		GUIStaticImage* m_map_img = nullptr;
		GUIStaticImage* m_map_lock_meng = nullptr;
		GUIStaticImage* m_map_lock = nullptr;
		GUIStaticText* m_map_size = nullptr;
		GUIStaticImage* m_map_small_info = nullptr;
		GUIStaticImage* m_map_select = nullptr;
	};
}
#pragma once
