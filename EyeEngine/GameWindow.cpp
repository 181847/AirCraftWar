#include "GameWindow.h"
#include <WindowsX.h>
#include <MyTools/MyAssert.h>
#include <iostream>
#include "DescriptorHeapHelper.h"
#include "D3D12Helper.h"

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

bool GameWindow::InitDirect3D()
{
#if defined(DEBUG) || defined(_DEBUG) 
	// Enable the D3D12 debug layer.
	{
		ComPtr<ID3D12Debug> debugController;
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
		debugController->EnableDebugLayer();
	}
#endif

	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&_dxgiFactory)));

	// Try to create hardware device.
	HRESULT hardwareResult = D3D12CreateDevice(
		nullptr,             // default adapter
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(&_d3dDevice));

	// Fallback to WARP device.
	if (FAILED(hardwareResult))
	{
		ComPtr<IDXGIAdapter> pWarpAdapter;
		ThrowIfFailed(_dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));

		ThrowIfFailed(D3D12CreateDevice(
			pWarpAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&_d3dDevice)));
	}

	ThrowIfFailed(_d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, 
		IID_PPV_ARGS(_fence.GetAddressOf())));

	_rtvDescriptorSize = _d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	_dsvDescriptorSize = _d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	_cbvSrvUavDescriptorSize = _d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
	msQualityLevels.Format = _backBufferFormat;
	msQualityLevels.SampleCount = 4;
	msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msQualityLevels.NumQualityLevels = 0;
	ThrowIfFailed(_d3dDevice->CheckFeatureSupport(
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&msQualityLevels,
		sizeof(msQualityLevels)));

	_4xMsaaQuality = msQualityLevels.NumQualityLevels;
	assert(_4xMsaaQuality > 0 && "Unexpected MSAA quality level.");

#ifdef _DEBUG
	LogAdapters();
#endif

	CreateCommandObjects();
	CreateSwapChain();

	return false;
}

void GameWindow::CreateCommandObjects()
{
	D3D12Helper::CreateCommandQueue(
		_d3dDevice.Get(), _commandQueue.GetAddressOf(), 
		D3D12_COMMAND_LIST_TYPE_DIRECT);

	D3D12Helper::CreateCommandAllocator(
		_d3dDevice.Get(), _directCmdListAlloc.GetAddressOf(),
		D3D12_COMMAND_LIST_TYPE_DIRECT);

	D3D12Helper::CreateCommandList(_d3dDevice.Get(),
		_commandList.GetAddressOf(), D3D12_COMMAND_LIST_TYPE_DIRECT,
		_directCmdListAlloc.Get());
}

void GameWindow::CreateSwapChain()
{
	DXGI_SAMPLE_DESC sampleDesc;
	sampleDesc.Count = _4xMsaaState ? 4 : 1;
	sampleDesc.Quality = _4xMsaaState ? (_4xMsaaQuality - 1) : 0;

	D3D12Helper::CreateSwapChain(
		_dxgiFactory.Get(), _commandQueue.Get(), 
		_SwapChain.GetAddressOf(),
		_hMainWnd, 
		D3D12Helper::DxgiMode(_backBufferFormat, _clientWidth, _clientHeight), 
		sampleDesc);
}

void GameWindow::FlushCommandQueue()
{
	_currentFence++;
	ThrowIfFailed(_commandQueue->Signal(_fence.Get(), _currentFence));

	D3D12Helper::MakeFenceWaitFor(_fence.Get(), _currentFence);
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


void GameWindow::LogAdapters()
{
	UINT i = 0;
	IDXGIAdapter* adapter = nullptr;
	std::vector<IDXGIAdapter*> adapterList;
	while (_dxgiFactory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC desc;
		adapter->GetDesc(&desc);

		std::wstring text = L"***Adapter: ";
		text += desc.Description;
		text += L"\n";

		OutputDebugString(text.c_str());

		adapterList.push_back(adapter);

		++i;
	}

	for (size_t i = 0; i < adapterList.size(); ++i)
	{
		LogAdapterOutputs(adapterList[i]);
		ReleaseCom(adapterList[i]);
	}
}

void GameWindow::LogAdapterOutputs(IDXGIAdapter * adapter)
{
	UINT i = 0;
	IDXGIOutput* output = nullptr;
	while (adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_OUTPUT_DESC desc;
		output->GetDesc(&desc);

		std::wstring text = L"***Output: ";
		text += desc.DeviceName;
		text += L"\n";
		OutputDebugString(text.c_str());

		LogOutputDisplayModes(output, _backBufferFormat);

		ReleaseCom(output);

		++i;
	}
}

void GameWindow::LogOutputDisplayModes(IDXGIOutput * output, DXGI_FORMAT format)
{
	UINT count = 0;
	UINT flags = 0;

	// Call with nullptr to get list count.
	output->GetDisplayModeList(format, flags, &count, nullptr);

	std::vector<DXGI_MODE_DESC> modeList(count);
	output->GetDisplayModeList(format, flags, &count, &modeList[0]);

	for (auto& x : modeList)
	{
		UINT n = x.RefreshRate.Numerator;
		UINT d = x.RefreshRate.Denominator;
		std::wstring text =
			L"Width = " + std::to_wstring(x.Width) + L" " +
			L"Height = " + std::to_wstring(x.Height) + L" " +
			L"Refresh = " + std::to_wstring(n) + L"/" + std::to_wstring(d) +
			L"\n";

		::OutputDebugString(text.c_str());
	}
}

}// namespace EyeEngine
