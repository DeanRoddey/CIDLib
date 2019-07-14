# ----------------------------------------------------------------------------
#  FILE: CIDBuild_Win32.Mak
#
#   This file builds the CIDBuild.exe program for the Win32 subsystem. Once
#   CIDBuild is built, it is used to build the rest of the system.
#
#   It expects some values to come in from the environment. There is more
#   stuff than this set by the standard SetCIDEnv.Cmd file, but this is all
#   that we require here.
#
#   CID_BUILDMODE should be set to either Dev or Prod
#       It is used to drive many compilation and link flags. One of them is
#       the BLDMODE_XXX flag which is used internally in CIDBuild to drive
#       build mode specific code. CIDBuild also passes this flag on to the
#       code it compiles, which uses it for the same purposes. The XXX is
#       the value of this variable.
#
#   CID_PLATFORM should be set to WIN32_WIN7, LINUX, etc... Used internally
#       in CIDBuild to drive platform specific compilation, as well as passed
#       on to the code that CIDBuild compiles, for the same purposes. It will
#       be converted to the form PLATFORM_XXX, So PLATFORM_WIN32_WIN7 and so
#       forth.
#
#   CID_SRCTREE should be set to the root of the src tree, e.g. X:\CID_Dev
#       Used to just set up all of the paths during the build of this
#       program.
#
#   The character mode is driven by the target platform. This in turn drives
#   the character mode that CIDBuild will apply to its target code that it
#   builds.
# ----------------------------------------------------------------------------


# ----------------------------------------------------------------------------
#   Limit the suffixes to what we want
# ----------------------------------------------------------------------------
.SUFFIXES:
.SUFFIXES: .cpp .obj .exe


# ----------------------------------------------------------------------------
#   Assume good env setup
# ----------------------------------------------------------------------------
ENVOK=Yes


# ----------------------------------------------------------------------------
#   Set up build flags according to dev/prod build mode, and validate that
#   its correct.
# ----------------------------------------------------------------------------
!IF "$(CID_BUILDMODE)" == "Prod"

CPPFLAGS=/O2 /DNDEBUG /MD /DBLDMODE_PROD
LINKFLAGS=

!ELSE IF "$(CID_BUILDMODE)" == "Dev"

CPPFLAGS=/Od /D_DEBUG /Zi /MDd /DBLDMODE_DEV
LINKFLAGS=/Debug

!ELSE

!MESSAGE CID_BUILDMODE must be set to Dev or Prod
ENVOK=No

!ENDIF

!IF "$(CID_SRCTREE)" == ""

!MESSAGE Must provide the CID_SRCTREE value
ENVOK=No

!ENDIF


# ----------------------------------------------------------------------------
#   Set up build flags according to the platform, and validate that its
#   valid.
# ----------------------------------------------------------------------------
!IF "$(CID_PLATFORM)" == "WIN32_WIN7"

CHARMODE=UNICODE
PLATDIR=Win32

!ELSE

!MESSAGE The $(CID_PLATFORM) platform is not supported by CIDBuild_Win32.Mak
ENVOK=No

!ENDIF


!IF "$(ENVOK)" == "Yes"


# ----------------------------------------------------------------------------
#   Now that the basic stuff is in place, lets do any subsequent setup
# ----------------------------------------------------------------------------
CPPFLAGS=$(CPPFLAGS) /DPLATFORM_$(CID_PLATFORM)


# ----------------------------------------------------------------------------
#   Build up the output directory
# ----------------------------------------------------------------------------
OUTDIR=$(CID_RESDIR)
TMPOUTDIR=$(CID_RESDIR)\CIDBuild.out


# ----------------------------------------------------------------------------
#   Set this to the base name of the specific tools driver for your
#   development environment. The stuff below will us it to compile for the
#   right one. Right now there is just Visual C++ driver.
# ----------------------------------------------------------------------------
TOOLDRVBASE=VCppDriver


# ----------------------------------------------------------------------------
#   A list of all of the Hpp and Obj files
#
#   If you want to create another Win32 tools driver and still use this
#   make file, then you'll need to provide some define for the base name of
#   the driver file and use it to create the right names in all of the lists
#   below. Right now there is just the VCppDriver, so this is not needed.
# ----------------------------------------------------------------------------
HPP_FILES=\
CIDBuild.hpp \
CIDBuild_BinFile.hpp \
CIDBuild_BlkParser.hpp \
CIDBuild_DependGraph.hpp \
CIDBuild_Facility.hpp \
CIDBuild_FileDepend.hpp \
CIDBuild_FindInfo.hpp \
CIDBuild_IDLinfo.hpp \
CIDBuild_Janitor.hpp \
CIDBuild_KeyValuePair.hpp \
CIDBuild_LineSpooler.hpp \
CIDBuild_List.hpp \
CIDBuild_MsgIdInfo.hpp \
CIDBuild_ProjectInfo.hpp \
CIDBuild_ProjectList.hpp \
CIDBuild_ResCompiler.hpp \
CIDBuild_String.hpp \
CIDBuild_RawStr.hpp \
CIDBuild_TextFile.hpp \
CIDBuild_ToolsDriver.hpp \
CIDBuild_Utils.hpp \
CIDBuild_VCppDriver.hpp \
$(PLATDIR)\CIDBuild_Platform.hpp

OBJ_FILES=\
$(TMPOUTDIR)\CIDBuild.obj \
$(TMPOUTDIR)\CIDBuild_BinFile.obj \
$(TMPOUTDIR)\CIDBuild_BlkParser.obj \
$(TMPOUTDIR)\CIDBuild_BinFile_Win32.obj \
$(TMPOUTDIR)\CIDBuild_DependGraph.obj \
$(TMPOUTDIR)\CIDBuild_Facility.obj \
$(TMPOUTDIR)\CIDBuild_Facility_Win32.obj \
$(TMPOUTDIR)\CIDBuild_FacilityActions.obj \
$(TMPOUTDIR)\CIDBuild_FileDepend.obj \
$(TMPOUTDIR)\CIDBuild_FindInfo.obj \
$(TMPOUTDIR)\CIDBuild_FindInfo_Win32.obj \
$(TMPOUTDIR)\CIDBuild_IDLInfo.obj \
$(TMPOUTDIR)\CIDBuild_LineSpooler.obj \
$(TMPOUTDIR)\CIDBuild_MsgIdInfo.obj \
$(TMPOUTDIR)\CIDBuild_ProjectInfo.obj \
$(TMPOUTDIR)\CIDBuild_ProjectInfo_Win32.obj \
$(TMPOUTDIR)\CIDBuild_ProjectList.obj \
$(TMPOUTDIR)\CIDBuild_RawStr.obj \
$(TMPOUTDIR)\CIDBuild_RawStr_Win32.obj \
$(TMPOUTDIR)\CIDBuild_Release.obj \
$(TMPOUTDIR)\CIDBuild_ResCompiler.obj \
$(TMPOUTDIR)\CIDBuild_ResCompiler_Dlg.obj \
$(TMPOUTDIR)\CIDBuild_ResCompiler_Msgs.obj \
$(TMPOUTDIR)\CIDBuild_ResCompiler_Resources.obj \
$(TMPOUTDIR)\CIDBuild_ResCompiler_Menu.obj \
$(TMPOUTDIR)\CIDBuild_String.obj \
$(TMPOUTDIR)\CIDBuild_TextFile.obj \
$(TMPOUTDIR)\CIDBuild_ToolsDriver.obj \
$(TMPOUTDIR)\CIDBuild_ToolsDriver_Win32.obj \
$(TMPOUTDIR)\CIDBuild_VCppDriver.obj \
$(TMPOUTDIR)\CIDBuild_Utils.obj \
$(TMPOUTDIR)\CIDBuild_Utils_Win32.obj



# ----------------------------------------------------------------------------
#   Kick off the dependencies
# ----------------------------------------------------------------------------
ALL :   $(CID_SRCTREE) \
        $(OUTDIR) \
        $(TMPOUTDIR) \
        $(OUTDIR)\CIDBuild.exe


CLEAN :
    -@erase $(TMPOUTDIR)\CIDBuild.obj
    -@erase $(TMPOUTDIR)\CIDBuild_BinFile.obj
    -@erase $(TMPOUTDIR)\CIDBuild_BlkParser.obj
    -@erase $(TMPOUTDIR)\CIDBuild_BinFile_Win32.obj
    -@erase $(TMPOUTDIR)\CIDBuild_DependGraph.obj
    -@erase $(TMPOUTDIR)\CIDBuild_Facility.obj
    -@erase $(TMPOUTDIR)\CIDBuild_Facility_Win32.obj
    -@erase $(TMPOUTDIR)\CIDBuild_FacilityActions.obj
    -@erase $(TMPOUTDIR)\CIDBuild_FileDepend.obj
    -@erase $(TMPOUTDIR)\CIDBuild_FindInfo.obj
    -@erase $(TMPOUTDIR)\CIDBuild_FindInfo_Win32.obj
    -@erase $(TMPOUTDIR)\CIDBuild_IDLInfo.obj
    -@erase $(TMPOUTDIR)\CIDBuild_LineSpooler.obj
    -@erase $(TMPOUTDIR)\CIDBuild_MsgIdInfo.obj
    -@erase $(TMPOUTDIR)\CIDBuild_ProjectInfo.obj
    -@erase $(TMPOUTDIR)\CIDBuild_ProjectInfo_Win32.obj
    -@erase $(TMPOUTDIR)\CIDBuild_ProjectList.obj
    -@erase $(TMPOUTDIR)\CIDBuild_RawStr.obj
    -@erase $(TMPOUTDIR)\CIDBuild_RawStr_Win32.obj
    -@erase $(TMPOUTDIR)\CIDBuild_Release.obj
    -@erase $(TMPOUTDIR)\CIDBuild_ResCompiler.obj
    -@erase $(TMPOUTDIR)\CIDBuild_ResCompiler_Dlg.obj
    -@erase $(TMPOUTDIR)\CIDBuild_ResCompiler_Msgs.obj
    -@erase $(TMPOUTDIR)\CIDBuild_ResCompiler_Resources.obj
    -@erase $(TMPOUTDIR)\CIDBuild_ResCompiler_Menu.obj
    -@erase $(TMPOUTDIR)\CIDBuild_String.obj
    -@erase $(TMPOUTDIR)\CIDBuild_TextFile.obj
    -@erase $(TMPOUTDIR)\CIDBuild_ToolsDriver.obj
    -@erase $(TMPOUTDIR)\CIDBuild_ToolsDriver_Win32.obj
    -@erase $(TMPOUTDIR)\CIDBuild_VCppDriver.obj
    -@erase $(TMPOUTDIR)\CIDBuild_Utils.obj
    -@erase $(TMPOUTDIR)\CIDBuild_Utils_Win32.obj
    -@erase $(TMPOUTDIR)\CIDMsgs.obj
    -@erase $(OUTDIR)\CIDBuild.*


# ----------------------------------------------------------------------------
#   Explicit link rule
# ----------------------------------------------------------------------------
$(OUTDIR)\CIDBuild.exe  :  $(OBJ_FILES)
    Link /nologo /subsystem:console /machine:I386 \
         /pdb:$(OUTDIR)\CIDBuild.Pdb /map:$(OUTDIR)\CIDBuild.Map \
         /out:$(OUTDIR)\CIDBuild.exe $(LINKFLAGS) $(OBJ_FILES)


$(TMPOUTDIR)\CIDBuild.obj                   : CIDBuild.cpp $(HPP_FILES)

$(TMPOUTDIR)\CIDBuild_BinFile.obj           : CIDBuild_BinFile.cpp $(HPP_FILES)

$(TMPOUTDIR)\CIDBuild_BlkParser.obj         : CIDBuild_BlkParser.cpp $(HPP_FILES)

$(TMPOUTDIR)\CIDBuild_BinFile_Win32.obj     : $(PLATDIR)\CIDBuild_BinFile_Win32.cpp $(HPP_FILES)

$(TMPOUTDIR)\CIDBuild_DependGraph.obj       : CIDBuild_DependGraph.cpp $(HPP_FILES)

$(TMPOUTDIR)\CIDBuild_Facility.obj          : CIDBuild_Facility.cpp $(HPP_FILES)

$(TMPOUTDIR)\CIDBuild_Facility_Win32.obj    : $(PLATDIR)\CIDBuild_Facility_Win32.cpp $(HPP_FILES)

$(TMPOUTDIR)\CIDBuild_FacilityActions.obj   : CIDBuild_FacilityActions.cpp $(HPP_FILES)

$(TMPOUTDIR)\CIDBuild_FileDepend.obj        : CIDBuild_FileDepend.cpp $(HPP_FILES)

$(TMPOUTDIR)\CIDBuild_FindInfo.obj          : CIDBuild_FindInfo.cpp $(HPP_FILES)

$(TMPOUTDIR)\CIDBuild_FindInfo_Win32.obj    : $(PLATDIR)\CIDBuild_FindInfo_Win32.cpp $(HPP_FILES)

$(TMPOUTDIR)\CIDBuild_IDLInfo.obj           : CIDBuild_IDLInfo.cpp $(HPP_FILES)

$(TMPOUTDIR)\CIDBuild_LineSpooler.obj       : CIDBuild_LineSpooler.cpp $(HPP_FILES)

$(TMPOUTDIR)\CIDBuild_MsgIdInfo.obj         : CIDBuild_MsgIdInfo.cpp $(HPP_FILES)

$(TMPOUTDIR)\CIDBuild_ProjectInfo.obj       : CIDBuild_ProjectInfo.cpp $(HPP_FILES)

$(TMPOUTDIR)\CIDBuild_ProjectInfo_Win32.obj : $(PLATDIR)\CIDBuild_ProjectInfo_Win32.cpp $(HPP_FILES)

$(TMPOUTDIR)\CIDBuild_ProjectList.obj       : CIDBuild_ProjectList.cpp $(HPP_FILES)

$(TMPOUTDIR)\CIDBuild_RawStr.obj            : CIDBuild_RawStr.cpp $(HPP_FILES)

$(TMPOUTDIR)\CIDBuild_RawStr_Win32.obj      : $(PLATDIR)\CIDBuild_RawStr_Win32.cpp $(HPP_FILES)

$(TMPOUTDIR)\CIDBuild_Release.obj           : CIDBuild_Release.cpp $(HPP_FILES)

$(TMPOUTDIR)\CIDBuild_ResCompiler.obj       : CIDBuild_ResCompiler.cpp $(HPP_FILES)

$(TMPOUTDIR)\CIDBuild_ResCompiler_Dlg.obj   : CIDBuild_ResCompiler_Dlg.cpp $(HPP_FILES)

$(TMPOUTDIR)\CIDBuild_ResCompiler_Msgs.obj  : CIDBuild_ResCompiler_Msgs.cpp $(HPP_FILES)

$(TMPOUTDIR)\CIDBuild_ResCompiler_Resources.obj  : CIDBuild_ResCompiler_Resources.cpp $(HPP_FILES)

$(TMPOUTDIR)\CIDBuild_ResCompiler_Menu.obj  : CIDBuild_ResCompiler_Menu.cpp $(HPP_FILES)

$(TMPOUTDIR)\CIDBuild_String.obj            : CIDBuild_String.cpp $(HPP_FILES)

$(TMPOUTDIR)\CIDBuild_TextFile.obj          : CIDBuild_TextFile.cpp $(HPP_FILES)

$(TMPOUTDIR)\CIDBuild_ToolsDriver.obj       : CIDBuild_ToolsDriver.cpp $(HPP_FILES)

$(TMPOUTDIR)\CIDBuild_ToolsDriver_Win32.obj : $(PLATDIR)\CIDBuild_ToolsDriver_Win32.cpp $(HPP_FILES)

$(TMPOUTDIR)\CIDBuild_VCppDriver.obj        : CIDBuild_VCppDriver.cpp $(HPP_FILES)

$(TMPOUTDIR)\CIDBuild_Utils.obj             : CIDBuild_Utils.cpp $(HPP_FILES)

$(TMPOUTDIR)\CIDBuild_Utils_Win32.obj       : $(PLATDIR)\CIDBuild_Utils_Win32.cpp $(HPP_FILES)


# ----------------------------------------------------------------------------
# Insure that all the output directories get created during a fresh build
# and that our pre-compiled headers file gets updated.
# ----------------------------------------------------------------------------
$(CID_SRCTREE) :
    if not exist $(CID_SRCTREE)\$(NULL) mkdir $(CID_SRCTREE)

$(OUTDIR) : $(CID_SRCTREE)
    if not exist $(OUTDIR)\$(NULL) mkdir $(OUTDIR)

$(TMPOUTDIR) : $(OUTDIR)
    if not exist $(TMPOUTDIR)\$(NULL) mkdir $(TMPOUTDIR)


# ----------------------------------------------------------------------------
#   Inference rule for creating the Obj files
# ----------------------------------------------------------------------------
.cpp{$(TMPOUTDIR)}.obj:
    cl $(CPPFLAGS) /Za /Zc:forScope,wchar_t /EHsc /W3 /nologo /c /D_WINDOWS /D_CONSOLE \
        /Fd$(TMPOUTDIR)\ /DNTDDI_VERSION=NTDDI_WIN7 /Fo$(TMPOUTDIR)\ $<

{$(PLATDIR)}.cpp{$(TMPOUTDIR)}.obj:
    cl $(CPPFLAGS) /EHsc /Zc:forScope,wchar_t /W3 /nologo /c /D_WINDOWS /D_CONSOLE \
        /Fd$(TMPOUTDIR)\ /DNTDDI_VERSION=NTDDI_WIN7 /Fo$(TMPOUTDIR)\ $<

!ENDIF
