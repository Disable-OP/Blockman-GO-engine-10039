#ifndef CLIENT_CMD_MGR_H
#define CLIENT_CMD_MGR_H
#include <map>
#include "Core.h"
#include "Object/Singleton.h"
#include "Util/StringUtil.h"
#include "Memory/MemAllocDef.h"

namespace BLOCKMAN {
	class ClientCmdMgr {
	public:
		static LORD::String CMD_STARTER;
		static LORD::String CMD_TOGGLE_DEBUG;
		static LORD::String CMD_SET_SMOOTH;
		static LORD::String CMD_GET_BLOCK;
		static LORD::String CMD_SAVE;
		static LORD::String CMD_CRASH;

		struct CmdCfg {
			int argsNum;
			std::function<bool(LORD::StringArray&)> func;
		};
		typedef std::map<LORD::String, CmdCfg> ClientCmdMap;

	public:
		static void init();
		static bool consumeCmd(const LORD::String & msg);

	private:
		static bool toggleDebug(LORD::StringArray& argsArr);
		static bool setSmooth(LORD::StringArray& argsArr);
		static bool getBlock(LORD::StringArray& argsArr);
		static bool save(LORD::StringArray& argsArr);
		static bool crash(LORD::StringArray& argsArr);

	private:
		static ClientCmdMap m_cmdMap;
	};
}
#endif // CLIENT_CMD_MGR_H
