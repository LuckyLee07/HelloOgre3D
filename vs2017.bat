@echo off
set SCRIPT_DIRECTORY=%~dp0
set CURRENT_DIRECTORY=%cd%
set ARGUMENTS=%*

cd /d %SCRIPT_DIRECTORY%
rmdir /S/Q build
rmdir /S/Q lib

cd bin
del /S/Q *.log *.pdb *.exe *.idb

cd ..

echo on
tools\premake\premake5 --os=windows --file=premake/premake.lua vs2017 %ARGUMENTS%

cd /d %CURRENT_DIRECTORY%

pause