#include "D3D12Helper.h"

namespace EyeEngine
{
	

void 
D3D12Helper::CreateCommandQueue(
	ID3D12Device *			pDevice,	ID3D12CommandQueue **			ppCmdQueue, 
	D3D12_COMMAND_LIST_TYPE type,		D3D12_COMMAND_QUEUE_FLAGS		flag, 
	UINT					nodeMask,	D3D12_COMMAND_QUEUE_PRIORITY	priority)
{
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = type;
	queueDesc.Flags = flag;
	queueDesc.NodeMask = nodeMask;
	queueDesc.Priority = priority;
	ThrowIfFailed(pDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(ppCmdQueue)));
}

void 
D3D12Helper::CreateCommandAllocator(
	ID3D12Device * pDevice, ID3D12CommandAllocator ** ppCmdAllocator, 
	D3D12_COMMAND_LIST_TYPE type)
{
	ThrowIfFailed(pDevice->CreateCommandAllocator(type,
		IID_PPV_ARGS(ppCmdAllocator)));
}

void 
D3D12Helper::CreateCommandList(
	ID3D12Device *					pDevice,
	ID3D12CommandAllocator *		pAllco, 
	ID3D12GraphicsCommandList **	ppCmdList,
	D3D12_COMMAND_LIST_TYPE			type,  
	UINT							nodeMask, 
	ID3D12PipelineState *			pInitPSO)
{
	ThrowIfFailed(pDevice->CreateCommandList(
		nodeMask,
		type,
		pAllco, // Associated command allocator
		pInitPSO,                   // Initial PipelineStateObject
		IID_PPV_ARGS(ppCmdList)));

	(*ppCmdList)->Close();
}

void D3D12Helper::CreateSwapChain(
	IDXGIFactory4* pDxgiFactory,
	ID3D12CommandQueue* pCmdQueue,
	IDXGISwapChain** ppSwapChain,
	HWND outputWindow, 
	DXGI_MODE_DESC bufferDesc, DXGI_SAMPLE_DESC sampleDesc, 
	UINT bufferCount, BOOL windowed, 
	DXGI_USAGE bufferUsage, DXGI_SWAP_EFFECT swapEffect, 
	UINT flags)
{
	DXGI_SWAP_CHAIN_DESC scDesc;
	scDesc.BufferDesc = bufferDesc;
	scDesc.SampleDesc = sampleDesc;
	scDesc.BufferUsage = bufferUsage;
	scDesc.BufferCount = bufferCount;
	scDesc.OutputWindow = outputWindow;
	scDesc.Windowed = windowed;
	scDesc.SwapEffect = swapEffect;
	scDesc.Flags = flags;

	ThrowIfFailed(pDxgiFactory->CreateSwapChain(pCmdQueue, &scDesc, ppSwapChain));
}

void D3D12Helper::CreateFence(ID3D12Device * pDevice, ID3D12Fence** ppFence, UINT64 initValue, D3D12_FENCE_FLAGS flags)
{
	ThrowIfFailed(pDevice->CreateFence(
		initValue, flags, IID_PPV_ARGS(ppFence)));
}

DXGI_MODE_DESC D3D12Helper::DxgiMode(
	DXGI_FORMAT format, UINT width, UINT height, 
	UINT refreshRate_Numerator, UINT refreshRate_Demominator, 
	DXGI_MODE_SCANLINE_ORDER scanlingOrder, 
	DXGI_MODE_SCALING scanling)
{
	DXGI_MODE_DESC modeDesc;
	modeDesc.Format = format;
	modeDesc.Width = width;
	modeDesc.Height = height;
	modeDesc.RefreshRate.Numerator = refreshRate_Numerator;
	modeDesc.RefreshRate.Denominator = refreshRate_Demominator;
	modeDesc.ScanlineOrdering = scanlingOrder;
	modeDesc.Scaling = scanling;
	return modeDesc;
}

void D3D12Helper::MakeFenceWaitFor(ID3D12Fence * pFence, UINT64 expectValue)
{
	if (pFence->GetCompletedValue() < expectValue)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		
		pFence->SetEventOnCompletion(expectValue, eventHandle);
		
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
}

UINT D3D12Helper::CheckMultiSampleQuality(ID3D12Device* pDevice, DXGI_FORMAT format, UINT sampleCount, D3D12_MULTISAMPLE_QUALITY_LEVEL_FLAGS flags)
{
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
	msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msQualityLevels.Format = format;
	msQualityLevels.NumQualityLevels = 0;
	msQualityLevels.SampleCount = 4;

	pDevice->CheckFeatureSupport(
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&msQualityLevels,
		sizeof(msQualityLevels));

	return msQualityLevels.NumQualityLevels;
}

void D3D12Helper::
CreateDescriptorHeap(
	ID3D12Device * d3dDevice,
	ID3D12DescriptorHeap ** pDescriptorHeap,
	UINT numDescriptors,
	D3D12_DESCRIPTOR_HEAP_TYPE type,
	D3D12_DESCRIPTOR_HEAP_FLAGS flag,
	UINT nodeMask)
{
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
	heapDesc.NumDescriptors = numDescriptors;
	heapDesc.Type = type;
	heapDesc.Flags = flag;
	heapDesc.NodeMask = nodeMask;

	ThrowIfFailed(d3dDevice->CreateDescriptorHeap(
		&heapDesc, IID_PPV_ARGS(pDescriptorHeap)));
}

}// namespace EyeEngine
