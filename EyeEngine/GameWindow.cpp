#include "GameWindow.h"
#include <MyTools/MyAssert.h>

namespace EyeEngine
{

GameWindow::GameWindow(HINSTANCE hInstance)
	:_hAppInst(hInstance)
{
	ASSERT(_window == nullptr);
	_window = this;
}

GameWindow::~GameWindow()
{
	if (_d3dDevice != nullptr)
	{
		FlushCommandQueue();
	}
}

GameWindow * EyeEngine::GameWindow::GetApp()
{
	return _window;
}

HINSTANCE GameWindow::AppInst() const
{
	return _hAppInst;
}

HWND GameWindow::MainWnd() const
{
	return _hMainWnd;
}

float GameWindow::AspectRatio() const
{
	return static_cast<float>(_clientWidth) / _clientHeight;
}

bool GameWindow::Get4xMsaaState() const
{
	return _m4xMsaaState;
}

void GameWindow::Set4xMsaaState(bool value)
{
	_m4xMsaaState = value;
}

int GameWindow::Run()
{
	MSG msg = { 0 };
	_timer.Reset();
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// if there is no message, 
			// do our game loop.
			_timer.Tick();

			if ( ! _gamePaused)
			{
				CalculateFrameStats();
				Update(_timer);
				Draw(_timer);
			}
			else
			{
				Sleep(100);
			}
		}
	}
	return 0;
}

bool GameWindow::Initialize()
{
	if ( ! InitMainWindow())
	{
		return false;
	}

	if ( ! InitDirect3D())
	{
		return false;
	}

	// init the swapChain.
	OnResize();

	return true;
}

void GameWindow::CreateRtvAndDsvDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = _swapChainBufferCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	ThrowIfFailed(_d3dDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(_rtvHeap.GetAddressOf())));
}

}// namespace EyeEngine
