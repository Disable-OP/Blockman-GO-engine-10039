/********************************************************************
filename: 	AttributeModifier.h
file path:	dev\client\Src\Blockman\Entity

version:	1
author:		ajohn
company:	supernano
date:		2017-2-14
*********************************************************************/
#ifndef __ATTRIBUTE_MODIFIER_HEADER__
#define __ATTRIBUTE_MODIFIER_HEADER__

#include "Util/UUID.h"
#include "Nbt/NBT.h"

namespace BLOCKMAN
{
class IAttributeInstance;
class IAttributeMap;

class AttributeModifier : public ObjectAlloc
{
protected:
	float m_amount = 0.f;
	int m_operation = 1;
	String m_name = "";
	UUID m_id;
	/** If false, this modifier is not saved in NBT. Used for "natural" modifiers like speed boost from sprinting */
	bool m_isSaved = false;

	bool m_needFree = false;

public:
	AttributeModifier(const String& nameIn, float amountIn, int operationIn);
	AttributeModifier(const UUID& idIn, const String& nameIn, float amountIn, int operationIn);
	const UUID& getID() { return m_id; }
	const String& getName() { return m_name; }
	int getOperation() { return m_operation; }
	float getAmount() {	return m_amount; }
	bool isSaved() { return m_isSaved; }
	void setSaved(bool saved) {	m_isSaved = saved; }
	bool operator ==(const AttributeModifier& rhs) const { return m_id == rhs.m_id; }
	int hashCode() { return m_id.hashCode(); }
	bool needFree() { return m_needFree;  }
	void setNeedFree(bool need) { m_needFree = need; }
	String toString();
};

typedef vector<AttributeModifier*>::type AttModifierArr;
typedef set<AttributeModifier*>::type AttModifierSet;
typedef map<String, AttributeModifier*>::type AttributeModifierMap;

class IAttribute : public ObjectAlloc
{
public:
	virtual const String& getName() const = 0;
	virtual float clampValue(float value) = 0;
	virtual float getDefaultValue() = 0;
	virtual bool getShouldWatch() = 0;
	virtual IAttribute* getParent() = 0;
};

class BaseAttribute : public IAttribute
{
private:
	IAttribute* parent = nullptr;
	String unlocalizedName = "";
	float defaultValue = 0.f;
	bool shouldWatch = false;

protected:
	BaseAttribute(IAttribute* parentIn, const String& unlocalizedNameIn, float defaultValueIn);

public:
	/** implement override functions */
	virtual const String& getName() const { return unlocalizedName; }
	virtual float getDefaultValue() { return defaultValue; }
	virtual bool getShouldWatch() { return shouldWatch; }
	virtual IAttribute* getParent() { return parent; }

	void setShouldWatch(bool shouldWatchIn) { shouldWatch = shouldWatchIn; }
	bool operator==(const BaseAttribute& rhs) const { return unlocalizedName == rhs.getName(); }
};

class RangedAttribute : public BaseAttribute
{
private:
	float minimumValue = 0.f;
	float maximumValue = 0.f;
	String description = "";

public:
	RangedAttribute(IAttribute* parentIn, const String& unlocalizedNameIn, float defaultValue, float minimumValueIn, float maximumValueIn);
	void setDescription(const String& descriptionIn) { description = descriptionIn; }
	const String& getDescription() { return description; }
	float clampValue(float value) { return Math::Clamp(value, minimumValue, maximumValue); }
};

class IAttributeInstance : public ObjectAlloc
{
public:
	/** Get the Attribute this is an instance of */
	virtual IAttribute* getAttribute() = 0;
	virtual float getBaseValue() = 0;
	virtual void setBaseValue(float baseValue) = 0;
	virtual AttModifierSet& getModifiersByOperation(int operation) = 0;
	virtual void getModifiers(AttModifierSet& setOut) = 0;
	virtual bool hasModifier(AttributeModifier* modifier) = 0;
	/** Returns attribute modifier, if any, by the given UUID */
	virtual AttributeModifier* getModifier(const UUID& uuid) = 0;
	virtual void applyModifier(AttributeModifier* modifier) = 0;
	virtual void removeModifier(AttributeModifier* modifier) = 0;
	virtual void removeModifier(const UUID& uuid) = 0;
	virtual void removeAllModifiers() = 0;
	virtual float getAttributeValue() = 0;
};

class ModifiableAttributeInstance : public IAttributeInstance
{
protected:
	/** The BaseAttributeMap this attributeInstance can be found in */
	IAttributeMap* attributeMap = nullptr;
	/** The Attribute this is an instance of */
	IAttribute* genericAttribute = nullptr;
	
	map<int, AttModifierSet>::type mapByOperation;
	map<String, AttModifierSet>::type mapByName;
	map<UUID, AttributeModifier*>::type mapByUUID;;
	float baseValue = 0.f;
	bool needsUpdate = true; // = true;
	float cachedValue = 0.f;

public:
	ModifiableAttributeInstance(IAttributeMap* attributeMapIn, IAttribute* genericAttributeIn);

	/** implement override functions */
	virtual IAttribute* getAttribute() { return genericAttribute; }
	virtual float getBaseValue() { return baseValue; }
	virtual void setBaseValue(float baseValue);
	virtual AttModifierSet& getModifiersByOperation(int operation);
	virtual void getModifiers(AttModifierSet& setOut);
	virtual AttributeModifier* getModifier(const UUID& uuid);
	virtual bool hasModifier(AttributeModifier* modifier);
	virtual void applyModifier(AttributeModifier* modifier);
	virtual void removeModifier(AttributeModifier* modifier);
	virtual void removeModifier(const UUID& uuid);
	virtual void removeAllModifiers();
	virtual float getAttributeValue();

	void flagForUpdate();
	float computeValue();
	void getAppliedModifiers(int operation, AttModifierSet& collection);
};

class IAttributeMap : public ObjectAlloc
{
public:
	typedef map<IAttribute*, IAttributeInstance*>::type AttributeInsMap;
	typedef map<String, IAttributeInstance*>::type		AttributeInsMapByName;
	typedef multimap<IAttribute*, IAttribute*>::type	AttributeMultiMap;
	typedef map<String, AttributeModifier*>::type		AttrbuteModifierMap;

protected:
	AttributeInsMap attributes;
	AttributeInsMapByName attributesByName;
	AttributeMultiMap descendantsByParent;

protected:
	virtual IAttributeInstance* createInstance(IAttribute* attribute) = 0;

public:
	virtual ~IAttributeMap();
	virtual IAttributeInstance* getAttributeInstance(IAttribute* attribute);
	virtual IAttributeInstance* getAttributeInstanceByName(const String& attributeName);
	virtual IAttributeInstance* registerAttribute(IAttribute* attribute);
	AttributeInsMapByName& getAllAttributes() { return attributesByName; }

	virtual void onAttributeModified(IAttributeInstance* instance) { }

	void removeAttributeModifiers(const AttrbuteModifierMap& modifiers);
	void applyAttributeModifiers(const AttrbuteModifierMap& modifiers);
};

class AttributeMap : public IAttributeMap
{
protected:
	set<IAttributeInstance*>::type	attributeInstanceSet;
	AttributeInsMapByName			descriptionToAttributeInstanceMap;

protected:
	virtual IAttributeInstance* createInstance(IAttribute* attribute);

public:
	virtual ModifiableAttributeInstance* getAttributeInstance(IAttribute* attribute);
	virtual ModifiableAttributeInstance* getAttributeInstanceByName(const String& attributeName);
	virtual IAttributeInstance* registerAttribute(IAttribute* attribute);
	virtual void onAttributeModified(IAttributeInstance* instance);
	
	set<IAttributeInstance*>::type& getAttributeInstanceSet() { return attributeInstanceSet; }
	void getWatchedAttributes(set<IAttributeInstance*>::type& outset);
};

class SharedMonsterAttributes
{
public:
	static IAttribute* EAMAX_HEALTH;		// = (new RangedAttribute("generic.maxHealth", 20.0D, 0.0D, Double.MAX_VALUE)).func_111117_a("Max Health").func_111112_a(true);
	static IAttribute* FOLLOW_RANGE;		// = (new RangedAttribute("generic.followRange", 32.0D, 0.0D, 2048.0D)).func_111117_a("Follow Range");
	static IAttribute* KNOCKBACK_RESISTANCE;// = (new RangedAttribute("generic.knockbackResistance", 0.0D, 0.0D, 1.0D)).func_111117_a("Knockback Resistance");
	static IAttribute* MOVEMENT_SPEED;		// = (new RangedAttribute("generic.movementSpeed", 0.699999988079071D, 0.0D, Double.MAX_VALUE)).func_111117_a("Movement Speed").func_111112_a(true);
	static IAttribute* ATTACK_DAMAGE;		// = new RangedAttribute("generic.attackDamage", 2.0D, 0.0D, Double.MAX_VALUE);

public:
	static NBTTagList* writeBaseAttributeMapToNBT(IAttributeMap* attributeMap);
	static NBTTagCompound* writeAttributeModifierToNBT(AttributeModifier* modifer);
	static void setAttributeModifiers(IAttributeMap* attributemap, NBTTagList* pNbtTag);
	static AttributeModifier* readAttributeModifierFromNBT(NBTTagCompound* compound);

	static void initialize();
	static void unInitialize();

protected:
	static NBTTagCompound* writeAttributeInstanceToNBT(IAttributeInstance* pAttributeInstance);
	static void applyModifiersToAttributeInstance(IAttributeInstance* instance, NBTTagCompound* compound);
};

}
#endif