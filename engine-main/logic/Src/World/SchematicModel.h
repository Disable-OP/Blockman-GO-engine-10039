#ifndef __SCHEMATIC_HEADER__
#define __SCHEMATIC_HEADER__

#include "BM_TypeDef.h"
#include "Nbt/NbtBindingMacros.h"
#include <vector>
#include "Nbt/Serializer.h"

class NBTTagCompound;


namespace BLOCKMAN
{
	class SchematicModel : public ObjectAlloc
	{
		NBTB_DECLARE_FRIEND(SchematicModel);
	public:
		SchematicModel();
		SchematicModel(i16 width, i16 height, i16 length);
		virtual ~SchematicModel();

		inline i16 getWidth() const { return m_width; }
		inline i16 getHeight() const { return m_height; }
		inline i16 getLength() const { return m_length; }
		ui8 getBlockId(int index) const;
		ui8 getMate(int index) const;
		
		using InputStream = NbtParser::StreamType;

		void resetData();
		bool deserialize(const char* path);

	private:
		i16 m_width = 0;
		i16 m_height = 0;
		i16 m_length = 0;
		std::vector<ui8> m_blocks_id;
		std::vector<ui8> m_mates_id;
	};

NBTB_BEGIN(SchematicModel, void)
	NBTB_ALIAS(m_width, "Width", NBTB_SIMPLE(TAG_SHORT))
	NBTB_ALIAS(m_height, "Height", NBTB_SIMPLE(TAG_SHORT))
	NBTB_ALIAS(m_length, "Length", NBTB_SIMPLE(TAG_SHORT))
	NBTB_ALIAS(m_blocks_id, "Blocks", NBTB_SIMPLE(TAG_BYTE_ARRAY))
	NBTB_ALIAS(m_mates_id, "Data", NBTB_SIMPLE(TAG_BYTE_ARRAY))
NBTB_END

}

#endif

