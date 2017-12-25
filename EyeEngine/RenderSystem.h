#pragma once
#include <DirectX12/Common/d3dUtil.h>
#include <DirectX12/Common/Camera.h>
#include "FrameResource.h"

namespace EyeEngine
{

class RenderSystem
{
protected:
	ID3D12CommandQueue*		_cmdQueue;
	ID3D12CommandList*		_cmdList;

	std::vector<std::unique_ptr<FrameResource>> _frameResources;

public:
	RenderSystem(UINT frameResourceCount);
	DELETE_COPY_CONSTRUCTOR(RenderSystem)
	~RenderSystem();

	template<typename...FRAME_RESOURCE_INIT_ARGS>
	void Init(FRAME_RESOURCE_INIT_ARGS...args);
};

template<typename ...FRAME_RESOURCE_INIT_ARGS>
inline void RenderSystem::Init(FRAME_RESOURCE_INIT_ARGS ...args)
{
}

}// namespace EyeEngine

