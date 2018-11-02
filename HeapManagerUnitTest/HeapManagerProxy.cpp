#include "stdafx.h"
#include "HeapManagerProxy.h"

#include <assert.h>
#include <stdio.h>

#include "HeapManager.h"

#define  __TRACK_ALLOCATIONS

namespace HeapManagerProxy
{

	HeapManager * CreateHeapManager(void * i_pMemory, size_t i_sizeMemory, unsigned int i_numDescriptors)
	{
		return HeapManager::Create(i_pMemory, i_sizeMemory, i_numDescriptors);
	}

	void Destroy(HeapManager * i_pManager)
	{
		assert(i_pManager);
		i_pManager->Destroy();
	}

	void * alloc(HeapManager * i_pManager, size_t i_size)
	{
		assert(i_pManager);
		return i_pManager->Alloc(i_size);
	}

	void * alloc(HeapManager * i_pManager, size_t i_size, unsigned int i_alignment)
	{
		assert(i_pManager);

		return i_pManager->Alloc(i_size, i_alignment);
	}

	bool free(HeapManager * i_pManager, void * i_ptr)
	{
		assert(i_pManager);

		return i_pManager->Free(i_ptr);
	}

	bool Contains(const HeapManager * i_pManager, void * i_ptr)
	{
		assert(i_pManager);

		return i_pManager->Contains(i_ptr);
	}


	void Collect(HeapManager * i_pManager)
	{
		assert(i_pManager);

		i_pManager->Collect();
	}

	bool IsAllocated(const HeapManager * i_pManager, void * i_ptr)
	{
		assert(i_pManager);

		return i_pManager->IsAllocated(i_ptr);
	}

	size_t GetLargestFreeBlock(const HeapManager * i_pManager)
	{
		assert(i_pManager);

		return i_pManager->GetMaxiumAllocatableMemory();
	}

	size_t GetTotalFreeMemory(const HeapManager * i_pManager)
	{
		assert(i_pManager);

		return i_pManager->GetLeftMemory();
	}

	void ShowFreeBlocks(const HeapManager * i_pManager)
	{
		assert(i_pManager);

		i_pManager->ShowFreeBlocks();
	}

	void ShowOutstandingAllocations(const HeapManager * i_pManager)
	{
		assert(i_pManager);

#ifdef __TRACK_ALLOCATIONS
		i_pManager->ShowOutstandingAllocations();
#else
		printf("HeapManager compiled without __TRACK_ALLOCATIONS defined.\n");
#endif

	}


} // namespace HeapManagerProxy