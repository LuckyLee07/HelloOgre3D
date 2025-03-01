@REM Copyright (c) 2013 David Young dayoung@goliathdesigns.com
@REM 
@REM This software is provided 'as-is', without any express or implied
@REM warranty. In no event will the authors be held liable for any damages
@REM arising from the use of this software.
@REM 
@REM Permission is granted to anyone to use this software for any purpose,
@REM including commercial applications, and to alter it and redistribute it
@REM freely, subject to the following restrictions:
@REM 
@REM  1. The origin of this software must not be misrepresented; you must not
@REM  claim that you wrote the original software. If you use this software
@REM  in a product, an acknowledgment in the product documentation would be
@REM  appreciated but is not required.
@REM 
@REM  2. Altered source versions must be plainly marked as such, and must not be
@REM  misrepresented as being the original software.
@REM 
@REM  3. This notice may not be removed or altered from any source
@REM  distribution.
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
