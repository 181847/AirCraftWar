#include "FrameResource.h"
#include <MyTools/MyAssert.h>


namespace EyeEngine
{

FrameResource::FrameResource(
	ID3D12Device * pDevice, 
	const std::vector<MeshGeometry*>& geos, 
	const std::vector<UINT>& maxInstancePerGeo, 
	UINT materialCount)
{
	ASSERT(geos.size() != maxInstancePerGeo.size() && "geometry count and instance limitations count didn't match");

	for (UINT geoIndex = 0; geoIndex < geos.size(); ++geoIndex)
	{
		_instanceSets.emplace_back(geos[geoIndex], maxInstancePerGeo[geoIndex]);
	}

	ThrowIfFailed(
		pDevice->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT, 
			IID_PPV_ARGS(_cmdAlloca.GetAddressOf())));
}

FrameResource::~FrameResource()
{
}

}// namespace EyeEngine
