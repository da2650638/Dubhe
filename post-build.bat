@echo off

REM Run from root directory
REM %cd% = Current Directory
if not exist "%cd%\output\bin\assets\shaders" mkdir "%cd%\output\bin\assets\shaders"

echo "Compiling shaders..."

echo "assets/shaders/Builtin.ObjectShader.vert.glsl -> output/bin/assets/shaders/Builtin.ObjectShader.vert.spv"
%VULKAN_SDK%\Bin\glslc.exe -fshader-stage=vert assets\shaders\Builtin.ObjectShader.vert.glsl -o output\bin\assets\shaders\Builtin.ObjectShader.vert.spv
if %ERRORLEVEL% NEQ 0 (echo Error: %ERRORLEVEL% && exit)

echo "assets/shaders/Builtin.ObjectShader.frag.glsl -> output/bin/assets/shaders/Builtin.ObjectShader.frag.spv"
%VULKAN_SDK%\Bin\glslc.exe -fshader-stage=frag assets\shaders\Builtin.ObjectShader.frag.glsl -o output\bin\assets\shaders\Builtin.ObjectShader.frag.spv
if %ERRORLEVEL% NEQ 0 (echo Error: %ERRORLEVEL% && exit)

echo "Copying assets..."
echo xcopy "assets" "output\bin\assets" /h /i /c /k /e /r /y
xcopy "assets" "output\bin\assets" /h /i /c /k /e /r /y