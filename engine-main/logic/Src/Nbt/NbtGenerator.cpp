#include "NbtGenerator.h"

namespace BLOCKMAN
{

#define WRITE_SIMPLE_TAG_METHOD(_name, _valueType, _tagType)                   \
void NbtGenerator::_name(const String &name, const _valueType value)           \
{                                                                              \
	checkInsideListAndWriteHead(name, NbtTagType::_tagType);                   \
	writeBinary(value, output);                                                \
	updateCurrentListStatus();                                                 \
}

	WRITE_SIMPLE_TAG_METHOD(writeTagByte, uint8_t, TAG_BYTE)
	WRITE_SIMPLE_TAG_METHOD(writeTagShort, uint16_t, TAG_SHORT)
	WRITE_SIMPLE_TAG_METHOD(writeTagInt, uint32_t, TAG_INT)
	WRITE_SIMPLE_TAG_METHOD(writeTagLong, uint64_t, TAG_LONG)
	WRITE_SIMPLE_TAG_METHOD(writeTagFloat, float, TAG_FLOAT)
	WRITE_SIMPLE_TAG_METHOD(writeTagDouble, double, TAG_DOUBLE)

	void NbtGenerator::writeTagString(const String &name, const String &value)
	{
		checkInsideListAndWriteHead(name, NbtTagType::TAG_STRING);
		writeString(value);
		updateCurrentListStatus();
	}

	void NbtGenerator::writeTagByteArrayStart(const String &name, const uint32_t length)
	{
		checkInsideListAndWriteHead(name, NbtTagType::TAG_BYTE_ARRAY);
		writeBinary(length, output);
		if (length > 0)
		{
			nestedWriteStack.push({ NbtTagType::TAG_BYTE_ARRAY, length, 0 });
		}
		else
		{
			updateCurrentListStatus();
		}
	}

	void NbtGenerator::writeTagIntArrayStart(const String &name, const uint32_t length)
	{
		checkInsideListAndWriteHead(name, NbtTagType::TAG_INT_ARRAY);
		writeBinary(length, output);
		if (length > 0)
		{
			nestedWriteStack.push({ NbtTagType::TAG_INT_ARRAY, length, 0 });
		}
		else
		{
			updateCurrentListStatus();
		}
	}

	void NbtGenerator::writeTagListStart(const String &name, const NbtTagType tagType, const uint32_t length)
	{
		checkInsideListAndWriteHead(name, NbtTagType::TAG_LIST);
		writeBinary(tagType, output);
		writeBinary(length, output);
		if (length > 0)
		{
			nestedWriteStack.push({ NbtTagType::TAG_LIST, length, 0 });
		}
		else
		{
			updateCurrentListStatus();
		}
	}

	void NbtGenerator::writeTagCompoundStart(const String &name)
	{
		checkInsideListAndWriteHead(name, NbtTagType::TAG_COMPOUND);
		nestedWriteStack.push({ NbtTagType::TAG_COMPOUND, 0, 0 });
	}

	void NbtGenerator::writeTagCompoundEnd()
	{
		writeBinary(NbtTagType::TAG_END, output);
		nestedWriteStack.pop();
		updateCurrentListStatus();
	}
}