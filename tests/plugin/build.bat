@echo off
rem Builds the test plugin (rftest_plugin.dll) and the package.loadlib test DLL (rftest_loadlib.dll) into tests\plugin\out.
rem Needs Visual Studio (any edition with the C++ workload); vswhere finds it.
setlocal
set "HERE=%~dp0"
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" (
    echo vswhere.exe not found: install Visual Studio with the C++ workload
    exit /b 1
)
for /f "usebackq delims=" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VS=%%i"
if not defined VS (
    echo no Visual Studio installation with the C++ x64 tools found
    exit /b 1
)
call "%VS%\VC\Auxiliary\Build\vcvars64.bat" >nul || exit /b 1

set "OUT=%HERE%out"
if not exist "%OUT%" mkdir "%OUT%"
rem vcvars may change the current directory, so every output path is explicit.
rem The plugin runs code in a Lua state it creates, so it links the Lua REFramework links: every C file in
rem dependencies\lua\src, built without extra defines like REFramework's own lua target.
set "LUASRC=%HERE%..\..\dependencies\lua\src"
set "LUAOUT=%OUT%\lua"
if not exist "%LUAOUT%" mkdir "%LUAOUT%"
cl /nologo /c /O2 /W3 "%LUASRC%\*.c" /Fo"%LUAOUT%\\" || exit /b 1
cl /nologo /LD /EHsc /O2 /std:c++20 /W3 /external:anglebrackets /external:W0 /I "%HERE%..\..\include" /I "%LUASRC%" "%HERE%rftest_plugin.cpp" "%LUAOUT%\*.obj" /Fo"%OUT%\rftest_plugin.obj" /Fe"%OUT%\rftest_plugin.dll" /link /NOLOGO || exit /b 1
cl /nologo /LD /O2 "%HERE%rftest_loadlib.c" /Fo"%OUT%\rftest_loadlib.obj" /Fe"%OUT%\rftest_loadlib.dll" /link /NOLOGO || exit /b 1
echo built %OUT%\rftest_plugin.dll and %OUT%\rftest_loadlib.dll
