:: Use our local packman module for testing
@set PM_MODULE_EXT=%~dp0\..\common\packman.py
@call "%~dp0packman.cmd" %*
:: Clean out so this setting doesn't leak into the current console
@set PM_MODULE_EXT=
@if errorlevel 1 exit /b 1
