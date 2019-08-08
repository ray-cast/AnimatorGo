#!/bin/sh +x

# Make sure the various variables that we need are set

export GW_DEPS_ROOT="$PWD""/../../../../../"

if [ -e $PWD"/../Externals/CMakeModules" ]; then
	export GW_DEPS_ROOT="$PWD""/../"
fi

[ -z "$GW_DEPS_ROOT" ] && echo "GW_DEPS_ROOT not defined." && exit 1;


export PX_OUTPUT_ROOT="$PWD"


# Generate projects here

rm -r -f compiler/osx32-cmake/
mkdir compiler/osx32-cmake/
cd compiler/osx32-cmake/
cmake ../cmake/Mac -G Xcode -DTARGET_BUILD_PLATFORM=Mac -DPX_32BIT=1 -DPX_OUTPUT_LIB_DIR=$PX_OUTPUT_ROOT/Lib/osx32-cmake 
cd ../../

rm -r -f compiler/osx64-cmake/
mkdir compiler/osx64-cmake/
cd compiler/osx64-cmake/
cmake ../cmake/Mac -G Xcode -DTARGET_BUILD_PLATFORM=Mac -DPX_OUTPUT_LIB_DIR=$PX_OUTPUT_ROOT/Lib/osx64-cmake 
cd ../../



