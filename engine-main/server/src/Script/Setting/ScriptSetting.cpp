#include "ScriptSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"

namespace BLOCKMAN
{
	bool ScriptSetting::loadScriptDir(String & path)
	{
		CsvReader* pReader = new CsvReader();
		std::string filePath = PathUtil::ConcatPath(path, "res/client/ScriptSetting.csv").c_str();
		bool bRet = pReader->LoadFile(filePath);
		if (!bRet)
		{
			LordLogError("Can not open ScriptSetting setting file %s", filePath.c_str());
			return false;
		}
		pReader->Read(); // discard the second line
		String GameType = "";
		String ScriptPath = "";
		bool Enable = false;
		while (pReader->Read())
		{
			pReader->GetBool("Enable", Enable);
			if (Enable)
			{
				String Cursor = "";
				pReader->GetString("RootDirectory", Cursor);
				ScriptPath += "\\";
				ScriptPath += Cursor;
				pReader->GetString("GroupDirectory", Cursor);
				ScriptPath += "\\";
				ScriptPath += Cursor;
				pReader->GetString("Name", Cursor);
				ScriptPath += "\\";
				ScriptPath += Cursor;
				break;
			}
		}
		pReader->Close();
		LordSafeDelete(pReader);
		path = path + ScriptPath;
		return Enable;
	}
}