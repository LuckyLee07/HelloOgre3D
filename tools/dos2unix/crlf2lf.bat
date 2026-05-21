rem for /R %G in (*.xml) do unix2dos "%G"
for /r %1 %%i in (*.xml,*.lua,*.cpp,*.c,*.h) do (
	%~dp0\dos2unix-7.4.3-win64\bin\dos2unix %%i %%i
)
echo ok
pause