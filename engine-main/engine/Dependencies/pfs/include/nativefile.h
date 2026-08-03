#pragma once

namespace PFS { class CFileIO; class CFileImage; }

namespace PFSX {

class IGetFileIO
{
public:
	IGetFileIO() {}
	virtual ~IGetFileIO() {}
public:
	virtual PFS::CFileIO& GetRawFileIO() const = 0;
};
class CNativeFile : public PFS::CBaseFile, public IGetFileIO
{
	friend class CNativeImageData;
	
	PFS::CFileIO*		m_fileio;
public:
	CNativeFile();
	virtual ~CNativeFile();
public:
	virtual bool Open( const std::wstring& filename, PFS::FILE_MODE fm, PFS::FILE_ACCESS fa);
	virtual void Close();
	virtual PFS::CFileIO& GetRawFileIO() const { return *m_fileio; }
public:
	virtual size_t Read( void* data, size_t size);
	virtual size_t Write( const void* data, size_t size);
	virtual fsize_t GetSize();
	virtual fsize_t Seek( foff_t pos, PFS::FILE_SEEK_METHOD fsm);
	virtual fsize_t GetPos();
	virtual bool SetEnd();
	virtual PFS::CImageData GetImage();
	virtual bool ReadAll( void* data, fsize_t size );
};

} // namespace PFSX {