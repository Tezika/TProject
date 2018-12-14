#pragma once

namespace Engine
{
	class HeapManager;
	class FixedSizeAllocator;

//#define USE_FIXED_ALLOCATOR

#ifdef USE_FIXED_ALLOCATOR
	struct FSAInitData
	{
		size_t	sizeBlock;
		size_t	numBlocks;
	};

	bool InitializeFixedSizeAllocators();
	bool InitializeFSAInitData();
	FixedSizeAllocator * FindFixedSizeAllocator(size_t i_size);

	bool FreeFromFixedSizeAllocators(void * i_ptr);
#endif;

	// InitializeMemorySystem - initialize your memory system including your HeapManager and some FixedSizeAllocators
	bool InitializeMemorySystem(void * i_pHeapMemory, size_t i_sizeHeapMemory, unsigned int i_OptionalNumDescriptors);

	// Collect - coalesce free blocks in attempt to create larger blocks
	void Collect();

	// DestroyMemorySystem - destroy your memory systems
	void DestroyMemorySystem();

	HeapManager * GetDefaultHeap();
}



