@echo off
setlocal enabledelayedexpansion


rem use the following 2 lines to manually point to the google test folder \\sw\%FOLDER_NEEDLE%
rem set FOLDER=E:\nx0\physx\PxShared\1.0\trunk
rem goto :manualfolder

set FOLDER_NEEDLE=physx\tools

call "%~dp0/locate.bat" FOLDER %FOLDER_NEEDLE%
:manualfolder
set returnVal=%FOLDER%

( endlocal
    set "%~1=%returnVal%"
)
goto :eof