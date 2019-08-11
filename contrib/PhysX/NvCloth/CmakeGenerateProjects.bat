@echo off
REM Usage: CmakeGenerateProjects.bat <0|1:use_cuda (default is 1)> <0|1:use_dx11 (default is 1)>
set EXIT_CODE=0

REM Make sure the various variables that we need are set
CD /D %~dp0

REM Make sure the various variables that we need are set

call "./scripts/locate_cuda.bat" CUDA_PATH_
echo CUDA_PATH_ = %CUDA_PATH_%

set GW_DEPS_ROOT=%~dp0..\
echo "Note: You need to run this with admin rights for the first time to set GW_DEPS_ROOT globally."
REM single " is not a mistake
setx GW_DEPS_ROOT "%GW_DEPS_ROOT%
REM this one is for local use
set GW_DEPS_ROOT=%GW_DEPS_ROOT%
echo GW_DEPS_ROOT = %GW_DEPS_ROOT%

IF NOT DEFINED GW_DEPS_ROOT goto GW_DEPS_ROOT_UNDEFINED

set PX_OUTPUT_ROOT=%~dp0

REM handle optional command line arguments
set USE_CUDA=1
set USE_DX11=1
IF %1. NEQ . (set USE_CUDA=%1)
IF %2. NEQ . (set USE_DX11=%2)

REM Install cmake using packman
set PACKMAN=call scripts/packman/packman.cmd
%PACKMAN% pull -p windows "scripts/packman/packages/cmake.packman.xml"
IF %ERRORLEVEL% NEQ 0 (
    set EXIT_CODE=%ERRORLEVEL%
    goto End
)
set CMAKE=%PM_cmake_PATH%/bin/cmake.exe

REM Common for all generators
set CMAKE_COMMON_PARAMS=-DTARGET_BUILD_PLATFORM=windows -DNV_CLOTH_ENABLE_DX11=%USE_DX11% -DNV_CLOTH_ENABLE_CUDA=%USE_CUDA% -DPX_GENERATE_GPU_PROJECTS=1 -DCUDA_TOOLKIT_ROOT_DIR="%CUDA_PATH_%" -DWIN8SDK_PATH="%WIN8SDK_PATH%" -DSTATIC_WINCRT=1

REM Generate projects here

REM vc11 isn't officially supported
REM rmdir /s /q compiler\vc11win32-cmake\
REM mkdir compiler\vc11win32-cmake\
REM pushd compiler\vc11win32-cmake\
REM %CMAKE% ..\cmake\windows -G "Visual Studio 11 2012" -AWin32 %CMAKE_COMMON_PARAMS% -DPX_OUTPUT_DLL_DIR=%PX_OUTPUT_ROOT%\bin\vc11win32-cmake -DPX_OUTPUT_LIB_DIR=%PX_OUTPUT_ROOT%\lib\vc11win32-cmake -DPX_OUTPUT_EXE_DIR=%PX_OUTPUT_ROOT%\bin\vc11win32-cmake
REM popd

REM rmdir /s /q compiler\vc11win64-cmake\
REM mkdir compiler\vc11win64-cmake\
REM pushd compiler\vc11win64-cmake\
REM %CMAKE% ..\cmake\windows -G "Visual Studio 11 2012" -Ax64 %CMAKE_COMMON_PARAMS% -DPX_OUTPUT_DLL_DIR=%PX_OUTPUT_ROOT%\bin\vc11win64-cmake -DPX_OUTPUT_LIB_DIR=%PX_OUTPUT_ROOT%\lib\vc11win64-cmake -DPX_OUTPUT_EXE_DIR=%PX_OUTPUT_ROOT%\bin\vc11win64-cmake
REM popd

rmdir /s /q compiler\vc12win32-cmake\
mkdir compiler\vc12win32-cmake\
pushd compiler\vc12win32-cmake\
%CMAKE% ..\cmake\windows -G "Visual Studio 12 2013" -AWin32 %CMAKE_COMMON_PARAMS% -DPX_OUTPUT_DLL_DIR=%PX_OUTPUT_ROOT%\bin\vc12win32-cmake -DPX_OUTPUT_LIB_DIR=%PX_OUTPUT_ROOT%\lib\vc12win32-cmake -DPX_OUTPUT_EXE_DIR=%PX_OUTPUT_ROOT%\bin\vc12win32-cmake
popd

rmdir /s /q compiler\vc12win64-cmake\
mkdir compiler\vc12win64-cmake\
pushd compiler\vc12win64-cmake\
%CMAKE% ..\cmake\windows -G "Visual Studio 12 2013" -Ax64 %CMAKE_COMMON_PARAMS% -DPX_OUTPUT_DLL_DIR=%PX_OUTPUT_ROOT%\bin\vc12win64-cmake -DPX_OUTPUT_LIB_DIR=%PX_OUTPUT_ROOT%\lib\vc12win64-cmake -DPX_OUTPUT_EXE_DIR=%PX_OUTPUT_ROOT%\bin\vc12win64-cmake
popd

rmdir /s /q compiler\vc14win64-cmake\
mkdir compiler\vc14win64-cmake\
pushd compiler\vc14win64-cmake\
%CMAKE% ..\cmake\windows -G "Visual Studio 14 2015" -Ax64 %CMAKE_COMMON_PARAMS% -DPX_OUTPUT_DLL_DIR=%PX_OUTPUT_ROOT%\bin\vc14win64-cmake -DPX_OUTPUT_LIB_DIR=%PX_OUTPUT_ROOT%\lib\vc14win64-cmake -DPX_OUTPUT_EXE_DIR=%PX_OUTPUT_ROOT%\bin\vc14win64-cmake
popd

goto End


:GW_DEPS_ROOT_UNDEFINED
echo GW_DEPS_ROOT has to be defined, pointing to the root of the dependency tree.
set EXIT_CODE=1
pause
goto End

:CUDA_ROOT_UNDEFINED
echo CUDA_BIN_PATH has to be defined, pointing to the bin folder of your local CUDA install.
set EXIT_CODE=1
pause
goto End

:End
if /I "%EXIT_CODE%" EQU "1" (
   echo CmakeGenerateProjects.bat: the error code is %EXIT_CODE%
)
exit /b 0
