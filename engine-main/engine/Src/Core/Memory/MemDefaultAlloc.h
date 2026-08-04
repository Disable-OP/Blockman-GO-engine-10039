/********************************************************************
filename: 	MemDefaultAlloc.h
file path:	dev\engine\Src\Core\Memory\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __MemoryDefaultAlloc_H__
#define __MemoryDefaultAlloc_H__

#ifdef LORD_MEMORY_ALLOCATOR_DEFAULT
 
namespace LORD
{
	class LORD_CORE_API DefaultImpl
	{
	public:
		static void*	allocBytes(size_t count, const char* file, int line, const char* func);
		static void		deallocBytes(void* ptr);
		static void*	allocBytesAligned(size_t align, size_t count, const char* file, int line, const char* func);
		static void		deallocBytesAligned(size_t align, void* ptr);
	};
    
	class LORD_CORE_API DefaultImplNoMemTrace
	{
	public:
		static void*	allocBytes(size_t count);
		static void		deallocBytes(void* ptr);
		static void*	allocBytesAligned(size_t align, size_t count);
		static void		deallocBytesAligned(size_t align, void* ptr);
	};
    
	class LORD_CORE_API DefaultPolicy
	{
	public:
		static inline void* allocateBytes(size_t count, const char* file = NULL, int line = 0, const char* func = NULL)
		{
			return DefaultImpl::allocBytes(count, file, line, func);
		}
		static inline void deallocateBytes(void* ptr)
		{
			DefaultImpl::deallocBytes(ptr);
		}
		// Get the maximum size of a single allocation
		static inline size_t getMaxAllocationSize()
		{
			return (std::numeric_limits<size_t>::max)();
		}
        
	private:
		// No instantiation
		DefaultPolicy()
		{
		}
	};
    
	class LORD_CORE_API DefaultNoMemTracePolicy
	{
	public:
		static inline void* allocateBytes(size_t count)
		{
			return DefaultImplNoMemTrace::allocBytes(count);
		}
		static inline void deallocateBytes(void* ptr)
		{
			DefaultImplNoMemTrace::deallocBytes(ptr);
		}
		// Get the maximum size of a single allocation
		static inline size_t getMaxAllocationSize()
		{
			return (std::numeric_limits<size_t>::max)();
		}
        
	private:
		// No instantiation
		DefaultNoMemTracePolicy()
		{
		}
	};
    
    template <size_t Alignment = 0>
	class DefaultAlignedPolicy
	{
	public:
		// compile-time check alignment is available.
		typedef int IsValidAlignment[Alignment <= 128 && ((Alignment & (Alignment-1)) == 0) ? +1 : -1];
        
		static inline void* allocateBytes(size_t count, const char* file = NULL, int line = 0, const char* func = NULL)
		{
			return DefaultImpl::allocBytesAligned(Alignment, count, file, line, func);
		}
        
		static inline void deallocateBytes(void* ptr)
		{
			DefaultImpl::deallocBytesAligned(Alignment, ptr);
		}
        
		// Get the maximum size of a single allocation
		static inline size_t getMaxAllocationSize()
		{
			return (std::numeric_limits<size_t>::max)();
		}
	private:
		// no instantiation allowed
		DefaultAlignedPolicy()
		{
		}
	};
    
	template <size_t Alignment = 0>
	class DefaultAlignedNoMemTracePolicy
	{
	public:
		// compile-time check alignment is available.
		typedef int IsValidAlignment[Alignment <= 128 && ((Alignment & (Alignment-1)) == 0) ? +1 : -1];
        
		static inline void* allocateBytes(size_t count)
		{
			return DefaultImplNoMemTrace::allocBytesAligned(Alignment, count);
		}
        
		static inline void deallocateBytes(void* ptr)
		{
			DefaultImplNoMemTrace::deallocBytesAligned(Alignment, ptr);
		}
        
		// Get the maximum size of a single allocation
		static inline size_t getMaxAllocationSize()
		{
			return (std::numeric_limits<size_t>::max)();
		}
	private:
		// no instantiation allowed
		DefaultAlignedNoMemTracePolicy()
		{
		}
	};
}
#endif

#endif
