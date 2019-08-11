#!/bin/sh +x

# Make sure the various variables that we need are set

export GW_DEPS_ROOT="$PWD""/../"

[ -z "$GW_DEPS_ROOT" ] && echo "GW_DEPS_ROOT not defined." && exit 1;


export PX_OUTPUT_ROOT="$PWD"



# Install cmake using packman
export PACKMAN="./scripts/packman/packman.sh"
source $PACKMAN pull -p mac "./scripts/packman/packages/cmake.packman.xml" || exit 1
export CMAKE="$PM_cmake_PATH/bin/cmake"


# Cuda isn't supported on mac yet
export USE_CUDA=0


# Generate projects here

rm -r -f compiler/osx64-cmake/
mkdir compiler/osx64-cmake/
cd compiler/osx64-cmake/
$CMAKE ../cmake/mac -G Xcode -DTARGET_BUILD_PLATFORM=mac -DNV_CLOTH_ENABLE_CUDA=$USE_CUDA -DUSE_CUDA=$USE_CUDA -DPX_GENERATE_GPU_PROJECTS=0 -DPX_OUTPUT_DLL_DIR=$PX_OUTPUT_ROOT/bin/osx64-cmake -DPX_OUTPUT_LIB_DIR=$PX_OUTPUT_ROOT/lib/osx64-cmake -DPX_OUTPUT_EXE_DIR=$PX_OUTPUT_ROOT/bin/osx64-cmake || exit 1 
cd ../../

