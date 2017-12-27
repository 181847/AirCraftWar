#pragma once
#include "CommonDirectXTKHeadFiles.h"

namespace EyeEngine
{
class D3D12Helper
{
public:
	// Create a command queue, throw a DxException when failed.
	static void CreateCommandQueue(
#pragma region CreateCommandQueue arguments
		ID3D12Device*					pDevice, 
		ID3D12CommandQueue**			ppCmdQueue,
		D3D12_COMMAND_LIST_TYPE			type = D3D12_COMMAND_LIST_TYPE_DIRECT, 
		D3D12_COMMAND_QUEUE_FLAGS		flag = D3D12_COMMAND_QUEUE_FLAG_NONE,
		UINT							nodeMask = 0, 
		D3D12_COMMAND_QUEUE_PRIORITY	priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL
#pragma  endregion
		);

	// Create a command allocator, throw a DxException when failed.
	static void CreateCommandAllocator(
		ID3D12Device*				pDevice, 
		ID3D12CommandAllocator**	ppCmdAllocator,
		D3D12_COMMAND_LIST_TYPE		type = D3D12_COMMAND_LIST_TYPE_DIRECT);

	// Create a command list and close it, throw a DxException when failed.
	static void CreateCommandList
	(
#pragma region CreateCommandList arguments
		ID3D12Device*					pDevice, 
		ID3D12CommandAllocator*			pAllco,
		ID3D12GraphicsCommandList**		ppCmdList,
		D3D12_COMMAND_LIST_TYPE			type = D3D12_COMMAND_LIST_TYPE_DIRECT,
		UINT							nodeMask = 0, 
		ID3D12PipelineState*			pInitPSO = nullptr
#pragma endregion
	);

	// Create a swapChain throw a DxException when failed.
	// for bufferDesc, you can use function DxgiMode(...).
	static void CreateSwapChain
	(
#pragma region CreateSwapChain function args
		IDXGIFactory*			pDxgiFactory,
		ID3D12CommandQueue*		pCmdQueue,
		IDXGISwapChain**		ppSwapChain,
		HWND				outputWindow,
		DXGI_MODE_DESC		bufferDesc,
		DXGI_SAMPLE_DESC	sampleDesc,
		UINT				bufferCount = 2,
		BOOL				windowed = true,
		DXGI_USAGE			bufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
		DXGI_SWAP_EFFECT	swapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
		UINT				flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
#pragma endregion
	);

	// Create a fence, throw DxException when failed.
	static void CreateFence
	(
		ID3D12Device*		pDevice, 
		ID3D12Fence**		ppFence,
		UINT64				initValue = 0, 
		D3D12_FENCE_FLAGS	flags = D3D12_FENCE_FLAG_NONE
	);

	// help generate the DXGI_MODE_DESC structure.
	static DXGI_MODE_DESC DxgiMode
	(
#pragma region DxgiMode function args
		DXGI_FORMAT		format, 
		UINT			width, 
		UINT			height, 
		UINT			refreshRate_Numerator = 60, 
		UINT			refreshRate_Demominator = 1, 
		DXGI_MODE_SCANLINE_ORDER	scanlingOrder = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED, 
		DXGI_MODE_SCALING			scanling = DXGI_MODE_SCALING_UNSPECIFIED
#pragma endregion
	);

	// Block the fence until its value reach the expectValue
	static void MakeFenceWaitFor(ID3D12Fence* pFence, UINT64 expectValue);

	// check and return the featuren of multiSample quality.
	static UINT CheckMultiSampleQuality(
		ID3D12Device*	pDevice,
		DXGI_FORMAT		format, 
		UINT			sampleCount, 
		D3D12_MULTISAMPLE_QUALITY_LEVEL_FLAGS flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE);

	// Create the DescritptorHeap,
	// throw DxException if failed.
	static void CreateDescriptorHeap(
		ID3D12Device* d3dDevice,
		ID3D12DescriptorHeap ** pHeap,
		UINT numDescriptors,
		D3D12_DESCRIPTOR_HEAP_TYPE type,
		D3D12_DESCRIPTOR_HEAP_FLAGS flag = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
		UINT nodeMask = 0);
};
}
