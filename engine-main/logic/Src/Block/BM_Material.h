/********************************************************************
filename: 	mcMaterial.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-11-3
*********************************************************************/
#ifndef __NC_MATERIAL_HEADER__
#define __NC_MATERIAL_HEADER__

#include "Core.h"
using namespace LORD;

namespace BLOCKMAN
{

class BM_Material
{
public:
	BM_Material(
		int colorIdx = 0,
		bool canBurn = false,
		bool replaceable = false,
		bool isTranslucent = false,
		bool requiresNoTool = true,
		int mobilityFlag = 0,
		bool isAdventureModeExempt = true,
		bool issolid = true,
		bool isliquid = false,
		bool blockGrass = true,
		bool blockMement = true);

	// Returns if blocks of these materials are liquids.
	bool isLiquid() const { return m_isLiquid; }

	// Returns if blocks of these materials are solid.
	bool isSolid() const { return m_isSolid; }

	// Will prevent grass from growing on dirt underneath and kill any grass below it if it returns true
	bool canBlockGrass() const { return m_blockGrass; }

	// Returns if this material is considered solid or not
	bool blocksMovement() const { return m_blockMoment; }

	// Marks the material as translucent
	BM_Material& setTranslucent() { m_isTranslucent = true; return *this; }

	// Makes blocks with this material require the correct tool to be harvested.
	BM_Material& setRequiresTool() { m_requiresNoTool = false; return *this; }

	// Set the canBurn bool to True and return the current object.
	BM_Material& setBurning() { m_canBurn = true; return *this; }

	// Returns if the block can burn or not.
	bool getCanBurn() const { return m_canBurn; }

	// Sets {@link #replaceable} to true.
	BM_Material& setReplaceable() { m_replaceable = true; return *this; }

	// Returns whether the material can be replaced by other blocks when placed - eg snow, vines and tall grass.
	bool isReplaceable() const { return m_replaceable; }

	// Indicate if the material is opaque
	bool isOpaque() const { return m_isTranslucent ? false : blocksMovement(); }

	// Returns true if the material can be harvested without a tool (or with the wrong tool)
	bool isToolNotRequired() const { return m_requiresNoTool; }

	// Returns the mobility information of the material, 0 = free, 1 = can't push but can move over, 2 = total immobility and stop pistons.
	int getMaterialMobility() const {	return m_mobilityFlag; }

	// This type of material can't be pushed, but pistons can move over it.
	BM_Material& setNoPushMobility() { m_mobilityFlag = 1; return *this; }

	// This type of material can't be pushed, and pistons are blocked to move.
	BM_Material& setImmovableMobility() { m_mobilityFlag = 2; return *this; }

	// @see #isAdventureModeExempt()
	BM_Material& setAdventureModeExempt() {	m_isAdventureModeExempt = true;	return *this; }

	// Returns true if blocks with this material can always be mined in adventure mode.
	bool isAdventureModeExempt() const { return m_isAdventureModeExempt; }

	bool operator == (const BM_Material& rhs) const;
	bool operator != (const BM_Material& rhs) const { return !(*this == rhs);  }

public:
	static const BM_Material BM_MAT_air;
	static const BM_Material BM_MAT_grass;// = new BM_Material(MapColor.grassColor);
	static const BM_Material BM_MAT_ground;// = new BM_Material(MapColor.dirtColor);
	static const BM_Material BM_MAT_wood;// = (new BM_Material(MapColor.woodColor)).setBurning();
	static const BM_Material BM_MAT_rock;// = (new BM_Material(MapColor.stoneColor)).setRequiresTool();
	static const BM_Material BM_MAT_iron;// = (new BM_Material(MapColor.ironColor)).setRequiresTool();
	static const BM_Material BM_MAT_anvil;// = (new BM_Material(MapColor.ironColor)).setRequiresTool().setImmovableMobility();
	static const BM_Material BM_MAT_water;// = (new MaterialLiquid(MapColor.waterColor)).setNoPushMobility();
	static const BM_Material BM_MAT_lava;// = (new MaterialLiquid(MapColor.tntColor)).setNoPushMobility();
	static const BM_Material BM_MAT_leaves;// = (new BM_Material(MapColor.foliageColor)).setBurning().setTranslucent().setNoPushMobility();
	static const BM_Material BM_MAT_plants;// = (new MaterialLogic(MapColor.foliageColor)).setNoPushMobility();
	static const BM_Material BM_MAT_vine;// = (new MaterialLogic(MapColor.foliageColor)).setBurning().setNoPushMobility().setReplaceable();
	static const BM_Material BM_MAT_sponge;// = new BM_Material(MapColor.clothColor);
	static const BM_Material BM_MAT_cloth;// = (new BM_Material(MapColor.clothColor)).setBurning();
	static const BM_Material BM_MAT_fire;// = (new MaterialTransparent(MapColor.airColor)).setNoPushMobility();
	static const BM_Material BM_MAT_sand;// = new BM_Material(MapColor.sandColor);
	static const BM_Material BM_MAT_circuits;// = (new MaterialLogic(MapColor.airColor)).setNoPushMobility();
	static const BM_Material BM_MAT_carpet;// = (new MaterialLogic(MapColor.clothColor)).setBurning();
	static const BM_Material BM_MAT_glass;// = (new BM_Material(MapColor.airColor)).setTranslucent().setAdventureModeExempt();
	static const BM_Material BM_MAT_redstoneLight;// = (new BM_Material(MapColor.airColor)).setAdventureModeExempt();
	static const BM_Material BM_MAT_tnt;// = (new BM_Material(MapColor.tntColor)).setBurning().setTranslucent();
	static const BM_Material BM_MAT_coral;// = (new BM_Material(MapColor.foliageColor)).setNoPushMobility();
	static const BM_Material BM_MAT_ice;// = (new BM_Material(MapColor.iceColor)).setTranslucent().setAdventureModeExempt();
	static const BM_Material BM_MAT_packedIce;// = (new BM_Material(MapColor.iceColor)).setAdventureModeExempt();
	static const BM_Material BM_MAT_snow;// = (new MaterialLogic(MapColor.snowColor)).setReplaceable().setTranslucent().setRequiresTool().setNoPushMobility();
	static const BM_Material BM_MAT_craftedSnow;// = (new BM_Material(MapColor.snowColor)).setRequiresTool();
	static const BM_Material BM_MAT_cactus;// = (new BM_Material(MapColor.foliageColor)).setTranslucent().setNoPushMobility();
	static const BM_Material BM_MAT_clay;// = new BM_Material(MapColor.clayColor);
	static const BM_Material BM_MAT_pumpkin;// = (new BM_Material(MapColor.foliageColor)).setNoPushMobility();
	static const BM_Material BM_MAT_dragonEgg;// = (new BM_Material(MapColor.foliageColor)).setNoPushMobility();
	static const BM_Material BM_MAT_portal;// = (new MaterialPortal(MapColor.airColor)).setImmovableMobility();
	static const BM_Material BM_MAT_cake;// = (new BM_Material(MapColor.airColor)).setNoPushMobility();
	static const BM_Material BM_MAT_web;// = (new MaterialWeb(MapColor.clothColor)).setRequiresTool().setNoPushMobility();
	static const BM_Material BM_MAT_piston;// = (new BM_Material(MapColor.stoneColor)).setImmovableMobility();

protected:

	// Bool defining if the block can burn or not.
	bool m_canBurn = false;

	// Determines whether blocks with this material can be "overwritten" by other blocks when placed - e.g. snow, vines and tall grass.
	bool m_replaceable = false;

	// Indicates if the material is translucent, e.g. leaves, glass
	bool m_isTranslucent = false;

	// e.g. rock is solid but water is not
	bool m_isSolid = false;

	// e.g. only water, lava is liquid
	bool m_isLiquid = false;

	// Will prevent grass from growing on dirt underneath and kill any grass below it if it is true
	bool m_blockGrass = false;

	// redundant for m_solid????
	bool m_blockMoment = false;

	// no usage now, The color index used to draw the blocks of this material on maps.
	int m_colorIdx = 0;

	// Determines if the material can be harvested without a tool (or with the wrong tool)
	bool m_requiresNoTool = false;

	// Mobility information flag. 0 indicates that this block is normal, 1 indicates that it can't push other blocks, 2 * indicates that it can't be pushed.
	int m_mobilityFlag = 0;

	// no usage now, we only support survival mode now
	bool m_isAdventureModeExempt = false;
};

}

#endif
