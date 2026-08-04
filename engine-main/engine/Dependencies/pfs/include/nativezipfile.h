#pragma once

namespace PFSX {

class CZipReadFile : public PFS::CBaseFile
{
protected:
	class CData;
	CData*		m_data;
public:
	CZipReadFile( PFS::CBaseFile& bf, fsize_t fullsize, bool zip = true);
	virtual ~CZipReadFile();
public:
	virtual size_t Read( void* data, size_t size);
	virtual size_t Write( const void* data, size_t size);
	virtual fsize_t Seek( foff_t pos, PFS::FILE_SEEK_METHOD fsm);
	virtual fsize_t GetSize();
	virtual fsize_t GetPos();
	virtual bool SetEnd();
	virtual PFS::CImageData GetImage();
	virtual bool ReadAll( void* data, fsize_t size );
};

class CZipWriteFile : public PFS::CBaseFile
{
protected:
	class CData;
	CData*		m_data;
public:
	CZipWriteFile( PFS::CBaseFile& bf, int level, bool zip = true);
	virtual ~CZipWriteFile();
public:
	virtual size_t Read( void* data, size_t size);
	virtual size_t Write( const void* data, size_t size);
	virtual fsize_t Seek( foff_t pos, PFS::FILE_SEEK_METHOD fsm);
	virtual fsize_t GetSize();
	virtual fsize_t GetPos();
	virtual bool SetEnd();
	virtual PFS::CImageData GetImage();
	virtual bool ReadAll( void* data, fsize_t size );
};

} // namespace PFSX {

