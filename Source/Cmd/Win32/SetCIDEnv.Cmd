@ECHO OFF
REM ---------------------------------------------------------------------------
REM This file provides environment setup services for building CIDLib, i.e.
REM working on CIDLib itself. Of course you could build programs based on
REM CIDLib from within this environment as well, but probably you'd want to
REM to do that in a simple setup which just requires the headers and libraries
REM to be in place and in the right paths.
REM
REM The usage is:
REM
REM     SetCIDEnv [Prod Dev] [WIN32_XP etc...] DriveWithColon RootPath
REM
REM     Param 1 indicates what type of build environment to set up. It will
REM             be used to set the CID_BUILDMODE variable, which is used in
REM             many places to build the correct output directories.
REM     Param 2 indicates what platform to target, which should of course
REM             make sense for the current platform. It will be used to set
REM             the CID_PLATFORM variable. For Windows it is currently
REM             WIN32_WIN7.
REM     Param 3 Indicates the drive letter of the source tree, with colon!
REM             It will be used to set the CID_DRIVE variable.
REM     Param 4 Indicates the root of the CIDLib tree, without the drive but
REM             with the leading slash, e.g. \Whatever\CID_Dev. This and the
REM             drive will be used to set the CID_SRCTREE variable. DO NOT
REM             provide a trailing slash!
REM
REM On output, it will have set the following major stuff:
REM
REM These are for compile time only and are required. They are driven
REM directly by the parameters passed to this program.
REM
REM     CID_VERSION
REM     CID_VERSIONUL
REM         These are set from the version that is hard coded in this file
REM         below. It used to come in as a parameter, but having it part of
REM         the source controlled content is probably better. The UL one is
REM         with underlines replacing periods.
REM
REM     CID_DEPENDS
REM         The path to the CID_Dev\Depends directory, which has various dependencies
REM         we need to set up paths to and such.
REM
REM     CID_DRIVE
REM         This is used to build up the CID_SRCTREE and to set up other
REM         stuff below. It lets us have the driver letter separate from the
REM         build tree path, so its just a convenience. It is set by the
REM         drive parameter so it is in the form X:, i.e. drive letter and
REM         colon.
REM
REM     CID_PLATFORM
REM         This is the target platform, which is set from an incoming parm.
REM         For Windows currently WIN32_WIN7.
REM
REM     CID_BUILDMODE
REM         The build mode (Prod|Dev) driven by an incoming parm
REM
REM     CID_SRCTREE
REM         The path to the top of the CIDLib source tree (X:\Whatever\CID_Dev.
REM
REM     CID_RESDIR
REM         The results directory. This is where built output goes to. It is
REM         per-version so we can have more than one version built if we need
REM         to go back and forth in the version control system.
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
REM Set this appropriately for the version to build
REM ---------------------------------------------------------------------------
SET CID_VERSION=5.3.948
SET CID_VERSIONUL=%CID_VERSION:.=_%


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
REM Set our command window title
REM ---------------------------------------------------------------------------
TITLE CIDLib [%1, %CID_VERSION%]


REM ---------------------------------------------------------------------------
REM Set up the path to the CIDLib source tree. It's sort of redundant and more
REM something that you would use building applications based on CIDLib, but
REM set it anyway.
REM ---------------------------------------------------------------------------
SET CIDLIB_SRCDIR=%3%4


REM ---------------------------------------------------------------------------
REM  Set up the dependencies path which we have to pass to the Visual C++ setup below
REM ---------------------------------------------------------------------------
SET CID_DEPENDS=%3%4\Depends


REM ---------------------------------------------------------------------------
REM Set up the Visual C++ environment. This adds in all of the standard MS stuff
REM plus all of the SDK stuff we set in the environment.
REM ---------------------------------------------------------------------------
CALL %CIDLIB_SRCDIR%\Source\Cmd\Win32\VisualC++.Cmd %1 %CID_DEPENDS%


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
REM then the first invocation of CIDBuild or any build programs will probably
REM fail.
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
ECHO    SetCIDEnv [Prod | Dev] [WIN32_WIN7] Drive RootDir
ECHO    ----------------------------------------
ECHO    Param 1 indicates the build mode, production or debug.
ECHO    Param 2 indicates the current build platform.
ECHO    Param 3 is the drive of the source tree, with the colon!
ECHO    Param 4 is the root dir of the source tree, without a
ECHO            drive letter, but with a leading slash (e.g.
ECHO            \CID_Dev) NO trailing slash!
ECHO    ----------------------------------------

:DONE


REM ---------------------------------------------------------------------------
REM Clean up temps.
REM ---------------------------------------------------------------------------
SET LOGCHARMODE=

