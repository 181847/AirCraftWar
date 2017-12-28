#include <EyeEngine\GameWindow.h>
#include <EyeEngine\EyeLogger.h>
#include <iostream>
#include <Windows.h>

#define _CRTDBG_MAP_ALLOC

#include <stdlib.h>

#include <crtdbg.h>

#pragma comment(lib, "EyeEngine.lib")

int main(int argc, char ** argv)
{
	HINSTANCE hInstance;
	hInstance = GetModuleHandle(NULL);

	EyeEngine::EyeLogger::LogInfo(L"Game start, output one: %d ", 1);

#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetBreakAlloc(166);
#endif

	EyeEngine::GameWindow mainWindow(hInstance);
	if (!mainWindow.Initialize())
		return 0;

	mainWindow.Run();

	std::cout << "Game end, press enter to quit.." << std::endl;
	std::getchar();

	return 0;
}