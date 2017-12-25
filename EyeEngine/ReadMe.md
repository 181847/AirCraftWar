# EyeEngine
这是一个简陋的游戏引擎，使用Win32+DirectX来创建主要的游戏窗口。

大部分的窗口创建代码来自 **《Introduction to 3D Game Programming with DirectX12 》**

## Systems
System  |   职能
--------|---------
RenderSystem    |   负责将顶点数据绘制出来，内部存储一个摄像机、灯光、网格顶点、网格实例，向CommandQueue提交绘制数据，并且进行基本的同步。

