/********************************************************************
filename: 	GameSettings.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-12-12
*********************************************************************/
#ifndef __GAME_SETTINGS_HEADER__
#define __GAME_SETTINGS_HEADER__

#include "Core.h"
using namespace LORD;

namespace BLOCKMAN
{

class Blockman;
class MovementInput;

enum class MoveState
{
	RunState = 0,
	SneakState,
	SprintState,
	TotalState
};

class KeyBinding : public ObjectAlloc
{
public:
	static const String KEY_FORWORD;
	static const String KEY_LEFT;
	static const String KEY_RIGHT;
	static const String KEY_BACK;
	static const String KEY_JUMP;
	static const String KEY_INVENTORY;
	static const String KEY_DROP;
	static const String KEY_CHAT;
	static const String KEY_SNEAK;
	static const String KEY_ATTACK;
	static const String KEY_USE;
	static const String KEY_PLAYERLIST;
	static const String KEY_PICKITEM;
	static const String KEY_COMMAND;
	static const String KEY_CHANGE;
	static const String KEY_EXIT;
	static const String KEY_ANDROID_BACK;
	static const String KEY_TOP_LEFT;
	static const String KEY_TOP_RIGHT;

public:
	typedef vector<KeyBinding*>::type KeyBindingArr;
	typedef map<int, KeyBinding*>::type KeyBindingMap;
	typedef map<String, KeyBinding*>::type NameKeyBindingMap;

public:
	static KeyBindingArr keybindArray;
	static KeyBindingMap keybindMap;
	static NameKeyBindingMap nameKeyBindMap;
	
	String keyDescription;
	int keyCode = 0;

	/** because _303 wanted me to call it that(Caironater) */
	bool pressed = false;
	int pressTime = 0;

public:
	static void onTick(int k);
	static void setKeyBindState(int k, bool pressed);
	static void setKeyBindState(const String& name, bool pressed);
	static void unPressAllKeys();
	static void resetKeyBindingArrayAndHash();
    static void clearKeyBingdingArrayAndHash();
	static KeyBinding* getKeyBinding(int k);
	
	KeyBinding(const String& des, int k);
    ~KeyBinding();
	void setKeyState(bool pressed);
	bool isPressed();
	void unpressKey();
};

class GameSettings : public ObjectAlloc
{
private:
	struct MouseInfo
	{
		LORD::Vector2 point;
		ui32 time;
	};

	static String RENDER_DISTANCES[4];
	static String DIFFICULTIES[4]; 

	/** GUI scale values */
	static String GUISCALES[4]; 
	static String CHAT_VISIBILITIES[3]; 
	static String PARTICLES[3]; 

	/** Limit framerate labels */
	static String LIMIT_FRAMERATES[3];
	static String AMBIENT_OCCLUSIONS[3];

	static ui32 mouseHistoryDuration;
	deque<MouseInfo>::type m_mouseHistory;
	bool m_mouseMoving = false;

public:
	float musicVolume = 0.f;
	float soundVolume = 0.f;
	bool invertMouse = false;
	int renderDistance = 0;
	bool viewBobbing = false;
	bool anaglyph = false;

	/** Advanced OpenGL */
	bool advancedOpengl = false;
	int limitFramerate = 0;
	bool fancyGraphics = false;

	int getRenderRange();		 // view 
	int getRenderChunksRange();  // render.
	int getDetectChunksRange();  // prepare.
	int getLoadChunksRange();    // load.

	/** Smooth Lighting */
	int ambientOcclusion = 0;

	/** Clouds flag */
	bool clouds = false;

	/** The name of the selected texture pack. */
	String skin;
	int chatVisibility = 0;
	bool chatColours = false;
	bool chatLinks = false;
	bool chatLinksPrompt = false;
	float chatOpacity = 0.f;
	bool serverTextures = false;
	bool snooperEnabled = false;
	bool fullScreen = false;
	bool enableVsync = false;
	bool hideServerAddress = false;

	/** Whether to show advanced information on item tooltips, toggled by F3+H */
	bool advancedItemTooltips = false;

	/** Whether to pause when the game loses focus, toggled by F3+P */
	bool pauseOnLostFocus = false;

	/** Whether to show your cape */
	bool showCape = false;
	bool touchscreen = false;
	int overrideWidth = 0;
	int overrideHeight = 0;
	bool heldItemTooltips = false;
	float chatScale = 0.f;
	float chatWidth = 0.f;
	float chatHeightUnfocused = 0.f;
	float chatHeightFocused = 0.f;

	KeyBinding* keyBindForward = nullptr;
	KeyBinding* keyBindLeft = nullptr;
	KeyBinding* keyBindBack = nullptr;
	KeyBinding* keyBindRight = nullptr;
	KeyBinding* keyBindJump = nullptr;
	KeyBinding* keyBindInventory = nullptr;
	KeyBinding* keyBindDrop = nullptr;
	KeyBinding* keyBindChat = nullptr;
	KeyBinding* keyBindSneak = nullptr;
	KeyBinding* keyBindAttack = nullptr;
	KeyBinding* keyBindUseItem = nullptr;
	KeyBinding* keyBindPlayerList = nullptr;
	KeyBinding* keyBindPickBlock = nullptr;
	KeyBinding* keyBindCommand = nullptr;
	KeyBinding* keyBindChange = nullptr;
	KeyBinding* keyBindExit = nullptr;
	KeyBinding* keyBindAndroindBack = nullptr;
	KeyBinding* keyBindTopLeft = nullptr;
	KeyBinding* keyBindTopRight = nullptr;

	// private File optionsFile;
	int difficulty = 0;
	bool hideGUI = false;

	/** true if debug info should be displayed instead of version */
	bool showDebugInfo = false;
	bool showDebugProfilerChart = false;

	/** The lastServer string. */
	String lastServer;

	/** No clipping for singleplayer */
	bool noclip = false;

	/** Smooth Camera Toggle */
	bool smoothCamera = false;
	bool debugCamEnable = false;

	/** No clipping movement rate */
	float noclipRate = 0.f;

	/** Change rate for debug camera */
	float debugCamRate = 0.f;
	float gammaSetting = 0.f;

	/** GUI scale */
	int guiScale = 0;

	/** Determines amount of particles. 0 = All, 1 = Decreased, 2 = Minimal */
	int particleSetting = 0;

	/** Game settings language */
	String language;

	/*scale from 0.5f ~ 1.5f*/
	float playerActivityGuiSize = 0;
	const float GUI_HEIGHT = 300.0f / 720.0f;
	const float GUI_WIDTH = 300.0f / 1280.0f;

	//pole varible
	int usePole = 0;
	float m_poleForward = 0.0f;
	float m_poleStrafe = 0.0f;
	int isJumpSneakDefault = 1;
	bool m_isEditFurniture = false;

	float m_fpsSetting;

	MoveState m_curMoveState = MoveState::RunState;

	enum SettingPersonViewType
	{
		SPVT_FIRST = 0, // first person view
		SPVT_THIRD = 1, // third person view
		SPVT_SECOND = 2, // second person view
	};

public:
	/* store the movement input of the this client */
	MovementInput * m_clientMovementInput = nullptr;

private:
	int personView = SPVT_FIRST;
	float cameraSensitive = 0.f;
	float fovSetting = 0.f;
	float mouseSensitivity = 0.f;

	bool m_isAutoRunStatus = false;

public:
	GameSettings();

	~GameSettings();

	int getPersonView() const { return personView; }
	void setPersonView(int view) { personView = view; }
	float getCameraSensitive() { return cameraSensitive; }
	void setCameraSensitive(float sensitive) { cameraSensitive = sensitive; }
	float getFovSetting() { return fovSetting; }
	void setFovSetting(float fov) { fovSetting = fov; }
	float getMouseSensitivity() { return mouseSensitivity; }
	void setMouseSensitivity(float sensitive) { mouseSensitivity = sensitive; }
	float getMainGuiWidth() { return playerActivityGuiSize * GUI_WIDTH; }
	float getMainGuiHeight() { return playerActivityGuiSize * GUI_HEIGHT; }

	static void setMouseHistoryDuration(ui32 duration) { mouseHistoryDuration = duration; }

	const String& getKeyBindingDescription(int k);
	String getOptionDisplayString(int k);
	static String getKeyDisplayString(int k);
	
	void setKeyBinding(int k, int nk);
	bool shouldRenderClouds() { return clouds; }
	void beginMouseMove(float x, float y);
	void endMouseMove(float x, float y);
	void setMouseMoveState(bool bState) { m_mouseMoving = bState; }
	bool isMouseMoving() const { return m_mouseMoving; }
	void setMousePos(float x, float y);
	Vector2 getMousePos() const;
	void switchMoveState();
	void setMoveState(int state) { m_curMoveState = (MoveState)(state % (int)MoveState::TotalState); }
	void updateClientInput();

	void setAutoRunStatus(bool isAutoRunStatus) { m_isAutoRunStatus = isAutoRunStatus; }
	bool getAutoRunStatus() { return m_isAutoRunStatus; }

public:
	enum USER_ACTION_TYPE{
		ACTION_NULL,
		ACTION_CLICK,
		ACTION_LONG_TOUCH_BEGIN,
		ACTION_LONG_TOUCH_END
	};

	USER_ACTION_TYPE userAction = ACTION_NULL;

};

}

#endif
