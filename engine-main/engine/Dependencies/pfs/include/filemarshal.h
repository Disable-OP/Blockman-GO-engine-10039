#pragma once

namespace PFS {

class CFileStream;
class CFileStreamException {};

class CFileMarshal
{
public:
	CFileMarshal();
	virtual ~CFileMarshal();
public:
	virtual CFileStream& marshal( CFileStream& os) const = 0;
	virtual const CFileStream& unmarshal( const CFileStream& os) = 0;
};

class CFileStream : public PFS::CUncopyableObject
{
private:
	mutable PFS::CBaseFile&		m_file;

public:
	CFileStream( PFS::CBaseFile& file);
	virtual ~CFileStream();

	fsize_t Seek( foff_t pos, PFS::FILE_SEEK_METHOD fsm) const;
private:
	template<typename T> CFileStream& push_byte( T t)
	{
		return push_byte( &t, sizeof( t));
	}

	template<typename T> void pop_byte( T &t) const
	{
		pop_byte( &t, sizeof( t));
	}
	unsigned char pop_byte_8() const
	{
		unsigned char	value;
		pop_byte( value);
		return value;
	}
	unsigned short pop_byte_16() const
	{
		unsigned short	value;
		pop_byte( value);
		return value;
	}
	unsigned long pop_byte_32() const
	{
		unsigned long	value;
		pop_byte( value);
		return value;
	}
	unsigned long long pop_byte_64() const
	{
		unsigned long long	value;
		pop_byte( value);
		return value;
	}

	template<typename T> static inline T& remove_const(const T &t) { return const_cast<T&>(t); }

public:
	CFileStream& push_byte( const void* x, size_t len);
	CFileStream& push_string( const std::wstring& str);
	CFileStream& push_data( const CDataBlock& data);
	CFileStream& operator << ( char x)               { return push_byte( x); }
	CFileStream& operator << ( unsigned char x)      { return push_byte( x); }
	CFileStream& operator << ( bool x)               { return push_byte( x); }
	CFileStream& operator << ( short x)              { return push_byte( x); }
	CFileStream& operator << ( unsigned short x)     { return push_byte( x); }
	CFileStream& operator << ( int x)                { return push_byte( x); }
	CFileStream& operator << ( unsigned int x)       { return push_byte( x); }
	CFileStream& operator << ( long x)               { return push_byte( x); }
	CFileStream& operator << ( unsigned long x)      { return push_byte( x); }
	CFileStream& operator << ( long long x)          { return push_byte( x); }
	CFileStream& operator << ( unsigned long long x) { return push_byte( x); }
	CFileStream& operator << ( double x)			 { return push_byte( x); }
	CFileStream& operator << ( const std::wstring& str) { return push_string( str); }
	CFileStream& operator << ( const CDataBlock& data) { return push_data( data); }
	CFileStream& operator << ( const CFileMarshal& x) 
	{
		x.marshal( *this);
		return *this; 
	}

public:
	CFileStream& push_size(size_t x);
	size_t pop_size() const;

	void pop_byte( void* x, size_t len) const;
	void pop_string( std::wstring& str) const;
	void pop_data( CDataBlock& data) const;
	const CFileStream& operator >> ( const char &x) const
	{
		remove_const( x) = pop_byte_8();
		return *this;
	}
	const CFileStream& operator >> ( const unsigned char &x) const
	{
		remove_const( x) = pop_byte_8();
		return *this;
	}
	const CFileStream& operator >> ( const bool &x) const
	{
		remove_const( x) = !!pop_byte_8();
		return *this;
	}
	const CFileStream& operator >> ( const short &x) const
	{
		remove_const( x) = pop_byte_16();
		return *this;
	}
	const CFileStream& operator >> ( const unsigned short &x) const
	{
		remove_const( x) = pop_byte_16();
		return *this;
	}
	const CFileStream& operator >> ( const int &x) const
	{
		remove_const( x) = pop_byte_32();
		return *this;
	}
	const CFileStream& operator >> ( const unsigned int &x) const
	{
		remove_const( x) = pop_byte_32();
		return *this;
	}
	const CFileStream& operator >> ( const long &x) const
	{
		remove_const( x) = pop_byte_32();
		return *this;
	}
	const CFileStream& operator >> ( const unsigned long &x) const
	{
		remove_const( x) = pop_byte_32();
		return *this;
	}
	const CFileStream& operator >> ( const long long &x) const
	{
		remove_const( x) = pop_byte_64();
		return *this;
	}
	const CFileStream& operator >> ( const unsigned long long &x) const
	{
		remove_const( x) = pop_byte_64();
		return *this;
	}
	const CFileStream& operator >> ( const double &x) const
	{
		pop_byte( remove_const( x));
		return *this;
	}
	const CFileStream& operator >> ( const std::wstring& x) const
	{
		pop_string( remove_const( x));
		return *this;
	}
	const CFileStream& operator >> ( const CDataBlock& x)  const
	{
		pop_data( remove_const( x));
		return *this;
	}
	const CFileStream& operator >> ( const CFileMarshal& x)  const
	{
		remove_const( x).unmarshal( *this);
		return *this; 
	}
};

} // namespace PFSX {
