#include "Attributes.h"

namespace BLOCKMAN
{

AttributeModifier::AttributeModifier(const String& nameIn, float amountIn, int operationIn)
	: m_amount(amountIn)
	, m_operation(operationIn)
	, m_name(nameIn)
	, m_id(UUID::randomUUID())
	, m_isSaved(false)
	, m_needFree(false)
{
	LordAssert(m_name != StringUtil::BLANK);
	LordAssert(m_operation >= 0 && m_operation <= 2);
}

AttributeModifier::AttributeModifier(const UUID& idIn, const String& nameIn, float amountIn, int operationIn)
	: m_amount(amountIn)
	, m_operation(operationIn)
	, m_name(nameIn)
	, m_id(idIn)
	, m_isSaved(false)
	, m_needFree(false)
{
	LordAssert(m_name != StringUtil::BLANK);
	LordAssert(m_operation >= 0 && m_operation <= 2);
}

String AttributeModifier::toString()
{
	return StringUtil::Format("AttributeModifier{ amount=%f, operation=%d, name=\'%s\', id=%s, serialize=%d}",
		m_amount, m_operation, m_name.c_str(), m_id.toString().c_str(), m_isSaved ? "true" : "false");
}

BaseAttribute::BaseAttribute(IAttribute* parentIn, const String& unlocalizedNameIn, float defaultValueIn)
	: parent(parentIn)
	, unlocalizedName(unlocalizedNameIn)
	, defaultValue(defaultValueIn)
{
	LordAssert(unlocalizedName != StringUtil::BLANK);
}

RangedAttribute::RangedAttribute(IAttribute* parentIn, const String& unlocalizedNameIn, float defaultValue, float minimumValueIn, float maximumValueIn)
	: BaseAttribute(parentIn, unlocalizedNameIn, defaultValue)
	, minimumValue(minimumValueIn)
	, maximumValue(maximumValueIn)
{
	LordAssert(minimumValue <= maximumValue);
	LordAssert(defaultValue >= minimumValue);
	LordAssert(defaultValue <= maximumValue);
}

ModifiableAttributeInstance::ModifiableAttributeInstance(IAttributeMap* attributeMapIn, IAttribute* genericAttributeIn)
	: attributeMap(attributeMapIn)
	, genericAttribute(genericAttributeIn)
	, needsUpdate(true)
{
	baseValue = genericAttributeIn->getDefaultValue();

	AttModifierSet emptySet;
	for (int i = 0; i < 3; ++i)
		mapByOperation.insert(std::make_pair(i, emptySet));
}

void ModifiableAttributeInstance::setBaseValue(float _baseValue)
{
	if (_baseValue != getBaseValue())
	{
		baseValue = _baseValue;
		flagForUpdate();
	}
}

AttModifierSet& ModifiableAttributeInstance::getModifiersByOperation(int operation)
{
	map<int, AttModifierSet>::type::iterator it = mapByOperation.find(operation);
	LordAssert(it != mapByOperation.end());
	return it->second;
}

void ModifiableAttributeInstance::getModifiers(AttModifierSet& setOut)
{
	setOut.clear();

	for (int i = 0; i < 3; ++i)
	{
		AttModifierSet& iset = getModifiersByOperation(i);
		setOut.insert(iset.begin(), iset.end());
	}
}

void ModifiableAttributeInstance::flagForUpdate()
{
	needsUpdate = true;
	attributeMap->onAttributeModified(this);
}

AttributeModifier* ModifiableAttributeInstance::getModifier(const UUID& uuid)
{
	map<UUID, AttributeModifier*>::type::iterator it = mapByUUID.find(uuid);
	if (it != mapByUUID.end())
		return it->second;
	return NULL;
}

bool ModifiableAttributeInstance::hasModifier(AttributeModifier* modifier)
{
	return mapByUUID.find(modifier->getID()) != mapByUUID.end();
}

void ModifiableAttributeInstance::applyModifier(AttributeModifier* modifier)
{
	LordAssert(!hasModifier(modifier));

	map<String, AttModifierSet>::type::iterator it = mapByName.find(modifier->getName());
	if (it == mapByName.end())
	{
		AttModifierSet emptyset;
		mapByName.insert(std::make_pair(modifier->getName(), emptyset));
	}

	mapByOperation[modifier->getOperation()].insert(modifier);
	mapByName[modifier->getName()].insert(modifier);
	mapByUUID.insert(std::make_pair(modifier->getID(), modifier));
	flagForUpdate();
}

void ModifiableAttributeInstance::removeModifier(AttributeModifier* modifier)
{
	for (int i = 0; i < 3; ++i)
	{
		for (auto it = mapByOperation[i].begin(); it != mapByOperation[i].end(); ) {
			if ((*it)->getID() == modifier->getID()) {
				if ((*it)->needFree()) {
					LordDelete((*it));
				}
				it = mapByOperation[i].erase(it);
			}
			else {
				it++;
			}
		}
	}

	map<String, AttModifierSet>::type::iterator it = mapByName.find(modifier->getName());

	if (it != mapByName.end())
	{
		it->second.erase(modifier);

		if (it->second.empty())
			mapByName.erase(modifier->getName());
	}

	mapByUUID.erase(modifier->getID());
	flagForUpdate();
}

void ModifiableAttributeInstance::removeModifier(const UUID& uuid)
{
	AttributeModifier* attributemodifier = getModifier(uuid);

	if (attributemodifier != NULL)
		removeModifier(attributemodifier);
}

void ModifiableAttributeInstance::removeAllModifiers()
{
	AttModifierSet collection;
	getModifiers(collection);

	for (AttModifierSet::iterator it = collection.begin(); it != collection.end(); ++it)
	{
		removeModifier(*it);
	}
}

float ModifiableAttributeInstance::getAttributeValue()
{
	if (needsUpdate)
	{
		cachedValue = computeValue();
		needsUpdate = false;
	}

	return cachedValue;
}

float ModifiableAttributeInstance::computeValue()
{
	float d0 = getBaseValue();

	AttModifierSet attModifierset;
	getAppliedModifiers(0, attModifierset);
	for (AttModifierSet::iterator it = attModifierset.begin(); it != attModifierset.end(); ++it)
	{
		d0 += (*it)->getAmount();
	}

	float d1 = d0;
	getAppliedModifiers(1, attModifierset);
	for (AttModifierSet::iterator it = attModifierset.begin(); it != attModifierset.end(); ++it)
	{
		d1 += d0 * (*it)->getAmount();
	}
	getAppliedModifiers(2, attModifierset);
	for (AttModifierSet::iterator it = attModifierset.begin(); it != attModifierset.end(); ++it)
	{
		d1 *= 1.0f + (*it)->getAmount();
	}

	return genericAttribute->clampValue(d1);
}

void ModifiableAttributeInstance::getAppliedModifiers(int operation, AttModifierSet& collection)
{
	collection.clear();

	AttModifierSet& collection1 = getModifiersByOperation(operation);
	collection.insert(collection1.begin(), collection1.end());
	/*
	for (IAttribute* iattribute = genericAttribute->getParent(); iattribute != NULL; iattribute = iattribute->getParent())
	{
		IAttributeInstance* iattributeinstance = attributeMap->getAttributeInstance(iattribute);

		if (iattributeinstance != NULL)
		{
			AttModifierSet& collection1 = iattributeinstance->getModifiersByOperation(operation);
			collection.insert(collection1.begin(), collection1.end());
		}
	}
	*/
}

IAttributeMap::~IAttributeMap()
{
	for (AttributeInsMap::iterator it = attributes.begin(); it != attributes.end(); ++it)
	{
		if (it->second)
			LordDelete(it->second);
	}
}

IAttributeInstance* IAttributeMap::getAttributeInstance(IAttribute* attribute)
{
	AttributeInsMap::iterator it = attributes.find(attribute);
	if (it != attributes.end())
		return it->second;
	return NULL;
}

IAttributeInstance* IAttributeMap::getAttributeInstanceByName(const String& attributeName)
{
	AttributeInsMapByName::iterator it = attributesByName.find(attributeName);
	if (it != attributesByName.end())
		return it->second;
	return NULL;
}

IAttributeInstance* IAttributeMap::registerAttribute(IAttribute* attribute)
{
	LordAssert(attributesByName.find(attribute->getName()) == attributesByName.end());

	IAttributeInstance* iattributeinstance = createInstance(attribute);

	attributesByName.insert(std::make_pair(attribute->getName(), iattributeinstance));
	attributes.insert(std::make_pair(attribute, iattributeinstance));
	for (IAttribute* iattribute = attribute->getParent(); iattribute != NULL; iattribute = iattribute->getParent())
	{
		descendantsByParent.insert(std::make_pair(iattribute, attribute));
	}

	return iattributeinstance;
}

void IAttributeMap::removeAttributeModifiers(const AttrbuteModifierMap& modifiers)
{
	for (AttrbuteModifierMap::const_iterator it = modifiers.begin(); it != modifiers.end(); ++it)
	{
		IAttributeInstance* attributeinstance = getAttributeInstanceByName(it->first);

		if (attributeinstance != NULL)
		{
			attributeinstance->removeModifier(it->second);
		}
	}
}

void IAttributeMap::applyAttributeModifiers(const AttrbuteModifierMap& modifiers)
{
	for (AttrbuteModifierMap::const_iterator it = modifiers.begin(); it != modifiers.end(); ++it)
	{
		IAttributeInstance* attributeinstance = getAttributeInstanceByName(it->first);

		if (attributeinstance != NULL)
		{
			attributeinstance->removeModifier(it->second);
			attributeinstance->applyModifier(it->second);
		}
	}
}

IAttributeInstance* AttributeMap::createInstance(IAttribute* attribute)
{
	return LordNew ModifiableAttributeInstance(this, attribute);
}

ModifiableAttributeInstance* AttributeMap::getAttributeInstance(IAttribute* attribute)
{
	return (ModifiableAttributeInstance*)IAttributeMap::getAttributeInstance(attribute);
}

ModifiableAttributeInstance* AttributeMap::getAttributeInstanceByName(const String& attributeName)
{
	IAttributeInstance* iattributeinstance = IAttributeMap::getAttributeInstanceByName(attributeName);

	if (iattributeinstance == NULL)
	{
		AttributeInsMapByName::iterator it = descriptionToAttributeInstanceMap.find(attributeName);
		if (it != descriptionToAttributeInstanceMap.end())
			iattributeinstance = it->second;
	}

	return (ModifiableAttributeInstance*)iattributeinstance;
}

IAttributeInstance* AttributeMap::registerAttribute(IAttribute* attribute)
{
	IAttributeInstance* iattributeinstance = IAttributeMap::registerAttribute(attribute);
	RangedAttribute* prangedattribute = dynamic_cast<RangedAttribute*>(attribute);
	if (prangedattribute && prangedattribute->getDescription() != StringUtil::BLANK)
	{
		descriptionToAttributeInstanceMap.insert(std::make_pair(prangedattribute->getDescription(), iattributeinstance));
	}
	return iattributeinstance;
}

void AttributeMap::onAttributeModified(IAttributeInstance* instance)
{
	if (instance->getAttribute()->getShouldWatch())
	{
		attributeInstanceSet.insert(instance);
	}

	IAttribute* k = instance->getAttribute();
	IAttribute* iattribute;
	for (AttributeMultiMap::iterator it = descendantsByParent.begin(); it != descendantsByParent.end(); ++it)
	{
		if (it->first != k)
			continue;
		iattribute = it->second;
		ModifiableAttributeInstance* modifiableattributeinstance = getAttributeInstance(iattribute);

		if (modifiableattributeinstance != NULL)
		{
			modifiableattributeinstance->flagForUpdate();
		}
	}
}

void AttributeMap::getWatchedAttributes(set<IAttributeInstance*>::type& outset)
{
	for (AttributeInsMapByName::iterator it = attributesByName.begin(); it != attributesByName.end(); ++it)
	{
		IAttributeInstance* iattributeinstance = it->second;
		if (iattributeinstance->getAttribute()->getShouldWatch())
			outset.insert(iattributeinstance);
	}
}

IAttribute* SharedMonsterAttributes::EAMAX_HEALTH = NULL;		// = (new RangedAttribute("generic.maxHealth", 20.0D, 0.0D, Double.MAX_VALUE)).func_111117_a("Max Health").func_111112_a(true);
IAttribute* SharedMonsterAttributes::FOLLOW_RANGE = NULL;		// = (new RangedAttribute("generic.followRange", 32.0D, 0.0D, 2048.0D)).func_111117_a("Follow Range");
IAttribute* SharedMonsterAttributes::KNOCKBACK_RESISTANCE = NULL;// = (new RangedAttribute("generic.knockbackResistance", 0.0D, 0.0D, 1.0D)).func_111117_a("Knockback Resistance");
IAttribute* SharedMonsterAttributes::MOVEMENT_SPEED = NULL;		// = (new RangedAttribute("generic.movementSpeed", 0.699999988079071D, 0.0D, Double.MAX_VALUE)).func_111117_a("Movement Speed").func_111112_a(true);
IAttribute* SharedMonsterAttributes::ATTACK_DAMAGE = NULL;		// = new RangedAttribute("generic.attackDamage", 2.0D, 0.0D, Double.MAX_VALUE);

void SharedMonsterAttributes::initialize()
{
	RangedAttribute* att = NULL;
	att = LordNew RangedAttribute(NULL, "generic.maxHealth", 20.0f, 0.0f, Math::MAX_FLOAT);
	att->setDescription("Max Health");
	att->setShouldWatch(true);
	EAMAX_HEALTH = att;

	att = LordNew RangedAttribute(NULL, "generic.followRange", 32.0f, 0.0f, 2048.0f);
	att->setDescription("Follow Range");
	FOLLOW_RANGE = att;

	att = LordNew RangedAttribute(NULL, "generic.knockbackResistance", 0.0f, 0.0f, 1.0f);
	att->setDescription("Knockback Resistance");
	KNOCKBACK_RESISTANCE = att;
	
	att = LordNew RangedAttribute(NULL, "generic.movementSpeed", 0.7f, 0.0f, Math::MAX_FLOAT);
	att->setDescription("Movement Speed");
	att->setShouldWatch(true);
	MOVEMENT_SPEED = att;

	ATTACK_DAMAGE = LordNew RangedAttribute(NULL, "generic.attackDamage", 2.0f, 0.0f, Math::MAX_FLOAT);
}

void SharedMonsterAttributes::unInitialize()
{
	LordSafeDelete(EAMAX_HEALTH);
	LordSafeDelete(FOLLOW_RANGE);
	LordSafeDelete(KNOCKBACK_RESISTANCE);
	LordSafeDelete(MOVEMENT_SPEED);
	LordSafeDelete(ATTACK_DAMAGE);
}

NBTTagList* SharedMonsterAttributes::writeBaseAttributeMapToNBT(IAttributeMap* attributeMap)
{
	NBTTagList* pResult = LordNew NBTTagList();

	IAttributeMap::AttributeInsMapByName& attrs = attributeMap->getAllAttributes();
	for (IAttributeMap::AttributeInsMapByName::iterator it = attrs.begin(); it != attrs.end(); ++it)
	{
		pResult->appendTag(writeAttributeInstanceToNBT(it->second));
	}
	return pResult;
}

NBTTagCompound* SharedMonsterAttributes::writeAttributeInstanceToNBT(IAttributeInstance* pAttributeInstance)
{
	NBTTagCompound* pResult = LordNew NBTTagCompound();
	IAttribute* attribute = pAttributeInstance->getAttribute();
	pResult->setString("Name", attribute->getName());
	pResult->setDouble("Base", pAttributeInstance->getBaseValue());

	AttModifierSet modifiers;
	pAttributeInstance->getModifiers(modifiers);

	NBTTagList* pNbtModifiers = NULL;
	if (!modifiers.empty())
		pNbtModifiers = LordNew NBTTagList();

	for (AttModifierSet::iterator it = modifiers.begin(); it != modifiers.end(); ++it)
	{
		AttributeModifier* pmodifier = *it;

		if (pmodifier->isSaved())
		{
			pNbtModifiers->appendTag(writeAttributeModifierToNBT(pmodifier));
		}
	}
	if (pNbtModifiers)
		pResult->setTag("Modifiers", pNbtModifiers);

	return pResult;
}

NBTTagCompound* SharedMonsterAttributes::writeAttributeModifierToNBT(AttributeModifier* modifer)
{
	NBTTagCompound* pResult = LordNew NBTTagCompound();
	pResult->setString("Name", modifer->getName());
	pResult->setDouble("Amount", modifer->getAmount());
	pResult->setInteger("Operation", modifer->getOperation());
	pResult->setLong("UUIDMost", modifer->getID().getMostSignificantBits());
	pResult->setLong("UUIDLeast", modifer->getID().getLeastSignificantBits());
	return pResult;
}

void SharedMonsterAttributes::setAttributeModifiers(IAttributeMap* attributemap, NBTTagList* pNbtTag)
{
	for (int i = 0; i < pNbtTag->tagCount(); ++i)
	{
		NBTTagCompound* pcompound = (NBTTagCompound*)pNbtTag->tagAt(i);
		IAttributeInstance* pattributeInstance = attributemap->getAttributeInstanceByName(pcompound->getString("Name"));

		if (pattributeInstance != NULL)
		{
			applyModifiersToAttributeInstance(pattributeInstance, pcompound);
		}
		else
		{
			LordLogError("Ignoring unknown attribute %s", pcompound->getString("Name").c_str());
		}
	}
}

void SharedMonsterAttributes::applyModifiersToAttributeInstance(IAttributeInstance* instance, NBTTagCompound* compound)
{
	instance->setBaseValue(compound->getFloat("Base"));

	if (compound->hasKey("Modifiers"))
	{
		NBTTagList* nbtTag = compound->getTagList("Modifiers");

		for (int i = 0; i < nbtTag->tagCount(); ++i)
		{
			AttributeModifier* modifier = readAttributeModifierFromNBT((NBTTagCompound*)nbtTag->tagAt(i));
			if (!modifier)
				continue;

			AttributeModifier* modifier2 = instance->getModifier(modifier->getID());

			if (!modifier2)
			{
				instance->removeModifier(modifier2);
			}
			instance->applyModifier(modifier);
		}
	}
}

AttributeModifier* SharedMonsterAttributes::readAttributeModifierFromNBT(NBTTagCompound* compound)
{
	i64 most = compound->getLong("UUIDMost");
	i64 least = compound->getLong("UUIDLeast");
	UUID id(most, least);
	return LordNew AttributeModifier(id, compound->getString("Name"), compound->getFloat("Amount"), compound->getInteger("Operation"));
}

}