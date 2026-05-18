@echo off
set SCRIPT_DIRECTORY=%~dp0
set CURRENT_DIRECTORY=%cd%
set ARGUMENTS=%*
set TRACY_ENABLE=1

cd /d %SCRIPT_DIRECTORY%
rem rmdir /S/Q build
rem rmdir /S/Q libs

cd bin
del /S/Q *.log *.pdb *.exe *.idb
cd ..

set ENABLE_FGUI=1
rem set ENABLE_TRACY=1

echo on
tools\premake\premake5 --os=windows --file=premake/premake.lua vs2022 %ARGUMENTS%

cd /d %CURRENT_DIRECTORY%

pause
