#include "GameWindow.h"
#include <WindowsX.h>
#include <MyTools/MyAssert.h>
#include <iostream>
#include "DescriptorHeapHelper.h"

namespace EyeEngine
{

// process function for windows events.
LRESULT CALLBACK 
MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return GameWindow::GetApp()->MsgProc(hwnd, msg, wParam, lParam);
}

GameWindow * GameWindow::_window = nullptr;

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
		//FlushCommandQueue();
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
	return _4xMsaaState;
}

void GameWindow::Set4xMsaaState(bool value)
{
	_4xMsaaState = value;
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
				//CalculateFrameStats();
				//Update(_timer);
				//Draw(_timer);
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

	/*if ( ! InitDirect3D())
	{
		return false;
	}*/

	// init the swapChain.
	OnResize();

	return true;
}

LRESULT GameWindow::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		// WM_ACTIVATE is sent when the window is activated or deactivated.  
		// We pause the game when the window is deactivated and unpause it 
		// when it becomes active.  
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			_gamePaused = true;
			_timer.Stop();
		}
		else
		{
			_gamePaused = false;
			_timer.Start();
		}
		return 0;

		// WM_SIZE is sent when the user resizes the window.  
	case WM_SIZE:
		// Save the new client area dimensions.
		_clientWidth = LOWORD(lParam);
		_clientHeight = HIWORD(lParam);
		if (_d3dDevice)
		{
			if (wParam == SIZE_MINIMIZED)
			{
				_gamePaused = true;
				_minimized = true;
				_maximized = false;
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				_gamePaused = false;
				_minimized = false;
				_maximized = true;
				OnResize();
			}
			else if (wParam == SIZE_RESTORED)
			{

				// Restoring from minimized state?
				if (_minimized)
				{
					_gamePaused = false;
					_minimized = false;
					OnResize();
				}

				// Restoring from maximized state?
				else if (_maximized)
				{
					_gamePaused = false;
					_maximized = false;
					OnResize();
				}
				else if (_resizing)
				{
					// If user is dragging the resize bars, we do not resize 
					// the buffers here because as the user continuously 
					// drags the resize bars, a stream of WM_SIZE messages are
					// sent to the window, and it would be pointless (and slow)
					// to resize for each WM_SIZE message received from dragging
					// the resize bars.  So instead, we reset after the user is 
					// done resizing the window and releases the resize bars, which 
					// sends a WM_EXITSIZEMOVE message.
				}
				else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
				{
					OnResize();
				}
			}
		}
		return 0;

		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
	case WM_ENTERSIZEMOVE:
		_gamePaused = true;
		_resizing = true;
		_timer.Stop();
		return 0;

		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
	case WM_EXITSIZEMOVE:
		_gamePaused = false;
		_resizing = false;
		_timer.Start();
		OnResize();
		return 0;

		// WM_DESTROY is sent when the window is being destroyed.
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

		// The WM_MENUCHAR message is sent when a menu is active and the user presses 
		// a key that does not correspond to any mnemonic or accelerator key. 
	case WM_MENUCHAR:
		// Don't beep when we alt-enter.
		return MAKELRESULT(0, MNC_CLOSE);

		// Catch this message so to prevent the window from becoming too small.
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		//OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		
		return 0;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		//OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MOUSEMOVE:
		//OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		std::cout << "Mouse moved:  " << GET_X_LPARAM(lParam) << "\t:\t" << GET_Y_LPARAM(lParam) << std::endl;
		return 0;
	case WM_KEYUP:
		if (wParam == VK_ESCAPE)
		{
			PostQuitMessage(0);
		}
		else if ((int)wParam == VK_F2)
			Set4xMsaaState(!_4xMsaaState);

		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

void GameWindow::CreateRtvAndDsvDescriptorHeaps()
{
	DescriptorHeapHelper::CreateDescriptorHeap(_d3dDevice.Get(), _rtvHeap.GetAddressOf(),
		_swapChainBufferCount, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	DescriptorHeapHelper::CreateDescriptorHeap(_d3dDevice.Get(), _dsvHeap.GetAddressOf(),
		1, D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

void GameWindow::OnResize()
{
	// TODO: implement the OnResize .
}

bool GameWindow::InitMainWindow()
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = _hAppInst;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"GameWnd";

	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"RegisterClass Failed.", L"GameWindow::InitMainWindow", 0);
		return false;
	}

	// figure out the window size to fit to the game's resolution.
	RECT R = { 0, 0, _clientWidth, _clientHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	_hMainWnd = CreateWindow(L"GameWnd", _mainWndCaption.c_str(),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, _hAppInst, 0);

	if (!_hMainWnd)
	{
		MessageBox(0, L"CreateWindow Failed.", L"GameWindow::InitMainWindow", 0);
		return false;
	}

	ShowWindow(_hMainWnd, SW_SHOW);
	UpdateWindow(_hMainWnd);

	return true;
}

}// namespace EyeEngine
