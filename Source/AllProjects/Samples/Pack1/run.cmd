@ECHO OFF
SETLOCAL

SET APPCMD=%CID_RESDIR%\Pack1.exe %CIDLIB_SRCDIR%\Source\AllProjects\Samples %CID_RESDIR%\Pack1Test

IF "%1"=="debug" GOTO DO_DEBUG
%APPCMD%
GOTO DONE

:DO_DEBUG
devenv /debugexe %APPCMD%

:DONE


