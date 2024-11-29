@echo off

set CURR_DIR=%cd%

pushd game

echo frameworks tolua s-------------

set PATH_TOLUA_EXE=%CURR_DIR%\..\..\tools\tolua++\tolua++.exe

%PATH_TOLUA_EXE% -o ClientToLua.cpp ClientToLua.pkg

echo frameworks tolua e--------------

popd

pause