#include "stdafx.h"
#include "assert.h"
#include "HeapManager.h"
#include "stdio.h"
#include "string.h"

Engine::HeapManager::HeapManager()
{
}

Engine::HeapManager::HeapManager(void* pMemory, size_t i_sizeMemory, unsigned int i_numDescription)
{
	pMemory_ = pMemory;
	i_sizeOfMemory_ = i_sizeMemory;
	i_numOfDescription_ = i_numDescription;
	i_usedMemory_ = 0;
	this->Initialize();
}

Engine::HeapManager::~HeapManager()
{
}

Engine::HeapManager* Engine::HeapManager::Create(void* i_pMemory, size_t i_sizeMemory, unsigned int i_numDescription)
{
	assert(i_pMemory);
	return new HeapManager(i_pMemory, i_sizeMemory, i_numDescription);
}

void Engine::HeapManager::Destroy()
{
	//clear three list data
	DEBUG_PRINT("The heapmanager destroy successfully.");
}

void* Engine::HeapManager::Alloc(size_t i_size)
{
	//Create a descriptor firstly
	DEBUG_PRINT("The size of descriptor is %d", sizeof(BlockDescriptor));
	BlockDescriptor* pDescriptor = static_cast<BlockDescriptor*>(pMemory_);
	pMemory_ = static_cast<char*>(pMemory_) + sizeof(BlockDescriptor);
	pDescriptor->m_pBlockSAtartAddr = pMemory_;
	pMemory_ = static_cast<char*>(pMemory_) + i_size;
	pDescriptor->m_sizeBlock = i_size;
	if (pDescriptorHead_ == nullptr)
	{
		pDescriptor->next = nullptr;
		pDescriptorHead_ = pDescriptor;
	}
	else
	{
		//Insert the node after the head;
		auto temp = pDescriptorHead_->next;
		pDescriptorHead_->next = pDescriptor;
		pDescriptor->next = temp;
	}
	return pDescriptor->m_pBlockSAtartAddr;
}

void* Engine::HeapManager::Alloc(size_t i_size, unsigned int i_alignment)
{
	return nullptr;
}

bool Engine::HeapManager::Free(void* i_ptr)
{
	assert(i_ptr);
	return true;
}

void Engine::HeapManager::Initialize()
{
	pDescriptorHead_ = nullptr;
	//update the available using memory for allocator
	i_sizeOfMemory_ = i_sizeOfMemory_ - sizeof(BlockDescriptor) * i_numOfDescription_;
	DEBUG_PRINT("The heapmanager setup successfully.");
}
