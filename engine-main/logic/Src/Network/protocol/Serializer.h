/********************************************************************
filename: 	Serialize
file path:	H:\sandboxol\client\blockmango-client\dev\common\Src\network\packet\Serialize.h

version:	1
author:		qutianxiang
company:    sandboxol
date:		2017/06/20
*********************************************************************/
#ifndef __SERIALIZE_H__
#define __SERIALIZE_H__
#include <map>
#include <memory>
#include "Network/protocol/DataPacket.h"
#include "Network/protocol/NetDataStream.h"
#include "TypeDef.h"
#include "Core.h"

#include <array>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <Std/type_traits.h>
#include "TypeTraits/RemoveSmartPointer.h"
#include "TypeTraits/IsSmartPointer.h"
#include "TypeTraits/IsTemplateOf.h"

using namespace LORD;

template<typename T>
struct is_encodableClass
{
private:
	template<typename U>
	static uint8_t test(decltype(&U::netWrite, &U::netRead)) { return 0; }
	template<typename U>
	static uint16_t test(...) { return 0; }
public:
	static constexpr bool value = sizeof(test<T>(0)) == sizeof(uint8_t);
};

template<typename T>
struct is_support7bitEncodeType
{
public:
	static constexpr bool value = (std::is_integral<T>::value 
		|| std::is_enum<T>::value);
};

template<typename T>
struct is_arrary_container
{
public:
	static constexpr bool value = false;
};

template<typename T, typename A>
struct is_arrary_container< std::vector<T, A> >
{
	static constexpr bool value = true;
};

template<typename T, typename A>
struct is_arrary_container< std::list<T, A> >
{
	static constexpr bool value = true;
};

template<typename T>
struct is_map_container
{
	static constexpr bool value = false;
};

template<typename T1, typename T2, typename T3, typename T4>
struct is_map_container< std::map<T1, T2, T3, T4> >
{
	static constexpr bool value = true;
};

template<typename T>
struct is_std_array
{
	static constexpr bool value = false;
};

template<typename T, size_t size>
struct is_std_array<std::array<T, size>>
{
	static constexpr bool value = true;
};

class Serializer
{
public:
	Serializer(){};
	~Serializer(){};

	template<typename T>
	static void write(const T &t, NetDataStream *dataStream)
	{
		innerWrite<T>(t, dataStream);
	}

	template<typename T>
	static void read(T &t, NetDataStream *dataStream)
	{
		innerRead(t, dataStream);
	}

	template<typename ... Args>
	static void writeMultiple(NetDataStream *dataStream, Args&& ... args)
	{
		using swallow_t = int[];
		(void)swallow_t {0, (write(std::forward<Args>(args), dataStream), 0) ...};
	}

	template<typename ... Args>
	static void readMultiple(NetDataStream *dataStream, Args&& ... args)
	{
		using swallow_t = int[];
		(void)swallow_t {0, (read(std::forward<Args>(args), dataStream), 0) ...};
	}

	template<typename T>
	static typename  std::enable_if<is_encodableClass<T>::value, void>::type innerWrite(const T &t, NetDataStream *dataStream)
	{
		t.netWrite(dataStream);
	}

	template<typename T>
	static typename  std::enable_if<is_encodableClass<T>::value, void>::type innerRead(T &t, NetDataStream *dataStream)
	{
		t.netRead(dataStream);
	}

	template<typename T>
	static typename  std::enable_if<std::is_integral<T>::value, void>::type innerWrite(const T &t, NetDataStream *NetDataStream);

	template<typename T>
	static typename  std::enable_if<std::is_integral<T>::value, void>::type innerRead(T &t, NetDataStream *NetDataStream);

	template<typename T>
	static typename  std::enable_if<std::is_enum<T>::value, void>::type innerWrite(const T &t, NetDataStream *NetDataStream);

	template<typename T>
	static typename  std::enable_if<std::is_enum<T>::value, void>::type innerRead(T &t, NetDataStream *NetDataStream);

	template<typename T>
	static typename  std::enable_if<is_arrary_container<T>::value, void>::type innerWrite(const T &t, NetDataStream *NetDataStream);

	template<typename T>
	static typename  std::enable_if<is_arrary_container<T>::value, void>::type innerRead(T &t, NetDataStream *NetDataStream);

	template<typename T>
	static typename  std::enable_if<is_std_array<T>::value, void>::type innerWrite(const T &t, NetDataStream *NetDataStream);

	template<typename T>
	static typename  std::enable_if<is_std_array<T>::value, void>::type innerRead(T &t, NetDataStream *NetDataStream);

	template<typename T, std::size_t size>
	static void writeFromArray(const std::array<T, size> &t, NetDataStream *NetDataStream);
	
	template<typename T, std::size_t size>
	static void readToArray(std::array<T, size> &t, NetDataStream *NetDataStream);

	template<typename T>
	static typename  std::enable_if<is_map_container<T>::value, void>::type innerWrite(const T &t, NetDataStream *NetDataStream);

	template<typename T>
	static typename  std::enable_if<is_map_container<T>::value, void>::type innerRead(T &t, NetDataStream *NetDataStream);

	template<typename T>
	static typename  std::enable_if<std::is_pointer<T>::value, void>::type innerWrite(const T &t, NetDataStream *NetDataStream);

	template<typename T>
	static typename  std::enable_if<std::is_pointer<T>::value, void>::type innerRead(T &t, NetDataStream *NetDataStream);

	template<typename T>
	static typename  std::enable_if<BLOCKMAN::is_smart_pointer<T>::value, void>::type innerWrite(const T &t, NetDataStream *NetDataStream);

	template<typename T>
	static typename  std::enable_if<BLOCKMAN::is_smart_pointer<T>::value, void>::type innerRead(T &t, NetDataStream *NetDataStream);

private:
};



template<typename T>
inline
typename  std::enable_if<std::is_enum<T>::value, void>::type Serializer::innerWrite(const T &t, NetDataStream *NetDataStream)
{
	int wt = static_cast<int>(t);
	innerWrite(wt, NetDataStream);
}

template<typename T>
inline
typename  std::enable_if<std::is_enum<T>::value, void>::type Serializer::innerRead(T &t, NetDataStream *NetDataStream)
{
	int rt;
	innerRead(rt, NetDataStream);

	t = static_cast<T>(rt);
}


template<typename T>
inline 
typename  std::enable_if<std::is_integral<T>::value, void>::type Serializer::innerWrite(const T &t, NetDataStream *NetDataStream)
{
	T temp = t;
	temp = (temp << 1) ^ (temp >> (sizeof(T) * 8 - 1));

	if ((((temp >> (sizeof(T) * 8 - 1)) & 0x1) == 1)) // 保存符号位之后 最高位是1，处理一下逻辑位移移进来的1.
	{
		NetDataStream->Write((ui8)(temp | 0x80));
		temp = temp >> 7;
		T high = 0x7f;
		high = high << (sizeof(T) * 8 - 7);
		high = ~high;
		temp = temp & high;
	}

	ui8 wt = 0;
	while ((temp >> 7) != 0)
	{
		wt = (ui8)(temp | 0x80);
		NetDataStream->Write(wt);
		temp = temp >> 7;
	}

	NetDataStream->Write((ui8)(temp & 0x7f));
}

template<typename T>
inline
typename  std::enable_if<std::is_integral<T>::value, void>::type Serializer::innerRead(T &t, NetDataStream *NetDataStream) 
{
	T temp = 0;
	T ret = 0;
	ui32 bitMask = 0;
	do
	{
		if (NetDataStream->GetNumberOfUnreadBits() == 0)
		{
			throw std::logic_error("7 bit decode error");
		}

		ui8 readByte;
		NetDataStream->Read(readByte);
		temp = readByte;
		ret |= ((temp & 0x7f) << bitMask);
		bitMask += 7;
	} while ((temp & 0x80) != 0);

	T retMask = ret & 0x1;
	ret = ret >> 1;

	T high = 1;
	high = high << (sizeof(T) * 8 - 1);
	high = ~high;
	ret &= high;

	if (retMask == 1)
	{
		if (std::is_unsigned<T>::value)
		{
			ret |= (retMask << (sizeof(T) * 8 - 1));
		}
		else
		{
			ret ^= -1;
		}
	}

	t = ret;
}


template<>
 inline void Serializer::write(const std::string & t, NetDataStream *NetDataStream)
{
	ui32 len = t.size();
	Serializer::write(len, NetDataStream);
	NetDataStream->Write(t.c_str(), t.size());
}

template<>
 inline void Serializer::read(std::string & t, NetDataStream *NetDataStream)
{
	ui32 stringLen;
	Serializer::read(stringLen,NetDataStream);
	char *str = new char[stringLen + 1];
	memset(str, 0, stringLen + 1);
	NetDataStream->Read(str, stringLen);
	t = std::string(str);
	delete[] str;
}

 template<>
 inline void Serializer::write(const String & t, NetDataStream *NetDataStream)
 {
	 ui32 len = t.size();
	 Serializer::write(len, NetDataStream);
	 NetDataStream->Write(t.c_str(), t.size());
 }

 template<>
 inline void Serializer::read(String & t, NetDataStream *NetDataStream)
 {
	 ui32 stringLen;
	 Serializer::read(stringLen, NetDataStream);
	 char *str = new char[stringLen + 1];
	 memset(str, 0, stringLen + 1);
	 NetDataStream->Read(str, stringLen);
	 t = String(str);
	 delete[] str;
 }

 template<>
 inline void Serializer::write(const bool & t, NetDataStream *NetDataStream)
 {
	 NetDataStream->Write(t);
 }

 template<>
 inline void Serializer::read(bool & t, NetDataStream *NetDataStream)
 {
	 bool rt;
	 NetDataStream->Read(rt);
	 t = rt;
 }

 template<>
 inline void Serializer::write(const Real & t, NetDataStream *NetDataStream)
 {
	 Real wt = t;
	 NetDataStream->Write((char *)&wt, sizeof(Real));
 }

 template<>
 inline void Serializer::read(Real & t, NetDataStream *NetDataStream)
 {
	 Real rt;
	 NetDataStream->Read((char*)&rt, sizeof(rt));
	 t = rt;
 }

 template<>
 inline void Serializer::write(const Vector3i & t, NetDataStream *NetDataStream)
 {
	 Serializer::write(t.x, NetDataStream);
	 Serializer::write(t.y, NetDataStream);
	 Serializer::write(t.z, NetDataStream);
 }

 template<>
 inline void Serializer::read(Vector3i & t, NetDataStream *NetDataStream)
 {
	 Serializer::read(t.x, NetDataStream);
	 Serializer::read(t.y, NetDataStream);
	 Serializer::read(t.z, NetDataStream);
 }

 template<>
 inline void Serializer::write(const Vector3 & t, NetDataStream *NetDataStream)
 {
	 Serializer::write(t.x, NetDataStream);
	 Serializer::write(t.y, NetDataStream);
	 Serializer::write(t.z, NetDataStream);
 }

 template<>
 inline void Serializer::read(Vector3 & t, NetDataStream *NetDataStream)
 {
	 Serializer::read(t.x, NetDataStream);
	 Serializer::read(t.y, NetDataStream);
	 Serializer::read(t.z, NetDataStream);
 }


 template<typename T>
 inline
	 typename  std::enable_if<is_arrary_container<T>::value, void>::type Serializer::innerWrite(const T &t, NetDataStream *dataStream)
 {
	 Serializer::write(t.size(), dataStream);
	 for (auto ele : t)
	 {
		 Serializer::write(ele, dataStream);
	 }
 }

 template<typename T>
 inline 
	 typename  std::enable_if<is_arrary_container<T>::value, void>::type Serializer::innerRead(T &t, NetDataStream *dataStream)
 {
	 size_t size = 0;
	 Serializer::read(size, dataStream);
	 t.clear();
	 while(size != 0)
	 {
		 size--;
		 typename T::value_type node = {};
		 Serializer::read(node, dataStream);
		 t.push_back(node);
	 }
 }

 template<typename T>
 inline typename  std::enable_if<is_std_array<T>::value, void>::type Serializer::innerWrite(const T &t, NetDataStream *dataStream)
 {
	 Serializer::writeFromArray(t, dataStream);
 }

 template<typename T>
 inline typename  std::enable_if<is_std_array<T>::value, void>::type Serializer::innerRead(T &t, NetDataStream *dataStream)
 {
	 Serializer::readToArray(t, dataStream);
 }

 template<typename T, std::size_t size>
 inline void Serializer::writeFromArray(const std::array<T, size>& t, NetDataStream * dataStream)
 {
	 for (const auto& element : t)
	 {
		 Serializer::write(element, dataStream);
	 }
 }

 template<typename T, std::size_t size>
 inline void Serializer::readToArray(std::array<T, size>& t, NetDataStream * dataStream)
 {
	 for (auto& element : t)
	 {
		 Serializer::read(element, dataStream);
	 }
 }

 template<typename T>
 inline
	 typename  std::enable_if<std::is_pointer<T>::value, void>::type Serializer::innerWrite(const T &t, NetDataStream *dataStream)
 {
	 //static_assert(!std::is_polymorphic<std::remove_pointer_t<T>>::value, "polymorphic type is not supported");
	 Serializer::write(*t, dataStream);
 }

 template<typename T>
 inline
	 typename  std::enable_if<std::is_pointer<T>::value, void>::type Serializer::innerRead(T &t, NetDataStream *dataStream)
 {
	 //static_assert(!std::is_polymorphic<std::remove_pointer_t<T>>::value, "polymorphic type is not supported");
	 t = LordNew(std::remove_pointer_t<T>);
	 Serializer::read(*t, dataStream);
 }

 template<typename T>
 inline
	 typename  std::enable_if<BLOCKMAN::is_smart_pointer<T>::value, void>::type Serializer::innerWrite(const T &t, NetDataStream *dataStream)
 {
	// static_assert(!std::is_polymorphic<BLOCKMAN::remove_smart_pointer_t<T>>::value, "polymorphic type is not supported");
	 Serializer::write(*t, dataStream);
 }

 template<typename T>
 inline
	 typename  std::enable_if<BLOCKMAN::is_smart_pointer<T>::value, void>::type Serializer::innerRead(T &t, NetDataStream *dataStream)
 {
	 //static_assert(!std::is_polymorphic<BLOCKMAN::remove_smart_pointer_t<T>>::value, "polymorphic type is not supported");
	 t.reset(LordNew(BLOCKMAN::remove_smart_pointer_t<T>));
	 Serializer::read(*t, dataStream);
 }

#endif


