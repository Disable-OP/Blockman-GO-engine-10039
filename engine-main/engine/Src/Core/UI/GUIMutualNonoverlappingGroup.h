#ifndef __MUTUAL_NONOVERLAPPING_GROUP_HEADER__
#define __MUTUAL_NONOVERLAPPING_GROUP_HEADER__

#include "GUIWindow.h"

namespace LORD
{
	class GUIMutualNonoverlappingGroup : public GUIWindow
	{
		using GUIWindow::GUIWindow;
	private:
		using WindowVector = vector<GUIWindow*>::type;

		void parallelDepthFirstDraw(const WindowVector& renderList);

	public:
		void Render() override
		{
			parallelDepthFirstDraw(m_DrawList);
		}
	};
}

#endif