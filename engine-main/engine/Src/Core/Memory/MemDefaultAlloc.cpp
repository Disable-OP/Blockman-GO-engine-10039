#include "Core.h"
#include "MemTracker.h"
#include "MemDefaultAlloc.h"
 
#ifdef LORD_MEMORY_ALLOCATOR_DEFAULT

namespace LORD
{
	//////////////////////////////////////////////////////////////////////////
    
	void* DefaultImpl::allocBytes(size_t count, const char* file, int line, const char* func)
	{
		void* ptr = malloc(count);
#ifdef LORD_MEMORY_TRACKER
		MemTracker::get()._recordAlloc(ptr, count, 0, file, line, func);
#else
		// avoid unused params warning
		file = func = "";
		line = 0;
#endif
		return ptr;
	}
    
	void DefaultImpl::deallocBytes(void* ptr)
	{
		// deal with null
		if (!ptr)
			return;
#ifdef LORD_MEMORY_TRACKER
		MemTracker::get()._recordDealloc(ptr);
#endif
		free(ptr);
	}
    
	void* DefaultImpl::allocBytesAligned(size_t align, size_t count, const char* file, int line, const char* func)
	{
		// default to platform SIMD alignment if none specified
		void *ptr =  malloc(count);
#ifdef LORD_MEMORY_TRACKER
		MemTracker::get()._recordAlloc(ptr, count, 0, file, line, func);
#else
		// avoid unused params warning
		file = func = "";
		line = 0;
#endif
		return ptr;
	}
    
	void DefaultImpl::deallocBytesAligned(size_t align, void* ptr)
	{
		// deal with null
		if (!ptr)
			return;
#ifdef LORD_MEMORY_TRACKER
		MemTracker::get()._recordDealloc(ptr);
#endif
		free(ptr);
	}
    
	//////////////////////////////////////////////////////////////////////////
    
	void* DefaultImplNoMemTrace::allocBytes(size_t count)
	{
		return malloc(count);
	}
    
	void DefaultImplNoMemTrace::deallocBytes(void* ptr)
	{
		// deal with null
		if (!ptr)
			return;
        
		free(ptr);
	}
    
	void* DefaultImplNoMemTrace::allocBytesAligned(size_t align, size_t count)
	{
        return malloc(count);
	}
    
	void DefaultImplNoMemTrace::deallocBytesAligned(size_t align, void* ptr)
	{
		// deal with null
		if (!ptr)
			return;
        
        free(ptr);
	}
}

#endif
