#include "game.h"
#include  "Engine-init.h"

using namespace MonsterChase;

int main()
{ 
	Engine::Initialize();
	// insert code here...
	Game::GetInstance().Initialize();
	while (!Game::GetInstance().ShouldEnd())
	{
		Game::GetInstance().Run();
	}
	Game::GetInstance().Stop();
	_CrtDumpMemoryLeaks();
	system("pause");
}