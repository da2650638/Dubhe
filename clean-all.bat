@ECHO OFF
REM Clean Everything

ECHO "Cleaning everything..."

REM Dubhe
make -f "Makefile.Dubhe.windows.mak" clean
IF %ERRORLEVEL% NEQ 0 (echo Error:%ERRORLEVEL% && exit)

REM Sandbox
make -f "Makefile.Sandbox.windows.mak" clean
IF %ERRORLEVEL% NEQ 0 (echo Error:%ERRORLEVEL% && exit)

ECHO "All assemblies cleaned successfully."