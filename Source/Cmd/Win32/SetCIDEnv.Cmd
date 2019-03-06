@ECHO OFF
REM ---------------------------------------------------------------------------
REM This file provides environment setup services for building CIDLib, i.e.
REM working on CIDLib itself, not building programs based on CIDLib. Building
REM programs based on CIDLib is a much simpler scenario and should probably
REM be done within the context of the target development environment. It only
REM requires that the headers, Libs, and DLLs be installed and in the correct
REM paths.
REM
REM
REM This is called by CIDLibDev.Cmd which is a higher level setup file in the
REM EnvSetup directory. Before calling us, it will set up SET CID_VERSION with
REM the M.m.r version of CIDLib to use. That is sort of the most fundamental thing
REM and it will change each time we start a new version branch. The other stuff set
REM here tends to remain stable for long periods of time.
REM
REM The usage is:
REM
REM     SetCIDEnv [Prod Dev] [WIN32_XP etc...] DriveWithColon RootPath
REM
REM
REM     Param 1 indicates what type of build environment to set up. It will
REM             be used to set the CID_BUILDMODE variable, which is used in
REM             many places to build the correct output directories.
REM     Param 2 indicates what platform to target, which should of course
REM             make sense for the current platform. It will be used to set
REM             the CID_PLATFORM variable.
REM     Param 3 Indicates the drive letter of the source tree, with colon!
REM             It will be used to set the CID_DRIVE variable.
REM     Param 4 Indicates the root of the CIDLib tree, without the drive but
REM             with the leading slash, e.g. D:\Whatever\CID_Dev. This and the
REM             drive will be used to set the CID_SRCTREE variable. DO NOT
REM             provide a trailing slash!
REM
REM On output, it will have set the following major stuff:
REM
REM These are for compile time only and are required. They are driven
REM directly by the parameters passed to this program.
REM
REM     CID_DRIVE
REM         This is used to build up the CID_SRCTREE and to set up other
REM         stuff below. It lets us have the driver letter separate from the
REM         build tree path, so its just a convenience. It is set by the
REM         drive parameter so it is in the form X:, i.e. drive letter and
REM         colon.
REM
REM     CID_PLATFORM
REM     CID_BUILDMODE
REM     CID_SRCTREE
REM         These (plus CID_VERSION already set by our caller) are all used to
REM         drive the build environment. First they are used by the Make file
REM         that builds CIDBuild. After that they are used in the convenience
REM         .Cmd files that invoke CIDBuild for us. In this way, they also control
REM         aspects of the rest of the code built, since CIDBuild takes this
REM         information and drives the build process for everything else.
REM
REM
REM These are for runtime purposes, and will usually be setup during
REM installation on client machines as well as during development. They are
REM set here for testing purposes.
REM
REM     CID_LOCALLOG
REM         The path to the log file for local logging, its text format, and
REM         a mutex name to control simultaneous logging. The CIDLib logging
REM         code will make use of this if found to install a logger.
REM ---------------------------------------------------------------------------


REM ---------------------------------------------------------------------------
REM Make sure we got our four parameters and no more than that.
REM ---------------------------------------------------------------------------
IF "%4" == "" GOTO SHOW_USAGE
IF NOT "%5" == "" GOTO SHOW_USAGE


REM ---------------------------------------------------------------------------
REM Set up the build mode to Dev or Prod according to the first parameter.
REM If its not one of the valid values, then show usage and exit because this
REM is so important in the build process.
REM ---------------------------------------------------------------------------
IF "%1"=="Prod" GOTO SET_BUILDMODE
IF "%1"=="Dev" GOTO SET_BUILDMODE
GOTO SHOW_USAGE

:SET_BUILDMODE
SET CID_BUILDMODE=%1




REM ---------------------------------------------------------------------------
REM Make sure that the platform parameter is a valid platform. And set up
REM some stuff that is per-platform for later use.
REM ---------------------------------------------------------------------------
IF "%2"=="WIN32_WIN7" GOTO PLAT_WIN7
GOTO SHOW_USAGE

:PLAT_WIN7
SET CID_PLATFORM=WIN32_WIN7
SET LOGCHARMODE=UTF-16
GOTO DONE_PLAT

:DONE_PLAT


REM ---------------------------------------------------------------------------
REM Set some standard compile time stuff from command line params. We assume
REM that the params are correct since its hard to check here. If they are bad,
REM then the first invocation of a Cmd file that runs the CIDBuild program
REM will catch it since CIDBuild will complain.
REM ---------------------------------------------------------------------------
SET CID_DRIVE=%3
SET CID_SRCTREE=%CID_DRIVE%%4
SET CID_RESDIR=%CID_SRCTREE%\Output\%CID_VERSIONUL%\%CID_BUILDMODE%Result



REM ---------------------------------------------------------------------------
REM Add the result directory to the path and the Cmd directory so that batch
REM files and regular debug development executable files will be found.
REM ---------------------------------------------------------------------------
SET PATH=%PATH%;%CID_RESDIR%;%CID_SRCTREE%\Source\Cmd\Win32


REM ---------------------------------------------------------------------------
REM Set up the standard local log file and logging sync semaphore to point
REM to some reasonable values.
REM ---------------------------------------------------------------------------
SET CID_LOCALLOG=%CID_RESDIR%\Log.Text;%LOGCHARMODE%;DefLgMtx


REM ---------------------------------------------------------------------------
REM These are just for the testing program TestCIDLib, which needs to be able
REM to test for some values that it knows exist and have a particular value.
REM You can throw away these lines if you don't run the test programs.
REM ---------------------------------------------------------------------------
SET TestVar1=The value of TestVar1
SET TestVar2=The value of TestVar2

GOTO DONE


REM ---------------------------------------------------------------------------
REM If the parameters are not correct, then we jump here to show the usage
REM ---------------------------------------------------------------------------
:SHOW_USAGE

ECHO Usage
ECHO    SetCIDEnv [Prod | Dev] [WIN32_WIN7] Drive Root
ECHO    ----------------------------------------
ECHO    Param 1 indicates the build mode, production or debug.
ECHO    Param 2 indicates the current build platform.
ECHO    Param 3 is the drive of the source tree, with the colon!
ECHO    Param 4 is the root dir of the source tree, without a
ECHO            drive letter, but with a leading slash (e.g.
ECHO            \CID_Dev) NO trailing slash!
ECHO *
ECHO
ECHO    CID_VERSION must already be set to the version to apply to the built
ECHO    code, in the form 1.2.3 (e.g. Maj.min.revision.) And CID_VERSIONUL must
ECHO    be set to the same but with underlines, so 1_2_3.
ECHO    ----------------------------------------

:DONE


REM ---------------------------------------------------------------------------
REM Clean up temps.
REM ---------------------------------------------------------------------------
SET LOGCHARMODE=

