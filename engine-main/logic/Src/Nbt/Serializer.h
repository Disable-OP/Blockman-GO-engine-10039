#ifndef __SERIALIZER_HEADER__
#define __SERIALIZER_HEADER__

#include <memory>
#include <istream>
#include <ostream>
#include <fstream>
#include "Nbt/NbtBinding.h"
#include "Nbt/NbtGenerator.h"
#include "Nbt/NbtParser.h"
#include "Core.h"

namespace BLOCKMAN
{
	template<typename T>
	void serialize(const T* pObject, NbtGenerator & generator, const bool encloseInCompoundTag = true)
	{
		NbtBinding<T>::getInstance()->serialize(pObject, generator, encloseInCompoundTag);
	}
	template<typename T>
	bool deserialize(T* pObject, NbtParser & parser, const bool startWithCompoundTag = true)
	{
		return NbtBinding<T>::getInstance()->deserialize(pObject, parser, startWithCompoundTag);
	}

	template<typename T, typename OutputStream>
	inline void serialize(const T* pObject, OutputStream & output, const bool encloseInCompoundTag = true)
	{
		NbtGenerator generator(output);
		serialize(pObject, generator, encloseInCompoundTag);
	}

	template<typename T, typename InputStream>
	inline bool deserialize(T* pObject, InputStream & input)
	{
		NbtParser parser(input);
		return deserialize(pObject, parser);
	}
}

#endif
