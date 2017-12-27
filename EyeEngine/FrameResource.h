#pragma once/*
#include <DirectX12/Common/d3dUtil.h>
#include <DirectX12/Common/MathHelper.h>
#include <DirectX12/Common/UploadBuffer.h>*/
#include "CommonDirectXTKHeadFiles.h"
#include <MyTools/MyTools.h>

namespace EyeEngine
{

#pragma region Frame Resource Structures
// [ world position, materialIndexs]
struct InstanceData
{
	DirectX::XMFLOAT4X4 World = MathHelper::Identity4x4();
	UINT MaterialIndex;
	UINT InstancePad0;
	UINT InstancePad1;
	UINT InstancePad2;
};

// [Matrix about camera, Lights, GameTime]
struct PassConstants
{
	DirectX::XMFLOAT4X4 View = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 InvView = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 Proj = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 InvProj = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 ViewProj = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 InvViewProj = MathHelper::Identity4x4();
	DirectX::XMFLOAT3 EyePosW = { 0.0f, 0.0f, 0.0f };
	float cbPerObjectPad1 = 0.0f;
	DirectX::XMFLOAT2 RenderTargetSize = { 0.0f, 0.0f };
	DirectX::XMFLOAT2 InvRenderTargetSize = { 0.0f, 0.0f };
	float NearZ = 0.0f;
	float FarZ = 0.0f;
	float TotalTime = 0.0f;
	float DeltaTime = 0.0f;

	DirectX::XMFLOAT4 AmbientLight = { 0.0f, 0.0f, 0.0f, 1.0f };

	// Indices [0, NUM_DIR_LIGHTS) are directional lights;
	// indices [NUM_DIR_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHTS) are point lights;
	// indices [NUM_DIR_LIGHTS+NUM_POINT_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHT+NUM_SPOT_LIGHTS)
	// are spot lights for a maximum of MaxLights per object.
	Light Lights[MaxLights];
};

struct MaterialData
{
	DirectX::XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
	float Roughness = 0.5f;

	// Used in texture mapping.
	DirectX::XMFLOAT4X4 MatTransform = MathHelper::Identity4x4();

	UINT DiffuseMapIndex = 0;
	UINT NormalMapIndex = 0;
	UINT MaterialPad1;
	UINT MaterialPad2;
};

template<typename GEOMETRY_TYPE>
struct InstanceSet
{
public:
	GEOMETRY_TYPE* _pGeometry = nullptr;
	UINT _maxInstance = 0;
	UINT _numAvaliableInstance = 0;
	std::unique_ptr<UploadBuffer<InstanceData>> _instanceCB;

	InstanceSet(ID3D12Device* pDevice, GEOMETRY_TYPE* pGeo, UINT maxInstanceCount);
};
#pragma endregion

#pragma region InstanceSet functions

template<typename GEOMETRY_TYPE>
inline InstanceSet<GEOMETRY_TYPE>::InstanceSet(ID3D12Device* pDevice, GEOMETRY_TYPE * pGeo, UINT maxInstanceCount)
	:_pGeometry(pGeo), _maxInstance(maxInstanceCount), _numAvaliableInstance(0)
{
	_instanceCB = std::make_unique<UploadBuffer<InstanceData>>(pDevice, maxInstanceCount, false);
}

#pragma endregion

class FrameResource
{
public:
	UINT64 _fenceValue;
	// unique command allocator
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> _cmdAlloca;
	// all the instance for each geometry
	std::vector<std::unique_ptr<InstanceSet<MeshGeometry>>> _instanceSets;

	// The main game constant buffer, store the [projectViewMatrix, lights]
	std::unique_ptr<UploadBuffer<PassConstants>> _passCB;

	// All the Matrial buffer.
	std::unique_ptr<UploadBuffer<MaterialData>> _materialCB;
	
public:
	FrameResource(
		ID3D12Device * pDevice, 
		UINT materialCount);
	~FrameResource();

	DELETE_COPY_CONSTRUCTOR(FrameResource)
};


}// namespace EyeEngine
