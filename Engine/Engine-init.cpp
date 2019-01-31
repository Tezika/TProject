#include "stdafx.h"
#include "Engine-init.h"
#include "ConsolePrint.h"
#include "Assert.h"
#include "HeapManager.h"
#include <Windows.h>
#include "RenderManager.h"

namespace Engine
{
	bool Engine::Initialize()
	{

#ifdef USE_CUSTOM_ALLOCATOR
		const size_t 		sizeHeap = 1024 * 1024;
		const unsigned int 	numDescriptors = 2048;
		// Allocate memory for my custom heap.
		void * pHeapMemory = HeapAlloc( GetProcessHeap(), 0, sizeHeap );
		assert( pHeapMemory );

		// Create a heap manager for my custom heap.
		HeapManager * pHeapManager = HeapManager::Create( pHeapMemory, sizeHeap, numDescriptors );
		assert( pHeapManager );

		if ( pHeapManager == nullptr )
			return false;

		HeapManager::s_pDefalutHeapManager = pHeapManager;
#endif

		// Initialize the sub systems
		// For Rendering
		bool bSuccess = false;
		bSuccess = Render::RenderManager::GetInstance().Initialize();
		assert( bSuccess );

		DEBUG_PRINT( "The engine initialized succuessfully!" );
		return true;
	}

	void Engine::Destroy()
	{
#ifdef USE_CUSTOM_ALLOCATOR
		// Free customize heap and destroy the heap
		assert( HeapManager::s_pDefalutHeapManager );
		HeapManager::s_pDefalutHeapManager->Destroy();
		HeapFree( GetProcessHeap(), 0, HeapManager::s_pDefalutHeapManager->GetAssociateMemory() );
		HeapManager::s_pDefalutHeapManager = nullptr;
#endif // USE_CUSTOM_ALLOCATOR


		// Destoy the sub systems
		// For Rendering
		bool bSuccess = false;
		bSuccess = Render::RenderManager::GetInstance().Destroy();
		DEBUG_PRINT( "The engine destroied successfully!" );
	}
}
