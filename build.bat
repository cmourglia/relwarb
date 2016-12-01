@echo off

if NOT EXIST build mkdir build
pushd build

set DEBUG="True"

set C_BASE_FLAGS=/WX /W4 /wd4100 /wd4189 /FC /GR- /EHa- /nologo
set C_DEB_FLAGS=/Z7 /O1
set C_REL_FLAGS=/Ox

set LD_BASE_FLAGS=user32.lib gdi32.lib opengl32.lib /nologo

if %DEBUG% == "" (
    set CFLAGS=%C_BASE_FLAGS% %C_REL_FLAGS%
    set LDFLAGS=%LD_BASE_FLAGS%

) else (
    set CFLAGS=%C_BASE_FLAGS% %C_DEB_FLAGS%
    set LDFLAGS=%LD_BASE_FLAGS% /debug
)


cl %CFLAGS% /c ..\src\win32_relwrab.cpp
link win32_relwrab.obj %LDFLAGS% /out:relwrab.exe

popd
