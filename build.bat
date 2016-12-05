@echo off

if NOT EXIST build mkdir build
pushd build

if EXIST relwarb.pdb del relwarb.pdb

set DEBUG="True"

set C_NOWARN=/wd4100 /wd4189 /wd4065 /wd4505 /wd4101
set C_BASE_FLAGS=/WX /W4 /FC /GR- /EHa- /nologo %C_NOWARN% /DWIN32_LEAN_AND_MEAN
set C_DEB_FLAGS=/Z7 /Od /D_DEBUG /D_GL_ALLOW_ERROR_CHECKING
set C_REL_FLAGS=/Ox

set LD_BASE_FLAGS=user32.lib gdi32.lib opengl32.lib /nologo /opt:ref

if %DEBUG% == "" (
    set CFLAGS=%C_BASE_FLAGS% %C_REL_FLAGS%
    set LDFLAGS=%LD_BASE_FLAGS%

) else (
    set CFLAGS=%C_BASE_FLAGS% %C_DEB_FLAGS%
    set LDFLAGS=%LD_BASE_FLAGS% /debug
)

cl %CFLAGS% /c ..\src\relwarb.cpp
cl %CFLAGS% /c ..\src\relwarb_opengl.cpp
cl %CFLAGS% /c ..\src\win32_relwarb.cpp

link win32_relwarb.obj relwarb_opengl.obj relwarb.obj %LDFLAGS% /out:relwarb.exe

popd
