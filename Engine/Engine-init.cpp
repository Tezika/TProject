#include "stdafx.h"
#include "Engine-init.h"
#include "ConsolePrint.h"
#include "Assert.h"
#include "HeapManager.h"
#include "SubSystems.h"
#include "MemorySystem.h"
#include "MessageSystem.h"

namespace Engine
{
	bool Engine::Initialize( HINSTANCE i_hInstance, int i_nCmdShow )
	{

#ifdef USE_CUSTOM_MEMORYMANAGEMENT
		// Initial the memory system
		// For 100M memory
		const size_t 		sizeHeap = 100 * 1024 * 1024;
		const unsigned int 	numDescriptors = 2048;
		// Allocate memory for my test heap.
		void * pHeapMemory = HeapAlloc( GetProcessHeap(), 0, sizeHeap );
		assert( pHeapMemory );

		// Create your HeapManager and FixedSizeAllocators.
		bool successd = InitializeMemorySystem( pHeapMemory, sizeHeap, numDescriptors );
		assert( successd );
#endif

		// Initialize the sub systems
		bool bSuccess = false;

		// For Physics
		bSuccess = Physics::PhysicsManager::GetInstance().Initialize();
		assert( bSuccess );

		// For Rendering
		bSuccess = Render::RenderManager::GetInstance().Initialize( i_hInstance, i_nCmdShow );
		assert( bSuccess );

		// For Controller Manager
		bSuccess = Controller::ControllerManager::GetInstance().Initialize();
		assert( bSuccess );

		// For Message System
		bSuccess = Messaging::MessageSystem::GetInstance().Initialize();
		assert( bSuccess );

		return true;
	}

	void Engine::Destroy()
	{
		// Destroy the sub systems firstly
		// For Physics
		bool bSuccess = false;
		bSuccess = Physics::PhysicsManager::GetInstance().Destroy();
		assert( bSuccess );

		// For Controller Manager
		bSuccess = Controller::ControllerManager::GetInstance().Destroy();
		assert( bSuccess );

		// For Rendering
		bSuccess = Render::RenderManager::GetInstance().Destroy();
		assert( bSuccess );

		// For Messaging
		bSuccess = Messaging::MessageSystem::GetInstance().Destroy();
		assert( bSuccess );


#ifdef USE_CUSTOM_MEMORYMANAGEMENT
		// Destroy the memory management system
		DestroyMemorySystem();
#endif // USE_FIXED_ALLOCATORS
	}
}
