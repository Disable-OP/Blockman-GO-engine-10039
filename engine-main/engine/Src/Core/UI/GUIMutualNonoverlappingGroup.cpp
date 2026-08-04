#include "GUIMutualNonoverlappingGroup.h"

namespace LORD
{
	void GUIMutualNonoverlappingGroup::parallelDepthFirstDraw(const WindowVector & renderList)
	{
		for (auto window : renderList)
		{
			if (!window->IsVisible())
			{
				continue;
			}
			window->OnBeforeDraw();
			window->DrawBackground();
			window->DrawSelf();
		}
		WindowVector childWindows;
		size_t index = 0;
		while (true)
		{
			for (auto window : renderList)
			{
				if (!window->IsVisible())
				{
					continue;
				}
				if (window->GetDrawlistCount() > index)
				{
					childWindows.push_back(window->GetChildByIndexInDrawlist(index));
				}
			}
			if (childWindows.size() == 0)
			{
				break;
			}
			parallelDepthFirstDraw(childWindows);
			childWindows.clear();
			++index;
		}
		for (auto window : renderList)
		{
			if (!window->IsVisible())
			{
				continue;
			}
			window->OnEndDraw();
		}
	}
}
