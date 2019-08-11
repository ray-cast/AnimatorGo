#!/usr/bin/env bash

# Make sure the various variables that we need are set

export GW_DEPS_ROOT="$PWD""/../"

[ -z "$GW_DEPS_ROOT" ] && echo "GW_DEPS_ROOT not defined." && exit 1;


export PX_OUTPUT_ROOT="$PWD"


# Install cmake using packman
export PACKMAN="scripts/packman/packman.sh"
source $PACKMAN pull -p mac "scripts/packman/packages/cmake.packman.xml" || exit 1
export CMAKE="$PM_cmake_PATH/bin/cmake"


# Build projects here
cd compiler/ios-cmake/
$CMAKE --build . --target ALL_BUILD --clean-first --config release -- -parallelizeTargets -jobs 5 || exit 1
cd ../../

cd compiler/ios-cmake/
$CMAKE --build . --target ALL_BUILD --clean-first --config debug -- -parallelizeTargets -jobs 5 || exit 1
cd ../../

