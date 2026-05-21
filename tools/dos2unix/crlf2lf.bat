@echo off
rem Drag a directory onto this bat to convert text files from CRLF to LF.
set "TARGET=%~1"
if "%TARGET%"=="" set "TARGET=."
for /r "%TARGET%" %%i in (*.xml *.lua *.cpp *.c *.h *.cc *.md *.py *.json *.bat) do (
	"%~dp0dos2unix-7.4.3-win64\bin\dos2unix.exe" "%%i"
)
echo ok
pause
