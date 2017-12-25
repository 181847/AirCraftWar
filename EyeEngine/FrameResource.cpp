#include "FrameResource.h"
#include <MyTools/MyAssert.h>


namespace EyeEngine
{

FrameResource::FrameResource(
	ID3D12Device * pDevice, 
	UINT materialCount)
{
	ThrowIfFailed(
		pDevice->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT, 
			IID_PPV_ARGS(_cmdAlloca.GetAddressOf())));

	_passCB = std::make_unique<UploadBuffer<PassConstants>>(pDevice, 1, true);
	_materialCB = std::make_unique<UploadBuffer<MaterialData>>(pDevice, materialCount, false);
}

FrameResource::~FrameResource()
{
}

}// namespace EyeEngine
