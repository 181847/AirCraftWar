# EyeEngine
这是一个简陋的游戏引擎，使用Win32+DirectX来创建主要的游戏窗口。

大部分的窗口创建代码来自 **《Introduction to 3D Game Programming with DirectX12 》**

## 日志记录功能
```c++
#include "EyeLogger.h"
LOG_DEBUG("测试用调试信息")；
LOG_INFO("测试用日志信息")；
LOG_WARN("测试用警告信息")；
LOG_FATAL("测试用致命错误信息")；
```

## Systems
System  |   职能
--------|---------
RenderSystem    |   负责所有D3D对象的创建， 但是我在构想的时候不想把这个系统限定在窗口的SwapChain之上，所以有一些接口留有余地，而不是限定死了必须使用SwapChain作为渲染目标。<br/>但是目前而言还是以SwapChain为主。<br/>而这些和SwapChain相关的函数名都会带有Window字样。

<br/>
<br/>
<br/>
<br/>

## D3D12Helper
定义一系列方便调用的D3D包装函数，比如创建CommandQueue，比如阻塞Fence直到大于某个特定的值。

函数名 |   功能
----|--------
CreateCommandQueue  |   创建CommandQueue
CreateCommandAllocator  |   创建CommandAllocator
CreateCommandList   |   创建CommandList
CreateSwapChain |   创建SwapChain
DxgiMode    |   用于引导生成**DXGI_MODE_DESC**结构体，这个方法可以用于CreateSwapChain的参数中。
MakeFenceWaitFor    |   阻塞线程，直到Fence的值大于等于指定的值
