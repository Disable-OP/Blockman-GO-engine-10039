#pragma once

#include "Core.h"

namespace BLOCKMAN
{
	class Chunk;
	using ChunkPtr = std::shared_ptr<Chunk>;

	// WARNING: Derived class of IChunkStorage must be thread safe
	class IChunkStorage
	{
	public:
		virtual ~IChunkStorage() = default;
		virtual void storeChunk(const ChunkPtr& chunk) = 0;
		virtual void flushStorage() {};

	protected:
		IChunkStorage() = default;
	};
}