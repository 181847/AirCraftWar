#pragma once
#include <DirectX12/Common/d3dUtil.h>
#include <DirectX12/Common/Camera.h>
#include <DirectX12/Common/GameTimer.h>
#include "FrameResource.h"

namespace EyeEngine
{

// The RenderSystem take responsibility of the original window creation operation,
// the RenderSystem go to make all the D3DObjects.
// 
class RenderSystem
{
protected:
#pragma region variables about D3D12 Objects
	Microsoft::WRL::ComPtr<IDXGIFactory4> _dxgiFactory;
	Microsoft::WRL::ComPtr<IDXGISwapChain> _swapChain;
	Microsoft::WRL::ComPtr<ID3D12Device> _d3dDevice;

	Microsoft::WRL::ComPtr<ID3D12Fence> _fence;
	UINT64 _currentFence = 0;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> _commandQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> _directCmdListAlloc;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> _commandList;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _rtvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _dsvHeap;

	D3D12_VIEWPORT _screenViewport;
	D3D12_RECT _scissorRect;

	UINT _rtvDescriptorSize = 0;
	UINT _dsvDescriptorSize = 0;
	UINT _cbvSrvUavDescriptorSize = 0;

	D3D_DRIVER_TYPE _d3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
	DXGI_FORMAT _backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT _depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	int _clientWidth = 800;
	int _clientHeight = 600;

	static const UINT _swapChainBufferCount = 2;
	UINT _currBackBuffer = 0;
	Microsoft::WRL::ComPtr<ID3D12Resource> _swapChainBuffer[_swapChainBufferCount];
	Microsoft::WRL::ComPtr<ID3D12Resource> _depthStencilBuffer;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _rtvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _dsvHeap;
#pragma endregion

	// notice:
	// This is a reference, should be init as the RenderSystem created.
	UINT64&					_currFence;

#pragma region variables about frameResources
	const UINT _numFrameResource;
	std::vector<std::unique_ptr<FrameResource>> _frameResources;
	// the current frame resource Index

	UINT _currFrameResourceIndex = 0;
#pragma endregion

public:
	RenderSystem();

	DELETE_COPY_CONSTRUCTOR(RenderSystem)
	~RenderSystem();

	// Create [dxgiFactory, d3dDevice, fence].
	// Get the multiSample support.
	// Throw DxException when failed.
	void CreateBaseD3DOjbects();

	// Wait CommandQueue to reach a spcific point.
	void FlushCommandQueue();

#pragma region Window functions
	// Create a SwapChain for the windowed application.
	void WindowCreateSwapChain(HWND hWindow);

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
#pragma endregion

	// update [lights, cameras, new instance, change instance pose],
	// change to another FrameResource to get 
	void Update(GameTimer& gt);

	// Commite drawing command to command queue and execute it.
	void Draw(GameTimer& gt);

protected:
	// Switch to the next frameResource, 
	// and ready to store new data.
	// Block the thread if the new frameResource is still in use.
	void TickFrameResource();

	// Get the FrameResource with the index _currFrameResourceIndex.
	FrameResource* GetCurrentFrameResouce();
};

}// namespace EyeEngine

