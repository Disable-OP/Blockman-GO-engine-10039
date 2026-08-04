#ifndef __NBT_PARSER_HEADER__
#define __NBT_PARSER_HEADER__

#include <istream>
#include <stack>

#include "Nbt/NBT.h"
#include "Util/IO.h"
#include "Stream/ZlibStream.h"

#include "Core.h"

namespace BLOCKMAN
{

	template<typename InputStream>
	inline void readBinary(NbtTagType &value, InputStream &is)
	{
		uint8_t v;
		readBinary(v, is);
		value = static_cast<NbtTagType>(v);
	}

	class NbtTagEvent
	{
	public:
		enum class NbtTagSubtype
		{
			START,
			END
		};
		static constexpr size_t MAX_TAG_NAME_SIZE = 256;

		NbtTagType tagType = NbtTagType::TAG_END;
		NbtTagSubtype tagSubtype = NbtTagSubtype::END;
		char tagName[MAX_TAG_NAME_SIZE + 1] = { 0 };
		int8_t byteValue = 0;
		int16_t shortValue = 0;
		int32_t intValue = 0;
		int64_t longValue = 0;
		float floatValue = 0.0f;
		double doubleValue = 0.0f;
		NbtTagType listTagType = NbtTagType::TAG_END;
		unsigned listLength = 0;
		String stringValue;

		inline bool isCompoundStart() const
		{
			return tagType == NbtTagType::TAG_COMPOUND && tagSubtype == NbtTagSubtype::START;
		}

		inline bool isCompoundEnd() const
		{
			return tagType == NbtTagType::TAG_COMPOUND && tagSubtype == NbtTagSubtype::END;
		}

		inline bool isListType() const
		{
			return tagType == NbtTagType::TAG_LIST || tagType == NbtTagType::TAG_BYTE_ARRAY || tagType == NbtTagType::TAG_INT_ARRAY;
		}

		inline bool isListStart() const
		{
			return isListType() && tagSubtype == NbtTagSubtype::START;
		}

		inline bool isListEnd() const
		{
			return isListType() && tagSubtype == NbtTagSubtype::END;
		}
	};

    class NbtParser
    {
	private:
		using ParseNextTagFunction = void(NbtParser::*)();

	public:
		using StreamType = ZlibInputStream;

    private:
        struct NestingRecord 
        {
			NbtTagType tagType;
            int itemLeftInList;
			ParseNextTagFunction parseNextTag;

			NestingRecord(const NbtTagEvent& event, ParseNextTagFunction parseNextTag)
			{
				tagType = event.tagType;
				this->parseNextTag = parseNextTag;
				itemLeftInList = event.listLength;
			}
        };

		//struct Cursor
		//{
		//	std::streampos streamPos;
		//	NbtTagEvent event;
		//};

		std::stack<NestingRecord, typename vector<NestingRecord>::type> nestedEventStack;
		StreamType &input;
        NbtTagEvent event;
        bool started = false;
		ParseNextTagFunction parseNextTag;
		int* itemLeftInList = nullptr;

		void parseCompleteTag();
        void parseIncompleteTag(const NbtTagType &tagType);
		void finishParseListTag();


		template<NbtTagType tagType>
		inline void parseIncompleteTag();

		template<NbtTagType tagType>
		inline void parseIncompleteTagInList()
		{
			if ((*itemLeftInList)-- <= 0)
			{
				finishParseListTag();
			}
			else
			{
				parseIncompleteTag<tagType>();
			}
		}

		template<NbtTagType tagType>
		inline void parseFirstIncompleteTagInList()
		{
			event.tagType = tagType;
			event.tagName[0] = '\0';
			parseNextTag = &NbtParser::parseIncompleteTagInList<tagType>;
			parseIncompleteTagInList<tagType>();
		}

		void parseFirstTag()
		{
			started = true;
			parseNextTag = &NbtParser::parseCompleteTag;
			parseCompleteTag();
		}

    public:
        explicit NbtParser(StreamType &input) : input(input), parseNextTag(&NbtParser::parseFirstTag) {}

		//inline Cursor tellg()
		//{
		//	return { input.tellg(), event };
		//}

		//inline void seekg(Cursor cursor)
		//{
		//	input.seekg(cursor.streamPos);
		//	event = cursor.event;
		//}

        inline NbtTagEvent& currentEvent()
        {
            return event;
        }

        inline bool isStarted()
        {
            return started;
        }

        void jumpToEndEvent();

		void nextEvent()
		{
			(this->*parseNextTag)();
		}
    };

	template<>
	inline void NbtParser::parseIncompleteTag<NbtTagType::TAG_END>()
	{
		event.tagName[0] = '\0';
		event.tagType = NbtTagType::TAG_COMPOUND;
		event.tagSubtype = NbtTagEvent::NbtTagSubtype::END;
		parseNextTag = nestedEventStack.top().parseNextTag;
		nestedEventStack.pop();
		if (!nestedEventStack.empty())
		{
			itemLeftInList = &nestedEventStack.top().itemLeftInList;
		}
	}
	template<>
	inline void NbtParser::parseIncompleteTag<NbtTagType::TAG_BYTE>()
	{
		readBinary(event.byteValue, input);
	}
	template<>
	inline void NbtParser::parseIncompleteTag<NbtTagType::TAG_SHORT>()
	{
		readBinary(event.shortValue, input);
	}
	template<>
	inline void NbtParser::parseIncompleteTag<NbtTagType::TAG_INT>()
	{
		readBinary(event.intValue, input);
	}
	template<>
	inline void NbtParser::parseIncompleteTag<NbtTagType::TAG_LONG>()
	{
		readBinary(event.longValue, input);
	}
	template<>
	inline void NbtParser::parseIncompleteTag<NbtTagType::TAG_FLOAT>()
	{
		readBinary(event.floatValue, input);
	}
	template<>
	inline void NbtParser::parseIncompleteTag<NbtTagType::TAG_DOUBLE>()
	{
		readBinary(event.doubleValue, input);
	}
	template<>
	inline void NbtParser::parseIncompleteTag<NbtTagType::TAG_STRING>()
	{
		constexpr size_t BUFFER_SIZE = 1024;
		static char buffer[BUFFER_SIZE + 1] = {};
		uint16_t strLen;
		readBinary(strLen, input);
		event.stringValue.reserve(strLen);
		event.stringValue.clear();
		while (strLen > BUFFER_SIZE)
		{
			if (!input.read(buffer, BUFFER_SIZE))
			{
				throw StreamError("NbtParser::parseIncompleteTag1->failed to read input stream");
			}
			event.stringValue.append(buffer);
			strLen -= BUFFER_SIZE;
		}
		if (!input.read(buffer, strLen))
		{
			throw StreamError("NbtParser::parseIncompleteTag2->failed to read input stream");
		}
		buffer[strLen] = 0;
		event.stringValue.append(buffer);
	}
	template<>
	inline void NbtParser::parseIncompleteTag<NbtTagType::TAG_BYTE_ARRAY>()
	{
		int32_t listLen;
		readBinary(listLen, input);
		event.listLength = listLen;
		event.listTagType = NbtTagType::TAG_BYTE;
		event.tagSubtype = NbtTagEvent::NbtTagSubtype::START;
		nestedEventStack.push({ event, parseNextTag });
		itemLeftInList = &nestedEventStack.top().itemLeftInList;
		parseNextTag = &NbtParser::parseFirstIncompleteTagInList<NbtTagType::TAG_BYTE>;
	}
	template<>
	inline void NbtParser::parseIncompleteTag<NbtTagType::TAG_INT_ARRAY>()
	{
		int32_t listLen;
		readBinary(listLen, input);
		event.listLength = listLen;
		event.listTagType = NbtTagType::TAG_INT;
		event.tagSubtype = NbtTagEvent::NbtTagSubtype::START;
		nestedEventStack.push({ event, parseNextTag });
		itemLeftInList = &nestedEventStack.top().itemLeftInList;
		parseNextTag = &NbtParser::parseFirstIncompleteTagInList<NbtTagType::TAG_INT>;
	}
	template<>
	inline void NbtParser::parseIncompleteTag<NbtTagType::TAG_LIST>()
	{
		int32_t listLen;
		readBinary(event.listTagType, input);
		readBinary(listLen, input);
		event.listLength = listLen;
		event.tagSubtype = NbtTagEvent::NbtTagSubtype::START;
		nestedEventStack.push({ event, parseNextTag });
		itemLeftInList = &nestedEventStack.top().itemLeftInList;
		switch (event.listTagType)
		{
		case NbtTagType::TAG_END:
			if (listLen != 0)
			{
				LordLogError("Tag List of Tag End should be zero length");
			}
			parseNextTag = &NbtParser::parseFirstIncompleteTagInList<NbtTagType::TAG_END>;
			break;
		case NbtTagType::TAG_BYTE:
			parseNextTag = &NbtParser::parseFirstIncompleteTagInList<NbtTagType::TAG_BYTE>;
			break;
		case NbtTagType::TAG_SHORT:
			parseNextTag = &NbtParser::parseFirstIncompleteTagInList<NbtTagType::TAG_SHORT>;
			break;
		case NbtTagType::TAG_INT:
			parseNextTag = &NbtParser::parseFirstIncompleteTagInList<NbtTagType::TAG_INT>;
			break;
		case NbtTagType::TAG_LONG:
			parseNextTag = &NbtParser::parseFirstIncompleteTagInList<NbtTagType::TAG_LONG>;
			break;
		case NbtTagType::TAG_FLOAT:
			parseNextTag = &NbtParser::parseFirstIncompleteTagInList<NbtTagType::TAG_FLOAT>;
			break;
		case NbtTagType::TAG_DOUBLE:
			parseNextTag = &NbtParser::parseFirstIncompleteTagInList<NbtTagType::TAG_DOUBLE>;
			break;
		case NbtTagType::TAG_STRING:
			parseNextTag = &NbtParser::parseFirstIncompleteTagInList<NbtTagType::TAG_STRING>;
			break;
		case NbtTagType::TAG_BYTE_ARRAY:
			parseNextTag = &NbtParser::parseFirstIncompleteTagInList<NbtTagType::TAG_BYTE_ARRAY>;
			break;
		case NbtTagType::TAG_INT_ARRAY:
			parseNextTag = &NbtParser::parseFirstIncompleteTagInList<NbtTagType::TAG_INT_ARRAY>;
			break;
		case NbtTagType::TAG_LIST:
			parseNextTag = &NbtParser::parseFirstIncompleteTagInList<NbtTagType::TAG_LIST>;
			break;
		case NbtTagType::TAG_COMPOUND:
			parseNextTag = &NbtParser::parseFirstIncompleteTagInList<NbtTagType::TAG_COMPOUND>;
			break;
		default:
			LordLogWarning("parseing invalid incomplete tag");
			break;
		}
	}
	template<>
	inline void NbtParser::parseIncompleteTag<NbtTagType::TAG_COMPOUND>()
	{
		event.tagSubtype = NbtTagEvent::NbtTagSubtype::START;
		nestedEventStack.push({ event, parseNextTag });
		parseNextTag = &NbtParser::parseCompleteTag;
	}

}
#endif
