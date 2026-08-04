//////////////////////////////////////////////////////////////////////////
//
// Marshal.h
//
// 由当前编译器决定字节序的打包解包相关接口和实现
//
// author : liudongxue
// date : 2008-11-17
//
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "pfsutility.h" // for PFS::CDataBlock

namespace PFS
{
namespace MARSHAL
{	

	//
	// 打包解包异常
	//
	class CMarshalException { };

	// 前置声明
	class CMemoryStream;

	//
	// 所有可被CMemoryStream打包解包的自定义对象所应该实现的接口
	//
	class IMarshalable
	{
	public:
		virtual CMemoryStream& marshal(CMemoryStream &os) const = 0;
		virtual CMemoryStream& unmarshal(CMemoryStream &os) = 0;
	};
	
	//
	// 对内存数据进行打包解包的流
	//
	class CMemoryStream
	{
		CDataBlock  data;
		unsigned int pos;

		template<typename T>
		CMemoryStream& push_byte(T t)
		{
			data.Append(&t, sizeof(t));
			return *this;
		}

		template<typename T>
		CMemoryStream& pop_byte(T &t) 
		{
			if (pos + sizeof(t) > data.GetSize()) 
				throw CMarshalException();

			t = *(T *)( (char*)data.GetData() + pos );
			pos += sizeof(t);
			return *this;
		}

	public:
		//////////////////////////////////////////////////////////////////////////
		//
		// 构造函数
		//
		//////////////////////////////////////////////////////////////////////////

		CMemoryStream() : pos(0) {}
		CMemoryStream(const CDataBlock &o) : data(o), pos(0) {}
		CMemoryStream(const CMemoryStream &os) : data(os.data), pos(0) {}
		const CMemoryStream& operator = (const CMemoryStream &os) 
		{
			if (&os != this)
			{
				pos  = os.pos;
				data = os.data;
			}
			return *this;
		}

	public:
		//////////////////////////////////////////////////////////////////////////
		//
		// 非常规接口
		//
		//////////////////////////////////////////////////////////////////////////

		bool operator == (const CMemoryStream &os) const { return data == os.data; }
		bool operator != (const CMemoryStream &os) const { return data != os.data; }

		size_t size() const { return data.GetSize(); }
		void swap (CMemoryStream &os) { data.Swap(os.data); unsigned int _pos = pos; pos = os.pos; os.pos = _pos; }
		operator CDataBlock& () { return data; }
		void reserve(size_t size) { data.Reserve(size); }
		void clear() { data.Clear(); pos = 0; }
		bool eos() const { return pos == data.GetSize(); }		

	public:
		//////////////////////////////////////////////////////////////////////////
		//
		// 写操作接口
		//
		//////////////////////////////////////////////////////////////////////////
		CMemoryStream& operator << (char x) 
		{
			return push_byte(x);
		}

		CMemoryStream& operator << (unsigned char x) 
		{
			return push_byte(x);
		}

		CMemoryStream& operator << (bool x) 
		{
			return push_byte((unsigned char)x);
		}

		CMemoryStream& operator << (short x) 
		{
			return push_byte(x);
		}

		CMemoryStream& operator << (unsigned short x) 
		{
			return push_byte(x);
		}

		CMemoryStream& operator << (int x) 
		{
			return push_byte(x);
		}

		CMemoryStream& operator << (unsigned int x) 
		{
			return push_byte(x);
		}

		CMemoryStream& operator << (long x) 
		{
			return push_byte(x);
		}

		CMemoryStream& operator << (unsigned long x) 
		{
			return push_byte(x);
		}

		CMemoryStream& operator << (long long x) 
		{
			return push_byte(x);
		}

		CMemoryStream& operator << (unsigned long long x) 
		{
			return push_byte(x);
		}

		CMemoryStream& operator << (float x) 
		{
			return push_byte(x);
		}

		CMemoryStream& operator << (double x) 
		{
			return push_byte(x);
		}

		CMemoryStream& operator << (const IMarshalable &x)
		{ 
			return x.marshal(*this);
		}

		CMemoryStream& push_byte(const char *x, size_t len)
		{
			data.Append(x, len);
			return *this;
		}

	public:
		//////////////////////////////////////////////////////////////////////////
		//
		// 读操作接口
		//
		//////////////////////////////////////////////////////////////////////////

		CMemoryStream& operator >> (char &x)
		{
			return pop_byte(x);
		}
		
		CMemoryStream& operator >> (unsigned char &x)
		{
			return pop_byte(x);
		}

		CMemoryStream& operator >> (bool &x) 
		{
			unsigned char _x;
			pop_byte(_x);
			x = !!_x;
			return *this;
		}

		CMemoryStream& operator >> (short &x)
		{
			return pop_byte(x);
		}

		CMemoryStream& operator >> (unsigned short &x)
		{
			return pop_byte(x);
		}

		CMemoryStream& operator >> (int &x)
		{
			return pop_byte(x);
		}

		CMemoryStream& operator >> (unsigned int &x)
		{
			return pop_byte(x);
		}

		CMemoryStream& operator >> (long &x)
		{
			return pop_byte(x);
		}

		CMemoryStream& operator >> (unsigned long &x)
		{
			return pop_byte(x);
		}

		CMemoryStream& operator >> (long long &x)
		{
			return pop_byte(x);
		}

		CMemoryStream& operator >> (unsigned long long &x)
		{
			return pop_byte(x);
		}

		CMemoryStream& operator >> (float &x)
		{
			return pop_byte(x);
		}

		CMemoryStream& operator >> (double &x)
		{
			return pop_byte(x);
		}

		CMemoryStream& operator >> (IMarshalable &x)
		{
			return x.unmarshal(*this);
		}
		
		CMemoryStream& pop_byte(char *x, size_t len)
		{
			if (pos + len > data.GetSize())
			{
				throw CMarshalException();
			}

			memcpy(x, (char*)data.GetData()+pos, len);
			pos += (unsigned int)len;
			return *this;
		}

	}; // CMemoryStream
	
};// namespace MARSHAL

}; // namespace PFS