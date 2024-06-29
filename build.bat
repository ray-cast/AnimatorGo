if exist build (
  cd build
) else (
  mkdir build && cd build
)

conan remote add iteale http://conan.iteale.com:19479

conan install .. -g cmake_multi -s arch=x86_64 -s build_type=Debug -s compiler.runtime=MDd --build missing
conan install .. -g cmake_multi -s arch=x86_64 -s build_type=Release -s compiler.runtime=MD --build missing

if %errorlevel% == 0 (
  cmake .. -G "Visual Studio 16"
) else (
  goto ExitLabelFailure
)

if %errorlevel% == 0 (
  cmake --build . --config Debug
  cmake --build . --config Release
) else (
  goto ExitLabelFailure
)

if %errorlevel% == 0 (
  where makensis
  if %errorlevel% == 0 (
    makensis ../samples/unreal/nsis/install.nsi
  )
) else (
  goto ExitLabelFailure
)

:ExitLabelSuccess
cd ..
echo Success Compilation
goto EndLabel

:ExitLabelFailure
cd ..
echo Error Compilation
goto EndLabel

:EndLabel
pause