#include "cBlock.h"
#include "cBlockManager.h"

#include "Render/TextureAtlas.h"
#include "Render/Colorizer.h"
#include "cItem/cItem.h"

/** header files from LogicModule. */
#include "Block/BlockManager.h"
#include "Item/ItemStack.h"
#include "Item/Item.h"
#include "Item/Items.h"
#include "Entity/Enchantment.h"
#include "Entity/EntityItem.h"
#include "Entity/EntityPlayer.h"
#include "World/IBlockAccess.h"
#include "World/World.h"
#include "World/RayTracyResult.h"
#include "WorldGenerator/BiomeGen.h"
#include "Stats/StatList.h"
#include "Util/Random.h"
#include "Util/ClientEvents.h"
#include "Audio/SoundDef.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "Setting/FruitsSetting.h"

namespace BLOCKMAN
{

//StepSound cBlock::soundPowderFootstep("powder", 1.0F, 1.0F);
//StepSound cBlock::soundWoodFootstep("wood", 1.0F, 1.0F);
//StepSound cBlock::soundGravelFootstep("gravel", 1.0F, 1.0F);
//StepSound cBlock::soundGrassFootstep("grass", 1.0F, 1.0F);
//StepSound cBlock::soundStoneFootstep("stone", 1.0F, 1.0F);
//StepSound cBlock::soundMetalFootstep("metal", 1.0F, 1.5F);
//StepSound cBlock::soundGlassFootstep("glass", 1.0F, 1.0F);
//StepSound cBlock::soundClothFootstep("cloth", 1.0F, 1.0F);
//StepSound cBlock::soundSandFootstep("sand", 1.0F, 1.0F);
//StepSound cBlock::soundSnowFootstep("snow", 1.0F, 1.0F);
//StepSound cBlock::soundLadderFootstep("ladder", 1.0F, 1.0F);
//StepSound cBlock::soundAnvilFootstep("anvil", 0.3F, 1.0F);

StepSound cBlock::soundPowderFootstep(ST_PlaceStoneBlock, ST_StepOnStone, ST_BreakStoneBlock);// û��powder,����stone
StepSound cBlock::soundWoodFootstep(ST_PlaceWoodBlock, ST_StepOnWood, ST_BreakWoodBlock);
StepSound cBlock::soundGravelFootstep(ST_PlaceGravelBlock, ST_StepOnGravel, ST_BreakGravelBlock);
StepSound cBlock::soundGrassFootstep(ST_PlaceGrassBlock, ST_StepOnGrass, ST_BreakGrassBlock);
StepSound cBlock::soundStoneFootstep(ST_PlaceStoneBlock, ST_StepOnStone, ST_BreakStoneBlock);
StepSound cBlock::soundMetalFootstep(ST_PlaceIronBlock, ST_StepOnStone, ST_BreakIronBlock); //û��metal,��iron
StepSound cBlock::soundGlassFootstep(ST_PlaceGlassBlock, ST_StepOnGlass, ST_BreakGlassBlock);
StepSound cBlock::soundClothFootstep(ST_PlaceClothBlock, ST_StepOnCloth, ST_BreakClothBlock);
StepSound cBlock::soundSandFootstep(ST_PlaceSandBlock, ST_StepOnSand, ST_BreakSandBlock);
StepSound cBlock::soundSnowFootstep(ST_PlaceSnowBlock, ST_StepOnSnow, ST_BreakSnowBlock);
StepSound cBlock::soundLadderFootstep(ST_PlaceLadderBlock, ST_StepOnLadder, ST_BreakLadderBlock);
StepSound cBlock::soundAnvilFootstep(ST_PlaceAnvilBlock, ST_StepOnAnvil, ST_BreakAnvil);


///////////////////////////////////////////////////////////////////////////////////// BlockClient
cBlock::cBlock(const String& atlas)
: m_spriteName(atlas)
, m_sprite(NULL)
, m_stepSound(&soundPowderFootstep)
{
}

cBlock::~cBlock()
{
}

AtlasSprite* cBlock::getIcon(int side, int meta)
{
	return m_sprite;
}

AtlasSprite* cBlock::getSpriteFromSide(int side)
{
	return getIcon(side, 0);
}

AtlasSprite* cBlock::getBlockSprite(IBlockAccess* piBlockAccess, const BlockPos& pos, int side)
{
	return getIcon(side, piBlockAccess->getBlockMeta(pos));
}


float cBlock::getBlockBrightness(IBlockAccess* pBlockAccess, const BlockPos& pos)
{
	int blockID = pBlockAccess->getBlockId(pos);
	int light = BlockManager::sLightValue[blockID];

	return pBlockAccess->getBrightness(pos, light);
}

int cBlock::getMixedBrightnessForBlock(IBlockAccess* piBlockAccess, const BlockPos& pos)
{
	int blockID = piBlockAccess->getBlockId(pos);
	int lightValue = BlockManager::sLightValue[blockID];

	return piBlockAccess->getLightBrightnessForSkyBlocks(pos, lightValue);
}

float cBlock::getAmbientOcclusionLightValue(IBlockAccess* pBlockAccess, const BlockPos& pos)
{
	return pBlockAccess->isBlockNormalCube(pos) ? 0.2f : 1.0f;
}

void cBlock::registerIcons(TextureAtlasRegister* atlas)
{
	m_sprite = atlas->getAtlasSprite(getSpriteName());
}

ui32 cBlock::colorFromBiome(COLORIZER_TYPE colorType, IBlockAccess* piBlockAccess, const BlockPos& pos)
{
	BiomeGenBase* pGen = piBlockAccess->getBiomeGenForCoords(pos.x, pos.z);
	
	if (!pGen)
		return 0;

	float temperature = pGen->getFloatTemperature();
	temperature = Math::Clamp(temperature, 0.f, 1.f);
	float rainfall = pGen->getFloatRainfall();
	rainfall = Math::Clamp(rainfall, 0.f, 1.f);

	int multype = pGen->getColorMultiplierType();
	if (multype == 0)
		return Colorizer::Instance()->getColor(colorType, temperature, rainfall);
	if (multype == 1)
		return ((Colorizer::Instance()->getColor(colorType, temperature, rainfall) & 0xFEFEFE) + 0x4E0E4E) / 2;
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////// BlockClient
BlockClient::BlockClient(int id, const BM_Material& mat, const String& name, const String& atlas)
: Block(id, mat, name)
, cBlock(atlas)
{}

///////////////////////////////////////////////////////////////////////////////////// BlockFlowerClient
cBlockFlower::cBlockFlower(const String& name)
: cBlock(name)
{}

///////////////////////////////////////////////////////////////////////////////////// cFlower
cFlower::cFlower(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockFlower(id, mat, name)
, cBlockFlower(atlas)
{}

///////////////////////////////////////////////////////////////////////////////////// BlockTallGrassClient
String cBlockTallGrass::grassTypes[grassTypeCount] = { String("deadbush"), String("tallgrass"), String("fern") };

cBlockTallGrass::cBlockTallGrass(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockTallGrass(id, mat, name)
, cBlockFlower(atlas)
{
	for (int i = 0; i < grassTypeCount; ++i)
		sprites[i] = NULL;
}

int cBlockTallGrass::getBlockColor()
{
	float u = 0.5f;
	float v = 1.0f;
	return Colorizer::Instance()->getColor(COLORIZER_TYPE_GRASS, u, v);
}

ui32 cBlockTallGrass::colorMultiplier(IBlockAccess* pBlockAccess, const BlockPos& pos)
{
	int meta = pBlockAccess->getBlockMeta(pos);
	if (meta == 0)
		return 0xFFFFFFFF;
	
	return colorFromBiome(COLORIZER_TYPE_GRASS, pBlockAccess, pos);
}

AtlasSprite* cBlockTallGrass::getIcon(int side, int meta)
{
	if (meta >= 3)
	{
		meta = 0;
	}

	return sprites[meta];
}

void cBlockTallGrass::registerIcons(TextureAtlasRegister* pAtlas)
{
	for (int i = 0; i < grassTypeCount; ++i)
	{
		sprites[i] = pAtlas->getAtlasSprite(grassTypes[i]);
	}
}

///////////////////////////////////////////////////////////////////////////////////// BlockDeadBushClient
cBlockDeadBush::cBlockDeadBush(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockDeadBush(id, mat, name)
, cBlockFlower(atlas)
{
}

///////////////////////////////////////////////////////////////////////////////////// BlockFluidClient
cBlockFluid::cBlockFluid(const String& atlas)
: cBlock(atlas)
{}

ui32 cBlockFluid::colorMultiplier(IBlockAccess* pBlockAccess, const BlockPos& pos)
{
	if (!isWater())
	{
		return 0xFFFFFFFF;
	}
	else
	{
		int r = 0;
		int g = 0;
		int b = 0;

		for (int z = -1; z <= 1; ++z)
		{
			for (int x = -1; x <= 1; ++x)
			{
				int color = pBlockAccess->getBiomeGenForCoords(pos.x + x, pos.z + z)->m_waterColor;
				r += (color & 0xFF0000) >> 16;
				g += (color & 0x00FF00) >> 8;
				b += color & 0x0000FF;
			}
		}

		return 0xFF000000 | (r / 9 & 255) << 16 | (g / 9 & 255) << 8 | b / 9 & 255;
	}
}

float cBlockFluid::getBlockBrightness(IBlockAccess* pBlockAccess, const BlockPos& pos)
{
	float brightness = pBlockAccess->getLightBrightness(pos);
	float brightness_up = pBlockAccess->getLightBrightness(pos.getPosY());
	return brightness > brightness_up ? brightness : brightness_up;
}

int cBlockFluid::getMixedBrightnessForBlock(IBlockAccess* pBlockAccess, const BlockPos& pos)
{
	int light = pBlockAccess->getLightBrightnessForSkyBlocks(pos, 0);
	int light_up = pBlockAccess->getLightBrightnessForSkyBlocks(pos.getPosY(), 0);
	int light_block = light & 255;
	int light_up_block = light_up & 255;
	int light_sky = (light >> 16) & 255;
	int light_up_sky = (light_up >> 16) & 255;
	return
		(light_block > light_up_block ? light_block : light_up_block) |
		((light_sky > light_up_sky ? light_sky : light_up_sky) << 16);
}

void cBlockFluid::randomDisplayTick(World* pWorld, const BlockPos& pos, Random& rand)
{
	if (isWater() && rand.nextInt(10) == 0)
	{
		int meta = pWorld->getBlockMeta(pos);
		if (meta <= 0 || meta >= 8)
		{
			pWorld->spawnParticle("suspended", Vector3(pos) + rand.nextVector(), Vector3::ZERO);
		}
	}

	if (isWater())// && rand.nextInt(64) == 0)
	{
		//int meta = pWorld->getBlockMeta(pos);
		//if (meta > 0 && meta < 8)
		//{
			//pWorld->playSound(Vector3(pos) + 0.5f, "liquid.water", rand.nextFloat() * 0.25F + 0.75F, rand.nextFloat() * 1.0F + 0.5F, false);
			if (pWorld->canPlaySpecialSound(0))
			{
				pWorld->playSoundByType(Vector3(pos) + 0.5f, ST_Water);
			}
		//}
	}

	if (!isWater() &&
		pWorld->getBlockMaterial(pos.getPosY()) == BM_Material::BM_MAT_air &&
		!pWorld->isBlockOpaqueCube(pos.getPosY()))
	{
		if (rand.nextInt(100) == 0)
		{
			float x = pos.x + rand.nextFloat();
			float y = pos.y + 0.8f;
			float z = pos.z + rand.nextFloat();
			pWorld->spawnParticle("lava", { x, y, z }, Vector3::ZERO);
			//pWorld->playSound({ x, y, z }, "liquid.lavapop", 0.2F + rand.nextFloat() * 0.2F, 0.9F + rand.nextFloat() * 0.15F, false);
			pWorld->playSoundByType({ x, y, z }, ST_LavaPop);
		}

		if (rand.nextInt(200) == 0)
		{
			//pWorld->playSound(pos, "liquid.lava", 0.2F + rand.nextFloat() * 0.2F, 0.9F + rand.nextFloat() * 0.15F, false);
			pWorld->playSoundByType(pos, ST_Lava);
		}
	}

	if (rand.nextInt(10) == 0 &&
		pWorld->doesBlockHaveSolidTopSurface(pos.getNegY()) &&
		!pWorld->getBlockMaterial(pos + BlockPos(0, -2, 0)).blocksMovement())
	{
		float x = pos.x + rand.nextFloat();
		float y = pos.y - 1.05f;
		float z = pos.z + rand.nextFloat();

		if (isWater())
		{
			pWorld->spawnParticle("dripWater", { x, y, z }, Vector3::ZERO);
		}
		else
		{
			pWorld->spawnParticle("dripLava", { x, y, z }, Vector3::ZERO);
		}
	}
}

int cBlockFluid::getRenderBlockPass()
{
	return isWater() ? TRP_TRANSPARENT_UV : TRP_SOLID_UV;
}

void cBlockFluid::registerIcons(TextureAtlasRegister* pAtlas)
{
	if (isWater())
	{
		sprites[0] = pAtlas->getAtlasSprite("water_still");
		sprites[1] = pAtlas->getAtlasSprite("water_flow");
	}
	else
	{
		sprites[0] = pAtlas->getAtlasSprite("lava_still");
		sprites[1] = pAtlas->getAtlasSprite("lava_flow");
	}
}

AtlasSprite* cBlockFluid::getIcon(int side, int meta)
{
	return side != 0 && side != 1 ? sprites[1] : sprites[0];
}

///////////////////////////////////////////////////////////////////////////////////// BlockStationaryClient
cBlockStationary::cBlockStationary(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockStationary(id, mat, name)
, cBlockFluid(atlas)
{
}

bool cBlockStationary::isWater()
{
	return m_blockMaterial == BM_Material::BM_MAT_water;
}

bool cBlockStationary::shouldSideBeRendered(IBlockAccess* pBlockAccess, const BlockPos& pos, int side)
{
	const BM_Material& mat = pBlockAccess->getBlockMaterial(pos);
	return mat == m_blockMaterial ? false : (side == 1 ? true : (mat == BM_Material::BM_MAT_ice ? false : Block::shouldSideBeRendered(pBlockAccess, pos, side)));
}

///////////////////////////////////////////////////////////////////////////////////// BlockFlowingClient
cBlockFlowing::cBlockFlowing(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockFlowing(id, mat, name)
, cBlockFluid(atlas)
{
}

bool cBlockFlowing::isWater()
{
	return m_blockMaterial == BM_Material::BM_MAT_water;
}

bool cBlockFlowing::shouldSideBeRendered(IBlockAccess* pBlockAccess, const BlockPos& pos, int side)
{
	const BM_Material& mat = pBlockAccess->getBlockMaterial(pos);
	return mat == m_blockMaterial ? false : (side == 1 ? true : (mat == BM_Material::BM_MAT_ice ? false : Block::shouldSideBeRendered(pBlockAccess, pos, side)));
}

///////////////////////////////////////////////////////////////////////////////////// BlockMushroomClient
cBlockMushroom::cBlockMushroom(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockMushroom(id, mat, name)
, cBlockFlower(atlas)
{
}

///////////////////////////////////////////////////////////////////////////////////// BlockCactusClient
cBlockCactus::cBlockCactus(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockCactus(id, mat, name)
, cBlock(atlas)
{
}

Box cBlockCactus::getSelectedBoundingBox(World* pWorld, const BlockPos& pos)
{
	float bound = 0.0625F;
	return Box(pos.x + bound, (float)pos.y, pos.z + bound, pos.x + 1 - bound, pos.y + 1 - bound, pos.z + 1 - bound);
}

AtlasSprite* cBlockCactus::getIcon(int side, int meta)
{
	return side == 1 ? cactusTopIcon : (side == 0 ? cactusBottomIcon : m_sprite);
}

void cBlockCactus::registerIcons(TextureAtlasRegister* pAtlas)
{
	m_sprite = pAtlas->getAtlasSprite(getSpriteName() + "_side");
	cactusTopIcon = pAtlas->getAtlasSprite(getSpriteName() + "_top");
	cactusBottomIcon = pAtlas->getAtlasSprite(getSpriteName() + "_bottom");
}

///////////////////////////////////////////////////////////////////////////////////// BlockRotatedPillarClient
cBlockRotatedPillar::cBlockRotatedPillar(const String& atlas)
: cBlock(atlas)
{
}

AtlasSprite* cBlockRotatedPillar::getIcon(int side, int meta)
{
	int var3 = meta & 12;
	int var4 = meta & 3;
	if (var3 == 0 && (side == 1 || side == 0))
	{
		return getSpriteTop(var4);
	}
	else if (var3 == 4 && (side == 5 || side == 4))
	{
		return getSpriteTop(var4);
	}
	else if (var3 == 8 && (side == 2 || side == 3))
	{
		return getSpriteTop(var4);
	}

	return getSpriteSide(var4);
}

///////////////////////////////////////////////////////////////////////////////////// BlockLogBaseClient
cBlockLogBase::cBlockLogBase(const String& atlas)
: cBlockRotatedPillar(atlas)
{
	for (int i = 0; i < 4; ++i)
	{
		wood_side[i] = NULL;
		wood_top[i] = NULL;
	}
}

void cBlockLogBase::registerIcons(TextureAtlasRegister* pAtlas)
{
	size_t i = 0;
	for (const auto& type : getWoodTypes())
	{
		wood_side[i] = pAtlas->getAtlasSprite(getSpriteName() + "_" + type);
		wood_top[i] = pAtlas->getAtlasSprite(getSpriteName() + "_" + type + "_top");
		++i;
		LordAssert(i <= 4);
	}
}

///////////////////////////////////////////////////////////////////////////////////// BlockHayClient

cBlockHay::cBlockHay(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockHay(id, mat, name)
, cBlockRotatedPillar(atlas)
{
}

void cBlockHay::registerIcons(TextureAtlasRegister* atlas)
{
	spriteTop = atlas->getAtlasSprite(getSpriteName() + "_top");
	m_sprite = atlas->getAtlasSprite(getSpriteName() + "_side");
}

///////////////////////////////////////////////////////////////////////////////////// BlockLeavesClient
String cBlockLeaves::leavesTypes[leavesTypeCount] = { "oak", "spruce", "birch", "jungle", "acacia", "big_oak" };
String cBlockLeaves::leavesSpriteNames[2][leavesTypeCount] = {
	{ "leaves_oak", "leaves_spruce", "leaves_birch", "leaves_jungle", "leaves_acacia", "leaves_big_oak" },
	{ "leaves_oak_opaque", "leaves_spruce_opaque", "leaves_birch_opaque", "leaves_jungle_opaque", "leaves_acacia_opaque", "leaves_big_oak_opaque" } };

cBlockLeaves::cBlockLeaves(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockLeaves(id, mat, name)
, cBlock(atlas)
{
	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < leavesTypeCount; ++j)
			sprites[i][j] = NULL;
	}
}

int cBlockLeaves::getRenderType() const
{
	return RENDER_TYPE_TRAPDOOR;
}

int cBlockLeaves::getBlockColor()
{
	float u = 0.5f;
	float v = 1.0f;
	return Colorizer::Instance()->getColor(COLORIZER_TYPE_FOLIAGE, u, v);
}

int cBlockLeaves::getRenderColor(int meta)
{
	if ((meta & 3) == 1)
		return 0x619961;
	else if ((meta & 3) == 2)
		return 0x80A755;
	else
		return 0x48B518;
}

ui32 cBlockLeaves::colorMultiplier(IBlockAccess* pBlockAccess, const BlockPos& pos)
{
	int meta = pBlockAccess->getBlockMeta(pos);

	if ((meta & 3) == 1)
	{
		return 0xFF619961;
	}
	else if ((meta & 3) == 2)
	{
		return 0xFF80A755;
	}
	else
	{
		int r = 0;
		int g = 0;
		int b = 0;

		for (int z = -1; z <= 1; ++z)
		{
			for (int x = -1; x <= 1; ++x)
			{
				int color = colorFromBiome(COLORIZER_TYPE_FOLIAGE, pBlockAccess, pos);
				r += (color & 0xFF0000) >> 16;
				g += (color & 0x00FF00) >> 8;
				b += (color & 0x0000FF) >> 0;
			}
		}

		return 0xFF000000 | (r / 9 & 255) << 16 | (g / 9 & 255) << 8 | b / 9 & 255;
	}
}

bool cBlockLeaves::shouldSideBeRendered(IBlockAccess* pBlockAccess, const BlockPos& pos, int side)
{
	int blockID = pBlockAccess->getBlockId(pos);
	return m_isOpaqueCube && blockID == m_blockID ? false : Block::shouldSideBeRendered(pBlockAccess, pos, side);
}

void cBlockLeaves::randomDisplayTick(World* pWorld, const BlockPos& pos, Random& rand)
{
	if (pWorld->canLightningStrikeAt(pos.getPosY()) &&
		!pWorld->doesBlockHaveSolidTopSurface(pos.getNegY()) &&
		rand.nextInt(15) == 1)
	{
		float x = pos.x + rand.nextFloat();
		float y = pos.y - 0.05f;
		float z = pos.z + rand.nextFloat();
		pWorld->spawnParticle("dripWater", Vector3(x, y, z), Vector3::ZERO);
	}
}

AtlasSprite* cBlockLeaves::getIcon(int side, int meta)
{
	int leavestype = meta & 3;
	return sprites[isOpaque][leavestype];
}

void cBlockLeaves::registerIcons(TextureAtlasRegister* pAtlas)
{
	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < leavesTypeCount; ++j)
		{
			sprites[i][j] = pAtlas->getAtlasSprite(leavesSpriteNames[i][j]);
		}
	}
}

void cBlockLeaves::setOpaque(bool opaque)
{
	m_isOpaqueCube = opaque;
	isOpaque = opaque ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////////// BlockSnowClient
cBlockSnow::cBlockSnow(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockSnow(id, mat, name)
, cBlock(atlas)
{
}

bool cBlockSnow::shouldSideBeRendered(IBlockAccess* pBlockAccess, const BlockPos& pos, int side)
{
	return side == 1 ? true : Block::shouldSideBeRendered(pBlockAccess, pos, side);
}

///////////////////////////////////////////////////////////////////////////////////// BlockSandClient
cBlockSand::cBlockSand(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockSand(id, mat, name)
, cBlock(atlas)
{}

///////////////////////////////////////////////////////////////////////////////////// BlockVineClient
cBlockVine::cBlockVine(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockVine(id, mat, name)
, cBlock(atlas)
{
}

ui32 cBlockVine::colorMultiplier(IBlockAccess* pBlockAccess, const BlockPos& pos)
{
	return colorFromBiome(COLORIZER_TYPE_FOLIAGE, pBlockAccess, pos);
}

///////////////////////////////////////////////////////////////////////////////////// BlockCocoaClient
cBlockCocoa::cBlockCocoa(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockCocoa(id, mat, name)
, cBlock(atlas)
{
}

void cBlockCocoa::registerIcons(TextureAtlasRegister* pAtlas)
{
	for (int i = 0; i < 3; ++i)
	{
		sprites[i] = pAtlas->getAtlasSprite(getSpriteName() + "_stage_" + StringUtil::ToString(i));
	}
}

AtlasSprite* cBlockCocoa::getItonByAge(int side)
{
	if (side < 0 || side >= 3)
	{
		side = 2;
	}

	return sprites[side];
}

///////////////////////////////////////////////////////////////////////////////////// BlockGrassClient
cBlockGrass::cBlockGrass(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockGrass(id, mat, name)
, cBlock(atlas)
, iconGrassTop(NULL)
, iconSnowSide(NULL)
, iconGrassSideOverlay(NULL)
{
}

AtlasSprite* cBlockGrass::getIcon(int side, int meta)
{
	if (side == 1)
		return iconGrassTop;
	else if (side == 0)
	{
		return cBlockManager::scBlocks[BLOCK_ID_DIRT]->getSpriteFromSide(side);
	}
	return m_sprite;
}

AtlasSprite* cBlockGrass::getBlockSprite(IBlockAccess* pBlockAccess, const BlockPos& pos, int side)
{
	if (side == 1)
	{
		return iconGrassTop;
	}
	else if (side == 0)
	{
		cBlock* pcblock = cBlockManager::scBlocks[BLOCK_ID_DIRT];
		return pcblock->getSpriteFromSide(side);
	}
	
	const BM_Material& var6 = pBlockAccess->getBlockMaterial(pos.getPosY());
	return var6 != BM_Material::BM_MAT_snow &&
		var6 != BM_Material::BM_MAT_craftedSnow ? m_sprite : iconSnowSide;
}

void cBlockGrass::registerIcons(TextureAtlasRegister* pAtlas)
{
	m_sprite = pAtlas->getAtlasSprite(getSpriteName() + "_side");
	iconGrassTop = pAtlas->getAtlasSprite(getSpriteName() + "_top");
	iconSnowSide = pAtlas->getAtlasSprite(getSpriteName() + "_side_snowed");
	iconGrassSideOverlay = pAtlas->getAtlasSprite(getSpriteName() + "_side_overlay");
}

int cBlockGrass::getBlockColor()
{
	float u = 0.5f;
	float v = 1.0f;
	return Colorizer::Instance()->getColor(COLORIZER_TYPE_GRASS, u, v);
}

ui32 cBlockGrass::colorMultiplier(IBlockAccess* pBlockAccess, const BlockPos& pos)
{
	int r = 0;
	int g = 0;
	int b = 0;

	for (int z = -1; z <= 1; ++z)
	{
		for (int x = -1; x <= 1; ++x)
		{
			BlockPos pos1(pos.x + x, pos.y, pos.z + z);
			int color = colorFromBiome(COLORIZER_TYPE_GRASS, pBlockAccess, pos1);
			r += (color & 0xFF0000) >> 16;
			g += (color & 0x00FF00) >> 8;
			b += (color & 0x0000FF);
		}
	}

	return  0xFF000000 | (r / 9 & 255) << 16 | (g / 9 & 255) << 8 | (b / 9 & 255);
}

AtlasSprite* cBlockGrass::getIconSideOverlay()
{
	cBlockGrass* grassblock = (cBlockGrass*)BlockManager::grass;
	return grassblock->iconGrassSideOverlay;
}
///////////////////////////////////////////////////////////////////////////////////// BlockWoodClient
String cBlockWood::woodType[woolTypeCount] = { "oak", "spruce", "birch", "jungle", "acacia", "big_oak" };

cBlockWood::cBlockWood(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockWood(id, mat, name)
, cBlock(atlas)
{
	for (int i = 0; i < woolTypeCount; ++i)
		sprites[i] = NULL;
}

AtlasSprite* cBlockWood::getIcon(int side, int meta)
{
	if (meta < 0 || meta >= woolTypeCount)
	{
		meta = 0;
	}

	return sprites[meta];
}

void cBlockWood::registerIcons(TextureAtlasRegister* pAtlas)
{
	for (int i = 0; i < woolTypeCount; ++i)
	{
		sprites[i] = pAtlas->getAtlasSprite(getSpriteName() + "_" + woodType[i]);
	}
}

///////////////////////////////////////////////////////////////////////////////////// BlockLilyPadClient
cBlockLilyPad::cBlockLilyPad(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockLilyPad(id, mat, name)
, cBlockFlower(atlas)
{
}

///////////////////////////////////////////////////////////////////////////////////// BlockReedClient
cBlockReed::cBlockReed(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockReed(id, mat, name)
, cBlock(atlas)
{
}

///////////////////////////////////////////////////////////////////////////////////// BlockSandStoneClient
String cBlockSandStone::SAND_TYPE_NAME[SAND_TYPE_COUNT] = { "normal", "carved", "smooth" };

cBlockSandStone::cBlockSandStone(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockSandStone(id, mat, name)
, cBlock(atlas)
{
}

AtlasSprite* cBlockSandStone::getIcon(int side, int meta)
{
	if (side != 1 && (side != 0 || meta != 1 && meta != 2))
	{
		if (side == 0)
		{
			return m_icon_bottom;
		}
		else
		{
			if (meta < 0 || meta >= SAND_TYPE_COUNT)
			{
				meta = 0;
			}

			return m_icon_side[meta];
		}
	}
	return m_icon_top;
}

void cBlockSandStone::registerIcons(TextureAtlasRegister* pAtlas)
{
	for (int i = 0; i < SAND_TYPE_COUNT; ++i)
	{
		m_icon_side[i] = pAtlas->getAtlasSprite(getSpriteName() + "_" + SAND_TYPE_NAME[i]);
	}

	m_icon_top = pAtlas->getAtlasSprite(getSpriteName() + "_top");
	m_icon_bottom = pAtlas->getAtlasSprite(getSpriteName() + "_bottom");
}

///////////////////////////////////////////////////////////////////////////////////// BlockMyceliumClient
cBlockMycelium::cBlockMycelium(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockMycelium(id, mat, name)
, cBlock(atlas)
{
}

AtlasSprite* cBlockMycelium::getIcon(int side, int meta)
{
	if (side == 1)
		return m_icon_top;
	else if (side == 0)
		return cBlockManager::scBlocks[BLOCK_ID_DIRT]->getSpriteFromSide(side);
	return m_sprite;
}

AtlasSprite* cBlockMycelium::getBlockSprite(IBlockAccess* piBlockAccess, const BlockPos& pos, int side)
{
	if (side == 1)
		return m_icon_top;
	else if (side == 0)
		return cBlockManager::scBlocks[BLOCK_ID_DIRT]->getSpriteFromSide(side);

	const BM_Material& mat = piBlockAccess->getBlockMaterial(pos.getPosY());
	return
		mat != BM_Material::BM_MAT_snow &&
		mat != BM_Material::BM_MAT_craftedSnow ? m_sprite : m_icon_side;
}

void cBlockMycelium::registerIcons(TextureAtlasRegister* atlas)
{
	m_sprite = atlas->getAtlasSprite(getSpriteName() + "_side");
	m_icon_top = atlas->getAtlasSprite(getSpriteName() + "_top");
	m_icon_side = atlas->getAtlasSprite("grass_side_snowed");
}

void cBlockMycelium::randomDisplayTick(World* pWorld, const BlockPos& pos, Random& rand)
{
	cBlock::randomDisplayTick(pWorld, pos, rand);

	if (rand.nextInt(10) == 0)
	{
		pWorld->spawnParticle("townaura", Vector3(pos) + Vector3(rand.nextFloat(), 1.f, rand.nextFloat()), Vector3::ZERO);
	}
}

///////////////////////////////////////////////////////////////////////////////////// BlockChestClient
cBlockChest::cBlockChest(int id, const BM_Material& mat, const String& name, const String& atlas, int chest_type)
: BlockChest(id, mat, name, chest_type)
, cBlock(atlas)
{
}

void cBlockChest::randomDisplayTick(World* pWorld, const BlockPos& pos, Random& rand)
{
	//Vector3 fpos = Vector3(pos) + Vector3(0.5f, 0.2f, 0.5f);

	//float rx = Math::UnitRandom() - 0.5f;
	//float rz = Math::UnitRandom() - 0.5f;

	//pWorld->spawnParticle("chestFx", fpos + Vector3(rx, 0.f, rz), Vector3((rx + 0.5f) * 0.1f, 0.1f, (rz + 0.5f)*0.1f));
}

void cBlockChest::registerIcons(TextureAtlasRegister* pAtlas)
{
	m_sprite = pAtlas->getAtlasSprite("planks_oak");
}

bool cBlockChest::onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit)
{
	IInventory* pInventory = getInventory(pWorld, pos);

	if (pInventory != NULL)
	{
		ChestActivateEvent::emit(pInventory, pos, face, hit);
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////////// BlockBedClient
cBlockBed::cBlockBed(int id, const String& name, const String& atlas)
: BlockBed(id, name)
, cBlock(atlas)
{
}

AtlasSprite* cBlockBed::getIcon(int side, int meta)
{
	if (side == 0)
	{
		return cBlockManager::scBlocks[BLOCK_ID_PLANKS]->getSpriteFromSide(side);
	}
	
	int dir = getDirection(meta);
	int beddir = bedDirection[dir][side];
	int bHead = isBlockHeadOfBed(meta) ? 1 : 0;
	return (bHead != 1 || beddir != 2) && (bHead != 0 || beddir != 3) ? (beddir != 5 && beddir != 4 ? bedTopIcons[bHead] : bedSideIcons[bHead]) : bedEndIcons[bHead];
}

void cBlockBed::registerIcons(TextureAtlasRegister* atlas)
{
	bedTopIcons[0] = atlas->getAtlasSprite(getSpriteName() + "_feet_top");
	bedTopIcons[1] = atlas->getAtlasSprite(getSpriteName() + "_head_top");

	bedEndIcons[0] = atlas->getAtlasSprite(getSpriteName() + "_feet_end");
	bedEndIcons[1] = atlas->getAtlasSprite(getSpriteName() + "_head_end");

	bedSideIcons[0] = atlas->getAtlasSprite(getSpriteName() + "_feet_side");
	bedSideIcons[1] = atlas->getAtlasSprite(getSpriteName() + "_head_side");
}

///////////////////////////////////////////////////////////////////////////////////// BlockSilverfishClient
cBlockSilverfish::cBlockSilverfish(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockSilverfish(id, mat, name)
, cBlock(atlas)
{
}

AtlasSprite* cBlockSilverfish::getIcon(int side, int meta)
{
	if (meta == 1)
	{
		return cBlockManager::scBlocks[BLOCK_ID_COBBLE_STONE]->getSpriteFromSide(meta);
	}
	else if (meta == 2)
	{
		return cBlockManager::scBlocks[BLOCK_ID_STONE_BRICK]->getSpriteFromSide(meta);
	}
	else
	{
		return cBlockManager::scBlocks[BLOCK_ID_STONE]->getSpriteFromSide(meta);
	}
}

///////////////////////////////////////////////////////////////////////////////////// BlockFarmlandClient
cBlockFarmland::cBlockFarmland(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockFarmland(id, mat, name)
, cBlock(atlas)
{
	m_spriteWet = NULL;
	m_spriteDry = NULL;
}

AtlasSprite* cBlockFarmland::getIcon(int side, int meta)
{
	return side == 1 ? (meta > 0 ? m_spriteWet : m_spriteDry) : cBlockManager::scBlocks[BLOCK_ID_DIRT]->getSpriteFromSide(side);
}

void cBlockFarmland::registerIcons(TextureAtlasRegister* atlas)
{
	m_spriteWet = atlas->getAtlasSprite(getSpriteName() + "_wet");
	m_spriteDry = atlas->getAtlasSprite(getSpriteName() + "_dry");
}
///////////////////////////////////////////////////////////////////////////////////// BlockDoorClient
cBlockDoor::cBlockDoor(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockDoor(id, mat, name)
, cBlock(atlas)
{
}

cBlockDoor::~cBlockDoor()
{
	LordSafeDelete(m_spriteLower[1]);
	LordSafeDelete(m_spriteUpper[1]);
}

AtlasSprite* cBlockDoor::getIcon(int side, int meta)
{
	return m_spriteLower[0];
}

void cBlockDoor::registerIcons(TextureAtlasRegister* atlas)
{
	m_spriteUpper[0] = atlas->getAtlasSprite(getSpriteName() + "_upper");
	m_spriteLower[0] = atlas->getAtlasSprite(getSpriteName() + "_lower");
	//m_spriteUpper[1] = atlas->getAtlasSprite(getSpriteName() + "_upper");
	//m_spriteLower[1] = atlas->getAtlasSprite(getSpriteName() + "_lower");
	m_spriteUpper[1] = LordNew AtlasSpriteFlipped(*m_spriteUpper[0], true, false);
	m_spriteLower[1] = LordNew AtlasSpriteFlipped(*m_spriteLower[0], true, false);
}

AtlasSprite* cBlockDoor::getBlockSprite(IBlockAccess* pBlockAccess, const BlockPos& pos, int face)
{
	if (face != 1 && face != 0)
	{
		int meta = getFullMetadata(pBlockAccess, pos);
		int dir = meta & 3;
		bool var8 = (meta & 4) != 0;
		bool isFlip = false;
		bool isUpper = (meta & 8) != 0;

		if (var8)
		{
			if (dir == 0 && face == 2)
			{
				isFlip = !isFlip;
			}
			else if (dir == 1 && face == 5)
			{
				isFlip = !isFlip;
			}
			else if (dir == 2 && face == 3)
			{
				isFlip = !isFlip;
			}
			else if (dir == 3 && face == 4)
			{
				isFlip = !isFlip;
			}
		}
		else
		{
			if (dir == 0 && face == 5)
			{
				isFlip = !isFlip;
			}
			else if (dir == 1 && face == 3)
			{
				isFlip = !isFlip;
			}
			else if (dir == 2 && face == 4)
			{
				isFlip = !isFlip;
			}
			else if (dir == 3 && face == 2)
			{
				isFlip = !isFlip;
			}

			if ((meta & 16) != 0)
			{
				isFlip = !isFlip;
			}
		}

		return isUpper ? m_spriteUpper[isFlip ? 1 : 0] : m_spriteLower[isFlip ? 1 : 0];
	}
	else
	{
		return m_spriteLower[0];
	}
}

bool cBlockDoor::checkDoor(World * pWorld, const BlockPos & pos, EntityPlayer * pPlayer)
{
	bool bRet = BlockDoor::checkDoor(pWorld, pos, pPlayer);

	if (!bRet)
	{
		String msg = LanguageManager::Instance()->getString(LanguageKey::DOOR_HAS_NO_KEY);
		ChatMessageReceiveEvent::emit(msg);
	}

	return bRet;
}

Box cBlockDoor::getSelectedBoundingBox(World* pWorld, const BlockPos& pos)
{
	setBlockBoundsBasedOnState(pWorld, pos);
	return Block::getSelectedBoundingBox(pWorld, pos);
}

bool cBlockDoor::onBlockActivated(World * pWorld, const BlockPos & pos, EntityPlayer * pPlayer, int face, const Vector3 & hit)
{
	bool bIsDoorOpen = isDoorOpen(pWorld, pos);
	if (bIsDoorOpen)
	{
		pWorld->playSoundByType(Vector3(pos) + 0.5f, ST_CloseDoor);
	}
	else
	{
		pWorld->playSoundByType(Vector3(pos) + 0.5f, ST_OpenDoor);
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////// BlockSnowBlockClient
cBlockSnowBlock::cBlockSnowBlock(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockSnowBlock(id, mat, name)
, cBlock(atlas)
{
}

///////////////////////////////////////////////////////////////////////////////////// BlockCakeClient
cBlockCake::cBlockCake(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockCake(id, mat, name)
, cBlock(atlas)
{}

Box cBlockCake::getSelectedBoundingBox(World* pWorld, const BlockPos& pos)
{
	int meta = pWorld->getBlockMeta(pos);
	float f1 = 0.0625F;
	float f2 = (float)(1 + meta * 2) / 16.0F;
	float f3 = 0.5F;
	return Box(pos.x + f2, (float)pos.y, pos.z + f1, pos.x + 1.f - f1, pos.y + f3, pos.z + 1.f - f1);

}

AtlasSprite* cBlockCake::getIcon(int side, int meta)
{
	return side == 1 ? m_spriteCakeTop :
		(side == 0 ? m_spriteCakeBottom :
		(meta > 0 && side == 4 ? m_spriteCakeInner : m_sprite));
}

void cBlockCake::registerIcons(TextureAtlasRegister* atlas)
{
	m_sprite = atlas->getAtlasSprite(getSpriteName() + "_side");
	m_spriteCakeInner = atlas->getAtlasSprite(getSpriteName() + "_inner");
	m_spriteCakeTop = atlas->getAtlasSprite(getSpriteName() + "_top");
	m_spriteCakeBottom = atlas->getAtlasSprite(getSpriteName() + "_bottom");
}

///////////////////////////////////////////////////////////////////////////////////// BlockTrapDoorClient
cBlockTrapDoor::cBlockTrapDoor(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockTrapDoor(id, mat, name)
, cBlock(atlas)
{
}

Box cBlockTrapDoor::getSelectedBoundingBox(World* pWorld, const BlockPos& pos)
{
	setBlockBoundsBasedOnState(pWorld, pos);
	return Block::getSelectedBoundingBox(pWorld, pos);
}

///////////////////////////////////////////////////////////////////////////////////// BlockStoneBrickClient
String cBlockStoneBrick::stoneTypes[stoneBrickTypeCount] = { "default", "mossy", "cracked", "chiseled" };
String cBlockStoneBrick::stonePostfix[stoneBrickTypeCount] = { "", "mossy", "cracked", "carved" };

cBlockStoneBrick::cBlockStoneBrick(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockStoneBrick(id, mat, name)
, cBlock(atlas)
{}

AtlasSprite* cBlockStoneBrick::getIcon(int side, int meta)
{
	if (meta < 0 || meta >= stoneBrickTypeCount)
	{
		meta = 0;
	}

	return m_sprites[meta];
}

void cBlockStoneBrick::registerIcons(TextureAtlasRegister* atlas)
{
	for (int i = 0; i < stoneBrickTypeCount; ++i)
	{
		String spriteName = getSpriteName();

		if (stonePostfix[i] != "")
		{
			spriteName = spriteName + "_" + stonePostfix[i];
		}

		m_sprites[i] = atlas->getAtlasSprite(spriteName);
	}
}

///////////////////////////////////////////////////////////////////////////////////// BlockMushroomCapClient
String cBlockMushroomCap::mushroomCapTypes[mushroomCapTypeCount] = { "skin_brown", "skin_red" };

cBlockMushroomCap::cBlockMushroomCap(int id, const BM_Material& mat, const String& name, const String& atlas, int mushroomType)
: BlockMushroomCap(id, mat, name, mushroomType)
, cBlock(atlas)
{
}

AtlasSprite* cBlockMushroomCap::getIcon(int face, int meta)
{
	return meta == 10 && face > 1 ? m_spriteSkinStem :
		(meta >= 1 && meta <= 9 && face == 1 ? m_sprites[m_mushroomType] :
		(meta >= 1 && meta <= 3 && face == 2 ? m_sprites[m_mushroomType] :
		(meta >= 7 && meta <= 9 && face == 3 ? m_sprites[m_mushroomType] :
		((meta == 1 || meta == 4 || meta == 7) && face == 4 ? m_sprites[m_mushroomType] :
		((meta == 3 || meta == 6 || meta == 9) && face == 5 ? m_sprites[m_mushroomType] :
		(meta == 14 ? m_sprites[m_mushroomType] :
		(meta == 15 ? m_spriteSkinStem : m_spriteInside)))))));
}

void cBlockMushroomCap::registerIcons(TextureAtlasRegister* atlas)
{
	for (int i = 0; i < mushroomCapTypeCount; ++i)
	{
		m_sprites[i] = atlas->getAtlasSprite(getSpriteName() + "_" + mushroomCapTypes[i]);
	}

	m_spriteInside = atlas->getAtlasSprite(getSpriteName() + "_" + "inside");
	m_spriteSkinStem = atlas->getAtlasSprite(getSpriteName() + "_" + "skin_stem");
}

///////////////////////////////////////////////////////////////////////////////////// BlockMelonClient
cBlockMelon::cBlockMelon(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockMelon(id, mat, name)
, cBlock(atlas)
{}

AtlasSprite* cBlockMelon::getIcon(int side, int meta)
{
	return side != 1 && side != 0 ? m_sprite : m_spriteTop;
}

void cBlockMelon::registerIcons(TextureAtlasRegister* atlas)
{
	m_sprite = atlas->getAtlasSprite(getSpriteName() + "_side");
	m_spriteTop = atlas->getAtlasSprite(getSpriteName() + "_top");
}

///////////////////////////////////////////////////////////////////////////////////// BlockOreClient
cBlockOre::cBlockOre(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockOre(id, mat, name)
, cBlock(atlas)
{
}

///////////////////////////////////////////////////////////////////////////////////// BlockOreStorageClient
cBlockOreStorage::cBlockOreStorage(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockOreStorage(id, mat, atlas)
, cBlock(atlas)
{
}

///////////////////////////////////////////////////////////////////////////////////// BlockPumpkinClient
cBlockPumpkin::cBlockPumpkin(int id, const BM_Material& mat, const String& name, const String& atlas, bool type)
: BlockPumpkin(id, mat, name, type)
, cBlock(atlas)
{}

AtlasSprite* cBlockPumpkin::getIcon(int side, int meta)
{
	return side == 1 ? m_spriteTop :
		(side == 0 ? m_spriteTop :
		(meta == 2 && side == 2 ? m_spriteFace :
		(meta == 3 && side == 5 ? m_spriteFace :
		(meta == 0 && side == 3 ? m_spriteFace :
		(meta == 1 && side == 4 ? m_spriteFace : m_sprite)))));
}

void cBlockPumpkin::registerIcons(TextureAtlasRegister* atlas)
{
	m_spriteFace = atlas->getAtlasSprite(getSpriteName() + "_face_" + (m_blockType ? "on" : "off"));
	m_spriteTop = atlas->getAtlasSprite(getSpriteName() + "_top");
	m_sprite = atlas->getAtlasSprite(getSpriteName() + "_side");
}

///////////////////////////////////////////////////////////////////////////////////// BlockCommandBlockClient
cBlockCommandBlock::cBlockCommandBlock(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockCommandBlock(id, mat, name)
, cBlock(atlas)
{
}

///////////////////////////////////////////////////////////////////////////////////// BlockDaylightDetectorClient
cBlockDaylightDetector::cBlockDaylightDetector(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockDaylightDetector(id, mat, name)
, cBlock(atlas)
{
}

AtlasSprite* cBlockDaylightDetector::getIcon(int side, int meta)
{
	return side == 1 ? m_sprites[0] : m_sprites[1];
}

void cBlockDaylightDetector::registerIcons(TextureAtlasRegister* atlas)
{
	m_sprites[0] = atlas->getAtlasSprite(getSpriteName() + "_top");
	m_sprites[1] = atlas->getAtlasSprite(getSpriteName() + "_side");
}

///////////////////////////////////////////////////////////////////////////////////// BlockDispenserClient
cBlockDispenser::cBlockDispenser(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockDispenser(id, mat, name)
, cBlock(atlas)
{

}

AtlasSprite* cBlockDispenser::getIcon(int side, int meta)
{
	int metamod = meta & 7;
	return side == metamod ? (metamod != 1 && metamod != 0 ? m_spriteFurnaceFrontHorizontal : m_spriteFurnaceFrontVertical) :
		(metamod != 1 && metamod != 0 ? (side != 1 && side != 0 ? m_sprite : m_spriteFurnaceTop) : m_spriteFurnaceTop);
}

void cBlockDispenser::registerIcons(TextureAtlasRegister* atlas)
{
	m_sprite = atlas->getAtlasSprite("furnace_side");
	m_spriteFurnaceTop = atlas->getAtlasSprite("furnace_top");
	m_spriteFurnaceFrontHorizontal = atlas->getAtlasSprite(getSpriteName() + "_front_horizontal");
	m_spriteFurnaceFrontVertical = atlas->getAtlasSprite(getSpriteName() + "_front_vertical");
}

///////////////////////////////////////////////////////////////////////////////////// BlockEnchantmentTableClient
cBlockEnchantmentTable::cBlockEnchantmentTable(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockEnchantmentTable(id, mat, name)
, cBlock(atlas)
{
}

AtlasSprite* cBlockEnchantmentTable::getIcon(int side, int meta)
{
	return side == 0 ? m_spriteBottom : (side == 1 ? m_spriteTop : m_sprite);
}

void cBlockEnchantmentTable::registerIcons(TextureAtlasRegister* atlas)
{
	m_sprite = atlas->getAtlasSprite(getSpriteName() + "_" + "side");
	m_spriteTop = atlas->getAtlasSprite(getSpriteName() + "_" + "top");
	m_spriteBottom = atlas->getAtlasSprite(getSpriteName() + "_" + "bottom");
}


void cBlockEnchantmentTable::randomDisplayTick(World* pWorld, const BlockPos& pos, Random& rand)
{
	cBlock::randomDisplayTick(pWorld, pos, rand);

	int range = 2;
	for (int ix = pos.x - range; ix <= pos.x + range; ++ix)
	{
		for (int iz = pos.z - range; iz <= pos.z + range; ++iz)
		{
			if (ix > pos.x - range && ix < pos.x + range && iz == pos.z - 1)
			{
				iz = pos.z + 2;
			}

			if (rand.nextInt(16) == 0)
			{
				for (int iy = pos.y; iy <= pos.y + 1; ++iy)
				{
					BlockPos pos1(ix, iy, iz);
					if (pWorld->getBlockId(pos1) == BLOCK_ID_BOOKSHELF)
					{
						BlockPos pos2 = (pos1 - pos) / 2 + pos;
						pos2.y = iy;
						if (!pWorld->isAirBlock(pos2))
						{
							break;
						}

						Vector3 spawn = Vector3(pos) + Vector3(0.5f, 2.0f, 0.5f);
						Vector3 vel;
						vel.x = ix - pos.x + rand.nextFloat() - 0.5f;
						vel.y = iy - pos.y - rand.nextFloat() - 1.0F;
						vel.z = iz - pos.z + rand.nextFloat() - 0.5F;
						pWorld->spawnParticle("enchantmenttable", spawn, vel);
					}
				}
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////// BlockFurnaceClient
cBlockFurnace::cBlockFurnace(int id, const BM_Material& mat, const String& name, const String& atlas, bool isActive)
: BlockFurnace(id, mat, name, isActive)
, cBlock(atlas)
{

}

AtlasSprite* cBlockFurnace::getIcon(int side, int meta)
{
	return side == 1 ? m_spriteFurnaceTop :
		(side == 0 ? m_spriteFurnaceTop :
		(side != meta ? m_sprite : m_spriteFurnaceFront));
}

void cBlockFurnace::registerIcons(TextureAtlasRegister* atlas)
{
	m_sprite = atlas->getAtlasSprite("furnace_side");
	m_spriteFurnaceFront = atlas->getAtlasSprite(m_isActive ? "furnace_front_on" : "furnace_front_off");
	m_spriteFurnaceTop = atlas->getAtlasSprite("furnace_top");
}

///////////////////////////////////////////////////////////////////////////////////// BlockJukeBoxClient
cBlockJukeBox::cBlockJukeBox(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockJukeBox(id, mat, name)
, cBlock(atlas)
{
}

AtlasSprite* cBlockJukeBox::getIcon(int side, int meta)
{
	return side == 1 ? m_sprite : m_spriteTop;
}

void cBlockJukeBox::registerIcons(TextureAtlasRegister* atlas)
{
	m_sprite = atlas->getAtlasSprite(getSpriteName() + "_side");
	m_spriteTop = atlas->getAtlasSprite(getSpriteName() + "_top");
}

///////////////////////////////////////////////////////////////////////////////////// BlockMobSpawnerClient
cBlockMobSpawner::cBlockMobSpawner(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockMobSpawner(id, mat, name)
, cBlock(atlas)
{
}

///////////////////////////////////////////////////////////////////////////////////// BlockNoteClient
cBlockNote::cBlockNote(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockNote(id, mat, name)
, cBlock(atlas)
{
}

///////////////////////////////////////////////////////////////////////////////////// BlockStairsClient
cBlockStairs::cBlockStairs(int id, const BM_Material& mat, const String& name, const String& atlas, Block* pBlock, int meta)
: BlockStairs(id, mat, name, pBlock, meta)
, cBlock(atlas)
{
	/** so far ,the cBlockManager::scBlocks is not initialize. use static_cast to assign the pBlockClient. */
	m_pBlockClient = dynamic_cast<cBlock*>(m_modelBlock);
	LordAssert(m_pBlockClient);
}

int cBlockStairs::getMixedBrightnessForBlock(IBlockAccess* pBlockAccess, const BlockPos& pos)
{
	return m_pBlockClient->getMixedBrightnessForBlock(pBlockAccess, pos);
}

float cBlockStairs::getBlockBrightness(IBlockAccess* pBlockAccess, const BlockPos& pos)
{
	return m_pBlockClient->getBlockBrightness(pBlockAccess, pos);
}

Box cBlockStairs::getSelectedBoundingBox(World* pWorld, const BlockPos& pos)
{
	return m_modelBlock->getSelectedBoundingBox(pWorld, pos);
}

AtlasSprite* cBlockStairs::getIcon(int side, int meta)
{
	return m_pBlockClient->getIcon(side, m_modelBlockMeta);
}

void cBlockStairs::randomDisplayTick(World* pWorld, const BlockPos& pos, Random& rand)
{
	m_pBlockClient->randomDisplayTick(pWorld, pos, rand);
}

int cBlockStairs::getRenderBlockPass()
{
	return m_pBlockClient->getRenderBlockPass();
}

///////////////////////////////////////////////////////////////////////////////////// BlockStepClient
String cBlockStep::stepTypes[stepTypeCount] = { "stone", "sand", "wood", "cobble", "brick", "smoothStoneBrick", "netherBrick", "quartz" };

cBlockStep::cBlockStep(int id, const BM_Material& mat, const String& name, const String& atlas, bool isDoubleSlab)
: BlockStep(id, mat, name, isDoubleSlab)
, cBlock(atlas)
{

}

AtlasSprite* cBlockStep::getIcon(int side, int meta)
{
	int type = meta & 7;

	if (m_isDoubleSlab && (meta & 8) != 0)
	{
		side = 1;
	}

	return type == 0 ? (side != 1 && side != 0 ? m_spriteSide : m_sprite) :
		(type == 1 ? cBlockManager::scBlocks[BLOCK_ID_SAND_STONE]->getSpriteFromSide(side) :
		(type == 2 ? cBlockManager::scBlocks[BLOCK_ID_PLANKS]->getSpriteFromSide(side) :
		(type == 3 ? cBlockManager::scBlocks[BLOCK_ID_COBBLE_STONE]->getSpriteFromSide(side) :
		(type == 4 ? cBlockManager::scBlocks[BLOCK_ID_BRICK]->getSpriteFromSide(side) :
		(type == 5 ? cBlockManager::scBlocks[BLOCK_ID_STAIRS_STONE_BRICK]->getIcon(side, 0) :
		(type == 6 ? cBlockManager::scBlocks[BLOCK_ID_NETHER_BRICK]->getSpriteFromSide(1) :
		(type == 7 ? cBlockManager::scBlocks[BLOCK_ID_BLOCK_NETHER_QUARTZ]->getSpriteFromSide(side) : m_sprite)))))));
}

void cBlockStep::registerIcons(TextureAtlasRegister* atlas)
{
	m_sprite = atlas->getAtlasSprite("stone_slab_top");
	m_spriteSide = atlas->getAtlasSprite("stone_slab_side");
}

bool cBlockStep::shouldSideBeRendered(IBlockAccess* pBlockAccess, const BlockPos& pos, int side)
{
	if (m_isDoubleSlab)
		return Block::shouldSideBeRendered(pBlockAccess, pos, side);
	if (side != 1 && side != 0 && !Block::shouldSideBeRendered(pBlockAccess, pos, side))
		return false;
	
	BlockPos offset = pos + offsetForSide[oppositeSide[side]];
	bool var9 = (pBlockAccess->getBlockMeta(offset) & 8) != 0;

	return var9 ? (side == 0 ? true :
		(side == 1 && Block::shouldSideBeRendered(pBlockAccess, pos, side) ? true : 
		!isBlockSingleSlab(pBlockAccess->getBlockId(pos)) || (pBlockAccess->getBlockMeta(pos) & 8) == 0)) :
		(side == 1 ? true : (side == 0 && Block::shouldSideBeRendered(pBlockAccess, pos, side) ? true :
		!isBlockSingleSlab(pBlockAccess->getBlockId(pos)) || (pBlockAccess->getBlockMeta(pos) & 8) != 0));
}

///////////////////////////////////////////////////////////////////////////////////// BlockWoodSlabClient
cBlockWoodSlab::cBlockWoodSlab(int id, const BM_Material& mat, const String& name, const String& atlas, bool isDoubleSlab)
: BlockWoodSlab(id, mat, name, isDoubleSlab)
, cBlock(atlas)
{
}

bool cBlockWoodSlab::shouldSideBeRendered(IBlockAccess* pBlockAccess, const BlockPos& pos, int side)
{
	if (m_isDoubleSlab)
		return Block::shouldSideBeRendered(pBlockAccess, pos, side);
	if (side != 1 && side != 0 && !Block::shouldSideBeRendered(pBlockAccess, pos, side))
		return false;

	BlockPos offset = pos + offsetForSide[oppositeSide[side]];
	bool var9 = (pBlockAccess->getBlockMeta(offset) & 8) != 0;

	return var9 ? (side == 0 ? true :
		(side == 1 && Block::shouldSideBeRendered(pBlockAccess, pos, side) ? true :
		!isBlockSingleSlab(pBlockAccess->getBlockId(pos)) || (pBlockAccess->getBlockMeta(pos) & 8) == 0)) :
		(side == 1 ? true : (side == 0 && Block::shouldSideBeRendered(pBlockAccess, pos, side) ? true :
		!isBlockSingleSlab(pBlockAccess->getBlockId(pos)) || (pBlockAccess->getBlockMeta(pos) & 8) != 0));
}

AtlasSprite* cBlockWoodSlab::getIcon(int side, int meta)
{
	return  cBlockManager::scBlocks[BLOCK_ID_PLANKS]->getIcon(side, meta & 7);
}

///////////////////////////////////////////////////////////////////////////////////// cBlockRedStoneSlab

cBlockRedStoneSlab::cBlockRedStoneSlab(int id, const BM_Material& mat, const String& name, const String& atlas, bool isDoubleSlab)
	: BlockRedStoneSlab(id, mat, name, isDoubleSlab)
	, cBlock(atlas)
{

}

AtlasSprite* cBlockRedStoneSlab::getIcon(int side, int meta)
{
	return side != 1 && side != 0 ? m_spriteSide : m_sprite;
}

void cBlockRedStoneSlab::registerIcons(TextureAtlasRegister* atlas)
{
	m_sprite = atlas->getAtlasSprite("red_sandstone_top");
	m_spriteSide = atlas->getAtlasSprite("red_sandstone_normal");
}

bool cBlockRedStoneSlab::shouldSideBeRendered(IBlockAccess* pBlockAccess, const BlockPos& pos, int side)
{
	if (m_isDoubleSlab)
		return Block::shouldSideBeRendered(pBlockAccess, pos, side);
	if (side != 1 && side != 0 && !Block::shouldSideBeRendered(pBlockAccess, pos, side))
		return false;

	BlockPos offset = pos + offsetForSide[oppositeSide[side]];
	bool var9 = (pBlockAccess->getBlockMeta(offset) & 8) != 0;

	return var9 ? (side == 0 ? true :
		(side == 1 && Block::shouldSideBeRendered(pBlockAccess, pos, side) ? true :
		!isBlockSingleSlab(pBlockAccess->getBlockId(pos)) || (pBlockAccess->getBlockMeta(pos) & 8) == 0)) :
		(side == 1 ? true : (side == 0 && Block::shouldSideBeRendered(pBlockAccess, pos, side) ? true :
		!isBlockSingleSlab(pBlockAccess->getBlockId(pos)) || (pBlockAccess->getBlockMeta(pos) & 8) != 0));
}

///////////////////////////////////////////////////////////////////////////////////// BlockCropsClient
cBlockCrops::cBlockCrops(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockCrops(id, mat, name)
, cBlockFlower(atlas)
{
}

AtlasSprite* cBlockCrops::getIcon(int side, int meta)
{
	if (meta < 0 || meta > 7)
	{
		meta = 7;
	}

	return m_sprites[meta];
}

void cBlockCrops::registerIcons(TextureAtlasRegister* atlas)
{
	for (int i = 0; i < 8; ++i)
	{
		m_sprites[i] = atlas->getAtlasSprite(getSpriteName() + "_stage_" + StringUtil::ToString(i));
	}
}

/////////////////////////////////////////////////////////////////////////////////////
cBlockCarrot::cBlockCarrot(int id, const BM_Material& mat, const String& name, const String& atlas)
: cBlockCrops(id, mat, name, atlas)
{
}

AtlasSprite* cBlockCarrot::getIcon(int side, int meta)
{
	if (meta < 7)
	{
		if (meta == 6)
		{
			meta = 5;
		}

		return m_sprites[meta >> 1];
	}
	else
	{
		return m_sprites[3];
	}
}

void cBlockCarrot::registerIcons(TextureAtlasRegister* atlas)
{
	for (int i = 0; i < 4; ++i)
	{
		m_sprites[i] = atlas->getAtlasSprite(getSpriteName() + "_stage_" + StringUtil::ToString(i));
	}
}

///////////////////////////////////////////////////////////////////////////////////// BlockPotatoClient
cBlockPotato::cBlockPotato(int id, const BM_Material& mat, const String& name, const String& atlas)
: cBlockCrops(id, mat, name, atlas)
{
}

AtlasSprite* cBlockPotato::getIcon(int side, int meta)
{
	if (meta < 7)
	{
		if (meta == 6)
		{
			meta = 5;
		}

		return m_sprites[meta >> 1];
	}
	else
	{
		return m_sprites[3];
	}
}

void cBlockPotato::registerIcons(TextureAtlasRegister* atlas)
{
	for (int i = 0; i < 4; ++i)
	{
		m_sprites[i] = atlas->getAtlasSprite(getSpriteName() + "_stage_" + StringUtil::ToString(i));
	}
}

///////////////////////////////////////////////////////////////////////////////////// BlockStemClient
cBlockStem::cBlockStem(int id, const BM_Material& mat, const String& name, const String& atlas, Block* fruit)
: BlockStem(id, mat, name, fruit)
, cBlockFlower(atlas)
{
}

AtlasSprite* cBlockStem::getIcon(int side, int meta)
{
	return side == 0 ? m_sprite : m_spriteConnected;
}

void cBlockStem::registerIcons(TextureAtlasRegister* atlas)
{
	m_sprite = atlas->getAtlasSprite(getSpriteName() + "_disconnected");
	m_spriteConnected = atlas->getAtlasSprite(getSpriteName() + "_connected");
}

ui32 cBlockStem::colorMultiplier(IBlockAccess* pBlockAccess, const BlockPos& pos)
{
	return getRenderColor(pBlockAccess->getBlockMeta(pos));
}

int cBlockStem::getRenderColor(int meta)
{
	int r = meta * 32;
	int g = 255 - meta * 8;
	int b = meta * 4;
	return r << 16 | g << 8 | b;
}

/////////////////////////////////////////////////////////////////////////////////////  BlockButtonWoodClient
cBlockButtonWood::cBlockButtonWood(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockButtonWood(id, mat, name)
, cBlock(atlas)
{
}

AtlasSprite* cBlockButtonWood::getIcon(int side, int meta)
{
	return  cBlockManager::scBlocks[BLOCK_ID_PLANKS]->getSpriteFromSide(1);
}

///////////////////////////////////////////////////////////////////////////////////// BlockButtonStoneClient
cBlockButtonStone::cBlockButtonStone(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockButtonStone(id, mat, name)
, cBlock(atlas)
{
}

AtlasSprite* cBlockButtonStone::getIcon(int side, int meta)
{
	return  cBlockManager::scBlocks[BLOCK_ID_STONE]->getSpriteFromSide(1);
}

///////////////////////////////////////////////////////////////////////////////////// BlockLadderClient
cBlockLadder::cBlockLadder(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockLadder(id, mat, name)
, cBlock(atlas)
{
}

Box cBlockLadder::getSelectedBoundingBox(World* pWorld, const BlockPos& pos)
{
	setBlockBoundsBasedOnState(pWorld, pos);
	return Block::getSelectedBoundingBox(pWorld, pos);
}

///////////////////////////////////////////////////////////////////////////////////// BlockFenceClient
cBlockFence::cBlockFence(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockFence(id, mat, name)
, cBlock(atlas)
{
}

///////////////////////////////////////////////////////////////////////////////////// BlockPaneClient
cBlockPane::cBlockPane(int id, const BM_Material& mat, const String& name, const String& atlas, const String& atlasTop, bool canDropItSelf)
: BlockPane(id, mat, name, canDropItSelf)
, cBlock(atlas)
, m_spriteTopName(atlasTop)
{
}

bool cBlockPane::shouldSideBeRendered(IBlockAccess *pBlockAccess, const BlockPos& pos, int side)
{
	int blockID = pBlockAccess->getBlockId(pos);
	return blockID == m_blockID ? false : Block::shouldSideBeRendered(pBlockAccess, pos, side);
}

void cBlockPane::registerIcons(TextureAtlasRegister* atlas)
{
	m_sprite = atlas->getAtlasSprite(getSpriteName());
	m_spriteTop = atlas->getAtlasSprite(m_spriteTopName);
}

///////////////////////////////////////////////////////////////////////////////////// BlockFenceGateClient
cBlockFenceGate::cBlockFenceGate(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockFenceGate(id, mat, name)
, cBlock(atlas)
{
}

AtlasSprite* cBlockFenceGate::getIcon(int side, int meta)
{
	return  cBlockManager::scBlocks[BLOCK_ID_PLANKS]->getSpriteFromSide(side);
}

///////////////////////////////////////////////////////////////////////////////////// BlockWallClient
cBlockWall::cBlockWall(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockWall(id, mat, name)
, cBlock(atlas)
{
}

bool cBlockWall::shouldSideBeRendered(IBlockAccess* pBlockAccess, const BlockPos& pos, int side)
{
	return side == 0 ? Block::shouldSideBeRendered(pBlockAccess, pos, side) : true;
}

AtlasSprite* cBlockWall::getIcon(int side, int meta)
{
	return meta == 1 ? cBlockManager::scBlocks[BLOCK_ID_COBBLE_STONE_MOSSY]->getSpriteFromSide(side):
		cBlockManager::scBlocks[BLOCK_ID_COBBLE_STONE]->getSpriteFromSide(side);
}

///////////////////////////////////////////////////////////////////////////////////// BlockGlassClient
cBlockGlass::cBlockGlass(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockGlass(id, mat, name)
, cBlock(atlas)
{
}

bool cBlockGlass::shouldSideBeRendered(IBlockAccess* pBlockAccess, const BlockPos& pos, int side)
{
	int blockID = pBlockAccess->getBlockId(pos);
	return !m_flag && blockID == m_blockID ? false : Block::shouldSideBeRendered(pBlockAccess, pos, side);
}

cBlockAirWall::cBlockAirWall(int id, const BM_Material& mat, const String& name, const String& atlas)
	: BlockAirWall(id, mat, name)
	, cBlock(atlas)
{
}

///////////////////////////////////////////////////////////////////////////////////// BlockIceClient
cBlockIce::cBlockIce(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockIce(id, mat, name)
, cBlock(atlas)
{
}

bool cBlockIce::shouldSideBeRendered(IBlockAccess* pBlockAccess, const BlockPos& pos, int side)
{
	int blockID = pBlockAccess->getBlockId(pos);
	return !m_flag && blockID == m_blockID ? false : Block::shouldSideBeRendered(pBlockAccess, pos, 1 - side);
}

///////////////////////////////////////////////////////////////////////////////////// BlockPackedIceClient
cBlockPackedIce::cBlockPackedIce(int id, const BM_Material & mat, const String & name, const String & atlas)
	: BlockPackedIce(id, mat, name)
	, cBlock(atlas)
{
}

///////////////////////////////////////////////////////////////////////////////////// BlockPortalClient
cBlockPortal::cBlockPortal(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockPortal(id, mat, name)
, cBlock(atlas)
{
}

bool cBlockPortal::shouldSideBeRendered(IBlockAccess* pBlockAccess, const BlockPos& pos, int side)
{
	if (pBlockAccess->getBlockId(pos) == m_blockID)
	{
		return false;
	}
	else
	{
		bool var6 = pBlockAccess->getBlockId(pos.getNegX()) == m_blockID && pBlockAccess->getBlockId(pos.getNegX2()) != m_blockID;
		bool var7 = pBlockAccess->getBlockId(pos.getPosX()) == m_blockID && pBlockAccess->getBlockId(pos.getPosX2()) != m_blockID;
		bool var8 = pBlockAccess->getBlockId(pos.getNegZ()) == m_blockID && pBlockAccess->getBlockId(pos.getNegZ2()) != m_blockID;
		bool var9 = pBlockAccess->getBlockId(pos.getPosZ()) == m_blockID && pBlockAccess->getBlockId(pos.getPosZ2()) != m_blockID;
		bool var10 = var6 || var7;
		bool var11 = var8 || var9;
		return var10 && side == 4 ? true : (var10 && side == 5 ? true : (var11 && side == 2 ? true : var11 && side == 3));
	}
}

void cBlockPortal::randomDisplayTick(World* pWorld, const BlockPos& pos, Random& rand)
{
	if (rand.nextInt(100) == 0)
	{
		//pWorld->playSound(Vector3(pos) + 0.5f, "portal.portal", 0.5F, rand.nextFloat() * 0.4F + 0.8F, false);
		pWorld->playSoundByType(Vector3(pos) + 0.5f, ST_Portal);
	}

	for (int i = 0; i < 4; ++i)
	{
		Vector3 loc = Vector3(pos) + rand.nextVector();
		int ext = rand.nextInt(2) * 2 - 1;
		Vector3 vec = (rand.nextVector() - Vector3::HALF) * 0.5f;

		if (pWorld->getBlockId(pos.getNegX()) != m_blockID &&
			pWorld->getBlockId(pos.getPosX()) != m_blockID)
		{
			loc.x = pos.x + 0.5f + 0.25f * ext;
			vec.x = rand.nextFloat() * 2.0F * ext;
		}
		else
		{
			loc.z = pos.z + 0.5f + 0.25f * ext;
			vec.z = rand.nextFloat() * 2.0F * ext;
		}

		pWorld->spawnParticle("portal", loc, vec);
	}
}

///////////////////////////////////////////////////////////////////////////////////// BlockQuartzClient
String cBlockQuartz::quartzTextureTypes[quartzTypeCount] = { "side", "chiseled", "lines", "", "" };

cBlockQuartz::cBlockQuartz(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockQuartz(id, mat, name)
, cBlock(atlas)
{
}

void cBlockQuartz::registerIcons(TextureAtlasRegister* atlas)
{
	for (int i = 0; i < quartzTypeCount; ++i)
	{
		if (quartzTextureTypes[i] == "")
		{
			quartzblockIcons[i] = quartzblockIcons[i - 1];
		}
		else
		{
			quartzblockIcons[i] = atlas->getAtlasSprite(getSpriteName() + "_" + quartzTextureTypes[i]);
		}
	}

	quartzblock_top = atlas->getAtlasSprite(getSpriteName() + "_" + "top");
	quartzblock_chiseled_top = atlas->getAtlasSprite(getSpriteName() + "_" + "chiseled_top");
	quartzblock_lines_top = atlas->getAtlasSprite(getSpriteName() + "_" + "lines_top");
	quartzblock_bottom = atlas->getAtlasSprite(getSpriteName() + "_" + "bottom");
}

AtlasSprite* cBlockQuartz::getIcon(int side, int meta)
{
	if (meta != 2 && meta != 3 && meta != 4)
	{
		if (side != 1 && (side != 0 || meta != 1))
		{
			if (side == 0)
			{
				return quartzblock_bottom;
			}
			else
			{
				if (meta < 0 || meta >= quartzTypeCount)
				{
					meta = 0;
				}

				return quartzblockIcons[meta];
			}
		}
		else
		{
			return meta == 1 ? quartzblock_chiseled_top : quartzblock_top;
		}
	}
	else
	{
		return meta == 2 && (side == 1 || side == 0) ? quartzblock_lines_top :
			(meta == 3 && (side == 5 || side == 4) ? quartzblock_lines_top :
			(meta == 4 && (side == 2 || side == 3) ? quartzblock_lines_top : quartzblockIcons[meta]));
	}
}

///////////////////////////////////////////////////////////////////////////////////// BlockBeaconClient
cBlockBeacon::cBlockBeacon(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockBeacon(id, mat, name)
, cBlock(atlas)
{
}

///////////////////////////////////////////////////////////////////////////////////// BlockAnvilClient
String cBlockAnvil::statuses[anvilTypeCount] = { "intact", "slightlyDamaged", "veryDamaged" };
String cBlockAnvil::anvilSpriteNames[anvilTypeCount] = { "anvil_top_damaged_0", "anvil_top_damaged_1", "anvil_top_damaged_2" };


cBlockAnvil::cBlockAnvil(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockAnvil(id, mat, name)
, cBlock(atlas)
, m_anvilRenderState(0)
{
}

void cBlockAnvil::registerIcons(TextureAtlasRegister* atlas)
{
	m_sprite = atlas->getAtlasSprite("anvil_base");
	for (int i = 0; i < anvilTypeCount; ++i)
	{
		m_sprites[i] = atlas->getAtlasSprite(anvilSpriteNames[i]);
	}
}

AtlasSprite* cBlockAnvil::getIcon(int side, int meta)
{
	if (m_anvilRenderState == 3 && side == 1)
	{
		int index = (meta >> 2) % anvilTypeCount;
		return m_sprites[index];
	}
	return m_sprite;
}

///////////////////////////////////////////////////////////////////////////////////// BlockPoweredOreClient
cBlockPoweredOre::cBlockPoweredOre(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockPoweredOre(id, mat, name)
, cBlock(atlas)
{
}

///////////////////////////////////////////////////////////////////////////////////// BlockFlowerPotClient
cBlockFlowerPot::cBlockFlowerPot(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockFlowerPot(id, mat, name)
, cBlock(atlas)
{
}

///////////////////////////////////////////////////////////////////////////////////// BlockSaplingClient
cBlockSapling::cBlockSapling(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockSapling(id, mat, name)
, cBlockFlower(atlas)
{
}

void cBlockSapling::registerIcons(TextureAtlasRegister* atlas)
{
	for (int i = 0; i < saplingTypeCount; ++i)
	{
		m_sprites[i] = atlas->getAtlasSprite(getSpriteName() + "_" + WOOD_TYPES[i]);
	}
}

AtlasSprite* cBlockSapling::getIcon(int side, int meta)
{
	return m_sprites[meta & 3];
}


///////////////////////////////////////////////////////////////////////////////////// BlockRedstoneLightClient
cBlockRedstoneLight::cBlockRedstoneLight(int id, const BM_Material& mat, const String& name, const String& atlas, bool powered)
: BlockRedstoneLight(id, mat, name, powered)
, cBlock(atlas)
{
}

///////////////////////////////////////////////////////////////////////////////////// BlockColoredClient
cBlockColored::cBlockColored(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockColored(id, mat, name)
, cBlock(atlas)
{
}

void cBlockColored::registerIcons(TextureAtlasRegister* atlas)
{
	for (int i = 0; i < colorType; ++i)
	{
		m_sprites[i] = atlas->getAtlasSprite(getSpriteName() + "_" + cItemDye::dyeSpriteNames[getDyeFromBlock(i)]);
	}
}

AtlasSprite* cBlockColored::getIcon(int side, int meta)
{
	return m_sprites[meta % colorType];
}

///////////////////////////////////////////////////////////////////////////////////// BlockCarpetClient
cBlockCarpet::cBlockCarpet(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockCarpet(id, mat, name)
, cBlock(atlas)
{
}

bool cBlockCarpet::shouldSideBeRendered(IBlockAccess* pBlockAccess, const BlockPos& pos, int side)
{
	return side == 1 ? true : Block::shouldSideBeRendered(pBlockAccess, pos, side);
}

AtlasSprite* cBlockCarpet::getIcon(int side, int meta)
{
	return  cBlockManager::scBlocks[BLOCK_ID_CLOTH]->getIcon(side, meta);
}

///////////////////////////////////////////////////////////////////////////////////// BlockWebClient
cBlockWeb::cBlockWeb(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockWeb(id, mat, name)
, cBlock(atlas)
{
}

//////////////////////////////////////////////////////////////////////////////////// BlockTNTClient
cBlockTNT::cBlockTNT(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockTNT(id, mat, name)
, cBlock(atlas)
{
}
    
void cBlockTNT::registerIcons(TextureAtlasRegister* atlas)
{
	m_sprite = atlas->getAtlasSprite(getSpriteName() + "_side");
	m_spriteTop = atlas->getAtlasSprite(getSpriteName() + "_top");
	m_spriteBottom = atlas->getAtlasSprite(getSpriteName() + "_bottom");
}

AtlasSprite* cBlockTNT::getIcon(int side, int meta)
{
	return side == 0 ? m_spriteBottom : (side == 1) ? m_spriteTop : m_sprite;
}

///////////////////////////////////////////////////////////////////////////////////// BlockBookshelfClient
cBlockBookshelf::cBlockBookshelf(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockBookshelf(id, mat, name)
, cBlock(atlas)
{
}

AtlasSprite* cBlockBookshelf::getIcon(int side, int meta)
{
	return side != 1 && side != 0 ? cBlock::getIcon(side, meta) : cBlockManager::scBlocks[BLOCK_ID_PLANKS]->getSpriteFromSide(side);
}

///////////////////////////////////////////////////////////////////////////////////// BlockWorkbenchClient
cBlockWorkbench::cBlockWorkbench(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockWorkbench(id, mat, name)
, cBlock(atlas)
{
}

void cBlockWorkbench::registerIcons(TextureAtlasRegister* atlas)
{
	m_sprite = atlas->getAtlasSprite(getSpriteName() + "_side");
	m_spriteTop = atlas->getAtlasSprite(getSpriteName() + "_top");
	m_spriteFront = atlas->getAtlasSprite(getSpriteName() + "_front");
}

AtlasSprite* cBlockWorkbench::getIcon(int side, int meta)
{
	if (side == 1)
	{
		return m_spriteTop;
	}
	else if (side == 0)
	{
		return  cBlockManager::scBlocks[BLOCK_ID_PLANKS]->getSpriteFromSide(side);
	}
	else
	{
		return side != 2 && side != 4 ? m_sprite : m_spriteFront;
	}
}

bool cBlockWorkbench::onBlockActivated(World * pWorld, const BlockPos & pos, EntityPlayer * pPlayer, int face, const Vector3 & hit)
{
	CraftingTableActivateEvent::emit();
	return true;
}

///////////////////////////////////////////////////////////////////////////////////// BlockClayClient
cBlockClay::cBlockClay(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockClay(id, mat, name)
, cBlock(atlas)
{
}

///////////////////////////////////////////////////////////////////////////////////// BlockRedstoneOreClient
cBlockRedstoneOre::cBlockRedstoneOre(int id, const BM_Material& mat, const String& name, const String& atlas, bool glowing)
: BlockRedstoneOre(id, mat, name, glowing)
, cBlock(atlas)
{
}

void cBlockRedstoneOre::randomDisplayTick(World* pWorld, const BlockPos& pos, Random& rand)
{
	if (m_glowing)
	{
		sparkle(pWorld, pos);
	}
}

void cBlockRedstoneOre::sparkle(World* pWorld, const BlockPos& pos)
{
	Random& rand = pWorld->m_Rand;
	float ext = 0.0625;

	for (int i = 0; i < 6; ++i)
	{
		Vector3 vec = Vector3(pos) + rand.nextVector();

		if (i == 0 && !pWorld->isBlockOpaqueCube(pos.getPosY()))
		{
			vec.y = pos.y + 1 + ext;
		}

		if (i == 1 && !pWorld->isBlockOpaqueCube(pos.getNegY()))
		{
			vec.y = pos.y - ext;
		}

		if (i == 2 && !pWorld->isBlockOpaqueCube(pos.getPosZ()))
		{
			vec.z = pos.z + 1 + ext;
		}

		if (i == 3 && !pWorld->isBlockOpaqueCube(pos.getNegZ()))
		{
			vec.z = pos.z - ext;
		}

		if (i == 4 && !pWorld->isBlockOpaqueCube(pos.getPosX()))
		{
			vec.x = pos.x + 1 + ext;
		}

		if (i == 5 && !pWorld->isBlockOpaqueCube(pos.getNegX()))
		{
			vec.x = pos.x - ext;
		}

		if (vec.x < (float)pos.x ||
			vec.x >(float)(pos.x + 1) ||
			vec.y < 0.f ||
			vec.y >(float)(pos.y + 1) ||
			vec.z < (float)pos.z ||
			vec.z >(float)(pos.z + 1))
		{
			pWorld->spawnParticle("reddust", vec, Vector3::ZERO);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////// BlockRailPoweredClient
cBlockRailPowered::cBlockRailPowered(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockRailPowered(id, mat, name)
, cBlock(atlas)
{

}

void cBlockRailPowered::registerIcons(TextureAtlasRegister* atlas)
{
	m_sprite = atlas->getAtlasSprite(getSpriteName());
	m_spritePowered = atlas->getAtlasSprite(getSpriteName() + "_powered");
}

AtlasSprite* cBlockRailPowered::getIcon(int side, int meta)
{
	return (meta & 8) == 0 ? m_sprite : m_spritePowered;
}

///////////////////////////////////////////////////////////////////////////////////// BlockRailClient
cBlockRail::cBlockRail(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockRail(id, mat, name)
, cBlock(atlas)
{
}

void cBlockRail::registerIcons(TextureAtlasRegister* atlas)
{
	m_sprite = atlas->getAtlasSprite(getSpriteName());
	m_spriteTurned = atlas->getAtlasSprite(getSpriteName() + "_turned");
}

AtlasSprite* cBlockRail::getIcon(int side, int meta)
{
	return meta >= 6 ? m_spriteTurned : m_sprite;
}

///////////////////////////////////////////////////////////////////////////////////// BlockDetectorRailClient
cBlockDetectorRail::cBlockDetectorRail(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockDetectorRail(id, mat, name)
, cBlock(atlas)
{
}

void cBlockDetectorRail::registerIcons(TextureAtlasRegister* atlas)
{
	m_sprite = atlas->getAtlasSprite(getSpriteName());
	m_spritePowered = atlas->getAtlasSprite(getSpriteName() + "_powered");
}

AtlasSprite* cBlockDetectorRail::getIcon(int side, int meta)
{
	return (meta & 8) != 0 ? m_spritePowered : m_sprite;
}

///////////////////////////////////////////////////////////////////////////////////// BlockPistonBaseClient
cBlockPistonBase::cBlockPistonBase(int id, const BM_Material& mat, const String& name, const String& atlas, bool isSticky)
: BlockPistonBase(id, mat, name, isSticky)
, cBlock(atlas)
{
}

void cBlockPistonBase::registerIcons(TextureAtlasRegister* atlas)
{
	m_sprite = atlas->getAtlasSprite("piston_side");
	m_spriteTop = atlas->getAtlasSprite(m_isSticky ? "piston_top_sticky" : "piston_top_normal");
	m_spriteBottom = atlas->getAtlasSprite("piston_bottom");
	m_spriteInner = atlas->getAtlasSprite("piston_inner");
}

AtlasSprite* cBlockPistonBase::getIcon(int side, int meta)
{
	int dir = getOrientation(meta);
	if (dir > 5)
	{
		return m_spriteTop;
	}

	if (side == dir)
	{
		if (!isExtended(meta)
			&& m_min.x <= 0.0 && m_min.y <= 0.0 && m_min.z <= 0.0
			&& m_max.x >= 1.0 && m_max.y >= 1.0 && m_max.z >= 1.0)
		{
			return m_spriteTop;
		}
		else
		{
			return m_spriteInner;
		}
	}
	return (side == oppositeSide[dir] ? m_spriteBottom : m_sprite);
}

AtlasSprite* cBlockPistonBase::getIconByName(const String& name)
{
	cBlockPistonBase* pPiston = (cBlockPistonBase*)cBlockManager::scBlocks[BLOCK_ID_PISTON_BASE];
	cBlockPistonBase* pStickPiston = (cBlockPistonBase*)cBlockManager::scBlocks[BLOCK_ID_PISTON_STICKY_BASE];
	if (name == "piston_side")
		return pPiston->m_sprite;
	else if (name == "piston_top_normal")
		return pPiston->m_spriteTop;
	else if (name == "piston_top_sticky")
		return pStickPiston->m_spriteTop;
	else if (name == "piston_inner")
		return pPiston->m_spriteInner;

	return nullptr;
}

///////////////////////////////////////////////////////////////////////////////////// BlockPistonExtensionClient
cBlockPistonExtension::cBlockPistonExtension(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockPistonExtension(id, mat, name)
, cBlock(atlas)
{
}

AtlasSprite* cBlockPistonExtension::getIcon(int side, int meta)
{
	int d = getDirectionMeta(meta);

	if (side == d)
	{
		if (m_spriteHead != NULL)
		{
			return m_spriteHead;
		}
		else
		{
			if ((meta & 8) != 0)
			{
				return cBlockPistonBase::getIconByName("piston_top_sticky");
			}
			else
			{
				return cBlockPistonBase::getIconByName("piston_top_normal");
			}
		}
	}
	else if (d < 6 && side == oppositeSide[d])
	{
		return cBlockPistonBase::getIconByName("piston_top_normal");
	}
	return cBlockPistonBase::getIconByName("piston_side");
}

///////////////////////////////////////////////////////////////////////////////////// cBlockPistonMoving
cBlockPistonMoving::cBlockPistonMoving(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockPistonMoving(id, mat, name)
, cBlock(atlas)
{}

///////////////////////////////////////////////////////////////////////////////////// BlockTorchClient
cBlockTorch::cBlockTorch(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockTorch(id, mat, name)
, cBlock(atlas)
{
}

void cBlockTorch::randomDisplayTick(World* pWorld, const BlockPos& pos, Random& rand)
{
	int meta = pWorld->getBlockMeta(pos);
	Vector3 spawn = Vector3(pos) + Vector3(0.5f, 0.7f, 0.5f);
	float off1 = 0.22f;
	float off2 = 0.27f;

	if (meta == 1)
	{
		pWorld->spawnParticle("smoke", spawn + Vector3(-off2, off1, 0), Vector3::ZERO);
		pWorld->spawnParticle("flame", spawn + Vector3(-off2, off1, 0), Vector3::ZERO);
	}
	else if (meta == 2)
	{
		pWorld->spawnParticle("smoke", spawn + Vector3(off2, off1, 0), Vector3::ZERO);
		pWorld->spawnParticle("flame", spawn + Vector3(off2, off1, 0), Vector3::ZERO);
	}
	else if (meta == 3)
	{
		pWorld->spawnParticle("smoke", spawn + Vector3(0, off1, -off2), Vector3::ZERO);
		pWorld->spawnParticle("flame", spawn + Vector3(0, off1, -off2), Vector3::ZERO);
	}
	else if (meta == 4)
	{
		pWorld->spawnParticle("smoke", spawn + Vector3(0, off1, off2), Vector3::ZERO);
		pWorld->spawnParticle("flame", spawn + Vector3(0, off1, off2), Vector3::ZERO);
	}
	else
	{
		pWorld->spawnParticle("smoke", spawn, Vector3::ZERO);
		pWorld->spawnParticle("flame", spawn, Vector3::ZERO);
	}
}

///////////////////////////////////////////////////////////////////////////////////// BlockRedstoneTorchClient
cBlockRedstoneTorch::cBlockRedstoneTorch(int id, const BM_Material& mat, const String& name, const String& atlas, bool isActive)
: BlockRedstoneTorch(id, mat, name, isActive)
, cBlock(atlas)
{
}

void cBlockRedstoneTorch::randomDisplayTick(World* pWorld, const BlockPos& pos, Random& rand)
{
	if (!m_isActive)
		return;

	int meta = pWorld->getBlockMeta(pos);
	Vector3 loc = Vector3(pos) + Vector3(0.5f, 0.7f, 0.5f) + rand.nextVector() * 0.2f;
	float ey = 0.22f;
	float exz = 0.27f;

	if (meta == 1)
	{
		pWorld->spawnParticle("reddust", loc + Vector3(-exz, ey, 0.f), Vector3::ZERO);
	}
	else if (meta == 2)
	{
		pWorld->spawnParticle("reddust", loc + Vector3(exz, ey, 0.f), Vector3::ZERO);
	}
	else if (meta == 3)
	{
		pWorld->spawnParticle("reddust", loc + Vector3(0., ey, -exz), Vector3::ZERO);
	}
	else if (meta == 4)
	{
		pWorld->spawnParticle("reddust", loc + Vector3(0., ey, exz), Vector3::ZERO);
	}
	else
	{
		pWorld->spawnParticle("reddust", loc, Vector3::ZERO);
	}
}

///////////////////////////////////////////////////////////////////////////////////// BlockFireClient
cBlockFire::cBlockFire(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockFire(id, mat, name)
, cBlock(atlas)
{
}

void cBlockFire::registerIcons(TextureAtlasRegister* atlas)
{
	m_sprites[0] = atlas->getAtlasSprite(getSpriteName() + "_layer_0");
	m_sprites[1] = atlas->getAtlasSprite(getSpriteName() + "_layer_1");
}

AtlasSprite* cBlockFire::getIcon(int side, int meta)
{
	if (side > 1)
	{
		side = 1;
	}
	return m_sprites[side];
}

void cBlockFire::randomDisplayTick(World* pWorld, const BlockPos& pos, Random& rand)
{
	//if (rand.nextInt(24) == 0)
	//{
		//pWorld->playSound(Vector3(pos) + 0.5f, "fire.fire", 1.0F + rand.nextFloat(), rand.nextFloat() * 0.7F + 0.3F, false);
		if (pWorld->canPlaySpecialSound(1, 50))
		{
			pWorld->playSoundByType(Vector3(pos) + 0.5f, ST_Fire);
		}
	//}

	if (!pWorld->doesBlockHaveSolidTopSurface(pos.getNegY()) && !canBlockCatchFire(pWorld, pos.getNegY()))
	{
		if (canBlockCatchFire(pWorld, pos.getNegY()))
		{
			for (int i = 0; i < 2; ++i)
			{
				Vector3 loc = Vector3(pos) + rand.nextVector() * Vector3(0.1f, 1.f, 1.f);
				pWorld->spawnParticle("largesmoke", loc, Vector3::ZERO);
			}
		}

		if (canBlockCatchFire(pWorld, pos.getPosY()))
		{
			for (int i = 0; i < 2; ++i)
			{
				Vector3 loc = Vector3(pos.getPosX()) + rand.nextVector() * Vector3(-0.1f, 1.f, 1.f);
				pWorld->spawnParticle("largesmoke", loc, Vector3::ZERO);
			}
		}

		if (canBlockCatchFire(pWorld, pos.getNegZ()))
		{
			for (int i = 0; i < 2; ++i)
			{
				Vector3 loc = Vector3(pos) + rand.nextVector() * Vector3(1.f, 1.f, 0.1f);
				pWorld->spawnParticle("largesmoke", loc, Vector3::ZERO);
			}
		}

		if (canBlockCatchFire(pWorld, pos.getPosZ()))
		{
			for (int i = 0; i < 2; ++i)
			{
				Vector3 loc = Vector3(pos.getPosZ()) + rand.nextVector() * Vector3(1.f, 1.f, -0.1f);
				pWorld->spawnParticle("largesmoke", loc, Vector3::ZERO);
			}
		}

		if (canBlockCatchFire(pWorld, pos.getPosY()))
		{
			for (int i = 0; i < 2; ++i)
			{
				Vector3 loc = Vector3(pos.getPosY()) + rand.nextVector() * Vector3(1.f, -0.1f, 1.f);
				pWorld->spawnParticle("largesmoke", loc, Vector3::ZERO);
			}
		}
	}
	else
	{
		for (int i = 0; i < 3; ++i)
		{
			Vector3 loc = Vector3(pos) + rand.nextVector() * Vector3(1.f, 0.5f, 1.f) + Vector3(0.f, 0.5f, 0.f);
			pWorld->spawnParticle("largesmoke", loc, Vector3::ZERO);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////// BlockLeverClient
cBlockLever::cBlockLever(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockLever(id, mat, name)
, cBlock(atlas)
{}

///////////////////////////////////////////////////////////////////////////////////// BlockNetherStalkClient
cBlockNetherStalk::cBlockNetherStalk(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockNetherStalk(id, mat, name)
, cBlockFlower(atlas)
{}

void cBlockNetherStalk::registerIcons(TextureAtlasRegister* atlas)
{
	for (int i = 0; i < 3; ++i)
	{
		m_sprites[i] = atlas->getAtlasSprite(getSpriteName() + "_stage_" + StringUtil::ToString(i));
	}
}

AtlasSprite* cBlockNetherStalk::getIcon(int side, int meta)
{
	if (meta > 2)
	{
		meta = 2;
	}
	return m_sprites[meta];
}

///////////////////////////////////////////////////////////////////////////////////// BlockBrewingStandClient
cBlockBrewingStand::cBlockBrewingStand(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockBrewingStand(id, mat, name)
, cBlock(atlas)
{}

void cBlockBrewingStand::registerIcons(TextureAtlasRegister* atlas)
{
	m_sprite = atlas->getAtlasSprite(getSpriteName());
	m_spriteBase = atlas->getAtlasSprite(getSpriteName() + "_base");
}

AtlasSprite* cBlockBrewingStand::getIcon(int side, int meta)
{
	return side == 0 ? m_sprite : m_spriteBase;
}

void cBlockBrewingStand::randomDisplayTick(World* pWorld, const BlockPos& pos, Random& rand)
{
	Vector3 loc = Vector3(pos) + Vector3(0.4f, 0.7f, 0.4f) + rand.nextVector() * Vector3(0.2f, 0.3f, 0.2f);
	pWorld->spawnParticle("smoke", loc, Vector3::ZERO);
}

///////////////////////////////////////////////////////////////////////////////////// BlockCauldronClient
cBlockCauldron::cBlockCauldron(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockCauldron(id, mat, name)
, cBlock(atlas)
, m_pass(TRP_SOLID)
{}

void cBlockCauldron::registerIcons(TextureAtlasRegister* atlas)
{
	m_spriteInner = atlas->getAtlasSprite(getSpriteName() + "_inner");
	m_spriteTop = atlas->getAtlasSprite(getSpriteName() + "_top");
	m_spriteBottom = atlas->getAtlasSprite(getSpriteName() + "_bottom");
	m_sprite = atlas->getAtlasSprite(getSpriteName() + "_side");
}

AtlasSprite* cBlockCauldron::getIcon(int side, int meta)
{
	return side == 1 ? m_spriteTop : (side == 0 ? m_spriteBottom : m_sprite);
}

///////////////////////////////////////////////////////////////////////////////////// BlockDragonEggClient
cBlockDragonEgg::cBlockDragonEgg(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockDragonEgg(id, mat, name)
, cBlock(atlas)
{}

///////////////////////////////////////////////////////////////////////////////////// BlockTripWireClient
cBlockTripWire::cBlockTripWire(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockTripWire(id, mat, name)
, cBlock(atlas)
{}

///////////////////////////////////////////////////////////////////////////////////// BlockTripWireSourceClient
cBlockTripWireSource::cBlockTripWireSource(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockTripWireSource(id, mat, name)
, cBlock(atlas)
{}

///////////////////////////////////////////////////////////////////////////////////// BlockHopperClient
cBlockHopper::cBlockHopper(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockHopper(id, mat, name)
, cBlock(atlas)
{}

void cBlockHopper::registerIcons(TextureAtlasRegister* atlas)
{
	m_spriteOutside = atlas->getAtlasSprite("hopper_outside");
	m_spriteTop = atlas->getAtlasSprite("hopper_top");
	m_spriteInside = atlas->getAtlasSprite("hopper_inside");
}

AtlasSprite* cBlockHopper::getIcon(int side, int meta)
{
	return side == 1 ? m_spriteTop : m_spriteOutside;
}

AtlasSprite* cBlockHopper::getHopperIcon(const String& name)
{
	if (name == "hopper_outside")
		return m_spriteOutside;

	if (name == "hopper_inside")
		return m_spriteInside;

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////////// BlockDropperClient
cBlockDropper::cBlockDropper(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockDropper(id, mat, name)
, cBlock(atlas)
{}

AtlasSprite* cBlockDropper::getIcon(int side, int meta)
{
	int metamod = meta & 7;
	return side == metamod ? (metamod != 1 && metamod != 0 ? m_spriteFurnaceFrontHorizontal : m_spriteFurnaceFrontVertical) :
		(metamod != 1 && metamod != 0 ? (side != 1 && side != 0 ? m_sprite : m_spriteFurnaceTop) : m_spriteFurnaceTop);
}

void cBlockDropper::registerIcons(TextureAtlasRegister* atlas)
{
	m_sprite = atlas->getAtlasSprite("furnace_side");
	m_spriteFurnaceTop = atlas->getAtlasSprite("furnace_top");
	m_spriteFurnaceFrontHorizontal = atlas->getAtlasSprite(getSpriteName() + "_front_horizontal");
	m_spriteFurnaceFrontVertical = atlas->getAtlasSprite(getSpriteName() + "_front_vertical");
}

///////////////////////////////////////////////////////////////////////////////////// BlockBasePressurePlateClient
cBlockBasePressurePlate::cBlockBasePressurePlate(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockBasePressurePlate(id, mat, name)
, cBlock(atlas)
{}

///////////////////////////////////////////////////////////////////////////////////// BlockPressurePlateClient
cBlockPressurePlate::cBlockPressurePlate(int id, const BM_Material& mat, const String& name, const String& atlas, MOB_TYPE mobType)
: BlockPressurePlate(id, mat, name, mobType)
, cBlock(atlas)
{}

///////////////////////////////////////////////////////////////////////////////////// BlockPressurePlateWeightedClient
cBlockPressurePlateWeighted::cBlockPressurePlateWeighted(int id, const BM_Material& mat, const String& name, const String& atlas, int maxItemsWeighted, MOB_TYPE mobType)
: BlockPressurePlateWeighted(id, mat, name, maxItemsWeighted, mobType)
, cBlock(atlas)
{}

///////////////////////////////////////////////////////////////////////////////////// BlockSoulSandClient
cBlockSoulSand::cBlockSoulSand(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockSoulSand(id, mat, name)
, cBlock(atlas)
{}

///////////////////////////////////////////////////////////////////////////////////// BlockGlowStoneClient
cBlockGlowStone::cBlockGlowStone(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockGlowStone(id, mat, name)
, cBlock(atlas)
{}

///////////////////////////////////////////////////////////////////////////////////// BlockEndPortalFrameClient
cBlockEndPortalFrame::cBlockEndPortalFrame(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockEndPortalFrame(id, mat, name)
, cBlock(atlas)
{}

/** implement override functions from BlockClient */
void cBlockEndPortalFrame::registerIcons(TextureAtlasRegister* atlas)
{
	m_sprite = atlas->getAtlasSprite(getSpriteName() + "_side");
	m_spriteTop = atlas->getAtlasSprite(getSpriteName() + "_top");
	m_spriteEye = atlas->getAtlasSprite(getSpriteName() + "_eye");
}

AtlasSprite* cBlockEndPortalFrame::getIcon(int side, int meta)
{
	if (side == 1)
	{
		return m_spriteTop;
	}
	else if (side == 0)
	{
		return  cBlockManager::scBlocks[BLOCK_ID_WHITE_STONE]->getSpriteFromSide(side);
	}
	else
	{
		return m_sprite;
	}
}


///////////////////////////////////////////////////////////////////////////////////// cBlockSign
cBlockSign::cBlockSign(int id, const BM_Material& mat, const String& name, const String& atlas, int signClass, bool freestanding)
: BlockSign(id, mat, name, signClass, freestanding)
, cBlock(atlas)
{}

AtlasSprite* cBlockSign::getIcon(int side, int meta)
{
	return cBlockManager::scBlocks[BLOCK_ID_PLANKS]->getSpriteFromSide(side);
}

Box cBlockSign::getSelectedBoundingBox(World* pWorld, const BlockPos& pos)
{
	setBlockBoundsBasedOnState(pWorld, pos);
	return Block::getSelectedBoundingBox(pWorld, pos);
}

///////////////////////////////////////////////////////////////////////////////////// cBlockComparator
cBlockComparator::cBlockComparator(int id, const BM_Material& mat, const String& name, const String& atlas, bool powered)
: BlockComparator(id, mat, name, powered)
, cBlock(atlas)
{}

AtlasSprite* cBlockComparator::getIcon(int side, int meta)
{
	bool actived = isRepeaterPowered || (meta & 8) != 0;
	if (side == 0)
	{
		if (actived)
			return  cBlockManager::scBlocks[BLOCK_ID_TORCH_REDSTONE_ACTIVE]->getSpriteFromSide(side);
		else
			return cBlockManager::scBlocks[BLOCK_ID_TORCH_REDSTONE_IDLE]->getSpriteFromSide(side);
	}
	else if (side == 1)
	{
		if (actived)
			return cBlockManager::scBlocks[BLOCK_ID_REDSTONE_COMPARATOR_ACTIVE]->getSprite();
		else
			return m_sprite;
	}
	return cBlockManager::scBlocks[BLOCK_ID_STONE_DOUBLE_SLAB]->getSpriteFromSide(1);
}
///////////////////////////////////////////////////////////////////////////////////// cBlockRedstoneWire
cBlockRedstoneWire::cBlockRedstoneWire(int id, const BM_Material& mat, const String& name, const String& atlas)
	: BlockRedstoneWire(id, mat, name)
	, cBlock(atlas)
	, m_sprite_cross(nullptr)
	, m_sprite_line(nullptr)
	, m_sprite_corss_overlay(nullptr)
	, m_sprite_line_overlay(nullptr)
{}

void cBlockRedstoneWire::randomDisplayTick(World* pWorld, const BlockPos& pos, Random& rand)
{
	int strength = pWorld->getBlockMeta(pos);

	if (strength > 0)
	{
		Vector3 loc = (Vector3)pos;
		loc.x += 0.5f + (rand.nextFloat() - 0.5f) * 0.2f;
		loc.y += 0.0625F;
		loc.z += 0.5f + (rand.nextFloat() - 0.5f) * 0.2f;
		float fStrength = (float)strength / 15.0F;
		Vector3 vel;
		vel.x = fStrength * 0.6F + 0.4F;
		if (strength == 0)
			vel.x = 0.0F;
		vel.y = fStrength * fStrength * 0.7F - 0.5F;
		vel.z = fStrength * fStrength * 0.6F - 0.7F;
		if (vel.y < 0.0F)
			vel.y = 0.0F;
		if (vel.z < 0.0F)
			vel.z = 0.0F;

		pWorld->spawnParticle("reddust", loc, vel);
	}
}

void cBlockRedstoneWire::registerIcons(TextureAtlasRegister* atlas)
{
	m_sprite_cross = atlas->getAtlasSprite(getSpriteName() + "_cross");
	m_sprite_line = atlas->getAtlasSprite(getSpriteName() + "_line");
	m_sprite_corss_overlay = atlas->getAtlasSprite(getSpriteName() + "_cross_overlay");
	m_sprite_line_overlay = atlas->getAtlasSprite(getSpriteName() + "_line_overlay");
	m_sprite = m_sprite_cross;
}

AtlasSprite* cBlockRedstoneWire::getSpriteByName(const String& name)
{
	cBlockRedstoneWire* pWire = dynamic_cast<cBlockRedstoneWire*>(cBlockManager::scBlocks[BLOCK_ID_REDSTONE_WIRE]);
	if (name == "cross")
		return pWire->m_sprite_cross;
	else if (name == "line")
		return pWire->m_sprite_line;
	else if (name == "cross_overlay")
		return pWire->m_sprite_corss_overlay;
	else if (name == "line_overlay")
		return pWire->m_sprite_line_overlay;
	return nullptr;
}

///////////////////////////////////////////////////////////////////////////////////// cBlockRedstoneRepeater
float cBlockRedstoneRepeater::repeaterTorchOffset[4] = { -0.0625f, 0.0625f, 0.1875f, 0.3125f};

cBlockRedstoneRepeater::cBlockRedstoneRepeater(int id, const BM_Material& mat, const String& name, const String& atlas, bool powered)
: BlockRedstoneRepeater(id, mat, name, powered)
, cBlock(atlas)
{}

AtlasSprite* cBlockRedstoneRepeater::getIcon(int side, int meta)
{
	if (side == 0)
	{
		if (isRepeaterPowered)
			return cBlockManager::scBlocks[BLOCK_ID_TORCH_REDSTONE_ACTIVE]->getSpriteFromSide(side);
		else
			return cBlockManager::scBlocks[BLOCK_ID_TORCH_REDSTONE_IDLE]->getSpriteFromSide(side);
	}
	else if (side == 1)
	{
		return m_sprite;
	}
	return cBlockManager::scBlocks[BLOCK_ID_STONE_DOUBLE_SLAB]->getSpriteFromSide(1);
}

void cBlockRedstoneRepeater::randomDisplayTick(World* pWorld, const BlockPos& pos, Random& rand)
{
	if (!isRepeaterPowered)
		return;
	
	int meta = pWorld->getBlockMeta(pos);
	int dir = getDirection(meta);
	Vector3 loc = Vector3(pos) + Vector3(0.5f, 0.4f, 0.5f) + (rand.nextVector() - Vector3::HALF) * 0.2f;
	float fx = 0.0f;
	float fz = 0.0f;

	if (rand.nextInt(2) == 0)
	{
		switch (dir)
		{
		case 0: fz = -0.3125f; break;
		case 1: fx = 0.3125f; break;
		case 2: fz = 0.3125f; break;
		case 3:	fx = -0.3125f; break;
		}
	}
	else
	{
		int idx = (meta & 12) >> 2;

		switch (dir)
		{
		case 0:	fz = repeaterTorchOffset[idx]; break;
		case 1:	fx = -repeaterTorchOffset[idx]; break;
		case 2:	fz = -repeaterTorchOffset[idx]; break;
		case 3: fx = repeaterTorchOffset[idx]; break;
		}
	}

	pWorld->spawnParticle("reddust", loc + Vector3(fx, 0.f, fz), Vector3::ZERO);
}

///////////////////////////////////////////////////////////////////////////////////// cBlockEndPortal
cBlockEndPortal::cBlockEndPortal(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockEndPortal(id, mat, name)
, cBlock(atlas)
{}

bool cBlockEndPortal::shouldSideBeRendered(IBlockAccess* piBlockAccess, const BlockPos& pos, int side)
{
	return side != 0 ? false : Block::shouldSideBeRendered(piBlockAccess, pos, side);
}

void cBlockEndPortal::randomDisplayTick(World* pWorld, const BlockPos& pos, Random& rand)
{
	Vector3 loc = Vector3(pos) + Vector3(rand.nextFloat(), 0.8f, rand.nextFloat());
	pWorld->spawnParticle("smoke", loc, Vector3::ZERO);
}

///////////////////////////////////////////////////////////////////////////////////// cBlockEnderChest
cBlockEnderChest::cBlockEnderChest(int id, const BM_Material& mat, const String& name, const String& atlas, int block_ender_chest_type)
: BlockEnderChest(id, mat, name, block_ender_chest_type)
, cBlock(atlas)
{}

void cBlockEnderChest::randomDisplayTick(World* pWorld, const BlockPos& pos, Random& rand)
{
	for (int i = 0; i < 3; ++i)
	{
		int i1 = rand.nextInt(2) * 2 - 1;
		int i2 = rand.nextInt(2) * 2 - 1;
		Vector3 vel = (rand.nextVector() - Vector3::HALF) * 0.125f;
		vel.x = rand.nextFloat() * i1;
		vel.z = rand.nextFloat() * i2;
		Vector3 loc;
		loc.x = pos.x + 0.75f * i1;
		loc.z = pos.z + 0.75f * i2;
		loc.y = pos.y + rand.nextFloat();

		pWorld->spawnParticle("portal", loc, vel);
	}
}

bool cBlockEnderChest::onBlockActivated(World * pWorld, const BlockPos & pos, EntityPlayer * pPlayer, int face, const Vector3 & hit)
{
	IInventory* pInventory = (IInventory*)pPlayer->getInventoryEnderChest();
	if (pInventory != NULL)
	{
		ChestActivateEvent::emit(pInventory, pos, face, hit);
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////////// cBlockSkull
cBlockSkull::cBlockSkull(int id, const BM_Material& mat, const String& name, const String& atlas)
: BlockSkull(id, mat, name)
, cBlock(atlas)
{}

AtlasSprite* cBlockSkull::getIcon(int side, int meta)
{
	return cBlockManager::scBlocks[BLOCK_ID_SLOW_SAND]->getSpriteFromSide(side);
}

String cBlockSkull::getItemSpriteName() const
{
	return getSpriteName() + "_" + ItemSkull::skullTypes[0];
}

///////////////////////////////////////////////////////////////////////////////////// cBlockWithEffect
cBlockWithEffect::cBlockWithEffect(int id, const BM_Material& mat, const String& name, const String& atlas)
	: BlockWithEffect(id, mat, name)
	, cBlock(atlas)
{}

void cBlockWithEffect::randomDisplayTick(World * pWorld, const BlockPos & pos, Random & rand)
{
//	Vector3 fpos = Vector3(pos) + Vector3(0.5f, 0.2f, 0.5f);
//
//	float rx = Math::UnitRandom() - 0.5f;
//	float rz = Math::UnitRandom() - 0.5f;
//
//	pWorld->spawnParticle("chestFx", fpos + Vector3(rx, 0.f, rz), Vector3((rx + 0.5f) * 0.1f, 0.1f, (rz + 0.5f)*0.1f));
}

void cBlockWithEffect::registerIcons(TextureAtlasRegister * atlas)
{
	m_sprite = atlas->getAtlasSprite(getSpriteName() + "_top");
	m_front = atlas->getAtlasSprite(getSpriteName() + "_front");
	m_side = atlas->getAtlasSprite(getSpriteName() + "_side");
}

AtlasSprite * cBlockWithEffect::getIcon(int side, int meta)
{
	if (side == 0 || side == 1)
		return m_sprite;

	if (side == 2)
		return m_front;

	return m_side;
}

///////////////////////////////////////////////////////////////////////////////////// BlockDirtClient
cBlockDirt::cBlockDirt(int id, const BM_Material& mat, const String& name, const String& atlas)
	: BlockDirt(id, mat, name)
	, cBlock(atlas)
{
	m_spriteTop = NULL;
}

AtlasSprite* cBlockDirt::getIcon(int side, int meta)
{
	if (side == 1)
		return m_spriteTop;
	return m_sprite;
}

void cBlockDirt::registerIcons(TextureAtlasRegister* atlas)
{
	m_sprite = atlas->getAtlasSprite(getSpriteName());
	m_spriteTop = atlas->getAtlasSprite(getSpriteName() + "_top");
}

cBlockNewCrops::cBlockNewCrops(int id, const BM_Material & mat, const String & name, const String & atlas, int seedsId, int renderType)
	: BlockNewCrops(id, mat, name, seedsId)
	, cBlockFlower(atlas)
	, m_renderType(renderType)
{
}

AtlasSprite * cBlockNewCrops::getIcon(int side, int meta)
{
	if (meta < 0 || meta > 5)
	{
		meta = 5;
	}

	return m_sprites[meta];
}

void cBlockNewCrops::registerIcons(TextureAtlasRegister * atlas)
{
	for (int i = 0; i < 6; ++i)
	{
		m_sprites[i] = atlas->getAtlasSprite(getSpriteName() + "_stage_" + StringUtil::ToString(i));
	}
}

cBlockSlime::cBlockSlime(int id, const BM_Material& mat, const String& name, const String& atlas)
	: BlockSlime(id, mat, name),
	cBlock(atlas)
{

}

cBlockBanner::cBlockBanner(int id, const BM_Material& mat, const String& name, const String& atlas, int color, bool is_slate)
	: BlockBanner(id, mat, name)
	, cBlock(atlas)
	, color(color)
	, m_is_slate(is_slate)
{
}

bool cBlockBanner::shouldSideBeRendered(IBlockAccess * pBlockAccess, const BlockPos & pos, int side)
{
	return true;
}

void cBlockBanner::registerIcons(TextureAtlasRegister * atlas)
{
	m_sprite = atlas->getAtlasSprite("banner");
	m_sprite_slate = atlas->getAtlasSprite("banner_slate");
}

AtlasSprite * cBlockBanner::getIcon(int side, int meta)
{
	if (m_is_slate)
	{
		return m_sprite_slate;
	}
	else
	{
		return m_sprite;
	}
}

int cBlockBanner::getRenderColor(int meta)
{
	return color;
}

cBlockRoad::cBlockRoad(int id, const BM_Material & mat, const String & name, const String & atlas)
	: BlockRoad(id, mat, name)
	, cBlock(atlas)
{
}

bool cBlockRoad::shouldSideBeRendered(IBlockAccess* pBlockAccess, const BlockPos& pos, int side)
{
	return side == 1 ? true : Block::shouldSideBeRendered(pBlockAccess, pos, side);
}

AtlasSprite* cBlockRoad::getIcon(int side, int meta)
{
	return  m_sprite;
}

cBlockFruits::cBlockFruits(int id, const BM_Material & mat, const String & name, const String & atlas, int renderType)
	: BlockFruits(id, mat, name)
	, cBlock(atlas)
	, m_renderType(renderType)
{
}

AtlasSprite* cBlockFruits::getIcon(int side, int meta)
{
	if (side == 1)
	{
		return m_spriteTop;
	}
	else
	{
		return m_sprite;
	}
}

void cBlockFruits::registerIcons(TextureAtlasRegister* pAtlas)
{
	m_sprite = pAtlas->getAtlasSprite("grass_side");
	m_spriteTop = pAtlas->getAtlasSprite(getSpriteName());
}

int cBlockFruits::getMaxMeta()
{
	auto fruits = FruitsSetting::findFruitById(m_blockID);
	int maxMeta = fruits ? fruits->maxStage : 15;
	return maxMeta > 15 ? 15 : maxMeta;
}

cBlockNest::cBlockNest(int id, const BM_Material & mat, const String & name, const String & atlas)
	: BlockNest(id, mat, name)
	, cBlock(atlas)
{
}

bool cBlockNest::shouldSideBeRendered(IBlockAccess* pBlockAccess, const BlockPos& pos, int side)
{
	return side == 1 ? true : Block::shouldSideBeRendered(pBlockAccess, pos, side);
}

AtlasSprite * cBlockNest::getIcon(int side, int meta)
{
	return m_sprite;
}
///////////////////////////////////////////////////////////////////////////////////// 

///////////////////////////////////////////////////////////////////////////////////// BlockStainedGlass
cBlockStainedGlass::cBlockStainedGlass(int id, const BM_Material& mat, const String& name, const String& atlas)
	: BlockStainedGlass(id, mat, name)
	, cBlock(atlas)
{
}

bool cBlockStainedGlass::shouldSideBeRendered(IBlockAccess* pBlockAccess, const BlockPos& pos, int side)
{
	int blockID = pBlockAccess->getBlockId(pos);
	return !m_flag && blockID == m_blockID ? false : Block::shouldSideBeRendered(pBlockAccess, pos, side);
}

void cBlockStainedGlass::registerIcons(TextureAtlasRegister* atlas)
{
	for (int i = 0; i < colorType; ++i)
	{
		m_sprites[i] = atlas->getAtlasSprite(getSpriteName() + "_" + cItemDye::dyeSpriteNames[getDyeFromBlock(i)]);
	}
}

AtlasSprite* cBlockStainedGlass::getIcon(int side, int meta)
{
	return m_sprites[meta % colorType];
}

ui32 cBlockStainedGlass::colorMultiplier(IBlockAccess* pBlockAccess, const BlockPos& pos)
{
	int r = 0;
	int g = 0;
	int b = 0;

	for (int z = -1; z <= 1; ++z)
	{
		for (int x = -1; x <= 1; ++x)
		{
			int color = pBlockAccess->getBiomeGenForCoords(pos.x + x, pos.z + z)->m_waterColor;
			r += (color & 0xFF0000) >> 16;
			g += (color & 0x00FF00) >> 8;
			b += color & 0x0000FF;
		}
	}

	return 0xFF000000 | (r / 9 & 255) << 16 | (g / 9 & 255) << 8 | b / 9 & 255;
}

int cBlockStainedGlass::getRenderBlockPass()
{
	return TRP_TRANSPARENT_3D;
}

cBlockTransparency::cBlockTransparency(int id, const BM_Material& mat, const String& name, const String& atlas)
	: BlockTransparency(id, mat, name)
	, cBlock(atlas)
{
}

///////////////////////////////////////////////////////////////////////////////////// 
}
