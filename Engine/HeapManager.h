#pragma once
#include "TList.h"

namespace Engine
{
	struct BlockDescriptor
	{
	public:
		void* m_pBlockStarAddr; // pointer to the block of memory this BlockDescriptor describes
		size_t m_sizeBlock; // size of that block of memory
		BlockDescriptor* next; //the pointer to the next blockdescriptor
	};

	class HeapManager
	{
	public:
		static HeapManager* Create(void*, size_t, unsigned int);
		static size_t s_MinumumToLeave;

		HeapManager();
		HeapManager(void*, size_t, unsigned int);
		~HeapManager();
		void* Alloc(size_t);
		void* Alloc(size_t, unsigned int);
		bool Contains(void*) const;
		void Collect();
		bool Free(void*);
		void Destroy();

		size_t GetUsedMemory() const { return i_usedMemory_; }
		size_t GetLeftMemory() const { return i_sizeOfMemory_ - i_usedMemory_; }

		bool IsAllocated(void * i_ptr) const;
		void ShowFreeBlocks() const;
		void ShowOutstandingAllocations() const;

	private:
		void* pMemory_;
		BlockDescriptor* pFreeHead_;
		BlockDescriptor* pOutstandingHead_;
		size_t i_sizeOfMemory_;
		size_t i_numOfDescription_;

		size_t i_test_;

		//Using for debuging
		size_t i_usedMemory_;

		//Initialized function
		void Initialize();
		
		BlockDescriptor* InsertNodeToHead(BlockDescriptor* head, BlockDescriptor* node);
	};
}
