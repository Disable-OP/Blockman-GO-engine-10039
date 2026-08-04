  #ifndef __GUI_ITEM_SLOT_HEADER__
#define __GUI_ITEM_SLOT_HEADER__

#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIProgressBar.h"
#include "GUI/CustomGuiWindow.h"
#include "GUI/GuiItemStack.h"

using namespace LORD;

namespace BLOCKMAN
{
	class Ingredient;

	enum ArmorType
	{
		NONE = -1,
		HELMET,
		CHESTPLATE,
		LEGGINGS,
		BOOTS
	};

	class GuiItemSlot : public CustomGuiWindow
	{
	private:
		using IngredientPtr = std::shared_ptr<Ingredient>;

	public:
		enum class Style : signed char
		{
			INVALID = -1,
			ROCK_TRANS,
			ROCK_SOLID,
			METAL
		};

	private:
		using BaseClass = CustomGuiWindow;
		GUIStaticImage* m_window = nullptr;
		GUIStaticImage* m_armorTypeImage = nullptr;
		GUIWindow* m_enchantmentIndicator = nullptr;
		GuiItemStack* m_guiItemStack = nullptr;
		GUIProgressBar* m_dropItemProgress = nullptr;
		GUIProgressBar* m_skillCdProgress = nullptr;
		ArmorType m_armor_type = NONE;
		bool m_selected = false;
		Style m_style = Style::ROCK_TRANS;
		Style m_oldStyle = Style::INVALID;
		bool is_modified_style = false;

		bool onItemStackTouchDown(const EventArgs & events)
		{
			WindowEventArgs new_event(this);
			fireEvent("ItemStackTouchDown", new_event);
			return true;
		}

		bool onItemStackTouchUp(const EventArgs & events)
		{
			WindowEventArgs new_event(this);
			fireEvent("ItemStackTouchUp", new_event);
			return true;
		}

		bool onItemStackLongTouchStart(const EventArgs & events)
		{
			WindowEventArgs new_event(this);
			fireEvent("ItemStackLongTouchStart", new_event);
			return true;
		}

		bool onItemStackLongTouchEnd(const EventArgs & events) {
			WindowEventArgs new_event(this);
			fireEvent("ItemStackLongTouchEnd", new_event);
			return true;
		}

		void removeComponents();
		void updateStyle();

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;

	public:
		GuiItemSlot(const GUIWindowType& nType, const GUIString& strName);
		~GuiItemSlot() = default;

		bool isSelected()
		{
			return m_selected;
		}
		void setSelected(bool selected)
		{
			m_selected = selected;
			is_modified_style = true;
		}
		void setItemStack(ItemStackPtr pItemStack)
		{
			m_guiItemStack->setItemStack(pItemStack);
		}
		void setIngredient(IngredientPtr pIngredient)
		{
			m_guiItemStack->setIngredient(pIngredient);
		}
		void setStyle(Style style)
		{
			m_style = style;
			is_modified_style = true;
		}
		void setItemStackBackgroundColor(const Color& color)
		{
			m_guiItemStack->SetBackgroundColor(color);
		}
		void setItemStackNumberVisibility(GuiItemStack::NumberVisibility visibility)
		{
			m_guiItemStack->setNumberVisibility(visibility);
		}
		void setItemStackGrayedOut(bool grayedOut)
		{
			m_guiItemStack->setGrayedOut(grayedOut);
		}

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;

		UVector2 getGuiItemStackPosition()
		{
			return m_guiItemStack->getAbsolutePositionFromRoot();
		}

		GuiItemStack* cloneGuiItemStack(const GUIString& newName);
		void setArmorType(ArmorType type);
		void setDropItemProgress(float progress);
		void setItemSkillCdProgress(int curValue, int maxValue);
	};
}

#endif
