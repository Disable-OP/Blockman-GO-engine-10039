#ifndef __NBT_GENERATOR_HEADER__
#define __NBT_GENERATOR_HEADER__

#include <stack>
#include <ostream>

#include "Nbt/NBT.h"
#include "Util/IO.h"
#include "Core.h"
#include "Stream/ZlibStream.h"

namespace BLOCKMAN
{

	template<typename OutputStream>
	inline void writeBinary(const NbtTagType &value, OutputStream &os)
	{
		writeBinary(static_cast<uint8_t>(value), os);
	}

    class NbtGenerator
    {
	public:
		using OutputStream = ZlibOutputStream;
    private:
        struct NestingRecord
        {
            const NbtTagType tagType;
            const unsigned listLength;
            unsigned listItemWritten;
        };

		std::stack<NestingRecord, typename vector<NestingRecord>::type> nestedWriteStack;
		OutputStream &output;

        inline void writeString(const String &name)
        {
            writeBinary<uint16_t>(static_cast<uint16_t>(name.length()), output);
			output.write(name.c_str(), name.size());
        }

        inline void checkInsideListAndWriteHead(const String &name, NbtTagType tagType)
        {
            if (!insideList())
            {
                writeBinary(tagType, output);
                writeString(name);
            }
        }

        void updateCurrentListStatus()
        {
            if (insideList())
            {
                ++nestedWriteStack.top().listItemWritten;
                if (allListItemIsWritten())
                {
                    nestedWriteStack.pop();
                    updateCurrentListStatus();
                }
            }
        }

        inline bool insideList()
        {
            return !nestedWriteStack.empty()
                && (nestedWriteStack.top().tagType == NbtTagType::TAG_INT_ARRAY
                    || nestedWriteStack.top().tagType == NbtTagType::TAG_BYTE_ARRAY
                    || nestedWriteStack.top().tagType == NbtTagType::TAG_LIST);
        }

        inline bool allListItemIsWritten()
        {
            return nestedWriteStack.top().listItemWritten >= nestedWriteStack.top().listLength;
        }

    public:
		NbtGenerator(OutputStream &output) : output(output) {}

        void writeTagByte(const String &name, const uint8_t value);
        void writeTagShort(const String &name, const uint16_t value);
        void writeTagInt(const String &name, const uint32_t value);
        void writeTagLong(const String &name, const uint64_t value);
        void writeTagFloat(const String &name, const float value);
        void writeTagDouble(const String &name, const double value);
        void writeTagString(const String &name, const String &value);
        void writeTagByteArrayStart(const String &name, const uint32_t length);
        void writeTagIntArrayStart(const String &name, const uint32_t length);
        void writeTagListStart(const String &name, const NbtTagType tagType, const uint32_t length);
        void writeTagCompoundStart(const String &name);
        void writeTagCompoundEnd();

        inline void writeTagByte(const uint8_t &value)
        {
            writeTagByte("", value);
        }

        inline void writeTagShort(const uint16_t &value)
        {
            writeTagShort("", value);
        }

        inline void writeTagInt(const uint32_t &value)
        {
            writeTagInt("", value);
        }

        inline void writeTagLong(const uint64_t &value)
        {
            writeTagLong("", value);
        }

        inline void writeTagFloat(const float &value)
        {
            writeTagFloat("", value);
        }

        inline void writeTagDouble(const double &value)
        {
            writeTagDouble("", value);
        }

        inline void writeTagString(const String &value)
        {
            writeTagString("", value);
        }

        inline void writeTagByteArrayStart(const uint32_t length)
        {
            writeTagByteArrayStart("", length);
        }

        inline void writeTagIntArrayStart(const uint32_t length)
        {
            writeTagIntArrayStart("", length);
        }

        inline void writeTagListStart(const NbtTagType tagType, const uint32_t length)
        {
            writeTagListStart("", tagType, length);
        }

        inline void writeTagCompoundStart()
        {
            writeTagCompoundStart("");
        }

    };

}
#endif
