#include "DescriptorHeapHelper.h"

namespace EyeEngine
{


void DescriptorHeapHelper::
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

