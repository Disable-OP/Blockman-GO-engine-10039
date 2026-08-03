#ifndef __NBT_BINDING_HEADER__
#define __NBT_BINDING_HEADER__

#include <functional>
#include "Core.h"
#include "Nbt/NbtParser.h"
#include "Nbt/NbtGenerator.h"
#include "Functional/AssignOp.h"
#include "Functional/IdentityOp.h"

using namespace LORD;

namespace BLOCKMAN
{

	template<typename BindedClass>
	class NbtBinding;

	template<>
	class NbtBinding<void>
	{
		template<typename T, typename U>
		friend class NbtBindingBase;

	private:
		bool handleNbtTagEvent(const NbtTagEvent&, NbtParser &, void*)
		{
			return false;
		}

	public:
		static NbtBinding<void>* getInstance()
		{
			static NbtBinding<void> binding;
			return &binding;
		}

		bool deserialize(void* pObject, NbtParser & parser, const bool startWithCompoundTag)
		{
			return false;
		}

		void serialize(const void* pObject, NbtGenerator &generator, const bool encloseInCompoundTag)
		{
		}
	};

	template<typename BindedClass, typename BindedBaseClass>
	class NbtBindingBase
	{
	private:
		using MemberSerializer = std::function<void(const BindedClass*, NbtGenerator&)>;
		using MemberDeserializer = std::function<bool(BindedClass*, const NbtTagEvent&, NbtParser&)>;
		typename vector<MemberSerializer>::type memberSerializers;
		typename vector<MemberDeserializer>::type memberDeserializers;

	protected:
		NbtBindingBase() = default;

		bool handleNbtTagEvent(const NbtTagEvent& event, NbtParser &parser, BindedClass* pObject)
		{
			for (auto deserializer : memberDeserializers)
			{
				if (deserializer(pObject, event, parser))
				{
					return true;
				}
			}
			return NbtBinding<BindedBaseClass>::getInstance()->handleNbtTagEvent(event, parser, pObject);
		}

		template<typename TagReader, typename TagWriter, typename MemberType, typename Class, typename AssignOp = assign_op_t, typename ConvertOp = identity_op_t>
		void registerBinding(
			MemberType Class::* memberPtr,
			const char* tagName,
			const std::pair<AssignOp, ConvertOp>& converters = { assign_op, identity_op })
		{
			MemberType BindedClass::* castedMemberPtr = memberPtr;
			memberDeserializers.push_back([castedMemberPtr, tagName, converters](BindedClass* pObject, const NbtTagEvent& event, NbtParser& parser) -> bool {
				if (strcmp(event.tagName, tagName) != 0)
				{
					return false;
				}
				if (event.tagType != TagReader::tagType)
				{
					// LordLogWarning("The tag %s is expected to be of tag type %d", event.tagName, event.tagType);
					return false;
				}
				TagReader::template read<MemberType>(pObject->*castedMemberPtr, event, parser, converters.first);
				return true;
			});
			memberSerializers.push_back([castedMemberPtr, tagName, converters](const BindedClass* pObject, NbtGenerator& generator) {
				TagWriter::template write<MemberType>(pObject->*castedMemberPtr, tagName, generator, converters.second);
			});
		}

		void registerCustomHandler(const char* tagName, 
			std::function<void(BindedClass*, const NbtTagEvent&, NbtParser&)> reader, 
			std::function<void(const BindedClass*, const String&, NbtGenerator&)> writer)
		{
			memberDeserializers.push_back([tagName, reader](BindedClass* pObject, const NbtTagEvent& event, NbtParser& parser) -> bool {
				if (strcmp(event.tagName, tagName) != 0)
				{
					return false;
				}
				reader(pObject, event, parser);
				return true;
			});
			memberSerializers.push_back(std::bind(writer, std::placeholders::_1, String(tagName), std::placeholders::_2));
		}

	public:
		static NbtBinding<BindedClass>* getInstance();

		bool deserialize(BindedClass* pObject, NbtParser & parser, const bool startWithCompoundTag)
		{
			if (!parser.isStarted())
			{
				parser.nextEvent();
			}
			if (startWithCompoundTag)
			{
				if (!parser.currentEvent().isCompoundStart())
				{
					return false;
				}
				parser.nextEvent();
			}
			for (; !parser.currentEvent().isCompoundEnd(); parser.nextEvent())
			{
				if (this->handleNbtTagEvent(parser.currentEvent(), parser, pObject))
				{
					continue;
				}
				// TODO: log the ignored tag
				if (parser.currentEvent().isCompoundStart() || parser.currentEvent().isListStart())
				{
					parser.jumpToEndEvent();
				}
			}
			return true;
		}

		void serialize(const BindedClass* pObject, NbtGenerator &generator, const bool encloseInCompoundTag)
		{
			if (encloseInCompoundTag)
			{
				generator.writeTagCompoundStart();
			}
			NbtBinding<BindedBaseClass>::getInstance()->serialize(pObject, generator, false);
			for (auto serializer : memberSerializers)
			{
				serializer(pObject, generator);
			}
			if (encloseInCompoundTag)
			{
				generator.writeTagCompoundEnd();
			}
		}
	};

	template<typename BindedClass, typename BindedBaseClass>
	inline NbtBinding<BindedClass>* NbtBindingBase<BindedClass, BindedBaseClass>::getInstance()
	{
		static NbtBinding<BindedClass> binding;
		return &binding;
	}

	template<typename TagReader, typename TagWriter>
	struct TagOperators
	{
		using Reader = TagReader;
		using Writer = TagWriter;
	};
}
#endif // !__NBT_BINDING_HEADER__
