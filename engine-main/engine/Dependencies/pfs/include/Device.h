// 
// DeviceSystem - CDevice, CDeviceFile
// 2010-11-15
//
#pragma once

namespace PFS
{
	class IAbstractDevice; // 内部实现类，不对外提供定义。
	class CBaseFile;
	class CDeviceFile;

	class CDevice
	{
	public:
		CDevice(void);
		~CDevice(void);

		//
		// Mounts the storage device to the device object.
		//
		// device - the full path of the storage device. 
		// Can be native file path/native directory path or pfs file path/pfs directory.
		// mt - the mount type.
		//
		bool Mount(const std::wstring& device, MOUNT_TYPE mt = MT_READONLY);

		//
		// Umounts the storage device.
		//
		void Unmount();

		//
		// Is the file existing in the storage device?
		//
		bool IsFileExisting( const std::wstring& filename ) const;

		//
		// Is the directory existing in the storage device?
		//
		bool IsDirectoryExisting( const std::wstring& dirname ) const;

		//
		// Deletes a file in the storage device.
		//
		bool RemoveFile( const std::wstring& filename );

	private:
		//
		// 只允许CDeviceFile访问OpenFile/CloseFile方法，
		// 其他场合不要使用OpenFile/CloseFile，简化CDeviceFile对象的生存期管理模型。
		// 
		friend class CDeviceFile;
		CBaseFile* OpenFile(const std::wstring& device, PFS::FILE_MODE fm, PFS::FILE_ACCESS fa);
		void CloseFile(CBaseFile* pFile);

	private:
		IAbstractDevice* m_pAbstractDevice;
	};

	class CDeviceFile : public CFile
	{
	public:
		CDeviceFile();
		CDeviceFile( CDevice* pDevice );
		virtual ~CDeviceFile();

		//
		// Opens the file in the storage device.
		//
		// filename - the relative path of the file that in the device.
		// It is not required to be begin with '/' or '\\', but it could be.
		//
		virtual bool Open( CDevice* pDevice, const std::wstring& filename,
			PFS::FILE_MODE fm, PFS::FILE_ACCESS fa);

		// After constructed with CDeviceFile( CDevice* pDevice ), 
		// use this method to open a file in the device.
		// If m_pDevice is NULL, opens a file as that is in CFile::Open().
		virtual bool Open( const std::wstring& filename, 
			FILE_MODE fm, FILE_ACCESS fa);

		virtual void Close();

	protected:
		CDevice* m_pDevice;
	};

}