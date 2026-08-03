#pragma once
#ifndef __GUI_ENCHANTMENT_PANEL_HEADER__
#define __GUI_ENCHANTMENT_PANEL_HEADER__

#include "gui_layout.h"
#include "UI/GUIWindow.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIButton.h"
#include "GUI/RootGuiLayout.h"
#include "gui_layout.h"
#include <vector>

using namespace LORD;

namespace BLOCKMAN
{
	class gui_enchantmentPanel : public  gui_layout
	{
	private:
		SubscriptionGuard m_subscriptionGuard;

	public:
		gui_enchantmentPanel();
		virtual ~gui_enchantmentPanel();
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		void onShow() override;

		RootGuiLayout* getParent() { return dynamic_cast<RootGuiLayout*>(m_parentLayout); }

		static const int MAX_ENAHTMENT_EFFECT_NUM = 4;
		static const int MAX_ENANTMENT_BAG_NUM = 36;
		static const int MAX_ENANTMENT_BAG_COLUMN = 7;
		static const int MAX_ENANTMENT_BAG_ROW = 6;

		struct EffectDesInfo
		{
			String title;
			String des;
		};
		
		enum RefreshType
		{
			RT_OPEN = 0,
			RT_ENCHANTMENT,
		};

		bool onCloseClick(const EventArgs & events);
		bool onEquipClick(const EventArgs & events);
		bool onEnchantmentBtn(const EventArgs & events);
		bool onEnchantmentEffectBtn(const EventArgs & events, int index);
		bool onBagStackClick(const EventArgs & events, int index);
		bool refreshAll(const String& data);
		bool getAllInfo(const String& data, int & equptId, int & consumeId, int & consumeNum, std::vector<int>& bag, std::vector<EffectDesInfo>& effect_info,
			bool & hItemhaveItemEnchantment, int & refresh_type, int & diamond, String & consumeBg);
		String getItemImg(int itemId);
		void refreshAllSelf();

	private:
		GUIStaticImage* m_equipment = nullptr;
		GUIStaticImage* m_equipmentImg = nullptr;
		GUIStaticImage* m_consume = nullptr;
		GUIStaticImage* m_consumeImg = nullptr;
		GUIButton* m_enchantmentEffectBtn[MAX_ENAHTMENT_EFFECT_NUM];
		GUIStaticImage *m_enchantmentEffectSelect[MAX_ENAHTMENT_EFFECT_NUM];
		GUIStaticText *m_enchantmentEffectTitle[MAX_ENAHTMENT_EFFECT_NUM];
		GUIStaticText *m_enchantmentEffectDes[MAX_ENAHTMENT_EFFECT_NUM];
		GUIStaticImage *m_enchantmentEffectImg[MAX_ENAHTMENT_EFFECT_NUM];
		GUIStaticImage *m_enchantmentEffectBg[MAX_ENAHTMENT_EFFECT_NUM];
		GUIButton* m_closeBtn = nullptr;
		GUIButton* m_enchantmentBtn = nullptr;
		GUILayout* m_bagLayout = nullptr;
		GUIStaticImage* m_bagItemStack[MAX_ENANTMENT_BAG_NUM];
		GUIStaticImage* m_bagItemStackImg[MAX_ENANTMENT_BAG_NUM];
		GUIStaticText* m_lackItemText = nullptr;
		GUIStaticImage* m_consumeBg = nullptr;
		int	m_equipId = 0;
		int m_consumeId = 0;
		int m_consumeNum = 0;
		std::vector<int> m_bag;
		int m_select_effect = 0;
		int m_effect_num = 0;
		int m_need_diamond = -1;
	};
}

#endif
