# Summary

Baikal initiative has been started as a sample application demonstrating the usage of AMD® RadeonRays intersection engine, but evolved into a fully functional rendering engine aimed at graphics researchers, educational institutions and open-source enthusiasts in general.

Baikal is fast and efficient GPU-based global illumination renderer implemented using OpenCL and relying on AMD® RadeonRays intersection engine. It is cross-platform and vendor independent. The only requirement it imposes on the hardware is OpenCL 1.2 support. Baikal maintains high level of performance across all vendors, but it is specifically optimized for AMD® GPUs and APUs.

![Image](https://github.com/GPUOpen-LibrariesAndSDKs/RadeonProRender-Baikal/blob/master/Doc/Images/5.jpg)

# Build
## System requirements
The renderer is cross-platform and the following compilers are supported:

- Visual Studio 2015 and later

- Xcode 4 and later

- GCC 4.8 and later

- CMake 3.8 and later

The following packages are required to build the renderer:

- Python 2.7
- OpenImageIO (for MacOS/Linux builds)
- glew (for MacOS/Linux builds)
- GLFW3 (for MacOS/Linux builds)
- AMD APP SDK or CUDA SDK

## Build instructions

Baikal is using git submodules, use the following command line to recursively clone the repo including submodules:

```
git clone --recursive https://github.com/GPUOpen-LibrariesAndSDKs/RadeonProRender-Baikal.git
```

### Windows
- Create Visual Studio 2015 Solution

`cmake -G "Visual Studio 14 2015 Win64"`

### OSX

- Install Homebrew

`/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"`

- Install OpenImageIO

`brew install homebrew/science/openimageio`

- Install GLFW3

`brew install glfw3`

- Make build folder and generate make files

`mkdir build`

`cd build`

`cmake -DCMAKE_BUILD_TYPE=<Release or Debug> ..`

`make`

### Linux(Ubuntu)

- Install complementary libraries

`sudo apt-get install g++`

- Install build dependencies

`sudo apt-get install libopenimageio-dev libglew-dev libglfw3-dev`

- Create the Makefile and build

`mkdir build`

`cd build`

`cmake -DCMAKE_BUILD_TYPE=<Release ro Debug> ..`

`make`

### Options

Available premake options:

- `BAIKAL_ENABLE_DENOISER` enables EAW denoiser in interactive output:

- `BAIKAL_ENABLE_RPR` generates RadeonProRender API implemenatiton C-library and couple of RPR tutorials.

## Run

## Run Baikal standalone app
 - `export LD_LIBRARY_PATH=<RadeonProRender-Baikal path>/build/bin/:${LD_LIBRARY_PATH}`
 - `cd BaikalStandalone`
 - `../build/bin/BaikalStandalone`

Possible command line args:
- `-platform index` select specific OpenCL platform
- `-device index` select specific OpenCL device
- `-p path` path to mesh/material files
- `-f file` mesh file to render
- `-w` set window width
- `-h` set window height
- `-ns num` limit the number of samples per pixel
- `-cs speed` set camera movement speed
- `-cpx x -cpy y -cpz z` set camera position
- `-tpx x -tpy y -tpz z` set camera target
- `-interop [0|1]` disable | enable OpenGL interop (enabled by default, might be broken on some Linux systems)
- `-config [gpu|cpu|mgpu|mcpu|all]` set device configuration to run on: single gpu (default) | single cpu | all available gpus | all available cpus | all devices

The list of supported texture formats:

- png
- bmp
- jpg
- gif
- exr
- hdr
- tx
- dds (limited support)
- tga

You can download additional test scenes from [RadeonProRender-Tests](https://github.com/GPUOpen-LibrariesAndSDKs/RadeonProRender-Tests). Once extracted, you can load e.g. the Classroom example like this:

`../build/bin/BaikalStandalone -p /path/to/extracted/folder/Classroom/ -f classroom.obj`

The path can be absolute or relative to `BaikalStandalone`.

## Run BaikalDataGenerator
 - `export LD_LIBRARY_PATH=<RadeonProRender-Baikal path>/build/bin/:${LD_LIBRARY_PATH}`
 - `cd BaikalStandalone`
 - `../build/bin/BaikalStandalone`

Mandatory command line args:
- `-light_file` full path to config file with light description
- `-camera_file` full path to config file with camera states description
- `-spp_file` full path to config file with necessary sampling per pixel enumeration
- `-scene_file` full path to scene
- `-output_dir` directory to store generated dataset
- `-width` generated ouput width
- `-height` generated ouput height

Possible command line args:
- `-gamma` enables gamma corection for 3 chanel color output. '-gamma 1' means that gamma correction is enabled, otherwise disabled

## Run unit tests
- `export LD_LIBRARY_PATH=<RadeonProRender-Baikal path>/build/bin/:${LD_LIBRARY_PATH}`
 - `cd BaikalTest`
 - `../build/bin/BaikalTest`

Unit tests are producing test images into BaikalTest/OutputImages and compare them to reference images expected to be at BaikalTest/ReferenceImages.

Possible command line args:
- `-genref 1` generate reference images


# Hardware  support

The renderer has been tested on the following hardware and OSes:

## Linux
 - Ubuntu Linux 14.04
 - AMD FirePro driver 15.201: W9100, W8100, W9000, W7000, W7100, S9300x2, W5100
 - AMD Radeon driver 15.302: R9 Nano, R9 Fury X, R9 290
 - NVIDIA driver 352.79: GeForce GTX970, Titan X

## Windows
 - Windows 7/8.1/10
 - AMD FirePro driver 15.201: W9100, W8100, W9000, W7000, W7100, S9300x2, W5100
 - AMD Radeon driver 16.4: R9 Nano, R9 Fury X, R9 290, Pro Duo
 - NVIDIA driver 364.72: GeForce GTX970, Titan X

## OSX
 - OSX High Sierra
 - Mac Pro (Late 2013) AMD FirePro D500 x2
 - Macbook Pro Retina 13" (Early 2013) Intel HD 4300
 - Macbook Pro Retina 13" (2017) Intel Iris
 - Macbook 12" (Early 2015) Intel HD 5300

# Known Issues

## Windows
 
 - BaikalStandalone hangs sometimes on Windows on Nvidia cards. The issue is caused by ImGUI incompatibility with GL/CL interop. Removing ImGUI render call fixes the issue.
 
 ## Linux

 - If <CL/cl.h> is missing try to specify OpenCL SDK location.
 - If your are experiencing problems creating your CL context with a default config chances are CL-GL interop is broken on your system, try running the sample app with -interop 0 command line option (expect performance drop). 
 
AMD:
`export $AMDAPPSDKROOT=<SDK_PATH>`
NVIDIA:
`export $CUDA_PATH=<SDK_PATH>`

---
# Features
Being more of an experimental renderer, than a production rendering solution, Baikal still maintains a good set of features.

## Light transport

Baikal is essentially a biased path-tracer, however it is highly configurable and might be configured to run ray-tracing (sampling multiple light sources at once) instead of pure path tracing. Implementation of bidirectional path tracing solver is currently work in progress. 

## Geometry

To maintain high efficiency Baikal only supports triangle meshes and instances. However quads can be used while working with Baikal via RadeonPro Render API, since API layer is capable of pre-tessellating quads and passing triangle meshes down to Baikal. The size of triangle meshes is only limited by available GPU memory. Instancing can be used to greatly reduce memory footprint by reusing geometric data and acceleration structures.

Internally meshes and instances are represented using Shape interface. If used through RPR API meshes and instances are represented by rpr_shape opaque pointer type. 

## Materials

Baikal supports compound materials assembled from the following building blocks:
* Matte BRDF (Lambertian)
* Microfacet BRDF (GGX or Beckmann distributions)
* Ideal reflection BRDF
* Ideal refraction BTDF
* Microfacet refraction BTDF (GGX or Beckmann distributions)
* Transparent BTDF
* Translucent BTDF
* Emission

Each of these building blocks can have:
* Built-in Fresnel factor
* Normal map
* Bump map

Building blocks are combined together using one of the following blend modes:
* Mix – linear interpolation of components using fixed weight
* Fresnel blend – linear interpolation of components with weight depending on angle of incidence

Materials can use RGBA uint8, float16 or float32 uncompressed textures for albedos, weights and normal/bump maps.

Internally materials are represented as a subclasses of Material class: SingleBxdf for individual components and MultiBxdf for compound materials. At RPR API level they are represented by rpr_material_node opaque pointer. Note, that not all RPR materials are currently supported by Baikal (see section below on that). 

## Lights

Baikal supports the following types of lights:
* Point light
* Directional light
* Spot light
* Area light (emissive geometry)
* Image-based environment light

All the lights are internally represented by different subclasses of Light interface. If being used via RPR API, lights are represented by rpr_light opaque pointer.

## Sampling


Baikal can use one of the following samplers for random points/directions generation:

* Random sampler (less efficient, mainly for debugging)
* Sobol quasi- Monte-Carlo sampler
* Correlated multi-jittered sampler

In addition Baikal is using multiple importance sampling to reduce variance for both direct and indirect illumination. It also applies Russian roulette to terminate low-probability paths.

## GPU execution model	

Baikal is based on split-kernel architecture to avoid VGPR occupancy bottlenecks and broadly uses GPU-optimized parallel primitives to restructure the work to better fit massively parallel GPU architecture. First, the renderer is designed for progressive preview and has synchronous nature. Meaning it has Render function which is getting called by the user in the loop and every call to this function adds a single sample to each pixel refining the image. This model allows to keep the latency under control and manipulate the scene and the camera while doing rendering. 
Inside the Render function each OpenCL work item is assigned a single pixel, but as iterations(bounces) progress, less and less rays remain alive, so Render function compacts  the work to minimize GPU thread divergence.

![Image](https://github.com/GPUOpen-LibrariesAndSDKs/RadeonProRender-Baikal/blob/master/Doc/Images/4.png)

## Performance
In terms of latency the renderer is capable of maintaining high FPS while doing progressive rendering for moderately sized scenes. For example on the following “Science Fiction” scene (775K triangles) Baikal is producing 15 FPS in full HD resolution on R9 Nano card. 

In terms of intersection throughput performance is determined by underlying RadeonRays engine. Several examples using “Science Fiction” scene on R9 Fury X:


![Image](https://github.com/GPUOpen-LibrariesAndSDKs/RadeonProRender-Baikal/blob/master/Doc/Images/1.jpg)
“Science Fiction” scene is a courtesy of Juan Carlos Silva, 3drender.com.

Primary rays: 773Mrays/s (2.68ms)

Secondary rays: 285Mrays/s (7.27ms)

Shadow rays: 1109Mrays/s (1.87ms)


![Image](https://github.com/GPUOpen-LibrariesAndSDKs/RadeonProRender-Baikal/blob/master/Doc/Images/2.jpg)
“Science Fiction” scene is a courtesy of Juan Carlos Silva, 3drender.com.

Primary rays: 470Mrays/s (4.42ms)

Secondary rays: 195Mrays/s (10.66ms)

Shadow rays: 800Mrays/s (2.59ms)


![Image](https://github.com/GPUOpen-LibrariesAndSDKs/RadeonProRender-Baikal/blob/master/Doc/Images/3.jpg)
“Science Fiction” scene is a courtesy of Juan Carlos Silva, 3drender.com.

Primary rays: 562Mrays/s (3.69ms)

Secondary rays: 270Mrays/s (7.67ms)

Shadow rays: 1219Mrays/s (1.7ms)


# RadeonPro Render API support
We provide an implementation of RPR API with Baikal, which is still in an early state of development, so many features available in internal RPR core are not available in open-source back end. The list of unsupported features follow:

* Full material system (currently only basic BRDFs and blends of them are supported, no procedurals and arithmetic nodes)
* Volumetrics (currently work in progress in Baikal)
* IES lights
* Visibility flags
* Displacement and subdivision
* Tilt shift camera
* Bokeh shape controls
* Multiple UVs
* Post-processing
* Analytic sky system


