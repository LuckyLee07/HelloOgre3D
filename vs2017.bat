@echo off
set SCRIPT_DIRECTORY=%~dp0
set CURRENT_DIRECTORY=%cd%
set ARGUMENTS=%*
set PREMAKE_ARGUMENTS=%ARGUMENTS%

if "%ENABLE_FGUI%"=="1" set PREMAKE_ARGUMENTS=--with-fairygui %PREMAKE_ARGUMENTS%

cd /d %SCRIPT_DIRECTORY%
rmdir /S/Q build
rmdir /S/Q lib

cd bin
del /S/Q *.log *.pdb *.exe *.idb
cd ..

set ENABLE_FGUI=1
rem set ENABLE_TRACY=1

echo on
tools\premake\premake5 --os=windows --file=premake/premake.lua vs2017 %PREMAKE_ARGUMENTS%

cd /d %CURRENT_DIRECTORY%

pause
