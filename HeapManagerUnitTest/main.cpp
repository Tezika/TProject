#include "stdafx.h"
#include "Engine-init.h"
#include "conio.h"

extern bool HeapManager_UnitTest();

int main()
{
	Engine::Initialize();
	HeapManager_UnitTest();
	_getch();
    return 0;
}

