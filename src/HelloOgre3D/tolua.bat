@echo off
echo frameworks tolua s-------------

set PATH_TOLUA_EXE=..\..\tools\tolua++\tolua++.exe
%PATH_TOLUA_EXE% -o ClientToLua.cpp ClientToLua.pkg

echo frameworks tolua e--------------

pause