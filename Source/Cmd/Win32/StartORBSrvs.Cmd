@ECHO OFF
REM
REM This file will start up the fundamental ORB based servers just a command line
REM programs, which is how you'd run them for development and debug work. It sets
REM up the name and log servers to log to a Logs directory under the results
REM directory (they can't use the log server themselves.) And it sets up the config
REM server to create a repository in the results directory.
REM
REM In some cases you may want to make your own copy of this and set it up differently
REM but for the most part this is fine, and will work fine for the ORB based samples.
REM
start CIDNameSrv.exe /LocalLog=%CID_RESDIR%\Logs\Log.Txt;UTF-8;CQCLgMtx
start CIDLogSrv.exe /LocalLog=%CID_RESDIR%\Logs\Log.Txt;UTF-8;CQCLgMtx /OutPath=%CID_RESDIR%\Logs
start CIDCfgSrv.exe "/RepoPath=%CID_RESDIR%\"
