@ECHO OFF

REM ___________________________________
SET GITEXE=git

SET SAGA=D:\saga\saga-code\bug-fix-release\saga-gis
SET SAGA_ROOT=%SAGA%
SET SAGA_CMD="%SAGA_ROOT%\bin\saga_vc_win32\saga_cmd.exe"
SET SAGA_SRC=saga_src
SET SAGA_VER_MAJOR=7
SET SAGA_VER_MINOR=6
SET SAGA_VER_RELEASE=3
SET SAGA_VER_TEXT=%SAGA_VER_MAJOR%.%SAGA_VER_MINOR%.%SAGA_VER_RELEASE%
SET SAGA_VERSION=saga-%SAGA_VER_TEXT%

SET RESPATH=%SAGA_ROOT%\src\saga_core\saga_gui\res

REM ___________________________________
REM GIT Source Code Repository
%GITEXE% clone git://git.code.sf.net/p/saga-gis/code %SAGA_SRC%
PUSHD %SAGA_SRC%
%GITEXE% checkout release-%SAGA_VER_TEXT%
RMDIR /S/Q .git
POPD

REM ___________________________________
%SAGA_CMD% -f=q dev_tools 0 -TARGET="%RESPATH%\saga.lng.txt" -DIRECTORY="%SAGA_SRC%"

%SAGA_CMD% -f=s dev_tools 1 -MASTER="%RESPATH%\saga.lng.txt" -IMPORT="%RESPATH%\saga.ger.txt" -UNION="%RESPATH%\saga.ger.txt" -METHOD=1
%SAGA_CMD% -f=s dev_tools 1 -MASTER="%RESPATH%\saga.lng.txt" -IMPORT="%RESPATH%\saga.bra.txt" -UNION="%RESPATH%\saga.bra.txt" -METHOD=1

DEL "%RESPATH%\*.mtab"

RMDIR /S/Q %SAGA_SRC%

REM ___________________________________
PAUSE