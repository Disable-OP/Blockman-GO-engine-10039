#pragma once
#include "Core.h"
#include "gui_layout.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIProgressBar.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIButton.h"
#include "Util/ClientEvents.h"
#include "GUI/RootGuiLayout.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_blockLoading : public gui_layout
	{
	private:
		RootGuiLayout * getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
	public:
		gui_blockLoading();
		virtual ~gui_blockLoading();
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool RefreshProgress(float fProgress, bool bFirstUpdate);

	private:
		void setRunImage();

		GUIStaticText * m_loadingText = nullptr;
		GUIProgressBar* m_loadingProgressBar = NULL;
		GUIWindow*		m_movingWindow = nullptr;
		GUIStaticImage* m_runImage = nullptr;

		SubscriptionGuard m_subscriptionGuard;
		
		int m_curTick = 0;
		UVector2    m_originPos;
		float       m_distance = 0.0f;
	};

}
