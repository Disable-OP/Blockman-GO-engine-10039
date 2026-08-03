#include "RenderEntity.h"
#include "RenderGlobal.h"
#include "ItemRender.h"
#include "ActiveRenderInfo.h"
#include "VisualTessThread.h"

#include "Block/Block.h"
#include "Block/Blocks.h"
#include "Block/BlockManager.h"
#include "Block/BM_Material.h"
#include "World/GameSettings.h"
#include "World/RayTracyResult.h"
#include "World/World.h"
#include "World/WorldProvider.h"
#include "Entity/EntityLivingBase.h"
#include "Entity/Enchantment.h"
#include "Item/Potion.h"
#include "Item/Items.h"
#include "WorldGenerator/BiomeGen.h"
#include "World/WorldChunkManager.h"
#include "World/WorldEffectManager.h"

#include "cEntity/PlayerControlller.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "EntityRenders/EntityRenderable.h"
#include "cWorld/WorldClient.h"
#include "cEffects/EffectRenderer.h"
#include "cEffects/EntitySmokeFx.h"
#include "cEffects/EntityRainFx.h"

#include "Object/Root.h"
#include "Scene/Camera.h"
#include "Tessolator/TessManager.h"
#include "Tessolator/TessRenderable.h"
#include "Tessolator/FxRenderable.h"
#include "game.h"
#include "Common.h"

#include "rapidjson/document.h"
#include "Setting/LogicSetting.h"
#include "Entity/EntityBird.h"

namespace BLOCKMAN
{

MouseFilter::MouseFilter()
	: a(0.f)
	, b(0.f)
	, c(0.f)
{}

float MouseFilter::smooth(float par1, float par2)
{
	a += par1;
	par1 = (a - b) * par2;
	c += (par1 - c) * 0.5F;

	if (par1 > 0.0F && par1 > c || par1 < 0.0F && par1 < c)
	{
		par1 = c;
	}

	b += par1;
	return par1;
}

bool RenderEntity::anaglyphEnable = false;
int RenderEntity::anaglyphField = 0;

RenderEntity::RenderEntity(Blockman* _mc)
{
	bm = _mc;

	itemRenderer = LordNew ItemRenderer(_mc);
	debugViewDirection = 0;
	rendererUpdateCount = 0;
	farPlaneDistance = 0;
	thirdPersonDistance = 3.f;
	thirdPersonDistanceTemp = 3.f;
	debugCamYaw = 0.f;
	prevDebugCamYaw = 0.f;
	debugCamPitch = 0.f;
	prevDebugCamPitch = 0.f;
	smoothCamYaw = 0.f;
	smoothCamPitch = 0.f;
	smoothCamFilterX = 0.f;
	smoothCamFilterY = 0.f;
	smoothCamPartialTicks = 0.f;
	debugCamFOV = 0.f;
	prevDebugCamFOV = 0.f;
	camRoll = 0.f;
	prevCamRoll = 0.f;
	lightmapTexture = NULL;
	lightmapColors = NULL;
	fovModifierHand = 0.f;
	fovModifierHandPrev = 0.f;
	fovMultiplierTemp = 0.f;
	cloudFog = false;
	cameraZoom = 1.0;
	cameraYaw = 0.0;
	cameraPitch = 0.0;
	prevFrameTime = Root::Instance()->getCurrentTime();
	renderEndNanoTime = 0;
	lightmapUpdateNeeded = true;
	torchFlickerX = 0.f;
	torchFlickerDX = 0.f;
	torchFlickerY = 0.f;
	torchFlickerDY = 0.f;
	rainSoundCounter = 0;
	fogColor2 = 0.f;
	fogColor1 = 0.f;
	prevMouseX = 0;
	prevMouseY = 0;
	pointedEntity = NULL;
	cameraOffset = Vector3::ZERO;

	createLightMap();

	for (int i=0; i<24; ++i)
	{
		m_lightSettings[i].MainLightColor = Color(0.94f, 0.89f, 0.769f, 1.641f);
		m_lightSettings[i].SubLightColor = Color(0.531f, 0.765f, 0.751f, 0.645f);
	}
	loadLightSettings();
}

RenderEntity::~RenderEntity()
{
	LordSafeFree(lightmapColors);
	LordSafeDelete(itemRenderer);
	TextureManager::Instance()->releaseResource(lightmapTexture);
	if (rainXCoords)
	{
		LordSafeFree(rainXCoords);
		LordSafeFree(rainYCoords);
		TessManager::Instance()->destroyFxRenderable(m_rainRenderer);
		TessManager::Instance()->destroyFxRenderable(m_snowRenderer);
		TextureManager::Instance()->releaseResource(m_rainTex);
		TextureManager::Instance()->releaseResource(m_snowTex);
		m_rainRenderer = nullptr;
		m_snowRenderer = nullptr;
		m_rainTex = nullptr;
		m_snowTex = nullptr;
	}
}

int RenderEntity::createLightMap()
{
	LordAssert(!lightmapTexture);

	const int lightmapSize = 16;

	int nBufferSize = lightmapSize * lightmapSize * sizeof(int);
	lightmapColors = (int*)LordMalloc(nBufferSize);
	memset(lightmapColors, 1, nBufferSize);
	Buffer buff(nBufferSize, lightmapColors);

	lightmapTexture = TextureManager::Instance()->createManual("lightMap", Texture::TT_2D, PF_RGBA8_UNORM,
		Texture::TU_STATIC, lightmapSize, lightmapSize, 1, 1, buff);

	TessManager::Instance()->setLightMap(lightmapTexture);

	return true;
}

void RenderEntity::updateRenderer()
{
	updateFovModifierHand();
	updateTorchFlicker();
	fogColor2 = fogColor1;
	thirdPersonDistance = LogicSetting::Instance()->getThirdPersonDistance();
	thirdPersonDistanceTemp = thirdPersonDistance;
	prevDebugCamYaw = debugCamYaw;
	prevDebugCamPitch = debugCamPitch;
	prevDebugCamFOV = debugCamFOV;
	prevCamRoll = camRoll;
	float var1;
	float var2;

	if (bm->m_gameSettings->smoothCamera)
	{
		var1 = bm->m_gameSettings->getMouseSensitivity() * 0.6F + 0.2F;
		var2 = var1 * var1 * var1 * 8.0F;
		smoothCamFilterX = mouseFilterXAxis.smooth(smoothCamYaw, 0.05F * var2);
		smoothCamFilterY = mouseFilterYAxis.smooth(smoothCamPitch, 0.05F * var2);
		smoothCamPartialTicks = 0.0F;
		smoothCamYaw = 0.0F;
		smoothCamPitch = 0.0F;
	}

	if (!bm->renderViewEntity)
	{
		bm->renderViewEntity = bm->m_pPlayer;
	}

	// first use camera postion as the entity pos.
	var1 = bm->m_pWorld->getLightBrightness(bm->renderViewEntity->getPosition().getFloor());

	var2 = (float)(3 - bm->m_gameSettings->renderDistance) / 3.0F;
	float var3 = var1 * (1.0F - var2) + var2;
	fogColor1 += (var3 - fogColor1) * 0.1F;
	++rendererUpdateCount;
	itemRenderer->updateEquippedItem();
	// addRainParticles();
}

void RenderEntity::updateFovModifierHand()
{
	EntityPlayerSP* pPlayer = (EntityPlayerSP*)bm->renderViewEntity;
	fovMultiplierTemp = pPlayer->getFOVMultiplier();
	fovModifierHandPrev = fovModifierHand;
	fovModifierHand += (fovMultiplierTemp - fovModifierHand) * 0.5F;

	if (fovModifierHand > 1.5F)
	{
		fovModifierHand = 1.5F;
	}

	if (fovModifierHand < 0.1F)
	{
		fovModifierHand = 0.1F;
	}
}

void RenderEntity::updateTorchFlicker()
{
	torchFlickerDX = (float)((double)torchFlickerDX + (Math::UnitRandom() - Math::UnitRandom()) * Math::UnitRandom() * Math::UnitRandom());
	torchFlickerDY = (float)((double)torchFlickerDY + (Math::UnitRandom() - Math::UnitRandom()) * Math::UnitRandom() * Math::UnitRandom());
	torchFlickerDX = (float)((double)torchFlickerDX * 0.9);
	torchFlickerDY = (float)((double)torchFlickerDY * 0.9);
	torchFlickerX += (torchFlickerDX - torchFlickerX) * 1.0F;
	torchFlickerY += (torchFlickerDY - torchFlickerY) * 1.0F;
	lightmapUpdateNeeded = true;
}

void RenderEntity::loadLightSettings()
{
	DataStream* pJSONStream = ResourceGroupManager::Instance()->openResource("24HoursLighting.json");
	if (pJSONStream == NULL)
	{
		LordLogError("RenderQueueDefine file 24HoursLighting.json not found.");
		return;
	}

	size_t fileSize = pJSONStream->size();
	char* jsonData = (char*)LordMalloc(fileSize + 1);
	pJSONStream->read(jsonData, fileSize);
	jsonData[fileSize] = 0;

	rapidjson::Document* doc = new rapidjson::Document;
	doc->Parse(jsonData);
	if (doc->HasParseError())
	{
		LordLogError("invalid 24HoursLighting file.");
	}
	else
	{
		LordAssert(doc->IsObject());
		const rapidjson::Value& settings = doc->FindMember("LightSettings")->value;

		LordAssert(settings.IsArray());

		for (rapidjson::Value::ConstValueIterator it = settings.Begin(); it != settings.End(); ++it)
		{
			String mainColor = it->FindMember("MainLightColor")->value.GetString();
			String subColor = it->FindMember("SubLightColor")->value.GetString();
			//String fogColor = it->FindMember("FogColor")->value.GetString();
			//String clouldColor = it->FindMember("CloudLight")->value.GetString();
			//String skyColor = it->FindMember("SkyColor")->value.GetString();
			int idx = StringUtil::ParseInt(it->FindMember("time")->value.GetString());

			m_lightSettings[idx].MainLightColor = StringUtil::ParseColor(mainColor);
			m_lightSettings[idx].SubLightColor = StringUtil::ParseColor(subColor);
			//float v = StringUtil::ParseFloat(clouldColor);
			//m_lightSettings[idx].clouldLight = Color(v, v, v, 1.f);
			//m_lightSettings[idx].skyColor = StringUtil::ParseColor255(skyColor);
			//m_lightSettings[idx].fogColor = StringUtil::ParseColor255(fogColor);
		}
	}

	LordDelete(doc);
	LordFree(jsonData);
	LordDelete(pJSONStream);
}

void RenderEntity::updateLightSetting(float rdt)
{
	WorldClient* pWorld = (WorldClient*)(bm->m_pWorld);
	int worldTime = pWorld->getWorldInfo().getWorldTime() % 24000;
	int index = worldTime / 1000;
	float ratio = (worldTime - index * 1000) / 1000.f;
	index += 8;
	if (index >= 24)
		index -= 24;
	float celestial = bm->getWorld()->getCelestialAngle(rdt);
	Quaternion rotateY(Vector3::UNIT_Y, -Math::PI_DIV3);
	Quaternion rotateX(Vector3::UNIT_X, celestial * Math::PI_2);
	Quaternion orientation = rotateY * rotateX;
	Vector3 lightDir = orientation * Vector3::UNIT_Y;
	lightDir.normalize();

	//Vector3 mainDir = m_lightSettings[index].MainLightDir + (m_lightSettings[(index + 1) % 24].MainLightDir - m_lightSettings[index].MainLightDir) * ratio;
	//mainDir.normalize();
	Color mainLight = m_lightSettings[index].MainLightColor + (m_lightSettings[(index + 1) % 24].MainLightColor - m_lightSettings[index].MainLightColor) * ratio;
	Color subLight = m_lightSettings[index].SubLightColor + (m_lightSettings[(index + 1) % 24].SubLightColor - m_lightSettings[index].SubLightColor) * ratio;
	//cloudColor = m_lightSettings[index].clouldLight + (m_lightSettings[(index + 1) % 24].clouldLight - m_lightSettings[index].clouldLight) * ratio;
	//fogColor = m_lightSettings[index].fogColor + (m_lightSettings[(index + 1) % 24].fogColor - m_lightSettings[index].fogColor) * ratio;
	//skyColor = m_lightSettings[index].skyColor + (m_lightSettings[(index + 1) % 24].skyColor - m_lightSettings[index].skyColor) * ratio;

	SceneManager::Instance()->setMainLightDir(lightDir);
	SceneManager::Instance()->setMainLightColor(mainLight);
	SceneManager::Instance()->setSubLightColor(subLight);
	//SceneManager::Instance()->setFogColor(fogColor);
}


void RenderEntity::updateLightmap(float rdt)
{
	static float stickCnt = 0;
	stickCnt += rdt;

	WorldClient* pWorld = (WorldClient*)(bm->m_pWorld);

	if (!pWorld)
		return;

	bool enableLightVersion = bm->m_pPlayer->isPotionActive(Potion::nightVision);
	float var16 = 0.0f;
	if(enableLightVersion)
		var16 = getNightVisionBrightness(bm->m_pPlayer, rdt);
	if (stickCnt > 25.f)
	{
		LordLogInfo("enableLightVersion: %s NightVisionBrightness: %f", enableLightVersion ? "true" : "false", var16);
		stickCnt = 0.f;
	}

	for (int i = 0; i < 256; ++i)
	{
		float var4 = pWorld->getSunBrightness(1.0F) * 0.95F + 0.05F;
		float var5 = pWorld->m_provider->lightBrightnessTable[i / 16] * var4;
		float var6 = pWorld->m_provider->lightBrightnessTable[i % 16] * (torchFlickerX * 0.1F + 1.5F);

		if (pWorld->m_lastLightningBolt > 0)
		{
			var5 = pWorld->m_provider->lightBrightnessTable[i / 16];
		}

		float var7 = var5 * (pWorld->getSunBrightness(1.0F) * 0.65F + 0.35F);
		float var8 = var5 * (pWorld->getSunBrightness(1.0F) * 0.65F + 0.35F);
		float var11 = var6 * ((var6 * 0.6F + 0.4F) * 0.6F + 0.4F);
		float var12 = var6 * (var6 * var6 * 0.6F + 0.4F);
		float var13 = var7 + var6;
		float var14 = var8 + var11;
		float var15 = var5 + var12;
		var13 = var13 * 0.96F + 0.03F;
		var14 = var14 * 0.96F + 0.03F;
		var15 = var15 * 0.96F + 0.03F;
		
		if (pWorld->m_provider->dimensionId == 1)
		{
			var13 = 0.22F + var6 * 0.75F;
			var14 = 0.28F + var11 * 0.75F;
			var15 = 0.25F + var12 * 0.75F;
		}

		float var17;
		if (enableLightVersion)
		{
			
			var17 = 1.0F / var13;

			if (var17 > 1.0F / var14)
			{
				var17 = 1.0F / var14;
			}

			if (var17 > 1.0F / var15)
			{
				var17 = 1.0F / var15;
			}

			var13 = var13 * (1.0F - var16) + var13 * var17 * var16;
			var14 = var14 * (1.0F - var16) + var14 * var17 * var16;
			var15 = var15 * (1.0F - var16) + var15 * var17 * var16;
		}

		if (var13 > 1.0F)
			var13 = 1.0F;
		if (var14 > 1.0F)
			var14 = 1.0F;
		if (var15 > 1.0F)
			var15 = 1.0F;

		var16 = bm->m_gameSettings->gammaSetting;
		var17 = 1.0F - var13;
		float var18 = 1.0F - var14;
		float var19 = 1.0F - var15;
		var17 = 1.0F - var17 * var17 * var17 * var17;
		var18 = 1.0F - var18 * var18 * var18 * var18;
		var19 = 1.0F - var19 * var19 * var19 * var19;
		var13 = var13 * (1.0F - var16) + var17 * var16;
		var14 = var14 * (1.0F - var16) + var18 * var16;
		var15 = var15 * (1.0F - var16) + var19 * var16;
		var13 = var13 * 0.96F + 0.03F;
		var14 = var14 * 0.96F + 0.03F;
		var15 = var15 * 0.96F + 0.03F;

		if (var13 > 1.0F)
			var13 = 1.0F;
		if (var14 > 1.0F)
			var14 = 1.0F;
		if (var15 > 1.0F)
			var15 = 1.0F;

		if (var13 < 0.0F)
			var13 = 0.0F;
		if (var14 < 0.0F)
			var14 = 0.0F;

		if (var15 < 0.0F)
			var15 = 0.0F;

		short var20 = 255;
		/*const Color& grading = lightmapColorGrading[i];
		var13 = var13 * grading.r * grading.a * 2.f;
		var14 = var14 * grading.g * grading.a * 2.f;
		var15 = var15 * grading.b * grading.a * 2.f;*/
		//var13 = var13 * grading.r ;
		//var14 = var14 * grading.g ;
		//var15 = var15 * grading.b ;
		int var21 = (int)(var13 * 255.0F);
		int var22 = (int)(var14 * 255.0F);
		int var23 = (int)(var15 * 255.0F);
		lightmapColors[i] = var20 << 24 | var21 << 16 | var22 << 8 | var23;
	}

	ERect rt(0, 0, 16, 16);
	lightmapTexture->updateSubTex2D(0, rt, lightmapColors);
	lightmapUpdateNeeded = false;
}


float RenderEntity::getNightVisionBrightness(EntityPlayer* pPlayer, float rdt)
{
	int duration = pPlayer->getActivePotionEffect(Potion::nightVision)->getDuration();
	return duration > 200 ? 1.0F : 0.7F + Math::Sin((duration - rdt) * Math::PI * 0.2F) * 0.3F;
}

void RenderEntity::addRainParticles()
{
	Blockman* pBm = Blockman::Instance();
	WorldClient* pWorld = (WorldClient*)(bm->m_pWorld);
	float rainStrength = pWorld->getRainStrength(1.0F);
	EffectRenderer* pER = EffectRenderer::Instance();

	if (!bm->m_gameSettings->fancyGraphics)
	{
		rainStrength /= 2.0F;
	}

	if (rainStrength != 0.0F)
	{
		random.setSeed((i64)rendererUpdateCount * 312987231L);
		EntityLivingBase* pPlayer = bm->renderViewEntity;
		BlockPos ipos = pPlayer->getPosition().getFloor();
		i8 range = 10;
		Vector3 soundPos = Vector3::ZERO;
		int rainseed = 0;
		int count = (int)(100.0F * rainStrength * rainStrength);

		if (pBm->m_gameSettings->particleSetting == 1)
		{
			count >>= 1;
		}
		else if (pBm->m_gameSettings->particleSetting == 2)
		{
			count = 0;
		}

		for (int i = 0; i < count; ++i)
		{
			int x = ipos.x + random.nextInt(range) - random.nextInt(range);
			int z = ipos.z + random.nextInt(range) - random.nextInt(range);
			int y = pWorld->getPrecipitationHeight(x, z);
			int id = pWorld->getBlockId(BlockPos(x, y - 1, z));
			BiomeGenBase* pGen = pWorld->getBiomeGenForCoords(x, z);

			if (y <= ipos.y + range && 
				y >= ipos.y - range &&
				pGen->canSpawnLightningBolt() &&
				pGen->getFloatTemperature() >= 0.2F)
			{
				float off_x = random.nextFloat();
				float off_z = random.nextFloat();

				if (id > 0)
				{
					Block* pBlock = BlockManager::sBlocks[id];
					if (pBlock->getMaterial() == BM_Material::BM_MAT_lava)
					{
						EntitySmokeFX* pfx = LordNew EntitySmokeFX(pWorld, Vector3(off_x + x, 0.1f + y - pBlock->getBlockBoundsMinY(), off_z + z), Vector3::ZERO);
						pER->addEffect(pfx);
					}
					else
					{
						++rainseed;

						if (random.nextInt(rainseed) == 0)
							soundPos = Vector3(off_x + x, y + 0.1f - pBlock->getBlockBoundsMinY(), z + off_z);
						
						EntityRainFX* pfx = LordNew EntityRainFX(pWorld, Vector3(off_x + x, y + 0.1f - pBlock->getBlockBoundsMinY(), off_z + z));
						pER->addEffect(pfx);
					}
				}
			}
		}

		if (rainseed > 0 && random.nextInt(3) < rainSoundCounter++)
		{
			rainSoundCounter = 0;

			BlockPos iipos = pPlayer->getPosition().getFloor();
			if (soundPos.y > pPlayer->getPosition().y + 1.f &&
				pWorld->getPrecipitationHeight(iipos.x, iipos.z) > iipos.y)
			{
				pWorld->playSoundByType(soundPos, ST_Rain);
			}
			else
			{
				pWorld->playSoundByType(soundPos, ST_Rain);
			}
		}
	}
}

void RenderEntity::renderRainSnow(float rdt)
{
	Blockman* pBm = Blockman::Instance();
	WorldClient* pWorld = (WorldClient*)(bm->m_pWorld);
	float strength = pWorld->getRainStrength(rdt);

	// debug.
	strength = 0.8f;

	if (strength <= 0.0F)
		return;

	if (rainXCoords == nullptr)
	{
		rainXCoords = (float*)LordMalloc(1024 * sizeof(float));
		rainYCoords = (float*)LordMalloc(1024 * sizeof(float));
		m_snowTex = TextureManager::Instance()->createTexture("snow_r.png");
		m_snowTex->load();
		m_rainTex = TextureManager::Instance()->createTexture("rain.png");
		m_rainTex->load();
		m_rainRenderer = TessManager::Instance()->createFxRenderable(m_rainTex, FRP_ALPHA_CHANEL);
		m_snowRenderer = TessManager::Instance()->createFxRenderable(m_snowTex, FRP_ALPHA_CHANEL);
		for (int i = 0; i < 32; ++i)
		{
			for (int j = 0; j < 32; ++j)
			{
				float fa = (float)(j - 16);
				float fb = (float)(i - 16);
				float fc = Math::Sqrt(fa * fa + fb * fb);
				rainXCoords[i << 5 | j] = -fb / fc;
				rainYCoords[i << 5 | j] = fa / fc;
			}
		}
	}

	EntityLivingBase* pPlayer = pBm->renderViewEntity;
	BlockPos ipos = pPlayer->getPosition().getFloor();
	Vector3 rpos = pPlayer->getLastTickPosition(rdt);
	//Vector3 rpos = pPlayer->lastTickPos + (pPlayer->position - pPlayer->lastTickPos) * rdt;
	int baseY = int(Math::Floor(rpos.y));
	i8 range = 5;

	if (pBm->m_gameSettings->fancyGraphics)
		range = 10;

	i8 iflag = -1;
	float fcount = rendererUpdateCount + rdt;

	if (pBm->m_gameSettings->fancyGraphics)
		range = 10;

	m_rainRenderer->beginPolygon();
	m_snowRenderer->beginPolygon();

	for (int z = ipos.z - range; z <= ipos.z + range; ++z)
	{
		for (int x = ipos.x - range; x <= ipos.x + range; ++x)
		{
			int idx = (z - ipos.z + 16) * 32 + x - ipos.x + 16;
			float ax = rainXCoords[idx] * 0.5F;
			float az = rainYCoords[idx] * 0.5F;
			BiomeGenBase* pBiome = pWorld->getBiomeGenForCoords(x, z);

			// for debug.
			// if (pBiome->canSpawnLightningBolt() || pBiome->getEnableSnow())
			{
				int height = pWorld->getPrecipitationHeight(x, z);
				int minh = ipos.y - range;
				int maxh = ipos.y + range;

				if (minh < height)
					minh = height;
				if (maxh < height)
					maxh = height;

				float vz = 1.0F;
				int vy = height;

				if (height < baseY)
					vy = baseY;

				if (minh != maxh)
				{
					random.setSeed((long)(x * x * 3121 + x * 45238971 ^ z * z * 418711 + z * 13761));
					float temperature = pBiome->getFloatTemperature();
					float fz;
					float fx;

					if (pWorld->getWorldChunkManager()->getTemperatureAtHeight(temperature, height) >= 1.15F) // rain.
					{
						FxVertexFmt* vertex;
						if (!m_rainRenderer->getLockPtr(1, vertex))
							continue;

						fz = ((float)(rendererUpdateCount + x * x * 3121 + x * 45238971 + z * z * 418711 + z * 13761 & 31) + rdt) / 32.0F * (3.0F + random.nextFloat());
						float cx = x + 0.5F - pPlayer->position.x;
						fx = z + 0.5F - pPlayer->position.z;
						float var37 = Math::Sqrt(cx * cx + fx * fx) / (float)range;
						int bright = pWorld->getLightBrightnessForSkyBlocks(BlockPos(x, vy, z), 0);
						int color = Color(1.f, 1.f, 1.f, ((1.0F - var37 * var37) * 0.5F + 0.5F) * strength).getARGB();
						vertex[0].set(x - ax + 0.5f - rpos.x, minh - rpos.y, z - az + 0.5f - rpos.z, color, bright, 0.0F * vz, minh * vz / 4.0F + fz * vz);
						vertex[1].set(x + ax + 0.5f - rpos.x, minh - rpos.y, z + az + 0.5f - rpos.z, color, bright, 1.0F * vz, minh * vz / 4.0F + fz * vz);
						vertex[2].set(x + ax + 0.5f - rpos.x, maxh - rpos.y, z + az + 0.5f - rpos.z, color, bright, 1.0F * vz, maxh * vz / 4.0F + fz * vz);
						vertex[3].set(x - ax + 0.5f - rpos.x, maxh - rpos.y, z - az + 0.5f - rpos.z, color, bright, 0.0F * vz, maxh * vz / 4.0F + fz * vz);
					}
					else // snow
					{
						FxVertexFmt* vertex;
						if (!m_snowRenderer->getLockPtr(1, vertex))
							continue;

						fz = ((float)(rendererUpdateCount & 511) + rdt) / 512.0F;
						float u = random.nextFloat() + fcount * 0.01F * (float)random.nextGaussian();
						float v = random.nextFloat() + fcount * (float)random.nextGaussian() * 0.001F;
						fx = x + 0.5F - pPlayer->position.x;
						float ez = z + 0.5F - pPlayer->position.z;
						float alpha = Math::Sqrt(fx * fx + ez * ez) / (float)range;
						int bright = (pWorld->getLightBrightnessForSkyBlocks(BlockPos(x, vy, z), 0) * 3 + 15728880) / 4;
						int color = Color(1.f, 1.f, 1.f, ((1.0F - alpha * alpha) * 0.3F + 0.5F) * strength).getARGB();
						vertex[0].set(x - ax + 0.5f - rpos.x, minh - rpos.y, z - az + 0.5f - rpos.z, color, bright, 0.0F * vz + u, minh * vz / 4.0F + fz * vz + v);
						vertex[1].set(x + ax + 0.5f - rpos.x, minh - rpos.y, z + az + 0.5f - rpos.z, color, bright, 1.0F * vz + u, minh * vz / 4.0F + fz * vz + v);
						vertex[2].set(x + ax + 0.5f - rpos.x, maxh - rpos.y, z + az + 0.5f - rpos.z, color, bright, 1.0F * vz + u, maxh * vz / 4.0F + fz * vz + v);
						vertex[3].set(x - ax + 0.5f - rpos.x, maxh - rpos.y, z - az + 0.5f - rpos.z, color, bright, 0.0F * vz + u, maxh * vz / 4.0F + fz * vz + v);
					}
				}
			}
		}
	}

	m_rainRenderer->endPolygon();
	m_snowRenderer->endPolygon();
	if (m_rainRenderer->getQuadNum() > 0)
		TessManager::Instance()->addFxRenderable(m_rainRenderer);
	if (m_snowRenderer->getQuadNum() > 0)
		TessManager::Instance()->addFxRenderable(m_snowRenderer);
}

void RenderEntity::updateEnchantTexMats(float rdt)
{
	Matrix4 matTex, mattemp;
	float s = 0.125f;
	matTex.identity(); matTex.scale(s, s, s);
	float t = (Blockman::Instance()->getSystemTime() % 3000) / 3000.f * 6.f;
	mattemp.identity(); mattemp.translate(t, 0.f, 0.f);
	matTex.joint(mattemp);
	mattemp.identity(); mattemp.rotateZ(-50 * Math::DEG2RAD);
	matTex.joint(mattemp);
	TessManager::Instance()->setEnchantTexMat(0, matTex);

	matTex.identity(); matTex.scale(s, s, s);
	t = (Blockman::Instance()->getSystemTime() % 4873) / 4873.f * 6.f;
	mattemp.identity(); mattemp.translate(-t, 0.f, 0.f);
	matTex.joint(mattemp);
	mattemp.identity(); mattemp.rotateZ(10 * Math::DEG2RAD);
	matTex.joint(mattemp);
	TessManager::Instance()->setEnchantTexMat(1, matTex);
}

int RenderEntity::getLightMapColor(float u, float v)
{
	u = Math::Clamp(u, 0.f, 1.f);
	v = Math::Clamp(v, 0.f, 1.f);
	return lightmapColors[int(v * 16) * 16 + int(u * 16)];
}

int RenderEntity::getLightMapColor(int skyBlockLight)
{
	Color bright(skyBlockLight);
	return getLightMapColor(bright.b, bright.r);
}


void RenderEntity::getMouseOver(float rdt)
{
	if (!bm->renderViewEntity)
		return;

	if (!bm->m_pWorld)
		return;

	clearMouseOver();
	if (m_isOnVehicle)
		return;

	float effectiveDistance = 0.f;
	auto player = bm->m_pPlayer;
	if (player && player->inventory && player->inventory->getCurrentItem())
	{
		if (auto item = dynamic_cast<ItemTool*>(player->inventory->getCurrentItem()->getItem()))
		{
			effectiveDistance = item->getEffectiveDistance();
		}

		if (auto item = dynamic_cast<ItemBuild*>(player->inventory->getCurrentItem()->getItem()))
		{
			effectiveDistance = 4.f;
		}
	}

	float reachDis = bm->m_playerControl->getBlockReachDistance()  + effectiveDistance;

	Vector3 viewerPos = bm->renderViewEntity->getPosition(rdt);
	Vector3 playerPos = bm->renderViewEntity->getPosition(rdt);

	GameSettings* pSetting = bm->m_gameSettings;
	if (pSetting->getPersonView() == GameSettings::SPVT_FIRST)
	{
		bm->objectMouseOver = bm->renderViewEntity->rayTrace(reachDis, rdt);
	}
	else if (pSetting->getPersonView() == GameSettings::SPVT_THIRD)
	{
		Camera* mainCamera = SceneManager::Instance()->getMainCamera();
		Ray ray;
		mainCamera->getCameraRay(ray, pSetting->getMousePos());
		
		Vector3 pos = ray.o;
		Vector3 lookat = ray.dir;
		Vector3 end = pos + lookat * reachDis * 2.0f;
		bm->objectMouseOver = bm->m_pWorld->rayTraceBlocks(pos, end);
		if (bm->objectMouseOver.result)
		{
			float mouseToPlayer = playerPos.distanceTo(bm->objectMouseOver.hitVec);
			if (mouseToPlayer > reachDis)
			{
				bm->objectMouseOver.reset();
			}
			else
			{
				RayTraceResult limitRange = bm->m_pWorld->rayTraceBlocks(playerPos, bm->objectMouseOver.hitVec);
				if (limitRange.result && mouseToPlayer > playerPos.distanceTo(limitRange.hitVec))
				{
					bm->objectMouseOver.reset();
				}
			}
		}
		viewerPos = pos;
	}
	else
		return;

	float reachDis2 = reachDis;

	if (bm->m_playerControl->extendedReach())
	{
		reachDis = 6.0f;
		reachDis2 = 6.0f;
	}
	else
	{
		if (reachDis > 5.0f)
		{
			reachDis2 = 5.0f;
		}

		reachDis = reachDis2;
	}

	if (bm->objectMouseOver.result)
	{
		reachDis2 = bm->objectMouseOver.hitVec.distanceTo(viewerPos);
	}

	Vector3 lookDir = bm->renderViewEntity->getLook(rdt);
	Vector3 lookAt = viewerPos + lookDir * reachDis;
	if (pSetting->getPersonView() == GameSettings::SPVT_THIRD)
	{
		lookAt += lookDir * reachDis;
	}
	EntityArr entities = bm->m_pWorld->getEntitiesWithinAABBExcludingEntity(bm->renderViewEntity, bm->renderViewEntity->boundingBox.addCoord(lookDir * reachDis).expand(1.f));
	float var11 = reachDis2;

	for (int i = 0; i < int(entities.size()); ++i)
	{
		Entity* pEntity = entities[i];

		float borderSize = pEntity->getCollisionBorderSize();
		Box aabb = pEntity->boundingBox.expand(borderSize);
		RayTraceResult var17 = RayTraceResult::calculateIntercept(aabb, viewerPos, lookAt);

		if (aabb.isPointInside(viewerPos))
		{
			if (0.0f <= var11)
			{
				pointedEntity = pEntity;
				var11 = 0.0f;
			}
		}
		else if (var17.result)
		{
			float var18 = playerPos.distanceTo(var17.hitVec);

			if (var18 <= var11)
			{
				RayTraceResult limitRange = bm->m_pWorld->rayTraceBlocks(playerPos, var17.hitVec);
				if (limitRange.result && var18 > playerPos.distanceTo(limitRange.hitVec))
				{
					continue;
				}

				if (pEntity == bm->renderViewEntity->ridingEntity)
				{
					if (var11 == 0.0f)
					{
						pointedEntity = pEntity;
					}
				}
				else
				{
					pointedEntity = pEntity;
					var11 = var18;
				}
			}
		}
	}

	if (LogicSetting::Instance()->isDisableSelectEntity() && dynamic_cast<EntityPlayer*>(pointedEntity) || dynamic_cast<EntityBird*>(pointedEntity))
	{
		return;
	}

	if (pointedEntity && (var11 < reachDis2 || bm->objectMouseOver.result))
	{
		bm->objectMouseOver.set(pointedEntity);

		if (pointedEntity->isClass(ENTITY_CLASS_LIVING_BASE))
		{
			bm->pointedEntityLiving = (EntityLivingBase*)pointedEntity;
		}
	}
}

void RenderEntity::updateCameraAndRender(float dt, float rdt)
{
	if (lightmapUpdateNeeded)
	{
		updateLightmap(rdt);
	}

	prevFrameTime = Root::Instance()->getCurrentTime();

	float dx, dy;
	if (!bm->m_gameSettings->isMouseMoving())
	{
		m_prevMousePos = Vector2::INVALID;
		dx = dy = 0;
	}
	else if (m_prevMousePos == Vector2::INVALID)
	{
		m_prevMousePos = bm->m_gameSettings->getMousePos();
		dx = dy = 0;
	}
	else
	{
		float sensitivity = bm->m_gameSettings->getMouseSensitivity() * 0.6F * (bm->m_gameSettings->getCameraSensitive() * 0.2F + 0.1F) + 0.2F;
		float mouseLen = sensitivity * sensitivity * sensitivity * 16.0F;
		auto curMousePos = bm->m_gameSettings->getMousePos();
		auto mouseDelta = curMousePos - m_prevMousePos;
		m_prevMousePos = curMousePos;
		dx = mouseDelta.x * mouseLen;
		dy = -mouseDelta.y * mouseLen;
	}

	int mode = 1;

	if (bm->m_gameSettings->invertMouse)
	{
		mode = -1;
	}

	if (bm->m_gameSettings->smoothCamera)
	{
		smoothCamYaw += dx;
		smoothCamPitch += dy;
		float dt2t = rdt - smoothCamPartialTicks;
		smoothCamPartialTicks = rdt;
		dx = smoothCamFilterX * dt2t;
		dy = smoothCamFilterY * dt2t;
		bm->m_pPlayer->setAngles(dx, dy * mode);
	}
	else
	{
		bm->m_pPlayer->setAngles(dx, dy * mode);
	}

	if (bm->m_pWorld)
	{
		int var18 = performanceToFps(bm->m_gameSettings->limitFramerate);
		if (bm->m_gameSettings->limitFramerate == 0)
		{
			renderWorld(dt, rdt, 0L);
		}
		else
		{
			renderWorld(dt, rdt, renderEndNanoTime + (long)(1000000000 / var18));
			renderEndNanoTime = Root::Instance()->getCurrentTime();
		}
	}
}

void RenderEntity::setupViewBobbing(float rdt, Matrix4& matView)
{
	if (m_isOnVehicle)
		return;

	if (bm->renderViewEntity->isClass(ENTITY_CLASS_PLAYER))
	{
		EntityPlayer* pPlayer = (EntityPlayer*)bm->renderViewEntity;
		float deltaDis = pPlayer->distanceWalkedModified - pPlayer->prevDistanceWalkedModified;
		float walkDis = -(pPlayer->distanceWalkedModified + deltaDis * rdt);
		float yaw = pPlayer->prevCameraYaw + (pPlayer->cameraYaw - pPlayer->prevCameraYaw) * rdt;
		float pitch = pPlayer->prevCameraPitch + (pPlayer->cameraPitch - pPlayer->prevCameraPitch) * rdt;
		float viewBobingX = Math::Sin(walkDis * Math::PI) * yaw * 0.5F;
		float viewBobingY = -Math::Abs(Math::Cos(walkDis * Math::PI) * yaw);
		matView.translate(viewBobingX, viewBobingY, 0.f);
		Matrix4 temp = Matrix4::IDENTITY;
		temp.rotateZ(Math::Sin(walkDis * Math::PI) * yaw * Math::DEG2RAD * 3.0F);
		matView.joint(temp);
		temp.identity(); temp.rotateX(Math::Abs(Math::Cos(walkDis * Math::PI - 0.2F) * yaw * Math::DEG2RAD) * 5.0F);
		matView.joint(temp);
		temp.identity(); temp.rotateX(pitch * Math::DEG2RAD);
		matView.joint(temp);
	}
}

void RenderEntity::orientCamera(float rdt, Matrix4& matView)
{
	EntityLivingBase* pViewer = bm->renderViewEntity;
	float offsetY = pViewer->yOffset - 1.62F;
	//Vector3 viewPos = pViewer->prevPos + (pViewer->position - pViewer->prevPos) * rdt;
	Vector3 viewPos = pViewer->getPosition(rdt);
	viewPos.y -= offsetY;
	
	Matrix4 temp;
	temp.identity(); temp.rotateZ((prevCamRoll + (camRoll - prevCamRoll) * rdt) * Math::DEG2RAD);
	matView = matView *  temp;
	float currentYaw = (pViewer->prevRotationYaw + (pViewer->rotationYaw - pViewer->prevRotationYaw) * rdt) * Math::DEG2RAD;
	float currentPitch = (pViewer->prevRotationPitch + (pViewer->rotationPitch - pViewer->prevRotationPitch) * rdt) * Math::DEG2RAD;
	
	Vector3 offset = Vector3::ZERO;


	if (bm->m_closeup_isShow)
	{
		float distance = bm->m_closeup_farDis;
		if (m_closeupDeltaDis < bm->m_closeup_farDis - bm->m_closeup_nearDis)
		{
			distance -= m_closeupDeltaDis;
			m_closeupDeltaDis += bm->m_closeup_velocity * Root::Instance()->getFrameTime() * 0.001f;
		}
		else
		{
			m_closeupDeltaDis = bm->m_closeup_farDis - bm->m_closeup_nearDis;
			distance = bm->m_closeup_nearDis;
		}
		currentYaw = bm->m_closeup_yaw * Math::DEG2RAD;
		currentPitch = bm->m_closeup_pitch * Math::DEG2RAD;
		float cx = Math::Sin(currentYaw) * Math::Cos(currentPitch);
		float cz = -Math::Cos(currentYaw) * Math::Cos(currentPitch);
		float cy = Math::Sin(currentPitch);
		offset = Vector3(cx, cy, cz) * distance;
	}
	else if (pViewer->isPlayerSleeping())
	{
		offsetY = offsetY + 1.0f;
		temp.identity(); temp.translate(Vector3(0.f, 0.3f, 0.f));
		matView = matView *  temp;

		if (!bm->m_gameSettings->debugCamEnable)
		{
			BlockPos pos = pViewer->getPosition().getFloor();
			int blockID = bm->m_pWorld->getBlockId(pos);

			if (blockID == BLOCK_ID_BED)
			{
				int meta = bm->m_pWorld->getBlockMeta(pos);
				int rot = meta & 3;
				temp.identity(); temp.rotateY(rot * Math::PI_DIV2);
				matView = matView * temp;
			}

			temp.identity(); temp.rotateY(-currentYaw - Math::PI);
			matView = matView * temp;
			temp.identity(); temp.rotateX(-currentPitch);
			matView = matView * temp;
		}
	}
	else if (bm->m_gameSettings->getPersonView() > GameSettings::SPVT_FIRST || m_isOnVehicle)
	{
		float distance = thirdPersonDistanceTemp + (thirdPersonDistance - thirdPersonDistanceTemp) * rdt;

		if (bm->m_gameSettings->debugCamEnable)
		{
			float debugYaw = (prevDebugCamYaw + (debugCamYaw - prevDebugCamYaw) * rdt) * Math::DEG2RAD;
			float debugPitch = (prevDebugCamPitch + (debugCamPitch - prevDebugCamPitch) * rdt) * Math::DEG2RAD;

			temp.identity(); temp.translate(Vector3(0.0F, 0.0F, -distance));
			matView = matView * temp;
			temp.identity(); temp.rotateX(debugPitch);
			matView = matView * temp;
			temp.identity(); temp.rotateY(debugYaw);
			matView = matView * temp;
		}
		else
		{
			float debugYaw = pViewer->rotationYaw * Math::DEG2RAD;
			float debugPitch = pViewer->rotationPitch * Math::DEG2RAD;

			if (bm->m_gameSettings->getPersonView() == GameSettings::SPVT_SECOND)
			{
				debugPitch += Math::PI;
			}

			float cx = Math::Sin(debugYaw) * Math::Cos(debugPitch);
			float cz = -Math::Cos(debugYaw) * Math::Cos(debugPitch);
			float cy = Math::Sin(debugPitch);

			float cdx = cx * distance;
			float cdy = cy * distance;
			float cdz = cz * distance;

			EntityPlayerSPMP* player = dynamic_cast<EntityPlayerSPMP*>(pViewer);
			if (player)
			{
				cdx += player->m_thirdPersonCameraOffset.x;
				cdy += player->m_thirdPersonCameraOffset.y;
				cdz += player->m_thirdPersonCameraOffset.z;
				Vector3 dir = Vector3(cdx, cdy, cdz);
				distance = dir.len();
				dir.normalize();
				cx = dir.x;
				cy = dir.y;
				cz = dir.z;
			}

			if (m_isPlayingEndingAnimation)
			{
				offset = Vector3(cx, cy, cz) * distance;
				if (m_endingAnimationRotationAngle < 180.0f)
				{
					m_endingAnimationRotationAngle += 0.36f * Root::Instance()->getFrameTime();
				}
				else
				{
					m_endingAnimationRotationAngle = 180.0f;
				}
				temp.identity(); temp.rotateAxisReplace(Vector3::UNIT_Y, -m_endingAnimationRotationAngle * Math::DEG2RAD);
				matView *= temp;
				float cosTemp = Math::Cos(m_endingAnimationRotationAngle * Math::DEG2RAD);
				float sinTemp = Math::Sin(m_endingAnimationRotationAngle * Math::DEG2RAD);
				float newX = offset.x * cosTemp - offset.z * sinTemp;
				float newZ = offset.x * sinTemp + offset.z * cosTemp;
				offset.x = newX;
				offset.z = newZ;
			}
			else
			{
				for (int i = 0; i < 8; ++i)
				{
					float ifx = (float)((i & 1) * 2 - 1);
					float ify = (float)((i >> 1 & 1) * 2 - 1);
					float ifz = (float)((i >> 2 & 1) * 2 - 1);
					ifx *= 0.15f;
					ify *= 0.15f;
					ifz *= 0.15f;
					RayTraceResult tracyResult = bm->m_pWorld->rayTraceBlocks(
						Vector3(viewPos.x + ifx, viewPos.y + ify, viewPos.z + ifz),
						Vector3(viewPos.x + cdx + ifx, viewPos.y + cdy + ify, viewPos.z + cdz + ifz));

					if (tracyResult.result)
					{
						float mindis = (tracyResult.hitVec - viewPos).len();

						if (mindis < distance)
						{
							distance = mindis;
						}
					}
				}

				offset = Vector3(cx, cy, cz) * distance;
			}

			if (bm->m_gameSettings->getPersonView() == GameSettings::SPVT_SECOND)
			{
				temp.identity(); temp.rotateY(Math::PI);
				matView *= temp;
			}
		}
	}
	else
	{
		temp.identity(); temp.translate(Vector3(0, 0, -0.1f));
		matView *= temp;
	}

	if (!bm->m_gameSettings->debugCamEnable)
	{
		temp.identity(); temp.rotateX(currentPitch);
		matView *= temp;
		temp.identity(); temp.rotateY(-currentYaw);
		matView *= temp;
	}

	temp.identity(); temp.translate(Vector3(0.0F, offsetY, 0.0F));
	matView *= temp;

	cameraOffset = offset;

	// var4 = pViewer.prevPosX + (pViewer.posX - pViewer.prevPosX) * (double)par1;
	// var6 = pViewer.prevPosY + (pViewer.posY - pViewer.prevPosY) * (double)par1 - (double)offsetY;
	// var8 = pViewer.prevPosZ + (pViewer.posZ - pViewer.prevPosZ) * (double)par1;
	// cloudFog = bm->m_globalRender->hasCloudFog(var4, var6, var8, par1);
}

void RenderEntity::setupCameraTransform(float rdt)
{
	if (bm->m_gameSettings->getPersonView() == 0)
	{
		bm->m_pPlayer->m_thirdPersonCameraOffset = Vector3::ZERO;
	}
	else
	{
		float horizontalOffset = 0.7f;
		float verticalOffset = 0.6f;
		float yaw = bm->m_pPlayer->rotationYaw + 90.0f;
		float offsetX = -horizontalOffset * Math::Sin(yaw * Math::DEG2RAD);
		float offsetZ = horizontalOffset * Math::Cos(yaw * Math::DEG2RAD);
		bm->m_pPlayer->m_thirdPersonCameraOffset.x = offsetX;
		bm->m_pPlayer->m_thirdPersonCameraOffset.y = verticalOffset;
		bm->m_pPlayer->m_thirdPersonCameraOffset.z = offsetZ;
	}

	farPlaneDistance = float(bm->m_gameSettings->getRenderRange());

	float fov = getFOVModifier(rdt, true);
	float fov2 = getFOVModifier(rdt, false);

	Matrix4 effectView = Matrix4::IDENTITY;
	hurtCemaraEffect(rdt, effectView);

	Matrix4 bobbingView = Matrix4::IDENTITY;
	if (bm->m_gameSettings->viewBobbing)
	{
		setupViewBobbing(rdt, bobbingView);
	}
	TessManager::Instance()->setViewBobing(bobbingView);
	float bobingTransRatal = 0.1f;
	bobbingView.m30 *= bobingTransRatal;
	bobbingView.m31 *= bobingTransRatal;
	bobbingView.m32 *= bobingTransRatal;

	Matrix4 orientView = Matrix4::IDENTITY;
	orientCamera(rdt, orientView);
	orientView = effectView * orientView * bobbingView;

	Vector3 cameraPos = Vector3::ZERO * orientView;
	Vector3 cameraDir = Vector3::UNIT_Z * orientView;
	Vector3 cameraUp = Vector3::UNIT_Y * orientView;

	Entity* pViewer = bm->renderViewEntity;
	//Vector3 viewPos = pViewer->prevPos + (pViewer->position - pViewer->prevPos) * rdt;
	Vector3 viewPos = pViewer->getPosition(rdt);

	if (bm->m_closeup_isShow)
	{
		viewPos = bm->m_closeup_closeupPos;
		if (Math::Abs(bm->m_closeup_farDis - bm->m_closeup_nearDis - m_closeupDeltaDis) < 0.00001f)
		{
			m_closeupDuration += Root::Instance()->getFrameTime() * 0.001f;
			if (m_closeupDuration >= bm->m_closeup_duration)
			{
				resetCloseup();
				bm->resetCloseup();
			}
		}
	}

	if (bm->m_pPlayer->m_isShooting)
	{
		bm->m_pPlayer->m_isShooting = false;
		m_screenShakingValue = 1.4f;
	}
	if (m_screenShakingValue > 0.0f)
	{
		fov += m_screenShakingValue;
		m_screenShakingValue -= Root::Instance()->getFrameTime() * 0.001f;
		if (m_screenShakingValue < 0.0f)
		{
			m_screenShakingValue = 0.0f;
		}
	}

	/** set effect system view pos (include the viewbobing.) */
	EntityFx::interpPos = viewPos + cameraPos + cameraOffset;

	Camera* pCamera = SceneManager::Instance()->getMainCamera();
	pCamera->setPosition(viewPos + cameraPos + cameraOffset);
	pCamera->setDirection(cameraDir.normalizedCopy());
	pCamera->setUp(cameraUp.normalizedCopy());
	pCamera->setFov(fov * Math::DEG2RAD);
	pCamera->setFov2(fov2 * Math::DEG2RAD);
	pCamera->update();
}

void RenderEntity::hurtCemaraEffect(float rdt, Matrix4& matView)
{
	if (m_isOnVehicle)
		return;

	EntityPlayer* pPlayer = (EntityPlayer*)bm->renderViewEntity;
	float hurtTime = (float)pPlayer->hurtTime - rdt;
	float deadTime;

	if (pPlayer->capabilities.isWatchMode)
		return;

	if (pPlayer->getHealth() <= 0.0F)
	{
		deadTime = (float)pPlayer->deathTime + rdt;
		matView.rotateZ((40.0F - 8000.0F / (deadTime + 200.0F)) * Math::DEG2RAD);
	}

	if (hurtTime >= 0.0F)
	{
		hurtTime /= (float)pPlayer->maxHurtTime;
		hurtTime = Math::Sin(hurtTime * hurtTime * hurtTime * hurtTime * Math::PI);
		deadTime = pPlayer->attackedAtYaw;
		Matrix4 temp;
		temp.identity(); temp.rotateY(-deadTime * Math::DEG2RAD);
		matView.joint(temp);
		temp.identity(); temp.rotateZ(-hurtTime * 14.f * Math::DEG2RAD);
		matView.joint(temp);
		temp.identity(); temp.rotateY(deadTime * Math::DEG2RAD);
		matView.joint(temp);
	}
}

float RenderEntity::getFOVModifier(float rdt, bool useSetting)
{
	if (debugViewDirection > 0)
		return 90.0F;

	EntityPlayer* pPlayer = (EntityPlayer*)bm->renderViewEntity;
	float fov = 70.0F;

	if (useSetting)
	{
		fov = 45.f + bm->m_gameSettings->getFovSetting() * 30.0F;
		fov *= fovModifierHandPrev + (fovModifierHand - fovModifierHandPrev) * rdt;
	}

	if (pPlayer->getHealth() <= 0.0F)
	{
		float deadTime = pPlayer->deathTime + rdt;
		fov /= (1.0F - 500.0F / (deadTime + 500.0F)) * 2.0F + 1.0F;
	}

	int blockID = getBlockIdAtEntityViewpoint(bm->m_pWorld, pPlayer, rdt);

	if (blockID != 0 && BlockManager::sBlocks[blockID]->getMaterial() == BM_Material::BM_MAT_water)
	{
		fov = fov * 60.0F / 70.0F;
	}

	return fov + prevDebugCamFOV + (debugCamFOV - prevDebugCamFOV) * rdt;
}

void RenderEntity::renderHand(float rdt)
{
	if (m_isOnVehicle)
		return;

	if (debugViewDirection > 0)
		return;
	
	const GameSettings& gameSetting = *(bm->m_gameSettings);
	// GL11.glMatrixMode(GL11.GL_PROJECTION);
	// GL11.glLoadIdentity();
	float var3 = 0.07F;

	if (cameraZoom != 1.0f)
	{
		// GL11.glTranslatef((float)cameraYaw, (float)(-cameraPitch), 0.0F);
		// GL11.glScaled(cameraZoom, cameraZoom, 1.0D);
	}

	// Project.gluPerspective(getFOVModifier(par1, false), (float)bm.displayWidth / (float)bm.displayHeight, 0.05F, farPlaneDistance * 2.0F);

	if (bm->m_playerControl->enableEverythingIsScrewedUpMode())
	{
		float var4 = 0.6666667F;
		//GL11.glScalef(1.0F, var4, 1.0F);
	}

	//GL11.glMatrixMode(GL11.GL_MODELVIEW);
	//GL11.glLoadIdentity();

	//GL11.glPushMatrix();
	//hurtCameraEffect(rdt);
	if (gameSetting.getPersonView() == GameSettings::SPVT_FIRST &&
		!bm->renderViewEntity->isPlayerSleeping() &&
		!gameSetting.hideGUI &&
		!bm->m_playerControl->enableEverythingIsScrewedUpMode() &&
		!Blockman::Instance()->m_closeup_isShow)
	{
		itemRenderer->renderItemInFirstPerson(rdt);
	}

	// GL11.glPopMatrix();

	if (gameSetting.getPersonView() == GameSettings::SPVT_FIRST &&
		!bm->renderViewEntity->isPlayerSleeping())
	{
		itemRenderer->renderOverlays(rdt);
	}
}

void RenderEntity::renderWorld(float dt, float rdt, i64 currentTime)
{
	setupCameraTransform(rdt);

	RenderGlobal* renderGlobal = bm->m_globalRender;
	EntityLivingBase* pViewer = bm->renderViewEntity;
	Vector3 expectPos = pViewer->getLastTickPosition(rdt);
	renderGlobal->updateChunks();
	renderGlobal->updateRenders(expectPos);
	//ChunkRebuilder::Instance()->flushTask(bm->m_pWorld->getChunkService());

	// begin async virtualTessThread
	Blockman::Instance()->m_renderBegin = LORD::Time::Instance()->getMicroseconds();
	Blockman::Instance()->beginAsync(renderGlobal);// m_visualTessThread->Update(renderGlobal);

	if (lightmapUpdateNeeded)
	{
		updateLightmap(rdt);
	}

	updateLightSetting(rdt);

	if (!bm->renderViewEntity)
	{
		bm->renderViewEntity = bm->m_pPlayer;
	}

	if (bm->m_pPlayer && (bm->m_pPlayer->isOnVehicle() || bm->m_pPlayer->isOnAircraft()))
		m_isOnVehicle = true;
	else
		m_isOnVehicle = false;

	updateEnchantTexMats(rdt);

	TessManager::Instance()->tick(dt);
	updateFogColor(rdt);
	setupFog(1, rdt);
	ActiveRenderInfo::updateRenderInfo(bm->m_pPlayer, bm->m_gameSettings->getPersonView() == GameSettings::SPVT_SECOND);
	// getMouseOver(par1);
	
	
	// EffectRenderer var6 = bm.effectRenderer;
	renderGlobal->beginFrame();
	

	// if (bm->m_gameSettings->renderDistance < 2)
	{
		renderGlobal->renderSky(rdt);
	}
	renderGlobal->renderSun(rdt);
	renderGlobal->renderGlow(rdt);
	renderGlobal->renderPoisonCircle(rdt, currentTime);
	{
		switch (GameClient::CGame::Instance()->GetGameType()) {
		case ClientGameType::Sand_Digger:
		case ClientGameType::Chicken:
			break;
		default:
			if (!LogicSetting::Instance()->isHideClouds())
			{
				renderGlobal->renderClouds(dt);
			}
			break;
		}
	}
	
	// renderGlobal->clipRenderersByFrustum(camera, dtime);
	// renderGlobal->updateRenderers(camera);
	

	// render all the entities.
	Camera* pCamera = SceneManager::Instance()->getMainCamera();
	renderGlobal->renderEntities(expectPos, pCamera->getFrustum(), rdt);

	// render all the effects.
	EffectRenderer* effectRM = EffectRenderer::Instance();
	effectRM->beginRender();
	effectRM->renderLitParticles(pViewer, rdt);
	effectRM->renderParticles(pViewer, rdt);
	effectRM->endRender();

	if (cameraZoom == 1.0f
		&& !bm->m_gameSettings->hideGUI
		&& bm->objectMouseOver.result
		&& !pViewer->isInsideOfMaterial(BM_Material::BM_MAT_water)
		&& TouchManager::Instance()->getSceneTouchCount() > 0)
	{
		renderGlobal->drawSelectionBox(bm->objectMouseOver, 0, rdt);
	}
	else
		renderGlobal->clearSelection();

	renderGlobal->drawBlockDamageTexture((EntityPlayer*)pViewer, rdt);
	// renderRainSnow(rdt);

	if (cameraZoom == 1.0f && !m_isOnVehicle)
	{
		renderHand(rdt);
	}

	EntityPlayerSPMP* player = Blockman::Instance()->m_pPlayer;
	if (player)
	{
		if (player->m_guideArrowsPos.size() > 0)
		{
			for (auto iter = player->m_guideArrowsPos.begin(); iter != player->m_guideArrowsPos.end(); iter++)
			{
				Vector3 posTemp = player->getLastTickPosition(rdt);
				posTemp.y -= 1.0f;
				if (player->m_guideArrowsEffect[iter->first])
				{
					player->m_guideArrowsEffect[iter->first]->setExtenalParam(posTemp, iter->second);
				}
			}
		}
	}

	WorldEffectManager::Instance()->renderSimpleEffects(Root::Instance()->getFrameTime(), bm->m_pPlayer->position);

	renderGlobal->endFrame();
}

int RenderEntity::performanceToFps(int par0)
{
	int var1 = 200;

	if (par0 == 1)
	{
		var1 = 120;
	}

	if (par0 == 2)
	{
		var1 = 35;
	}

	return var1;
}

int RenderEntity::getBlockIdAtEntityViewpoint(World* pWorld, EntityLivingBase* pLiving, float rdt)
{
	Vector3 pos = pLiving->prevPos + (pLiving->position - pLiving->prevPos) * rdt;
	pos.y += pLiving->getEyeHeight();

	pos += cameraOffset;
		
	BlockPos iPos = pos.getFloor();
	int blockID = pWorld->getBlockId(iPos);

	if (blockID != 0 && BlockManager::sBlocks[blockID]->getMaterial().isLiquid())
	{
		float var6 = BlockFluid::getFluidHeightPercent(pWorld->getBlockMeta(iPos)) - 0.1111111F;
		float var7 = (float)(pos.y + 1) - var6;

		if (pos.y >= var7)
		{
			blockID = pWorld->getBlockId(iPos.getPosY());
		}
	}

	return blockID;
}

void RenderEntity::updateFogColor(float rdt)
{
	// Vector3 lastPos = camera->getLastPostion();
	WorldClient* pWorld = (WorldClient*)(bm->m_pWorld);
	EntityLivingBase* pViewer = bm->renderViewEntity;
	float var4 = 1.0F / (float)(4 - bm->m_gameSettings->renderDistance);
	var4 = 1.0F - (float)Math::Pow((double)var4, 0.25);
	Color skyColor = pWorld->getSkyColor(rdt);
	float skyColor_R = skyColor.r;
	float skyColor_G = skyColor.g;
	float skyColor_B = skyColor.b;
	fogColor = pWorld->getFogColor(rdt);
	float var11;

	// if (bm->m_gameSettings->renderDistance < 2)
	{
		Vector3 var10 = Math::Sin_tab(pWorld->getCelestialAngleRadians(rdt)) > 0.0F ? Vector3::NEG_UNIT_X : Vector3::UNIT_X;

		var11 = pViewer->getLookVec().dot(var10);

		if (var11 < 0.0F)
		{
			var11 = 0.0F;
		}

		if (var11 > 0.0F)
		{
			float* var12 = pWorld->m_provider->calcSunriseSunsetColors(pWorld->getCelestialAngle(rdt), rdt);

			if (var12 != NULL)
			{
				var11 *= var12[3];
				fogColor.r = fogColor.r * (1.f - var11) + var12[0] * var11;
				fogColor.g = fogColor.g * (1.f - var11) + var12[1] * var11;
				fogColor.b = fogColor.b * (1.f - var11) + var12[2] * var11;
			}
		}
	}
	fogColor += (skyColor - fogColor) * var4;
	float var19 = pWorld->getRainStrength(rdt);
	float var20;

	if (var19 > 0.0F)
	{
		var11 = 1.0F - var19 * 0.5F;
		var20 = 1.0F - var19 * 0.4F;
		fogColor.r *= var11;
		fogColor.g *= var11;
		fogColor.b *= var20;
	}

	var11 = pWorld->getWeightedThunderStrength(rdt);

	if (var11 > 0.0F)
	{
		var20 = 1.0F - var11 * 0.5F;
		fogColor *= var20;
	}

	int blockID = getBlockIdAtEntityViewpoint(pWorld, pViewer, rdt);
	float var22 = 0.f;

	if (cloudFog)
	{
		fogColor = pWorld->getCloudColour(rdt);
	}
	else if (blockID != 0 && BlockManager::sBlocks[blockID]->getMaterial() == BM_Material::BM_MAT_water)
	{
		var22 = (float)EnchantmentHelper::getRespiration(pViewer) * 0.2F;
		fogColor.r = 0.02F + var22;
		fogColor.g = 0.02F + var22;
		fogColor.b = 0.2F + var22;
	}
	else if (blockID != 0 && BlockManager::sBlocks[blockID]->getMaterial() == BM_Material::BM_MAT_lava)
	{
		fogColor.r = 0.6F;
		fogColor.g = 0.1F;
		fogColor.b = 0.0F;
	}

	var22 = fogColor2 + (fogColor1 - fogColor2) * rdt;
	fogColor *= var22;
	return;
	double var14 = pViewer->getLastTickPosition(rdt).y * pWorld->m_provider->getVoidFogYFactor();

	if (var14 < 1.0)
	{
		if (var14 < 0.0)
		{
			var14 = 0.0;
		}

		var14 *= var14;
		fogColor *= (float)var14;
	}
}

void RenderEntity::setupFog(int fogMode, float rdt)
{
	EntityLivingBase* pViewer = bm->renderViewEntity;
	bool creativeMode = false;

	if (pViewer->isClass(ENTITY_CLASS_PLAYER))
	{
		creativeMode = ((EntityPlayer*)pViewer)->capabilities.isCreativeMode;
	}


	int blockID = getBlockIdAtEntityViewpoint(bm->m_pWorld, pViewer, rdt);
	farPlaneDistance = (float)(bm->m_gameSettings->getRenderRange()) / 2.f;

	float var6;
	int flogMode = 0;
	float fogDensity = 1.0f;
	float fog_start = 0.f;
	float fog_end = farPlaneDistance;

	if (cloudFog)
	{
		flogMode = 1;
		fogDensity = 0.f;
	}
	else if (blockID > 0 && BlockManager::sBlocks[blockID]->getMaterial() == BM_Material::BM_MAT_water)
	{
		flogMode = 1;
		fogDensity = 5.5f;
	}
	else if (blockID > 0 && BlockManager::sBlocks[blockID]->getMaterial() == BM_Material::BM_MAT_lava)
	{
		flogMode = 1;
		fogDensity = 2.0f;
	}
	else
	{
		var6 = farPlaneDistance;
		flogMode = 0;
		if (fogMode < 0)
		{
			fog_start = 0.f;
			fog_end = var6 * 0.8f;
		}
		else
		{
			fog_start = var6 * 0.8f;
			fog_end = var6;
		}
		if (bm->m_pWorld->m_provider->doesXZShowFog((int)pViewer->getPosition().x, (int)pViewer->getPosition().z))
		{
			fog_start = var6 * 0.05F;
			fog_end = Math::Min(var6, 192.f) * 0.5f;
		}
	}

	// TessManager::Instance()->setFogParam(fog_start, fog_end, fogDensity);
	// TessManager::Instance()->setFogColor(fogColor);
	SceneManager::Instance()->setFogParam(Vector3(fog_start, fog_end*1.5f, fogDensity));
	SceneManager::Instance()->setFogColor(fogColor);
}

void RenderEntity::clearMouseOver()
{
	RayTraceResult result;
	bm->pointedEntityLiving = NULL;
	pointedEntity = NULL;
	bm->objectMouseOver = result;
}

}
