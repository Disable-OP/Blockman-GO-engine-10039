#include "BM_Material.h"
#include "BM_TypeDef.h"

namespace BLOCKMAN
{

BM_Material::BM_Material(
	int colorIdx,
	bool canBurn /* = false */,
	bool replaceable /* = false */, 
	bool isTranslucent /* = false */,
	bool requiresNoTool /* = true */,
	int mobilityFlag /* = 0 */,
	bool isAdventureModeExempt /* = true */,
	bool issolid /* = true */,
	bool isliquid /* = false */,
	bool blockGrass /* = true */, 
	bool blockMement /* = true */)
	: m_canBurn(canBurn)
	, m_replaceable(replaceable)
	, m_isTranslucent(isTranslucent)
	, m_colorIdx(colorIdx)
	, m_requiresNoTool(requiresNoTool)
	, m_mobilityFlag(mobilityFlag)
	, m_isAdventureModeExempt(isAdventureModeExempt)
	, m_isSolid(issolid)
	, m_isLiquid(isliquid)
	, m_blockGrass(blockGrass)
	, m_blockMoment(blockMement)
{ }

//												color		   burn   replace tran  tool  move adv   solid liquid  -grass -Move   
const BM_Material BM_Material::BM_MAT_air		(COLOR_AIR,	   false, false, false, true,  0, false, false, false, false, false);
const BM_Material BM_Material::BM_MAT_grass		(COLOR_GRASS,   false, false, false, true,  0, false, true,  false, true,  true);
const BM_Material BM_Material::BM_MAT_ground	(COLOR_DIRT,	   false, false, false, true,  0, false, true,  false, true,  true);
const BM_Material BM_Material::BM_MAT_wood		(COLOR_WOOD,	   true,  false, false, true,  0, false, true,  false, true,  true); //setBurning();
const BM_Material BM_Material::BM_MAT_rock		(COLOR_STONE,   false, false, false, false, 0, false, true,  false, true,  true); //setRequiresTool();
const BM_Material BM_Material::BM_MAT_iron		(COLOR_IRON,    false, false, false, false, 0, false, true,  false, true,  true); //setRequiresTool();
const BM_Material BM_Material::BM_MAT_anvil		(COLOR_IRON,    false, false, false, false, 2, false, true,  false, true,  true); //setRequiresTool().setImmovableMobility();
const BM_Material BM_Material::BM_MAT_water		(COLOR_WATER,   false, true, false, true,  1, false, false, true,  true,  false); //setNoPushMobility();
const BM_Material BM_Material::BM_MAT_lava		(COLOR_TNT,     false, false, false, true,  1, false, false, true,  true,  false); //setNoPushMobility();
const BM_Material BM_Material::BM_MAT_leaves	(CLOLOR_FOLIAGE,true,  false, true,  true,  1, false, true,  false, true,  true); //setBurning().setTranslucent().setNoPushMobility();
const BM_Material BM_Material::BM_MAT_plants	(CLOLOR_FOLIAGE,false, false, false, true,  1, false, false, false, false, false); //setNoPushMobility();
const BM_Material BM_Material::BM_MAT_vine		(CLOLOR_FOLIAGE,true,  true,  false, true,  1, false, false, false, false, false); //setBurning().setNoPushMobility().setReplaceable();
const BM_Material BM_Material::BM_MAT_sponge	(COLOR_CLOTH,   false, false, false, true,  0, false, true,  false, true,  true); //
const BM_Material BM_Material::BM_MAT_cloth		(COLOR_CLOTH,   true,  false, false, true,  0, false, true,  false, true,  true); //setBurning();
const BM_Material BM_Material::BM_MAT_fire	    (COLOR_AIR,     false, true, false, true,  1, false, false, false, false, false); //setNoPushMobility();
const BM_Material BM_Material::BM_MAT_sand      (COLOR_SAND,    false, false, false, true,  0, false, true,  false, true,  true); //
const BM_Material BM_Material::BM_MAT_circuits  (COLOR_AIR,     false, false, false, true,  1, false, false, false, false, false); //setNoPushMobility();
const BM_Material BM_Material::BM_MAT_carpet	(COLOR_CLOTH,   true,  false, false, true,  0, false, false, false, false, false); //setBurning();
const BM_Material BM_Material::BM_MAT_glass     (COLOR_AIR,     false, false, true,  true,  0, true,  true,  false, true,  true);  //setTranslucent().setAdventureModeExempt();
const BM_Material BM_Material::BM_MAT_redstoneLight(COLOR_AIR,  false, false, false, true,  0, true,  true,  false, true,  true);  //setAdventureModeExempt();
const BM_Material BM_Material::BM_MAT_tnt       (COLOR_TNT,     true,  false, true,  true,  0, false, true,  false, true,  true); //setBurning().setTranslucent();
const BM_Material BM_Material::BM_MAT_coral		(CLOLOR_FOLIAGE,false, false, false, true,  1, false, true,  false, true,  true); //setNoPushMobility();
const BM_Material BM_Material::BM_MAT_ice		(COLOR_ICE,	   false, false, true,	 true, 0, true,  true,  false, true,  true);  //setTranslucent().setAdventureModeExempt();
const BM_Material BM_Material::BM_MAT_packedIce (COLOR_ICE,     false, false, false, true,  0, true,  true,  false, true,  true);  //setAdventureModeExempt();
const BM_Material BM_Material::BM_MAT_snow		(COLOR_SNOW,    false, true,  true,  false, 1, false, false, false, false, false); //setReplaceable().setTranslucent().setRequiresTool().setNoPushMobility();
const BM_Material BM_Material::BM_MAT_craftedSnow(COLOR_SNOW,  false, false, false, false, 0, false, true,  false, true,  true); //setRequiresTool();
const BM_Material BM_Material::BM_MAT_cactus	(CLOLOR_FOLIAGE,false, false, true,  true,  1, false, true,  false, true,  true); //setTranslucent().setNoPushMobility();
const BM_Material BM_Material::BM_MAT_clay		(COLOR_CLAY,    false, false, false, true,  0, false, true,  false, true,  true); //
const BM_Material BM_Material::BM_MAT_pumpkin   (CLOLOR_FOLIAGE,false, false, false, true,  1, false, true,  false, true,  true); //setNoPushMobility();
const BM_Material BM_Material::BM_MAT_dragonEgg (CLOLOR_FOLIAGE,false, false, false, true,  1, false, true,  false, true,  true); //setNoPushMobility();
const BM_Material BM_Material::BM_MAT_portal	(COLOR_AIR,     false, false, false, true,  2, false, false, false, false, false); //setImmovableMobility();
const BM_Material BM_Material::BM_MAT_cake		(COLOR_AIR,     false, false, false, true,  1, false, true,  false, true,  true); //setNoPushMobility();
const BM_Material BM_Material::BM_MAT_web       (COLOR_CLOTH,   false, false, false, false, 1, false, true,  false, true,  false); //setRequiresTool().setNoPushMobility();
const BM_Material BM_Material::BM_MAT_piston	(COLOR_STONE,   false, false, false, true,  2, false, true,  false, true,  true); //setImmovableMobility();

bool BM_Material::operator==(const BM_Material& rhs) const
{
	return
		m_canBurn == rhs.m_canBurn &&
		m_replaceable == rhs.m_replaceable &&
		m_isTranslucent == rhs.m_isTranslucent &&
		m_colorIdx == rhs.m_colorIdx &&
		m_requiresNoTool == rhs.m_requiresNoTool &&
		m_mobilityFlag == rhs.m_mobilityFlag &&
		m_isAdventureModeExempt == rhs.m_isAdventureModeExempt &&
		m_isSolid == rhs.m_isSolid && 
		m_isLiquid == rhs.m_isLiquid &&
		m_blockGrass == rhs.m_blockGrass &&
		m_blockMoment == rhs.m_blockMoment;
}

}