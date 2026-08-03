#ifndef __TILE_ENTITYS_NBT_BINDINGS_HEADER__
#define __TILE_ENTITYS_NBT_BINDINGS_HEADER__

#include "Nbt/NbtBindingMacros.h"

namespace BLOCKMAN
{
	using TextArray = String[4];
	NBTB_BEGIN(TileEntitySign, TileEntity)
	NBTB_ALIAS(m_signTxt, "Text1", NBTB_SIMPLE(TAG_STRING), NBTB_CONVERTERS(
		[](TextArray& texts, const String& val) { texts[0] = val; }, 
		[](const TextArray& texts) { return texts[0]; })
	)
	NBTB_ALIAS(m_signTxt, "Text2", NBTB_SIMPLE(TAG_STRING), NBTB_CONVERTERS(
		[](TextArray& texts, const String& val) { texts[1] = val; },
		[](const TextArray& texts) { return texts[1]; })
	)
	NBTB_ALIAS(m_signTxt, "Text3", NBTB_SIMPLE(TAG_STRING), NBTB_CONVERTERS(
		[](TextArray& texts, const String& val) { texts[2] = val; },
		[](const TextArray& texts) { return texts[2]; })
	)
	NBTB_ALIAS(m_signTxt, "Text4", NBTB_SIMPLE(TAG_STRING), NBTB_CONVERTERS(
		[](TextArray& texts, const String& val) { texts[3] = val; },
		[](const TextArray& texts) { return texts[3]; })
	)
	NBTB_END
}

#endif // !__TILE_ENTITYS_NBT_BINDINGS_HEADER__
