@echo off

CD /D %~dp0
echo "Note: You need to run this with admin rights for the first time to set GW_DEPS_ROOT globally."
call "./scripts/locate_gw_root.bat" GW_DEPS_ROOT_F
@echo on
setx GW_DEPS_ROOT "%GW_DEPS_ROOT_F%
echo GW_DEPS_ROOT = %GW_DEPS_ROOT%
call "./scripts/locate_cmake.bat" CMAKE_PATH_F
echo CMAKE_PATH_F = %CMAKE_PATH_F%


SET PATH=%PATH%;"%CMAKE_PATH_F%"

REM Make sure the various variables that we need are set

call "./scripts/locate_cuda.bat" CUDA_PATH_
echo CUDA_PATH_ = %CUDA_PATH_%

IF EXIST %~dp0..\Externals\CMakeModules (
	set GW_DEPS_ROOT=%~dp0..\
)

IF NOT DEFINED GW_DEPS_ROOT GOTO GW_DEPS_ROOT_UNDEFINED

set PX_OUTPUT_ROOT=%~dp0

REM Generate projects here

rmdir /s /q compiler\vc11win32-cmake\
mkdir compiler\vc11win32-cmake\
pushd compiler\vc11win32-cmake\
cmake ..\cmake\windows -G "Visual Studio 11 2012" -AWin32 -DTARGET_BUILD_PLATFORM=Windows -DPX_GENERATE_GPU_PROJECTS=1 -DCUDA_TOOLKIT_ROOT_DIR="%CUDA_PATH_%" -DSTATIC_WINCRT=1 -DPX_OUTPUT_DLL_DIR=%PX_OUTPUT_ROOT%\Bin\vc11win32-cmake -DPX_OUTPUT_LIB_DIR=%PX_OUTPUT_ROOT%\Lib\vc11win32-cmake -DPX_OUTPUT_EXE_DIR=%PX_OUTPUT_ROOT%\Bin\vc11win32-cmake
popd

rmdir /s /q compiler\vc11win64-cmake\
mkdir compiler\vc11win64-cmake\
pushd compiler\vc11win64-cmake\
cmake ..\cmake\windows -G "Visual Studio 11 2012" -Ax64 -DTARGET_BUILD_PLATFORM=Windows -DPX_GENERATE_GPU_PROJECTS=1 -DCUDA_TOOLKIT_ROOT_DIR="%CUDA_PATH_%" -DSTATIC_WINCRT=1 -DPX_OUTPUT_DLL_DIR=%PX_OUTPUT_ROOT%\Bin\vc11win64-cmake -DPX_OUTPUT_LIB_DIR=%PX_OUTPUT_ROOT%\Lib\vc11win64-cmake -DPX_OUTPUT_EXE_DIR=%PX_OUTPUT_ROOT%\Bin\vc11win64-cmake
popd


rmdir /s /q compiler\vc12win32-cmake\
mkdir compiler\vc12win32-cmake\
pushd compiler\vc12win32-cmake\
cmake ..\cmake\windows -G "Visual Studio 12 2013" -AWin32 -DTARGET_BUILD_PLATFORM=Windows -DPX_GENERATE_GPU_PROJECTS=1 -DCUDA_TOOLKIT_ROOT_DIR="%CUDA_PATH_%" -DSTATIC_WINCRT=1 -DPX_OUTPUT_DLL_DIR=%PX_OUTPUT_ROOT%\Bin\vc12win32-cmake -DPX_OUTPUT_LIB_DIR=%PX_OUTPUT_ROOT%\Lib\vc12win32-cmake -DPX_OUTPUT_EXE_DIR=%PX_OUTPUT_ROOT%\Bin\vc12win32-cmake
popd

rmdir /s /q compiler\vc12win64-cmake\
mkdir compiler\vc12win64-cmake\
pushd compiler\vc12win64-cmake\
cmake ..\cmake\windows -G "Visual Studio 12 2013" -Ax64 -DTARGET_BUILD_PLATFORM=Windows -DPX_GENERATE_GPU_PROJECTS=1 -DCUDA_TOOLKIT_ROOT_DIR="%CUDA_PATH_%" -DSTATIC_WINCRT=1 -DPX_OUTPUT_DLL_DIR=%PX_OUTPUT_ROOT%\Bin\vc12win64-cmake -DPX_OUTPUT_LIB_DIR=%PX_OUTPUT_ROOT%\Lib\vc12win64-cmake -DPX_OUTPUT_EXE_DIR=%PX_OUTPUT_ROOT%\Bin\vc12win64-cmake
popd

rmdir /s /q compiler\vc14win32-cmake\
mkdir compiler\vc14win32-cmake\
pushd compiler\vc14win32-cmake\
cmake ..\cmake\windows -G "Visual Studio 14 2015" -AWin32 -DTARGET_BUILD_PLATFORM=Windows -DPX_GENERATE_GPU_PROJECTS=1 -DCUDA_TOOLKIT_ROOT_DIR="%CUDA_PATH_%" -DSTATIC_WINCRT=1 -DPX_OUTPUT_DLL_DIR=%PX_OUTPUT_ROOT%\Bin\vc14win32-cmake -DPX_OUTPUT_LIB_DIR=%PX_OUTPUT_ROOT%\Lib\vc14win32-cmake -DPX_OUTPUT_EXE_DIR=%PX_OUTPUT_ROOT%\Bin\vc14win32-cmake
popd

rmdir /s /q compiler\vc14win64-cmake\
mkdir compiler\vc14win64-cmake\
pushd compiler\vc14win64-cmake\
cmake ..\cmake\windows -G "Visual Studio 14 2015" -Ax64 -DTARGET_BUILD_PLATFORM=Windows -DPX_GENERATE_GPU_PROJECTS=1 -DCUDA_TOOLKIT_ROOT_DIR="%CUDA_PATH_%" -DSTATIC_WINCRT=1 -DPX_OUTPUT_DLL_DIR=%PX_OUTPUT_ROOT%\Bin\vc14win64-cmake -DPX_OUTPUT_LIB_DIR=%PX_OUTPUT_ROOT%\Lib\vc14win64-cmake -DPX_OUTPUT_EXE_DIR=%PX_OUTPUT_ROOT%\Bin\vc14win64-cmake
popd


GOTO :End

:GW_DEPS_ROOT_UNDEFINED
ECHO GW_DEPS_ROOT has to be defined, pointing to the root of the dependency tree.
PAUSE
GOTO END

:BOOST_ROOT_UNDEFINED
ECHO BOOST_ROOT has to be defined, pointing to the root of your local Boost install.
PAUSE
GOTO END

:CUDA_ROOT_UNDEFINED
ECHO CUDA_BIN_PATH has to be defined, pointing to the bin folder of your local CUDA install.
PAUSE

:End
