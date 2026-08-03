#include "ItemStack.h"

#include "Entity/Enchantment.h"
#include "Entity/EntityLivingBase.h"
#include "Entity/EntityPlayer.h"
#include "Block/Block.h"
#include "Item/Item.h"
#include "Item/Items.h"
#include "Nbt/NBT.h"
#include "Stats/Stats.h"
#include "Stats/StatList.h"
#include "Inventory/InventoryPlayer.h"
#include "Setting/LogicSetting.h"
#include "World/World.h"
#include "Script/Event/LogicScriptEvents.h"

namespace BLOCKMAN
{

ItemStack::ItemStack()
	: itemID(0)
	, animationsToGo(0)
	, stackSize(0)
	, itemDamage(0)
	, subItemID(0)
	, stackTagCompound(NULL)
	, m_currentClipBulletNum(0)
	, m_totalBulletNum(0)
	, m_cd(0.0f)
{}

ItemStack::ItemStack(Block* pBlock, int size, int meta)
	: itemID(pBlock->getItemBlockId())
	, animationsToGo(0)
	, stackSize(size)
	, itemDamage(meta)
	, subItemID(meta)
	, stackTagCompound(NULL)
	, m_currentClipBulletNum(0)
	, m_totalBulletNum(0)
	, m_cd(0.0f)
{}

ItemStack::ItemStack(Item* item, int size, int meta)
	: animationsToGo(0)
	, stackSize(size)
	, itemDamage(meta)
	, subItemID(meta)
	, stackTagCompound(NULL)
	, m_currentClipBulletNum(0)
	, m_totalBulletNum(0)
	, m_cd(0.0f)
{
	if (!item)
	{
		LordLogError("Attention!!! Intend to Init ItemStack by nullptr item! make its id as 256!!");
		itemID = 256;
	}
	else 
	{
		itemID = item->itemID;
	}
}

ItemStack::ItemStack(int id, int size, int meta)
	: itemID(id)
	, animationsToGo(0)
	, stackSize(size)
	, itemDamage(meta)
	, subItemID(meta)
	, stackTagCompound(NULL)
	, m_currentClipBulletNum(0)
	, m_totalBulletNum(0)
	, m_cd(0.0f)
{
}

ItemStack::ItemStack(int id, int subId, int size, int damage)
	: itemID(id)
	, subItemID(subId)
	, animationsToGo(0)
	, stackSize(size)
	, itemDamage(damage)
	, stackTagCompound(NULL)
	, m_currentClipBulletNum(0)
	, m_totalBulletNum(0)
	, m_cd(0.0f)
{}

//ItemStack::ItemStack(const ItemStack& rhs)
//	: itemID(rhs.itemID)
//	, animationsToGo(rhs.animationsToGo)
//	, stackSize(rhs.stackSize)
//	, itemDamage(rhs.itemDamage)
//	, stackTagCompound(NULL)
//{}

ItemStackPtr ItemStack::loadItemStackFromNBT(NBTTagCompound* pNbtCompound)
{
	ItemStackPtr pStack = LORD::make_shared<ItemStack>();
	pStack->readFromNBT(pNbtCompound);
	return pStack->getItem() != NULL ? pStack : NULL;
}

ItemStackPtr ItemStack::splitStack(int count)
{
	ItemStackPtr pStack = LORD::make_shared<ItemStack>(itemID, count, itemDamage);

	if (stackTagCompound)
	{
		pStack->stackTagCompound = (NBTTagCompound*)stackTagCompound->copy();
	}

	stackSize -= count;
	return pStack;
}

Item* ItemStack::getItem()
{
	return Item::getItemById(itemID);
}

const BulletClipSetting * ItemStack::getBulletClipSetting()
{
	const GunSetting* pGunSetting = getGunSetting();
	if(!pGunSetting)
		return nullptr;

	ItemBulletClip* pBulletClip = dynamic_cast<ItemBulletClip*>(Item::itemsList[pGunSetting->adaptedBulletId]);
	if (!pBulletClip)
		return nullptr;

	return pBulletClip->getBulletClipSetting();
}

const GunSetting * ItemStack::getGunSetting()
{
	Item* pItem = Item::getItemById(itemID);
	ItemGun* pGun = dynamic_cast<ItemGun*>(pItem);
	if (!pGun)
		return nullptr;

	return pGun->getGunSetting();
}

bool ItemStack::tryPlaceItemIntoWorld(EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit)
{
	Item* pItem = getItem();

	int size = stackSize;

	bool success = pItem->onItemUse(shared_from_this(), pPlayer, pWorld, pos, face, hit);

	if (success)
	{
		if (!pWorld->m_isClient && !SCRIPT_EVENT::PlacingConsumeBlockEvent::invoke(itemID, itemDamage))
		{
			stackSize = size;
		}

		pPlayer->addStat(StatList::objectUseStats[itemID], 1);
	}

	return success;
}

float ItemStack::getStrVsBlock(Block* pBlock)
{
	return getItem()->getStrVsBlock(shared_from_this(), pBlock);
}

ItemStackPtr ItemStack::useItemRightClick(World* pWorld, EntityPlayer* pPlayer)
{
	return getItem()->onItemRightClick(shared_from_this(), pWorld, pPlayer);
}

ItemStackPtr ItemStack::onFoodEaten(World* pWorld, EntityPlayer* pPlayer)
{
	return getItem()->onEaten(shared_from_this(), pWorld, pPlayer);
}

NBTTagCompound* ItemStack::writeToNBT(NBTTagCompound* pCompound)
{
	pCompound->setShort("id", (i16)itemID);
	pCompound->setByte("Count", (i8)stackSize);
	pCompound->setShort("Damage", (i16)itemDamage);

	if (stackTagCompound != NULL)
	{
		pCompound->setTag("tag", stackTagCompound);
	}

	return pCompound;
}

void ItemStack::readFromNBT(NBTTagCompound* pCompound)
{
	itemID = pCompound->getShort("id");
	stackSize = pCompound->getByte("Count");
	itemDamage = pCompound->getBool("Damage");

	if (itemDamage < 0)
		itemDamage = 0;

	if (pCompound->hasKey("tag"))
		stackTagCompound = pCompound->getCompoundTag("tag");
}

int ItemStack::getMaxStackSize()
{
	if(getItem())
		return getItem()->getItemStackLimit();
	return 0;
}

bool ItemStack::isStackable()
{
	return getMaxStackSize() > 1 && (!isItemStackDamageable() || !isItemDamaged());
}

bool ItemStack::isItemStackDamageable()
{
	Item* pItem = getItem();
	if (!pItem)
		return false;

	return pItem->getMaxDamage() > 0;
}

bool ItemStack::getHasSubtypes()
{
	Item* pItem = getItem();
	if (!pItem)
		return false;

	return pItem->getHasSubtypes();
}

bool ItemStack::isItemDamaged()
{
	return isItemStackDamageable() && itemDamage > 0;
}

void ItemStack::setItemDamage(int par1)
{
	itemDamage = par1;

	if (itemDamage < 0)
	{
		itemDamage = 0;
	}
}

int ItemStack::getMaxDamage()
{
	Item* pItem = getItem();
	if (!pItem)
		return 0;

	return pItem->getMaxDamage();
}

bool ItemStack::attemptDamageItem(int damage, Random* pRandom)
{
	if (!isItemStackDamageable())
	{
		return false;
	}
	else
	{
		if (damage > 0)
		{
			int var3 = EnchantmentHelper::getEnchantmentLevel(Enchantment::unbreaking->effectId, shared_from_this());
			int var4 = 0;

			for (int i = 0; var3 > 0 && i < damage; ++i)
			{
				if (EnchantmentDurability::negateDamage(shared_from_this(), var3, pRandom))
				{
					++var4;
				}
			}

			damage -= var4;

			if (damage <= 0)
			{
				return false;
			}
		}

		itemDamage += damage;
		return itemDamage >= getMaxDamage();
	}
}

void ItemStack::damageItem(int damage, EntityLivingBase* pLiving)
{
	EntityPlayer* pplayer = dynamic_cast<EntityPlayer*>(pLiving);
	if (!pplayer || !pplayer->capabilities.isCreativeMode)
	{
		if (!LogicSetting::Instance()->isCanDamageItem())
		{
			return;
		}
		if (isItemStackDamageable())
		{
			if (attemptDamageItem(damage, pLiving->getRNG()))
			{
				pLiving->renderBrokenItemStack(shared_from_this());
				--stackSize;

				if (pplayer)
				{
					pplayer->addStat(StatList::objectBreakStats[itemID], 1);

					ItemBow* pBow = dynamic_cast<ItemBow*>(getItem());
					if (stackSize == 0 && pBow)
					{
						pplayer->destroyCurrentEquippedItem();
					}
				}

				if (stackSize < 0)
				{
					stackSize = 0;
				}

				itemDamage = 0;
			}
		}
	}
}

void ItemStack::hitEntity(EntityLivingBase* pLiving, EntityPlayer* pPlayer)
{
	Item* pItem = getItem();
	if (!pItem)
		return;

	bool hited = pItem->hitEntity(shared_from_this(), pLiving, pPlayer);

	if (hited)
	{
		pPlayer->addStat(StatList::objectUseStats[itemID], 1);
	}
}

void ItemStack::onBlockDestroyed(World* pWorld, int blockID, const BlockPos& pos, EntityPlayer* pPlayer)
{
	Item* pItem = getItem();
	if (!pItem)
		return;

	bool destroyed = pItem->onBlockDestroyed(shared_from_this(), pWorld, blockID, pos, pPlayer);

	if (destroyed)
	{
		pPlayer->addStat(StatList::objectUseStats[itemID], 1);
	}
}

bool ItemStack::canHarvestBlock(Block* pBlock)
{
	Item* pItem = getItem();
	if (!pItem)
		return false;

	return pItem->canHarvestBlock(pBlock);
}

bool ItemStack::interactWithEntity(EntityPlayer* pPlayer, EntityLivingBase* pLiving)
{
	Item* pItem = getItem();
	if (!pItem)
		return false;

	return pItem->interactWithEntity(shared_from_this(), pPlayer, pLiving);
}

ItemStackPtr ItemStack::copy()
{
	ItemStackPtr pStack = LORD::make_shared<ItemStack>(itemID, stackSize, itemDamage);

	if (pStack)
	{
		pStack->setCurrentClipBulletNum(m_currentClipBulletNum);
		pStack->m_needAdjustStackIndex = m_needAdjustStackIndex;
	}
	
	if (pStack && pStack->getGunSetting()){
		pStack->setTotalBulletNum(m_totalBulletNum);
		if (!LogicSetting::Instance()->getGunIsNeedBulletStatus())
		{
			pStack->setCurrentClipBulletNum(m_currentClipBulletNum);
		}
	}
	if (stackTagCompound != NULL)
	{
		pStack->stackTagCompound = (NBTTagCompound*)stackTagCompound->copy();
	}

	return pStack;
}

bool ItemStack::areItemStackTagsEqual(ItemStackPtr pStack1, ItemStackPtr pStack2)
{
	return pStack1 == NULL && pStack2 == NULL ? true :
		(pStack1 != NULL && pStack2 != NULL ? 
			(pStack1->stackTagCompound == NULL && pStack2->stackTagCompound != NULL ? false : 
				pStack1->stackTagCompound == NULL || pStack1->stackTagCompound->equals(pStack2->stackTagCompound)) : false);
}

bool ItemStack::areItemStacksEqual(ItemStackPtr pStack1, ItemStackPtr pStack2)
{
	return pStack1 == NULL && pStack2 == NULL ? true : (pStack1 != NULL && pStack2 != NULL ? pStack1->isItemStackEqual(pStack2) : false);
}

bool ItemStack::reloadBulletClip(int canAddBulletNum)
{
	const BulletClipSetting* pBulletSetting = getBulletClipSetting();
	if (!pBulletSetting)
		return false;

	if (m_currentClipBulletNum >= m_totalBulletNum || canAddBulletNum <= 0) {
		return false;
	}

	m_currentClipBulletNum += canAddBulletNum;
	return true;
}

bool ItemStack::consumeBullet()
{
	if (m_currentClipBulletNum < 1)
		return false;

	m_currentClipBulletNum--;
	return true;
}

bool ItemStack::isItemStackEqual(ItemStackPtr pStack)
{
	return stackSize != pStack->stackSize ? false :
		(itemID != pStack->itemID ? false : 
			(itemDamage != pStack->itemDamage ? false :
				(stackTagCompound == NULL && pStack->stackTagCompound != NULL ? false :
					stackTagCompound == NULL || stackTagCompound->equals(pStack->stackTagCompound))));
}

bool ItemStack::isItemEqual(ItemStackPtr pStack)
{
	return itemID == pStack->itemID && itemDamage == pStack->itemDamage;
}

String ItemStack::getItemName()
{
	Item* pItem = getItem();
	if (!pItem)
		return "";

	return pItem->getUnlocalizedName(shared_from_this());
}

ItemStackPtr ItemStack::copyItemStack(ItemStackPtr pStack)
{
	return pStack == NULL ? NULL : pStack->copy();
}

String ItemStack::toString()
{
	Item* pItem = getItem();
	if (!pItem)
		return "";

	return StringUtil::ToString(stackSize) + "x" + pItem->getUnlocalizedName()
		+ "@" + StringUtil::ToString(itemDamage);
}

void ItemStack::updateAnimation(World* pWorld, Entity* pEntity, int slot, bool isCurrentItem)
{
	Item* pItem = getItem();
	if (!pItem)
		return;

	if (animationsToGo > 0)
	{
		--animationsToGo;
	}

	pItem->onUpdate(shared_from_this(), pWorld, pEntity, slot, isCurrentItem);
}

void ItemStack::onCrafting(World* pWorld, EntityPlayer* pPlayer, int amount)
{
	pPlayer->addStat(StatList::objectCraftStats[itemID], amount);
	getItem()->onCreated(shared_from_this(), pWorld, pPlayer);
}

int ItemStack::getMaxItemUseDuration()
{
	return getItem()->getMaxItemUseDuration(shared_from_this());
}

ACTION_TYPE ItemStack::getItemUseAction()
{
	return getItem()->getItemUseAction(shared_from_this());
}

void ItemStack::onPlayerStoppedUsing(World* pWorld, EntityPlayer* pPlayer, int timeLeft)
{
	getItem()->onPlayerStoppedUsing(shared_from_this(), pWorld, pPlayer, timeLeft);
}

NBTTagList* ItemStack::getEnchantmentTagList()
{
	return stackTagCompound == NULL ? NULL : (NBTTagList*)stackTagCompound->getTag("ench");
}

String ItemStack::getDisplayName()
{
	String strResult = getItem()->getItemDisplayName(shared_from_this());

	if (stackTagCompound != NULL && stackTagCompound->hasKey("display"))
	{
		NBTTagCompound* pCompound = stackTagCompound->getCompoundTag("display");

		if (pCompound->hasKey("Name"))
		{
			strResult = pCompound->getString("Name");
		}
	}

	return strResult;
}

void ItemStack::setItemName(const String& name)
{
	if (stackTagCompound == NULL)
	{
		stackTagCompound = LordNew NBTTagCompound("tag");
	}

	if (!stackTagCompound->hasKey("display"))
	{
		stackTagCompound->setCompoundTag("display", LordNew NBTTagCompound());
	}

	stackTagCompound->getCompoundTag("display")->setString("Name", name);
}

void ItemStack::clearCustomName()
{
	if (!stackTagCompound)
		return;
	
	if (stackTagCompound->hasKey("display"))
	{
		NBTTagCompound* pCompound = stackTagCompound->getCompoundTag("display");
		pCompound->removeTag("Name");

		if (pCompound->hasNoTags())
		{
			stackTagCompound->removeTag("display");

			if (stackTagCompound->hasNoTags())
			{
				LordDelete(stackTagCompound);
				setTagCompound(NULL);
			}
		}
	}
}

bool ItemStack::hasDisplayName()
{
	return stackTagCompound == NULL ? false : 
		(!stackTagCompound->hasKey("display") ? false :
			stackTagCompound->getCompoundTag("display")->hasKey("Name"));
}

StringArray ItemStack::getTooltip(EntityPlayer* pPlayer, bool advanced)
{
	StringArray result;
	/*Item* pStack = Item::itemsList[itemID];
	String var5 = getDisplayName();

	if (hasDisplayName())
	{
		var5 = EnumChatFormatting.ITALIC + var5 + EnumChatFormatting.RESET;
	}

	if (advanced)
	{
		String var6 = "";

		if (var5.length() > 0)
		{
			var5 = var5 + " (";
			var6 = ")";
		}

		if (getHasSubtypes())
		{
			var5 = var5 + String.format("#%04d/%d%s", new Object[]{ Integer.valueOf(itemID), Integer.valueOf(itemDamage), var6 });
		}
		else
		{
			var5 = var5 + String.format("#%04d%s", new Object[]{ Integer.valueOf(itemID), var6 });
		}
	}
	else if (!hasDisplayName() && itemID == Item.map.itemID)
	{
		var5 = var5 + " #" + itemDamage;
	}

	result.add(var5);
	
	if (hasTagCompound())
	{
		NBTTagList var14 = getEnchantmentTagList();

		if (var14 != null)
		{
			for (int var7 = 0; var7 < var14.tagCount(); ++var7)
			{
				short var8 = ((NBTTagCompound)var14.tagAt(var7)).getShort("id");
				short var9 = ((NBTTagCompound)var14.tagAt(var7)).getShort("lvl");

				if (Enchantment.enchantmentsList[var8] != null)
				{
					result.add(Enchantment.enchantmentsList[var8].getTranslatedName(var9));
				}
			}
		}

		if (stackTagCompound.hasKey("display"))
		{
			NBTTagCompound var16 = stackTagCompound.getCompoundTag("display");

			if (var16.hasKey("color"))
			{
				if (advanced)
				{
					result.add("Color: #" + Integer.toHexString(var16.getInteger("color")).toUpperCase());
				}
				else
				{
					result.add(EnumChatFormatting.ITALIC + StatCollector.translateToLocal("item.dyed"));
				}
			}

			if (var16.hasKey("Lore"))
			{
				NBTTagList var18 = var16.getTagList("Lore");

				if (var18.tagCount() > 0)
				{
					for (int var20 = 0; var20 < var18.tagCount(); ++var20)
					{
						result.add(EnumChatFormatting.DARK_PURPLE + "" + EnumChatFormatting.ITALIC + ((NBTTagString)var18.tagAt(var20)).data);
					}
				}
			}
		}
	}

	Multimap var15 = func_111283_C();

	if (!var15.isEmpty())
	{
		result.add("");
		Iterator var17 = var15.entries().iterator();

		while (var17.hasNext())
		{
			Entry var19 = (Entry)var17.next();
			AttributeModifier var21 = (AttributeModifier)var19.getValue();
			double var10 = var21.func_111164_d();
			double var12;

			if (var21.func_111169_c() != 1 && var21.func_111169_c() != 2)
			{
				var12 = var21.func_111164_d();
			}
			else
			{
				var12 = var21.func_111164_d() * 100.0D;
			}

			if (var10 > 0.0D)
			{
				result.add(EnumChatFormatting.BLUE + StatCollector.translateToLocalFormatted("attribute.modifier.plus." + var21.func_111169_c(), new Object[]{ field_111284_a.format(var12), StatCollector.translateToLocal("attribute.name." + (String)var19.getKey()) }));
			}
			else if (var10 < 0.0D)
			{
				var12 *= -1.0D;
				result.add(EnumChatFormatting.RED + StatCollector.translateToLocalFormatted("attribute.modifier.take." + var21.func_111169_c(), new Object[]{ field_111284_a.format(var12), StatCollector.translateToLocal("attribute.name." + (String)var19.getKey()) }));
			}
		}
	}

	if (advanced && isItemDamaged())
	{
		result.add("Durability: " + (getMaxDamage() - getItemDamageForDisplay()) + " / " + getMaxDamage());
	}*/

	return result;
}

bool ItemStack::hasEffect()
{
	return getItem()->hasEffect(shared_from_this());
}

RARITY_TYPE ItemStack::getRarity()
{
	return getItem()->getRarity(shared_from_this());
}

bool ItemStack::isItemEnchantable()
{
	return !getItem()->isItemTool(shared_from_this()) ? false : !isItemEnchanted();
}

void ItemStack::addEnchantmentbyId(int id, int level)
{
	Enchantment *data = Enchantment::getEnchantmentById(id);
	if(data != nullptr)
		this->addEnchantment(data, level);
}

void ItemStack::addEnchantment(Enchantment* enchantment, int level)
{
	if (stackTagCompound == NULL)
	{
		setTagCompound(LordNew NBTTagCompound());
	}

	if (!stackTagCompound->hasKey("ench"))
	{
		stackTagCompound->setTag("ench", LordNew NBTTagList("ench"));
	}

	NBTTagList* pNbtEnch = (NBTTagList*)stackTagCompound->getTag("ench");
	NBTTagCompound* pCompoundEnch = LordNew NBTTagCompound();
	pCompoundEnch->setShort("id", (short)enchantment->effectId);
	pCompoundEnch->setShort("lvl", (short)((i8)level));
	pNbtEnch->appendTag(pCompoundEnch);
	m_enchantmentAddedEvent.emit();
}

bool ItemStack::isItemEnchanted()
{
	return stackTagCompound != NULL && stackTagCompound->hasKey("ench");
}

void ItemStack::setTagInfo(const String& name, NBTBase* pNbt)
{
	if (stackTagCompound == NULL)
	{
		setTagCompound(LordNew NBTTagCompound());
	}

	stackTagCompound->setTag(name, pNbt);
}

bool ItemStack::canEditBlocks()
{
	return getItem()->canItemEditBlocks();
}

int ItemStack::getRepairCost()
{
	return hasTagCompound() &&
		stackTagCompound->hasKey("RepairCost") ? stackTagCompound->getInteger("RepairCost") : 0;
}

void ItemStack::setRepairCost(int cost)
{
	if (!hasTagCompound())
	{
		stackTagCompound = LordNew NBTTagCompound("tag");
	}

	stackTagCompound->setInteger("RepairCost", cost);
}

void ItemStack::getAttributeModifiers(AttributeModifierMap& outmap)
{
	if (hasTagCompound() && stackTagCompound->hasKey("AttributeModifiers"))
	{
		NBTTagList* nbtAttributes = stackTagCompound->getTagList("AttributeModifiers");

		for (int i = 0; i < nbtAttributes->tagCount(); ++i)
		{
			NBTTagCompound* pCompound = (NBTTagCompound*)nbtAttributes->tagAt(i);
			AttributeModifier* modifier = SharedMonsterAttributes::readAttributeModifierFromNBT(pCompound);

			if (modifier->getID().getLeastSignificantBits() != 0L &&
				modifier->getID().getMostSignificantBits() != 0L)
			{
				outmap.insert(std::make_pair(pCompound->getString("AttributeName"), modifier));
			}
		}
	}
	else
	{
		if(getItem())
			getItem()->getAttributeModifiers(outmap);
	}
}

ItemStackInfo ItemStack::getItemStackInfo()
{
	m_itemStackInfo.id = this->itemID;
	m_itemStackInfo.meta = this->itemDamage;
	m_itemStackInfo.num = this->stackSize;
	m_itemStackInfo.bullets = this->m_currentClipBulletNum;
	return m_itemStackInfo;
}

void ItemStack::updateCdTime(float fDeltaTime)
{
	if (m_cd > 0.0f)
	{
		m_cd -= fDeltaTime;
	}
}

bool ItemStack::isGunItem()
{
	return GunSetting::isGunItem(itemID);
}

bool ItemStack::isThrowItemSkill()
{
	return SkillItemSetting::isThrowSkillItem(itemID);
}

bool ItemStack::isItemSkill()
{
	return SkillItemSetting::isSkillItem(itemID);
}

bool ItemStack::isAutoCastSkill() 
{
	return SkillItemSetting::isAutoCastSkill(itemID);
}

bool ItemStack::isShowCrossHair() 
{
	return SkillItemSetting::isShowCrossHair(itemID);
}

bool ItemStack::isItemSkillBook()
{
	auto book = dynamic_cast<ItemSkillBook*>(getItem());
	return book != NULL;
}

bool ItemStack::isFirstRender()
{
	auto item = getItem();
	if (item)
	{
		return item->isFirstRender();
	}
	return false;
}

bool ItemStack::isThirdRender()
{
	auto item = getItem();
	if (item)
	{
		return item->isThirdRender();
	}
	return false;
}


}
