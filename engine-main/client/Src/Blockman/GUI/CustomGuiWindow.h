#ifndef __CUSTOM_GUI_WINDOW_HEADER__
#define __CUSTOM_GUI_WINDOW_HEADER__

#include <functional>
#include "UI/GUIWindow.h"

using namespace LORD;

namespace BLOCKMAN
{
	class CustomGuiWindow : public GUIWindow
	{
		using GUIWindow::GUIWindow;
	private:
		bool onChildTouchDown(const EventArgs & events);
		bool onChildTouchUp(const EventArgs & events);
		bool onChildTouchMove(const EventArgs & events);
		bool onChildMotionRelease(const EventArgs & events);
		bool onChildDragging(const EventArgs & events);
		bool onChildDragEnd(const EventArgs & events);
		bool onChildLongTouchStart(const EventArgs & events);
		bool onChildLongTouchEnd(const EventArgs & events);

	protected:
		static void addPrefixToAllWindowNames(const GUIString& prefix, GUIWindow * rootWindow);
		void renameComponents(GUIWindow* pWindow)
		{
			addPrefixToAllWindowNames(m_strName + '-', pWindow);
		}
		virtual vector<GUIWindow*>::type getPrivateChildren()
		{
			return {};
		}
		void subscribeToBubbleUpMouseEvents();

		GUIString getString(String key);

	public:
		void Update(ui32 nTimeElapse) override;
		UVector2 getAbsolutePositionFromRoot();

	};
}

#endif // !__CUSTOM_GUI_WINDOW_HEADER__
