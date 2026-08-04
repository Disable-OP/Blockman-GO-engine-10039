#include "GameSettings.h"
#include "Object/Root.h"
#include "Entity/EntityPlayer.h"

namespace BLOCKMAN
{

KeyBinding::KeyBindingArr KeyBinding::keybindArray;
KeyBinding::KeyBindingMap KeyBinding::keybindMap;
KeyBinding::NameKeyBindingMap KeyBinding::nameKeyBindMap;

const String KeyBinding::KEY_FORWORD = "key.forward";
const String KeyBinding::KEY_LEFT = "key.left";
const String KeyBinding::KEY_RIGHT = "key.right";
const String KeyBinding::KEY_BACK = "key.back";
const String KeyBinding::KEY_JUMP = "key.jump";
const String KeyBinding::KEY_INVENTORY = "key.inventory";
const String KeyBinding::KEY_DROP = "key.drop";
const String KeyBinding::KEY_CHAT = "key.chat";
const String KeyBinding::KEY_SNEAK = "key.sneak";
const String KeyBinding::KEY_ATTACK = "key.attack";
const String KeyBinding::KEY_USE = "key.use";
const String KeyBinding::KEY_PLAYERLIST = "key.playerlist";
const String KeyBinding::KEY_PICKITEM = "key.pickItem";
const String KeyBinding::KEY_COMMAND = "key.command";
const String KeyBinding::KEY_CHANGE = "key.change";
const String KeyBinding::KEY_EXIT= "key.exit";
const String KeyBinding::KEY_ANDROID_BACK = "key.android.back";
const String KeyBinding::KEY_TOP_LEFT = "key.top.left";
const String KeyBinding::KEY_TOP_RIGHT = "key.top.right";

void KeyBinding::onTick(int k)
{
	KeyBindingMap::iterator it = keybindMap.find(k);
	if (it != keybindMap.end())
	{
		++(it->second->pressTime);
	}
}

void KeyBinding::setKeyBindState(int k, bool press)
{
	KeyBindingMap::iterator it = keybindMap.find(k);
	if (it != keybindMap.end())
	{
		it->second->pressed = press;
	}
}

void KeyBinding::setKeyState(bool press)
{
	pressed = press;
}

void KeyBinding::setKeyBindState(const String& name, bool press)
{
	NameKeyBindingMap::iterator it = nameKeyBindMap.find(name);
	if (it != nameKeyBindMap.end())
		it->second->pressed = press;
}

void KeyBinding::unPressAllKeys()
{
	for (KeyBindingArr::iterator it = keybindArray.begin(); it != keybindArray.end(); ++it)
	{
		if(*it)
			(*it)->unpressKey();
	}
}

void KeyBinding::resetKeyBindingArrayAndHash()
{
	keybindMap.clear();
	nameKeyBindMap.clear();
	for (KeyBindingArr::iterator it = keybindArray.begin(); it != keybindArray.end(); ++it)
	{
		keybindMap.insert(std::make_pair((*it)->keyCode, *it));
		nameKeyBindMap.insert(std::make_pair((*it)->keyDescription, *it));
	}
}

void KeyBinding::clearKeyBingdingArrayAndHash()
{
    keybindArray.clear();
    keybindMap.clear();
    nameKeyBindMap.clear();
}
    
KeyBinding* KeyBinding::getKeyBinding(int k)
{
	KeyBindingMap::iterator it = keybindMap.find(k);
	if (it != keybindMap.end())
	{
		return it->second;
	}
	return NULL;
}

KeyBinding::KeyBinding(const String& des, int k)
{
	keyDescription = des;
	keyCode = k;
	keybindArray.push_back(this);
	keybindMap.insert(std::make_pair(k, this));
	nameKeyBindMap.insert(std::make_pair(des, this));
	pressed = false;
	pressTime = 0;
}

    KeyBinding::~KeyBinding()
    {
        clearKeyBingdingArrayAndHash();
    }
    
bool KeyBinding::isPressed()
{
	if (pressTime == 0)
	{
		return false;
	}
	else
	{
		--pressTime;
		return true;
	}
}

void KeyBinding::unpressKey()
{
	pressTime = 0;
	pressed = false;
}

String GameSettings::RENDER_DISTANCES[4] = { "options.renderDistance.far", "options.renderDistance.normal", "options.renderDistance.short", "options.renderDistance.tiny" };
String GameSettings::DIFFICULTIES[4] = { "options.difficulty.peaceful", "options.difficulty.easy", "options.difficulty.normal", "options.difficulty.hard" };
String GameSettings::GUISCALES[4] = { "options.guiScale.auto", "options.guiScale.small", "options.guiScale.normal", "options.guiScale.large" };
String GameSettings::CHAT_VISIBILITIES[3] = { "options.chat.visibility.full", "options.chat.visibility.system", "options.chat.visibility.hidden" };
String GameSettings::PARTICLES[3] = { "options.particles.all", "options.particles.decreased", "options.particles.minimal" };
String GameSettings::LIMIT_FRAMERATES[3] = { "performance.max", "performance.balanced", "performance.powersaver" };
String GameSettings::AMBIENT_OCCLUSIONS[3] = { "options.ao.off", "options.ao.min", "options.ao.max" };
ui32 GameSettings::mouseHistoryDuration = 80;

GameSettings::GameSettings()
{
	keyBindForward = LordNew KeyBinding(KeyBinding::KEY_FORWORD, 'W');
	keyBindLeft = LordNew KeyBinding(KeyBinding::KEY_LEFT, 'A');
	keyBindBack = LordNew KeyBinding(KeyBinding::KEY_BACK, 'S');
	keyBindRight = LordNew KeyBinding(KeyBinding::KEY_RIGHT, 'D');
	keyBindJump = LordNew KeyBinding(KeyBinding::KEY_JUMP, ' ');
	keyBindInventory = LordNew KeyBinding(KeyBinding::KEY_INVENTORY, 'Y');
	keyBindDrop = LordNew KeyBinding(KeyBinding::KEY_DROP, 'R');
	keyBindChat = LordNew KeyBinding(KeyBinding::KEY_CHAT, 'T');
	keyBindSneak = LordNew KeyBinding(KeyBinding::KEY_SNEAK, 16); // shift key
	keyBindAttack = LordNew KeyBinding(KeyBinding::KEY_ATTACK, -100);
	keyBindUseItem = LordNew KeyBinding(KeyBinding::KEY_USE, -99);
	keyBindPlayerList = LordNew KeyBinding(KeyBinding::KEY_PLAYERLIST, 'P');
	keyBindPickBlock = LordNew KeyBinding(KeyBinding::KEY_PICKITEM, -98);
	keyBindCommand = LordNew KeyBinding(KeyBinding::KEY_COMMAND, 'C');
	keyBindChange = LordNew KeyBinding(KeyBinding::KEY_CHANGE, 9);
	keyBindExit = LordNew KeyBinding(KeyBinding::KEY_EXIT, 27);
	keyBindAndroindBack = LordNew KeyBinding(KeyBinding::KEY_ANDROID_BACK, 4);
	keyBindTopLeft = LordNew KeyBinding(KeyBinding::KEY_TOP_LEFT, 'Q');
	keyBindTopRight = LordNew KeyBinding(KeyBinding::KEY_TOP_RIGHT, 'E');
	
	musicVolume = 1.0F;
	soundVolume = 1.0F;
	mouseSensitivity = 0.5F;
	invertMouse = false;

#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
#	if (LORD_MODE == LORD_MODE_DEBUG)
	renderDistance = 2;
	fancyGraphics = true;
#	else
	renderDistance = 2;
	fancyGraphics = true;
#	endif
#else
	renderDistance = 2;
	fancyGraphics = true;
#endif

	viewBobbing = true;
	anaglyph = false;
	advancedOpengl = false;
	limitFramerate = 1;
	ambientOcclusion = 2;
	clouds = true;
	skin = "Default";
	chatVisibility = 0;
	chatColours = true;
	chatLinks = true;
	chatLinksPrompt = true;
	chatOpacity = 1.0F;
	serverTextures = true;
	snooperEnabled = true;
	fullScreen = false;
	enableVsync = true;
	hideServerAddress = false;
	advancedItemTooltips = false;
	pauseOnLostFocus = true;
	showCape = true;
	touchscreen = false;
	overrideWidth = 0;
	overrideHeight = 0;
	heldItemTooltips = true;
	chatScale = 1.0F;
	chatWidth = 1.0F;
	chatHeightUnfocused = 0.44366196F;
	chatHeightFocused = 1.0F;
	hideGUI = false;
	personView = SPVT_FIRST;
	showDebugInfo = false;
	showDebugProfilerChart = false;
	noclip = false;
	smoothCamera = false;
	debugCamEnable = false;
	fovSetting = 0.7f;
	gammaSetting = 1.f;
	guiScale = 0;
	particleSetting = 0;

	difficulty = 2;
	lastServer = "";
	noclipRate = 1.0F;
	debugCamRate = 1.0F;
	language = "en_US";
	playerActivityGuiSize = 1.0f;
	cameraSensitive = 4.5;
	m_clientMovementInput = LordNew MovementInput();
}

GameSettings::~GameSettings()
{
	LordSafeDelete(keyBindForward);
	LordSafeDelete(keyBindLeft);
	LordSafeDelete(keyBindBack);
	LordSafeDelete(keyBindRight);
	LordSafeDelete(keyBindJump);
	LordSafeDelete(keyBindInventory);
	LordSafeDelete(keyBindDrop);
	LordSafeDelete(keyBindChat);
	LordSafeDelete(keyBindSneak);
	LordSafeDelete(keyBindAttack);
	LordSafeDelete(keyBindUseItem);
	LordSafeDelete(keyBindPlayerList);
	LordSafeDelete(keyBindPickBlock);
	LordSafeDelete(keyBindCommand);
	LordSafeDelete(keyBindChange);
	LordSafeDelete(keyBindExit);
	LordSafeDelete(keyBindAndroindBack);
	LordSafeDelete(keyBindTopLeft);
	LordSafeDelete(keyBindTopRight);
	LordSafeDelete(m_clientMovementInput);
}

const String& GameSettings::getKeyBindingDescription(int k)
{
	KeyBinding* kb = KeyBinding::getKeyBinding(k);
	if (kb)
		return kb->keyDescription;
	return StringUtil::BLANK;
}

String GameSettings::getOptionDisplayString(int k)
{
	KeyBinding* kb = KeyBinding::getKeyBinding(k);
	if (kb)
		getKeyDisplayString(kb->keyCode);
	return StringUtil::BLANK;
}

String GameSettings::getKeyDisplayString(int k)
{
	if (k < 0)
	{
		String mouseName = "Mouse_";
		mouseName += char(k + 101);
		return mouseName;
	}
	else
	{
		String keyName = "Key_";
		keyName += char(k);
		return keyName;
	}
	return StringUtil::BLANK;
}

void GameSettings::setKeyBinding(int k, int nk)
{
	KeyBinding* kb = KeyBinding::getKeyBinding(k);
	if (kb)
		kb->keyCode = nk;
}

void GameSettings::beginMouseMove(float x, float y)
{
	m_mouseMoving = true;
	m_mouseHistory.clear();
	setMousePos(x, y);
}

void GameSettings::endMouseMove(float x, float y)
{
	m_mouseMoving = false;
	setMousePos(x, y);
}

int GameSettings::getRenderRange()
{
	static int diss[3] = { 352, 256, 160 };
	return diss[renderDistance];
}

int GameSettings::getRenderChunksRange()
{
	int distance = getRenderRange();

	return distance / 16 + 1;
}

int GameSettings::getDetectChunksRange()
{
	return getRenderChunksRange() + 4;
}

int GameSettings::getLoadChunksRange()
{
	return getDetectChunksRange() + 4;
}

void GameSettings::setMousePos(float x, float y)
{
	auto time = Root::Instance()->getCurrentTime();
	m_mouseHistory.push_back({ {x, y}, time });
	while (m_mouseHistory.size() > 2 && time - m_mouseHistory[1].time > mouseHistoryDuration)
	{
		m_mouseHistory.pop_front();
	}
}

Vector2 GameSettings::getMousePos() const
{
	auto time = Root::Instance()->getCurrentTime();
	if (m_mouseHistory.empty())
	{
		return Vector2::ZERO;
	}
	if (time - m_mouseHistory.back().time >= mouseHistoryDuration)
	{
		return m_mouseHistory.back().point;
	}
	Vector2 ret = float(time - m_mouseHistory.back().time) * m_mouseHistory.back().point;
	for (int i = m_mouseHistory.size() - 2; i >= 0; --i)
	{
		if (time - m_mouseHistory[i].time > mouseHistoryDuration)
		{
			ret += float(mouseHistoryDuration - (time - m_mouseHistory[i + 1].time)) * m_mouseHistory[i].point;
			break;
		}
		ret += float(m_mouseHistory[i + 1].time - m_mouseHistory[i].time) * m_mouseHistory[i].point;
	}
	if (time - m_mouseHistory.front().time < mouseHistoryDuration)
	{
		ret += float(mouseHistoryDuration - (time - m_mouseHistory.front().time)) * m_mouseHistory.front().point;
	}
	ret /= float(mouseHistoryDuration);
	return ret;
}

void GameSettings::switchMoveState()
{
	int nextState = (int)m_curMoveState + 1;
	nextState = nextState % (int)MoveState::TotalState;
	m_curMoveState = (MoveState)nextState;
}

void GameSettings::updateClientInput()
{
	if (usePole < 1)
	{
		m_clientMovementInput->moveStrafe = 0.0F;
		m_clientMovementInput->moveForward = 0.0F;

		if (keyBindForward->pressed || getAutoRunStatus() )
		{
			++m_clientMovementInput->moveForward;
		}

		if (keyBindBack->pressed)
		{
			--m_clientMovementInput->moveForward;
		}

		if (keyBindLeft->pressed)
		{
			++m_clientMovementInput->moveStrafe;
		}

		if (keyBindRight->pressed)
		{
			--m_clientMovementInput->moveStrafe;
		}

		if (keyBindTopLeft->pressed)
		{
			m_clientMovementInput->moveForward = m_clientMovementInput->moveForward + 0.7f;
			m_clientMovementInput->moveStrafe = m_clientMovementInput->moveStrafe + 0.7f;
		}

		if (keyBindTopRight->pressed)
		{
			m_clientMovementInput->moveForward = m_clientMovementInput->moveForward + 0.7f;
			m_clientMovementInput->moveStrafe = m_clientMovementInput->moveStrafe - 0.7f;
		}
	}
	else
	{
		m_clientMovementInput->moveForward = m_poleForward;
		m_clientMovementInput->moveStrafe = m_poleStrafe;
	}

	m_clientMovementInput->down = keyBindSneak->pressed;
	m_clientMovementInput->jump = keyBindJump->pressed;
	m_clientMovementInput->sneak = m_clientMovementInput->sneakPressed || m_curMoveState == MoveState::SneakState;
	m_clientMovementInput->isSprintInput = m_curMoveState == MoveState::SprintState;

	if (m_clientMovementInput->sneak)
	{
		m_clientMovementInput->moveStrafe = m_clientMovementInput->moveStrafe * 0.3f;
		m_clientMovementInput->moveForward = m_clientMovementInput->moveForward * 0.3f;
	}
}

}
