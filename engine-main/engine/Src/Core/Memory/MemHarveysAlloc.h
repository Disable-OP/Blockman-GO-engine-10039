/********************************************************************
filename: 	MemHarveysAlloc.h
file path:	dev\engine\Src\Core\Memory\

version:	1
author:		Harveys
company:	supernano
*********************************************************************/
 
#ifndef __MemoryHarveysAlloc_H__
#define __MemoryHarveysAlloc_H__

#ifdef LORD_MEMORY_ALLOCATOR_HARVEYS

#define MEM_TIME(st)

#define USE_MEMPOOL 1

#define STATS 0

namespace LORD
{
	template< class T > inline T Align( const T Ptr, int Alignment )
	{
		return (T)(((unsigned long)Ptr + Alignment - 1) & ~(Alignment-1));
	}

	class LORD_CORE_API HarveysAllocImpl
	{
	public:
		static void* allocBytes(size_t count, 
			const char* file, int line, const char* func);
		static void deallocBytes(void* ptr);
		static void* allocBytesAligned(size_t align, size_t count, 
			const char* file, int line, const char* func);
		static void deallocBytesAligned(size_t align, void* ptr);

#if STATS
		static void exportlog(const std::string& logfile);
		static void checkmemlog(const std::string& logfile);
#endif
	};

	class LORD_CORE_API HarveysAllocImplNoMemTrace
	{
	public:
		static void* allocBytes(size_t count, 
			const char* file, int line, const char* func);
		static void deallocBytes(void* ptr);
		static void* allocBytesAligned(size_t align, size_t count, 
			const char* file, int line, const char* func);
		static void deallocBytesAligned(size_t align, void* ptr);
	};
	
	class LORD_CORE_API HarveysAllocPolicy
	{
	public:
		static inline void* allocateBytes(size_t count, 
			const char* file = 0, int line = 0, const char* func = 0)
		{
			return HarveysAllocImpl::allocBytes(count, file, line, func);
		}
		static inline void deallocateBytes(void* ptr)
		{
			HarveysAllocImpl::deallocBytes(ptr);
		}
		/// Get the maximum size of a single allocation
		static inline size_t getMaxAllocationSize()
		{
			return (std::numeric_limits<size_t>::max)();
		}
	private:
		// No instantiation
		HarveysAllocPolicy()
		{ }
	};

	template <size_t Alignment = 0>
	class HarveysAlignedAllocPolicy
	{
	public:
		// compile-time check alignment is available.
		typedef int IsValidAlignment
			[Alignment <= 128 && ((Alignment & (Alignment-1)) == 0) ? +1 : -1];

		static inline void* allocateBytes(size_t count, 
			const char* file = 0, int line = 0, const char* func = 0)
		{
			return HarveysAllocImpl::allocBytesAligned(Alignment, count, file, line, func);
		}

		static inline void deallocateBytes(void* ptr)
		{
			HarveysAllocImpl::deallocBytesAligned(Alignment, ptr);
		}

		/// Get the maximum size of a single allocation
		static inline size_t getMaxAllocationSize()
		{
			return (std::numeric_limits<size_t>::max)();
		}
	private:
		// no instantiation allowed
		HarveysAlignedAllocPolicy()
		{ }
	};

	class LORD_CORE_API HarveysAllocPolicyNoMemTrace
	{
	public:
		static inline void* allocateBytes(size_t count, 
			const char* file = 0, int line = 0, const char* func = 0)
		{
			return HarveysAllocImplNoMemTrace::allocBytes(count, file, line, func);
		}
		static inline void deallocateBytes(void* ptr)
		{
			HarveysAllocImplNoMemTrace::deallocBytes(ptr);
		}
		/// Get the maximum size of a single allocation
		static inline size_t getMaxAllocationSize()
		{
			return (std::numeric_limits<size_t>::max)();
		}
	private:
		// No instantiation
		HarveysAllocPolicyNoMemTrace()
		{ }
	};

	template <size_t Alignment = 0>
	class HarveysAlignedAllocPolicyNoMemTrace
	{
	public:
		// compile-time check alignment is available.
		typedef int IsValidAlignment
			[Alignment <= 128 && ((Alignment & (Alignment-1)) == 0) ? +1 : -1];

		static inline void* allocateBytes(size_t count, 
			const char* file = 0, int line = 0, const char* func = 0)
		{
			return HarveysAllocImplNoMemTrace::allocBytesAligned(Alignment, count, file, line, func);
		}

		static inline void deallocateBytes(void* ptr)
		{
			HarveysAllocImplNoMemTrace::deallocBytesAligned(Alignment, ptr);
		}

		/// Get the maximum size of a single allocation
		static inline size_t getMaxAllocationSize()
		{
			return (std::numeric_limits<size_t>::max)();
		}
	private:
		// no instantiation allowed
		HarveysAlignedAllocPolicyNoMemTrace()
		{ }
	};

	//////////////////////////////////////////////////////////////////////////
	class Mutex;
	class MutexLock;
	// 内存分配器
	class HarveysMalloc
	{
	public:
		HarveysMalloc();
		~HarveysMalloc();

		struct MemoryAllocationStats
		{
			unsigned int TotalUsed;
			unsigned int TotalAllocated;
			unsigned long CPUUsed;
			unsigned long CPUWaste;
			unsigned long CPUSlack;
		};

		// 分配内存
		void* Malloc(size_t uSize);
		// 释放内存
		void Free(void* pOriginal);
		// 内存申请状况
		void getAllocationInfo(MemoryAllocationStats& MemStats);
		// Dumps details about all allocations to an output device
#if STATS
		virtual void DumpAllocationstoLog( const std::string& logfile);
		virtual void CheckMemory(const std::string& logfile);
#endif
	protected:
		// 初始始化内存分配器
		void _init();
		// 退出关闭内存分配器
		void _exit();
		// Validates the allocator's heap
		bool ValidateHeap();
		// return the amount of memory allocated from the OS
#if STATS
		inline int BinnedGetTotalMemoryAllocatedFromOS();
#endif

		inline void* BinnedAllocFromOS();

		inline void BinnedFreeToOS(void* Ptr);

	private:
		// Counts.
		enum { POOL_COUNT = 42     };
		/** Maximum allocation for the pooled allocator */
		enum { POOL_MAX   = 32768+1 };//内存池快最大为32K
		enum {
			ALLOC_MAX = 50331648
		};//允许内存池最大为48MB
		enum { MEMBLOCK_MAX = ALLOC_MAX>>16 };//内存区块标记
		/** Size of indirect pools */ 
		enum { INDIRECT_TABLE_BIT_SIZE = 5 };
		enum { INDIRECT_TABLE_SIZE = ( 1 << INDIRECT_TABLE_BIT_SIZE ) }; //0x20
		enum { INDIRECT_TABLE_SHIFT = ( 32 - INDIRECT_TABLE_BIT_SIZE ) }; //27 : 0x1B : 11011

		/** Shift to get the 64k reference from the indirect tables */
		enum { POOL_BIT_SHIFT = 16 };
		/** Used to mask off the bits that have been used to lookup the indirect table */
		enum { POOL_MASK = ( ( 1 << ( INDIRECT_TABLE_SHIFT - POOL_BIT_SHIFT ) ) - 1 ) }; //2047 : 0x7FF : 11111111111


		struct _FREE_MEM;
		struct _POOL_TABLE;

		// 内存池信息.
		struct _POOL_INFO_BASE
		{
			// uBytes allocated for pool.
			unsigned long  		uBytes;			 //分配的池总容量
			// uBytes aligned to page size.
			unsigned long		uOsBytes;		 //占用系统总容量
			// Number of allocated elements in this pool, when counts down to zero can free the entire pool.
			unsigned long      	uTaken;			 //分配计数器
			// Memory base.
			char*        		pMem;		
			// Index of pool.
			_POOL_TABLE* 		pPoolTable;		
			// Pointer to first free memory in this pool.
			_FREE_MEM*   		pFirstFreeMem;  //未分配的块列表
			_POOL_INFO_BASE*	pNext;	//下一个池的地址
			_POOL_INFO_BASE**	pPrevLink;	//上一个池的地址

			void Link( _POOL_INFO_BASE*& Before )
			{
				if( Before )
				{
					Before->pPrevLink = &pNext;
				}
				pNext     = Before;
				pPrevLink = &Before;
				Before   = this;
			}

			void Unlink()
			{
				if( pNext )
				{
					pNext->pPrevLink = pPrevLink;
				}
				*pPrevLink = pNext;
			}
		};

		// 空闲内存块信息
		struct _FREE_MEM
		{
			_FREE_MEM*		pNext;		// pNext or MemLastPool[], always in order by pool.
			unsigned long	uBlocks;	// Number of consecutive free blocks here, at least 1.
		};

		// 内存池表.
		struct _POOL_TABLE
		{
			_POOL_INFO_BASE* pFirstPool;	//首池地址
			_POOL_INFO_BASE* pExaustedPool;	//尾池地址
			unsigned long uBlockSize;				//分块大小 (都是以64K为单位，如果块大小为1字节就会分65536块，32K大小分成2块)

#if STATS
			/** Number of currently active pools */
			unsigned long		NumActivePools;
			/** Largest number of pools simultaneously active */
			unsigned long		MaxActivePools;
			/** Number of requests currently active */
			unsigned long		ActiveRequests;
			/** High watermark of requests simultaneously active */
			unsigned long		MaxActiveRequests;
			/** Minimum request size (in bytes) */
			unsigned long		MinRequest;
			/** Maximum request size (in bytes) */
			unsigned long		MaxRequest;
			/** Total number of requests ever */
			unsigned __int64		TotalRequests;
			/** Total waste from all allocs in this table */
			unsigned __int64		TotalWaste;
#endif
		};

	private:
		// 系统分配内存
		_POOL_INFO_BASE* _createIndirect();

	private:
		struct MEMBLOCK 
		{
			MEMBLOCK* pNext;
			MEMBLOCK* pPrev;
			int index;
		};
		// Variables.
		_POOL_TABLE		mPoolTable[POOL_COUNT];
		_POOL_TABLE		mOsTable;
		_POOL_TABLE*	mMemSizeToPoolTable[POOL_MAX];
		_POOL_INFO_BASE*	mPoolIndirect[INDIRECT_TABLE_SIZE];
		int				mMemInit;
		unsigned long	mPackSize;

#if USE_MEMPOOL == 1
		char*			mMemoryPool;
		MEMBLOCK		mMemBlock[MEMBLOCK_MAX];
		MEMBLOCK*       m_pHeadMem;
		MEMBLOCK*       m_pEndMem;
#endif
#if STATS
		unsigned long		RealAllocs;
		unsigned long		RealFrees;
		unsigned long		OsCurrent;
		unsigned long		OsPeak;
		unsigned long		WasteCurrent;
		unsigned long		WastePeak;
		unsigned long		UsedCurrent;
		unsigned long		UsedPeak;
		unsigned long		CurrentAllocs;
		unsigned long		TotalAllocs;
		double				MemTime;
#endif
		//EE_AUTO_MUTEX
		Mutex* auto_mutex;
	};


	/** @} */
	/** @} */
}// namespace Echo

#endif 

#endif
