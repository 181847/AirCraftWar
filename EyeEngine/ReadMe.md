# EyeEngine
这是一个简陋的游戏引擎，使用Win32+DirectX来创建主要的游戏窗口。

大部分的窗口创建代码来自 **《Introduction to 3D Game Programming with DirectX12 》**

## Systems
System  |   职能
--------|---------
RenderSystem    |   负责所有D3D对象的创建， 但是我在构想的时候不想把这个系统限定在窗口的SwapChain之上，所以有一些接口留有余地，而不是限定死了必须使用SwapChain作为渲染目标。<br/>但是目前而言还是以SwapChain为主。<br/>而这些和SwapChain相关的函数名都会带有Window字样。

