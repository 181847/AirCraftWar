#include <EyeEngine\GameWindow.h>
#include <EyeEngine\EyeLogger.h>
#include <iostream>
#include <Windows.h>

#pragma comment(lib, "EyeEngine.lib")

int main(int argc, char ** argv)
{
	HINSTANCE hInstance;
	hInstance = GetModuleHandle(NULL);

	EyeEngine::EyeLogger::InitLogger();

	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	try
	{
		EyeEngine::GameWindow mainWindow(hInstance);
		if (!mainWindow.Initialize())
			return 0;

		return mainWindow.Run();
	}
	catch (DxException& e)
	{
		MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
		return 0;
	}

	std::cout << "Game end, press enter to quit.." << std::endl;
	std::getchar();
	return 0;
}