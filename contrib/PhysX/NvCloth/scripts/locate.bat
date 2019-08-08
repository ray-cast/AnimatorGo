@echo off
setlocal enabledelayedexpansion

set FOLDER_NEEDLE=%~2
echo looking for ...\%FOLDER_NEEDLE%\

set FOLDER=%~dp0
set LOOP_COUNT=1
:rootloop
if exist "%FOLDER%%FOLDER_NEEDLE%" goto :haverootpath
set FOLDER=%FOLDER%..\
set /a LOOP_COUNT=%LOOP_COUNT%+1
if %LOOP_COUNT% GTR 50 (goto :error)
goto :rootloop

:haverootpath
set FOLDER=%FOLDER%\%FOLDER_NEEDLE%\
pushd %FOLDER%
set FOLDER=%CD%
popd
echo Found %FOLDER%
:manualfolder
set returnVal=%FOLDER%
goto :returnpath


:error
echo Couldn't find folder ...\%FOLDER_NEEDLE%\
set returnVal=error\%FOLDER_NEEDLE%
pause
goto :returnpath


:returnpath

( endlocal
    set "%~1=%returnVal%"
)
goto :eof