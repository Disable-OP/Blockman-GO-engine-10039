/********************************************************************
filename: 	RenderEntity.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-12-12
*********************************************************************/
#ifndef __RENDER_ENTITY_HEADER__
#define __RENDER_ENTITY_HEADER__

#include "Util/Random.h"

namespace BLOCKMAN
{

class Blockman;
class World;
class EntityLivingBase;
class Entity;
class ItemRenderer;
class EntityPlayer;

class MouseFilter
{
public:
	MouseFilter();
	float smooth(float par1, float par2);
protected:
	float a = 0.f;
	float b = 0.f;
	float c = 0.f;
};

struct LightSetting
{
	Color MainLightColor;
	Color SubLightColor;
};

class RenderEntity : public ObjectAlloc
{
public:
	static bool anaglyphEnable;
	/** Anaglyph field (0=R, 1=GB) */
	static int anaglyphField;

	/** A reference to the Blockman object. */
	ItemRenderer* itemRenderer = nullptr;
		
	/** Debug view direction (0=OFF, 1=Front, 2=Right, 3=Back, 4=Left, 5=TiltLeft, 6=TiltRight) */
	int debugViewDirection = 0;

	Texture* getLightmap() { return lightmapTexture; }

protected:
	Blockman* bm = nullptr;
	/** Entity renderer update count */
	int rendererUpdateCount = 0;
	float farPlaneDistance = 0.f;  
	/** Pointed entity */
	Entity* pointedEntity = nullptr;
	MouseFilter mouseFilterXAxis;
	MouseFilter mouseFilterYAxis;
	int prevMouseX = 0;
	int prevMouseY = 0;

	float thirdPersonDistance = 0.f;

	/** Third person distance temp */
	float thirdPersonDistanceTemp = 0.f;
	float debugCamYaw = 0.f;
	float prevDebugCamYaw = 0.f;
	float debugCamPitch = 0.f;
	float prevDebugCamPitch = 0.f;

	bool m_isOnVehicle = false;

	/** Smooth cam yaw */
	float smoothCamYaw = 0.f;

	/** Smooth cam pitch */
	float smoothCamPitch = 0.f;

	/** Smooth cam filter X */
	float smoothCamFilterX = 0.f;

	/** Smooth cam filter Y */
	float smoothCamFilterY = 0.f;

	/** Smooth cam partial ticks */
	float smoothCamPartialTicks = 0.f;
	float debugCamFOV = 0.f;
	float prevDebugCamFOV = 0.f;
	float camRoll = 0.f;
	float prevCamRoll = 0.f;

	/** The texture id of the blocklight/skylight texture used for lighting effects */
	Texture* lightmapTexture = nullptr;
	int* lightmapColors = nullptr;

	/** FOV modifier hand */
	float fovModifierHand = 0.f;
	/** FOV modifier hand prev */
	float fovModifierHandPrev = 0.f;
	/** FOV multiplier temp */
	float fovMultiplierTemp = 0.f;

	/** Cloud fog mode */
	bool cloudFog = false;
	float cameraZoom = 0.f;
	float cameraYaw = 0.f;
	float cameraPitch = 0.f;
	/** the offset of the camera to player's eye position*/
	Vector3 cameraOffset;

	/** Previous frame time in milliseconds */
	i64 prevFrameTime = 0;

	/** End time of last render (ns) */
	i64 renderEndNanoTime = 0;

	/** Is set, updateCameraAndRender() calls updateLightmap(); set by updateTorchFlicker()*/
	bool lightmapUpdateNeeded = false;

	/** Torch flicker X */
	float torchFlickerX = 0.f;

	/** Torch flicker DX */
	float torchFlickerDX = 0.f;

	/** Torch flicker Y */
	float torchFlickerY = 0.f;

	/** Torch flicker DY */
	float torchFlickerDY = 0.f;
	Random random;

	/** Rain sound counter */
	int rainSoundCounter = 0;

	/** Rain X coords */
	float* rainXCoords = nullptr;

	/** Rain Y coords */
	float* rainYCoords = nullptr;

	FxRenderable* m_snowRenderer = nullptr;
	FxRenderable* m_rainRenderer = nullptr;
	Texture* m_rainTex = nullptr;
	Texture* m_snowTex = nullptr;

	/** red component of the fog color */
	Color fogColor;

	/** Fog color */
	float fogColor2 = 0.f;
	float fogColor1 = 0.f;
	Vector2 m_prevMousePos = Vector2::INVALID;

	LightSetting m_lightSettings[24];
		
	bool m_isPlayingEndingAnimation = false;
	float m_endingAnimationRotationAngle = 0.0f;

	float m_closeupDeltaDis = 0.0f;
	float m_closeupDuration = 0.0f;
	float m_screenShakingValue = 0.0f;

public:
	RenderEntity(Blockman* _mc);
	~RenderEntity();
	
	/** Finds what block or object the mouse is over at the specified partial tick time. Args: partialTickTime */
	void getMouseOver(float rdt);
	
	/** Will update any inputs that effect the camera angle (mouse) and then render the world and GUI */
	void updateCameraAndRender(float dt, float rdt);
	
	/** Updates the entity renderer */
	void updateRenderer();

	void renderWorld(float dt, float rdt, i64 tickcount);

	/** Converts performance value (0-2) to FPS (35-200) */
	static int performanceToFps(int par0);

	int getLightMapColor(float u, float v);
	int getLightMapColor(int skyBlockLight);
	
	void setEndingAnimationStatus(bool isPlayingEndingAnimation) { m_isPlayingEndingAnimation = isPlayingEndingAnimation; }
	void setEndingAnimationRotationAngle(float endingAnimationRotationAngle) { m_endingAnimationRotationAngle = endingAnimationRotationAngle; }

	void resetCloseup()
	{
		m_closeupDeltaDis = 0.0f;
		m_closeupDuration = 0.0f;
	}

protected:
	int createLightMap();
	void loadLightSettings();

	/** Update FOV modifier hand */
	void updateFovModifierHand();

	int getBlockIdAtEntityViewpoint(World* pWorld, EntityLivingBase* pLiving, float ticks);

	/** Changes the field of view of the player depending on if they are usesetting or not */
	float getFOVModifier(float rdt, bool usesetting);

	// void hurtCameraEffect(float par1);

	/** Setups all the GL settings for view bobbing. */
	void setupViewBobbing(float rdt, Matrix4& matView);

	/** sets up player's eye (or camera in third person mode) */
	void orientCamera(float rdt, Matrix4& matView);

	/** sets up projection, view effects, camera position/rotation */
	void setupCameraTransform(float rdt);

	/** if the player is in hurting, change the roll of the camera. */
	void hurtCemaraEffect(float rdt, Matrix4& matView);

	/** Render player hand */
	void renderHand(float rdt);
		
	/** Recompute a random value that is applied to block color in updateLightmap() */
	void updateTorchFlicker();

	void updateLightmap(float rdt);
	
	void updateEnchantTexMats(float rdt);
	
	/** Gets the night vision brightness */
	float getNightVisionBrightness(EntityPlayer* pPlayer, float rdt);

	/** Render clouds if enabled */
	// void renderCloudsCheck(RenderGlobal par1RenderGlobal, float par2);
	
	void addRainParticles();
	
	/** Render rain and snow */
	void renderRainSnow(float rdt);
	
	
	/** calculates fog and calls glClearColor */
	void updateFogColor(float rdt);
	
	/** Sets up the fog to be rendered. If the arg passed in is -1 the fog starts at 0 and
	goes to 80% of far plane distance and is used for sky rendering. */
	void setupFog(int fogMode, float dtTime);

	void updateLightSetting(float rdt);

private:
	void clearMouseOver();
};

}

#endif
