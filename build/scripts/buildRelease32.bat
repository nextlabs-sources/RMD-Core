@ECHO OFF

SETLOCAL

CALL setVersion.bat

CALL "C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\Tools\VsDevCmd.bat"

CALL importExternalArtifacts.bat x86 release

cd ..

devenv.com RMCCore.sln /Build "Release|x86"
IF ERRORLEVEL 1 GOTO END

cd scripts

:END
ENDLOCAL
