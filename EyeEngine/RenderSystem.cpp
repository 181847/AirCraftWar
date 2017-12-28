#include "RenderSystem.h"
#include "D3D12Helper.h"
#include "EyeLogger.h"
#include <iostream>

namespace EyeEngine
{
RenderSystem::RenderSystem(UINT numFrameResources)
	:_numFrameResource(numFrameResources)
{
}

RenderSystem::~RenderSystem()
{
}


#endif

}// namespace EyeEngine

