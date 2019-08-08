@echo off
setlocal enabledelayedexpansion

rem use the following 2 lines to manually point to the google test folder \\sw\%FOLDER_NEEDLE%
set FOLDER=C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v8.0
goto :manualfolder

set FOLDER_NEEDLE=physx\externals\CUDA\8.0.44
rem set FOLDER_NEEDLE=externals\CUDA\8.0.44

call "%~dp0/locate.bat" FOLDER %FOLDER_NEEDLE%
:manualfolder
set "returnVal=%FOLDER:\=/%"

( endlocal
    set "%~1=%returnVal%"
)
goto :eof