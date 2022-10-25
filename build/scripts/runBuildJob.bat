@ECHO OFF

SETLOCAL

IF "%BUILD_NUMBER%"=="" (
  echo Error: BUILD_NUMBER is not set!
  GOTO :EOF
)

REM
REM Check line-end in .INF files.  Make sure that they have Windows line-end.
REM Otherwise, Visual Studio will fail to add timestamps to the .inf files
REM without any error messages.
REM 

REM
REM Check line-end in .BAT files.  Make sure that they have Windows line-end.
REM Otherwise, CALL commands which call labels will not be able to find the
REM labels.
REM
echo Checking %0 ...
..\..\sources\tools\lineendchecker\output\x86_release\lineendchecker.exe /win %0
IF ERRORLEVEL 1 GOTO :EOF

REM
REM Configure various build variables.
REM
CALL configure.bat

REM
REM Release build
REM
CALL buildRelease32.bat
IF ERRORLEVEL 1 GOTO :EOF
CALL buildRelease64.bat
IF ERRORLEVEL 1 GOTO :EOF

REM
REM Debug build
REM
CALL buildDebug32.bat
IF ERRORLEVEL 1 GOTO :EOF
CALL buildDebug64.bat
IF ERRORLEVEL 1 GOTO :EOF

SET RES_PUBLISH_TO_BODA=false
if "%PUBLISH_TO_BODA%" == "Yes" set RES_PUBLISH_TO_BODA=true
if "%PUBLISH_TO_BODA%" == "yes" set RES_PUBLISH_TO_BODA=true
if "%PUBLISH_TO_BODA%" == "YES" set RES_PUBLISH_TO_BODA=true
if "%PUBLISH_TO_BODA%" == "1" set RES_PUBLISH_TO_BODA=true
if "%RES_PUBLISH_TO_BODA%"=="true" (
REM
REM Artifacts
REM
CALL publishArtifacts.bat
)
