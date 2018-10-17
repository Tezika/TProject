#pragma once
#ifndef __HEAP_MANAGER_PROXY_H
#define __HEAP_MANAGER_PROXY_H

#include <stdint.h>
#include "HeapManager.h"

namespace HeapManagerProxy
{
	using namespace Engine;

	HeapManager *	CreateHeapManager(void * i_pMemory, size_t i_sizeMemory, unsigned int i_numDescriptors);
	void			Destroy(HeapManager * i_pManager);

	void *			alloc(HeapManager * i_pManager, size_t i_size);
	void *			alloc(HeapManager * i_pManager, size_t i_size, unsigned int i_alignment);
	bool			free(HeapManager * i_pManager, void * i_ptr);

	bool			Contains(const HeapManager * i_pManager, void * i_ptr);

	void			Collect(HeapManager * i_pManager);

	size_t			GetLargestFreeBlock(const HeapManager * i_pManager);
	size_t			GetTotalFreeMemory(const HeapManager * i_pManager);

	bool			IsAllocated(const HeapManager * i_pManager, void * i_ptr);

	void			ShowFreeBlocks(const HeapManager * i_pManager);
	void			ShowOutstandingAllocations(const HeapManager * i_pManager);

} // namespace HeapManagerProxy

#endif // __HEAP_MANAGER_PROXY_H