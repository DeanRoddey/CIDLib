@ECHO OFF
SETLOCAL
SET APPCMD=%CID_RESDIR%\TestMacroDbg.exe ..\..\Tests2\TestMacroEng\Files ..\..\Tests2\TestMacroEng\Classes

IF "%1"=="debug" GOTO DO_DEBUG
start %APPCMD%
GOTO DONE

:DO_DEBUG
devenv /debugexe %APPCMD%

:DONE


