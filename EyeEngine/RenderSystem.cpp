#include "RenderSystem.h"
#include "D3D12Helper.h"
#include "EyeLogger.h"

namespace EyeEngine
{

RenderSystem::~RenderSystem()
{
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

void RenderSystem::Draw(GameTimer & gt)
{
	// Because the Update is called before Draw(gt),
	// so the commandAllocator of current FrameResource is no use,
	// we can reset it as desired.
	auto currFrameCmdAlloc = GetCurrentFrameResouce()->_cmdAlloca;

	ThrowIfFailed(currFrameCmdAlloc->Reset());

	ThrowIfFailed(_cmdList->Reset(currFrameCmdAlloc.Get(), nullptr));
	
}

void RenderSystem::TickFrameResource()
{
	_currFrameResourceIndex = (_currFrameResourceIndex + 1) % _numFrameResource;

	D3D12Helper::MakeFenceWaitFor(
		_fence, GetCurrentFrameResouce()->_fenceValue);
}

FrameResource * RenderSystem::GetCurrentFrameResouce()
{
	return _frameResources[_currFrameResourceIndex].get();
}

}// namespace EyeEngine

