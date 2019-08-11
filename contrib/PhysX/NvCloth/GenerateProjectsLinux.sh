#!/usr/bin/env bash

# Make sure the various variables that we need are set

export GW_DEPS_ROOT="$PWD""/../"

[ -z "$GW_DEPS_ROOT" ] && echo "GW_DEPS_ROOT not defined." && exit 1;


export PX_OUTPUT_ROOT="$PWD"


# Check for the right g++ version (we need at least 4.7 for c++11)
cur_ver="$(g++ -dumpversion)"
req_ver="4.7"
if [ "$(printf "$req_ver\n$cur_ver" | sort -V | head -n1)" == "$cur_ver" ] && [ "$cur_ver" != "$req_ver" ]; then
        echo "g++ version $cur_ver < $req_ver"
        if [ -f /usr/bin/g++-4.7 ]; then
            echo "Forcing cmake to use g++ 4.7"
            export CC=/usr/bin/gcc-4.7
            export CXX=/usr/bin/g++-4.7
        else
            echo "Unable to find any compatible g++ version. Exiting now"
            exit 1
        fi
 else
        echo "g++ version $cur_ver >= $req_ver"
 fi


# Install cmake using packman
export PACKMAN="scripts/packman/packman.sh"
source $PACKMAN pull -p linux "scripts/packman/packages/cmake.packman.xml" || exit 1
export CMAKE="$PM_cmake_PATH/bin/cmake"


# You can enable/disable cuda support here
export USE_CUDA=0


# Generate projects here

rm -r -f compiler/linux64-debug-cmake/
mkdir compiler/linux64-debug-cmake/
cd compiler/linux64-debug-cmake/
$CMAKE ../cmake/linux -G "Unix Makefiles" -DTARGET_BUILD_PLATFORM=linux -DCMAKE_BUILD_TYPE=debug -DNV_CLOTH_ENABLE_CUDA=$USE_CUDA -DPX_GENERATE_GPU_PROJECTS=0 -DPX_OUTPUT_DLL_DIR=$PX_OUTPUT_ROOT/bin/linux64-cmake -DPX_OUTPUT_LIB_DIR=$PX_OUTPUT_ROOT/lib/linux64-cmake -DPX_OUTPUT_EXE_DIR=$PX_OUTPUT_ROOT/bin/linux64-cmake || exit 1
cd ../../

rm -r -f compiler/linux64-checked-cmake/
mkdir compiler/linux64-checked-cmake/
cd compiler/linux64-checked-cmake/
$CMAKE ../cmake/linux -G "Unix Makefiles" -DTARGET_BUILD_PLATFORM=linux -DCMAKE_BUILD_TYPE=checked -DNV_CLOTH_ENABLE_CUDA=$USE_CUDA -DPX_GENERATE_GPU_PROJECTS=0 -DPX_OUTPUT_DLL_DIR=$PX_OUTPUT_ROOT/bin/linux64-cmake -DPX_OUTPUT_LIB_DIR=$PX_OUTPUT_ROOT/lib/linux64-cmake -DPX_OUTPUT_EXE_DIR=$PX_OUTPUT_ROOT/bin/linux64-cmake || exit 1
cd ../../

rm -r -f compiler/linux64-profile-cmake/
mkdir compiler/linux64-profile-cmake/
cd compiler/linux64-profile-cmake/
$CMAKE ../cmake/linux -G "Unix Makefiles" -DTARGET_BUILD_PLATFORM=linux -DCMAKE_BUILD_TYPE=profile -DNV_CLOTH_ENABLE_CUDA=$USE_CUDA -DPX_GENERATE_GPU_PROJECTS=0 -DPX_OUTPUT_DLL_DIR=$PX_OUTPUT_ROOT/bin/linux64-cmake -DPX_OUTPUT_LIB_DIR=$PX_OUTPUT_ROOT/lib/linux64-cmake -DPX_OUTPUT_EXE_DIR=$PX_OUTPUT_ROOT/bin/linux64-cmake || exit 1
cd ../../

rm -r -f compiler/linux64-release-cmake/
mkdir compiler/linux64-release-cmake/
cd compiler/linux64-release-cmake/
$CMAKE ../cmake/linux -G "Unix Makefiles" -DTARGET_BUILD_PLATFORM=linux -DCMAKE_BUILD_TYPE=release -DNV_CLOTH_ENABLE_CUDA=$USE_CUDA -DPX_GENERATE_GPU_PROJECTS=0 -DPX_OUTPUT_DLL_DIR=$PX_OUTPUT_ROOT/bin/linux64-cmake -DPX_OUTPUT_LIB_DIR=$PX_OUTPUT_ROOT/lib/linux64-cmake -DPX_OUTPUT_EXE_DIR=$PX_OUTPUT_ROOT/bin/linux64-cmake || exit 1
cd ../../
