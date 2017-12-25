#pragma once
#include <DirectX12/Common/d3dUtil.h>
#include <DirectX12/Common/GameTimer.h>

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
	float     AspectRatio()const;

	bool Get4xMsaaState()const;
	void Set4xMsaaState(bool value);

	int Run();

	virtual bool Initialize();
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:
	virtual void CreateRtvAndDsvDescriptorHeaps();
	virtual void OnResize();
	//virtual void Update(const GameTimer& gt) = 0;
	//virtual void Draw(const GameTimer& gt) = 0;

protected:

	bool InitMainWindow();
	bool InitDirect3D();
	void CreateCommandObjects();
	void CreateSwapChain();

	void FlushCommandQueue();

	void SetGameWindowText(const std::wstring& title);

	ID3D12Resource* CurrentBackBuffer()const;
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView()const;
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView()const;

	void CalculateFrameStats();

	void LogAdapters();
	void LogAdapterOutputs(IDXGIAdapter* adapter);
	void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);

protected:

	static GameWindow* _window;

	HINSTANCE _hAppInst = nullptr; // application instance handle
	HWND      _hMainWnd = nullptr; // main window handle
	bool      _gamePaused = false;  // is the game paused?
	bool      _minimized = false;  // is the application minimized?
	bool      _maximized = false;  // is the application maximized?
	bool      _resizing = false;   // are the resize bars being dragged?
	bool      _fullscreenState = false;// fullscreen enabled

									   // Set true to use 4X MSAA (?.1.8).  The default is false.
	bool      _4xMsaaState = false;    // 4X MSAA enabled
	UINT      _4xMsaaQuality = 0;      // quality level of 4X MSAA

									   // Used to keep track of the “delta-time?and game time (?.4).
	GameTimer _timer;


	int _clientWidth = 800;
	int _clientHeight = 600;
};

}// namespace EyeEngine
