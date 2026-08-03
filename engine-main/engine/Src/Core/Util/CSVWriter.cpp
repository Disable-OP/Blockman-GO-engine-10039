#include "CSVWriter.h"

#include <algorithm>
#include <iterator>
#include <sstream>

namespace LORD
{

	CsvWriter::CsvWriter(std::string file, std::vector<std::string> header) :
		_file_name(std::move(file)), _header(std::move(header))
	{
	}

	bool CsvWriter::Write(const std::map<std::string, std::string> &data) 
	{
		std::stringstream ss;

		for (size_t i = 0; i < _header.size(); ++i) {
			if (i != 0)
				ss << "\t";

			auto it = data.find(_header[i]);

			if (it != data.end())
				ss << it->second;
		}
		ss << std::endl;

		String line = ss.str().c_str();
		_ofstream << line;
		_ofstream.flush();

		return true;
	}

	bool CsvWriter::Write(const std::map<String, String> &data)
	{
		std::stringstream ss;

		for (size_t i = 0; i < _header.size(); ++i) {
			if (i != 0)
				ss << "\t";

			auto it = data.find(_header[i].c_str());

			if (it != data.end())
				ss << it->second;
		}
		ss << std::endl;

		String line = ss.str().c_str();
		_ofstream << line;
		_ofstream.flush();

		return true;
	}

	bool CsvWriter::Create()
	{
		_ofstream.open(_file_name.c_str(), std::ios::app);

		std::stringstream ss;

		for (size_t i = 0; i < _header.size(); ++i)
		{
			if (i != 0)
				ss << "\t";

			ss << _header[i];
		}
		ss << std::endl;

		String _header_line = ss.str().c_str();
		_ofstream << _header_line;
		_ofstream.flush();

		return true;

	}

	bool CsvWriter::CreateNew()
	{
		_ofstream.open(_file_name.c_str());

		std::stringstream ss;

		for (size_t i = 0; i < _header.size(); ++i)
		{
			if (i != 0)
				ss << "\t";

			ss << _header[i];
		}
		ss << std::endl;

		String _header_line = ss.str().c_str();
		_ofstream << _header_line;
		_ofstream.flush();

		return true;

	}

	CsvWriter::~CsvWriter()
	{
		_ofstream.close();
	}

	void CsvWriter::Close() 
	{
		_ofstream.close();
	}

}
