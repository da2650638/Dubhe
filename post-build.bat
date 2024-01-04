@echo off

REM Run from root directory
REM %cd% = Current Directory
if not exist "%cd%\output\bin\assets\shaders" mkdir "%cd%\output\bin\assets\shaders"

echo "Compiling shaders..."

echo "assets/shaders/builtin-shader.vert.glsl -> output/bin/assets/shaders/builtin-shader.vert.spv"
%VULKAN_SDK%\Bin\glslc.exe -fshader-stage=vert assets\shaders\builtin-shader.vert.glsl -o bin\assets\shaders\builtin-shader.vert.spv
if %ERRORLEVEL% NEQ 0 (echo Error: %ERRORLEVEL% && exit)

echo "assets/shaders/builtin-shader.frag.glsl -> output/bin/assets/shaders/builtin-shader.frag.spv"
%VULKAN_SDK%\Bin\glslc.exe -fshader-stage=frag assets\shaders\builtin-shader.frag.glsl -o bin\assets\shaders\builtin-shader.frag.spv
if %ERRORLEVEL% NEQ 0 (echo Error: %ERRORLEVEL% && exit)

echo "Copying assets..."
echo xcopy "assets" "bin\assets" /h /i /c /k /e /r /y
xcopy "assets" "bin\assets" /h /i /c /k /e /r /y