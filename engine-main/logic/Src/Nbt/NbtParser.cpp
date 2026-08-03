#include "NbtParser.h"

#include "Std/iterator.h"

namespace BLOCKMAN
{

	void NbtParser::parseIncompleteTag(const NbtTagType &tagType) {
		switch (tagType) {
		case NbtTagType::TAG_BYTE:
			parseIncompleteTag<NbtTagType::TAG_BYTE>();
			break;
		case NbtTagType::TAG_SHORT:
			parseIncompleteTag<NbtTagType::TAG_SHORT>();
			break;
		case NbtTagType::TAG_INT:
			parseIncompleteTag<NbtTagType::TAG_INT>();
			break;
		case NbtTagType::TAG_LONG:
			parseIncompleteTag<NbtTagType::TAG_LONG>();
			break;
		case NbtTagType::TAG_FLOAT:
			parseIncompleteTag<NbtTagType::TAG_FLOAT>();
			break;
		case NbtTagType::TAG_DOUBLE:
			parseIncompleteTag<NbtTagType::TAG_DOUBLE>();
			break;
		case NbtTagType::TAG_STRING:
			parseIncompleteTag<NbtTagType::TAG_STRING>();
			break;
		case NbtTagType::TAG_BYTE_ARRAY:
			parseIncompleteTag<NbtTagType::TAG_BYTE_ARRAY>();
			break;
		case NbtTagType::TAG_INT_ARRAY:
			parseIncompleteTag<NbtTagType::TAG_INT_ARRAY>();
			break;
		case NbtTagType::TAG_LIST:
			parseIncompleteTag<NbtTagType::TAG_LIST>();
			break;
		case NbtTagType::TAG_COMPOUND:
			parseIncompleteTag<NbtTagType::TAG_COMPOUND>();
			break;
		default:
			LordLogWarning("value TAG_END is tried to be read");
			break;
		}
	}

	void NbtParser::finishParseListTag()
	{
		parseNextTag = nestedEventStack.top().parseNextTag;
		event.tagType = nestedEventStack.top().tagType;
		event.tagSubtype = NbtTagEvent::NbtTagSubtype::END;
		nestedEventStack.pop();
		if (!nestedEventStack.empty())
		{
			itemLeftInList = &nestedEventStack.top().itemLeftInList;
		}
	}

	void NbtParser::jumpToEndEvent()
	{
		if (!started)
		{
			throw std::logic_error("parser is not started yet");
		}
		auto stackSize = nestedEventStack.size();
		while (stackSize <= nestedEventStack.size())
		{
			nextEvent();
		}
	}

	void NbtParser::parseCompleteTag()
	{
		readBinary(event.tagType, input);
		if (event.tagType == NbtTagType::TAG_END)
		{
			parseIncompleteTag<NbtTagType::TAG_END>();
			return;
		}

		uint16_t strLen;
		readBinary(strLen, input);
		if (strLen > NbtTagEvent::MAX_TAG_NAME_SIZE)
		{
			throw std::runtime_error("tag name is too long");
		}
		if (strLen > 0)
		{
			if (!input.read(event.tagName, strLen))
			{
				throw StreamError("NbtParser::parseCompleteTag->failed to read input stream");
			}
		}
		event.tagName[strLen] = '\0';

		parseIncompleteTag(event.tagType);
	}
}
