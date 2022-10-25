@ECHO OFF

SETLOCAL

set CODEBASE=Doom
set CODEBASE_LOWERCASE=doom
set TARGET_OS=Windows

CALL setVersion.bat

IF "%BUILD_NUMBER%"=="" (
  echo Error: BUILD_NUMBER is not set!
  GOTO :EOF
)

set S_DRIVE=\\nextlabs.com\share\data
set S_DRIVE_POSIX=//nextlabs.com/share/data

IF "%CONFIG_TYPE%"=="feature_smdc" (
  set VERSION_DIR=%S_DRIVE%\%OUTPUT_REPOSITORY_ROOT%\artifacts\%CODEBASE%\Core\%VERSION_MAJMIN%.%BRANCH_ID%
  set VERSION_DIR_POSIX=%S_DRIVE_POSIX%/%OUTPUT_REPOSITORY_ROOT_POSIX%/artifacts/%CODEBASE%/Core/%VERSION_MAJMIN%.%BRANCH_ID%
) ELSE IF "%CONFIG_TYPE%"=="feature_cdc" (
  set VERSION_DIR=%S_DRIVE%\%OUTPUT_REPOSITORY_ROOT%\artifacts\%CODEBASE%\Core\%VERSION_MAJMIN%.%BRANCH_ID%
  set VERSION_DIR_POSIX=%S_DRIVE_POSIX%/%OUTPUT_REPOSITORY_ROOT_POSIX%/artifacts/%CODEBASE%/Core/%VERSION_MAJMIN%.%BRANCH_ID%
) ELSE (
  set VERSION_DIR=%S_DRIVE%\%OUTPUT_REPOSITORY_ROOT%\artifacts\%CODEBASE%\Core\%VERSION_MAJMIN%
  set VERSION_DIR_POSIX=%S_DRIVE_POSIX%/%OUTPUT_REPOSITORY_ROOT_POSIX%/artifacts/%CODEBASE%/Core/%VERSION_MAJMIN%
)

IF "%CONFIG_TYPE%"=="feature_smdc" (
  set TYPE=feature
) ELSE IF "%CONFIG_TYPE%"=="feature_cdc" (
  set TYPE=feature
) ELSE IF "%CONFIG_TYPE%"=="develop" (
  set TYPE=develop
) ELSE (
  set TYPE=release
)

set ARTIFACT_FILE_PREFIX=%VERSION_DIR%\%TARGET_OS%\%VERSION_BUILD_SHORT%\%CODEBASE_LOWERCASE%-Core-%VERSION_MAJMIN%.%VERSION_BUILD_SHORT%-%TYPE%
set ARTIFACT_FILE_PREFIX_POSIX=%VERSION_DIR_POSIX%/%TARGET_OS%/%VERSION_BUILD_SHORT%/%CODEBASE_LOWERCASE%-Core-%VERSION_MAJMIN%.%VERSION_BUILD_SHORT%-%TYPE%

IF NOT EXIST %VERSION_DIR% md %VERSION_DIR%
IF ERRORLEVEL 1 GOTO END
IF NOT EXIST %VERSION_DIR%\%TARGET_OS% md %VERSION_DIR%\%TARGET_OS%
IF ERRORLEVEL 1 GOTO END
IF NOT EXIST %VERSION_DIR%\%TARGET_OS%\%VERSION_BUILD_SHORT% md %VERSION_DIR%\%TARGET_OS%\%VERSION_BUILD_SHORT%
IF ERRORLEVEL 1 GOTO END

cd ..\..

copy external\openssl\x86\lib\debug\libeay32.* build\build.msvc\Win32_Debug
copy external\openssl\x64\lib\debug\libeay32.* build\build.msvc\x64_Debug
copy external\openssl\x86\lib\release\libeay32.* build\build.msvc\Win32_Release
copy external\openssl\x64\lib\release\libeay32.* build\build.msvc\x64_Release

zip -D -r -9 %ARTIFACT_FILE_PREFIX_POSIX%-bin.zip sources/include/common/{brain.h,nlconfig.hpp} sources/include/common/{celog2,eframework.R4.0} sources/include/rmccore sources/common/brain/{cetype,nlstrfuncs,nlstrings,nlthread,nltypes}.h build/build.msvc/*/*.{lib,dll,exe,pdb}
IF ERRORLEVEL 1 GOTO END
echo INFO: Created %ARTIFACT_FILE_PREFIX_POSIX%-bin.zip

del /q %VERSION_DIR%\%TARGET_OS%\last_stable
IF ERRORLEVEL 1 GOTO END
copy %ARTIFACT_FILE_PREFIX%-bin.zip %VERSION_DIR%\%TARGET_OS%\last_stable
IF ERRORLEVEL 1 GOTO END
echo INFO: Copied %ARTIFACT_FILE_PREFIX_POSIX%-bin.zip to last_stable directory

cd build\scripts

:END
ENDLOCAL
