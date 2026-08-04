#ifndef __ANVIL_HEADER__
#define __ANVIL_HEADER__

#include <array>
#include <functional>
#include "Core.h"
#include "Functional/LordNewOp.h"
#include "Chunk/Chunk.h"
#include "Nbt/Serializer.h"
#include "Std/shared_mutex.h"

using namespace LORD;

namespace BLOCKMAN
{
	class Anvil;

	struct ChunkWithMeta
	{
		int version;
		Chunk* chunk;
	};

	NBTB_BEGIN(ChunkWithMeta, void)
		NBTB_ALIAS(version, "DataVersion", NBTB_SIMPLE(TAG_INT))
		NBTB_ALIAS(chunk, "Level", NBTB_COMPOUND(Chunk))
	NBTB_END

	// Anvil is thread safe
	class Anvil : public ObjectAlloc
	{
	protected:
		using InputStream = NbtParser::StreamType;
		using unique_lock = std::unique_lock<std::shared_mutex>;
		using shared_lock = std::shared_lock<std::shared_mutex>;

	public:
		enum class CompressionType : char
		{
			GZIP = 1,
			ZLIB = 2
		};

		struct ChunkData
		{
			unsigned sectorOffset = 0;
			unsigned sectorCount = 0;
			unsigned timestamp = 0;
			CompressionType compressionType = CompressionType::ZLIB;
			vector<char>::type compressedChunk;
		};

		static constexpr size_t CHUNK_COUNT_IN_LINE = 32;
		static constexpr size_t CHUNK_COUNT = CHUNK_COUNT_IN_LINE * CHUNK_COUNT_IN_LINE;
		static constexpr size_t SECTOR_SIZE = 4 * CHUNK_COUNT;

		Anvil(int x, int z) : m_x(x), m_z(z) {}
		virtual ~Anvil() = default;

		inline static int calculateChunkIndex(int x, int z) noexcept
		{
			return (x & (CHUNK_COUNT_IN_LINE - 1)) + (z & (CHUNK_COUNT_IN_LINE - 1)) * CHUNK_COUNT_IN_LINE;
		}

		inline std::pair<int, int> calculateChunkXZ(size_t index) const noexcept
		{
			return { index % CHUNK_COUNT_IN_LINE + m_x * CHUNK_COUNT_IN_LINE, index / CHUNK_COUNT_IN_LINE + m_z * CHUNK_COUNT_IN_LINE };
		}

		void serialize(std::ostream & output);
		bool deserialize(std::istream & input);
		Chunk* extractChunk(int x, int z);
		vector<Chunk*>::type extractAllChunks();
		bool placeChunk(Chunk* pChunk);
		size_t getChunkCount();
		int getX() const noexcept
		{
			return m_x;
		}
		int getZ() const noexcept
		{
			return m_z;
		}

	private:
		const int m_x = 0;
		const int m_z = 0;
		std::array<ChunkData, CHUNK_COUNT> m_chunkDatas;
		std::shared_mutex m_mutex;

		Chunk* extractChunk(int index);
		virtual Chunk* deserializeChunk(InputStream& is) const = 0;

	protected:
		template<typename ChunkClass, typename ChunkWithMetaClass>
		ChunkClass * deserializeChunk(InputStream& is) const
		{
			ChunkClass* ret;
			auto chunkWithMeta = LordNewT(ChunkWithMetaClass);
			bool result;
			try
			{
				result = BLOCKMAN::deserialize(chunkWithMeta, is);
			}
			catch (const StreamError& e)
			{
				LordLogError("failed to deserialize chunk: %s", e.what());
				result = false;
			}
			catch (const InvalidNbtFormatError& e)
			{
				LordLogError("failed to deserialize chunk: %s", e.what());
				result = false;
			}
			if (result)
			{
				ret = chunkWithMeta->chunk;
			}
			else
			{
				LordSafeDelete(chunkWithMeta->chunk);
				ret = nullptr;
			}
			LordSafeDeleteT(chunkWithMeta, ChunkWithMetaClass);
			return ret;
		}
	};

	using Region = Anvil;
}
#endif