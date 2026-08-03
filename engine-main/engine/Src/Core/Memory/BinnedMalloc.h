/********************************************************************
filename: 	BinnedMalloc.h
file path:	dev\engine\Src\Core\Memory\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef _BinnedMemory_H_
#define _BinnedMemory_H_

#if LORD_MEMORY_ALLOCATOR  == LORD_MEMORY_ALLOCATOR_BINNED

#include <stddef.h>
#include <new>

namespace LORD
{
    enum { DEFAULT_ALIGNMENT = 0 };

    class LORD_CORE_API MallocBinnedMgr
    {
    public:
        static void* Malloc( size_t count, unsigned int aligment = DEFAULT_ALIGNMENT);
        static void* Realloc( void* Ptr, size_t NewSize, unsigned int Alignment = DEFAULT_ALIGNMENT);
        static void Free(void* ptr);
        
    };
	
    class LORD_CORE_API BinnedAllocPolicy
    {
    public:
		static void* allocateBytes(size_t count, const char* file = 0, int line = 0, const char* func = 0);
            
        static inline void* reallocBytes( void* Ptr, size_t NewSize, const char* file = 0, int line = 0, const char* func = 0)
        {
            return MallocBinnedMgr::Realloc(Ptr, NewSize);
        }

		static void deallocateBytes(void* ptr);
        
        /// Get the maximum size of a single allocation
        static inline size_t getMaxAllocationSize()
        {
            return (std::numeric_limits<size_t>::max)();
        }
    private:
        // No instantiation
        BinnedAllocPolicy()
        { }
    };

    template <size_t Alignment = 0>
    class BinnedAlignedAllocPolicy
    {
    public:
        // compile-time assert alignment is available.
        typedef int IsValidAlignment
            [Alignment <= 128 && ((Alignment & (Alignment-1)) == 0) ? +1 : -1];

		static inline void* allocateBytes(size_t count, const char* file = 0, int line = 0, const char* func = 0)
		{
			return MallocBinnedMgr::Malloc(count, Alignment);
		}

        static inline void* reallocBytes( void* Ptr, size_t NewSize, const char* file = 0, int line = 0, const char* func = 0)
        {
            return MallocBinnedMgr::Realloc(Ptr, NewSize, Alignment);
        }


		static inline void deallocateBytes(void* ptr)
		{
			MallocBinnedMgr::Free(ptr);
		}
       
        /// Get the maximum size of a single allocation
        static inline size_t getMaxAllocationSize()
        {
            return (std::numeric_limits<size_t>::max)();
        }
    private:
        // no instantiation allowed
        BinnedAlignedAllocPolicy()
        { }
    };

    class LORD_CORE_API BinnedAllocPolicyNoMemTrace
    {
    public:
        static inline void* allocateBytes(size_t count,
            const char* file = 0, int line = 0, const char* func = 0)
        {
            return MallocBinnedMgr::Malloc(count);
        }
        static inline void* reallocBytes( void* Ptr, size_t NewSize, const char* file = 0, int line = 0, const char* func = 0)
        {
            return MallocBinnedMgr::Realloc(Ptr, NewSize);
        }
        static inline void deallocateBytes(void* ptr)
        {
            MallocBinnedMgr::Free(ptr);
        }
        /// Get the maximum size of a single allocation
        static inline size_t getMaxAllocationSize()
        {
            return (std::numeric_limits<size_t>::max)();
        }
    private:
        // No instantiation
        BinnedAllocPolicyNoMemTrace()
        { }
    };

    template <size_t Alignment = 0>
    class BinnedAlignedAllocPolicyNoMemTrace
    {
    public:
        // compile-time assert alignment is available.
        typedef int IsValidAlignment
            [Alignment <= 128 && ((Alignment & (Alignment-1)) == 0) ? +1 : -1];

        static inline void* allocateBytes(size_t count,
            const char* file = 0, int line = 0, const char* func = 0)
        {
            return MallocBinnedMgr::Malloc(count, Alignment);
        }
        static inline void* reallocBytes( void* Ptr, size_t NewSize, const char* file = 0, int line = 0, const char* func = 0)
        {
            return MallocBinnedMgr::Realloc(Ptr, NewSize);
        }
        static inline void deallocateBytes(void* ptr)
        {
            MallocBinnedMgr::Free(ptr);
        }

        /// Get the maximum size of a single allocation
        static inline size_t getMaxAllocationSize()
        {
            return (std::numeric_limits<size_t>::max)();
        }
    private:
        // no instantiation allowed
        BinnedAlignedAllocPolicyNoMemTrace()
        { }
    };

    
}//namespace Lord
//

//#define LORD_REPLACE_OPERATOR_NEW_DEL \
//void* __cdecl operator new(size_t uSize) \
//{ \
//    return ::LORD::MallocBinnedMgr::Malloc(uSize); \
//} \
//void* __cdecl operator new[](size_t uSize) \
//{ \
//	return ::LORD::MallocBinnedMgr::Malloc(uSize);\
//} \
//void __cdecl operator delete(void* ptr) \
//{ \
//    ::LORD::MallocBinnedMgr::Free(ptr); \
//} \
//void __cdecl operator delete[](void* ptr) \
//{ \
//	::LORD::MallocBinnedMgr::Free(ptr); \
//}



#endif

#endif
