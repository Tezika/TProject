#include "stdafx.h"
#include "Engine-init.h"
#include "conio.h"
#include <crtdbg.h>

extern bool HeapManager_UnitTest();

int main()
{
	Engine::Initialize();
	HeapManager_UnitTest();
	_CrtDumpMemoryLeaks();
	_getch();
    return 0;
}

