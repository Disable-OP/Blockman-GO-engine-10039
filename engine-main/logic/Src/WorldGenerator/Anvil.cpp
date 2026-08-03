#include "Anvil.h"
#include <string>
#include <chrono>
#include <algorithm>
#include <stdexcept>
#include "Util/IO.h"
#include "World/Section.h"
#include "Stream/ZlibStream.h"

namespace BLOCKMAN {
	using std::istream_iterator;
	using std::ostream_iterator;

	Chunk* Anvil::extractChunk(int x, int z)
	{
		return extractChunk(calculateChunkIndex(x, z));
	}

	vector<Chunk*>::type Anvil::extractAllChunks()
	{
		vector<Chunk*>::type ret;
		for (int i = 0; i < CHUNK_COUNT; ++i)
		{
			auto chunk = extractChunk(i);
			if (chunk)
			{
				ret.push_back(chunk);
			}
		}
		return ret;
	}

	bool Anvil::placeChunk(Chunk* pChunk) 
	{
		unique_lock lock(m_mutex);
		auto chunkIndex = calculateChunkIndex(pChunk->m_posX, pChunk->m_posZ);
		auto &compressedChunk = this->m_chunkDatas[chunkIndex].compressedChunk;
		auto &compressionType = this->m_chunkDatas[chunkIndex].compressionType;
		compressedChunk.clear();

		if (!pChunk->isNonexistent())
		{
			ChunkWithMeta chunkWithMeta = { 0, pChunk };
			switch (compressionType) {
			case Anvil::CompressionType::GZIP:
			{
				LordLogError("GZIP compression is not supported, chunkX:%d, chunkZ:%d", pChunk->m_posX, pChunk->m_posZ);
				return false;
			}
			case Anvil::CompressionType::ZLIB:
			{
				ZlibOutputStream os(compressedChunk);
				try
				{
					BLOCKMAN::serialize(&chunkWithMeta, os);
				}
				catch (const InvalidNbtFormatError& e)
				{
					LordLogError("Failed to place chunk (%d, %d): %s", pChunk->m_posX, pChunk->m_posZ, e.what());
					LordLogWarning("Compressed chunk data in anvil object might be invalid now becuase of the failure of chunk serialization");
					return false;
				}
				break;
			}
			}
		}

		if (compressedChunk.size() > 0)
		{
			this->m_chunkDatas[chunkIndex].sectorCount = (compressedChunk.size() + 5 - 1) / Anvil::SECTOR_SIZE + 1;
		}
		else
		{
			this->m_chunkDatas[chunkIndex].sectorCount = 0;
		}

		auto now = std::chrono::system_clock::now();
		auto durationSinceEpoch = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
		this->m_chunkDatas[chunkIndex].timestamp = static_cast<unsigned>(durationSinceEpoch.count());

		unsigned totalOffset = 2;
		for (auto &chunkData : m_chunkDatas)
		{
			chunkData.sectorOffset = totalOffset;
			totalOffset += chunkData.sectorCount;
		}
		return true;
	}

	Chunk* Anvil::extractChunk(int index)
	{
		shared_lock lock(m_mutex);
		auto &compressedChunk = this->m_chunkDatas[index].compressedChunk;
		if (compressedChunk.size() == 0)
		{
			return nullptr;
		}
		auto compressionType = this->m_chunkDatas[index].compressionType;
		switch (compressionType) {
		case Anvil::CompressionType::GZIP:
		{
			LordLogError("GZIP compression is not supported, index:%d", index);
			return nullptr;
		}
		case Anvil::CompressionType::ZLIB:
		{
			ZlibInputStream is(compressedChunk.data(), compressedChunk.size());
			auto requiredChunkPosition = calculateChunkXZ(index);
			auto chunk = deserializeChunk(is);
			if (!chunk)
			{
				LordLogError("Failed to extract chunk (%d, %d) from the anvil", requiredChunkPosition.first, requiredChunkPosition.second);
				return nullptr;
			}
			if (requiredChunkPosition.first != chunk->m_posX || requiredChunkPosition.second != chunk->m_posZ)
			{
				LordLogError("Chunk %d,%d is at the position of %d,%d", chunk->m_posX, chunk->m_posZ, requiredChunkPosition.first, requiredChunkPosition.second);
				chunk->m_posX = requiredChunkPosition.first;
				chunk->m_posZ = requiredChunkPosition.second;
			}
			return chunk;
		}
		default:
			LordLogError("unkown compression type: %s", StringUtil::ToString(static_cast<int>(compressionType)).c_str());
			return nullptr;
		}
	}

	size_t Anvil::getChunkCount()
	{
		shared_lock lock(m_mutex);
		size_t count = 0;
		for (const auto& chunkData : m_chunkDatas)
		{
			if (chunkData.sectorCount > 0)
			{
				++count;
			}
		}
		return count;
	}

	void Anvil::serialize(std::ostream & output)
	{
		shared_lock lock(m_mutex);
		for (unsigned i = 0; i < Anvil::CHUNK_COUNT; i++)
		{
			const auto &chunkData = m_chunkDatas[i];
			uint32_t locationHeader = (chunkData.sectorOffset & 0xFFF) << 8 | chunkData.sectorCount & 0xF;
			writeBinary(locationHeader, output);
		}
		for (unsigned i = 0; i < Anvil::CHUNK_COUNT; i++)
		{
			auto &chunkData = m_chunkDatas[i];
			uint32_t timestampHeader = static_cast<uint32_t>(chunkData.timestamp);
			writeBinary(timestampHeader, output);
		}
		for (const auto &chunkData : m_chunkDatas)
		{
			if (chunkData.sectorCount == 0)
			{
				continue;
			}
			output.seekp(chunkData.sectorOffset * Anvil::SECTOR_SIZE, std::ios_base::beg);
			writeBinary(static_cast<uint32_t>(chunkData.compressedChunk.size() + 1), output);
			writeBinary(static_cast<uint8_t>(chunkData.compressionType), output);
			std::copy(chunkData.compressedChunk.cbegin(), chunkData.compressedChunk.cend(), ostream_iterator<char>(output));
		}
	}

	bool Anvil::deserialize(std::istream & input)
	{
		unique_lock lock(m_mutex);
		for (unsigned i = 0; i < Anvil::CHUNK_COUNT && input; i++)
		{
			uint32_t locationHeader;
			readBinary(locationHeader, input);
			m_chunkDatas[i].sectorOffset = locationHeader >> 8;
			m_chunkDatas[i].sectorCount = locationHeader & 0xFF;
		}
		for (unsigned i = 0; i < Anvil::CHUNK_COUNT && input; i++)
		{
			uint32_t timestampHeader;
			readBinary(timestampHeader, input);
			m_chunkDatas[i].timestamp = timestampHeader;
		}
		for (auto &chunkData : m_chunkDatas)
		{
			if (chunkData.sectorCount == 0)
			{
				continue;
			}
			input.seekg(chunkData.sectorOffset * Anvil::SECTOR_SIZE, std::ios_base::beg);
			uint32_t length;
			readBinary(length, input);
			if (length <= 1 || length > Anvil::SECTOR_SIZE * chunkData.sectorCount - 4)
			{
				LordLogError("anvil file is correupted, x:%d, z:%d", m_x, m_z);
				return false;
			}
			uint8_t compressionType;
			readBinary(compressionType, input);
			chunkData.compressionType = static_cast<Anvil::CompressionType>(compressionType);
			chunkData.compressedChunk.reserve(length - 1);
			std::copy_n(istream_iterator<char>(input >> std::noskipws), length - 1, std::back_inserter(chunkData.compressedChunk));
		}
		return true;
	}
}
