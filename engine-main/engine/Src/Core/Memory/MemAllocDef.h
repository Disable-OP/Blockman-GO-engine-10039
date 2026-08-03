
/********************************************************************
filename: 	MemAllocDef.h
file path:	dev\engine\Src\Core\Memory\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_MEMALLOCDEF_H__
#define __LORD_MEMALLOCDEF_H__

#include "TypeDef.h"
#include "MemAllocObj.h"
#include "MemSTLAlloc.h"
#include <memory>



#if (LORD_MEMORY_ALLOCATOR == LORD_MEMORY_ALLOCATOR_NEDPOOLING)

#include "MemNedPooling.h"
namespace LORD
{
	// configure default allocators based on the options above
	// notice how we're not using the memory categories here but still roughing them out
	// in your allocators you might choose to create different policies per category

	// configurable category, for general malloc
	// notice how we ignore the category here, you could specialize
	class CategorisedAllocPolicy : public NedPoolingPolicy{};
	template <size_t align = 16> class CategorisedAlignAllocPolicy : public NedPoolingAlignedPolicy<align>{};

	class NoMemTraceAllocPolicy : public NedPoolingNoMemTracePolicy{};
	template <size_t align = 16> class AlignNoMemTraceAllocPolicy : public NedPoolingAlignedNoMemTracePolicy<align>{};
}
#endif
 
#if (LORD_MEMORY_ALLOCATOR == LORD_MEMORY_ALLOCATOR_HARVEYS)

#include "MemHarveysAlloc.h"
namespace LORD
{
	class CategorisedAllocPolicy : public HarveysAllocPolicy{};
	template <size_t align = 16> class CategorisedAlignAllocPolicy : public HarveysAlignedAllocPolicy<align>{};

	class NoMemTraceAllocPolicy : public HarveysAllocPolicyNoMemTrace{};
	template <size_t align = 16> class AlignNoMemTraceAllocPolicy : public HarveysAlignedAllocPolicyNoMemTrace<align>{};
}
#endif

#if (LORD_MEMORY_ALLOCATOR == LORD_MEMORY_ALLOCATOR_BINNED)

#include "BinnedMalloc.h"

namespace LORD
{
	class CategorisedAllocPolicy : public BinnedAllocPolicy {};
	template <size_t align = 16> class CategorisedAlignAllocPolicy : public BinnedAlignedAllocPolicy<align> {};

	class NoMemTraceAllocPolicy : public BinnedAllocPolicyNoMemTrace {};
	template <size_t align = 16> class AlignNoMemTraceAllocPolicy : public BinnedAlignedAllocPolicyNoMemTrace<align> {};
}

#endif

#if (LORD_MEMORY_ALLOCATOR == LORD_MEMORY_ALLOCATOR_NED)

#include "MemNedPooling.h"
namespace LORD
{
	// configure default allocators based on the options above
	// notice how we're not using the memory categories here but still roughing them out
	// in your allocators you might choose to create different policies per category

	// configurable category, for general malloc
	// notice how we ignore the category here, you could specialize
	class CategorisedAllocPolicy : public NedAllocPolicy{};
	template <size_t align = 16> class CategorisedAlignAllocPolicy : public NedAlignedAllocPolicy<align>{};

	class NoMemTraceAllocPolicy : public NedNoMemTraceAllocPolicy{};
	template <size_t align = 16> class AlignNoMemTraceAllocPolicy : public NedAlignedNoMemTraceAllocPolicy<align>{};
}

#endif

#if (LORD_MEMORY_ALLOCATOR == LORD_MEMORY_ALLOCATOR_DEFAULT)
#include "MemDefaultAlloc.h"
namespace LORD
{
    class CategorisedAllocPolicy : public DefaultPolicy{};
	template <size_t align = 16> class CategorisedAlignAllocPolicy : public DefaultAlignedPolicy<align>{};
    
	class NoMemTraceAllocPolicy : public DefaultNoMemTracePolicy{};
	template <size_t align = 16> class AlignNoMemTraceAllocPolicy : public DefaultAlignedNoMemTracePolicy<align>{};
}
#endif

	namespace LORD
	{
		// Useful shortcuts
		typedef CategorisedAllocPolicy GAP;
		typedef CategorisedAlignAllocPolicy<16> GAAP;

		typedef NoMemTraceAllocPolicy STLAP;
		typedef AlignNoMemTraceAllocPolicy<16> STLAAP;

		// Now define all the base classes for each allocation
		typedef AllocatedObject<GAP> GeneralAllocatedObject;

		// Per-class allocators defined here
		// NOTE: small, non-virtual classes should not subclass an allocator
		// the virtual function table could double their size and make them less efficient
		// use primitive or STL allocators / deallocators for those
		typedef GeneralAllocatedObject		ObjectAlloc;

		// Containers (by-value only)
		// Will  be of the form:
		// typedef STLAllocator<T, DefaultAllocPolicy, Category> TAlloc;
		// for use in vector<T, TAlloc>::type 

		//////////////////////////////////////////////////////////////////////////

		/** Utility function for constructing an array of objects with placement new,
		without using new[] (which allocates an undocumented amount of extra memory
		and so isn't appropriate for custom allocators).
		*/
		template<typename T>
		T* LordConstruct(T* basePtr, size_t count)
		{
			for (size_t i = 0; i < count; ++i)
			{
				new ((void*)(basePtr+i)) T();
			}
			return basePtr;
		}
	}


#if defined(LORD_MEMORY_TRACKER)

/// Allocate a block of raw memory.
#	define LORD_MALLOC(bytes) ::LORD::CategorisedAllocPolicy::allocateBytes(bytes, __FILE__, __LINE__, __FUNCTION__)
/// Allocate a block of memory for a primitive type.
#	define LORD_ALLOC_T(T, count) static_cast<T*>(::LORD::CategorisedAllocPolicy::allocateBytes(sizeof(T)*(count), __FILE__, __LINE__, __FUNCTION__))
/// Free the memory allocated with LORD_MALLOC or LORD_ALLOC_T.
#	define LORD_FREE(ptr) ::LORD::CategorisedAllocPolicy::deallocateBytes((void*)ptr)

/// Allocate space for one primitive type, external type or non-virtual type with constructor parameters
#	define LORD_NEW_T(T) new (::LORD::CategorisedAllocPolicy::allocateBytes(sizeof(T), __FILE__, __LINE__, __FUNCTION__)) T
/// Allocate a block of memory for 'count' primitive types - do not use for classes that inherit from AllocatedObject
#	define LORD_NEW_ARRAY_T(T, count) ::LORD::LordConstruct(static_cast<T*>(::LORD::CategorisedAllocPolicy::allocateBytes(sizeof(T)*(count), __FILE__, __LINE__, __FUNCTION__)), count) 
/// Free the memory allocated with LORD_NEW_T.
#	define LORD_DELETE_T(ptr, T) if(ptr){(ptr)->~T(); ::LORD::CategorisedAllocPolicy::deallocateBytes((void*)ptr);}
/// Free the memory allocated with LOR_NEW_ARRAY_T.
#	define LORD_DELETE_ARRAY_T(ptr, T, count) if(ptr){for (size_t b = 0; b < count; ++b) { (ptr)[b].~T();} ::LORD::CategorisedAllocPolicy::deallocateBytes((void*)ptr);}

// aligned allocation
/// Allocate a block of raw memory aligned to SIMD boundaries.
#	define LORD_MALLOC_SIMD(bytes) ::LORD::CategorisedAlignAllocPolicy::allocateBytes(bytes, __FILE__, __LINE__, __FUNCTION__)
/// Allocate a block of raw memory aligned to user defined boundaries.
#	define LORD_MALLOC_ALIGN(bytes, align) ::LORD::CategorisedAlignAllocPolicy<align>::allocateBytes(bytes, __FILE__, __LINE__, __FUNCTION__)
/// Allocate a block of memory for a primitive type aligned to SIMD boundaries.
#	define LORD_ALLOC_T_SIMD(T, count) static_cast<T*>(::LORD::CategorisedAlignAllocPolicy::allocateBytes(sizeof(T)*(count), __FILE__, __LINE__, __FUNCTION__))
/// Allocate a block of memory for a primitive type aligned to user defined boundaries.
#	define LORD_ALLOC_T_ALIGN(T, count, align) static_cast<T*>(::LORD::CategorisedAlignAllocPolicy<align>::allocateBytes(sizeof(T)*(count), __FILE__, __LINE__, __FUNCTION__))
/// Free the memory allocated with either LORD_MALLOC_SIMD or LORD_ALLOC_T_SIMD.
#	define LORD_FREE_SIMD(ptr) ::LORD::CategorisedAlignAllocPolicy::deallocateBytes(ptr)
/// Free the memory allocated with either LORD_MALLOC_ALIGN or LORD_ALLOC_T_ALIGN.
#	define LORD_FREE_ALIGN(ptr, align) ::LORD::CategorisedAlignAllocPolicy<align>::deallocateBytes(ptr)

/// Allocate space for one primitive type, external type or non-virtual type aligned to SIMD boundaries
#	define LORD_NEW_T_SIMD(T) new (::LORD::CategorisedAlignAllocPolicy::allocateBytes(sizeof(T), __FILE__, __LINE__, __FUNCTION__)) T
/// Allocate a block of memory for 'count' primitive types aligned to SIMD boundaries - do not use for classes that inherit from AllocatedObject
#	define LORD_NEW_ARRAY_T_SIMD(T, count) ::LORD::LordConstruct(static_cast<T*>(::LORD::CategorisedAlignAllocPolicy::allocateBytes(sizeof(T)*(count), __FILE__, __LINE__, __FUNCTION__)), count) 
/// Free the memory allocated with LOR_NEW_T_SIMD.
#	define LORD_DELETE_T_SIMD(ptr, T) if(ptr){(ptr)->~T(); ::LORD::CategorisedAlignAllocPolicy::deallocateBytes(ptr);}
/// Free the memory allocated with LORD_NEW_ARRAY_T_SIMD.
#	define LORD_DELETE_ARRAY_T_SIMD(ptr, T, count) if(ptr){for (size_t b = 0; b < count; ++b) { (ptr)[b].~T();} ::LORD::CategorisedAlignAllocPolicy::deallocateBytes(ptr);}
/// Allocate space for one primitive type, external type or non-virtual type aligned to user defined boundaries
#	define LORD_NEW_T_ALIGN(T, align) new (::LORD::CategorisedAlignAllocPolicy<align>::allocateBytes(sizeof(T), __FILE__, __LINE__, __FUNCTION__)) T
/// Allocate a block of memory for 'count' primitive types aligned to user defined boundaries - do not use for classes that inherit from AllocatedObject
#	define LORD_NEW_ARRAY_T_ALIGN(T, count, align) ::LORD::LordConstruct(static_cast<T*>(::LORD::CategorisedAlignAllocPolicy<align>::allocateBytes(sizeof(T)*(count), __FILE__, __LINE__, __FUNCTION__)), count) 
/// Free the memory allocated with LORD_NEW_T_ALIGN.
#	define LORD_DELETE_T_ALIGN(ptr, T, align) if(ptr){(ptr)->~T(); ::LORD::CategorisedAlignAllocPolicy<align>::deallocateBytes(ptr);}
/// Free the memory allocated with LORD_NEW_ARRAY_T_ALIGN.
#	define LORD_DELETE_ARRAY_T_ALIGN(ptr, T, count, align) if(ptr){for (size_t _b = 0; _b < count; ++_b) { (ptr)[_b].~T();} ::LORD::CategorisedAlignAllocPolicy<align>::deallocateBytes(ptr);}

#else // else - #if defined(LORD_MEMORY_TRACKER)

/// Allocate a block of raw memory.
#	define LORD_MALLOC(bytes) ::LORD::NoMemTraceAllocPolicy::allocateBytes(bytes)
/// Allocate a block of memory for a primitive type.
#	define LORD_ALLOC_T(T, count) static_cast<T*>(::LORD::NoMemTraceAllocPolicy::allocateBytes(sizeof(T)*(count)))
/// Free the memory allocated with LORD_MALLOC or LORD_ALLOC_T. Category is required to be restated to ensure the matching policy is used
#	define LORD_FREE(ptr) ::LORD::NoMemTraceAllocPolicy::deallocateBytes((void*)ptr)

/// Allocate space for one primitive type, external type or non-virtual type with constructor parameters
#	define LORD_NEW_T(T) new (::LORD::NoMemTraceAllocPolicy::allocateBytes(sizeof(T))) T
/// Allocate a block of memory for 'count' primitive types - do not use for classes that inherit from AllocatedObject
#	define LORD_NEW_ARRAY_T(T, count) ::LORD::LordConstruct(static_cast<T*>(::LORD::NoMemTraceAllocPolicy::allocateBytes(sizeof(T)*(count))), count) 
/// Free the memory allocated with LORD_NEW_T.
#	define LORD_DELETE_T(ptr, T) if(ptr){(ptr)->~T(); ::LORD::NoMemTraceAllocPolicy::deallocateBytes((void*)ptr);}
/// Free the memory allocated with LORD_NEW_ARRAY_T.
#	define LORD_DELETE_ARRAY_T(ptr, T, count) if(ptr){for (size_t b = 0; b < count; ++b) { (ptr)[b].~T();} ::LORD::NoMemTraceAllocPolicy::deallocateBytes((void*)ptr);}

// aligned allocation
/// Allocate a block of raw memory aligned to SIMD boundaries.
#	define LORD_MALLOC_SIMD(bytes) ::LORD::AlignNoMemTraceAllocPolicy::allocateBytes(bytes)
/// Allocate a block of raw memory aligned to user defined boundaries.
#	define LORD_MALLOC_ALIGN(bytes, align) ::LORD::AlignNoMemTraceAllocPolicy<align>::allocateBytes(bytes)
/// Allocate a block of memory for a primitive type aligned to SIMD boundaries.
#	define LORD_ALLOC_T_SIMD(T, count) static_cast<T*>(::LORD::AlignNoMemTraceAllocPolicy::allocateBytes(sizeof(T)*(count)))
/// Allocate a block of memory for a primitive type aligned to user defined boundaries.
#	define LORD_ALLOC_T_ALIGN(T, count, align) static_cast<T*>(::LORD::AlignNoMemTraceAllocPolicy<align>::allocateBytes(sizeof(T)*(count)))
/// Free the memory allocated with either LORD_MALLOC_SIMD or LORD_ALLOC_T_SIMD.
#	define LORD_FREE_SIMD(ptr) ::LORD::AlignNoMemTraceAllocPolicy::deallocateBytes((void*)ptr)
/// Free the memory allocated with either LORD_MALLOC_ALIGN or LORD_ALLOC_T_ALIGN.
#	define LORD_FREE_ALIGN(ptr, align) ::LORD::AlignNoMemTraceAllocPolicy<align>::deallocateBytes((void*)ptr)

/// Allocate space for one primitive type, external type or non-virtual type aligned to SIMD boundaries
#	define LORD_NEW_T_SIMD(T) new (::LORD::AlignNoMemTraceAllocPolicy::allocateBytes(sizeof(T))) T
/// Allocate a block of memory for 'count' primitive types aligned to SIMD boundaries - do not use for classes that inherit from AllocatedObject
#	define LORD_NEW_ARRAY_T_SIMD(T, count) ::LORD::LordConstruct(static_cast<T*>(::LORD::AlignNoMemTraceAllocPolicy::allocateBytes(sizeof(T)*(count))), count) 
/// Free the memory allocated with LORD_NEW_T_SIMD.
#	define LORD_DELETE_T_SIMD(ptr, T) if(ptr){(ptr)->~T(); ::LORD::AlignNoMemTraceAllocPolicy::deallocateBytes((void*)ptr);}
/// Free the memory allocated with LORD_NEW_ARRAY_T_SIMD.
#	define LORD_DELETE_ARRAY_T_SIMD(ptr, T, count) if(ptr){for (size_t b = 0; b < count; ++b) { (ptr)[b].~T();} ::LORD::AlignNoMemTraceAllocPolicy::deallocateBytes((void*)ptr);}
/// Allocate space for one primitive type, external type or non-virtual type aligned to user defined boundaries
#	define LORD_NEW_T_ALIGN(T, align) new (::LORD::AlignNoMemTraceAllocPolicy<align>::allocateBytes(sizeof(T))) T
/// Allocate a block of memory for 'count' primitive types aligned to user defined boundaries - do not use for classes that inherit from AllocatedObject
#	define LORD_NEW_ARRAY_T_ALIGN(T, count, align) ::LORD::LordConstruct(static_cast<T*>(::LORD::AlignNoMemTraceAllocPolicy<align>::allocateBytes(sizeof(T)*(count))), count) 
/// Free the memory allocated with LORD_NEW_T_ALIGN.
#	define LORD_DELETE_T_ALIGN(ptr, T, align) if(ptr){(ptr)->~T(); ::LORD::AlignNoMemTraceAllocPolicy<align>::deallocateBytes((void*)ptr);}
/// Free the memory allocated with LORD_NEW_ARRAY_T_ALIGN.
#	define LORD_DELETE_ARRAY_T_ALIGN(ptr, T, count, align) if(ptr){for (size_t _b = 0; _b < count; ++_b) { (ptr)[_b].~T();} ::LORD::AlignNoMemTraceAllocPolicy<align>::deallocateBytes((void*)ptr);}

#endif // end - #ifdef LORD_RUNMODE_DEBUG

namespace LORD
{
	/** Function which invokes OGRE_DELETE on a given pointer. 
	@remarks
	Useful to pass custom deletion policies to external libraries (e. g. boost).
	*/
	template<typename T>
	void deletePtr(T* ptr)
	{
		delete ptr;
	}
}

namespace LORD
{

	// for stl containers.
	template <typename T, typename A = SA<T, STLAP> >
	struct deque
	{
#if (LORD_STL_MEMORY_ALLOCATOR == LORD_STL_MEMORY_ALLOCATOR_CUSTOM)
		typedef typename std::deque<T, A> type;
		typedef typename std::deque<T, A>::iterator iterator;
		typedef typename std::deque<T, A>::const_iterator const_iterator;
#else
		typedef typename std::deque<T> type;
		typedef typename std::deque<T>::iterator iterator;
		typedef typename std::deque<T>::const_iterator const_iterator;
#endif
	};

	template <typename T, typename A = SA<T, STLAP> >
	struct vector
	{
#if (LORD_STL_MEMORY_ALLOCATOR == LORD_STL_MEMORY_ALLOCATOR_CUSTOM)
		typedef typename std::vector<T, A> type;
		typedef typename std::vector<T, A>::iterator iterator;
		typedef typename std::vector<T, A>::const_iterator const_iterator;
#else
		typedef typename std::vector<T> type;
		typedef typename std::vector<T>::iterator iterator;
		typedef typename std::vector<T>::const_iterator const_iterator;
#endif
	};

	template <typename T, typename A = SA<T, STLAP> >
	struct list
	{
#if (LORD_STL_MEMORY_ALLOCATOR == LORD_STL_MEMORY_ALLOCATOR_CUSTOM)
		typedef typename std::list<T, A> type;
		typedef typename std::list<T, A>::iterator iterator;
		typedef typename std::list<T, A>::const_iterator const_iterator;
#else
		typedef typename std::list<T> type;
		typedef typename std::list<T>::iterator iterator;
		typedef typename std::list<T>::const_iterator const_iterator;
#endif
	};

	template <typename T, typename P = std::less<T>, typename A = SA<T, STLAP> >
	struct set
	{
#if (LORD_STL_MEMORY_ALLOCATOR == LORD_STL_MEMORY_ALLOCATOR_CUSTOM)
		typedef typename std::set<T, P, A> type;
		typedef typename std::set<T, P, A>::iterator iterator;
		typedef typename std::set<T, P, A>::const_iterator const_iterator;
#else
		typedef typename std::set<T, P> type;
		typedef typename std::set<T, P>::iterator iterator;
		typedef typename std::set<T, P>::const_iterator const_iterator;
#endif
	};

	template <typename K, typename V, typename P = std::less<K>, typename A = SA<std::pair<const K, V>, STLAP> >
	struct map
	{
#if (LORD_STL_MEMORY_ALLOCATOR == LORD_STL_MEMORY_ALLOCATOR_CUSTOM)
		typedef typename std::map<K, V, P, A> type;
		typedef typename std::map<K, V, P, A>::iterator iterator;
		typedef typename std::map<K, V, P, A>::const_iterator const_iterator;
#else
		typedef typename std::map<K, V, P> type;
		typedef typename std::map<K, V, P>::iterator iterator;
		typedef typename std::map<K, V, P>::const_iterator const_iterator;
#endif
	};

	template <typename K, typename V, typename H = std::hash<K>, typename E = std::equal_to<K>, typename A = SA<std::pair<const K, V>, STLAP> >
	struct unordered_map
	{
#if (LORD_STL_MEMORY_ALLOCATOR == LORD_STL_MEMORY_ALLOCATOR_CUSTOM)
		typedef typename std::unordered_map<K, V, H, E, A> type;
		typedef typename std::unordered_map<K, V, H, E, A>::iterator iterator;
		typedef typename std::unordered_map<K, V, H, E, A>::const_iterator const_iterator;
#else
		typedef typename std::unordered_map<K, V, H, E> type;
		typedef typename std::unordered_map<K, V, H, E>::iterator iterator;
		typedef typename std::unordered_map<K, V, H, E>::const_iterator const_iterator;
#endif
	};

	template <typename K, typename V, typename P = std::less<K>, typename A = SA<std::pair<const K, V>, STLAP> >
	struct multimap
	{
#if (LORD_STL_MEMORY_ALLOCATOR == LORD_STL_MEMORY_ALLOCATOR_CUSTOM)
		typedef typename std::multimap<K, V, P, A> type;
		typedef typename std::multimap<K, V, P, A>::iterator iterator;
		typedef typename std::multimap<K, V, P, A>::const_iterator const_iterator;
#else
		typedef typename std::multimap<K, V, P> type;
		typedef typename std::multimap<K, V, P>::iterator iterator;
		typedef typename std::multimap<K, V, P>::const_iterator const_iterator;
#endif
	};

#define StdStringT(T) std::basic_string<T, std::char_traits<T>, std::allocator<T> >	
#define CustomMemoryStringT(T) std::basic_string<T, std::char_traits<T>, SA<T, STLAP> >

	template<typename T>
	bool operator <(const CustomMemoryStringT(T)& l,const StdStringT(T)& o)
	{
		return l.compare(0,l.length(),o.c_str(),o.length())<0;
	}
	template<typename T>
	bool operator <(const StdStringT(T)& l,const CustomMemoryStringT(T)& o)
	{
		return l.compare(0,l.length(),o.c_str(),o.length())<0;
	}
	template<typename T>
	bool operator <=(const CustomMemoryStringT(T)& l,const StdStringT(T)& o)
	{
		return l.compare(0,l.length(),o.c_str(),o.length())<=0;
	}
	template<typename T>
	bool operator <=(const StdStringT(T)& l,const CustomMemoryStringT(T)& o)
	{
		return l.compare(0,l.length(),o.c_str(),o.length())<=0;
	}
	template<typename T>
	bool operator >(const CustomMemoryStringT(T)& l,const StdStringT(T)& o)
	{
		return l.compare(0,l.length(),o.c_str(),o.length())>0;
	}
	template<typename T>
	bool operator >(const StdStringT(T)& l,const CustomMemoryStringT(T)& o)
	{
		return l.compare(0,l.length(),o.c_str(),o.length())>0;
	}
	template<typename T>
	bool operator >=(const CustomMemoryStringT(T)& l,const StdStringT(T)& o)
	{
		return l.compare(0,l.length(),o.c_str(),o.length())>=0;
	}
	template<typename T>
	bool operator >=(const StdStringT(T)& l,const CustomMemoryStringT(T)& o)
	{
		return l.compare(0,l.length(),o.c_str(),o.length())>=0;
	}

	template<typename T>
	bool operator ==(const CustomMemoryStringT(T)& l,const StdStringT(T)& o)
	{
		return l.compare(0,l.length(),o.c_str(),o.length())==0;
	}
	template<typename T>
	bool operator ==(const StdStringT(T)& l,const CustomMemoryStringT(T)& o)
	{
		return l.compare(0,l.length(),o.c_str(),o.length())==0;
	}

	template<typename T>
	bool operator !=(const CustomMemoryStringT(T)& l,const StdStringT(T)& o)
	{
		return l.compare(0,l.length(),o.c_str(),o.length())!=0;
	}
	template<typename T>
	bool operator !=(const StdStringT(T)& l,const CustomMemoryStringT(T)& o)
	{
		return l.compare(0,l.length(),o.c_str(),o.length())!=0;
	}

	template<typename T>
	CustomMemoryStringT(T) operator +=(const CustomMemoryStringT(T)& l,const StdStringT(T)& o)
	{
		return CustomMemoryStringT(T)(l)+=o.c_str();
	}
	template<typename T>
	CustomMemoryStringT(T) operator +=(const StdStringT(T)& l,const CustomMemoryStringT(T)& o)
	{
		return CustomMemoryStringT(T)(l.c_str())+=o.c_str();
	}

	template<typename T>
	CustomMemoryStringT(T) operator +(const CustomMemoryStringT(T)& l,const StdStringT(T)& o)
	{
		return CustomMemoryStringT(T)(l)+=o.c_str();
	}

	template<typename T>
	CustomMemoryStringT(T) operator +(const StdStringT(T)& l,const CustomMemoryStringT(T)& o)
	{
		return CustomMemoryStringT(T)(l.c_str())+=o.c_str();
	}

	template<typename T>
	CustomMemoryStringT(T) operator +(const T* l,const CustomMemoryStringT(T)& o)
	{
		return CustomMemoryStringT(T)(l)+=o;
	}

#undef StdStringT
#undef CustomMemoryStringT

	typedef ::std::fstream			FStream;
	typedef ::std::ifstream			IFStream;
	typedef ::std::ofstream			OFStream;

	typedef ui8		utf8;
	typedef ui32	utf32;

#if (LORD_STL_MEMORY_ALLOCATOR == LORD_STL_MEMORY_ALLOCATOR_CUSTOM)

	typedef std::basic_string<char, std::char_traits<char>, SA<char, STLAP > >	String;
	typedef std::basic_string<wchar_t, std::char_traits<wchar_t>, SA<wchar_t, STLAP > >	WString;
	typedef std::basic_stringstream<char, std::char_traits<char>, SA<char, STLAP > > StringStream;

	typedef std::basic_string<utf32, std::char_traits<utf32>, SA<utf32, STLAP> >	UTF32String;
#else

	typedef std::string			String;
	typedef std::wstring		WString;
	typedef std::stringstream	StringStream;

	typedef std::basic_string<utf32, std::char_traits<utf32> >	UTF32String;

#endif

	typedef String UTF8String;

	typedef vector<String>::type		StringArray;
	typedef vector<WString>::type		WStringArray;
}

#endif
