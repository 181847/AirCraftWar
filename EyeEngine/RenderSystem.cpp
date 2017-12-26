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

void RenderSystem::WindowDraw(GameTimer & gt)
{
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

