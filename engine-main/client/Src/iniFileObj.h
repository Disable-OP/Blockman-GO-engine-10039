#ifndef _INI_FILE_H_
#define _INI_FILE_H_

#include "Core.h"

namespace GameClient
{
	struct FileData
	{
		FILE*	handle;
		char*	buffer;

		FileData()
		{
			handle = NULL;
			buffer = NULL;
		}
	};

	class IniFileObj : public LORD::ObjectAlloc, public LORD::Singleton<IniFileObj>
	{
	public:
		IniFileObj();
		~IniFileObj();

		static IniFileObj* Instance();

		// flag 0 read, 1 write
		int		open(const char* filename, int flag);

		void	close(int handle);

		void	write(int handle, const char* data);

		const char* readall(int handle);

		void	writeKeyValue(int handle, const char* key, const char* value);

		std::string readKeyValue(int handle, const char* key);

		// 读ini文件
		std::string readIni(const char* section, const char* key, const char* default_value, const char* file);
		// 写ini文件
		void		writeIni(const char* section, const char* key, const char* value, const char* file);



	private:

		LORD::map<int, FileData>::type		handleList;
	};


}

#endif
