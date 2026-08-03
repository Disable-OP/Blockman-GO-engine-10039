#ifndef __NBT_TAG_WRITER_HEADER__
#define __NBT_TAG_WRITER_HEADER__

#include <functional>
#include <algorithm>
#include "Core.h"
#include "Std/iterator.h"
#include "Nbt/Serializer.h"
#include "Nbt/NBT.h"
#include "Nbt/NbtGenerator.h"
#include "Functional/IdentityOp.h"
#include "Util/ContainerUtils.h"
#include "TypeTraits/IsTemplateOf.h"
#include "TypeTraits/ContainerTraits.h"

namespace BLOCKMAN
{
	template<NbtTagType _tagType>
	struct SimpleNbtTagWriter {};

#define BLOCKMAN_SIMPLE_NBT_TAG_WRITER(                                        \
	_tagType, _eventProperty, _writeMethod)                                    \
template<>                                                                     \
struct SimpleNbtTagWriter<NbtTagType::_tagType>                                \
{                                                                              \
	template<typename _PropertyType>                                           \
	using ConvertOpType = std::function<                                       \
		decltype(NbtTagEvent()._eventProperty)(const _PropertyType&)>;         \
	static constexpr NbtTagType tagType = NbtTagType::_tagType;                \
	template<typename _PropertyType>                                           \
	inline static void write(const _PropertyType &property,                    \
		const String &tagName, NbtGenerator &generator,                        \
		const ConvertOpType<_PropertyType> &convert = identity_op)             \
	{                                                                          \
		generator._writeMethod(tagName, convert(property));                    \
	}                                                                          \
};
	BLOCKMAN_SIMPLE_NBT_TAG_WRITER(TAG_BYTE, byteValue, writeTagByte)
	BLOCKMAN_SIMPLE_NBT_TAG_WRITER(TAG_SHORT, shortValue, writeTagShort)
	BLOCKMAN_SIMPLE_NBT_TAG_WRITER(TAG_INT, intValue, writeTagInt)
	BLOCKMAN_SIMPLE_NBT_TAG_WRITER(TAG_LONG, longValue, writeTagLong)
	BLOCKMAN_SIMPLE_NBT_TAG_WRITER(TAG_FLOAT, floatValue, writeTagFloat)
	BLOCKMAN_SIMPLE_NBT_TAG_WRITER(TAG_DOUBLE, doubleValue, writeTagDouble)
	BLOCKMAN_SIMPLE_NBT_TAG_WRITER(TAG_STRING, stringValue, writeTagString)

	template<NbtTagType _tagType>
	struct RawNbtTagWriter
	{
		static constexpr NbtTagType tagType = _tagType;
		template<typename _PropertyType>
		using ConvertOpType = std::function<void(const _PropertyType &, const String &, NbtGenerator &)>;
		template<typename _PropertyType>
		inline static void write(const _PropertyType &property, const String &tagName, NbtGenerator &generator, const ConvertOpType<_PropertyType> &writer)
		{
			writer(property, tagName, generator);
		}
	};

	template<typename _Class>
	struct CompoundNbtTagWriter {
		template<typename _PropertyType>
		using ConvertOpType = std::function<_Class*(const _PropertyType&)>;
		static constexpr NbtTagType tagType = NbtTagType::TAG_COMPOUND;
		template<typename _PropertyType>
		static void write(const _PropertyType &property, const String &tagName, NbtGenerator &generator, const ConvertOpType<_PropertyType> &convert = identity_op);
	};

	template<typename _SubNbtTagWriter>
	struct ListNbtTagWriter {
	public:
		template<typename _Container>
		using ConvertOpType = typename _SubNbtTagWriter::template ConvertOpType<typename container_traits<_Container>::value_type>;
		static constexpr NbtTagType tagType = NbtTagType::TAG_LIST;
		template<typename _Container>
		inline static void write(const _Container &property, const String &tagName, NbtGenerator &generator, const ConvertOpType<_Container> &convert = identity_op)
		{
			delegateWrite(property, tagName, generator, convert, isListOfCompound);
		}
	private:
		static is_template_of<CompoundNbtTagWriter, _SubNbtTagWriter> isListOfCompound;
		template<typename _Container>
		static void delegateWrite(const _Container &property, const String &tagName, NbtGenerator &generator, const ConvertOpType<_Container> &convert, const std::true_type &);
		template<typename _Container>
		static void delegateWrite(const _Container &property, const String &tagName, NbtGenerator &generator, const ConvertOpType<_Container> &convert, const std::false_type &);
	};

	template<typename _SubNbtTagWriter>
	is_template_of<CompoundNbtTagWriter, _SubNbtTagWriter> ListNbtTagWriter<_SubNbtTagWriter>::isListOfCompound{};

	template<typename _SubNbtTagWriter>
	template<typename _Container>
	void ListNbtTagWriter<_SubNbtTagWriter>::delegateWrite(const _Container &property, const String &tagName,
		NbtGenerator &generator, const ConvertOpType<_Container> &convert, const std::true_type &)
	{
		unsigned count = 0;
		for (const auto &element : property)
		{
			if (element)
			{
				++count;
			}
		}
		generator.writeTagListStart(tagName, _SubNbtTagWriter::tagType, count);
		for (const auto &element : property)
		{
			if (element)
			{
				_SubNbtTagWriter::write(element, "", generator, convert);
			}
		}
	}

	template<typename _SubNbtTagWriter>
	template<typename _Container>
	void ListNbtTagWriter<_SubNbtTagWriter>::delegateWrite(const _Container &property, const String &tagName,
		NbtGenerator &generator, const ConvertOpType<_Container> &convert, const std::false_type &)
	{
		generator.writeTagListStart(tagName, _SubNbtTagWriter::tagType, size(property));
		for (const auto &element : property)
		{
			_SubNbtTagWriter::write(element, "", generator, convert);
		}
	}

	template<>
	struct SimpleNbtTagWriter<NbtTagType::TAG_BYTE_ARRAY> {
		template<typename _Container>
		using ConvertOpType = ListNbtTagWriter<SimpleNbtTagWriter<NbtTagType::TAG_BYTE>>::ConvertOpType<_Container>;
		static constexpr NbtTagType tagType = NbtTagType::TAG_BYTE_ARRAY;
		template<typename _Container>
		static void write(const _Container &property, const String &tagName, NbtGenerator &generator, const ConvertOpType<_Container> &convert = identity_op);
	};

	template<>
	struct SimpleNbtTagWriter<NbtTagType::TAG_INT_ARRAY> {
		template<typename _Container>
		using ConvertOpType = ListNbtTagWriter<SimpleNbtTagWriter<NbtTagType::TAG_INT>>::ConvertOpType<_Container>;
		static constexpr NbtTagType tagType = NbtTagType::TAG_INT_ARRAY;
		template<typename _Container>
		static void write(const _Container &property, const String &tagName, NbtGenerator &generator, const ConvertOpType<_Container> &convert = identity_op);
	};

	template<NbtTagType _tagType>
	struct HalfNbtTagWriter {};

	template<>
	struct HalfNbtTagWriter<NbtTagType::TAG_BYTE_ARRAY> {
		static constexpr NbtTagType tagType = NbtTagType::TAG_BYTE_ARRAY;
		template<typename _Container>
		using ConvertOpType = SimpleNbtTagWriter<NbtTagType::TAG_BYTE>::ConvertOpType<typename container_traits<_Container>::value_type>;
		template<typename _Container>
		static void write(const _Container &property, const String &tagName, NbtGenerator &generator, const ConvertOpType<_Container> &convert = identity_op);
	};

	using std::cbegin;
	using std::cend;
	using std::size;

	template<typename _Class>
	template<typename _PropertyType>
	void CompoundNbtTagWriter<_Class>::write(const _PropertyType &property, const String &tagName, NbtGenerator &generator, const ConvertOpType<_PropertyType> &convert)
	{
		if (property)
		{
			generator.writeTagCompoundStart(tagName);
			serialize<_Class>(convert(property), generator, false);
			generator.writeTagCompoundEnd();
		}
	}

	template<typename _Container>
	void SimpleNbtTagWriter<NbtTagType::TAG_BYTE_ARRAY>::write(const _Container &property, const String &tagName, NbtGenerator &generator, const ConvertOpType<_Container> &convert)
	{
		generator.writeTagByteArrayStart(tagName, size(property));
		using ElementType = typename container_traits<_Container>::value_type;
		std::for_each(cbegin(property), cend(property), [&generator, &convert](const ElementType &element)
		{
			SimpleNbtTagWriter<NbtTagType::TAG_BYTE>::write(element, "", generator, convert);
		});
	}

	template<typename _Container>
	void SimpleNbtTagWriter<NbtTagType::TAG_INT_ARRAY>::write(const _Container &property, const String &tagName, NbtGenerator &generator, const ConvertOpType<_Container> &convert)
	{
		generator.writeTagIntArrayStart(tagName, size(property));
		using ElementType = typename container_traits<_Container>::value_type;
		std::for_each(cbegin(property), cend(property), [&generator, &convert](const ElementType &element)
		{
			SimpleNbtTagWriter<NbtTagType::TAG_INT>::write(element, "", generator, convert);
		});
	}

	template<typename _Container>
	void HalfNbtTagWriter<NbtTagType::TAG_BYTE_ARRAY>::write(const _Container &property, const String &tagName, NbtGenerator &generator, const ConvertOpType<_Container> &convert)
	{
		generator.writeTagByteArrayStart(tagName, (size(property) + 1) / 2);
		using ValueType = typename ConvertOpType<_Container>::result_type;
		const unsigned shift = sizeof(ValueType) * 4;
		const unsigned mask = (1 << shift) - 1;
		for (auto iter = cbegin(property), end = cend(property); iter != end; ++iter)
		{
			ValueType value = convert(*iter);
			value &= mask;
			value <<= shift;
			if (++iter != end)
			{
				value |= convert(*iter) & mask;
				SimpleNbtTagWriter<NbtTagType::TAG_BYTE>::write(value, "", generator);
			}
			else
			{
				value >>= shift;
				SimpleNbtTagWriter<NbtTagType::TAG_BYTE>::write(value, "", generator);
			}
		}
	}
}

#endif
