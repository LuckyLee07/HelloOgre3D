@echo off
set SCRIPT_DIRECTORY=%~dp0
set CURRENT_DIRECTORY=%cd%
set ARGUMENTS=%*

cd /d %SCRIPT_DIRECTORY%

if not exist "build" (
	mkdir build
) else (
	rem del /S /Q build\*.sln >nul 2>errors.log
	rem del /S /Q build\*.vcxproj >nul 2>errors.log
	:: 删除中间文件
	for /r build %%f in (*.obj *.pch *.idb *.pdb *.tlog *.lastbuildstate *.cache) do (
		del /F /Q "%%f" >nul 2>&1
	)
)

if exist bin (
	cd bin
		del /S/Q *.log *.pdb *.exe *.idb
	cd ..
)

echo on
tools\premake\premake5 --os=windows --file=premake/premake.lua vs2019 %ARGUMENTS%

cd /d %CURRENT_DIRECTORY%
