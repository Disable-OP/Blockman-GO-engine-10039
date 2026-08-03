/********************************************************************
filename: 	NBTBase.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-11-3
*********************************************************************/
#ifndef __NBT_BASE_HEADER__
#define __NBT_BASE_HEADER__

#include <stdexcept>
#include "Core.h"
using namespace LORD;

namespace BLOCKMAN
{
	class InvalidNbtFormatError : public std::runtime_error
	{
		using std::runtime_error::runtime_error;
	};

	class NbtParser;
	enum class NbtTagType : uint8_t
	{
		TAG_END = 0,
		TAG_BYTE = 1,
		TAG_SHORT = 2,
		TAG_INT = 3,
		TAG_LONG = 4,
		TAG_FLOAT = 5,
		TAG_DOUBLE = 6,
		TAG_BYTE_ARRAY = 7,
		TAG_STRING = 8,
		TAG_LIST = 9,
		TAG_COMPOUND = 10,
		TAG_INT_ARRAY = 11
	};

class NBTBase : public ObjectAlloc
{
public:
	static String NBTTypes[12]; // = { "END", "BYTE", "SHORT", "INT", "LONG", "FLOAT", "DOUBLE", "BYTE[]", "STRING", "LIST", "COMPOUND", "INT[]" };

	NBTBase(const String& par1Str);
	virtual ~NBTBase();

	/** Write the actual data contents of the tag, implemented in NBT extension classes */
	virtual void write(DataStream* pData) = 0;

	/** Read the actual data contents of the tag, implemented in NBT extension classes */
	virtual void load(DataStream* pData, int var2) = 0;
	virtual void load(NbtParser& parser, int depth) = 0;

	/** Gets the type byte for the tag.	*/
	virtual i8 getId() const = 0;

	/** Sets the name for this tag and returns this for convenience. */
	void setName(const String& par1Str) { name = par1Str; }
	const String& getName() const { return name; }
	
	/** Reads and returns a tag from the given DataInput, or the End tag if no tag could be read. */
	static NBTBase* readNamedTag(DataStream* pData);
	static NBTBase* readNamedTag(DataStream* pData, int par1);
	static NBTBase* read(NbtParser& parser);
	static NBTBase* read(NbtParser& parser, int depth);

	/** Writes the specified tag to the given DataOutput, writing the type byte, the UTF string key and then calling the tag to write its data. */
	static void writeNamedTag(NBTBase& nbtBase, DataStream* pData);

	/** Creates and returns a new tag of the specified type, or null if invalid. */
	static NBTBase* createTag(i8 tagType, const String& tagName);

	/** Returns the string name of a tag with the specified type, or 'UNKNOWN' if invalid. */
	static String getTagName(i8 tagType);

	virtual String toString() const { return StringUtil::BLANK; }

	/** Creates a clone of the tag.	*/
	virtual NBTBase* copy() = 0;

	virtual bool equals(const NBTBase* rhs) const;

	virtual int hashCode() const;

protected:
	/** The UTF string key used to lookup values. */
	String name;	
};

class NBTTagEnd : public NBTBase
{
public:
	NBTTagEnd();
	
	/** Read the actual data contents of the tag, implemented in NBT extension classes */
	virtual void load(DataStream* pData, int par2) {}
	virtual void load(NbtParser& parser, int depth) {}

	/** Write the actual data contents of the tag, implemented in NBT extension classes */
	virtual void write(DataStream* pData) {}

	/** Gets the type byte for the tag. */
	virtual i8 getId() const { return 0;	}

	virtual String toString() const { return "END"; }

	/** Creates a clone of the tag. */
	virtual NBTBase* copy();
};

class NBTTagByte : public NBTBase
{
public:
	NBTTagByte(const String& par1Str);
	NBTTagByte(const String& par1Str, i8 par2);

	virtual void write(DataStream* pData);
	virtual void load(DataStream* pData, int par2);
	virtual void load(NbtParser& parser, int depth);
	virtual i8 getId() const { return 1; }
	virtual String toString() const;
	virtual NBTBase* copy();
	virtual bool equals(const NBTBase* rhs) const;
	virtual int hashCode() const;

public:
	i8 data;
};

class NBTTagShort : public NBTBase
{
public:
	NBTTagShort(const String& par1Str);
	NBTTagShort(const String& par1Str, i16 par2);

	virtual void write(DataStream* pData);
	virtual void load(DataStream* pData, int par2);
	virtual void load(NbtParser& parser, int depth);
	virtual i8 getId() const { return 2; }
	virtual String toString() const;
	virtual NBTBase* copy();
	virtual bool equals(const NBTBase* rhs) const;
	virtual int hashCode() const;

public:
	i16 data;
};

class NBTTagInt : public NBTBase
{
public:
	NBTTagInt(const String& par1Str);
	NBTTagInt(const String& par1Str, i32 par2);

	virtual void write(DataStream* pData);
	virtual void load(DataStream* pData, int par2);
	virtual void load(NbtParser& parser, int depth);
	virtual i8 getId() const { return 3; }
	virtual String toString() const;
	virtual NBTBase* copy();
	virtual bool equals(const NBTBase* rhs) const;
	virtual int hashCode() const;

public:
	i32 data;
};

class NBTTagLong : public NBTBase
{
public:
	NBTTagLong(const String& par1Str);
	NBTTagLong(const String& par1Str, i64 par2);

	virtual void write(DataStream* pData);
	virtual void load(DataStream* pData, int par2);
	virtual void load(NbtParser& parser, int depth);
	virtual i8 getId() const { return 4; }
	virtual String toString() const;
	virtual NBTBase* copy();
	virtual bool equals(const NBTBase* rhs) const;
	virtual int hashCode() const;

public:
	i64 data;
};

class NBTTagFloat : public NBTBase
{
public:
	NBTTagFloat(const String& par1Str);
	NBTTagFloat(const String& par1Str, float par2);

	virtual void write(DataStream* pData);
	virtual void load(DataStream* pData, int par2);
	virtual void load(NbtParser& parser, int depth);
	virtual i8 getId() const { return 5; }
	virtual String toString() const;
	virtual NBTBase* copy();
	virtual bool equals(const NBTBase* rhs) const;
	virtual int hashCode() const;

public:
	float data = 0.0f;
};

class NBTTagDouble : public NBTBase
{
public:
	NBTTagDouble(const String& par1Str);
	NBTTagDouble(const String& par1Str, double par2);

	virtual void write(DataStream* pData);
	virtual void load(DataStream* pData, int par2);
	virtual void load(NbtParser& parser, int depth);
	virtual i8 getId() const { return 6; }
	virtual String toString() const;
	virtual NBTBase* copy();
	virtual bool equals(const NBTBase* rhs) const;
	virtual int hashCode() const;

public:
	double data = 0.0f;
};

class NBTTagByteArray : public NBTBase
{
public:
	NBTTagByteArray(const String& par1Str);
	NBTTagByteArray(const String& par1Str, i8* arr, int arrSize);

	~NBTTagByteArray();

	virtual void write(DataStream* pData);
	virtual void load(DataStream* pData, int par2);
	virtual void load(NbtParser& parser, int depth);
	virtual i8 getId() const { return 7; }
	virtual String toString() const;
	virtual NBTBase* copy();
	virtual bool equals(const NBTBase* rhs) const;
	virtual int hashCode() const;

public:
	i8* byteArray;
	int arraySize;
};

class NBTTagString : public NBTBase
{
public:
	NBTTagString(const String& par1Str);
	NBTTagString(const String& par1Str, const String& str);

	virtual void write(DataStream* pData);
	virtual void load(DataStream* pData, int par2);
	virtual void load(NbtParser& parser, int depth);
	virtual i8 getId() const { return 8; }
	virtual String toString() const;
	virtual NBTBase* copy();
	virtual bool equals(const NBTBase* rhs) const;
	virtual int hashCode() const;

public:
	String data;
};

class NBTTagList : public NBTBase
{
public:
	typedef vector<NBTBase*>::type TagList;
public:
	NBTTagList();
	NBTTagList(const String& par1Str);
	~NBTTagList();

	virtual void write(DataStream* pData);
	virtual void load(DataStream* pData, int par2);
	virtual void load(NbtParser& parser, int depth);
	virtual i8 getId() const { return 9; }
	virtual String toString() const;

	void appendTag(NBTBase* pNBT);
	NBTBase* removeTag(int index);
	NBTBase* tagAt(int index);
	int tagCount() const { return m_list.size(); }

	virtual NBTBase* copy();
	virtual bool equals(const NBTBase* rhs) const;
	virtual int hashCode() const;

public:
	i8	tagType;
	TagList m_list;
};

class NBTTagCompound : public NBTBase
{
public:
	typedef map<String, NBTBase*>::type TagMap;
public:
	NBTTagCompound();
	NBTTagCompound(const String& par1Str);
	~NBTTagCompound();

	void release();

	virtual void write(DataStream* pData);
	virtual void load(DataStream* pData, int par2);
	virtual void load(NbtParser& parser, int depth);
	virtual i8 getId() const { return 10; }
	TagMap& getTags() { return tagMap; }

	void setTag(const String& tagName, NBTBase* pNBT);
	void setByte(const String& tagName, i8 val);
	void setShort(const String& tagName, short val);
	void setInteger(const String& tagName, int val);
	void setLong(const String& tagName, i64 val);
	void setFloat(const String& tagName, float val);
	void setDouble(const String& tagName, double val);
	void setString(const String& tagName, const String& val);
	void setByteArray(const String& tagName, i8* arr, int arrSize);
	void setIntArray(const String& tagName, int* arr, int arrSize);
	void setCompoundTag(const String& tagName, NBTTagCompound* pCompound);
	void setBool(const String& tagName, bool val);

	NBTBase* getTag(const String& tagName) const;
	bool hasKey(const String& tagName) const;
	void removeTag(const String& tagName);

	i8 getByte(const String& tagName) const;
	short getShort(const String& tagName) const;
	int getInteger(const String& tagName) const;
	i64 getLong(const String& tagName) const;
	float getFloat(const String& tagName) const;
	double getDouble(const String& tagName) const;
	String getString(const String& tagName) const;
	i8* getByteArray(const String& tagName) const;
	int* getIntArray(const String& tagName) const;
	int getArraySize(const String& tagName) const;
	NBTTagCompound* getCompoundTag(const String& tagName) const;
	NBTTagList* getTagList(const String& tagName) const;
	bool getBool(const String& tagName) const;

	virtual String toString() const;
	bool hasNoTags() const { return tagMap.empty(); }
	virtual NBTBase* copy();
	virtual bool equals(const NBTBase* rhs) const;
	virtual int hashCode() const;

protected:
	TagMap tagMap;
};

class NBTTagIntArray : public NBTBase
{
public:
	NBTTagIntArray(const String& par1Str);
	NBTTagIntArray(const String& par1Str, int* arr, int arrSize);
	~NBTTagIntArray();

	virtual void write(DataStream* pData);
	virtual void load(DataStream* pData, int par2);
	virtual void load(NbtParser& parser, int depth);
	virtual i8 getId() const { return 11; }
	virtual String toString() const;
	virtual NBTBase* copy();
	virtual bool equals(const NBTBase* rhs) const;
	virtual int hashCode() const;

public:
	int* intArray;
	int arraySize;
};

}

#endif
