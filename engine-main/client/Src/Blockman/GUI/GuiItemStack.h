#ifndef __GUI_ITEM_STACK_HEADER__
#define __GUI_ITEM_STACK_HEADER__

#include <memory>
#include "UI/GUIWindow.h"
#include "GUI/CustomGuiWindow.h"
#include "GUI/gui_renderItems.h"
#include "Item/ItemStack.h"
#include "UI/GUIStaticText.h"

using namespace LORD;

namespace LORD
{
	class GUIStaticImage;
	class GUIStaticText;
}

namespace BLOCKMAN
{
	class Ingredient;

	class GuiItemStack : public CustomGuiWindow
	{
	private:
		using IngredientPtr = std::shared_ptr<Ingredient>;

	public:
		enum class NumberVisibility
		{
			AUTO,
			ALWAYS_VISIBLE,
			NEVER_VISIBLE
		};

	private:
		using BaseClass = CustomGuiWindow;
		constexpr static char* JSON_NAME = "ItemStack.json";
		GUIWindow* m_window = nullptr;
		GUIStaticImage* m_iconImage = nullptr;
		GUIStaticText* m_countText = nullptr;
		ItemStackPtr m_itemStack = nullptr;
		IngredientPtr m_ingredient = nullptr;
		GUIStaticText* m_gunNum = nullptr;
		int m_oldItemId = -2, m_oldItemDamage = -2;
		NumberVisibility m_numberVisibility = NumberVisibility::AUTO;
		bool m_isGrayedOut = false;

        static gui_renderItems* pRenderItems;
		void updateCount(int num);
		void updateIcon(int itemId, int damage = 0);
		void updateGunNum();

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;

	public:
		GuiItemStack(const GUIWindowType& nType, const GUIString& strName, ItemStackPtr pItemStack = nullptr);
		~GuiItemStack() = default;

        static void initialize();
        static void unInitialize();
        
		bool SetProperty(const GUIString& strName, const GUIString& strValue) override;
		bool GetProperty(const GUIString& strName, GUIString& strValue) override;
		void GetPropertyList(GUIPropertyList& properlist) override;
		bool GetPropertyType(const GUIString& strName, GUIPropertyType& type) override;
		static String getItemSpriteName(int itemId, int damage, int pass);
		static void showItemIcon(GUIStaticImage* pItemIcon, ItemStackPtr pStack, int itemId, int damage = 0, String image = "");
		static gui_renderItems* getRenderItems();
		static String SwitchRenderItemsJson(int itemId);

		ItemStackPtr getItemStack()
		{
			return m_itemStack;
		}
		void setItemStack(ItemStackPtr itemStack)
		{
			m_ingredient = nullptr;
			m_itemStack = itemStack;
		}
		void setIngredient(IngredientPtr ingredient)
		{
			m_itemStack = nullptr;
			m_ingredient = ingredient;
		}
		void setNumberVisibility(NumberVisibility numberVisibility)
		{
			m_numberVisibility = numberVisibility;
		}
		void setGrayedOut(bool grayedOut)
		{
			m_isGrayedOut = grayedOut;
		}

		void setGunNum(GUIStaticText* guiText)
		{
			this->m_gunNum = guiText;
		}

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;
		void Clone(const GUIString& strNamePreffix, GUIWindow* pWindow) override;
	};
}

#endif // !__GUI_ITEM_STACK_HEADER__

