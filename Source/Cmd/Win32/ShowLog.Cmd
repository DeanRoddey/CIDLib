@ECHO OFF
REM ---------------------------------------------------------------------------
REM  This file will show the log file. The SetCIDEnv.Cmd file sets it to
REM  be in the same directory as the error dumps, so we use that. The actual
REM  CID_LOCALLOG variable has other stuff in it so we cannot use it directly.
REM ---------------------------------------------------------------------------
type %CID_ERRDUMPDIR%\Log.Text
