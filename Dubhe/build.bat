REM Build Script for engine
@ECHO OFF
SetLocal EnableDelayedExpansion

REM Get a list of all the .c files
SET cFilenames=
FOR /R %%f in (*.c) do (
    SET cFilenames=!cFilenames! %%f
)

REM echo "Files:" %cFilenames%

SET assembly=Dubhe
SET compilerFlags=-g -shared -Wvarargs -Wall -Werror
REM -Wall -Werror
SET includeFlags=-Isrc -I%VULKAN_SDK%/Include
SET linkerFlags=-luser32 -lvulkan-1 -L%VULKAN_SDK%/Lib
SET defines=-D_DEBUG -DKEXPORT -D_CRT_SECURE_NO_WARNINGS

ECHO "Preparing environments..."
set binDirs=../bin
if not exist "%binDirs%" (
    mkdir "%binDirs%"
    echo "Created directory ../bin"
) else (
    echo "The directory ../bin is already exist"
)

ECHO "Building %assembly%..."
clang %cFilenames% %compilerFlags% -o ../bin/%assembly%.dll %defines% %includeFlags% %linkerFlags%