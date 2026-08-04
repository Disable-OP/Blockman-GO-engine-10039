#include "iniFileObj.h"
#include "Object/Root.h"
#include "External/inifile.h"

#define BUF_SIZE 256

using namespace LORD;
namespace GameClient
{

	IniFileObj* IniFileObj::Instance()
	{
		return ms_pSingleton;
	}

	IniFileObj::IniFileObj()
	{

	}

	IniFileObj::~IniFileObj()
	{

	}

	int IniFileObj::open(const char* filename, int flag)
	{
		static int handle = 1;

		int ret = 0;

		String fullpath = Root::Instance()->getWriteablePath() + filename;

		FILE* file = NULL;
		if (flag == 0)
		{
			file = fopen(fullpath.c_str(), "rb");
		}
		else
		{
			file = fopen(fullpath.c_str(), "wb");
		}

		if (file)
		{
			FileData data;
			data.handle = file;

			if (flag == 0)
			{
				// init buffer
				/* 获取文件大小 */
				fseek(file, 0, SEEK_END);
				long lSize = ftell(file);
				rewind(file);

				/* 分配内存存储整个文件 */
				data.buffer = (char*)LordMalloc(sizeof(char)*lSize + 1);
				data.buffer[lSize] = 0;

				/* 将文件拷贝到buffer中 */
				long result = fread(data.buffer, 1, lSize, file);
				if (result != lSize)
				{
					LordAssert("FileOperationSystem reading error");
				}
			}

			handleList[handle] = data;
			ret = handle;
			++handle;
		}

		return ret;
	}

	void IniFileObj::close(int handle)
	{
		map<int, FileData>::type::iterator iter = handleList.find(handle);
		if (iter != handleList.end())
		{
			LordSafeFree(iter->second.buffer);

			if (iter->second.handle)
			{
				fclose(iter->second.handle);
				iter->second.handle = NULL;
			}
		}
	}

	void IniFileObj::write(int handle, const char* data)
	{
		map<int, FileData>::type::iterator iter = handleList.find(handle);
		if (iter != handleList.end())
		{
			fwrite(data, strlen(data), 1, iter->second.handle);
		}
	}

	const char* IniFileObj::readall(int handle)
	{
		map<int, FileData>::type::iterator iter = handleList.find(handle);
		if (iter != handleList.end())
		{
			return iter->second.buffer;
		}
		else
		{
			return NULL;
		}
	}

	void IniFileObj::writeKeyValue(int handle, const char* key, const char* value)
	{
		String data = StringUtil::Format("%s = %s\r\n", key, value);
		map<int, FileData>::type::iterator iter = handleList.find(handle);
		if (iter != handleList.end())
		{
			fwrite(data.c_str(), strlen(data.c_str()), 1, iter->second.handle);
		}
	}

	std::string IniFileObj::readKeyValue(int handle, const char* key)
	{
		map<int, FileData>::type::iterator iter = handleList.find(handle);
		if (iter != handleList.end() && iter->second.buffer)
		{
			StringArray keylist = StringUtil::Split(iter->second.buffer, "\r\n");

			for (size_t i = 0; i < keylist.size(); ++i)
			{
				size_t pos = keylist[i].find("=");
				String strKey = keylist[i].substr(0, pos);
				String strValue = keylist[i].substr(pos + 1);
				strKey = StringUtil::RemoveCharactors(strKey, ' ');
				strValue = StringUtil::RemoveCharactors(strValue, ' ');

				if (strKey == key)
				{
					return strValue.c_str();
				}
			}
		}

		return "";
	}

	std::string IniFileObj::readIni(const char* section, const char* key, const char* default_value, const char* file)
	{
		char value[BUF_SIZE] = { 0 };
		std::string result = default_value;
		String fullpath = Root::Instance()->getWriteablePath() + file;
		if (!read_profile_string(section, key, value, BUF_SIZE, "", fullpath.c_str()))
		{
			LordLogError("read ini file fail: %s\n", fullpath.c_str());
		}
		else
		{
			result = value;
		}

		return result;
	}

	void IniFileObj::writeIni(const char* section, const char* key, const char* value, const char* file)
	{
		String fullpath = Root::Instance()->getWriteablePath() + file;
		//write name key value pair
		if (!write_profile_string(section, key, value, fullpath.c_str()))
		{
			LordLogError("write name pair to ini file fail: %s\n", fullpath.c_str());
		}
	}

}

