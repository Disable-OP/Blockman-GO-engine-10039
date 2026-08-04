#ifndef __ANVIL_SERVER_HEADER__
#define __ANVIL_SERVER_HEADER__

#include "Core.h"
#include "WorldGenerator/Anvil.h"
#include "Blockman/Chunk/ChunkServer.h"

namespace BLOCKMAN
{
	struct ChunkServerWithMeta
	{
		int version;
		ChunkServer* chunk;
	};

	NBTB_BEGIN(ChunkServerWithMeta, void)
		NBTB_ALIAS(version, "DataVersion", NBTB_SIMPLE(TAG_INT))
		NBTB_ALIAS(chunk, "Level", NBTB_COMPOUND(ChunkServer))
	NBTB_END

	class AnvilServer : public Anvil
	{
		using Anvil::Anvil;
	private:
		Chunk * deserializeChunk(InputStream& is) const override
		{
			return Anvil::deserializeChunk<ChunkServer, ChunkServerWithMeta>(is);
		}
	};
}

#endif // !__ANVIL_CLIENT_HEADER__
