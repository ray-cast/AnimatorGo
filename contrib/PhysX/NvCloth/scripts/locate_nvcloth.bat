@echo off
setlocal enabledelayedexpansion


rem use the following 2 lines to manually point to the google test folder \\sw\%FOLDER_NEEDLE%
set FOLDER=%~dp0/../
goto :manualfolder

set FOLDER_NEEDLE=NvCloth

call "%~dp0/locate.bat" FOLDER %FOLDER_NEEDLE%
:manualfolder
set returnVal=%FOLDER%

( endlocal
    set "%~1=%returnVal%"
)
goto :eof