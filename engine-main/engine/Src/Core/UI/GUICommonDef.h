/********************************************************************
filename: 	GUICommonDef.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef __GUI_COMMON_DEF_H__
#define __GUI_COMMON_DEF_H__

#include "Core.h"
#include "GUIString.h"
#include "GUIEventArgs.h"

namespace LORD
{
	#define MULTI_THREAD_LOAD_IMAGESET
	
	#define PixelAligned(x)	( (float)(int)(( x ) + (( x ) > 0.0f ? 0.5f : -0.5f)) )

	const int MAX_MOTION_TARGET_COUNT = 10;

	//const unsigned int ENABLED_COLOR  = 0xFFFFFFFF;
	//const unsigned int DISABLED_COLOR = 0xFF7F7F7F;

	typedef vector<GUIString>::type GUIPropertyList;

	// all kinds of call back function
	typedef void (*ACTIVE_EDITBOX_CALLBACK)(bool bActive);

	enum GUIMaterialType
	{
		GUIMaterialType_INVALID = -1,
		GUIMaterialType_CULL_NONE,
		GUIMaterialType_CULL_BACK,
		GUIMaterialType_FONT,
		GUIMaterialType_MASK,
		GUIMaterialType_MASK_CULL_NONE,

		// 上面的对应材质，都要有一个对应的disable材质
		GUIMaterialType_CULL_NONE_GRAY = 10000,
		GUIMaterialType_CULL_BACK_GRAY,
		GUIMaterialType_FONT_GRAY, // 这个材质是不用的，字体不用disable
		GUIMaterialType_MASK_GRAY,
		GUIMaterialType_MASK_CULL_NONE_GRAY,
	};

	template<typename _Derived>
	struct CustomStringLiteral
	{
		friend bool operator==(const CustomStringLiteral& lhs, const CustomStringLiteral& rhs)
		{
			return strcmp(lhs.value, rhs.value) == 0;
		}
		friend bool operator!=(const CustomStringLiteral& lhs, const CustomStringLiteral& rhs)
		{
			return !(lhs == rhs);
		}
		const char* value;
		String str() const
		{
			return value;
		}
	protected:
		constexpr CustomStringLiteral(const char* value) : value(value) {}
	};

	struct GUIWindowType : CustomStringLiteral<GUIWindowType>
	{
		// GUIWindowType只能通过string literal的形式创建
		friend constexpr GUIWindowType operator"" _gwt(const char* str, size_t size);
	private:
		constexpr GUIWindowType(const char* value) : CustomStringLiteral<GUIWindowType>(value) {}
	};

	inline constexpr GUIWindowType operator"" _gwt(const char* str, size_t size)
	{
		return str;
	}

	constexpr GUIWindowType GWT_DEFAULT_WINDOW		= "DefaultWindow"_gwt;
	constexpr GUIWindowType GWT_STATIC_IMAGE		= "StaticImage"_gwt;
	constexpr GUIWindowType GWT_STATIC_TEXT			= "StaticText"_gwt;
	constexpr GUIWindowType GWT_BUTTON				= "Button"_gwt;
	constexpr GUIWindowType GWT_EDIT				= "Edit"_gwt;
	constexpr GUIWindowType GWT_CHECK				= "CheckBox"_gwt;
	constexpr GUIWindowType GWT_RADIO				= "RadioButton"_gwt;
	constexpr GUIWindowType GWT_PROGRESS			= "ProgressBar"_gwt;
	constexpr GUIWindowType GWT_SLIDER				= "Slider"_gwt;
	constexpr GUIWindowType GWT_EFFECT				= "Effect"_gwt;
	constexpr GUIWindowType GWT_ACTOR				= "Actor"_gwt;
	constexpr GUIWindowType GWT_LIST				= "List"_gwt;
	constexpr GUIWindowType GWT_LIST_HORIZONTAL = "ListHorizontal"_gwt;
	constexpr GUIWindowType GWT_LAYOUT				= "Layout"_gwt;
	constexpr GUIWindowType GWT_MULTILINE_EDIT		= "MultilineEdit"_gwt;
	constexpr GUIWindowType GWT_BUBBLE_CONTAIN		= "BubbleContain"_gwt;
	constexpr GUIWindowType GWT_BUBBLE_CONTAINEX	= "BubbleContainEx"_gwt;
	constexpr GUIWindowType GWT_INPUT_BOX			= "InputBox"_gwt;
	constexpr GUIWindowType GWT_SCENE_MAP			= "SceneMap"_gwt;
	constexpr GUIWindowType GWT_TABLE_VIEW			= "TableView"_gwt;
	constexpr GUIWindowType GWT_GRID_VIEW			= "GridView"_gwt;
	constexpr GUIWindowType GWT_PAGE_VIEW			= "PageView"_gwt;
	constexpr GUIWindowType GWT_SCROLLED_CONTAINER	= "ScrolledContainer"_gwt;
	constexpr GUIWindowType GWT_SCROLLABLE_PANE		= "ScrollablePane"_gwt;
	constexpr GUIWindowType GWT_SCROLL_CARD			= "ScrollCard"_gwt;
	constexpr GUIWindowType GWT_ANIMATE				= "Animate"_gwt;
	constexpr GUIWindowType GWT_CELL				= "Cell"_gwt;
	constexpr GUIWindowType GWT_MUTUAL_NONOVERLAPPING_GROUP = "MutualNonoverlappingGroup"_gwt;


	struct GUIWindowProperty : CustomStringLiteral<GUIWindowProperty>
	{
		// GUIWindowProperty只能通过string literal的形式创建
		friend constexpr GUIWindowProperty operator"" _gwp(const char* str, size_t size);
		operator GUIString() const
		{
			return value;
		}
	private:
		constexpr GUIWindowProperty(const char* value) : CustomStringLiteral<GUIWindowProperty>(value) {}
	};

	inline constexpr GUIWindowProperty operator"" _gwp(const char* str, size_t size)
	{
		return str;
	}

	constexpr GUIWindowProperty GWP_WINDOW_TYPE = "WindowType"_gwp;
	constexpr GUIWindowProperty GWP_WINDOW_NAME = "WindowName"_gwp;
	constexpr GUIWindowProperty GWP_WINDOW_MATERIAL = "Material"_gwp;
	constexpr GUIWindowProperty GWP_WINDOW_MASK_TEXTURE = "MaskTexture"_gwp;
	constexpr GUIWindowProperty GWP_WINDOW_DRAW_COLOR = "DrawColor"_gwp;
	constexpr GUIWindowProperty GWP_WINDOW_ENABLED = "Enabled"_gwp;
	constexpr GUIWindowProperty GWP_WINDOW_AREA = "Area"_gwp;
	constexpr GUIWindowProperty GWP_WINDOW_VA = "VerticalAlignment"_gwp;
	constexpr GUIWindowProperty GWP_WINDOW_HA = "HorizontalAlignment"_gwp;
	constexpr GUIWindowProperty GWP_WINDOW_TEXT = "Text"_gwp;
	constexpr GUIWindowProperty GWP_WINDOW_FONT = "Font"_gwp;
	constexpr GUIWindowProperty GWP_WINDOW_TOUCHABLE = "Touchable"_gwp;
	constexpr GUIWindowProperty GWP_WINDOW_VISIBLE = "Visible"_gwp;
	constexpr GUIWindowProperty GWP_WINDOW_AUTOSIZE = "AutoSize"_gwp;
	constexpr GUIWindowProperty GWP_WINDOW_LEVEL = "Level"_gwp;
	constexpr GUIWindowProperty GWP_WINDOW_ALWAYS_ON_TOP = "AlwaysOnTop"_gwp;
	constexpr GUIWindowProperty GWP_WINDOW_BACKGROUND_COLOR = "BackgroundColor"_gwp;
	constexpr GUIWindowProperty GWP_WINDOW_ALPHA = "Alpha"_gwp;
	constexpr GUIWindowProperty GWP_WINDOW_CLIPCHILD = "ClipChild"_gwp;
	constexpr GUIWindowProperty GWP_WINDOW_ROTATE = "Rotate"_gwp;
	constexpr GUIWindowProperty GWP_WINDOW_ROTATEY = "RotateY"_gwp;
	constexpr GUIWindowProperty GWP_WINDOW_ROTATEX = "RotateX"_gwp;
	constexpr GUIWindowProperty GWP_WINDOW_MULTI_CLICK = "MultiClick"_gwp;
	constexpr GUIWindowProperty GWP_WINDOW_STRETCH_TYPE = "StretchType"_gwp;
	constexpr GUIWindowProperty GWP_WINDOW_STRETCH_OFFSET = "StretchOffset"_gwp;
	constexpr GUIWindowProperty GWP_WINDOW_OPEN_SOUND = "WndOpenSound"_gwp;
	constexpr GUIWindowProperty GWP_WINDOW_CLOSE_SOUND = "WndCloseSound"_gwp;
	constexpr GUIWindowProperty GWP_WINDOW_ENABLE_TEXT_COLOR_RECT = "EnableTextColorRect"_gwp;
	constexpr GUIWindowProperty GWP_WINDOW_TEXT_COLOR_LT = "TextColorLeftTop"_gwp;
	constexpr GUIWindowProperty GWP_WINDOW_TEXT_COLOR_RT = "TextColorRightTop"_gwp;
	constexpr GUIWindowProperty GWP_WINDOW_TEXT_COLOR_LB = "TextColorLeftBottom"_gwp;
	constexpr GUIWindowProperty GWP_WINDOW_TEXT_COLOR_RB = "TextColorRightBottom"_gwp;
	constexpr GUIWindowProperty GWP_WINDOW_EFFECT_NAME = "EffectFileName"_gwp;
	constexpr GUIWindowProperty GWP_WINDOW_EFFECT_ROTATE = "EffectRotate"_gwp;

	constexpr GUIWindowProperty GWP_WINDOW_LUA_VARIABLE = "OutputLuaVariable"_gwp;
	constexpr GUIWindowProperty GWP_WINDOW_LUA_EVENT = "OutputLuaEvent"_gwp;
	constexpr GUIWindowProperty GWP_WINDOW_MIDDLE_LAYOUT_CHILD = "MiddleLayoutChild"_gwp;
	constexpr GUIWindowProperty GWP_WINDOW_ENABLE_LONG_TOUCH = "EnableLongTouch"_gwp;
	constexpr GUIWindowProperty GWP_WINDOW_MAX_WIDTH = "MaxWidth"_gwp;
	constexpr GUIWindowProperty GWP_WINDOW_MIN_WIDTH = "MinWidth"_gwp;
	constexpr GUIWindowProperty GWP_WINDOW_MAX_HEIGHT = "MaxHeight"_gwp;
	constexpr GUIWindowProperty GWP_WINDOW_MIN_HEIGHT = "MinHeight"_gwp;
	constexpr GUIWindowProperty GWP_SI_IMAGE_NAME = "ImageName"_gwp;
	constexpr GUIWindowProperty GWP_TEXT_COLOR = "TextColor"_gwp;
	constexpr GUIWindowProperty GWP_TEXT_HORZ_ALIGN = "TextHorzAlignment"_gwp;
	constexpr GUIWindowProperty GWP_TEXT_VERT_ALIGN = "TextVertAlignment"_gwp;
	constexpr GUIWindowProperty GWP_TEXT_WORDWRAP = "TextWordWrap"_gwp;
	constexpr GUIWindowProperty GWP_TEXT_SELFADAPT_HIGH = "TextSelfAdaptHigh"_gwp;
	constexpr GUIWindowProperty GWP_TEXT_SHADOW = "TextShadow"_gwp;
	constexpr GUIWindowProperty GWP_TEXT_SHADOW_COLOR = "TextShadowColor"_gwp;
	constexpr GUIWindowProperty GWP_TEXT_BORDER = "TextBorder"_gwp;
	constexpr GUIWindowProperty GWP_TEXT_BORDER_COLOR = "TextBorderColor"_gwp;
	constexpr GUIWindowProperty GWP_TEXT_ALLSHOW_ONELINE = "AllShowOneLine"_gwp;
	constexpr GUIWindowProperty GWP_TEXT_SCALE = "TextScale"_gwp;
	constexpr GUIWindowProperty GWP_BTN_TEXT_COLOR = "BtnTextColor"_gwp;
	constexpr GUIWindowProperty GWP_BTN_NORM_IMAGE = "NormalImage"_gwp;
	constexpr GUIWindowProperty GWP_BTN_PUSH_IMAGE = "PushedImage"_gwp;
	constexpr GUIWindowProperty GWP_BTN_SOUND_NAME = "SoundName"_gwp;
	constexpr GUIWindowProperty GWP_BTN_TEXT_RENDER_OFFSET = "TextRenderOffset"_gwp;
	constexpr GUIWindowProperty GWP_BTN_HEXAGON_TOP_IMG = "HexgonTopImage"_gwp;
	constexpr GUIWindowProperty GWP_BTN_HEXAGON_TOP_COLOR = "HexgonTopColor"_gwp;
	constexpr GUIWindowProperty GWP_BTN_HEXAGON_SIDE_IMG = "HexgonSideImage"_gwp;
	constexpr GUIWindowProperty GWP_BTN_HEXAGON_SIDE_COLOR = "HexgonSideColor"_gwp;
	constexpr GUIWindowProperty GWP_BTN_HEXAGON_OVER_IMG = "HexgonOverlayerImage"_gwp;
	constexpr GUIWindowProperty GWP_BTN_HEXAGON_OVER_COLOR = "HexgonOverlayerColor"_gwp;
	constexpr GUIWindowProperty GWP_EDIT_BACK_IMAGE = "EditBackImage"_gwp;
	constexpr GUIWindowProperty GWP_EDIT_PASSWORD = "Password"_gwp;
	constexpr GUIWindowProperty GWP_EDIT_TEXT_OFFSET = "TextOffset"_gwp;
	constexpr GUIWindowProperty GWP_EDIT_MAX_TEXT_LENGTH = "MaxTextLength"_gwp;
	constexpr GUIWindowProperty GWP_RADIO_GROUP = "RadioGroup"_gwp;
	constexpr GUIWindowProperty GWP_PRO_PROGRESS = "Progress"_gwp;
	constexpr GUIWindowProperty GWP_PRO_BACK_IMAGE = "ProgressBackImage"_gwp;
	constexpr GUIWindowProperty GWP_PRO_IMAGE = "ProgressImage"_gwp;
	constexpr GUIWindowProperty GWP_PRO_HEADER_IMAGE = "ProgressHeaderImage"_gwp;
	constexpr GUIWindowProperty GWP_PRO_IS_VERTICAL = "ProgressIsVertical"_gwp;
	constexpr GUIWindowProperty GWP_QUICK_SLIDE = "QuickSlide"_gwp;
	constexpr GUIWindowProperty GWP_LIST_NORMAL_IMAGE_BACK = "ListItemNormalImage"_gwp;
	constexpr GUIWindowProperty GWP_LIST_SELECT_IMAGE_BACK = "ListItemSelectImage"_gwp;
	constexpr GUIWindowProperty GWP_LIST_ITEM_HEIGHT = "ListItemHeight"_gwp;
	constexpr GUIWindowProperty GWP_LIST_BLINK_SELECT = "ListBlinkSelect"_gwp;
	constexpr GUIWindowProperty GWP_LIST_ALLOW_SCROLL = "AllowScroll"_gwp;
	constexpr GUIWindowProperty GWP_LIST_BETWEEN_DISTANCE = "BetweenDistance"_gwp;
	constexpr GUIWindowProperty GWP_LAYOUT_IMAGE_BACK = "LayoutBackImage"_gwp;
	constexpr GUIWindowProperty GWP_BUBBLE_ITEM_BACK_IMAGE = "BubbleItemBackImage"_gwp;
	constexpr GUIWindowProperty GWP_INPUTBOX_BACK_IMAGE = "InputBoxBackImage"_gwp;
	constexpr GUIWindowProperty GWP_PAGE_TURN = "PageTurning"_gwp;
	constexpr GUIWindowProperty GWP_PAGE_INNER_SLIDE = "PageInnerSlid"_gwp;
	constexpr GUIWindowProperty GWP_PAGE_LOOP_SWITCH = "LoopTurning"_gwp;
	constexpr GUIWindowProperty GWP_SCROLL_PANE_HORI_SCROLL = "HoriScrollEnable"_gwp;
	constexpr GUIWindowProperty GWP_SCROLL_PANE_VERT_SCROLL = "VertScrollEnable"_gwp;
	constexpr GUIWindowProperty GWP_ANIMATE_ID = "AnimateID"_gwp;
	constexpr GUIWindowProperty GWP_ANIMATE_PLAY = "Play"_gwp;
	constexpr GUIWindowProperty GWP_ACTOR_ACTOR_NAME = "ActorName"_gwp;
	constexpr GUIWindowProperty GWP_ACTOR_SKILL_NAME = "SkillName"_gwp;
	constexpr GUIWindowProperty GWP_ACTOR_ROTATIONX = "ActorRotationX"_gwp;
	constexpr GUIWindowProperty GWP_ACTOR_ROTATIONY = "ActorRotationY"_gwp;
	constexpr GUIWindowProperty GWP_ACTOR_ROTATIONZ = "ActorRotationZ"_gwp;
	constexpr GUIWindowProperty GWP_ACTOR_SCALE = "ActorScale"_gwp;
	constexpr GUIWindowProperty GWP_GRID_VIEW_ITEM_ALIGNMENT = "ItemAlignment"_gwp;
	constexpr GUIWindowProperty GWP_TEXT_LINE_EXTRA_SPACE = "TextLineExtraSpace"_gwp;

	enum GUIPropertyType
	{
		GPT_BOOL,
		GPT_FLOAT,
		GPT_INT,
		GPT_STRING,
		GPT_COLOR,
		GPT_AREA,
		GPT_VA,
		GPT_HA,
		GPT_IMAGE,
		GPT_FONT,
		GPT_VECTOR2,
//		GPT_ANIMATE_TYPE,
		GPT_STRETCH_TYPE,
		GPT_STRETCH_OFFSET,
		GPT_MATERIAL,
// 		GPT_FLY_ANIMATE_TPYE,
// 		GPT_FLY_ACC_DIR,
// 		GPT_SELECT_FLY_ANIME,

		GPT_LUA_EVENT,
	};

	enum VerticalAlignment
	{
		VA_TOP,
		VA_CENTRE,
		VA_BOTTOM
	};

	enum HorizontalAlignment
	{
		HA_LEFT,
		HA_CENTRE,
		HA_RIGHT
	};

	enum GUIEventType
	{
		GET_WINDOW_TOUCH_DOWN,
		GET_WINDOW_TOUCH_UP,
		GET_WINDOW_MOTION_RELEASE,
		GET_WINDOW_DOUBLE_CLICK,
		GET_WINDOW_TOUCH_SLIDE,
		// button
		GET_BUTTON_DOWN,
		GET_BUTTON_UP,
		GET_BUTTON_CLICK,
		GET_CHECK_STATE_CHANGED,
		GET_RADIO_STATE_CHANGED,
		GET_LIST_SELECT_CHANGED,
		GET_SCROLLCARD_CLICK,
		//slider
		GET_SLIDER_MOVE,
		//scene map click
		GET_SCENE_MAP_CLICK,
		//page turning
		GET_PAGE_CHANGED,
		GET_PAGE_START_MOVE,
		GET_PAGE_MOVE_FINISHED,
		//static text
		GET_STATICTEXT_TEXTCHANGED,
	};

    enum KeyboardReturnType
    {
        KRT_DEFAULT,
        KRT_DONE,
        KRT_SEND,
        KRT_SEARCH,
        KRT_GO
    };

    /**
     * \brief The EditBox::InputMode defines the type of text that the user is allowed
     * to enter.
     */
    enum InputMode
    {
        /**
         * The user is allowed to enter any text, including line breaks.
         */
        IM_ANY,

        /**
         * The user is allowed to enter an e-mail address.
         */
        IM_EMAIL_ADDRESS,

        /**
         * The user is allowed to enter an integer value.
         */
        IM_NUMERIC,

        /**
         * The user is allowed to enter a phone number.
         */
        IM_PHONE_NUMBER,

        /**
         * The user is allowed to enter a URL.
         */
        IM_URL,

        /**
         * The user is allowed to enter a real number value.
         * This extends kEditBoxInputModeNumeric by allowing a decimal point.
         */
        IM_DECIMAL,

        /**
         * The user is allowed to enter any text, except for line breaks.
         */
        IM_SINGLE_LINE,
    };

    /**
     * \brief The EditBox::InputFlag defines how the input text is displayed/formatted.
     */
    enum InputFlag
    {
        /**
         * Indicates that the text entered is confidential data that should be
         * obscured whenever possible. This implies EDIT_BOX_INPUT_FLAG_SENSITIVE.
         */
        IF_PASSWORD,

        /**
         * Indicates that the text entered is sensitive data that the
         * implementation must never store into a dictionary or table for use
         * in predictive, auto-completing, or other accelerated input schemes.
         * A credit card number is an example of sensitive data.
         */
        IF_SENSITIVE,

        /**
         * This flag is a hint to the implementation that during text editing,
         * the initial letter of each word should be capitalized.
         */
        IF_INITIAL_CAPS_WORD,

        /**
         * This flag is a hint to the implementation that during text editing,
         * the initial letter of each sentence should be capitalized.
         */
        IF_INITIAL_CAPS_SENTENCE,

        /**
         * Capitalize all characters automatically.
         */
        IF_INTIAL_CAPS_ALL_CHARACTERS,
    };

	struct GUIVertexDeclaration
	{
		Vector3			position;
		unsigned int	color;
		Vector2			texCoord;
	};

	class GUIWindow;
	struct GUIHyperlinkInfo
	{
		ERect			rtHyperlinkArea;
		GUIString		strHyperlinkInfo;
		GUIWindow*		pWindow;
	};

	enum StretchType
	{
		ST_NONE,			
		ST_TOP_BOTTOM,
		ST_LEFT_RIGHT,
		ST_NINE,
	};
	
	struct StretchOffset
	{
		StretchOffset()
			: fLeft(0.0f)
			, fRight(0.0f)
			, fTop(0.0f)
			, fBottom(0.0f)
		{
		}

		bool operator !=(const StretchOffset& other)
		{
			return (fLeft != other.fLeft || fRight != other.fRight || fTop != other.fTop || fBottom != other.fBottom);
		}
		float fLeft;
		float fRight;
		float fTop;
		float fBottom;
	};

	struct ColorRect
	{
		Color	cLeftTop;
		Color	cRightTop;
		Color	cLeftBottom;
		Color	cRightBottom;

		inline ColorRect()
			: cLeftTop(Color::WHITE)
			, cRightTop(Color::WHITE)
			, cLeftBottom(Color::WHITE)
			, cRightBottom(Color::WHITE)
		{}

		inline ColorRect(const Color& c)
			: cLeftTop(c)
			, cRightTop(c)
			, cLeftBottom(c)
			, cRightBottom(c)
		{}

		inline ColorRect(const Color& cLT, const Color& cRT, const Color& cLB, const Color& cRB)
			: cLeftTop(cLT)
			, cRightTop(cRT)
			, cLeftBottom(cLB)
			, cRightBottom(cRB)
		{}

		inline ColorRect(const ColorRect& colorRect)
			: cLeftTop(colorRect.cLeftTop)
			, cRightTop(colorRect.cRightTop)
			, cLeftBottom(colorRect.cLeftBottom)
			, cRightBottom(colorRect.cRightBottom)
		{
		}

		inline bool operator == (const ColorRect& colorRect) const
		{
			return cLeftTop == colorRect.cLeftTop && cRightTop == colorRect.cRightTop && cLeftBottom == colorRect.cLeftBottom && cRightBottom == colorRect.cRightBottom;
		}

		inline bool operator != (const ColorRect& colorRect) const
		{
			return cLeftTop != colorRect.cLeftTop || cRightTop != colorRect.cRightTop || cLeftBottom != colorRect.cLeftBottom || cRightBottom != colorRect.cRightBottom;
		}
	};

	typedef vector<GUIString>::type	TextStringList;
	typedef vector<GUIHyperlinkInfo>::type HyperlinkInfoList;

	//typedef void (*GUI_EVENT_CALLBACK)(GUIEventType eEventType, const GUIString& strDialogName, const GUIString& strWindowName, int nParam1, int nParam2);

	typedef void (*GUI_HYPERLINK_CALLBACK)(const GUIString& strHyperlinkInfo);

	typedef void (*GUI_RENDER_HITTEXT_CALLBACK)(void);
	//typedef bool (*GUI_FIRE_EVENT)(const EventArgs &);

	// gui event define
	// 另一种事件触发的方式，通过绑定事件
	// window
	static const GUIString EventWindowTouchDown("WindowTouchDown");
	static const GUIString EventWindowTouchMove("WindowTouchMove");
	static const GUIString EventMotionRelease("MotionRelease");
	static const GUIString EventWindowTouchUp("WindowTouchUp");
	static const GUIString EventWindowLongTouchStart("WindowLongTouchStart");
	static const GUIString EventWindowLongTouchEnd("WindowLongTouchEnd");
	static const GUIString EventWindowTextChanged("WindowTextChanged");
	static const GUIString EventWindowClick("WindowClick");
	static const GUIString EventWindowDoubleClick("WindowDoubleClick");

	// drag
	static const GUIString EventWindowDragStart("WindowDragStart");
	static const GUIString EventWindowDragging("WindowDragging");
	static const GUIString EventWindowDragEnd("WindowDragEnd");

	static const GUIString EventWindowActionEnd("UIActionEnd");

	// button
	static const GUIString EventButtonClick("ButtonClick");
	// check
	static const GUIString EventCheckStateChanged("CheckStateChanged");
	//radio change
	static const GUIString EventRadioStateChanged("RadioStateChanged");

	// GUIScrollCard
	static const GUIString EventScrollCardClick("ScrollCardClick");

	//GUIScrollLabelPane
	static const GUIString EventScrollLabelPaneContainerMove("ScrollLabelPaneContainerMove");

	//edit 接受输入
	static const GUIString EventEditTextInput("EditTextInput");

	//type def
	typedef vector<Vector3>::type VertexListType;
	
}

#endif
