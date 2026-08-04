#ifndef __RENDER_ITEMS_HEADER__
#define __RENDER_ITEMS_HEADER__

#include "Core.h"
#include "UI/GUIImage.h"
#include "UI/GUIImageset.h"


using namespace LORD;

namespace BLOCKMAN
{
	
	class gui_renderItems : public ObjectAlloc
	{
	public:
		gui_renderItems();
		~gui_renderItems();

		void renderItemInWindow(GUIWindow *pWindow, int itemID, int damage);
		void renderItem(GUIWindow *pWindow, int itemID, int damage);
		void renderBlockAsItem(GUIWindow *pWindow, int blockID, int metadata);

		
	protected:

		void renderStandardBlock(GUIWindow *pWindow, int blockID, int metadata);
		void renderBlockStairs(GUIWindow *pWindow, int blockID, int metadata);
		void renderBlockChest(GUIWindow* pwindow, int blockID, int metadata);

	private:
		ImageBufferListType m_actionVertex;
		GUIImageset *m_pImagesetBlock;
		GUIImageset *m_pImagesetItem;
		GUIImageset* m_pImagesetChest_normal;
		GUIImageset* m_pImagesetChest_christmas;
		GUIImageset* m_pImagesetChest_trapped;
		GUIImageset* m_pImagesetChest_ender;
	};

}



#endif
