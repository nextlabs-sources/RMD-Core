@ECHO OFF

SETLOCAL

CALL "C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\Tools\VsDevCmd.bat"

cd ..

devenv.com RMCCore.sln /Clean "Debug|x86"
IF ERRORLEVEL 1 GOTO END

IF EXIST build.msvc\Win32_Debug rd /s /q build.msvc\Win32_Debug

cd scripts

:END
ENDLOCAL
