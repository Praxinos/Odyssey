@echo off

:: Ensure local scope
setlocal

:: Ensure current dir
pushd %~dp0

:: CD to the plugin root: the paths of the Doxyfile are relative to it
cd ..\..

doxygen %~dp0Doxyfile
