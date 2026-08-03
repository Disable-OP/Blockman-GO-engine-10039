#pragma once
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include "Core.h"

namespace LORD
{
	class CsvReader
	{
	public:
		typedef std::map<std::string, int> ColumnIndexMap;

		CsvReader();
		virtual ~CsvReader();
		//加载Csv文件
		bool LoadFile(const std::string & fileName);
		//读取一行数据
		bool Read();
		void Close();
		//得到列名字段的索引
		int GetColumnIndex(const std::string & columnName);
		int GetHeight();

		//按列名提取数据
		bool GetBool(const std::string & columnName, bool & ret);
		bool GetInt(const std::string & columnName, int & ret);
		bool GetFloat(const std::string & columnName, float & ret);
		bool GetString(const std::string & columnName, std::string& ret);
		bool GetString(const std::string & columnName, String& ret);	

		//分割字符串为字符数组
		static void SplitString(const std::string & str, std::vector<std::string> & str_vec, char delimiter);

	private:
		ColumnIndexMap _columnIndex;    //字段所对应列的索引
		std::vector<std::string> _lineData;//Csv文件一行数据
		std::ifstream _fi;
		bool _fileIsOpen;
		std::string _line;
		std::stringstream _ss;
	};
}
