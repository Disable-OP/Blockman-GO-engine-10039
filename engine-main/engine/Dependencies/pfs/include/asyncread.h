#pragma once

namespace PFSX {

namespace helper { class CAsyncRead; class CSyncRead; }

enum { SIZE_ENTRY_FILE = -1, };

class CAyncReadBaseNotify : public PFS::CUncopyableObject
{
	friend helper::CAsyncRead;
public:
	CAyncReadBaseNotify() {}
	virtual ~CAyncReadBaseNotify() {}
protected:
	// 在辅助读取线程响应，返回true不再进行OnReady通告
	virtual bool DoRead( const std::wstring& filename, PFS::CImageData& data, int& error) = 0;
	// 在调用 AsyncReadUpdate 的线程中响应
	virtual void OnReady( int nError, const std::wstring& filename, const PFS::CImageData& data) {} 
};

class CAyncReadNotify : public CAyncReadBaseNotify
{
public:
	CAyncReadNotify() {}
	virtual ~CAyncReadNotify() {}
protected:
	virtual bool DoRead( const std::wstring& filename, PFS::CImageData& data, int& error);
	// 在辅助读取线程响应，返true不再进行OnReady通告
	virtual bool DoAfterRead( int nError, const std::wstring& filename, const PFS::CImageData& data) { return false; } 

	virtual bool DoOpen( PFS::CFile& file, const std::wstring& filename)
	{
		return file.Open( filename, PFS::FM_EXCL, PFS::FA_RDONLY);
	}
};

int AsyncReadInitialize( int levelcount = 1);
int AsyncReadUninitialize();
int AsyncReadUpdate( bool bAll = false);
int AsyncReadClear();
int AsyncRead( const std::wstring& filename, CAyncReadBaseNotify& notify, int level = 0);

class CSyncFile : public PFS::CImageFile
{
	friend helper::CSyncRead;
public:
	CSyncFile() {}
	virtual ~CSyncFile() {}
public:
	bool Open( const std::wstring& filename);
};

} // namespace PFSX {