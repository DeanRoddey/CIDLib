@ECHO OFF

IF EXIST %CID_RESDIR%\TestCIDWnd.sln GOTO LOAD_SLN
devenv /debugexe %CID_RESDIR%\TestCIDWnd.exe
GOTO END

:LOAD_SLN
devenv %CID_RESDIR%\TestCIDWnd.sln


:END


