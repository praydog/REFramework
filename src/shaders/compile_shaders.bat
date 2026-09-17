@echo off
rem Some code is from DirectXTK's CompileShaders.cmd

setlocal
set error=0

if %PROCESSOR_ARCHITECTURE%.==ARM64. (set FXCARCH=arm64) else (if %PROCESSOR_ARCHITECTURE%.==AMD64. (set FXCARCH=x64) else (set FXCARCH=x86))

set FXCOPTS=/nologo /WX /Ges /Zi /Zpc /Qstrip_reflect /Qstrip_debug

set PCFXC="%WindowsSdkVerBinPath%%FXCARCH%\fxc.exe"
if exist %PCFXC% goto continue
set PCFXC="%WindowsSdkBinPath%%WindowsSDKVersion%\%FXCARCH%\fxc.exe"
if exist %PCFXC% goto continue
set PCFXC="%WindowsSdkDir%bin\%WindowsSDKVersion%\%FXCARCH%\fxc.exe"
if exist %PCFXC% goto continue

set PCFXC=fxc.exe

:continue

if not defined CompileShadersOutput set CompileShadersOutput=Compiled
set StrTrim=%CompileShadersOutput%##
set StrTrim=%StrTrim: ##=%
set CompileShadersOutput=%StrTrim:##=%
@if not exist "%CompileShadersOutput%" mkdir "%CompileShadersOutput%"

rem The compiling part.
call :CompileShader dmc5_velocity_conversion ps DMC5VelocityPS
call :CompileShader dmc5_velocity_conversion vs DMC5VelocityVS

if %error% == 0 (
    echo Shaders compiled ok
) else (
    echo There were shader compilation errors!
    exit /b 1
)

endlocal
exit /b 0

:CompileShader
set fxc=%PCFXC% "%1.fx" %FXCOPTS% /T%2_5_0 /E%3 "/Fh%CompileShadersOutput%\%1_%3.inc" "/Fd%CompileShadersOutput%\%1_%3.pdb" /Vn%1_%3
echo.
echo %fxc%
%fxc% || set error=1
exit /b

:CompileShaderHLSL
set fxc=%PCFXC% "%1.hlsl" %FXCOPTS% /T%2_5_0 /E%3 "/Fh%CompileShadersOutput%\%1_%3.inc" "/Fd%CompileShadersOutput%\%1_%3.pdb" /Vn%1_%3
echo.
echo %fxc%
%fxc% || set error=1
exit /b