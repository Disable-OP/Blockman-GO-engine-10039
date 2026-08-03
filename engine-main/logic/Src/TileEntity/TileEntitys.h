/********************************************************************
filename: 	TileEntitys.h
file path:	dev\client\Src\Blockman\BlockEntity

version:	1
author:		ajohn
company:	supernano
date:		2017-2-24
*********************************************************************/
#ifndef __TILE_ENTITYS_HEADER__
#define __TILE_ENTITYS_HEADER__
#include "RTTI.h"
#include "BM_Container_def.h"
#include "TileEntity.h"
#include "Inventory/IInventory.h"
#include "Util/Random.h"
#include "Util/CommonEvents.h"

namespace BLOCKMAN
{

class TileEntityEnderChest : public TileEntity
{
	RTTI_DECLARE(TileEntityEnderChest);
public:
	TileEntityEnderChest();
	virtual ~TileEntityEnderChest();

	void openChest();
	void closeChest();
	bool isUseableByPlayer(EntityPlayer* pPlayer);
	int getChestType();

	/** implement override functions from TileEntity */
	virtual void updateEntity();
	virtual bool receiveClientEvent(int id, int type);
	virtual void invalidate();

public:
	/** The current angle of the chest lid (between 0 and 1) */
	float m_lidAngle = 0.f;
	/** The angle of the chest lid last tick */
	float m_prevLidAngle = 0.f;
	/** The number of players currently using this ender chest. */
	int m_numUsingPlayers = 0;

protected:
	/** Server sync counter (once per 20 ticks) */
	int m_ticksSync = 0;

};


class TileEntityPiston : public TileEntity
{
	RTTI_DECLARE(TileEntityPiston);
public:
	TileEntityPiston();
	TileEntityPiston(int pistonId, int meta, int dir, bool bExtending, bool renderHead);
	virtual ~TileEntityPiston();

	int getStoredBlockID() { return m_blockID; }
	int getBlockMetadata() { return m_meta; }
	bool isExtending() { return m_extending; }
	int getPistonOrientation() { return m_dir; }
	bool shouldRenderHead() { return m_renderHead; }

	/** Get interpolated progress value (between lastProgress and progress) given the fractional time between ticks as an argument. */
	float getProgress(float rdt);
	float getOffsetX(float rdt);
	float getOffsetY(float rdt);
	float getOffsetZ(float rdt);
	Vector3 getOffset(float rdt);

	/** removes a pistons tile entity (and if the piston is moving, stops it) */
	void clearPistonTileEntity();

	/** implement override functions from TileEntity */
	virtual void updateEntity();
	virtual void readFromNBT(NBTTagCompound* pNbtCompound);
	virtual void writeToNBT(NBTTagCompound* pNbtCompound);

protected:
	void updatePushedObjects(float progress, float dis);

protected:
	int m_blockID = 0;
	int m_meta = 0;

	/** the side the front of the piston is on */
	int m_dir = 0;

	/** if this piston is extending or not */
	bool m_extending = false;
	bool m_renderHead = false;
	float m_progress = 0.f;

	/** the progress in (de)extending */
	float m_lastProgress = 0.f;
	EntityArr m_pushedObjects;
};

class TileEntityComparator : public TileEntity
{
	RTTI_DECLARE(TileEntityComparator);
public:
	TileEntityComparator();
	virtual ~TileEntityComparator();

	int getOutputSignal() { return outputSignal; }
	void setOutputSignal(int signal) { outputSignal = signal; }

	/** implement override functions from TileEntity */
	virtual void writeToNBT(NBTTagCompound* pNbtCompound);
	virtual void readFromNBT(NBTTagCompound* pNbtCompound);

protected:
	int outputSignal = 0;
};

class TileEntitySkull : public TileEntity
{
	RTTI_DECLARE(TileEntitySkull);
public:
	TileEntitySkull();
	virtual ~TileEntitySkull();

	/** implement override functions from TileEntity */
	virtual void writeToNBT(NBTTagCompound* pNbtCompound);
	virtual void readFromNBT(NBTTagCompound* pNbtCompound);
	// public Packet getDescriptionPacket()

	void setSkullType(int type, const String& strType) { m_skullType = type; extraType = strType; }
	int getSkullType() { return m_skullType; }
	int getSkullRotation() { return m_skullRotation; }
	void setSkullRotation(int rot) { m_skullRotation = rot; }
	const String& getExtraType() const { return extraType; }

protected:
	/** Entity type for this skull. */
	int m_skullType = 0;
	/** The skull's rotation. */
	int m_skullRotation = 0;
	/** Extra data for this skull, used as player username by player heads */
	String extraType;
};


class TileEntityRecordPlayer : public TileEntity
{
	RTTI_DECLARE(TileEntityRecordPlayer);
public:
	TileEntityRecordPlayer();
	virtual ~TileEntityRecordPlayer();

	/** implement override functions from TileEntity */
	virtual void readFromNBT(NBTTagCompound* pNbtCompound);
	virtual void writeToNBT(NBTTagCompound* pNbtCompound);

	ItemStackPtr getRecord() { return record; }
	void setRecord(ItemStackPtr par1ItemStack);

protected:
	/** ID of record which is in Jukebox */
	ItemStackPtr record = nullptr;
};

class TileEntitySign : public TileEntity
{
	RTTI_DECLARE(TileEntitySign);
	NBTB_DECLARE_FRIEND(TileEntitySign);
public:
	static const int SIGN_COUNT = 4;

	TileEntitySign();
	virtual ~TileEntitySign();

	/** implement override functions from TileEntity */
	virtual void writeToNBT(NBTTagCompound* pNbtCompound);
	virtual void readFromNBT(NBTTagCompound* pNbtCompound);
	// Packet getDescriptionPacket()

	bool isEditable() { return m_isEditable; }
	void setEditable(bool editable);
	void setPlayer(EntityPlayer* pPlayer) { m_player = pPlayer; }
	EntityPlayer* getPlayer() { return m_player; }
	String getText(size_t line) const
	{
		return m_signTxt[line];
	}
	void setText(size_t line, const String& text)
	{
		m_signTxt[line] = text;
		SignTextsChangeEvent::emit(this);
	}
	void clearAllTexts()
	{
		for (auto& text : m_signTxt)
		{
			text.clear();
		}
		SignTextsChangeEvent::emit(this);
	}

private:
	/** An array of four strings storing the lines of text on the sign. */
	String m_signTxt[SIGN_COUNT];

	/** The index of the line currently being edited. Only used on client side, but defined on both. Note this is only
	* really used when the > < are going to be visible. */
	int m_lineEdited = 0;

protected:
	bool m_isEditable = false;
	EntityPlayer* m_player = nullptr;
};

class TileEntityMobSpawner :public TileEntity
{
	RTTI_DECLARE(TileEntityMobSpawner);
public:
	TileEntityMobSpawner();
	virtual ~TileEntityMobSpawner();

	/** implement override functions from TileEntity */
	virtual void readFromNBT(NBTTagCompound* pNbtCompound);
	virtual void writeToNBT(NBTTagCompound* pNbtCompound);
	virtual void updateEntity();
	// virtual Packet getDescriptionPacket()
	virtual bool receiveClientEvent(int id, int type);

	/** Returns the spawner logic associated with this spawner */
	// MobSpawnerBaseLogic getSpawnerLogic() { return m_spawnLogic; }

protected:
	// todo.
	// MobSpawnerBaseLogic m_spawnLogic = new TileEntityMobSpawnerLogic(this);
};

class TileEntityNote : public TileEntity
{
	RTTI_DECLARE(TileEntityNote);
public:
	TileEntityNote();
	virtual ~TileEntityNote();

	/** implement override functions from TileEntity */
	virtual void writeToNBT(NBTTagCompound* pNbt);
	virtual void readFromNBT(NBTTagCompound* pNbt);

	/** change pitch by -> (currentPitch + 1) % 25 */
	void changePitch();

	/** plays the stored note */
	void triggerNote(World* pWorld, const BlockPos& pos);

public:
	/** Note to play */
	i8 m_note = 0;

	/** stores the latest redstone state */
	bool m_prevRedstoneState = false;
};

class TileEntityEnchantmentTable : public TileEntity
{
	RTTI_DECLARE(TileEntityEnchantmentTable);
public:
	TileEntityEnchantmentTable();
	virtual ~TileEntityEnchantmentTable();

	/** implement override functions from TileEntity */
	virtual void writeToNBT(NBTTagCompound* pNbt);
	virtual void readFromNBT(NBTTagCompound* pNbt);
	virtual void updateEntity();

	String getName() { return hasCustomName() ? m_customName : "container.enchant"; }
	bool hasCustomName() { return !m_customName.empty(); }
	void setCustomName(const String& par1Str) { m_customName = par1Str; }

public:
	/** Used by the render to make the book 'bounce' */
	int m_tickCount = 0;

	/** Value used for determining how the page flip should look. */
	float m_pageFlip = 0.f;

	/** The last tick's pageFlip value. */
	float m_pageFlipPrev = 0.f;
	float m_flipT = 0.f;
	float m_flipA = 0.f;

	/** The amount that the book is open. */
	float m_bookSpread = 0.f;

	/** The amount that the book is open. */
	float m_bookSpreadPrev = 0.f;
	float m_bookRotation2 = 0.f;
	float m_bookRotationPrev = 0.f;
	float m_bookRotation = 0.f;

protected:
	static Random rand;
	String m_customName;

};

class TileEntityEndPortal : public TileEntity
{
	RTTI_DECLARE(TileEntityEndPortal);
public:
	TileEntityEndPortal();
	~TileEntityEndPortal();
};

class TileEntityCommandBlock : public TileEntity /*, public ICommandSender*/
{
	RTTI_DECLARE(TileEntityCommandBlock);
public:
	TileEntityCommandBlock();
	~TileEntityCommandBlock();

	/** Sets the command this block will execute when powered. */
	void setCommand(const String& cmd);

	/** Return the command this command block is set to execute. */
	const String& getCommand() { return m_command; }

	/** Execute the command, called when the command block is powered. */
	int executeCommandOnPowered(World* pWorld);

	/** Gets the name of this command sender (usually username, but possibly "Rcon") */
	const String& getCommandSenderName() { return m_commandSenderName; }

	/** Sets the name of the command sender */
	void setCommandSenderName(const String& name) { m_commandSenderName = name; }

	int getSuccessCount() { return m_succesCount; }
	void setSuccessCount(int count) { m_succesCount = count; }

	/** implement override functions from ICommandSender */
	// virtual void sendChatToPlayer(ChatMessageComponent par1ChatMessageComponent) {}
	// virtual bool canCommandSenderUseCommand(int par1, String par2Str) { return par1 <= 2; }

	/** implement override functions from TileEntity */
	virtual void writeToNBT(NBTTagCompound* pNbt);
	virtual void readFromNBT(NBTTagCompound* pNbt);
	// virtual Packet getDescriptionPacket()

	/** Return the position for this command sender. */
	BlockPos getPlayerCoordinates() { return m_pos; }


protected:
	int m_succesCount = 0;

	/** The command this block will execute when powered. */
	String m_command; // = "";

					  /** The name of command sender (usually username, but possibly "Rcon") */
	String m_commandSenderName; // = "@";
};

class TileEntityDaylightDetector : public TileEntity
{
	RTTI_DECLARE(TileEntityDaylightDetector);
public:
	TileEntityDaylightDetector();
	virtual ~TileEntityDaylightDetector();

	/** implement override functions from TileEntity */
	virtual void updateEntity();
};

}

#include "TileEntitysNbtBindings.h"

#endif