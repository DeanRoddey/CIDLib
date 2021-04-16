# ----------------------------------------------------------------------------
#  FILE: CIDBuild_Linux.Mak
#
#   This file builds the CIDBuild program for the Linux subsystem.
#
#   It expects some values to come in from the environment. There is more
#   stuff than this set by the standard SetCIDEnv file, but this is all
#   that we require here. If any of these variables is not set, this make file
#   will set default values.
#
#   CID_BUILDMODE should be set to either Dev or Prod [Default: Dev]
#       It is used to drive many compilation and link flags. One of them is
#       the BLDMODE_XXX flag which is used internally in CIDBuild to drive
#       build mode specific code. CIDBuild also passes this flag on to the
#       code it compiles, which uses it for the same purposes. The XXX is
#       the value of this variable.
#
#   CID_SRCTREE should be set to the root of the source tree [Default: $HOME/src/CIDLib]
#       Used to just set up all of the paths during the build of this
#       program.
# ----------------------------------------------------------------------------


#
# Variables for which we already know the value
#
PLATDIR=Linux
RM := rm -fv
MKTREE := mkdir -p --verbose
RMTREE := rm -fvr
SRC_EXT := .cpp
HDR_EXT := .hpp
OUR_OUTDIR := $(CID_RESDIR)/CIDBuild.Out


ifeq ($(findstring $(CID_BUILDMODE),Dev Prod),)
badenv :
	@echo The build mode must be Dev or Prod
	exit 1
endif
ifeq ($(origin CID_SRCTREE),undefined)
badenv :
	@echo The CID_SRCTREE environment variable was not found
	exit 1
endif


# Set up build flags for the build mode
ifeq ($(CID_BUILDMODE), Dev)
	BUILD_FLAGS := -c -g -DBLDMODE_DEV -D_DEBUG
	LINK_FLAGS := -g
else
	BUILD_FLAGS := -c -O3 -DBLDMODE_PROD
	LINK_FLAGS := -O3
endif

BUILD_FLAGS += -DPLATFORM_LINUX -std=c++17 -m32
LINK_FLAGS += -lstdc++ -m32
# LINK_FLAGS += -lstdc++

# The list of headers. We just use them all for each file
SRC_HEADERS=\
CIDBuild.hpp \
CIDBuild_BinFile.hpp \
CIDBuild_BlkParser.hpp \
CIDBuild_DependGraph.hpp \
CIDBuild_Facility.hpp \
CIDBuild_FileDepend.hpp \
CIDBuild_FindInfo.hpp \
CIDBuild_IDLInfo.hpp \
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
CIDBuild_GCCDriver.hpp \
$(PLATDIR)/CIDBuild_Platform.hpp

# The cpp files we need to build
SRC_STEMS=\
CIDBuild \
CIDBuild_BinFile \
CIDBuild_BlkParser \
CIDBuild_DependGraph \
CIDBuild_Facility \
CIDBuild_FacilityActions \
CIDBuild_FileDepend \
CIDBuild_FindInfo \
CIDBuild_IDLInfo \
CIDBuild_GCCDriver \
CIDBuild_LineSpooler \
CIDBuild_MsgIdInfo \
CIDBuild_ProjectInfo \
CIDBuild_ProjectList \
CIDBuild_RawStr \
CIDBuild_Release \
CIDBuild_ResCompiler \
CIDBuild_ResCompiler_Dlg \
CIDBuild_ResCompiler_Menu \
CIDBuild_ResCompiler_Msgs \
CIDBuild_ResCompiler_Resources \
CIDBuild_String \
CIDBuild_TextFile \
CIDBuild_ToolsDriver \
CIDBuild_Utils

PLAT_STEMS=\
CIDBuild_BinFile_Linux \
CIDBuild_Facility_Linux \
CIDBuild_FindInfo_Linux \
CIDBuild_ProjectInfo_Linux \
CIDBuild_RawStr_Linux \
CIDBuild_ToolsDriver_Linux \
CIDBuild_Utils_Linux

#
# Create the source files with the .o extension to define
# the target files in the output directory.
#
OBJ_FILES := $(addprefix $(OUR_OUTDIR)/, $(addsuffix .o, $(SRC_STEMS)))
OBJ_FILES += $(addprefix $(OUR_OUTDIR)/, $(addsuffix .o, $(PLAT_STEMS)))

# And add the source extension to create the source files
#SRC_FILES := $(addsuffix $(SRC_EXT), $(SRC_STEMS)) 
#SRC_FILES += $(addprefix $(PLATDIR)/, $(addsuffix $(SRC_EXT), $(PLAT_STEMS)))

.SUFFIXES : .cpp, .o

# Declare the phony targets
.PHONY: all dirs showlist

#showlist :
#	@echo $(SRC_HEADERS)

all : dirs $(CID_RESDIR)/CIDBuild


$(CID_RESDIR)/CIDBuild : $(OBJ_FILES)
	g++ $(LINK_FLAGS) -o $(CID_RESDIR)/CIDBuild $^

$(OUR_OUTDIR)/%.o: ./%.cpp $(SRC_HEADERS)
	g++ $(BUILD_FLAGS) $< -o $@

$(OUR_OUTDIR)/%.o: ./Linux/%.cpp $(SRC_HEADERS)
	g++ $(BUILD_FLAGS) $< -o $@

# Make sure our output directories are created
dirs :
	@$(MKTREE) $(OUR_OUTDIR)

