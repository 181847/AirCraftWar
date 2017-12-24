#include <EyeEngine\GameWindow.h>
#include <iostream>
#include <Windows.h>

#pragma comment(lib, "EyeEngine.lib")

int main(int argc, char ** argv)
{
	HINSTANCE hInstance;
	hInstance = GetModuleHandle(NULL);
	EyeEngine::GameWindow mainWindow(hInstance);

	if (!mainWindow.Initialize())
	{
		std::cout << "mainWindow initialize failed." << std::endl;
		return 0;
	}
	mainWindow.Run();

	std::cout << "Game end, press enter to quit.." << std::endl;
	std::getchar();
	return 0;
}