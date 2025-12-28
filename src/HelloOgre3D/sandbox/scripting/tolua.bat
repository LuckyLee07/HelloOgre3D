@echo off

set CURR_DIR=%cd%

set PATH_TOLUA_EXE=%CURR_DIR%\..\..\..\..\tools\tolua++\tolua++.exe

echo ------------- sandbox tolua -------------
%PATH_TOLUA_EXE% -o SandboxToLua.cpp SandboxToLua.pkg

pause