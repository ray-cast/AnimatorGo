#!/bin/sh +x

# Make sure the various variables that we need are set

export GW_DEPS_ROOT="$PWD""/../../../../../"

if [ -e $PWD"/../Externals/CMakeModules" ]; then
	export GW_DEPS_ROOT="$PWD""/../"
fi

[ -z "$GW_DEPS_ROOT" ] && echo "GW_DEPS_ROOT not defined." && exit 1;


export PX_OUTPUT_ROOT="$PWD"


# Generate projects here

rm -r -f compiler/linux64-debug-cmake/
mkdir compiler/linux64-debug-cmake/
cd compiler/linux64-debug-cmake/
cmake ../cmake/Linux -G "Unix Makefiles" -DTARGET_BUILD_PLATFORM=Linux -DCMAKE_BUILD_TYPE=debug -DPX_GENERATE_GPU_PROJECTS=0 -DPX_OUTPUT_DLL_DIR=$PX_OUTPUT_ROOT/bin/linux64-cmake -DPX_OUTPUT_LIB_DIR=$PX_OUTPUT_ROOT/lib/linux64-cmake -DPX_OUTPUT_EXE_DIR=$PX_OUTPUT_ROOT/bin/linux64-cmake
cd ../../

rm -r -f compiler/linux64-checked-cmake/
mkdir compiler/linux64-checked-cmake/
cd compiler/linux64-checked-cmake/
cmake ../cmake/Linux -G "Unix Makefiles" -DTARGET_BUILD_PLATFORM=Linux -DCMAKE_BUILD_TYPE=checked -DPX_GENERATE_GPU_PROJECTS=0 -DPX_OUTPUT_DLL_DIR=$PX_OUTPUT_ROOT/bin/linux64-cmake -DPX_OUTPUT_LIB_DIR=$PX_OUTPUT_ROOT/lib/linux64-cmake -DPX_OUTPUT_EXE_DIR=$PX_OUTPUT_ROOT/bin/linux64-cmake
cd ../../

rm -r -f compiler/linux64-profile-cmake/
mkdir compiler/linux64-profile-cmake/
cd compiler/linux64-profile-cmake/
cmake ../cmake/Linux -G "Unix Makefiles" -DTARGET_BUILD_PLATFORM=Linux -DCMAKE_BUILD_TYPE=profile -DPX_GENERATE_GPU_PROJECTS=0 -DPX_OUTPUT_DLL_DIR=$PX_OUTPUT_ROOT/bin/linux64-cmake -DPX_OUTPUT_LIB_DIR=$PX_OUTPUT_ROOT/lib/linux64-cmake -DPX_OUTPUT_EXE_DIR=$PX_OUTPUT_ROOT/bin/linux64-cmake
cd ../../

rm -r -f compiler/linux64-release-cmake/
mkdir compiler/linux64-release-cmake/
cd compiler/linux64-release-cmake/
cmake ../cmake/Linux -G "Unix Makefiles" -DTARGET_BUILD_PLATFORM=Linux -DCMAKE_BUILD_TYPE=release -DPX_GENERATE_GPU_PROJECTS=0 -DPX_OUTPUT_DLL_DIR=$PX_OUTPUT_ROOT/bin/linux64-cmake -DPX_OUTPUT_LIB_DIR=$PX_OUTPUT_ROOT/lib/linux64-cmake -DPX_OUTPUT_EXE_DIR=$PX_OUTPUT_ROOT/bin/linux64-cmake
cd ../../