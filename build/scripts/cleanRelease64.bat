@ECHO OFF

SETLOCAL

CALL "C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\Tools\VsDevCmd.bat"

cd ..

devenv.com RMCCore.sln /Clean "Release|x64"
IF ERRORLEVEL 1 GOTO END

IF EXIST build.msvc\x64_Release rd /s /q build.msvc\x64_Release

cd scripts

:END
ENDLOCAL
