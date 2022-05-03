if exist build (
  cd build
) else (
  mkdir build && cd build
)
conan install .. --build=qt
cmake .. -G "Visual Studio 16"
cmake --build . --config Release
cd ..