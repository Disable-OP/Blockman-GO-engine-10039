
/********************************************************************
filename: 	Blockman.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-11-15
*********************************************************************/
#ifndef __BLOCKMAN_HEADER__
#define __BLOCKMAN_HEADER__

/** header files from common. */
#include "BM_TypeDef.h"
#include "World/RayTracyResult.h"
#include "UI/Touch.h"

namespace BLOCKMAN
{
class World;
class WorldSettings;
class WorldProvider;
class TextureAtlasRegister;
class RenderGlobal;
class GameSettings;
class RenderEntity;
class PlayerController;
class EntityPlayerSPMP;
class EntityLivingBase;
class EntityPlayerRenderable;
class RightArmRenderable;
class PlayerInteractionManager;
class RootGuiLayout;
class EntityPlayer;
class VisualTessThread;
class LightTransferThread;


class Blockman : public Singleton<Blockman>, public ObjectAlloc
{
public:
	enum MOUSE_BUTTON_STATE
	{
		MBS_UP = 0,
		MBS_DOWN = 1,
		MBS_MOVE = 2,
		MBS_WHEEL = 3
	};

	struct MouseInfo
	{
		i16 key;	
		i16 state;	
		i16 x;
		i16 y;
	};

	typedef LORD::deque<MouseInfo>::type MouseInfos;

protected:
	void sendClickSceneToController(bool touchHold);
	void longTouchScene();
	void clickScene();
	bool longTouchUseType(ItemStackPtr pStack);
	bool canContinuousUse(ItemStackPtr pStack);

public:
	static void initialize();
	static void unInitialize();
	void ManualAttack();

	Blockman();
	~Blockman();

	void init();
	void tick(float dt);
	void runTick();
	void setPersonView(int view);
	void generateWorld(int sex, int defaultIdle);
	void refreshActorHide();
	void saveChunks();
	void setDimensionAndSpawnPlayer(int dimension);
	void loadWorld(World* pWorld, const String& worldName, int sex, int defaultIdle);
	void onGameReady();
	void rebirth(const Vector3& spawnPos, int sex, const String& showName);
	i32 getSystemTime();
	String getDebugString();
	String getPlayerString();
	String getRayTraceString();
	String getAsyncString() { return m_asyncString; }
	void caculateCpuTime();
	void setClientDebugString(String str) {
		m_clientDebugString = str;
	}
	String getClientDebugString() {
		return m_clientDebugString;
	}
	void setServerDebugString(String str) {
		m_serverDebugString = str;
	}
	String getServerDebugString() {
		return m_serverDebugString;
	}

	//add by maxicheng
	static void initializeNoGUI();
	void init(i64 seed);
	World * getWorld(){return m_pWorld;}

	void switchPerspece();
	int  getCurrPersonView();
	void setPerspece(int view);
	void syncBlockDestroyeFailure(bool destroyeStatus, int destroyeFailureType);

	Vector3 getPlayerPosition();
	BlockPos getBlockPosition();
	float getPlayerYaw();
	int getBlockId();

	void beginAsync(RenderGlobal* rg);
	void finishAsync();
	void beginLightCalcAsync(bool isIdle);
	bool finishLightAsync();
	void initCloseup(bool isShow, const Vector3& playerPos, const Vector3& closeupPos, float farDistance, float nearDistance, float velocity, float duration, float yaw, float pitch);
	void resetCloseup();
	bool checkClickCoolingTime(ItemStackPtr heldItem);

private:
	String m_clientDebugString;
	String m_serverDebugString;

public:
	World*				m_pWorld = nullptr;
	WorldSettings*		m_worldSettings = nullptr;
	GameSettings*		m_gameSettings = nullptr;
	String				m_worldName;	
	PlayerController*	m_playerControl = nullptr;
	EntityPlayerSPMP*	m_pPlayer = nullptr;
	
	RootGuiLayout*		m_rootGuiLayout = nullptr;
	int					m_accelerateTime = 1;
	
	PlayerInteractionManager* m_interactionMgr = nullptr;
	
	RenderEntity*		m_entityRender = nullptr;
	RenderGlobal*		m_globalRender = nullptr;
	VisualTessThread*	m_visualTessThread = nullptr;
	LightTransferThread* m_lightTransferTrhead = nullptr;
	float				m_tickTime = 0.f;
	static const float	s_tickFPS;
	float				m_render_dt = 0.f;
	int					m_logicSystemTime = 0;

	EntityLivingBase*   renderViewEntity = nullptr;
	EntityLivingBase*	pointedEntityLiving = nullptr;

	RayTraceResult		objectMouseOver;

	int					joinPlayerCounter = 0;
	int					debugCrashKeyPressTime = 0;

	ui64                m_worlTick = 0;
	int					isBlockDestoryStatus = true;

	ui64				m_mainBegin = 0;
	ui64				m_renderBegin = 0;
	ui64				m_asyncBegin = 0;
	ui64				m_asyncEnd = 0;
	String				m_asyncString;

	Matrix4				m_firstPersonItemMat;

	bool				m_closeup_isShow = false;
	Vector3				m_closeup_playerPos = Vector3::ZERO;
	Vector3				m_closeup_closeupPos = Vector3::ZERO;
	float				m_closeup_farDis = 0.0f;
	float				m_closeup_nearDis = 0.0f;
	float				m_closeup_velocity = 0.0f;
	float				m_closeup_duration = 0.0f;
	float				m_closeup_yaw = 0.0f;
	float				m_closeup_pitch = 0.0f;
	bool				m_isShowGunFlameCoordinate = false;

private:
	/** Mouse click counter */
	int					m_clickCounter	= 0;
	int					m_lastUseItemId = -1;
	bool				m_isGameReady = false;
	Vector3i				m_newBolckPos = BlockPos::ZERO;
	i32					m_useToolGatherTime = 0;
	bool				m_cdDone = true;
};

}

#endif
