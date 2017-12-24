#pragma once
#include <DirectX12\Common\d3dUtil.h>

namespace EyeEngine
{
class D3D12Helper
{
public:
	// Create a command queue, throw a DxException when failed.
	static void CreateCommandQueue(ID3D12Device* pDevice, ID3D12CommandQueue** ppCmdQueue,
		D3D12_COMMAND_LIST_TYPE type, D3D12_COMMAND_QUEUE_FLAGS flag = D3D12_COMMAND_QUEUE_FLAG_NONE,
		UINT nodeMask = 0, D3D12_COMMAND_QUEUE_PRIORITY priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL);

	// Create a command allocator, throw a DxException when failed.
	static void CreateCommandAllocator(ID3D12Device* pDevice, ID3D12CommandAllocator** ppCmdAllocator,
		D3D12_COMMAND_LIST_TYPE type);

	// Create a command list and close it, throw a DxException when failed.
	static void CreateCommandList(ID3D12Device* pDevice, ID3D12GraphicsCommandList** ppCmdList,
		D3D12_COMMAND_LIST_TYPE type, ID3D12CommandAllocator* pAllco, UINT nodeMask = 0, ID3D12PipelineState* pInitPSO = nullptr);

	// Create a swapChain throw a DxException when failed.
	// for bufferDesc, you can use function DxgiMode(...).
	static void CreateSwapChain(
		IDXGIFactory4* pDxgiFactory,
		ID3D12CommandQueue* pCmdQueue,
		IDXGISwapChain** ppSwapChain,
		HWND outputWindow,
		DXGI_MODE_DESC bufferDesc,
		DXGI_SAMPLE_DESC sampleDesc,
		UINT bufferCount = 2,
		BOOL windowed = true,
		DXGI_USAGE bufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
		DXGI_SWAP_EFFECT swapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
		UINT flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

	static DXGI_MODE_DESC DxgiMode(
		DXGI_FORMAT format, UINT width, UINT height, 
		UINT refreshRate_Numerator = 60, UINT refreshRate_Demominator = 1, 
		DXGI_MODE_SCANLINE_ORDER scanlingOrder = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED, 
		DXGI_MODE_SCALING scanling = DXGI_MODE_SCALING_UNSPECIFIED);

};
}
