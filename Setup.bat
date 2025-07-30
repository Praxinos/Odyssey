@REM IDDN.FR.001.060015.014.S.X.2019.000.00000
@REM ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2025

@echo off
setlocal
pushd "%~dp0"

py Setup.py
if %ERRORLEVEL% NEQ 0 goto error

@REM Done!
goto :end

@REM Error happened. Wait for a keypress before quitting.
:error
pause

:end
popd
