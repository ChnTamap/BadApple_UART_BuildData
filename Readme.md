<h1>基于GXCT嵌入式STM32开发板的BadApple个人项目</h1>

![IMG](img.jpg)

# 介绍

一年前(2019.4)参加完蓝桥杯后因个人兴趣做的一个通过UART从PC传输BadApple视频至STM32开发板并显示在LCD上的小项目。

项目由三部分构成：

- 接收数据并显示在LCD的STM32程序
- PC端进行串行通讯发送数据并与STM32进行同步的上位机Win32控制台程序
- > 对图像数据进行编码及压缩的Win32控制台程序（即本Repo）

# 性能

由于STM32及串行通讯的性能问题，简单的对项目使用了一部分压缩算法，视频帧率大部分时间都是25FPS。同时设计了时间同步协议，当某帧渲染时间过长，会减少后续帧的空闲时间，直到与实际视频的时间同步，整个播放过程中原视频与LCD上的视频达到完美同步。

# 使用

将编译好的exe可执行文件与按顺序命名好的.bmp文件放在同个文件夹下并运行，程序将生成.bin文件，如./obj内部分测试文件。

.bmp文件命名规则：ph (x).bmp x代表编号（帧）。

.\src\test_TransPhoto.cpp 第六行 #define _OUTPUT_TEST_ 可控制是否在控制台预输出图像（大幅降低编码速度）