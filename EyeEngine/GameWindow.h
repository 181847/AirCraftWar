#pragma once
#include <DirectX12/Common/GameTimer.h>
#include "RenderSystem.h"

// Link necessary d3d12 libraries.
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Indicates to hybrid graphics systems to prefer the discrete part by default
extern "C"
{
	__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

namespace EyeEngine
{


class GameWindow
{
public:
	GameWindow(HINSTANCE hInstance);
	virtual ~GameWindow();
protected:

	GameWindow(const GameWindow& rhs) = delete;
	GameWindow& operator=(const GameWindow& rhs) = delete;
	

public:

	static GameWindow*	GetWindow();

	HINSTANCE			AppInst()const;
	HWND				MainWnd()const;

	int Run();

	virtual bool Initialize();
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:

	static GameWindow* _window;

	HINSTANCE _hAppInst = nullptr; // application instance handle
	HWND      _hMainWnd = nullptr; // main window handle
	
};

}// namespace EyeEngine
