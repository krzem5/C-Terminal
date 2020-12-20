@echo off
cls
set _INCLUDE=%INCLUDE%
set INCLUDE=./;../src/include;../rsrc/;%INCLUDE%
if exist build rmdir /s /q build
mkdir build
cd build
if %1.==. goto dbg
if %1==-r (
	cl /c /permissive- /GS /W3 /Zc:wchar_t /Gm- /sdl /Zc:inline /fp:precise /D "NDEBUG"  /D "_WINDOWS" /D "_USRDLL" /D "_WINDLL" /D "_UNICODE" /D "UNICODE" /errorReport:none /WX /Zc:forScope /Gd /Oi /FC /D "DLL1_EXPORTS" /EHsc /nologo /diagnostics:column /GL /Gy /Zi /O2 /Oi /MD ../src/main.c ../src/terminal/*.c ../rsrc/*.c&&link *.obj /OUT:terminal.exe /DYNAMICBASE "kernel32.lib" "user32.lib" "gdi32.lib" "winspool.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "odbc32.lib" "odbccp32.lib" /MACHINE:X64 /SUBSYSTEM:CONSOLE /ERRORREPORT:none /NOLOGO /TLBID:1 /WX /LTCG /OPT:REF /INCREMENTAL:NO /OPT:ICF&&goto run
	goto end
)
:dbg
fxc ../rsrc/texture_tint_2d.hlsl /T vs_4_0 /E texture_tint_2d_vs /Zpr /O0 /WX /Zss /nologo /Fh texture_tint_2d_vertex.h&&fxc ../rsrc/texture_tint_2d.hlsl /T ps_4_0 /E texture_tint_2d_ps /Zpr /O0 /WX /Zss /nologo /Fh texture_tint_2d_pixel.h&&cl /c /permissive- /GS /W3 /Zc:wchar_t /Gm- /sdl /Zc:inline /fp:precise /D "_DEBUG"  /D "_WINDOWS" /D "_USRDLL" /D "_WINDLL" /D "_UNICODE" /D "UNICODE" /errorReport:none /WX /Zc:forScope /Gd /Oi /FC /D "DLL1_EXPORTS" /EHsc /nologo /diagnostics:column /ZI /Od /RTC1 /MDd ../src/main.c ../src/terminal/*.c ../rsrc/*.c&&link *.obj /OUT:terminal.exe /DYNAMICBASE "kernel32.lib" "user32.lib" "gdi32.lib" "winspool.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "odbc32.lib" "odbccp32.lib" /MACHINE:X64 /SUBSYSTEM:CONSOLE /ERRORREPORT:none /NOLOGO /TLBID:1 /WX /DEBUG /INCREMENTAL&&goto run
goto end
:run
del *.obj
del *.pdb
del *.exp
del *.ilk
del *.idb
cls
terminal.exe
:end
cd ../
set INCLUDE=%_INCLUDE%
