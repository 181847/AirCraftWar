#pragma once
#include <DirectX12\Common\d3dUtil.h>

using namespace Microsoft::WRL;

namespace EyeEngine
{

class DescriptorHeapHelper
{
public:
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

}// namespace EyeEngine