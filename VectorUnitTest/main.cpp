#include "stdafx.h"
#include "Engine-init.h"
#include "conio.h"
#include <crtdbg.h>

extern bool Point2D_UnitTest();

int main()
{
	Engine::Initialize();
	Point2D_UnitTest();
	_CrtDumpMemoryLeaks();
	_getch();
	return 0;
}