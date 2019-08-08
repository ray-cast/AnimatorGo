@echo off
setlocal enabledelayedexpansion
SET /P AREYOUSURE=Are you sure you want to delete all generated/compiled files (Y/[N])?
IF /I "%AREYOUSURE%" NEQ "Y" GOTO EOF

rd .\bin /s /q
md .\bin
rd .\lib /s /q
md .\lib
rd .\RenderCloth\bin /s /q
md .\RenderCloth\bin
rd .\test\bin /s /q
md .\test\bin
rd .\test\lib /s /q
md .\test\lib

for /d %%G in (".\compiler\vc*") do (
rd %%G /s /q
)

for /d %%G in (".\test\compiler\vc*") do (
rd %%G /s /q
)

for /d %%G in (".\RenderCloth\compiler\vc*") do (
rd %%G /s /q
)

pause
