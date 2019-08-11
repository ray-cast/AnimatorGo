@echo off
set EXIT_CODE=0

REM Make sure the various variables that we need are set
echo "Note: You need to run this with admin rights for the first time to set GW_DEPS_ROOT globally."
REM single " is not a mistake
setx GW_DEPS_ROOT "%GW_DEPS_ROOT%
REM this one is for local use
set GW_DEPS_ROOT=%GW_DEPS_ROOT%
echo GW_DEPS_ROOT = %GW_DEPS_ROOT%

IF NOT DEFINED GW_DEPS_ROOT GOTO GW_DEPS_ROOT_UNDEFINED

set PX_OUTPUT_ROOT=%~dp0

REM Install cmake using packman
set PACKMAN=call scripts/packman/packman.cmd
%PACKMAN% pull -p windows "scripts/packman/packages/cmake.packman.xml"
IF %ERRORLEVEL% NEQ 0 (
    set EXIT_CODE=%ERRORLEVEL%
    GOTO :End
)
set CMAKE=%PM_cmake_PATH%/bin/cmake.exe

REM Generate projects here

:: 1. Use system NDK
IF %ANDROID_NDK_ROOT%. EQU . (
	echo The ANDROID_NDK_ROOT environment variable is not set. Please set it to point to NDK root folder
	set EXIT_CODE=1
	GOTO :End
)
set "NDK=%ANDROID_NDK_ROOT:\=/%"
set "NDK_MAKE=%NDK%/prebuilt/windows-x86_64/bin/make.exe"
:: 2. Use NDK from physx externals
REM set "PHYSX_ROOT=%GW_DEPS_ROOT:\=/%sw/physx"
REM set "NDK=%PHYSX_ROOT%/externals/android-ndk/r13b-win32"
REM set "NDK_MAKE=%NDK%/prebuilt/windows/bin/make.exe"
::

:: IF [%1] == [] 
set CONFIG_NAME=%1
IF %CONFIG_NAME%. EQU debug. (
	GOTO :Build
)
IF %CONFIG_NAME%. EQU profile. (
	GOTO :Build
)
IF %CONFIG_NAME%. EQU checked. (
	GOTO :Build
)
IF %CONFIG_NAME%. EQU release. (
	GOTO :Build
)
echo "Usage CmakeGenerateAndroid.bat <build_configuration:debug|profile|checked|release>"
set EXIT_CODE=1
GOTO :End

:Build
set ANDROID_API=21
REM set ANDROID_ABI="armeabi-v7a with NEON"
REM set ANDROID_ABI="armeabi-v7a"
set ANDROID_ABI="arm64-v8a"
set ANDROID_DIR_NAME=android-%ANDROID_ABI%-%CONFIG_NAME%-cmake

rmdir /s /q compiler\%ANDROID_DIR_NAME%\
mkdir compiler\%ANDROID_DIR_NAME%\
pushd compiler\%ANDROID_DIR_NAME%\

REM https://cmake.org/cmake/help/v3.7/manual/cmake-toolchains.7.html#cross-compiling-for-android-with-nvidia-nsight-tegra-visual-studio-edition
REM https://developer.android.com/ndk/guides/cmake.html

REM REM Common cmd line params
set CMAKE_COMMON_PARAMS=-DANDROID_NDK=%NDK% -DANDROID_ABI=%ANDROID_ABI% -DGW_DEPS_ROOT=%GW_DEPS_ROOT% -DPX_OUTPUT_DLL_DIR=%PX_OUTPUT_ROOT%\Bin\%ANDROID_DIR_NAME% -DPX_OUTPUT_LIB_DIR=%PX_OUTPUT_ROOT%\Lib\%ANDROID_DIR_NAME% -DPX_OUTPUT_EXE_DIR=%PX_OUTPUT_ROOT%\Bin\%ANDROID_DIR_NAME%

REM Compiler dependent cmd line params
set CMAKE_COMPILER_PARAMS=-G "MinGW Makefiles" -DCMAKE_MAKE_PROGRAM=%NDK_MAKE% -DCMAKE_BUILD_TYPE=%CONFIG_NAME% -DCMAKE_ANDROID_ARCH_ABI=%ANDROID_ABI%
REM set CMAKE_COMPILER_PARAMS=-G "Visual Studio 14 2015" -DCMAKE_ANDROID_ARCH=%ANDROID_ABI%

REM Toolchain dependent cmd line params
REM set CMAKE_TOOLCHAIN_PARAMS=-DCMAKE_TOOLCHAIN_FILE=C:\Users\lpanov\projects\nvidia\NvCloth\Externals\CMakeModules\Android\android.toolchain.cmake -DANDROID_NATIVE_API_LEVEL=android-%ANDROID_API% -DANDROID_ABI=%ANDROID_ABI% -DANDROID_STL="gnustl_static" -DTARGET_BUILD_PLATFORM=android
set CMAKE_TOOLCHAIN_PARAMS=-DCMAKE_SYSTEM_NAME=Android -DCMAKE_ANDROID_API_MIN=%ANDROID_API% -DCMAKE_ANDROID_API=%ANDROID_API%

%CMAKE% ..\cmake\android %CMAKE_COMPILER_PARAMS%  %CMAKE_TOOLCHAIN_PARAMS%  %CMAKE_COMMON_PARAMS%
IF %ERRORLEVEL% NEQ 0 (
	set EXIT_CODE=%ERRORLEVEL%
	GOTO :End
)

REM reference cmd line
REM cmake ..\cmake\android -G "MinGW Makefiles" -DANDROID_NDK=%NDK% -DCMAKE_MAKE_PROGRAM=%NDK_MAKE% -DTARGET_BUILD_PLATFORM=android -DCMAKE_BUILD_TYPE=%CONFIG_NAME% -DCMAKE_SYSTEM_NAME=Android -DCMAKE_ANDROID_API_MIN=%ANDROID_API% -DCMAKE_ANDROID_API=%ANDROID_API% -DCMAKE_ANDROID_ARCH=%ANDROID_ABI% -DCMAKE_ANDROID_ARCH_ABI=%ANDROID_ABI% -DANDROID_ABI=%ANDROID_ABI% -DCMAKE_ANDROID_GUI=FALSE -DPX_GENERATE_GPU_PROJECTS=0 -DPX_OUTPUT_DLL_DIR=%PX_OUTPUT_ROOT%\Bin\%ANDROID_DIR_NAME%-cmake -DPX_OUTPUT_LIB_DIR=%PX_OUTPUT_ROOT%\Lib\%ANDROID_DIR_NAME%-cmake -DPX_OUTPUT_EXE_DIR=%PX_OUTPUT_ROOT%\Bin\%ANDROID_DIR_NAME%-cmake

REM To build the project, go to compiler/%ANDROID_DIR_NAME% and run
REM %ANDROID_NDK_ROOT%\prebuilt\windows-x86_64\bin\make.exe 

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

:End
IF /I "%EXIT_CODE%" NEQ "0" (
   echo The error code is %EXIT_CODE%
   exit /b %EXIT_CODE%
)

echo Configuration succeeded!
exit /b 0