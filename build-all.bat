@ECHO OFF
REM Build All

ECHO "BUilding everything..."

PUSHD Dubhe
CALL build.bat
POPD
IF %ERRORLEVEL% NEQ 0 ( echo Error:%ERRORLEVEL% && exit )

PUSHD Sandbox
CALL build.bat
POPD
IF %ERRORLEVEL% NEQ 0 ( echo Error:%ERRORLEVEL% && exit )

ECHO "All assemblies build successfully"