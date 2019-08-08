@echo off
setlocal enabledelayedexpansion

echo The cmake bin path is hardcoded in %~dp0 change this locally if you installed cmake somewere else.
set FOLDER=C:\Program Files\CMake\bin
goto :manualfolder

set FOLDER_NEEDLE=CMake\bin\cmake.exe

call "%~dp0/locate.bat" FOLDER %FOLDER_NEEDLE%
:manualfolder
set returnVal=%FOLDER%

( endlocal
    set "%~1=%returnVal%"
)
goto :eof