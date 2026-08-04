#ifndef __ANVIL_CLIENT_HEADER__
#define __ANVIL_CLIENT_HEADER__

#include "cChunk/ChunkClient.h"

#include "WorldGenerator/Anvil.h"

#include "Core.h"

namespace BLOCKMAN
{
	struct ChunkClientWithMeta
	{
		int version;
		ChunkClient* chunk;
	};

	NBTB_BEGIN(ChunkClientWithMeta, void)
		NBTB_ALIAS(version, "DataVersion", NBTB_SIMPLE(TAG_INT))
		NBTB_ALIAS(chunk, "Level", NBTB_COMPOUND(ChunkClient))
	NBTB_END

	class AnvilClient : public Anvil
	{
		using Anvil::Anvil;
	private:
		Chunk * deserializeChunk(InputStream& is) const override
		{
			return Anvil::deserializeChunk<ChunkClient, ChunkClientWithMeta>(is);
		}
	};
}

#endif // !__ANVIL_CLIENT_HEADER__
