REM Build script for tests
@ECHO OFF
SetLocal EnableDelayedExpansion

REM Get a list of all the .c files.
SET cFilenames=
FOR /R %%f in (*.c) do (SET cFilenames=!cFilenames! %%f)

REM echo "Files:" %cFilenames%

SET assembly=tests
REM SET compilerFlags=-g -Wno-missing-braces
SET compilerFlags=-g -Werror=vla -fdeclspec
REM -Wall -Werror -save-temps=obj -O0
SET includeFlags=-Isrc -I../Dubhe/src/
SET linkerFlags=-L../output/bin/ -lDubhe.lib
SET defines=-D_DEBUG -DDIMPORT

ECHO "Building %assembly%%..."
clang %cFilenames% %compilerFlags% -o ../output/bin/%assembly%.exe %defines% %includeFlags% %linkerFlags%