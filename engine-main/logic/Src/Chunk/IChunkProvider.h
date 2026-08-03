#pragma once

#include "Core.h"

namespace BLOCKMAN
{
	class Chunk;
	using ChunkPtr = std::shared_ptr<Chunk>;

	// WARNING: Derived class of IChunkProvider must be thread safe
	class IChunkProvider
	{
	public:
		virtual ~IChunkProvider() = default;
		virtual ChunkPtr provideChunk(int x, int z) = 0;

	protected:
		IChunkProvider() = default;
	};
}