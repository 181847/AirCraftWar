#pragma once
#include <DirectX12/Common/d3dUtil.h>
#include <DirectX12/Common/Camera.h>
#include <DirectX12/Common/GameTimer.h>
#include "FrameResource.h"

namespace EyeEngine
{

class RenderSystem
{
protected:
#pragma region variables about D3D12 Objects
	ID3D12Device*			_device;
	ID3D12CommandQueue*		_cmdQueue;
	ID3D12CommandList*		_cmdList;
	ID3D12Fence*			_fence;
#pragma endregion

	// notice:
	// This is a reference, should be init as the RenderSystem created.
	UINT64&					_currFence;

#pragma region variables about frameResources
	const UINT _numFrameResource;
	std::vector<std::unique_ptr<FrameResource>> _frameResources;
	// the current frame resource Index

	UINT _currFrameResourceIndex = 0;
#pragma endregion

public:
	RenderSystem(
#pragma region RenderSystem constructor arguments
		UINT					frameResourceCount,
		ID3D12Device*			pDevice,
		ID3D12CommandQueue*		pCmdQueue,
		ID3D12CommandList*		pCmdList,
		ID3D12Fence*			pFence,
		UINT64&					currFence,
		const UINT				materialCount
#pragma endregion
	);

	DELETE_COPY_CONSTRUCTOR(RenderSystem)
	~RenderSystem();

	// update [lights, cameras, new instance, instance pose],
	// change to another FrameResource to get 
	void Update(GameTimer& gt);

	// Commite drawing command to command queue and execute it.
	void Draw(GameTimer& gt);

protected:
	// Switch to the next frameResource, 
	// and ready to store new data.
	// Block the thread if the last frameResource is still in use.
	void TickFrameResource();

	// Get the FrameResource with the index _currFrameResourceIndex.
	FrameResource* GetCurrentFrameResouce();
};

}// namespace EyeEngine

