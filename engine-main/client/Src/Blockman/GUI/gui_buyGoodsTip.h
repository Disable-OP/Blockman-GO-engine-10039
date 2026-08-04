#pragma once
#include "gui_layout.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "GUI/RootGuiLayout.h"
#include "Util/SubscriptionGuard.h"
#include "GUI/gui_renderItems.h"

using namespace LORD;
using namespace GameClient;

namespace BLOCKMAN
{
	class gui_buyGoodsTip : public gui_layout
	{
	public:
		gui_buyGoodsTip();
		virtual ~gui_buyGoodsTip();
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		void onShow();
		bool showContent(int count);
		bool showItemContent(int itemId, int meta, int count);
		bool showGoodsContent(const String & icon, int count);

	private:
		vector<i32>::type m_moneyTips;
		LORD::UVector2		m_originPos;
		const int			m_animateTime = 500;
		const int			m_stayTime = 500;
		GUIStaticImage*		m_tipBg = nullptr;
		GUIStaticText*		m_countText = nullptr;
		GUIStaticImage*		m_iconImage = nullptr;
		int					m_totalTime = 0;
		bool				m_isPlaying = false;
		float				m_moveDistance = 0.0f;
		SubscriptionGuard	m_subscriptionGuard;

		void refreshOriginState(int count);
		void updateIcon(int itemId, int meta);

		RootGuiLayout * getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
	};
}
