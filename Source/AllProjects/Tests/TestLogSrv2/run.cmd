start CIDNameSrv /LocalLog=%CID_RESDIR%/Logs/Log.Txt;UTF-16;CQCLgMtx
start CIDLogSrv /LocalLog=%CID_RESDIR%/Logs/Log.Txt;UTF-16;CQCLgMtx /OutPath=%CID_RESDIR%/Logs
REM devenv /debugexe \CID_Dev\DevResult\CIDLogSrv.exe /LocalLog=%CID_RESDIR%/Logs/Log.Txt;UTF-16;CQCLgMtx /OutPath=%CID_RESDIR%/Logs

