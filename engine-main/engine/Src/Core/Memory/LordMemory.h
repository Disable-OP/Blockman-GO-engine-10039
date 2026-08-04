/********************************************************************
filename: 	LordMemory.h
file path:	dev\engine\Src\Core\Memory\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_MEMORY_H__
#define __LORD_MEMORY_H__

#include "MemAllocDef.h"
#include "Std/type_traits.h"

// Global Interface Definitions

#ifdef LORD_MEMORY_TRACKER
#define LordNew									new(__FILE__, __LINE__, __FUNCTION__)
#define LordDelete								delete
#else
#define LordNew									new
#define LordDelete								delete
#endif
#define LordSafeDelete(ptr)						{ if(ptr) { LordDelete ptr; ptr = NULL; } }
#define LordSafeDeleteArray(ptr)				{ if(ptr) { LordDelete[] ptr; ptr = NULL; } }

#define LordSafeDeleteVector(v)					{ for ( size_t i=0; i<v.size(); i++) LordSafeDelete( v[i]); v.clear();}

#define LordNewT(T)								LORD_NEW_T(T)
#define LordNewArrayT(T, count)					LORD_NEW_ARRAY_T(T, count)
#define LordDeleteT(ptr, T)						LORD_DELETE_T(ptr, T)
#define LordDeleteArrayT(ptr, T, count)			LORD_DELETE_ARRAY_T(ptr, T, count)
#define LordSafeDeleteT(ptr, T)					{ if(ptr) { LordDeleteT(ptr, T); (ptr) = NULL; } }
#define LordSafeDeleteArrayT(ptr, T, count)		{ if(ptr) { LordDeleteArrayT(ptr, T, count); (ptr) = NULL; } }

#define LordMalloc(bytes)						LORD_MALLOC(bytes)
#define LordAlloc(T, count)						LORD_ALLOC_T(T, count)
#define LordFree(ptr)							LORD_FREE(ptr)
#define LordSafeFree(ptr)						{ if(ptr) { LordFree(ptr); (ptr) = NULL; } }
#define LordSafeFreeVector(v)					{ for ( size_t i=0; i<v.size(); i++) LordSafeFree( v[i]); v.clear();}
#define LordSafeFreeContainer(v)				{ for(auto it : v) LordSafeFree(it); v.clear(); }
#define LordSafeDeleteContainer(v)				{ for(auto it : v) LordSafeDelete(it); v.clear(); }

// for COM Object
#ifdef SafeRelease
#undef SafeRelease
#endif
#define SafeRelease(ptr)						{ if(ptr) { (ptr)->Release(); (ptr) = NULL; } }


namespace LORD 
{
	template<class T, class... _Types> inline
		std::shared_ptr<T> make_shared(_Types&&... _Args)
	{
		static_assert(!std::is_array<T>::value, "LORD::make_shared cannot be used for array type");
		//return std::allocate_shared<T, SA<T, STLAP> >(SA<T, STLAP>(), std::forward<_Types>(_Args)...);
		return std::shared_ptr<T>(new T(std::forward<_Types>(_Args)...));
	}

	template<class T, class... _Types> inline
		std::enable_if_t<std::is_base_of<ObjectAlloc, T>::value, std::unique_ptr<T>> make_unique(_Types&&... _Args)
	{
		static_assert(!std::is_array<T>::value, "LORD::make_unique cannot be used for array type");
		return std::unique_ptr<T>(LordNew T(std::forward<_Types>(_Args)...));
	}

	template<typename T>
	struct LordDeleter
	{
		void operator()(T* ptr) const
		{
			LordDeleteT(ptr, T);
		}
	};

	template<class T, class... _Types> inline
		std::enable_if_t<!std::is_base_of<ObjectAlloc, T>::value, std::unique_ptr<T, LordDeleter<T>>> make_unique(_Types&&... _Args)
	{
		static_assert(!std::is_array<T>::value, "LORD::make_unique cannot be used for array type");
		return std::unique_ptr<T, LordDeleter<T>>(LordNewT(T)(std::forward<_Types>(_Args)...));
	}
}
#endif