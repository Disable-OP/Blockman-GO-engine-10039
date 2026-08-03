#include "ResourceMonitor.h"

#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
#include "Object/ConfigFile.h"
#include <queue>

#include <Shlwapi.h>

#pragma comment(lib, "Shlwapi.lib")

#define ResMgr ResourceGroupManager

namespace LORD
{
	//单独线程处理文件监控

	static CRITICAL_SECTION gCriticalSection;

	static bool gLobalThreadControl;

	///////////////////////////以下全局变量不考虑架构和封装，为了快速实现功能///////////////
	//监控位置
	static String gLocationName;


	//变化队列
	typedef std::queue<ChangedInfo> ChangedInfoQueue;
	static ChangedInfoQueue gChangedInfoQueue;

	//文件handler key 文件类型 value 处理函数
	typedef map<String, handlerFuc>::type HandlerFuc;
	static HandlerFuc gHandlerFuc;

	//资源组集合, key 路径 value 资源组
	typedef map<String, String>::type LocationMap;
	static LocationMap gLocationMap;


	class FileMutex
	{
	public:
		FileMutex() { EnterCriticalSection(&gCriticalSection); }
		~FileMutex() { LeaveCriticalSection(&gCriticalSection); }
	};

	////////////////////////辅助函数//////////////////

	bool hasTrailingBackslash(const String & str)
	{
		if (str.length() > 0 && str[str.length() - 1] == '\\')
			return true;
		return false;
	}

	// 相对路径转绝对路径
	String RelativePathToAbsolute(const String& BaseDir, const String& RelativeDir)
	{
		// 调用PathCombine前需要将路径中的'/'转为'\\'
		String dir = RelativeDir;
		for (size_t i = 0; i < dir.size(); ++i)
		{
			if (dir[i] == '/')
			{
				dir[i] = '\\';
			}
		}

		char retPath[MAX_PATH];
		PathCombine(retPath, BaseDir.c_str(), dir.c_str());

		// 得到的结果需要再将'\\'转为'/'
		size_t i = 0;
		while (retPath[i])
		{
			if (retPath[i] == '\\')
			{
				retPath[i] = '/';
			}
			++i;
		}

		return String(retPath);
	}

	// 去掉相对路径的".."
	String FilterPathString(const String& str)
	{
		const char* pTemp = str.c_str();
		if (pTemp[0] == '.' || pTemp[1] == '.')
			return String(&pTemp[2]);

		if (pTemp[0] == '.')
			return String(&pTemp[1]);

		return str;
	}

	void splitFilename(const String& qualifiedName,
		String& outBasename, String& outPath)
	{
		String path = qualifiedName;
		// Replace \ with / first
		std::replace(path.begin(), path.end(), '\\', '/');
		// split based on final /
		size_t i = path.find_last_of('/');

		if (i == String::npos)
		{
			outPath.clear();
			outBasename = qualifiedName;
		}
		else
		{
			outBasename = path.substr(i + 1, path.size() - i - 1);
			outPath = path.substr(0, i + 1);
		}

	}

	String standardisePath(const String& init)
	{
		String path = init;

		std::replace(path.begin(), path.end(), '\\', '/');
		if (path[path.length() - 1] != '/')
			path += '/';

		return path;
	}

	//获得组名称和文件名称和文件类型名称
	bool getGroupNameAndFileExtName(const String & wholeName, String& archName, String& groupName, String& fileName, String & extName)
	{
		bool findItem = false;
		LocationMap::iterator it = gLocationMap.begin();

		// 专成小写并去掉文件名和最后的'\'
		String lowerWholeName = wholeName;
		StringUtil::LowerCase(lowerWholeName);
		String lowerFileName, lowerPathName;
		splitFilename(lowerWholeName, lowerFileName, lowerPathName);

		for (; it != gLocationMap.end(); it++)
		{
			// 转成小写再比较
			String strArch = it->first;
			StringUtil::LowerCase(strArch);

			if (strArch == lowerPathName)
			{
				archName = it->first;
				groupName = it->second;
				findItem = true;
				break;
			}
		}
		if (findItem)
		{
			String::size_type  position = wholeName.rfind('.');
			if (position == String::npos)
				return false;

			const char *ext = &wholeName[position + 1];
			extName = String(ext);

			position = wholeName.rfind('\\');
			if (position == String::npos)
			{
				position = wholeName.rfind('/');
				if (position == String::npos)
					return false;
			}

			const char *file = &wholeName[position + 1];
			fileName = String(file);

			return true;
		}

		return false;
	}


	///////////////////////////////////////////////////////////////////////////

	//helper class ,帮助解析 文件变化数据
	///////////////////////////////////////////////
	//Helper class:

	class FileNotifyInformation
	{
	public:
		FileNotifyInformation() {}
		~FileNotifyInformation() {}

		FileNotifyInformation(BYTE * buffer, DWORD size)
			: mChangeBuffer(buffer)
			, mBufferSize(size)
		{
			assert(buffer && size);
			mCurrentRecord = (PFILE_NOTIFY_INFORMATION)mChangeBuffer;
		}


		bool getNextNotifyInformation()
		{
			if (mCurrentRecord &&	mCurrentRecord->NextEntryOffset != 0UL)
			{
				PFILE_NOTIFY_INFORMATION pOld = mCurrentRecord;
				mCurrentRecord = (PFILE_NOTIFY_INFORMATION)((LPBYTE)mCurrentRecord + mCurrentRecord->NextEntryOffset);

				assert(((BYTE*)mCurrentRecord - mChangeBuffer) < (int)mBufferSize);
				return true;
			}
			return false;
		}

		DWORD	getAction() const
		{
			assert(mCurrentRecord);
			if (mCurrentRecord)
				return mCurrentRecord->Action;
			return 0UL;
		}

		String getFileName()const
		{
			if (mCurrentRecord)
			{
				char fileName[MAX_PATH + 1];
				wchar_t wFileName[MAX_PATH + 1];
				memset(wFileName, 0x00, sizeof(wFileName));

				memset(fileName, 0x00, MAX_PATH + 1);

				memcpy(wFileName, mCurrentRecord->FileName, mCurrentRecord->FileNameLength);
				WideCharToMultiByte(CP_ACP, 0, wFileName, -1, fileName, mCurrentRecord->FileNameLength, NULL, NULL);
				return String(fileName);
			}
			return "";
		}

		String getFileNameWithPath(const String & strRootPath) const
		{
			String strFileName(strRootPath);
			if (!hasTrailingBackslash(strRootPath))
				strFileName += ("\\");

			strFileName += getFileName();

			String strPath, strBasename;
			splitFilename(strFileName, strBasename, strPath);
			strPath = standardisePath(strPath);
			strFileName = strPath + strBasename;

			return strFileName;
		}

	private:
		BYTE * mChangeBuffer;
		DWORD  mBufferSize;
		PFILE_NOTIFY_INFORMATION mCurrentRecord;
	};


	/////////////////////////////////////////////////////
	//监控线程函数
	static unsigned int CALLBACK	_WorkThread(LPVOID pParam)
	{

		//信息接收buffer
		BYTE mRecordBuffer[4096];

		//创建监控路径的句柄
		HANDLE hDirectory = CreateFileA(gLocationName.c_str(),
			FILE_LIST_DIRECTORY,
			FILE_SHARE_READ | FILE_SHARE_DELETE | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_BACKUP_SEMANTICS,
			NULL);
		if (hDirectory == INVALID_HANDLE_VALUE)
		{
			assert("启动路径监控错误" && 0);
			exit(0);
		}

		//传回来的信息长度
		DWORD       mBufferLength;

		//去掉文件改名、添加、删除引起的Modified
		DWORD mLastAction = FILE_ACTION_MODIFIED;
		//ChangedInfo mLastChangedInfo;

		int mOnlyOnce = 0;

		while (true)
		{
			if (gLobalThreadControl)
			{
				CloseHandle(hDirectory);
				return 0;
			}
			Sleep(10);

			if (!ReadDirectoryChangesW(hDirectory,
				mRecordBuffer,
				4096,
				TRUE, //监控只路径
				FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE,// 只监控文件变化，不监控运行时文件夹变化
				&mBufferLength,
				0,
				NULL))
			{
				//失败
				assert("运行路径监控错误" && 0);
				break;
			}
			else
			{
				//判定各种情况，生成信息，塞到队列里面去
				FileNotifyInformation information(mRecordBuffer, 4096);
				while (true)
				{
					switch (information.getAction())
					{
					case FILE_ACTION_ADDED:
					{
						mLastAction = FILE_ACTION_ADDED;

						ChangedInfo changeInformation;
						changeInformation.mChangeType = CT_ADD;
						changeInformation.mFirstName = information.getFileNameWithPath(gLocationName);
						//mLastChangedInfo = changeInformation;

						String archName, groupName, fileName, extName;
						if (getGroupNameAndFileExtName(changeInformation.mFirstName, archName, groupName, fileName, extName))
						{
							changeInformation.mArchName = archName;
							changeInformation.mGroupName = groupName;
							changeInformation.mFileName = fileName;
							changeInformation.mExtName = extName;
							FileMutex mutex;
							gChangedInfoQueue.push(changeInformation);
						}
					}
					break;
					case FILE_ACTION_REMOVED:
					{
						mLastAction = FILE_ACTION_REMOVED;

						ChangedInfo changeInformation;
						changeInformation.mChangeType = CT_DELETE;
						changeInformation.mFirstName = information.getFileNameWithPath(gLocationName);
						//mLastChangedInfo = changeInformation;

						String archName, groupName, fileName, extName;
						if (getGroupNameAndFileExtName(changeInformation.mFirstName, archName, groupName, fileName, extName))
						{
							changeInformation.mArchName = archName;
							changeInformation.mGroupName = groupName;
							changeInformation.mFileName = fileName;
							changeInformation.mExtName = extName;
							FileMutex mutex;
							gChangedInfoQueue.push(changeInformation);
						}
					}
					break;
					case FILE_ACTION_MODIFIED:
					{
						if (mLastAction == FILE_ACTION_ADDED ||
							mLastAction == FILE_ACTION_REMOVED ||
							mLastAction == FILE_ACTION_RENAMED_OLD_NAME ||
							mLastAction == FILE_ACTION_RENAMED_NEW_NAME)
						{
							mLastAction = FILE_ACTION_MODIFIED;
							break;
						}

						ChangedInfo changeInformation;
						changeInformation.mChangeType = CT_MODIFIED;
						changeInformation.mFirstName = information.getFileNameWithPath(gLocationName);


						String archName, groupName, fileName, extName;
						if (getGroupNameAndFileExtName(changeInformation.mFirstName, archName, groupName, fileName, extName))
						{
							changeInformation.mArchName = archName;
							changeInformation.mGroupName = groupName;
							changeInformation.mFileName = fileName;
							changeInformation.mExtName = extName;
							FileMutex mutex;
							gChangedInfoQueue.push(changeInformation);
						}
					}
					break;
					case FILE_ACTION_RENAMED_OLD_NAME:
					{
						mLastAction = FILE_ACTION_RENAMED_OLD_NAME;

						ChangedInfo changeInformation;
						changeInformation.mChangeType = CT_RENAME;
						changeInformation.mFirstName = information.getFileNameWithPath(gLocationName);

						if (information.getNextNotifyInformation() && (information.getAction() == FILE_ACTION_RENAMED_NEW_NAME))
						{
							changeInformation.mSecondName = information.getFileNameWithPath(gLocationName);
						}
						else
						{
							assert("监控名称修改错误" && 0);
							exit(0);
						}

						//mLastChangedInfo = changeInformation;

						String archName, groupName, fileName, extName;
						if (getGroupNameAndFileExtName(changeInformation.mFirstName, archName, groupName, fileName, extName))
						{
							changeInformation.mArchName = archName;
							changeInformation.mGroupName = groupName;
							changeInformation.mFileName = fileName;
							changeInformation.mExtName = extName;

							// 重名名时扩展名应该用新的
							if (getGroupNameAndFileExtName(changeInformation.mSecondName, archName, groupName, fileName, extName))
							{
								changeInformation.mExtName = extName;
							}

							FileMutex mutex;
							gChangedInfoQueue.push(changeInformation);
						}
					}
					break;
					default:
						break;
					}
					if (!information.getNextNotifyInformation())
						break;
				}
			}
		}

		CloseHandle(hDirectory);
		return 0;
	}
	//---------------------------------------------------------------------
	ResourceMonitor::ResourceMonitor()
	{
		InitializeCriticalSection(&gCriticalSection);
	}
	//---------------------------------------------------------------------
	ResourceMonitor::~ResourceMonitor()
	{
		DeleteCriticalSection(&gCriticalSection);
	}

	void ResourceMonitor::initMonitor(const String& BaseDir, const String& RootPathMonitor, const String & cfgFileName)
	{
		//new Root();
		gLocationName = RootPathMonitor;

		gLobalThreadControl = false;
		unsigned int connectThreadID = -1;

		//收集文件夹 和资源组
		ConfigFile cf;
		cf.load(cfgFileName);


		size_t num;
		const ConfigFile::Settings* settingSection = cf.getAllSetting(num);

		String secName, typeName, archName;
		for (size_t i = 0; i < num; ++i)
		{
			secName = settingSection[i].name;
			ConfigFile::SettingsMultiMap* settings = settingSection[i].setting;
			for (ConfigFile::SettingsMultiMap::iterator jt = settings->begin();
				jt != settings->end(); ++jt)
			{
				typeName = jt->first;
				archName = jt->second;
				if (archName[0] == '*')
				{
					archName.erase(0, 1);
					StringUtil::Trim(archName);
				}
				archName = standardisePath(archName);
				archName = RelativePathToAbsolute(BaseDir, archName);
				gLocationMap[archName] = secName;
			}
		}

		addHandler("dds", &ResourceMonitor::handleTexture);
		addHandler("jpg", &ResourceMonitor::handleTexture);
		addHandler("tga", &ResourceMonitor::handleTexture);
		addHandler("skin", &ResourceMonitor::handleMesh);
		addHandler("mesh", &ResourceMonitor::handleMesh);
		addHandler("anim", &ResourceMonitor::handleAnimation);
		addHandler("effect", &ResourceMonitor::handleEffect);
		addHandler("att", &ResourceMonitor::handleAttack);
		addHandler("skel", &ResourceMonitor::handleSkeleton);
		addHandler("ogg", &ResourceMonitor::handleSound);
		addHandler("wav", &ResourceMonitor::handleSound);
		addHandler("mp3", &ResourceMonitor::handleSound);
		addHandler("dlg", &ResourceMonitor::handleGUIdlg);
		addHandler("xml", &ResourceMonitor::handleGUIdlg);
		addHandler("camAnim", &ResourceMonitor::handleCameraAnimation);

		//启动工作线程
		mMonitorHandle = (HANDLE)_beginthreadex(0, 0, _WorkThread, 0, 0, &connectThreadID);
	}

	void ResourceMonitor::updateMonitor()
	{
		while (true)
		{
			ChangedInfo information;

			if (gChangedInfoQueue.size())
			{
				FileMutex mutex;
				information = gChangedInfoQueue.front();
				//处理变化信息
				gChangedInfoQueue.pop();
			}

			if (information.mChangeType != CT_NULL)
			{
				HandlerFuc::iterator iter = gHandlerFuc.find(information.mExtName);
				if (iter != gHandlerFuc.end())
				{
					iter->second(information);
				}
			}
			else
			{
				break;
			}
		}
	}

	//关闭监控器
	void ResourceMonitor::closeMonitor()
	{
		//gLobalThreadControl = true;
		//if(WAIT_OBJECT_0 == WaitForSingleObject(mMonitorHandle,INFINITE))
		//{
		//CloseHandle(mMonitorHandle);
		TerminateThread(mMonitorHandle, 0);

		//}
		//else
		//{
		//	assert("退出线程错误！！"&&0);
		//}
	}

	//添加新的路径和组
	void ResourceMonitor::addMonitorPath(const String& path, const String& group, bool bOverWriteGroup)
	{
		if (path.empty() == true)
		{
			return;
		}

		String dir = path;
		for (size_t i = 0; i < dir.size(); ++i)
		{
			if (dir[i] == '\\')
			{
				dir[i] = '/';
			}
		}

		if (dir[dir.size() - 1] != '/')
		{
			dir += '/';
		}

		if (false == bOverWriteGroup && gLocationMap.find(dir) != gLocationMap.end())
		{
			return;
		}

		gLocationMap[dir] = group;
	}

	void ResourceMonitor::addHandler(const String& type, handlerFuc func)
	{
		gHandlerFuc[type] = func;
	}

	// 添加或重新打开资源
	static void AddOrReloadResource(ResourceManager* resMgr, const ChangedInfo& info)
	{
		Resource* ptrRes = resMgr->getByName(info.mFileName);
		if (!ptrRes)
		{
			// 若不存在，则声明资源即可，使用时再load
			ResourceMonitor::declareResource(info.mFileName, info.mArchName, info.mGroupName);
		}
		else
		{
			// 若已存在，则重新打开
			// 如果别的文件夹下有同名已加入资源的文件，则打开的会是那个
			ptrRes->reload();
		}
	}

	//处理纹理资源
	void ResourceMonitor::handleTexture(const ChangedInfo& info)
	{
		switch (info.mChangeType)
		{
		case CT_ADD:
		{
			AddOrReloadResource(TextureManager::Instance(), info);
		}
		break;
		case CT_DELETE:
		{
			// 不删除资源，因为如果该资源被某些对象引用就会出错，即使使用智能指针也可能存在一些问题。
		}
		break;
		case CT_RENAME:
		{
			// 提取新名字
			String strPath, strFile;
			splitFilename(info.mSecondName, strFile, strPath);

			// 生成新的ChangedInfo然后走添加的流程
			ChangedInfo newInfo = info;
			newInfo.mFileName = strFile;
			AddOrReloadResource(TextureManager::Instance(), newInfo);
		}
		break;
		case CT_MODIFIED:
		{
			AddOrReloadResource(TextureManager::Instance(), info);
		}
		break;
		case CT_NUMBER:
		case CT_NULL:
		default:
			break;
		}
	}

	//处理Mesh资源：Mesh和纹理一样由ResourceManager管理，所以处理方式基本相同
	//实际上仅这样是不够的，因为Entity依赖mesh，mesh改变，相关的所有Entity都要重新初始化
	//是否进行更细致的处理，视编辑器需求而定
	void ResourceMonitor::handleMesh(const ChangedInfo& info)
	{

	}

	// 处理Skeleton资源，与Mesh相同，存在SkeletonInstance和Skeleton冲突的问题
	void ResourceMonitor::handleSkeleton(const ChangedInfo& info)
	{

	}

	//处理Effect资源
	void ResourceMonitor::handleEffect(const ChangedInfo& info)
	{

	}

	//处理声音资源
	void ResourceMonitor::handleSound(const ChangedInfo& info)
	{
		switch (info.mChangeType)
		{
		case CT_ADD:
		{
			declareResource(info.mFileName, info.mArchName, info.mGroupName);
		}
		break;
		case CT_DELETE:
		{
		}
		break;
		case CT_RENAME:
		{
			String strPath, strFile;
			splitFilename(info.mSecondName, strFile, strPath);

			declareResource(strFile, info.mArchName, info.mGroupName);

		}
		break;
		case CT_MODIFIED:
		{
		}
		break;
		case CT_NUMBER:
		case CT_NULL:
		default:
			break;
		}
	}

	//处理att资源
	void ResourceMonitor::handleAttack(const ChangedInfo& info)
	{

	}

	void ResourceMonitor::handleAnimation(const ChangedInfo& info)
	{

	}

	void ResourceMonitor::handleCameraAnimation(const ChangedInfo& info)
	{

	}

	void ResourceMonitor::handleGUIdlg(const ChangedInfo& info)
	{
		switch (info.mChangeType)
		{
		case CT_ADD:
		{
			declareResource(info.mFileName, info.mArchName, info.mGroupName);
		}
		break;
		case CT_DELETE:
		{
		}
		break;
		case CT_RENAME:
		{
			String strPath, strFile;
			splitFilename(info.mSecondName, strFile, strPath);

			declareResource(strFile, info.mArchName, info.mGroupName);

		}
		break;
		case CT_MODIFIED:
		{
		}
		break;
		case CT_NUMBER:
		case CT_NULL:
		default:
			break;
		}
	}

	// 添加进LORD索引
	void ResourceMonitor::declareResource(const String& strResName, const String& strArch, const String& strGroup)
	{
		ResMgr::ResourceGroup* grp = ResMgr::Instance()->getResourceGroup(strGroup);

		// Get archive
		Archive* pArch = ArchiveManager::Instance()->load(strArch, "FileSystem");

		// Index resources
		StringArray vec = pArch->find(strResName);
		for (StringArray::iterator it = vec.begin(); it != vec.end(); ++it)
		{
			if ((*it) == strResName)
			{
				grp->addToIndex(strResName, pArch);
			}
		}
	}

	// 解释特效脚本
	void ResourceMonitor::parseEffectScript(const String& strResName, const String& strGroup)
	{

	}

}
#endif