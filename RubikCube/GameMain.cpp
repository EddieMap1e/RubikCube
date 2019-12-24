#include "GameApp.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{

	GameApp theApp(hInstance);

	if (!theApp.Init())
		return 0;

	return theApp.Run();
}