#include "GUIWindowFactory.h"
#include "Functional/LordNewOp.h"
#include "GUIStaticImage.h"
#include "GUIStaticText.h"
#include "GUIButton.h"
#include "GUIEditBox.h"
#include "GUICheckBox.h"
#include "GUIRadioButton.h"
#include "GUIProgressBar.h"
#include "GUISlider.h"
#include "GUIListBox.h"
#include "GUILayout.h"
#include "GUIMultiLineEdit.h"
#include "GUIBubbleContain.h"
#include "GUIInputBox.h"
#include "GUISceneMap.h"
#include "GUITableView.h"
#include "GUIGridView.h"
#include "GUIPageView.h"
#include "GUIScrolledContainer.h"
#include "GUIScrollablePane.h"
#include "GUIScrollCard.h"
#include "GUIAnimateWindow.h"
#include "GUICell.h"
#include "GUIMutualNonoverlappingGroup.h"
#include "GUIListHorizontalBox.h"

namespace LORD
{
	bool defaultInitialize(GUIWindowFactory* factory)
	{
		factory->registerWindowType(GWT_DEFAULT_WINDOW, lord_new_op<GUIWindow>());
		factory->registerWindowType(GWT_STATIC_IMAGE, lord_new_op<GUIStaticImage>());
		factory->registerWindowType(GWT_STATIC_TEXT, lord_new_op<GUIStaticText>());
		factory->registerWindowType(GWT_BUTTON, lord_new_op<GUIButton>());
		factory->registerWindowType(GWT_EDIT, lord_new_op<GUIEditBox>());
		factory->registerWindowType(GWT_CHECK, lord_new_op<GUICheckBox>());
		factory->registerWindowType(GWT_RADIO, lord_new_op<GUIRadioButton>());
		factory->registerWindowType(GWT_PROGRESS, lord_new_op<GUIProgressBar>());
		factory->registerWindowType(GWT_SLIDER, lord_new_op<GUISlider>());
		//factory->registerWindowType(GWT_EFFECT, lord_new_op<GUIEffectWindow>::value);
		factory->registerWindowType(GWT_LIST, lord_new_op<GUIListBox>());
		factory->registerWindowType(GWT_LIST_HORIZONTAL, lord_new_op<GUIListHorizontalBox>());
		factory->registerWindowType(GWT_LAYOUT, lord_new_op<GUILayout>());
		factory->registerWindowType(GWT_MULTILINE_EDIT, lord_new_op<GUIMultiLineEdit>());
		factory->registerWindowType(GWT_BUBBLE_CONTAIN, lord_new_op<GUIBubbleContain>());
		factory->registerWindowType(GWT_BUBBLE_CONTAINEX, lord_new_op<GUIBubbleContainEx>());
		factory->registerWindowType(GWT_INPUT_BOX, lord_new_op<GUIInputBox>());
		factory->registerWindowType(GWT_SCENE_MAP, lord_new_op<GUISceneMap>());
		factory->registerWindowType(GWT_TABLE_VIEW, lord_new_op<GUITableView>());
		factory->registerWindowType(GWT_GRID_VIEW, lord_new_op<GUIGridView>());
		factory->registerWindowType(GWT_PAGE_VIEW, lord_new_op<GUIPageView>());
		factory->registerWindowType(GWT_SCROLLED_CONTAINER, lord_new_op<GUIScrolledContainer>());
		factory->registerWindowType(GWT_SCROLLABLE_PANE, lord_new_op<GUIScrollablePane>());
		factory->registerWindowType(GWT_SCROLL_CARD, lord_new_op<GUIScrollCard>());
		factory->registerWindowType(GWT_ANIMATE, lord_new_op<GUIAnimateWindow>());
		factory->registerWindowType(GWT_CELL, lord_new_op<GUICell>());
		factory->registerWindowType(GWT_MUTUAL_NONOVERLAPPING_GROUP, lord_new_op<GUIMutualNonoverlappingGroup>());
		return true;
	}

	GUIWindowFactory* GUIWindowFactory::Instance()
	{
		static GUIWindowFactory* instance = LordNew GUIWindowFactory;
		static bool defaultInitialized = defaultInitialize(instance);
		return instance;
	}
}