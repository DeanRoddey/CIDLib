@ECHO OFF
SETLOCAL

SET APPCMD=%CID_RESDIR%\HTTPSrv1.exe C:\Develop\DevRoot\CQC_Dev\CQCData\HTMLRoot\CQSL\CQSLWebSite 587 "MStore:My,Cathy"

IF "%1"=="debug" GOTO DO_DEBUG
%APPCMD%
GOTO DONE

:DO_DEBUG
devenv /debugexe %APPCMD% /Single

:DONE

