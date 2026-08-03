#ifndef __NBT_TAG_READER_HEADER__
#define __NBT_TAG_READER_HEADER__

#include <functional>
#include "Nbt/NBT.h"
#include "Nbt/NbtParser.h"
#include "Functional/AssignOp.h"
#include "Util/ContainerUtils.h"
#include "TypeTraits/FunctionTraits.h"
#include "TypeTraits/ContainerTraits.h"
#include "Nbt/Serializer.h"

namespace BLOCKMAN
{
	template<NbtTagType _tagType>
	struct SimpleNbtTagReader {};

#define BLOCKMAN_SIMPLE_NBT_TAG_READER(_tagType, _eventProperty)               \
template<>                                                                     \
struct SimpleNbtTagReader<NbtTagType::_tagType> {                              \
	template<typename _PropertyType>                                           \
	using AssignOpType = std::function<void(_PropertyType&,                    \
		const std::remove_reference_t<                                         \
			decltype(NbtTagEvent()._eventProperty)>&)>;                        \
	static constexpr NbtTagType tagType = NbtTagType::_tagType;                \
	template<typename _PropertyType>                                           \
	inline static void read(                                                   \
		_PropertyType &property, const NbtTagEvent &event, NbtParser &,        \
		const AssignOpType<_PropertyType> &assign = assign_op)                 \
	{                                                                          \
		assign(property, getEventValue(event));                                \
	}                                                                          \
	inline static decltype(NbtTagEvent()._eventProperty)                       \
	getEventValue(const NbtTagEvent &event)                                    \
	{                                                                          \
		return event._eventProperty;                                           \
	}                                                                          \
};
	BLOCKMAN_SIMPLE_NBT_TAG_READER(TAG_BYTE, byteValue)
	BLOCKMAN_SIMPLE_NBT_TAG_READER(TAG_SHORT, shortValue)
	BLOCKMAN_SIMPLE_NBT_TAG_READER(TAG_INT, intValue)
	BLOCKMAN_SIMPLE_NBT_TAG_READER(TAG_LONG, longValue)
	BLOCKMAN_SIMPLE_NBT_TAG_READER(TAG_FLOAT, floatValue)
	BLOCKMAN_SIMPLE_NBT_TAG_READER(TAG_DOUBLE, doubleValue)
	BLOCKMAN_SIMPLE_NBT_TAG_READER(TAG_STRING, stringValue)

	template<NbtTagType _tagType>
	struct RawNbtTagReader
	{
		static constexpr NbtTagType tagType = _tagType;
		template<typename _PropertyType>
		using AssignOpType = std::function<void(_PropertyType &, const NbtTagEvent &, NbtParser &)>;
		template<typename _PropertyType>
		inline static void read(_PropertyType &property, const NbtTagEvent &event, NbtParser &parser, const AssignOpType<_PropertyType> &assign)
		{
			assign(property, event, parser);
		}
	};

	template<typename _Class>
	struct CompoundNbtTagReader {
		template<typename _PropertyType>
		using AssignOpType = std::function<void(_PropertyType&, _Class*)>;
		static constexpr NbtTagType tagType = NbtTagType::TAG_COMPOUND;
		template<typename _PropertyType>
		inline static void read(_PropertyType &property, const NbtTagEvent &event, NbtParser &parser, const AssignOpType<_PropertyType> &assign = assign_op)
		{
			std::unique_ptr<_Class> object(new _Class);
			if (!deserialize(object.get(), parser))
			{
				object.reset();
			}
			assign(property, object.release());
		}
	};

	template<typename _SubNbtTagReader>
	struct ListNbtTagReader {
		template<typename _Container>
		using AssignOpType = typename _SubNbtTagReader::template AssignOpType<typename container_traits<_Container>::value_type>;
		static constexpr NbtTagType tagType = NbtTagType::TAG_LIST;
		template<typename _Container>
		static void read(_Container &property, const NbtTagEvent &_event, NbtParser &parser, const AssignOpType<_Container> &assign = assign_op);
	};

	template<>
	struct SimpleNbtTagReader<NbtTagType::TAG_BYTE_ARRAY> {
		template<typename _Container>
		using AssignOpType = ListNbtTagReader<SimpleNbtTagReader<NbtTagType::TAG_BYTE>>::AssignOpType<_Container>;
		static constexpr NbtTagType tagType = NbtTagType::TAG_BYTE_ARRAY;
		template<typename _ListContainer>
		inline static void read(_ListContainer &property, const NbtTagEvent &event, NbtParser & parser, const AssignOpType<_ListContainer> &assign = assign_op)
		{
			ListNbtTagReader<SimpleNbtTagReader<NbtTagType::TAG_BYTE>>::read(property, event, parser, assign);
		}
	};

	template<>
	struct SimpleNbtTagReader<NbtTagType::TAG_INT_ARRAY> {
		template<typename _Container>
		using AssignOpType = ListNbtTagReader<SimpleNbtTagReader<NbtTagType::TAG_INT>>::AssignOpType<_Container>;
		static constexpr NbtTagType tagType = NbtTagType::TAG_INT_ARRAY;
		template<typename _ListContainer>
		inline static void read(_ListContainer &property, const NbtTagEvent &event, NbtParser & parser, const AssignOpType<_ListContainer> &assign = assign_op)
		{
			ListNbtTagReader<SimpleNbtTagReader<NbtTagType::TAG_INT>>::read(property, event, parser, assign);
		}
	};

	template<NbtTagType _tagType>
	struct HalfNbtTagReader {};

	template<>
	struct HalfNbtTagReader<NbtTagType::TAG_BYTE_ARRAY> {
		static constexpr NbtTagType tagType = NbtTagType::TAG_BYTE_ARRAY;
		template<typename _Container>
		using AssignOpType = SimpleNbtTagReader<NbtTagType::TAG_BYTE>::AssignOpType<typename container_traits<_Container>::value_type>;
		template<typename _Container>
		static void read(_Container &property, const NbtTagEvent &_event, NbtParser &parser, const AssignOpType<_Container> & assign = assign_op);
	};

	template<typename _SubNbtTagReader>
	template<typename _Container>
	void ListNbtTagReader<_SubNbtTagReader>::read(_Container &property, const NbtTagEvent &_event, NbtParser &parser, const AssignOpType<_Container> &assign)
	{
		if (_event.listLength > max_size(property))
		{
			throw InvalidNbtFormatError("list is too long");
		}
		// in case property is a vector, access the last element to allocate enough memory
		// maybe vector is not used now, so adding the following line makes it slower
		//element_at(property, _event.listLength - 1);
		parser.nextEvent();
		for (typename container_traits<_Container>::size_type i = 0;
			!parser.currentEvent().isListEnd() && i < max_size(property);
			parser.nextEvent(), ++i)
		{
			_SubNbtTagReader::read(element_at(property, i), parser.currentEvent(), parser, assign);
		}
	}

	template<typename _Container>
	void HalfNbtTagReader<NbtTagType::TAG_BYTE_ARRAY>::read(_Container &property, const NbtTagEvent &_event, NbtParser &parser, const AssignOpType<_Container> & assign)
	{
		if (_event.listLength * 2 > max_size(property))
		{
			throw InvalidNbtFormatError("list is too long");
		}
		// in case property is a vector, access the last element to allocate enough memory
		// maybe vector is not used now, so adding the following line makes it slower
		//element_at(property, _event.listLength * 2 - 1);
		auto valueBitSize = sizeof(typename function_traits<AssignOpType<_Container>>::template argument_type<1>) * 4;
		auto mask = (1 << valueBitSize) - 1;
		parser.nextEvent();
		for (typename container_traits<_Container>::size_type i = 0;
			!parser.currentEvent().isListEnd() && i < max_size(property);
			parser.nextEvent(), ++i)
		{
			assign(element_at(property, i), SimpleNbtTagReader<NbtTagType::TAG_BYTE>::getEventValue(parser.currentEvent()) >> valueBitSize & mask);
			if (++i < max_size(property))
			{
				assign(element_at(property, i), SimpleNbtTagReader<NbtTagType::TAG_BYTE>::getEventValue(parser.currentEvent()) & mask);
			}
		}
	}
}

#endif