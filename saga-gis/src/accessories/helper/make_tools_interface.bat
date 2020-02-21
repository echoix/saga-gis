@ECHO OFF

REM ___________________________________
SET SAGA=D:\saga\saga-code\bug-fix-release\saga-gis
SET SAGA_ROOT=%SAGA%
SET SAGA_CMD="%SAGA_ROOT%\bin\saga_vc_Win32\saga_cmd.exe"
SET SAGA_TOOLS_PY=%SAGA_ROOT%\src\accessories\python\helpers\saga_cmd_param_interface\tools.py

REM ___________________________________
%SAGA_CMD% dev_tools 6 -INPUT="%SAGA_TOOLS_PY%" -OUTPUT="%SAGA_TOOLS_PY%"

REM ___________________________________
PAUSE
