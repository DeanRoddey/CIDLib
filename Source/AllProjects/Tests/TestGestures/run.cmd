@ECHO OFF
SETLOCAL
SET APPCMD=%CID_RESDIR%\TestGestures.exe

IF "%1"=="debug" GOTO DO_DEBUG
start %APPCMD%
GOTO DONE

:DO_DEBUG
devenv /debugexe %APPCMD%

:DONE


