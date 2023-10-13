REM Build Script for engine
@ECHO OFF
SetLocal EnableDelayedExpansion

REM Get a list of all the .c files
SET cFilenames=
FOR /R %%f in (*.c) do (
    SET cFilenames=!cFilenames! %%f
)

REM echo "Files:" %cFilenames%

SET assembly=Sandbox
SET compilerFlags=-g
REM -Wall -Werror
SET includeFlags=-Isrc -I../Dubhe/src/
SET linkerFlags=-L../bin/ -lDubhe.lib
SET defines=-D_DEBUG -DKIMPORT

ECHO "Preparing environments..."
set binDirs=../bin
if not exist "%binDirs%" (
    mkdir "%binDirs%"
    echo "Created directory ../bin"
) else (
    echo "The directory ../bin is already exist"
)

ECHO "Building %assembly%..."
clang %cFilenames% %compilerFlags% -o ../bin/%assembly%.exe %defines% %includeFlags% %linkerFlags%