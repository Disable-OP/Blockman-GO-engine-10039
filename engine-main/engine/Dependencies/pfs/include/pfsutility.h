#pragma once

namespace PFS {
namespace helper { class CMemoryData; }

typedef unsigned long long	fsize_t; // 64bit的文件大小和文件指针位置
typedef long long			foff_t;	 // 64bit的文件指针相对偏移量

class CUncopyableObject
{
	CUncopyableObject( const CUncopyableObject&);
	CUncopyableObject& operator=( const CUncopyableObject&);
public:
	CUncopyableObject();
	virtual ~CUncopyableObject();
};

class CDataBlock
{
	helper::CMemoryData*	m_data;
public:

	//
	// 注意：c仅为初始保留容量(capacity)，而非size
	//
	CDataBlock( size_t c = 0);

	// 注意：
	// data 为初始化数据
	// s为初始大小(size)
	CDataBlock( const void* data, size_t s);
	~CDataBlock();
	CDataBlock( const CDataBlock& src);
	CDataBlock& operator=( const CDataBlock& src);
private:
	void Unique();

	// c is the reserved data length
	// s is the new size
	void UniqueReserveAndResize(size_t c, size_t s);
public:
	void* GetData();
	const void* GetData() const;
	size_t GetSize() const;
	void Resize( size_t s);
	void Reserve( size_t s);
	void Clear( bool deep = false);
	void Assign( const void* data, size_t size);
	void Append( const void* data, size_t size);
	void Append( const CDataBlock& data);
	void Swap( CDataBlock& dst);
public:
	bool Compare( const CDataBlock& dst) const;
	bool operator!=( const CDataBlock& dst) const;
	bool operator==( const CDataBlock& dst) const;
public:
	void Dump() const;
};

class CAbstactImageImplement : public CUncopyableObject
{
	volatile long m_counter;
public:
	CAbstactImageImplement();
	virtual ~CAbstactImageImplement();
public:
	virtual long AddRef();
	virtual long Release();

	virtual const void* GetData() const = 0;
	virtual fsize_t GetSize() const = 0;
};

class CImageData
{
	CAbstactImageImplement*	m_impl;

//	static void* operator new( size_t s) { return NULL; }
public:
	CImageData();
	CImageData( const CImageData& src);
	explicit CImageData( CAbstactImageImplement* impl);
	~CImageData();

	CImageData& operator=( const CImageData& src);
	CImageData& Attach( CAbstactImageImplement* impl) ;
	CAbstactImageImplement* Detach();
public:
	const void* GetData() const;
	fsize_t GetSize() const;
};

PFS::CImageData DataBlockToImageData( const PFS::CDataBlock& data);

class COutLog : public CUncopyableObject
{
public:
	COutLog() {}
	virtual ~COutLog() {}
public:
	virtual void Print( const wchar_t* info, ...);
	virtual void VPrint( const wchar_t* info, va_list vl) = 0;
};

} // namespace PFS {