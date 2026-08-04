#include "SchematicModel.h"

namespace BLOCKMAN
{
	SchematicModel::SchematicModel()
		: m_width(0)
		, m_height(0)
		, m_length(0)
	{
		m_blocks_id.clear();
		m_mates_id.clear();
	}

	SchematicModel::SchematicModel(i16 width, i16 height, i16 length)
		: m_width(width)
		, m_height(height)
		, m_length(length)
	{
		m_blocks_id.clear();
		m_mates_id.clear();
	}

	SchematicModel::~SchematicModel()
	{
		
	}

	ui8 SchematicModel::getBlockId(int index) const
	{
		if (index < 0 || index >= (int)m_blocks_id.size()) return 0;

		return m_blocks_id[index];
	}

	ui8 SchematicModel::getMate(int index) const
	{
		if (index < 0 || index >= (int)m_mates_id.size()) return 0;

		return m_mates_id[index];
	}

	void SchematicModel::resetData()
	{
		m_width = 0;
		m_height = 0;
		m_length = 0;
		m_blocks_id.clear();
		m_mates_id.clear();
	}

	bool SchematicModel::deserialize(const char* path)
	{
		std::ifstream stream(path, std::ios_base::in | std::ios_base::binary);
		if (!stream) {
			LordLogError("Failed to open schematic file: %s", path);
			return false;
		}

		vector<char>::type file_data;

		stream.seekg(0, stream.end);
		int length = (int)stream.tellg();
		stream.seekg(0, stream.beg);

		std::copy_n(std::istream_iterator<char>(stream >> std::noskipws), length - 1, std::back_inserter(file_data));

		ZlibInputStream is(file_data.data(), file_data.size());

		stream.close();

		try
		{
			BLOCKMAN::deserialize(this, is);
		}
		catch (const StreamError& e)
		{
			LordLogError("Failed to deserialize schematic StreamError: %s", e.what());
			return false;
		}
		catch (const InvalidNbtFormatError& e)
		{
			LordLogError("Failed to deserialize schematic InvalidNbtFormatError: %s", e.what());
			return false;
		}

		return true;
	}
}
