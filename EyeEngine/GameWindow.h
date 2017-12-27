#pragma once
#include <DirectX12/Common/d3dUtil.h>
#include <DirectX12/Common/GameTimer.h>
#include "RenderSystem.h"

// Link necessary d3d12 libraries.
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

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

	static GameWindow* GetApp();

	HINSTANCE AppInst()const;
	HWND      MainWnd()const;
	/*float     AspectRatio()const;

	bool Get4xMsaaState()const;
	void Set4xMsaaState(bool value);*/

	int Run();

	virtual bool Initialize();
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:
	/*virtual void CreateRtvAndDsvDescriptorHeaps();
	virtual void OnResize();*/
	//virtual void Update(const GameTimer& gt) = 0;
	//virtual void Draw(const GameTimer& gt) = 0;

protected:

	bool InitMainWindow();
	bool InitDirect3D();

	void SetGameWindowText(const std::wstring& title);

	void CalculateFrameStats();

	void OnMouseDown(WPARAM btnState, int x, int y);

	void OnMouseUp(WPARAM btnState, int x, int y);

protected:

	static GameWindow* _window;

	HINSTANCE _hAppInst = nullptr; // application instance handle
	HWND      _hMainWnd = nullptr; // main window handle
	bool      _gamePaused = false;  // is the game paused?
	bool      _minimized = false;  // is the application minimized?
	bool      _maximized = false;  // is the application maximized?
	bool      _resizing = false;   // are the resize bars being dragged?
	bool      _fullscreenState = false;// fullscreen enabled


									   // Used to keep track of the “delta-time?and game time (?.4).
	GameTimer _timer;
	RenderSystem _renderSys;

	std::wstring _mainWndCaption = L"MainWindow";

	int _clientWidth = 800;
	int _clientHeight = 600;
};

}// namespace EyeEngine
