#pragma once
#ifndef CSVWRITER_CSVWRITER_H
#define CSVWRITER_CSVWRITER_H

#include <string>
#include <vector>
#include <map>
#include "Core.h"

namespace LORD
{
	class CsvWriter {
	public:
		CsvWriter(std::string file, std::vector<std::string>);
		~CsvWriter();

		bool Create();
		bool CreateNew();
		bool Write(const std::map<std::string, std::string> &);
		bool Write(const std::map<String, String> &);
		void Close();

	private:
		std::string _file_name;
		std::vector<std::string> _header;

		OFStream _ofstream;
	};
}

#endif //CSVWRITER_CSVWRITER_H
