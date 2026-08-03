/********************************************************************
filename: 	ResourceMonitor.h
file path:	dev\engine\Src\Core\Resource\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef LORD_RESOURCEMONITOR_H
#define LORD_RESOURCEMONITOR_H

#include "Core.h"

#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
namespace LORD
{
	enum ChangeType
	{
		CT_NULL = 0,
		CT_ADD, //增加
		CT_DELETE, //删除
		CT_RENAME, //改名
		CT_MODIFIED,//修改
		CT_NUMBER, 
	};

	struct ChangedInfo
	{
		ChangeType mChangeType; //类型
		String mFirstName; //第一个文件名字，和前面的类型有关
		String mSecondName;//第二个文件名字，和前面类型有关

		String mFileName;// 文件名称
		String mExtName; //扩展名
		String mGroupName; //组名称
		String mArchName; // 档案名

		ChangedInfo()
		{
			mChangeType = CT_NULL;
			mFirstName = "";
			mSecondName = "";
			mFileName = "";
			mExtName = "";
			mGroupName = "";
			mArchName = "";
		}

		bool operator == (const ChangedInfo& other)
		{
			if(mChangeType == other.mChangeType &&
				mFirstName == other.mFirstName&&
				mSecondName == other.mSecondName)
				return true;
			else
				return false;
		}
	};

	typedef void (*handlerFuc)(const ChangedInfo&info);

	class LORD_CORE_API ResourceMonitor : public Singleton<ResourceMonitor> , public ObjectAlloc
	{
	public:
		static ResourceMonitor* Instance()
		{
			return ms_pSingleton;
		}
	public:
		ResourceMonitor();
		virtual ~ResourceMonitor();

	public:

		//用resource.cfg初始化管理器
		void initMonitor(const String& BaseDir, const String& RootPathMonitor, const String & cfgFileName);

		//更新监控器
		void updateMonitor();

		//关闭监控器
		void closeMonitor();

		//添加新的路径和组
		void addMonitorPath(const String& path, const String& group, bool bOverWriteGroup = false);

	public:

		//添加文件类型处理函数
		void addHandler(const String & type, handlerFuc);


	public:
		//处理纹理资源
		static void handleTexture(const ChangedInfo& info);

		//处理Skin资源
		static void handleSkin(const ChangedInfo& info);

		//处理Mesh资源
		static void handleMesh(const ChangedInfo& info);

		//处理骨骼资源
		static void handleSkeleton(const ChangedInfo& info);

		//处理Effect资源
		static void handleEffect(const ChangedInfo& info);

		//处理att资源
		static void handleAttack(const ChangedInfo& info);

		// 处理anim资源
		static void handleAnimation(const ChangedInfo& info);

		//处理声音资源
		static void handleSound(const ChangedInfo& info);

		//处理GUI的dlg资源
		static void handleGUIdlg(const ChangedInfo& info);

		//处理摄像机动画资源
		static void handleCameraAnimation(const ChangedInfo& info);
	public:
		// 添加进ECHO索引
		static void declareResource(const String& strResName, const String& strArch, const String& strGroup);

		// 解释特效脚本
		static void parseEffectScript(const String& strResName, const String& strGroup);

	private:

		//监控线程句柄
		HANDLE mMonitorHandle;

	};	
}
#endif
#endif