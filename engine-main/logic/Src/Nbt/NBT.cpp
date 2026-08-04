#include "NBT.h"
#include "Nbt/NbtParser.h"

namespace BLOCKMAN
{

String NBTBase::NBTTypes[12] = { "END", "BYTE", "SHORT", "INT", "LONG", "FLOAT", "DOUBLE", "BYTE[]", "STRING", "LIST", "COMPOUND", "INT[]" };

NBTBase::NBTBase(const String& par1Str)
	: name(par1Str)
{
}

NBTBase::~NBTBase()
{

}

NBTBase* NBTBase::readNamedTag(DataStream* pData)
{
	return readNamedTag(pData, 0);
}

NBTBase* NBTBase::readNamedTag(DataStream* pData, int par1)
{
	i8 nbttype = 0;
	pData->read(&nbttype, sizeof(i8));

	if (nbttype == 0)
	{
		return LordNew NBTTagEnd();
	}
	else
	{
		String tagValue = pData->readString();
		NBTBase* pNBTBase = createTag(nbttype, tagValue);

		try
		{
			pNBTBase->load(pData, par1);
			return pNBTBase;
		}
		catch (...)
		{
			// LordException("Error in load NBT data");
		}
	}
	return NULL; //silence the compiler warning.
}

NBTBase * NBTBase::read(NbtParser & parser)
{
	return read(parser, 0);
}

NBTBase * NBTBase::read(NbtParser & parser, int depth)
{
	i8 nbttype = static_cast<i8>(parser.currentEvent().tagType);
	if (nbttype == 0)
	{
		return LordNew NBTTagEnd();
	}
	else
	{
		NBTBase* pNBTBase = createTag(nbttype, parser.currentEvent().tagName);

		try
		{
			pNBTBase->load(parser, depth);
			return pNBTBase;
		}
		catch (...)
		{
			// LordException("Error in load NBT data");
		}
	}
	return NULL; //silence the compiler warning.
}

void NBTBase::writeNamedTag(NBTBase& nbtBase, DataStream* pData)
{
	i8 id = nbtBase.getId();
	pData->write(&id, sizeof(i8));

	if (id != 0)
	{
		pData->writeString(nbtBase.getName());
		nbtBase.write(pData);
	}
}

NBTBase* NBTBase::createTag(i8 tagType, const String& tagName)
{
	switch (tagType)
	{
	case 0:
		return LordNew NBTTagEnd();

	case 1:
		return LordNew NBTTagByte(tagName);

	case 2:
		return LordNew NBTTagShort(tagName);

	case 3:
		return LordNew NBTTagInt(tagName);

	case 4:
		return LordNew NBTTagLong(tagName);

	case 5:
		return LordNew NBTTagFloat(tagName);

	case 6:
		return LordNew NBTTagDouble(tagName);

	case 7:
		return LordNew NBTTagByteArray(tagName);

	case 8:
		return LordNew NBTTagString(tagName);

	case 9:
		return LordNew NBTTagList(tagName);

	case 10:
		return LordNew NBTTagCompound(tagName);

	case 11:
		return LordNew NBTTagIntArray(tagName);

	default:
		return NULL;
	}
}

String NBTBase::getTagName(i8 tagType)
{
	switch (tagType)
	{
	case 0:
		return "TAG_End";

	case 1:
		return "TAG_Byte";

	case 2:
		return "TAG_Short";

	case 3:
		return "TAG_Int";

	case 4:
		return "TAG_Long";

	case 5:
		return "TAG_Float";

	case 6:
		return "TAG_Double";

	case 7:
		return "TAG_Byte_Array";

	case 8:
		return "TAG_String";

	case 9:
		return "TAG_List";

	case 10:
		return "TAG_Compound";

	case 11:
		return "TAG_Int_Array";

	default:
		return "UNKNOWN";
	}
}

bool NBTBase::equals(const NBTBase* rhs) const
{
	return getId() == rhs->getId() && name == rhs->name;
}

int NBTBase::hashCode() const
{
	int h = 0;
	if (name.length() > 0)
	{
		const char* val = name.c_str();

		for (size_t i = 0; i < name.length(); i++)
		{
			h = 31 * h + val[i];
		}
	}
	return h ^ getId();
}

NBTTagEnd::NBTTagEnd()
	: NBTBase("")
{
}

NBTBase* NBTTagEnd::copy()
{
	return new NBTTagEnd(); 
}

NBTTagByte::NBTTagByte(const String& par1Str)
	: NBTBase(par1Str)
{
}

NBTTagByte::NBTTagByte(const String& par1Str, i8 par2)
	: NBTBase(par1Str)
	, data(par2)
{
}

void NBTTagByte::write(DataStream* pData)
{
	pData->write(&data, sizeof(i8));
}

void NBTTagByte::load(DataStream* pData, int par2)
{
	pData->read(&data, sizeof(i8));
}

void NBTTagByte::load(NbtParser & parser, int depth)
{
	data = parser.currentEvent().byteValue;
}

String NBTTagByte::toString() const 
{
	return StringUtil::ToString(data);
}

NBTBase* NBTTagByte::copy()
{
	return LordNew NBTTagByte(getName(), data);
}

bool NBTTagByte::equals(const NBTBase* rhs) const
{
	const NBTTagByte* pNbt = dynamic_cast<const NBTTagByte*>(rhs);
	if (!pNbt)
		return false;

	return NBTBase::equals(rhs) && data == pNbt->data;
}

int NBTTagByte::hashCode() const
{
	return NBTBase::hashCode() ^ data;
}

NBTTagShort::NBTTagShort(const String& par1Str)
	: NBTBase(par1Str)
{
}

NBTTagShort::NBTTagShort(const String& par1Str, i16 par2)
	: NBTBase(par1Str)
	, data(par2)
{
}

void NBTTagShort::write(DataStream* pData)
{
	pData->write(&data, sizeof(i16));
}

void NBTTagShort::load(DataStream* pData, int par2)
{
	pData->read(&data, sizeof(i16));
}

void NBTTagShort::load(NbtParser & parser, int depth)
{
	data = parser.currentEvent().shortValue;
}

String NBTTagShort::toString() const
{
	return StringUtil::ToString(data);
}

NBTBase* NBTTagShort::copy()
{
	return LordNew NBTTagShort(getName(), data);
}

bool NBTTagShort::equals(const NBTBase* rhs) const
{
	const NBTTagShort* pNbt = dynamic_cast<const NBTTagShort*>(rhs);
	if (!pNbt)
		return false;
	return NBTBase::equals(rhs) && data == pNbt->data;
}

int NBTTagShort::hashCode() const
{
	return NBTBase::hashCode() ^ data;
}

NBTTagInt::NBTTagInt(const String& par1Str)
	: NBTBase(par1Str)
{
}

NBTTagInt::NBTTagInt(const String& par1Str, i32 par2)
	: NBTBase(par1Str)
	, data(par2)
{
}

void NBTTagInt::write(DataStream* pData)
{
	pData->write(&data, sizeof(i32));
}

void NBTTagInt::load(DataStream* pData, int par2)
{
	pData->read(&data, sizeof(i32));
}

void NBTTagInt::load(NbtParser & parser, int depth)
{
	data = parser.currentEvent().intValue;
}

String NBTTagInt::toString() const
{
	return StringUtil::ToString(data);
}

NBTBase* NBTTagInt::copy()
{
	return LordNew NBTTagInt(getName(), data);
}

bool NBTTagInt::equals(const NBTBase* rhs) const
{
	const NBTTagInt* pNbt = dynamic_cast<const NBTTagInt*>(rhs);
	if (!pNbt)
		return false;
	return NBTBase::equals(rhs) && data == pNbt->data;
}

int NBTTagInt::hashCode() const
{
	return NBTBase::hashCode() ^ data;
}

NBTTagLong::NBTTagLong(const String& par1Str)
	: NBTBase(par1Str)
{
}

NBTTagLong::NBTTagLong(const String& par1Str, i64 par2)
	: NBTBase(par1Str)
	, data(par2)
{
}

void NBTTagLong::write(DataStream* pData)
{
	pData->write(&data, sizeof(i64));
}

void NBTTagLong::load(DataStream* pData, int par2)
{
	pData->read(&data, sizeof(i64));
}

void NBTTagLong::load(NbtParser & parser, int depth)
{
	data = parser.currentEvent().longValue;
}

String NBTTagLong::toString() const
{
	return StringUtil::ToString(data);
}

NBTBase* NBTTagLong::copy()
{
	return LordNew NBTTagLong(getName(), data);
}

bool NBTTagLong::equals(const NBTBase* rhs) const
{
	const NBTTagLong* pNbt = dynamic_cast<const NBTTagLong*>(rhs);
	if (!pNbt)
		return false;
	return NBTBase::equals(rhs) && data == pNbt->data;
}

int NBTTagLong::hashCode() const
{
	return NBTBase::hashCode() ^ (i32)(data ^ ((ui64)data >> 32));
}

NBTTagFloat::NBTTagFloat(const String& par1Str)
	: NBTBase(par1Str)
{
}

NBTTagFloat::NBTTagFloat(const String& par1Str, float par2)
	: NBTBase(par1Str)
	, data(par2)
{
}

void NBTTagFloat::write(DataStream* pData)
{
	pData->write(&data, sizeof(float));
}

void NBTTagFloat::load(DataStream* pData, int par2)
{
	pData->read(&data, sizeof(float));
}

void NBTTagFloat::load(NbtParser & parser, int depth)
{
	data = parser.currentEvent().floatValue;
}

String NBTTagFloat::toString() const
{
	return StringUtil::ToString(data);
}

NBTBase* NBTTagFloat::copy()
{
	return LordNew NBTTagFloat(getName(), data);
}

bool NBTTagFloat::equals(const NBTBase* rhs) const
{
	const NBTTagFloat* pNbt = dynamic_cast<const NBTTagFloat*>(rhs);
	if (!pNbt)
		return false;
	return NBTBase::equals(rhs) && data == pNbt->data;
}

int NBTTagFloat::hashCode() const
{
	int intVal = *(int*)&data;
	return NBTBase::hashCode() ^ intVal;
}

NBTTagDouble::NBTTagDouble(const String& par1Str)
	: NBTBase(par1Str)
{
}

NBTTagDouble::NBTTagDouble(const String& par1Str, double par2)
	: NBTBase(par1Str)
	, data(par2)
{
}

void NBTTagDouble::write(DataStream* pData)
{
	pData->write(&data, sizeof(double));
}

void NBTTagDouble::load(DataStream* pData, int par2)
{
	pData->read(&data, sizeof(double));
}

void NBTTagDouble::load(NbtParser & parser, int depth)
{
	data = parser.currentEvent().doubleValue;
}

String NBTTagDouble::toString() const
{
	return StringUtil::ToString(data);
}

NBTBase* NBTTagDouble::copy()
{
	return LordNew NBTTagDouble(getName(), data);
}

bool NBTTagDouble::equals(const NBTBase* rhs) const
{
	const NBTTagDouble* pNbt = dynamic_cast<const NBTTagDouble*>(rhs);
	if (!pNbt)
		return false;
	return NBTBase::equals(rhs) && data == pNbt->data;
}

int NBTTagDouble::hashCode() const
{
	i64 intVal = *(i64*)&data;
	return NBTBase::hashCode() ^ (i32)(intVal ^ ((ui64)intVal >> 32));
}


NBTTagByteArray::NBTTagByteArray(const String& par1Str)
	: NBTBase(par1Str)
	, byteArray(NULL)
	, arraySize(0)
{
}

NBTTagByteArray::NBTTagByteArray(const String& par1Str, i8* arr, int arrSize)
	: NBTBase(par1Str)
{
	byteArray = (i8*)LordMalloc(sizeof(i8)*arrSize);
	memcpy(byteArray, arr, arrSize*sizeof(i8));
	arraySize = arrSize;
}

NBTTagByteArray::~NBTTagByteArray()
{
	LordSafeFree(byteArray);
}

void NBTTagByteArray::write(DataStream* pData)
{
	pData->write(&arraySize, sizeof(int));
	pData->write(byteArray, sizeof(i8)*arraySize);
}

void NBTTagByteArray::load(DataStream* pData, int par2)
{
	LordSafeFree(byteArray);

	pData->read(&arraySize, sizeof(int));
	byteArray = (i8*)LordMalloc(sizeof(i8)*arraySize);
	pData->read(byteArray, sizeof(i8)*arraySize);
}

void NBTTagByteArray::load(NbtParser & parser, int depth)
{
	LordSafeFree(byteArray);
	arraySize = parser.currentEvent().listLength;
	byteArray = (i8*)LordMalloc(sizeof(i8)*arraySize);
	parser.nextEvent();
	for (size_t index = 0; !parser.currentEvent().isListEnd(); parser.nextEvent())
	{
		byteArray[index] = parser.currentEvent().byteValue;
	}
}

String NBTTagByteArray::toString() const
{
	char buff[64];
	sprintf(buff, "[%d bytes]", arraySize);
	return String(buff);
}

NBTBase* NBTTagByteArray::copy()
{
	return LordNew NBTTagByteArray(getName(), byteArray, arraySize);
}

bool NBTTagByteArray::equals(const NBTBase* rhs) const
{
	const NBTTagByteArray* pNbt = dynamic_cast<const NBTTagByteArray*>(rhs);
	if (!pNbt)
		return false;
	
	if (NBTBase::equals(rhs) && arraySize == pNbt->arraySize)
	{
		for (int i = 0; i < arraySize; ++i)
		{
			if (byteArray[i] != pNbt->byteArray[i])
				return false;
		}
		return true;
	}
	return false;
}

int NBTTagByteArray::hashCode() const
{
	int arrayhash = 1;
	for (int i = 0; i < arraySize; ++i)
		arrayhash = 31 * arrayhash + byteArray[i];

	return NBTBase::hashCode() ^ arrayhash;
}


NBTTagString::NBTTagString(const String& par1Str)
	: NBTBase(par1Str)
{
}

NBTTagString::NBTTagString(const String& par1Str, const String& str)
	: NBTBase(par1Str)
	, data(str)
{
}

void NBTTagString::write(DataStream* pData)
{
	pData->writeString(data);
}

void NBTTagString::load(DataStream* pData, int par2)
{
	data = pData->readString();
}

void NBTTagString::load(NbtParser & parser, int depth)
{
	data = parser.currentEvent().stringValue;
}

String NBTTagString::toString() const
{
	return data;
}

NBTBase* NBTTagString::copy()
{
	return LordNew NBTTagString(getName(), data);
}

bool NBTTagString::equals(const NBTBase* rhs) const
{
	const NBTTagString* pNbt = dynamic_cast<const NBTTagString*>(rhs);
	if (!pNbt)
		return false;
	return NBTBase::equals(rhs) && data == pNbt->data;
}

int NBTTagString::hashCode() const
{
	int h = 0;
	if (data.length() > 0)
	{
		const char* val = data.c_str();

		for (size_t i = 0; i < data.length(); i++)
		{
			h = 31 * h + val[i];
		}
	}

	return NBTBase::hashCode() ^ h;
}


NBTTagList::NBTTagList()
	: NBTBase("")
{
}

NBTTagList::NBTTagList(const String& par1Str)
	: NBTBase(par1Str)
{
}

NBTTagList::~NBTTagList()
{
	for (TagList::iterator it = m_list.begin(); it != m_list.end(); ++it)
	{
		LordSafeDelete(*it);
	}
}

void NBTTagList::write(DataStream* pData)
{
	int listLen = m_list.size();
	if (listLen > 0)
	{
		tagType = m_list[0]->getId();
	}
	else
	{
		tagType = 1;
	}

	pData->write(&tagType, sizeof(i8));
	pData->write(&listLen, sizeof(int));

	for (int i = 0; i < listLen; ++i)
	{
		m_list[i]->write(pData);
	}
}

void NBTTagList::load(DataStream* pData, int par2)
{
	if (par2 > 512)
	{
		LordException("Tried to read NBT tag with too high complexity, depth > 512");
	}
	else
	{
		int listSize = 0;
		pData->read(&tagType, sizeof(i8));
		pData->read(&listSize, sizeof(int));

		for (int i = 0; i < listSize; ++i)
		{
			NBTBase* pNBT = NBTBase::createTag(tagType, "");
			pNBT->load(pData, par2 + 1);
			m_list.push_back(pNBT);
		}
	}
}

void NBTTagList::load(NbtParser & parser, int depth)
{
	if (depth > 512)
	{
		LordException("Tried to read NBT tag with too high complexity, depth > 512");
	}
	else
	{
		tagType = static_cast<i8>(parser.currentEvent().listTagType);
		for (parser.nextEvent(); !parser.currentEvent().isListEnd(); parser.nextEvent())
		{
			NBTBase* pNBT = NBTBase::createTag(tagType, "");
			pNBT->load(parser, depth + 1);
			m_list.push_back(pNBT);
		}
	}
}

String NBTTagList::toString() const
{
	char buff[64];
	sprintf(buff, "%d entires of type %s", m_list.size(), NBTBase::getTagName(tagType).c_str());
	return String(buff);
}

void NBTTagList::appendTag(NBTBase* pNBT)
{
	LordAssert(pNBT);
	if (m_list.empty())
	{
		tagType = pNBT->getId();
	}
	else
	{
		LordAssertX(m_list[0]->getId() == pNBT->getId(), "NBTTagList mush append the same tag.");
	}
	m_list.push_back(pNBT);
}

NBTBase* NBTTagList::removeTag(int index)
{
	int i = 0;
	for (TagList::iterator it = m_list.begin(); it != m_list.end(); ++it, ++i)
	{
		if (i == index)
		{
			m_list.erase(it);
			return *it;
		}
	}
	return NULL;
}

NBTBase* NBTTagList::tagAt(int index)
{
	if (index >= int(m_list.size()))
	{
		return nullptr;
	}
	return m_list[index];
}

NBTBase* NBTTagList::copy()
{
	NBTTagList* pResult = LordNew NBTTagList(getName());
	pResult->tagType = tagType;

	for (TagList::iterator it = m_list.begin(); it != m_list.end(); ++it)
	{
		pResult->m_list.push_back((*it)->copy());
	}
	return pResult;
}

bool NBTTagList::equals(const NBTBase* rhs) const
{
	const NBTTagList* pNbt = dynamic_cast<const NBTTagList*>(rhs);
	if (!pNbt)
		return false;

	if (!NBTBase::equals(rhs))
		return false;

	if (tagType != tagType)
		return false;

	if (m_list.size() != pNbt->m_list.size())
		return false;

	TagList::const_iterator it = m_list.begin();
	TagList::const_iterator jt = pNbt->m_list.begin();
	for (; it != m_list.end(); ++it, ++jt)
	{
		if (!(*it)->equals(*jt))
			return false;
	}
	return true;
	
}

int NBTTagList::hashCode() const
{
	int h = 1;
	size_t listSize = m_list.size();
	for (size_t i=0; i<listSize; ++i )
	{
		h = 31 * h + (m_list[i])->hashCode();
	}

	return NBTBase::hashCode() ^ h;
}

NBTTagCompound::NBTTagCompound()
	: NBTBase("")
{}

NBTTagCompound::NBTTagCompound(const String& par1Str)
	: NBTBase(par1Str)
{}

NBTTagCompound::~NBTTagCompound()
{
	release();
}

void NBTTagCompound::release()
{
	for (TagMap::iterator it = tagMap.begin(); it != tagMap.end(); ++it)
	{
		LordSafeDelete(it->second);
	}
	tagMap.clear();
}

void NBTTagCompound::write(DataStream* pData)
{
	for (TagMap::iterator it = tagMap.begin(); it != tagMap.end(); ++it)
	{
		NBTBase* pNBT = it->second;
		pNBT->writeNamedTag(*pNBT, pData);

	}
	i8 endtag = 0;
	pData->write(&endtag, sizeof(i8));
}

void NBTTagCompound::load(DataStream* pData, int par2)
{
	if (par2 > 512)
	{
		LordException("Tried to read NBT tag with too high complexity, depth > 512");
	}
	else
	{
		release();
		NBTBase* pNBT = NULL;

		while ((pNBT = NBTBase::readNamedTag(pData, par2 + 1))->getId() != 0)
		{
			tagMap.insert(std::make_pair(pNBT->getName(), pNBT));
		}
	}
}

void NBTTagCompound::load(NbtParser & parser, int depth)
{
	if (depth > 512)
	{
		LordException("Tried to read NBT tag with too high complexity, depth > 512");
	}
	else
	{
		release();
		for (parser.nextEvent(); !parser.currentEvent().isCompoundEnd(); parser.nextEvent())
		{
			auto tag = NBTBase::read(parser, depth + 1);
			tagMap[tag->getName()] = tag;
		}
	}
}

void NBTTagCompound::setTag(const String& tagName, NBTBase* pNBT)
{
	pNBT->setName(tagName);
	tagMap.insert(std::make_pair(tagName, pNBT));
}

void NBTTagCompound::setByte(const String& tagName, i8 val)
{
	NBTBase* pNBT = LordNew NBTTagByte(tagName, val);
	tagMap.insert(std::make_pair(tagName, pNBT));
}

void NBTTagCompound::setShort(const String& tagName, short val)
{
	NBTBase* pNBT = LordNew NBTTagShort(tagName, val);
	tagMap.insert(std::make_pair(tagName, pNBT));
}

void NBTTagCompound::setInteger(const String& tagName, int val)
{
	NBTBase* pNBT = LordNew NBTTagInt(tagName, val);
	tagMap.insert(std::make_pair(tagName, pNBT));
}

void NBTTagCompound::setLong(const String& tagName, i64 val)
{
	NBTBase* pNBT = LordNew NBTTagLong(tagName, val);
	tagMap.insert(std::make_pair(tagName, pNBT));
}

void NBTTagCompound::setFloat(const String& tagName, float val)
{
	NBTBase* pNBT = LordNew NBTTagFloat(tagName, val);
	tagMap.insert(std::make_pair(tagName, pNBT));
}

void NBTTagCompound::setDouble(const String& tagName, double val)
{
	NBTBase* pNBT = LordNew NBTTagDouble(tagName, val);
	tagMap.insert(std::make_pair(tagName, pNBT));
}

void NBTTagCompound::setString(const String& tagName, const String& val)
{
	NBTBase* pNBT = LordNew NBTTagString(tagName, val);
	tagMap.insert(std::make_pair(tagName, pNBT));
}

void NBTTagCompound::setByteArray(const String& tagName, i8* arr, int arrSize)
{
	NBTBase* pNBT = LordNew NBTTagByteArray(tagName, arr, arrSize);
	tagMap.insert(std::make_pair(tagName, pNBT));
}

void NBTTagCompound::setIntArray(const String& tagName, int* arr, int arrSize)
{
	NBTBase* pNBT = LordNew NBTTagIntArray(tagName, arr, arrSize);
	tagMap.insert(std::make_pair(tagName, pNBT));
}

void NBTTagCompound::setCompoundTag(const String& tagName, NBTTagCompound* pCompound)
{
	pCompound->setName(tagName);
	tagMap.insert(std::make_pair(tagName, pCompound));
}

void NBTTagCompound::setBool(const String& tagName, bool val)
{
	setByte(tagName, (i8)(val ? 1 : 0));
}

NBTBase* NBTTagCompound::getTag(const String& tagName) const
{
	TagMap::const_iterator it = tagMap.find(tagName.c_str());
	if (it == tagMap.end())
		return NULL;
	
	return it->second;
}

bool NBTTagCompound::hasKey(const String& tagName) const 
{
	return tagMap.find(tagName) != tagMap.end(); 
}

void NBTTagCompound::removeTag(const String& tagName)
{
	TagMap::iterator it = tagMap.find(tagName);
	if (it != tagMap.end())
	{
		LordSafeDelete(it->second);
		tagMap.erase(it);
	}
}

i8 NBTTagCompound::getByte(const String& tagName) const
{
	TagMap::const_iterator it = tagMap.find(tagName);
	if (it == tagMap.end())
		return 0;
	
	NBTTagByte* pNBTByte = dynamic_cast<NBTTagByte*>(it->second);
	if (pNBTByte == NULL)
		return 0;

	return pNBTByte->data;
}

short NBTTagCompound::getShort(const String& tagName) const
{
	TagMap::const_iterator it = tagMap.find(tagName);
	if (it == tagMap.end())
		return 0;

	NBTTagShort* pNbtShort = dynamic_cast<NBTTagShort*>(it->second);
	if (pNbtShort == NULL)
		return 0;

	return pNbtShort->data;
}

int NBTTagCompound::getInteger(const String& tagName) const
{
	TagMap::const_iterator it = tagMap.find(tagName);
	if (it == tagMap.end())
		return 0;

	NBTTagInt* pNbtInt = dynamic_cast<NBTTagInt*>(it->second);
	if (pNbtInt == NULL)
		return 0;

	return pNbtInt->data;
}

i64 NBTTagCompound::getLong(const String& tagName) const
{
	TagMap::const_iterator it = tagMap.find(tagName);
	if (it == tagMap.end())
		return 0LL;

	NBTTagLong* pNbtLong = dynamic_cast<NBTTagLong*>(it->second);
	if (pNbtLong == NULL)
		return 0LL;

	return pNbtLong->data;
}

float NBTTagCompound::getFloat(const String& tagName) const
{
	TagMap::const_iterator it = tagMap.find(tagName);
	if (it == tagMap.end())
		return 0.f;

	NBTTagFloat* pNbtFloat = dynamic_cast<NBTTagFloat*>(it->second);
	if (pNbtFloat == NULL)
		return 0.f;

	return pNbtFloat->data;
}

double NBTTagCompound::getDouble(const String& tagName) const
{
	TagMap::const_iterator it = tagMap.find(tagName);
	if (it == tagMap.end())
		return 0.0;

	NBTTagDouble* pNbtDouble = dynamic_cast<NBTTagDouble*>(it->second);
	if (pNbtDouble == NULL)
		return 0.0;

	return pNbtDouble->data;
}

String NBTTagCompound::getString(const String& tagName) const
{
	TagMap::const_iterator it = tagMap.find(tagName);
	if (it == tagMap.end())
		return StringUtil::BLANK;

	NBTTagString* pNbtString = dynamic_cast<NBTTagString*>(it->second);
	if (pNbtString == NULL)
		return StringUtil::BLANK;

	return pNbtString->data;
}

i8* NBTTagCompound::getByteArray(const String& tagName) const
{
	TagMap::const_iterator it = tagMap.find(tagName);
	if (it == tagMap.end())
		return NULL;

	NBTTagByteArray* pNbtByteArray = dynamic_cast<NBTTagByteArray*>(it->second);
	if (pNbtByteArray == NULL)
		return NULL;

	return pNbtByteArray->byteArray;
}

int* NBTTagCompound::getIntArray(const String& tagName) const
{
	TagMap::const_iterator it = tagMap.find(tagName);
	if (it == tagMap.end())
		return NULL;

	NBTTagIntArray* pNbtIntArray = dynamic_cast<NBTTagIntArray*>(it->second);
	if (pNbtIntArray == NULL)
		return NULL;

	return pNbtIntArray->intArray;
}

int NBTTagCompound::getArraySize(const String& tagName) const
{
	TagMap::const_iterator it = tagMap.find(tagName);
	if (it == tagMap.end())
		return 0;

	NBTTagIntArray* pNbtIntArray = dynamic_cast<NBTTagIntArray*>(it->second);
	if (pNbtIntArray == NULL)
		return 0;

	return pNbtIntArray->arraySize;
}

NBTTagCompound* NBTTagCompound::getCompoundTag(const String& tagName) const
{
	TagMap::const_iterator it = tagMap.find(tagName);
	if (it == tagMap.end())
		return NULL;

	NBTTagCompound* pNbtCompound = dynamic_cast<NBTTagCompound*>(it->second);
	return pNbtCompound;
}

NBTTagList* NBTTagCompound::getTagList(const String& tagName) const
{
	TagMap::const_iterator it = tagMap.find(tagName);
	if (it == tagMap.end())
		return NULL;

	NBTTagList* pNbtLst = dynamic_cast<NBTTagList*>(it->second);
	return pNbtLst;
}

bool NBTTagCompound::getBool(const String& tagName) const
{
	return getByte(tagName) != 0;
}

String NBTTagCompound::toString() const
{
	String result = getName() + ":[";
	
	for (TagMap::const_iterator it = tagMap.begin(); it != tagMap.end(); ++it)
	{
		result += it->first;
		result += ":";
		result += it->second->toString();
		result += ",";
	}
	return result + "]";
}

NBTBase* NBTTagCompound::copy()
{
	NBTTagCompound* pCompound = LordNew NBTTagCompound(getName());

	for (TagMap::iterator it = tagMap.begin(); it != tagMap.end(); ++it)
	{
		pCompound->setTag(it->first, it->second->copy());
	}
	
	return pCompound;
}

bool NBTTagCompound::equals(const NBTBase* rhs) const
{
	const NBTTagCompound* pNbt = dynamic_cast<const NBTTagCompound*>(rhs);
	if (!pNbt)
		return false;

	if (!NBTBase::equals(rhs))
		return false;
	if (tagMap.size() != pNbt->tagMap.size())
		return false;

	TagMap::const_iterator it = tagMap.begin();
	TagMap::const_iterator jt = pNbt->tagMap.begin();
	for (; it != tagMap.end(); ++it, ++jt)
	{
		if (it->first != jt->first)
			return false;
		if (!it->second->equals(jt->second))
			return false;
	}
	return true;
}

int NBTTagCompound::hashCode() const
{
	int h = 1;
	for (TagMap::const_iterator it = tagMap.begin(); it != tagMap.end(); ++it)
	{
		h = 31 * h + it->second->hashCode();
	}
	return NBTBase::hashCode() ^ h;
}

NBTTagIntArray::NBTTagIntArray(const String& par1Str)
	: NBTBase(par1Str)
	, intArray(NULL)
	, arraySize(0)
{
}

NBTTagIntArray::NBTTagIntArray(const String& par1Str, int* arr, int arrSize)
	: NBTBase(par1Str)
{
	intArray = (int*)LordMalloc(sizeof(int)*arrSize);
	memcpy(intArray, arr, sizeof(int)*arrSize);
	arraySize = arrSize;
}

NBTTagIntArray::~NBTTagIntArray()
{
	LordSafeFree(intArray);
}

void NBTTagIntArray::write(DataStream* pData)
{
	pData->write(&arraySize, sizeof(int));
	pData->write(intArray, sizeof(int)*arraySize);
}

void NBTTagIntArray::load(DataStream* pData, int par2)
{
	LordSafeFree(intArray);

	pData->read(&arraySize, sizeof(int));
	intArray = (int*)LordMalloc(sizeof(int)*arraySize);
	pData->read(intArray, sizeof(int)*arraySize);
}

void NBTTagIntArray::load(NbtParser & parser, int depth)
{
	LordSafeFree(intArray);
	arraySize = parser.currentEvent().listLength;
	intArray = (int*)LordMalloc(sizeof(int)*arraySize);
	parser.nextEvent();
	for (size_t index = 0; !parser.currentEvent().isListEnd(); parser.nextEvent())
	{
		intArray[index++] = parser.currentEvent().intValue;
	}
}

String NBTTagIntArray::toString() const
{
	char buff[64];
	sprintf(buff, "[%d bytes]", arraySize);
	return String(buff);
}

NBTBase* NBTTagIntArray::copy()
{
	return LordNew NBTTagIntArray(getName(), intArray, arraySize);
}

bool NBTTagIntArray::equals(const NBTBase* rhs) const
{
	const NBTTagIntArray* pNbt = dynamic_cast<const NBTTagIntArray*>(rhs);
	if (!pNbt)
		return false;

	if (NBTBase::equals(rhs) && arraySize == pNbt->arraySize)
	{
		for (int i = 0; i < arraySize; ++i)
		{
			if (intArray[i] != pNbt->intArray[i])
				return false;
		}
		return true;
	}
	return false;
}

int NBTTagIntArray::hashCode() const
{
	int arrayhash = 1;
	for (int i = 0; i < arraySize; ++i)
		arrayhash = 31 * arrayhash + intArray[i];

	return NBTBase::hashCode() ^ arrayhash;
}


}
