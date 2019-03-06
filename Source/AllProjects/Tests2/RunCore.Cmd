@ECHO OFF
SETLOCAL
SET APPCMD=%CID_RESDIR%\TestFW.exe /CfgFile=.\CIDLibTests.TestCfg /Verbosity=High /Groups=Core
IF "%1"=="debug" GOTO DO_DEBUG

%APPCMD%
GOTO DONE

:DO_DEBUG
devenv /debugexe %APPCMD%

:DONE

