#pragma once
#include <DirectX12/Common/d3dUtil.h>
#include <DirectX12/Common/Camera.h>
#include <DirectX12/Common/GameTimer.h>
#include "FrameResource.h"
#include "DirectXTKIncludeHeader.h"

namespace EyeEngine
{

// The RenderSystem take responsibility of the original window creation operation,
// the RenderSystem go to make all the D3DObjects.
class RenderSystem
{
protected:
#pragma region variables about D3D12 Objects
	Microsoft::WRL::ComPtr<IDXGIFactory4>	_dxgiFactory;
	Microsoft::WRL::ComPtr<ID3D12Device>	_d3dDevice;

	Microsoft::WRL::ComPtr<ID3D12Fence>		_fence;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue>			_cmdQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator>		_directCmdAlloc;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>	_cmdList;

	UINT _rtvDescriptorSize			= 0;
	UINT _dsvDescriptorSize			= 0;
	UINT _cbvSrvUavDescriptorSize	= 0;

	D3D_DRIVER_TYPE _d3dDriverType = D3D_DRIVER_TYPE_HARDWARE;

	std::unique_ptr<DirectX::GraphicsMemory> m_graphicMemory;
#pragma endregion

#pragma region D3D12 Status variabls
	UINT64		_currFenceValue = 0;

#pragma region variables about Window usage such as [swapChain, viewPort, wndWidth, ...]

#pragma region variables about frameResources
	// the current frame resource Index
	UINT		_currFrameResourceIndex = 0;
	const UINT	_numFrameResource;
	std::vector<std::unique_ptr<FrameResource>> _frameResources;
#pragma endregion
										// Set true to use 4X MSAA (?.1.8).  The default is false.
	bool		_4xMsaaState = false;	// 4X MSAA enabled
	UINT		_4xMsaaQuality = 0;		// quality level of 4X MSAA

	static const UINT _swapChainBufferCount = 2;
	UINT _currBackBuffer = 0;

	Microsoft::WRL::ComPtr<IDXGISwapChain> _swapChain;

	Microsoft::WRL::ComPtr<ID3D12Resource> _swapChainBuffer[_swapChainBufferCount];
	Microsoft::WRL::ComPtr<ID3D12Resource> _depthStencilBuffer;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _rtvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _dsvHeap;

	D3D12_VIEWPORT _screenViewport;
	D3D12_RECT _scissorRect;
	DXGI_FORMAT _backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT _depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	int _clientWidth = 800;
	int _clientHeight = 600;

#pragma endregion

public:
	RenderSystem(UINT numFrameResources = 3);
	DELETE_COPY_CONSTRUCTOR(RenderSystem)
	~RenderSystem();

#pragma region  functions about common D3DObjects operation.
	// Initialize the common d3d objects.
	// [dxgiFactor, d3dDevice, fence, cmdQueue, cmdAlloc, cmdList0]
	void InitD3DCommon();

	// Create [dxgiFactory, d3dDevice, fence].
	// Get the multiSample support.
	// Throw DxException when failed.
	void CreateBasicD3DOjbects();

	// Get rtv/dsv/cbv... descriptor size from _d3dDevice.
	void InitDescriptorSizes();

	// Create [cmdQueue, cmdAllocator, cmdList].
	void CreateCommandObjects();

	// Wait CommandQueue to reach a specific point.
	void FlushCommandQueue();

	// Return whether the device has been created.
	bool IsDeviceCreated();
#pragma endregion

#pragma region functions for logging adapter and output display modes
	void LogAdapters();

	void LogAdapterOutputs(IDXGIAdapter* pAdapter);

	void LogOutputDisplayModes(IDXGIOutput * output, DXGI_FORMAT format);
#pragma endregion

#pragma region Window functions
	// Initialize the window's objects.
	void WindowInit(HWND hWindow, UINT width, UINT height);

	// for the back buffer and depthStencil buffer's creation.
	DXGI_SAMPLE_DESC WindowSampleDesc();

	// check the multiSample support for BackBuffer
	void WindowCheckMultiSampleSupport();

	// Create a SwapChain for the windowed application.
	void WindowCreateSwapChain(HWND hWindow, UINT width, UINT height);

	// For a window application, get the backBuffer resource.
	ID3D12Resource* WindowGetCurrentBackBuffer();

	// Get descriptor for current used backBuffer.
	D3D12_CPU_DESCRIPTOR_HANDLE WindowCurrentBackBufferView();

	// Get descriptor of the depthStacilBuffer's View, 
	// which is combined with the backBuffer;
	D3D12_CPU_DESCRIPTOR_HANDLE WindowDepthStencilView();

	// create rtv and dsv descriptor heap for backBuffer and depthStencil buffer.
	void WindowCreateRtvAndDsvDescriptorHeaps();

	// resize the swapChain and corresponding depthStencil buffer
	void WindowOnResize(int newWidth, int newHeight);

	void WindowSet4xMsaaState(bool value);
	bool WindowGet4xMsaaState();

	float WindowAspectRation();

	// clear the commandAlloca and commandList,
	// set the backBuffer/ depthStencilBuffer,
	// set ViewPort/ ScissorRect.
	void WindowClear();

	// swap the back and front buffer,
	// present the swapChain.
	void WindowPresent();

	// update [lights, cameras, new instance, change instance pose],
	// change to another FrameResource to get 
	//void WindowUpdate(GameTimer& gt);

	// Commit drawing command to command queue and execute it.
	// For test, just reset the swapChains buffer with different color.
	void WindowDraw(GameTimer& gt);

protected:
	// Switch to the next backBuffer, by adding one to the _currBackBuffer.
	void WindowTickSwapChain();

	// Switch to the next frameResource, 
	// and ready to store new data.
	// Block the thread if the new frameResource is still in use.
	void WindowTickFrameResource();

	// Get the FrameResource with the index _currFrameResourceIndex.
	FrameResource* WindowGetCurrentFrameResouce();
#pragma endregion

};

}// namespace EyeEngine

