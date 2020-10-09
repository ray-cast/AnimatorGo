Rabbit-Toolbox
======
###### [English](https://github.com/ray-cast/RabbitToolbox/blob/master/README_en.md) &nbsp; [中文](https://github.com/ray-cast/RabbitToolbox/blob/master/README_en.md)
Rabbit Toolbox 是一个MIT协议，开源的动画渲染软件。 为更多喜好 MikuMikuDance 的用户提供更高质量的物理演算以及渲染。用户可以轻松的迁移 MikuMikuDance 的项目到本软件中制作成视频，同时编码为各大门户视频网站最高支持的视频编码。

Rabbit Toolbox 同时提供了一个现代化和强大的ECS框架，使该项目开发变得简单。 系统以ECS的形式支持许多功能，如前向、逆向骨骼动画，实时渲染，离线渲染，物理，音频和视频。 通过附加各种组件，对象可以灵活地执行不同的任务。 

Rabbit Toolbox 拥有一个快速高效的全局照明渲染器，基于分离内核架构以避免VGPR占用瓶颈，并广泛使用GPU优化的并行图元来重构工作，以更好地适应大规模并行GPU架构，渲染器是为渐进式预览设计的，此模型允许在进行渲染时控制延迟并操纵场景和摄影机，具有同步性。 在所有供应商中保持着高水平的性能，全局照明渲染器使用OpenCL依赖于 AMD® RadeonRays 光线求交引擎，同时针对AMD®GPU和APU进行了优化。

<img style="vertical-align: top;" src="./screenshot.png" alt="logo">

特性：
------
* 基于图像的光照
* 完整的物理多层材质描述（金属，光滑，布料，清漆，次表面散射，自发光，等）
* 物理的灯光单元
* 景深控制
* 多光源支持
* OpenGL 实时渲染
* OpenCL 离线光追渲染
* 基于深度学习的后处理降噪
* 前向、逆向运动学的骨骼动画
* PMX、PMD 模型文件支持
* VMD 动画文件支持
* PMM 场景文件支持
* x264、x265的视频编码
* PNG、BMP、TGA、JPEG、DDS，HDR 图像文件
* Wav, Flac, Mp3, Ogg 音频文件播放
* NVIDIA's Material Definition Language 支持

系统需求：
------
1. 支持SSE4.1的CPU，如i5，用于AI 降噪
2. NVIDIA的独显用于GPGPU的光线追踪 
3. OpenCL 驱动 
4. Visual Studio 2019 Runtimes (软件根目录/_Redist/VC_redist.x64.exe)
6. 参与测试过的CPU，Intel i3, i5, i7, AMD 2600, 2600X
7. 参与测试过的显卡，GeFroce GTX 650, 1050, 1060, 2060

支持的编译器:
------
* Visual Studio 2017 (Only tested on 15.5.7 version)
* GNU GCC (Only tested on 5.4.0, 6.2.0, 7.3.0, 8.1.0 version)
* Clang & LLVM (Only tested on 5.0.0 version)
* Emscripten (Only tested on 1.37.40)

支持的平台:
------
* Windows (Only tested on XP, Vista, 7, 8, 10)
* Linux (Only tested on Ubuntu, Centos)
* HTML5
* Android (future)
* ios (future)
* mac (future)

支持的渲染后端:
------
* OpenGL 330
* OpenGL 450
* OpenGL ES 2.0
* OpenGL ES 3.0
* OpenGL ES 3.2

贡献
------
这个项目还在开发中，您可以请求合并提交代码，或者通过电子邮件随时与我们联系。 欢迎向我们提出任何问题或好的建议。 我们非常感谢您的贡献。

[License (MIT)](https://github.com/ray-cast/RabbitToolbox/blob/master/LICENSE)
-------------------------------------------------------------------------------
	Copyright (C) 2018-2019 Octoon Developers. All rights reserved.

	https://github.com/octoon/octoon

	Permission is hereby granted, free of charge, to any person obtaining a
	copy of this software and associated documentation files (the "Software"),
	to deal in the Software without restriction, including without limitation
	the rights to use, copy, modify, merge, publish, distribute, sublicense,
	and/or sell copies of the Software, and to permit persons to whom the
	Software is furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included
	in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
	OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
	BRIAN PAUL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
	AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
	CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
