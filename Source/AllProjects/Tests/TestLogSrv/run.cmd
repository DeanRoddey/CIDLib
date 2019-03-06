start CIDNameSrv.exe /LocalLog=%CID_RESDIR%\Logs\Log.Txt;UTF-16;CQCLgMtx
start CIDLogSrv.exe /LocalLog=%CID_RESDIR%\Logs\Log.Txt;UTF-16;CQCLgMtx /OutPath=%CID_RESDIR%\Logs
REM devenv /debugexe %CIDLIB_SRCDIR%\DevResult\CIDLogSrv.exe /LocalLog=%CID_RESDIR%\Logs\Log.Txt;UTF-16;CQCLgMtx /OutPath=%CID_RESDIR%\Logs

