@ECHO OFF

SETLOCAL

CALL setVersion.bat

CALL "C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\Tools\VsDevCmd.bat"

CALL importExternalArtifacts.bat x64 debug

cd ..

devenv.com RMCCore.sln /Build "Debug|x64"
IF ERRORLEVEL 1 GOTO END

cd scripts

:END
ENDLOCAL
