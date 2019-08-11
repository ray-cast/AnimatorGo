@echo off
setlocal enabledelayedexpansion


rem use the following 2 lines to manually point to the root folder
set FOLDER=%~dp0/
goto :manualfolder

set FOLDER_NEEDLE=sw\physx
rem set FOLDER_NEEDLE=NvCloth\src

call "%~dp0/locate.bat" FOLDER %FOLDER_NEEDLE%
:manualfolder
set returnVal=%FOLDER%\..\..\..\..\..\..\..\

( endlocal
    set "%~1=%returnVal%"
)
goto :eof
