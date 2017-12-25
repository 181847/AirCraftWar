#include "RenderSystem.h"
#include "D3D12Helper.h"

namespace EyeEngine
{
RenderSystem::RenderSystem(
	UINT frameResourceCount, 
	ID3D12Device*	pDevice,
	ID3D12CommandQueue * pCmdQueue,
	ID3D12GraphicsCommandList*	pCmdList,
	ID3D12Fence * pFence, 
	UINT64 & currFence,
	const UINT materialCount)
	:
	_device(pDevice), 
	_cmdQueue(pCmdQueue),
	_cmdList(pCmdList), 
	_fence(pFence),
	_currFence(currFence),
	_numFrameResource(frameResourceCount)
{
	for (unsigned int i = 0; i < frameResourceCount; ++i)
	{
		_frameResources.push_back(std::make_unique<FrameResource>(pDevice, materialCount));
	}
}

RenderSystem::~RenderSystem()
{
}

void RenderSystem::Draw(GameTimer & gt)
{
	// Because the Update is called before Draw(gt),
	// so the commandAllocator of current FrameResource is no use,
	// we can reset it as desired.
	auto currFrameCmdAlloc = GetCurrentFrameResouce()->_cmdAlloca;

	ThrowIfFailed(currFrameCmdAlloc->Reset());

	ThrowIfFailed(_cmdList->Reset(currFrameCmdAlloc.Get(), nullptr));
	
}

void RenderSystem::TickFrameResource()
{
	_currFrameResourceIndex = (_currFrameResourceIndex + 1) % _numFrameResource;

	D3D12Helper::MakeFenceWaitFor(
		_fence, GetCurrentFrameResouce()->_fenceValue);
}

FrameResource * RenderSystem::GetCurrentFrameResouce()
{
	return _frameResources[_currFrameResourceIndex].get();
}

}// namespace EyeEngine

