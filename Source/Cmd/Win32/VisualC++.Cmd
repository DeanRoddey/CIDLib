@ECHO OFF
REM
REM Sets up the core Visual C++ environment. This is invoked by the other
REM code base specific setup files, since they would all depend on the VC++
REM environment be set up.
REM
REM This is all done in terms of the stuff that is set in the environment to
REM point to all the tools and SDKs.
REM
REM We get a Prod or Dev parameter so that we can add to the path the appropriate
REM Visual C++ runtime directory. These are copied under the SDKTWEAKS path, so
REM that we can control them and always know where they are.
REM
REM DEPENDENCIES
REM
REM We assume that CID_DEPENDS is set before we are called, since we need that
REM to set up various dependencies. So both CIDLib and CQC setup files will
REM set CID_DEPENDS before invoking us.
REM
REM The user has to set up a standard set of environment variables that we will
REM use, some being set by the tools/SDKs as they are installed. There are set
REM of dependencies that are in the CID_Dev directory itself which we will set
REM automatically
REM
REM The ones from the environment are:
REM
REM     CID_SPEECHPLATDIR   - The higher level speech platform SDK
REM     CID_PLATFORMSSDK    - The windows SDK top level path
REM     CID_VSSTUDIOPATH    - The top level Visual studio path
REM     CID_WMFSDKDIR       - The Window Media Format SDK path
REM     CID_PLATSDKVER      - The version of the Platform SDK we are using
REM     CID_VSTOOLSVER      - The version of the Visual Studio tools we are using
REM
REM     - Only used in CQC currently
REM     CID_QTSDK           - The Quick Time SDK directory
REM     CID_LIBVLCPATH      - The main VLC executable path itself
REM
REM The ones that we set up ourself are:
REM
REM     iTunes SDK      - A small set of libs and headers for iTunes COM
REM     SDK Tweaks      - A couple files we need not mornally available
REM     Scintilla       - We have a custom build of Scintilla
REM
REM     - Only used in CQC currently
REM     LIBVLC SDK      - To the sdk directory in our CID_Dev area
REM

IF "%1"=="" GOTO BAD_PARMS

REM ---------------------------------------------------------------------------
REM  Do all the crazy include and lib setup
REM ---------------------------------------------------------------------------
SET PATH=%PATH%;%CID_PLATFORMSDK%\bin\%CID_PLATSDKVER%\x86;%CID_VSTUDIOPATH%\Enterprise\Common7\IDE;%CID_VSTUDIOPATH%\VC\BIN;
SET PATH=%PATH%%CID_VSTUDIOPATH%\Enterprise\Common7\Tools;%CID_VSTUDIOPATH%\Enterprise\VC\Tools\MSVC\%CID_VSTOOLSVER%\bin\HostX86\x86;
SET INCLUDE=%CID_PLATFORMSDK%\include\shared;%CID_PLATFORMSDK%\include\um;%INCLUDE%
SET INCLUDE=%INCLUDE%;%CID_VSTUDIOPATH%\Enterprise\VC\Tools\MSVC\%CID_VSTOOLSVER%\include;

SET LIB=%CID_VSTUDIOPATH%\VC\LIB;%LIB%

SET INCLUDE=%CID_PLATFORMSDK%\Include\%CID_PLATSDKVER%\ucrt;%INCLUDE%
SET INCLUDE=%INCLUDE%;%CID_PLATFORMSDK%\Include\%CID_PLATSDKVER%\um;
SET INCLUDE=%INCLUDE%;%CID_PLATFORMSDK%\Include\%CID_PLATSDKVER%\shared;
SET LIB=%CID_PLATFORMSDK%\lib\%CID_PLATSDKVER%\ucrt\x86;%LIB%;
SET LIB=%LIB%;%CID_PLATFORMSDK%\lib\%CID_PLATSDKVER%\um\x86;
SET LIB=%LIB%;%CID_VSTUDIOPATH%\Enterprise\VC\Tools\MSVC\%CID_VSTOOLSVER%\lib\x86;

SET INCLUDE=%INCLUDE%;%CID_WMFSDKDIR%\Include;%WMPSDKDIR%\Include;%ITUNESSDK%
SET LIB=%LIB%;%CID_WMFSDKDIR%\Lib;%CID_SPEECHPLATDIR%\Lib

REM Make sure these are up front
SET INCLUDE=%CID_SPEECHPLATDIR%\Include;%KINECTSDK20_DIR%\inc;%INCLUDE%
SET LIB=%CID_SPEECHPLATDIR%\Lib;%KINECTSDK20_DIR%\Lib\x86;%LIB%
SET PATH=%CID_SPEECHPLATDIR%\Tools;%PATH%

REM Add ATL/MFC stuff if required
SET INCLUDE=%INCLUDE%;%CID_VSTUDIOPATH%\Enterprise\VC\Tools\MSVC\%CID_VSTOOLSVER%\atlmfc\include;
SET LIB=%LIB%;%CID_VSTUDIOPATH%\Enterprise\VC\Tools\MSVC\%CID_VSTOOLSVER%\atlmfc\lib\x86;

REM Add a tweak directory where we put stuff we have to modify or pull down separately
SET INCLUDE=%CID_DEPENDS%\SDKTweaks;%INCLUDE%
SET LIB=%CID_DEPENDS%\SDKTweaks;%LIB%

REM ---------------------------------------------------------------------------
REM Add to the path the appropriate runtime library path
REM ---------------------------------------------------------------------------
IF "%1"=="Prod" SET Path=%Path%;%CID_DEPENDS%\VCRuntime\Microsoft.CRT;
IF "%1"=="Dev" SET Path=%Path%;%CQCRuntime%\VCRuntime\Microsoft.DebugCRT;


REM ---------------------------------------------------------------------------
REM  Add our built Scintilla stuff to the path (to load the DLL) and to the include
REM  path.
REM ---------------------------------------------------------------------------
SET PATH=%PATH%;%CID_DEPENDS%\SDKS\Scintilla;
SET INCLUDE=%INCLUDE%;%CID_DEPENDS%\SDKS\Scintilla;


REM ---------------------------------------------------------------------------
REM  Add our LibVLC stuff
REM ---------------------------------------------------------------------------
SET PATH=%PATH%;%CID_LIBVLCPATH%;
SET INCLUDE=%INCLUDE%;%CID_DEPENDS%\SDKS\LibVLC\Include;
SET LIB=%LIB%;%CID_DEPENDS%\SDKS\LibVLC\Lib;


REM ---------------------------------------------------------------------------
REM  Add the iTunes SDK
REM ---------------------------------------------------------------------------
SET PATH=%PATH%;%CID_QTSDK%;
SET INCLUDE=%INCLUDE%;%CID_DEPENDS%\SDKS\iTunes;
GOTO EXIT


REM Come here if bad parameters
:BAD_PARMS
ECHO You must provide a Dev or Prod parameter


:EXIT

