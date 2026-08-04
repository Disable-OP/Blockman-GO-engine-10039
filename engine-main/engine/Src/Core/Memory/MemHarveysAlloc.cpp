/* 
---------------------------------------------------------------------
**功能描述: 迁移Harveys的内存分配器
**文件说明: 
**作	者: 韦建伟
**创建时间: 2013-09-24
**备    注: 提供四个固定接口
---------------------------------------------------------------------
*/ 
#include "Core.h"
#include "MemTracker.h"
#include "MemHarveysAlloc.h"
#include "Resource/Threading.h"

#include <iostream>

#if STATS
#include <psapi.h>
#pragma comment(lib, "psapi.lib")
#endif

#ifdef LORD_MEMORY_ALLOCATOR_HARVEYS
#define ALIGNSIZE 65536

namespace LORD
{
	void* my_malloc(int size , int align)
	{
		if(size <= 0 || align <= 0)
			return NULL;

		int real_size = size + align;
		real_size += sizeof(unsigned int);

		void *ptr = malloc(real_size);
		if(NULL == ptr)
			return NULL;
		void *old = ptr;
		unsigned long value = (unsigned long)ptr;
		if(!(value % align))
		{
			value += align; 
		}
		else 
		{
			value = (value / align + 1) * align;
		}
		ptr = (void *)value;
		*(void **)((char *)ptr - sizeof(void *)) = old;
		return ptr;
	}

	void my_free(void *ptr)
	{
		void *old = *(void **)((char *)ptr - sizeof(void *));
		free(old);
	}

	HarveysMalloc::HarveysMalloc()
		:	mMemInit( 0 )
		,	mPackSize(4096)
#if STATS
		,	OsCurrent(0)
		,	OsPeak(0)
		,	WasteCurrent(0)
		,	WastePeak(0)
		,	UsedCurrent(0)
		,	UsedPeak(0)
		,	CurrentAllocs(0)
		,	TotalAllocs(0)
		,	MemTime(0.0)
		,	RealAllocs(0)
		,	RealFrees(0)
#endif
	{
		_init();
	}
	//-----------------------------------------------------------------------
	HarveysMalloc::~HarveysMalloc()
	{
		_exit();
	}

	void* HarveysMalloc::Malloc(size_t uSize)
	{
		MutexLock NameLock(*auto_mutex);

		LordAssert(uSize>0);
		LordAssert(mMemInit);
#if STATS
		MEM_TIME(MemTime -= (float)Root::Instance()->getCurrentTime());
		CurrentAllocs++;
		TotalAllocs++;
#endif
		_FREE_MEM* Free;
		
		if( uSize < POOL_MAX )
		{
			// Allocate from pool.
			_POOL_TABLE* pPoolTable = mMemSizeToPoolTable[uSize];
			LordAssert(uSize <= pPoolTable->uBlockSize);

#if STATS
			// keep track of memory lost to padding
			pPoolTable->TotalWaste += pPoolTable->uBlockSize - uSize;
			pPoolTable->TotalRequests++;
			pPoolTable->ActiveRequests++;
			pPoolTable->MaxActiveRequests = std::max<unsigned long>(pPoolTable->MaxActiveRequests, pPoolTable->ActiveRequests);
			pPoolTable->MaxRequest = uSize > pPoolTable->MaxRequest ? uSize : pPoolTable->MaxRequest;
			pPoolTable->MinRequest = uSize < pPoolTable->MinRequest ? uSize : pPoolTable->MinRequest;
#endif
			_POOL_INFO_BASE* Pool = pPoolTable->pFirstPool;
			if( !Pool )
			{
				// Must create a new pool.
				unsigned long uBlocks  = 65536 / pPoolTable->uBlockSize;
				unsigned long uBytes   = uBlocks * pPoolTable->uBlockSize;
				LordAssert(uBlocks>=1);
				LordAssert(uBlocks*pPoolTable->uBlockSize<=uBytes);

				// Allocate memory.
				Free = (_FREE_MEM*)BinnedAllocFromOS();//VirtualAlloc( NULL, uBytes, MEM_COMMIT, PAGE_READWRITE );
				LordAssert(Free);

				// Create pool in the indirect table.
				_POOL_INFO_BASE*& Indirect = mPoolIndirect[((unsigned long)Free>>INDIRECT_TABLE_SHIFT)]; //取地址低5位作为表(Table)索引,总共32个临时池表 : 2^5=32 
				if( !Indirect )
					Indirect = _createIndirect();
				Pool = &Indirect[((unsigned long)Free>>POOL_BIT_SHIFT) & POOL_MASK]; //取地址低11位做池(Pool)索引，高16位作为64K池容量地址

				// _init pool.
				Pool->Link( pPoolTable->pFirstPool );
				Pool->pMem            = (char*)Free;
				Pool->uBytes		  = uBytes;
				Pool->uOsBytes		  = Align(uBytes,mPackSize);
				Pool->pPoolTable		= pPoolTable;
				Pool->uTaken			= 0;
				Pool->pFirstFreeMem		= Free;
#if STATS
				pPoolTable->NumActivePools++;
				pPoolTable->MaxActivePools = std::max<unsigned long>(pPoolTable->MaxActivePools, pPoolTable->NumActivePools);
				OsPeak = std::max<unsigned long>(OsPeak, OsCurrent += Pool->uOsBytes);
				WastePeak = std::max<unsigned long>(WastePeak, WasteCurrent += Pool->uOsBytes - Pool->uBytes);
#endif
				// Create first free item.
				Free->uBlocks         = uBlocks;	//未分配的块数
				Free->pNext           = NULL;
			}

			// Pick first available block and unlink it.
			Pool->uTaken++;			//增加分配计数
			LordAssert(Pool->pFirstFreeMem);
			LordAssert(Pool->pFirstFreeMem->uBlocks>0);
			Free = (_FREE_MEM*)((char*)Pool->pFirstFreeMem + --Pool->pFirstFreeMem->uBlocks * pPoolTable->uBlockSize);
			if( Pool->pFirstFreeMem->uBlocks==0 ) //当块分配完后指向下一个池
			{
				Pool->pFirstFreeMem = Pool->pFirstFreeMem->pNext;
				if( !Pool->pFirstFreeMem )
				{
					// Move to exhausted list.
					Pool->Unlink();
					Pool->Link( pPoolTable->pExaustedPool );
				}
			}
#if STATS
			UsedPeak = std::max<unsigned long>(UsedPeak, UsedCurrent += pPoolTable->uBlockSize);
#endif
		}
		else //对于>32K的内存分配交给系统管理
		{
			// Use OS for large allocations.
			//int AlignedSize = Align(uSize,mPackSize);
			//Free = (_FREE_MEM*)my_malloc(AlignedSize, ALIGNSIZE);//(_FREE_MEM*)VirtualAlloc( NULL, AlignedSize, MEM_COMMIT, PAGE_READWRITE );
			//LordAssert(Free);
			//LordAssert(!((size_t)Free&65535));

			//// Create indirect.
			//_POOL_INFO_BASE*& Indirect = mPoolIndirect[((unsigned long)Free>>INDIRECT_TABLE_SHIFT)];
			//if( !Indirect )
			//	Indirect = _createIndirect();

			//// _init pool.
			//_POOL_INFO_BASE* Pool = &Indirect[((unsigned long)Free>>POOL_BIT_SHIFT) & POOL_MASK];
			//Pool->pMem		= (char*)Free;
			//Pool->uBytes	= uSize;
			//Pool->uOsBytes	= AlignedSize;
			//Pool->pPoolTable		= &mOsTable;

			size_t AlignedSize = Align(uSize,mPackSize);
			Free = (_FREE_MEM*)malloc(AlignedSize);
#if STATS
			OsPeak = std::max<unsigned long>(OsPeak, OsCurrent += AlignedSize);
			UsedPeak = std::max<unsigned long>(UsedPeak, UsedCurrent += uSize);
			WastePeak = std::max<unsigned long>(WastePeak, WasteCurrent += AlignedSize - uSize);
#endif
		}
#if STATS
		MEM_TIME(MemTime += (float)Root::Instance()->getCurrentTime());
#endif
		return Free;
	}
	//-----------------------------------------------------------------------
	void HarveysMalloc::Free(void* pOriginal) 
	{
		MutexLock NameLock(*auto_mutex);

		if( !pOriginal )
			return;
		LordAssert(mMemInit);
#if STATS
		MEM_TIME(MemTime -= (float)Root::Instance()->getCurrentTime());
		CurrentAllocs--;
#endif
		if((unsigned long)pOriginal<(unsigned long)mMemoryPool || (unsigned long)pOriginal>=(unsigned long)mMemoryPool+ALLOC_MAX)
		{
#if STATS
			UsedCurrent -= Pool->uBytes;
			OsCurrent -= Pool->uOsBytes;
			WasteCurrent -= Pool->uOsBytes - Pool->uBytes;
#endif
			free(pOriginal);
		}
		else
		{
			// Pool.
			_POOL_INFO_BASE* Pool = &mPoolIndirect[(unsigned long)pOriginal>>INDIRECT_TABLE_SHIFT][((unsigned long)pOriginal>>POOL_BIT_SHIFT)&POOL_MASK];
			LordAssert(Pool->uBytes!=0);
			if( Pool->pPoolTable!=&mOsTable )
			{
#if STATS
				Pool->pPoolTable->ActiveRequests--;
#endif
				// If this pool was exhausted, move to available list.
				if( !Pool->pFirstFreeMem )
				{
					Pool->Unlink();
					Pool->Link( Pool->pPoolTable->pFirstPool );
				}

				// Free a pooled allocation.
				_FREE_MEM* Free		= (_FREE_MEM *)pOriginal;
				Free->uBlocks		= 1;
				Free->pNext			= Pool->pFirstFreeMem;
				Pool->pFirstFreeMem		= Free;
#if STATS
				UsedCurrent -= Pool->pPoolTable->uBlockSize;
#endif

				//Free this pool.
				LordAssert(Pool->uTaken>=1);
				if( --Pool->uTaken == 0 )
				{
#if STATS
					Pool->pPoolTable->NumActivePools--;
#endif
					// Free the OS memory.
					Pool->Unlink();
					BinnedFreeToOS(Pool->pMem);
					//if( !BinnedFreeToOS(Pool->pMem)/*VirtualFree( Pool->pMem, 0, MEM_RELEASE )!=0*/ )
					//	LordAssert(false);

#if STATS
					OsCurrent -= Pool->uOsBytes;
					WasteCurrent -= Pool->uOsBytes - Pool->uBytes;
#endif
				}
			}
			else
			{
				// Free an OS allocation.
				LordAssert(!((unsigned long)pOriginal&65535));
#if STATS
				UsedCurrent -= Pool->uBytes;
				OsCurrent -= Pool->uOsBytes;
				WasteCurrent -= Pool->uOsBytes - Pool->uBytes;
#endif
				my_free(pOriginal);
				//if(!VirtualFree( pOriginal, 0, MEM_RELEASE )!=0 )
				//	LordAssert(false);
			}
		}
#if STATS
		MEM_TIME(MemTime += Root::Instance()->getCurrentTime());
#endif
	}
	//-----------------------------------------------------------------------
	void HarveysMalloc::_init()
	{
		LordAssert(!mMemInit);
		mMemInit = 1;

		auto_mutex = new Mutex();
		/*SYSTEM_INFO SI;
		GetSystemInfo( &SI );
		mPackSize = SI.dwPageSize;
		LordAssert(!(mPackSize&(mPackSize-1)));*/

#if STATS
		memset( &mOsTable, 0, sizeof( _POOL_TABLE ) );
		memset( mPoolTable, 0, sizeof( _POOL_TABLE ) * POOL_COUNT );
#endif

		// _init tables.
		mOsTable.pFirstPool    = NULL;
		mOsTable.pExaustedPool = NULL;
		mOsTable.uBlockSize    = 0;

		mPoolTable[0].pFirstPool    = NULL;
		mPoolTable[0].pExaustedPool = NULL;
		mPoolTable[0].uBlockSize    = 8;

		for(unsigned long i=1; i<5; i++ )
		{
			mPoolTable[i].pFirstPool    = NULL;
			mPoolTable[i].pExaustedPool = NULL;
			mPoolTable[i].uBlockSize    = (8<<((i+1)>>2))+(2<<i);//4,8,268,272; //(4+(i&3)) << (1+(i>>2));
#if STATS
			mPoolTable[i].MinRequest = mPoolTable[i].uBlockSize;
#endif
		}

		for(unsigned long i=5; i<POOL_COUNT; i++ )
		{
			mPoolTable[i].pFirstPool    = NULL;
			mPoolTable[i].pExaustedPool = NULL;
			mPoolTable[i].uBlockSize    = (4+((i+7)&3))<<(1+((i+7)>>2));//(4+(i&3)) << (1+(i>>2));
#if STATS
			mPoolTable[i].MinRequest = mPoolTable[i].uBlockSize;
#endif
		}
		for(unsigned long i=0; i<POOL_MAX; i++ )
		{
			unsigned long Index;
			for( Index=0; mPoolTable[Index].uBlockSize<i; Index++ );
			LordAssert(Index<POOL_COUNT);
			mMemSizeToPoolTable[i] = &mPoolTable[Index];
		}
		for(unsigned long i=0; i<32/*256*/; i++ )
		{
			mPoolIndirect[i] = NULL;
		}
		LordAssert(POOL_MAX-1==mPoolTable[POOL_COUNT-1].uBlockSize);
		
		// Lord不用内存池
#if USE_MEMPOOL == 1
		//创建链表头尾指针
		m_pHeadMem = new MEMBLOCK;/*VirtualAlloc(NULL, sizeof(MEMBLOCK), MEM_COMMIT, PAGE_READWRITE)*/;
		m_pEndMem = new MEMBLOCK;/*VirtualAlloc(NULL, sizeof(MEMBLOCK), MEM_COMMIT, PAGE_READWRITE)*/;
		m_pHeadMem->pNext = m_pEndMem;
		m_pHeadMem->pPrev = NULL;
		m_pEndMem->pNext = NULL;
		m_pEndMem->pPrev = m_pHeadMem;
		for (int i=0; i<MEMBLOCK_MAX; ++i)
		{
			//后插入新链表节点
			MEMBLOCK* newMem = &mMemBlock[i];
			newMem->pNext = m_pEndMem;
			newMem->pPrev = m_pEndMem->pPrev;
			newMem->index = i;
			m_pEndMem->pPrev->pNext = newMem;
			m_pEndMem->pPrev = newMem;
		}
		
		mMemoryPool = static_cast<char*>(my_malloc(ALLOC_MAX, ALIGNSIZE));/*static_cast<char*>(VirtualAlloc(NULL, ALLOC_MAX, MEM_COMMIT, PAGE_READWRITE))*/;
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	//-----------------------------------------------------------------------
	void HarveysMalloc::_exit()
	{
#if STATS
#if USE_MEMPOOL
		CheckMemory("Leak.log");
#else
		DumpAllocationstoLog("Leak.log");
#endif
#endif
#if USE_MEMPOOL == 1
		my_free(mMemoryPool);
		//VirtualFree(mMemoryPool, 0, MEM_RELEASE );
		mMemoryPool = NULL;
#endif

		LordAssert(mMemInit);
		mMemInit = 0;
	}
	//-----------------------------------------------------------------------
	HarveysMalloc::_POOL_INFO_BASE* HarveysMalloc::_createIndirect()
	{
		_POOL_INFO_BASE* Indirect = static_cast<_POOL_INFO_BASE*>(malloc(2048*sizeof(_POOL_INFO_BASE)));/*(_POOL_INFO_BASE*)VirtualAlloc( NULL, 2048*sizeof(_POOL_INFO_BASE), MEM_COMMIT, PAGE_READWRITE )*/;
		LordAssert(Indirect && "Out of memory!");
#if STATS
		WasteCurrent += 2048*sizeof(_POOL_INFO_BASE);
#endif
		return Indirect;
	}

	void HarveysMalloc::getAllocationInfo( MemoryAllocationStats& MemStats )
	{
#if STATS
		// determine how much memory has been allocated from the OS
		MemStats.TotalUsed = MemStats.TotalAllocated = BinnedGetTotalMemoryAllocatedFromOS();

		MemStats.CPUUsed = UsedCurrent;
		MemStats.CPUWaste = WasteCurrent;
		MemStats.CPUSlack = OsCurrent - WasteCurrent - UsedCurrent;

		double Waste = 0.0;
		for( int PoolIndex = 0; PoolIndex < POOL_COUNT; PoolIndex++ )
		{
			Waste += ( ( double )mPoolTable[PoolIndex].TotalWaste / ( double )mPoolTable[PoolIndex].TotalRequests ) * ( double )mPoolTable[PoolIndex].ActiveRequests;
		}
		MemStats.CPUWaste += ( unsigned long )Waste;
#endif
	}

	bool HarveysMalloc::ValidateHeap()
	{
		for( int i = 0; i < POOL_COUNT; i++ )
		{
			_POOL_TABLE* Table = &mPoolTable[i];
			for( _POOL_INFO_BASE** PoolPtr = &Table->pFirstPool; *PoolPtr; PoolPtr = &(*PoolPtr)->pNext )
			{
				_POOL_INFO_BASE* Pool = *PoolPtr;
				LordAssert(Pool->pPrevLink == PoolPtr);
				LordAssert(Pool->pFirstFreeMem);
				for( _FREE_MEM* Free = Pool->pFirstFreeMem; Free; Free = Free->pNext )
				{
					LordAssert(Free->uBlocks > 0);
				}
			}
			for( _POOL_INFO_BASE** PoolPtr = &Table->pExaustedPool; *PoolPtr; PoolPtr = &(*PoolPtr)->pNext )
			{
				_POOL_INFO_BASE* Pool = *PoolPtr;
				LordAssert(Pool->pPrevLink == PoolPtr);
				LordAssert(!Pool->pFirstFreeMem);
			}
		}

		return true;
	}

#if STATS
	int HarveysMalloc::BinnedGetTotalMemoryAllocatedFromOS()
	{
		// Just get memory information for the process and report the working set instead
		PROCESS_MEMORY_COUNTERS Counters;
		GetProcessMemoryInfo(GetCurrentProcess(), &Counters, sizeof(Counters));

		return Counters.WorkingSetSize;
	}
#endif

#if STATS
	void HarveysMalloc::DumpAllocationstoLog( const std::string& logfile )
	{
		ValidateHeap();
		int tatalFromOs = BinnedGetTotalMemoryAllocatedFromOS();
		std::ofstream os;
		os.open(logfile.c_str());

		os<<"Memory Allocation Status"<<std::endl;
		os<<"Current Memory "<<UsedCurrent/( 1024.0f*1024.0f )<<"MB used, plus "<<( OsCurrent-UsedCurrent )/( 1024.0f*1024.0f )<<"MB waste"<<std::endl;
		os<<"Peak Memory "<< UsedPeak/( 1024.0f*1024.0f )<<"MB used, plus "<<( OsPeak-UsedPeak )/( 1024.0f*1024.0f )<<"MB waste"<<std::endl;
		os<<"Allocs "<<CurrentAllocs<< " Current/"<<TotalAllocs<<" Total"<<std::endl;
		os<<"Seconds "<<MemTime<<"ms"<<std::endl;
		os<<" "<<std::endl;
		os<<"GlobalMemoryStatus"<<std::endl;
		os<<"AllocatedFromOS "<<tatalFromOs/( 1024.0f*1024.0f )<<"MB"<<std::endl;
		os<<" "<<std::endl;
		os<<"BlockSize NumPools MaxPools CurAllocs TotalAllocs MinReq MaxReq MemUsed MemAlign Efficiency"<<std::endl;
		os<<"--------- -------- -------- --------- ----------- ------ ------ ------- -------- ----------"<<std::endl;

		unsigned long TotalMemory = 0;
		unsigned long TotalActiveRequests = 0;
		unsigned long TotalPools = 0;
		unsigned __int64 TotalWaste = 0;
		unsigned __int64 TotalTotalRequests = 0;

		for( int i=0; i<POOL_COUNT; ++i)
		{
			_POOL_TABLE* Table = mPoolTable + i;
			unsigned long MemUsed = ( Table->uBlockSize * Table->ActiveRequests ) / 1024;
			unsigned long MemWaste = (unsigned long)( ((double)Table->TotalWaste/(double)Table->TotalRequests)*(double)Table->ActiveRequests ) / 1024;

			os<<Table->uBlockSize<<std::ends<<
				Table->NumActivePools<<std::ends<<
				Table->MaxActivePools<<std::ends<<
				Table->ActiveRequests<<std::ends<<
				( unsigned long )Table->TotalRequests<<std::ends<<
				Table->MinRequest<<std::ends<<
				Table->MaxRequest<<std::ends<<
				MemUsed - MemWaste<<std::ends<<
				MemWaste<<std::ends<<
				(MemUsed ? 100.0f*( MemUsed-MemWaste)/MemUsed : 100.0f) << std::endl;

			TotalMemory += MemUsed - MemWaste;
			TotalWaste += MemWaste;
			TotalActiveRequests += Table->ActiveRequests;
			TotalTotalRequests += Table->TotalRequests;
			TotalPools += Table->NumActivePools;
		}
		os<<" "<<std::endl;
		os<<TotalMemory<<"K allocated (with "<<TotalWaste<<"K waste). Efficiency "<<(TotalMemory ? 100.0f*(TotalMemory-TotalWaste)/TotalMemory : 100.0f)<<"%"<<std::endl;
		os<<"Allocations "<<TotalActiveRequests<<" Current / "<<TotalTotalRequests<<" Total (in "<<TotalPools<<" pools)"<<std::endl;

		os.close();
	}
#endif

	void* HarveysMalloc::BinnedAllocFromOS()
	{
#if STATS
		RealAllocs++;
#endif

#if USE_MEMPOOL == 1
		if (m_pHeadMem->pNext == m_pEndMem)
		{
			LordLogFatal("Memory is full!");

			LordException("Memory is full!");
			//return my_malloc(65536, ALIGNSIZE);//VirtualAlloc(NULL, 65536, MEM_COMMIT, PAGE_READWRITE);
		}
		unsigned long deltaAdress = m_pHeadMem->pNext->index<<POOL_BIT_SHIFT;
		m_pHeadMem->pNext->pNext->pPrev = m_pHeadMem;
		m_pHeadMem->pNext = m_pHeadMem->pNext->pNext;
		return (void*)(mMemoryPool + deltaAdress);
#else
		return my_malloc(65536, ALIGNSIZE);//VirtualAlloc(NULL, 65536, MEM_COMMIT, PAGE_READWRITE);
#endif
	}

	void HarveysMalloc::BinnedFreeToOS( void* Ptr )
	{
#if STATS
		RealFrees++;
#endif

#if USE_MEMPOOL == 1
		int index = ((unsigned long)Ptr-(unsigned long)mMemoryPool);
	
		if(index<0 || index>=ALLOC_MAX)
		{
			LordException("MemoryAdress is wrong!");
			//my_free(Ptr);//VirtualFree(Ptr, 0, MEM_RELEASE) != 0;
		}
		index = index>>POOL_BIT_SHIFT;

		MEMBLOCK* curMem = &mMemBlock[index];
		curMem->pNext = m_pEndMem;
		curMem->pPrev = m_pEndMem->pPrev;
		curMem->index = index;
		m_pEndMem->pPrev->pNext = curMem;
		m_pEndMem->pPrev = curMem;
#else
		my_free(Ptr);//VirtualFree(Ptr, 0, MEM_RELEASE) != 0;
#endif
	}
#if STATS
	void HarveysMalloc::CheckMemory(const std::string& logfile)
	{
#if USE_MEMPOOL == 1
		unsigned long TotalMemory = 0;
		unsigned long TotalActiveRequests = 0;
		unsigned long TotalPools = 0;
		unsigned __int64 TotalWaste = 0;
		unsigned __int64 TotalTotalRequests = 0;
		std::ofstream os;
		os.open(logfile.c_str());
		os<<"BlockSize NumPools MaxPools CurAllocs TotalAllocs MinReq MaxReq MemUsed MemAlign Efficiency"<<std::endl;
		os<<"--------- -------- -------- --------- ----------- ------ ------ ------- -------- ----------"<<std::endl;

		MEMBLOCK* iterMem = m_pHeadMem;
		while (iterMem->pNext != m_pEndMem)
		{
			char* pOriginal = mMemoryPool + (iterMem->pNext->index<<POOL_BIT_SHIFT);
			_POOL_INFO_BASE* Pool = &mPoolIndirect[(unsigned long)pOriginal>>INDIRECT_TABLE_SHIFT][((unsigned long)pOriginal>>POOL_BIT_SHIFT)&POOL_MASK];
			LordAssert(Pool->uBytes!=0);
			LordAssert(Pool->pPoolTable!=&mOsTable);
			_POOL_TABLE* Table = Pool->pPoolTable;
			unsigned long MemUsed = ( Table->uBlockSize * Table->ActiveRequests ) / 1024;
			unsigned long MemWaste = (unsigned long)( ((double)Table->TotalWaste/(double)Table->TotalRequests)*(double)Table->ActiveRequests ) / 1024;

			os<<Table->uBlockSize<<std::ends<<
				Table->NumActivePools<<std::ends<<
				Table->MaxActivePools<<std::ends<<
				Table->ActiveRequests<<std::ends<<
				( unsigned long )Table->TotalRequests<<std::ends<<
				Table->MinRequest<<std::ends<<
				Table->MaxRequest<<std::ends<<
				MemUsed - MemWaste<<std::ends<<
				MemWaste<<std::ends<<
				(MemUsed ? 100.0f*( MemUsed-MemWaste)/MemUsed : 100.0f) << std::endl;

			TotalMemory += MemUsed - MemWaste;
			TotalWaste += MemWaste;
			TotalActiveRequests += Table->ActiveRequests;
			TotalTotalRequests += Table->TotalRequests;
			TotalPools += Table->NumActivePools;

			iterMem = iterMem->pNext;
		}
		os<<" "<<std::endl;
		os<<TotalMemory<<"K allocated (with "<<TotalWaste<<"K waste). Efficiency "<<(TotalMemory ? 100.0f*(TotalMemory-TotalWaste)/TotalMemory : 100.0f)<<"%"<<std::endl;
		os<<"Allocations "<<TotalActiveRequests<<" Current / "<<TotalTotalRequests<<" Total (in "<<TotalPools<<" pools)"<<std::endl;

		os.close();
#endif
	}
#endif

	static HarveysMalloc* s_HarveysMalloc = NULL;
	//---------------------------------------------------------------------
	void* HarveysAllocImpl::allocBytes( size_t count, const char* file, int line, const char* func )
	{
		if(!s_HarveysMalloc)
		{
			s_HarveysMalloc = new HarveysMalloc();/*static_cast<HarveysMalloc*>(VirtualAlloc(NULL, sizeof(HarveysMalloc), MEM_COMMIT, PAGE_READWRITE));*/
			//new(s_HarveysMalloc) HarveysMalloc;
		}
		void* ptr = s_HarveysMalloc->Malloc(count);
#ifdef LORD_MEMORY_TRACKER
		if (count == 40 && line == 0)
		{
			static int a = 0;
			++a;
		}
		// this alloc policy doesn't do pools (yet, ned can do it)
		MemTracker::get()._recordAlloc(ptr, count, 0, file, line, func);
#endif
		return ptr;
	}
	
	//---------------------------------------------------------------------
	void HarveysAllocImpl::deallocBytes( void* ptr )
	{
		// deal with null
		if (!ptr)
			return;
#ifdef LORD_MEMORY_TRACKER
		MemTracker::get()._recordDealloc(ptr);
#endif
		s_HarveysMalloc->Free(ptr);
	}
	//---------------------------------------------------------------------
	void* HarveysAllocImpl::allocBytesAligned( size_t align, size_t count, const char* file, int line, const char* func )
	{
		// default to platform SIMD alignment if none specified
		unsigned char* result;
		if(align)
		{
			LordAssert(align <= 128 && ((align & (align-1))==0) );
			unsigned char* p = (unsigned char*)s_HarveysMalloc->Malloc(count + align);
			size_t offset = align - (size_t(p) & (align-1));

			result = p + offset;
			result[-1] = (unsigned char)offset;
		}
		else
		{
			result = (unsigned char*)s_HarveysMalloc->Malloc(count);
		}

#ifdef LORD_MEMORY_TRACKER
		MemTracker::get()._recordAlloc(result, count, 0, file, line, func);
#endif
		return result;
	}
	
	//---------------------------------------------------------------------
	void HarveysAllocImpl::deallocBytesAligned( size_t align, void* ptr )
	{
		// deal with null
		if (!ptr)
			return;
#ifdef LORD_MEMORY_TRACKER
		MemTracker::get()._recordDealloc(ptr);
#endif
		if(align)
		{
			unsigned char* mem = (unsigned char*)ptr;
			mem = mem - mem[-1];
			s_HarveysMalloc->Free(mem);
		}
		else
		{
			s_HarveysMalloc->Free(ptr);
		}
	}

#if STATS
	void HarveysAllocImpl::exportlog( const std::string& logfile )
	{
		s_HarveysMalloc->DumpAllocationstoLog(logfile);
	}

	void HarveysAllocImpl::checkmemlog( const std::string& logfile )
	{
		s_HarveysMalloc->CheckMemory(logfile);
	}
#endif
	void* HarveysAllocImplNoMemTrace::allocBytes( size_t count, const char* file, int line, const char* func )
	{
		if(!s_HarveysMalloc)
		{
			s_HarveysMalloc = new HarveysMalloc();/*static_cast<HarveysMalloc*>(VirtualAlloc(NULL, sizeof(HarveysMalloc), MEM_COMMIT, PAGE_READWRITE)); new(s_HarveysMalloc) HarveysMalloc;*/
		}

		return s_HarveysMalloc->Malloc(count);
	}

	void HarveysAllocImplNoMemTrace::deallocBytes( void* ptr )
	{
		if (!ptr)
			return;

		s_HarveysMalloc->Free(ptr);
	}

	void* HarveysAllocImplNoMemTrace::allocBytesAligned( size_t align, size_t count, const char* file, int line, const char* func )
	{
		// default to platform SIMD alignment if none specified
		unsigned char* result;
		if(align)
		{
			LordAssert(align <= 128 && ((align & (align-1))==0) );
			unsigned char* p = (unsigned char*)s_HarveysMalloc->Malloc(count + align);
			size_t offset = align - (size_t(p) & (align-1));

			result = p + offset;
			result[-1] = (unsigned char)offset;
		}
		else
		{
			result = (unsigned char*)s_HarveysMalloc->Malloc(count);
		}

		return result;
	}

	void HarveysAllocImplNoMemTrace::deallocBytesAligned( size_t align, void* ptr )
	{
		// deal with null
		if (!ptr)
			return;

		if(align)
		{
			unsigned char* mem = (unsigned char*)ptr;
			mem = mem - mem[-1];
			s_HarveysMalloc->Free(mem);
		}
		else
		{
			s_HarveysMalloc->Free(ptr);
		}
	}
}

#endif