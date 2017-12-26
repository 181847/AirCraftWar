#include "RenderSystem.h"
#include "D3D12Helper.h"
#include "EyeLogger.h"

namespace EyeEngine
{

RenderSystem::~RenderSystem()
{
}

void RenderSystem::InitD3DCommon()
{
	CreateBasicD3DOjbects();
	CreateCommandObjects();
	LogAdapters();
}

void RenderSystem::CreateBasicD3DOjbects()
{
	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(_dxgiFactory.GetAddressOf())));
	
	HRESULT hardwareResult = D3D12CreateDevice(nullptr,
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(_d3dDevice.GetAddressOf()));

	// If failed with hardware adapter, 
	// create with warpAdapter.
	if (FAILED(hardwareResult))
	{
		Microsoft::WRL::ComPtr<IDXGIAdapter> pWarpAdapter;
		ThrowIfFailed(_dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(pWarpAdapter.GetAddressOf())));

		ThrowIfFailed(D3D12CreateDevice(pWarpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(_d3dDevice.GetAddressOf())));
	}

	D3D12Helper::CreateFence(_d3dDevice.Get(), _fence.GetAddressOf());

}

void RenderSystem::CreateCommandObjects()
{
	D3D12Helper::CreateCommandQueue(_d3dDevice.Get(), _cmdQueue.GetAddressOf(), D3D12_COMMAND_LIST_TYPE_DIRECT);
	D3D12Helper::CreateCommandAllocator(_d3dDevice.Get(), _directCmdAlloc.GetAddressOf(), D3D12_COMMAND_LIST_TYPE_DIRECT);
	D3D12Helper::CreateCommandList(_d3dDevice.Get(), _directCmdAlloc.Get(), _cmdList.GetAddressOf(), D3D12_COMMAND_LIST_TYPE_DIRECT);
}

DXGI_SAMPLE_DESC RenderSystem::WindowSampleDesc()
{
	DXGI_SAMPLE_DESC desc;
	desc.Count = _4xMsaaState ? 4 : 1;
	desc.Quality = _4xMsaaState ? (_4xMsaaQuality - 1) : 0;
	return desc;
}

void RenderSystem::WindowCheckMultiSampleSupport()
{
	_4xMsaaQuality = D3D12Helper::CheckMultiSampleQuality(_d3dDevice.Get(), _backBufferFormat, 4);
	ASSERT(_4xMsaaQuality > 0 && "Unexpected multiSample quality");
}

void RenderSystem::WindowCreateSwapChain(HWND hWindow)
{
	WindowCheckMultiSampleSupport();
	DXGI_SAMPLE_DESC sampleDesc;
	sampleDesc.Count = _4xMsaaState ? 4 : 1;
	sampleDesc.Quality = _4xMsaaState ? (_4xMsaaQuality - 1) : 0;

	D3D12Helper::CreateSwapChain(
		_dxgiFactory.Get(), _cmdQueue.Get(), _swapChain.GetAddressOf(), 
		hWindow,
		D3D12Helper::DxgiMode(_backBufferFormat, _clientWidth, _clientHeight),
		sampleDesc, _swapChainBufferCount);
}

ID3D12Resource * RenderSystem::WindowGetCurrentBackBuffer()
{
	return _swapChainBuffer[_currBackBuffer].Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE RenderSystem::WindowCurrentBackBufferView()
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
		_rtvHeap->GetCPUDescriptorHandleForHeapStart(), 
		_currBackBuffer, 
		_rtvDescriptorSize);
}

D3D12_CPU_DESCRIPTOR_HANDLE RenderSystem::WindowDepthStencilView()
{
	return _dsvHeap->GetCPUDescriptorHandleForHeapStart();
}

void RenderSystem::WindowCreateRtvAndDsvDescriptorHeaps()
{
	// descriptors for swapchain.
	D3D12Helper::CreateDescriptorHeap(
		_d3dDevice.Get(), _rtvHeap.GetAddressOf(), 
		_swapChainBufferCount, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// descriptors for depthStencilBuffer
	D3D12Helper::CreateDescriptorHeap(
		_d3dDevice.Get(), _dsvHeap.GetAddressOf(),
		1, D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

void RenderSystem::WindowOnResize(int newWidth, int newHeight)
{
	assert(_dxgiFactory);
	assert(_d3dDevice);
	assert(_cmdQueue);

	_clientWidth = newWidth;
	_clientHeight = newHeight;
	
	// resize backBuffer.
	ThrowIfFailed(
		_swapChain->ResizeBuffers(
			_swapChainBufferCount, 
			newWidth, newHeight, 
			_backBufferFormat, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

	// clear back buffer's resources and rebuild them.
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandler(_rtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (int i = 0; i < _swapChainBufferCount; ++i)
	{
		_swapChainBuffer[i].Reset();
		ThrowIfFailed(_swapChain->GetBuffer(i, IID_PPV_ARGS(_swapChainBuffer[i].GetAddressOf())));
		_d3dDevice->CreateRenderTargetView(_swapChainBuffer[i].Get(), nullptr, rtvHeapHandler);
		rtvHeapHandler.Offset(1, _rtvDescriptorSize);
	}

#pragma region rebuild depthStencilBuffer
	_depthStencilBuffer.Reset();
	D3D12_RESOURCE_DESC depthStencilRecouceDesc;
	depthStencilRecouceDesc.Alignment = 0;
	depthStencilRecouceDesc.DepthOrArraySize = 1;
	depthStencilRecouceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilRecouceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	depthStencilRecouceDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	depthStencilRecouceDesc.Width = newWidth;
	depthStencilRecouceDesc.Height = newHeight;
	depthStencilRecouceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilRecouceDesc.MipLevels = 1;
	depthStencilRecouceDesc.SampleDesc = WindowSampleDesc();

	D3D12_CLEAR_VALUE optClear;
	optClear.Format = _depthStencilFormat;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;

	ThrowIfFailed(_d3dDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depthStencilRecouceDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&optClear,
		IID_PPV_ARGS(_depthStencilBuffer.GetAddressOf())));

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.Format = _depthStencilFormat;
	dsvDesc.Texture2D.MipSlice = 0;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	_d3dDevice->CreateDepthStencilView(_depthStencilBuffer.Get(), &dsvDesc, 
		_dsvHeap->GetCPUDescriptorHandleForHeapStart());
#pragma endregion

	_cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		_depthStencilBuffer.Get(), 
		D3D12_RESOURCE_STATE_COMMON, 
		D3D12_RESOURCE_STATE_DEPTH_WRITE));

	// execute all the commands
	ThrowIfFailed(_cmdList->Close());
	ID3D12CommandList* cmdList[] = { _cmdList.Get() };
	_cmdQueue->ExecuteCommandLists(_countof(cmdList), cmdList);
	FlushCommandQueue();

	// Update the viewport transform to cover the client area.
	_screenViewport.TopLeftX = 0;
	_screenViewport.TopLeftY = 0;
	_screenViewport.Width = static_cast<float>(_clientWidth);
	_screenViewport.Height = static_cast<float>(_clientHeight);
	_screenViewport.MinDepth = 0.0f;
	_screenViewport.MaxDepth = 1.0f;

	_scissorRect = { 0, 0, _clientWidth, _clientHeight };

}

void RenderSystem::FlushCommandQueue()
{
	++_currFenceValue;
	_cmdQueue->Signal(_fence.Get(), _currFenceValue);
	
	D3D12Helper::MakeFenceWaitFor(_fence.Get(), _currFenceValue);
}

void RenderSystem::LogAdapters()
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
		LOG_INFO(text);

		adapterList.push_back(adapter);

		++i;
	}

	for (size_t i = 0; i < adapterList.size(); ++i)
	{
		LogAdapterOutputs(adapterList[i]);
		ReleaseCom(adapterList[i]);
	}
}

void RenderSystem::LogAdapterOutputs(IDXGIAdapter * pAdapter)
{
	UINT i = 0;
	IDXGIOutput* output = nullptr;
	while (pAdapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_OUTPUT_DESC desc;
		output->GetDesc(&desc);

		std::wstring text = L"***Output: ";
		text += desc.DeviceName;
		text += L"\n";
		LOG_INFO(text);

		LogOutputDisplayModes(output, _backBufferFormat);

		ReleaseCom(output);

		++i;
	}
}

void RenderSystem::LogOutputDisplayModes(IDXGIOutput * output, DXGI_FORMAT format)
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

		LOG_INFO(text);
	}
}

void RenderSystem::WindowTickFrameResource()
{
	_currFrameResourceIndex = (_currFrameResourceIndex + 1) % _numFrameResource;

	D3D12Helper::MakeFenceWaitFor(
		_fence.Get(), WindowGetCurrentFrameResouce()->_fenceValue);
}

FrameResource * RenderSystem::WindowGetCurrentFrameResouce()
{
	return _frameResources[_currFrameResourceIndex].get();
}

}// namespace EyeEngine

