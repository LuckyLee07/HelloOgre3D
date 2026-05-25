@echo off

set CURR_DIR=%cd%

set PATH_TOLUA_EXE=%CURR_DIR%\..\..\tools\tolua++\tolua++.exe

pushd game
echo ------------- client tolua -------------
%PATH_TOLUA_EXE% -o GameToLua.cpp GameToLua.pkg
popd

pushd runtime
echo ------------- runtime tolua -------------
%PATH_TOLUA_EXE% -o RuntimeToLua.cpp RuntimeToLua.pkg
popd

pushd sandbox\scripting
echo ------------- sandbox tolua -------------
%PATH_TOLUA_EXE% -o SandboxToLua.cpp SandboxToLua.pkg
popd

pause
