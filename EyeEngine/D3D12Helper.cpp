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
	ID3D12Device * pDevice, ID3D12GraphicsCommandList ** ppCmdList,
	D3D12_COMMAND_LIST_TYPE type, ID3D12CommandAllocator * pAllco, 
	UINT nodeMask, ID3D12PipelineState * pInitPSO)
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

}// namespace EyeEngine
