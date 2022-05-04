Flowers
======
###### [English](https://github.com/ray-cast/RabbitToolbox/blob/master/README_en.md) &nbsp; [中文](https://github.com/ray-cast/RabbitToolbox/blob/master/README_en.md)
Rabbit Toolbox 是一个开源的商业动画渲染软件，本项目源码采用[知识共享署名-非商业性使用-相同方式共享 4.0 国际许可协议](http://creativecommons.org/licenses/by-nc-sa/4.0/)进行许可。提倡以简单、易用，高质量的物理演算以及渲染质量和性能。为喜爱二次元动画制作的爱好者降低视频制作门槛，用户可以轻松的迁移 [MikuMikuDance](http://www.geocities.jp/higuchuu4/index_e.htm) 的项目到本软件中制作成视频，同时编码为各大门户视频网站支持的视频编码格式。

Rabbit Toolbox 提供了一个现代化和强大的ECS框架，使该项目开发变得简单。 系统以ECS的形式支持许多功能，如：前向和逆向运动学的骨骼动画，实时渲染，全局照明渲染器，AI降噪，物理演算，布料演算，音频播放和视频输出。 通过附加各种组件，对象可以灵活地执行不同的任务。 

Rabbit Toolbox 拥有一个快速高效的全局照明渲染器，基于分离内核架构以避免VGPR占用瓶颈，并广泛使用GPU优化的并行图元来重构工作，以更好地适应大规模并行GPU架构，渲染器是为渐进式预览设计的，具有同步性，允许在进行渲染时控制延迟并操纵场景和摄影机。 在所有供应商中保持着高水平的性能，全局照明渲染器使用OpenCL依赖于 AMD® [RadeonRays](https://github.com/GPUOpen-LibrariesAndSDKs/RadeonRays_SDK) 光线求交引擎，同时针对AMD®GPU和APU进行了优化。

Rabbit Toolbox 的多层材质描述基于双向散射分布函数（bsdf），能够很好的引入[NVIDIA's Material Definition Language](https://developer.nvidia.com/mdl-sdk)材质定义语言所带来的生态系统，如程序纹理，木材，织物，半透明塑料等。

<img style="vertical-align: top;" src="./screenshot.png">
<img style="vertical-align: top;" src="./record.gif">
<img style="vertical-align: top;" src="./coffee_maker.png">
<img style="vertical-align: top;" src="./ajax.png">
<img style="vertical-align: top;" src="./staircase.png">

特性
------
* 完整的多层物理材质描述（金属，光滑，布料，清漆，次表面散射，自发光，等）
* 基于重要性采样的图像照明
* 物理的灯光单元
* 相机景深控制
* 多光源支持
* 基于物理的实时渲染
* 全局照明光线追踪渲染（OpenCL，[AMD® RadeonRays](https://github.com/GPUOpen-LibrariesAndSDKs/RadeonRays_SDK)）
* 基于深度学习的后处理降噪 ([Intel® Open Image Denoise](https://www.openimagedenoise.org/))
* 前向、逆向运动学的骨骼动画（Cyclic Coordinate Descent Method）
* 物理演算（PhysX）
* 布料演算（NVCloth）
* 模型文件支持（PMX、PMD，ABC）
* 动画文件支持（VMD）
* 场景文件支持（PMM）
* 视频编码（x264、x265）
* 图像文件（PNG、BMP、TGA、JPEG、DDS、HDR）
* 音频播放（OpenAL、Wav、Flac、Mp3、Ogg）
* 材质文件支持（[NVIDIA's Material Definition Language](https://developer.nvidia.com/mdl-sdk)）

下载
------
* https://github.com/ray-cast/flowers/releases/download/Demo/Demo.zip

系统需求
------
1. 支持SSE4.1的CPU，如i5，用于AI 降噪
2. NVIDIA的独显用于GPGPU的光线追踪 
3. OpenCL 驱动 
4. Visual Studio 2019 Runtimes (软件根目录/_Redist/VC_redist.x64.exe)
6. 参与测试过的CPU，Intel i3, i5, i7, AMD 2600, 2600X
7. 参与测试过的显卡，GeFroce GTX 650, 1050, 1060, 2060

支持的编译器
------
* Visual Studio 2017 (Only tested on 15.5.7 version)
* GNU GCC (Only tested on 5.4.0, 6.2.0, 7.3.0, 8.1.0 version)
* Clang & LLVM (Only tested on 5.0.0 version)
* Emscripten (Only tested on 1.37.40)
* Vcpkg (2020-11-1)

支持的平台
------
* Windows (Only tested on XP, Vista, 7, 8, 10)
* Linux (Only tested on Ubuntu, CentOS)
* HTML5
* Android (future)
* ios (Requires : automake autoconf)
* mac (future)

支持的渲染后端
------
* OpenGL 3.3
* OpenGL 4.5
* OpenGL ES 2.0
* OpenGL ES 3.0
* OpenGL ES 3.2

申明
-----
封面素体模型来至 @石英世界，如有侵犯请告知

贡献
------
这个项目还在开发中，您可以请求合并提交代码，或者通过电子邮件随时与我们联系。 欢迎向我们提出任何问题或好的建议。 我们非常感谢您的贡献。

[协议](https://github.com/ray-cast/RabbitToolbox/blob/master/LICENSE)
-------------------------------------------------------------------------------
[![img](https://i.creativecommons.org/l/by-nc-sa/4.0/80x15.png)](http://creativecommons.org/licenses/by-nc-sa/4.0/)
本作品采用[知识共享署名-非商业性使用-相同方式共享 4.0 国际许可协议](http://creativecommons.org/licenses/by-nc-sa/4.0/)进行许可。

	您可以自由地：
	共享 — 在任何媒介以任何形式复制、发行本作品
	演绎 — 修改、转换或以本作品为基础进行创作

	惟须遵守下列条件：
	署名 — 您必须给出适当的署名，提供指向本许可协议的链接，同时标明是否（对原始作品）作了修改。您可以用任何合理的方式来署名，但是不得以任何方式暗示许可人为您或您的使用背书。
	非商业性使用 — 您不得将本作品用于商业目的。
	相同方式共享 — 如果您再混合、转换或者基于本作品进行创作，您必须基于与原先许可协议相同的许可协议 分发您贡献的作品。
	没有附加限制 — 您不得适用法律术语或者 技术措施 从而限制其他人做许可协议允许的事情。

	声明：
	您不必因为公共领域的作品要素而遵守许可协议，或者您的使用被可适用的 例外或限制所允许。
	不提供担保。许可协议可能不会给与您意图使用的所必须的所有许可。例如，其他权利比如形象权、隐私权或人格权可能限制您如何使用作品。
