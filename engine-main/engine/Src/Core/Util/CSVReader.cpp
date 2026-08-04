#include "CSVReader.h"

namespace LORD
{
	CsvReader::CsvReader()
		: _fileIsOpen(false)
	{

	}

	CsvReader::~CsvReader()
	{
		this->Close();
	}

	bool CsvReader::LoadFile(const std::string & fileName)
	{
		if (_fileIsOpen)    //先关闭原来的文件
		{
			this->Close();
		}
		_fi.open(fileName, std::ios::binary);
		if (!_fi.is_open())
		{
			return false;
		}
		_fileIsOpen = true;
		//读取第一行数据，得到CSV文件各列字段名称，并生成索引
		if (!this->Read())
		{
			return false;
		}
		int columnCount = _lineData.size();
		for (int columnIndex = 0; columnIndex < columnCount; columnIndex++)
		{
			std::string columnName = _lineData[columnIndex];
			_columnIndex[columnName] = columnIndex;
		}
		return true;
	}

	void CsvReader::Close()
	{
		_fi.close();
		_lineData.clear();
		_columnIndex.clear();
		_fileIsOpen = false;
	}

	bool CsvReader::Read()
	{
		while (_fileIsOpen && !_fi.eof())
		{
			_line.clear();
			std::getline(_fi, _line);
			if (_line.empty()) continue;

			unsigned int pos = _line.find_last_of("\r"); // replace the \r in line
			if (pos <= _line.size())
			{
				_line.replace(pos, 1, "");
			}

			_lineData.clear();
			CsvReader::SplitString(_line, _lineData, '\t');
			if (_lineData.size() < _columnIndex.size())
			{
				continue;
			}
			return true;
		}
		return false;
	}

	void CsvReader::SplitString(const std::string & str, std::vector<std::string> & str_vec, char delimiter)
	{
		if (str.empty())
			return;
		std::string::size_type pos = 0;
		std::string::size_type pre_pos = 0;
		while ((pos = str.find_first_of(delimiter, pos)) != std::string::npos)
		{
			std::string tmp_str = str.substr(pre_pos, pos - pre_pos);
			str_vec.push_back(tmp_str);
			pre_pos = ++pos;
		}
		std::string tmp_str;
		if (pre_pos < str.size())
		{
			tmp_str = std::string(&str[pre_pos]);
		}
		str_vec.push_back(tmp_str);
	}

	int CsvReader::GetColumnIndex(const std::string & columnName)
	{
		ColumnIndexMap::const_iterator indexIt = _columnIndex.find(columnName);
		if (indexIt != _columnIndex.end())
		{
			return indexIt->second;
		}
		return -1;
	}

	int CsvReader::GetHeight()
	{
		return 0;
	}

	bool CsvReader::GetInt(const std::string & columnName, int & ret)
	{
		bool bResult = false;
		int columnIndex = this->GetColumnIndex(columnName);
		if (columnIndex == -1)
		{
			ret = 0;
			return false;
		}

		_ss.str("");
		_ss.clear();
		_ss.str(_lineData[columnIndex]);
		_ss >> ret;
		bResult = true;
		return bResult;
	}

	bool CsvReader::GetBool(const std::string & columnName, bool & ret)
	{
		bool bResult = false;
		int iRet = 0;
		int columnIndex = this->GetColumnIndex(columnName);
		if (columnIndex == -1)
		{
			ret = false;
			return false;
		}

		_ss.str("");
		_ss.clear();
		_ss.str(_lineData[columnIndex]);
		_ss >> iRet;
		ret = iRet > 0;
		bResult = true;
		return bResult;
	}

	bool CsvReader::GetFloat(const std::string & columnName, float & ret)
	{
		bool bResult = false;
		int columnIndex = this->GetColumnIndex(columnName);
		if (columnIndex == -1)
		{
			ret = 0.0f;
			return false;
		}

		_ss.str("");
		_ss.clear();
		_ss.str(_lineData[columnIndex]);
		_ss >> ret;
		bResult = true;
		return bResult;
	}

	bool CsvReader::GetString(const std::string & columnName, std::string& ret)
	{
		bool bResult = false;
		int columnIndex = this->GetColumnIndex(columnName);
		if (columnIndex == -1)
			return bResult;

		ret = _lineData[columnIndex];
		bResult = true;
		return bResult;
	}

	bool CsvReader::GetString(const std::string & columnName, String & ret)
	{
		bool bResult = false;
		int columnIndex = this->GetColumnIndex(columnName);
		if (columnIndex == -1)
			return bResult;

		ret = _lineData[columnIndex].c_str();
		bResult = true;
		return bResult;
	}
}


