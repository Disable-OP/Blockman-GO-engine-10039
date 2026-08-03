//////////////////////////////////////////////////////////////////////////
//
// InetFile.h
//
//  [10/27/2008 liudongxue]
//
//////////////////////////////////////////////////////////////////////////
#pragma once

#ifndef _WININET_
// 如果没有定义HINTERNET
typedef void* HINTERNET;
#endif

namespace PFS { class CInetIO; }
namespace PFSX {

class CInetFile : public PFS::CBaseFile
{	
public:
	CInetFile(void);
	~CInetFile(void);

public:
	//
	// summary:
	//		Open a internet object by the url and range parameters if any
	// parameters:
	//		[in] url - the object url.
	//		[in/optional] lRangeBegin - the first bytes to download(0 means the first byte).
	//		[in/optional] lRangeEnd - the last bytes to download(-1 means the last byte).
	// returns:
	//		TRUE: succeeded to open the file.
	//		FALSE: failed to open the file. Call GetLastError() to query the Win32 error code.
	virtual bool Open( const std::wstring& url );
	virtual bool Open( const std::wstring& url, long lRangeBegin, long lRangeEnd = -1L );

	//
	// summary:
	//		Opens the internet object by the connection handle and object name.
	//
	// parameters:
	//		[in] hConnect - A handle to an HTTP session returned by InternetConnect. 
	//		[in] lpszObjectName - the object name.
	//		[in/optional] lRangeBegin - the first bytes to download(0 means the first byte).
	//		[in/optional] lRangeEnd - the last bytes to download(-1 means the last byte).
	//
	// returns:
	//		TRUE: succeeded to open the file.
	//		FALSE: failed to open the file. Call GetLastError() to query the Win32 error code.
	//
	virtual bool Open( HINTERNET hConnect, const std::wstring& strObjectName );
	virtual bool Open( HINTERNET hConnect, const std::wstring& strObjectName, long lRangeBegin, long lRangeEnd = -1L );
	
	//
	// summary: Close the file
	//
	virtual void Close();

public:
	virtual size_t Read( void* data, size_t size);
	virtual size_t Write( const void* data, size_t size);
	virtual fsize_t GetSize();
	virtual fsize_t Seek( foff_t pos, PFS::FILE_SEEK_METHOD fsm);
	virtual fsize_t GetPos();
	virtual bool SetEnd();
	virtual PFS::CImageData GetImage();
	virtual bool ReadAll( void* data, fsize_t size );
private:
	PFS::CInetIO* m_fileio;
};

} // namespace PFSX {