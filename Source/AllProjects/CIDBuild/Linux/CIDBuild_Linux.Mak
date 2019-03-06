# ----------------------------------------------------------------------------
#  FILE: CIDBuild_Linux.Mak
#
#   This file builds the CIDBuild and CIDMsgs programs for the Linux
#   subsystem. CIDMsgs is just a standalone version of the message compiler
#   that is also part of CIDBuild. For those folks who want to use their own
#   IDE to build CIDLib programs, they still need a message compiler that is
#   separate from CIDBuild's project system.
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
#
#   CID_COMPILER should either be KCC or gcc [Default: KCC]
#       This allows one to choose one's favorite compiler. For now, only KCC
#       will actually build anything. This is because gcc does not support
#       ANSI C++. If gcc ever provides support for the standard, we're ready!
#
#   The character mode is driven by the target platform. This in turn drives
#   the character mode that CIDBuild will apply to its target code that it
#   builds.
# ----------------------------------------------------------------------------


#
# Variables for which we already know the value
#
CID_PLATFORM := LINUX
CID_CHARMODE := ASCII
RM := rm -fv
MKTREE := mkdir -p --verbose
RMTREE := rm -fvr
SRC_EXT := .cpp

#
# Variables for which we'll guess default values, if
# they're not already set
#
ifeq ($(findstring $(CID_BUILDMODE),Dev Prod),)
  CID_BUILDMODE := Dev
endif
ifeq ($(findstring $(CID_COMPILER),KCC gcc),)
  CID_COMPILER := KCC
endif
ifeq ($(origin CID_SRCTREE),undefined)
  CID_SRCTREE := $(HOME)/src/CIDLib
endif

#
# We need all the CID-specific make flags for recursive invocation.
#
CID_MAKEFLAGS := CID_BUILDMODE=$(CID_BUILDMODE) \
                 CID_COMPILER=$(CID_COMPILER) \
                 CID_SRCTREE=$(CID_SRCTREE)

#
# Directory variables
#
SRC_DIR := $(CID_SRCTREE)/Source/AllProjects/CIDBuild
OUT_DIR := $(CID_SRCTREE)/$(CID_BUILDMODE)Result
PROJ_OUT_DIR := $(OUT_DIR)/CIDBuild.Out
PLAT_DIR := Linux

#
# File name stems
# These will be used to build the source and object file names
#
# Shared stems are used in both executables
SHARED_STEMS := \
	CIDBuild_BinFile \
	CIDBuild_FindInfo \
	CIDBuild_LineSpooler \
	CIDBuild_MsgProcessor \
	CIDBuild_RawStr \
	CIDBuild_String \
	CIDBuild_TextFile \
	CIDBuild_Utils
SHARED_PLAT_STEMS := \
	CIDBuild_BinFile_Linux \
	CIDBuild_FindInfo_Linux \
	CIDBuild_RawStr_Linux \
	CIDBuild_Utils_Linux
# Build tools stems are only used in the build tool
BUILD_TOOL_STEMS := \
	CIDBuild \
	CIDBuild_DependGraph \
	CIDBuild_Facility \
	CIDBuild_FacilityActions \
	CIDBuild_FileDepend \
	CIDBuild_GCCDriver \
	CIDBuild_KCCDriver \
	CIDBuild_ProjectInfo \
	CIDBuild_ProjectList \
	CIDBuild_Release \
	CIDBuild_ToolsDriver \
	CIDBuild_VCppDriver
BUILD_TOOL_PLAT_STEMS := \
	CIDBuild_Facility_Linux \
	CIDBuild_ProjectInfo_Linux \
	CIDBuild_ToolsDriver_Linux
# Message compiler stems are only used in the message compiler
MSG_COMP_STEMS := \
	CIDMsgs

	
#
# Object file names
#
BUILD_TOOL_OBJECTS := $(addprefix $(PROJ_OUT_DIR)/,\
                        $(addsuffix .o,$(SHARED_STEMS) \
                                       $(SHARED_PLAT_STEMS) \
                                       $(BUILD_TOOL_STEMS) \
                                       $(BUILD_TOOL_PLAT_STEMS)) \
                       )
MSG_COMP_OBJECTS := $(addprefix $(PROJ_OUT_DIR)/,\
                      $(addsuffix .o,$(SHARED_STEMS) \
                                     $(SHARED_PLAT_STEMS) \
                                     $(MSG_COMP_STEMS)) \
                     )
# We use the sort function here because it provides the side-effect
# of eliminating duplicate entries.
OBJECTS := $(sort $(BUILD_TOOL_OBJECTS) $(MSG_COMP_OBJECTS))

#
# Source file names
#
BUILD_TOOL_SOURCES := $(addsuffix $(SRC_EXT),$(SHARED_STEMS) $(BUILD_TOOL_STEMS)) \
                      $(addprefix $(PLAT_DIR)/,\
		        $(addsuffix $(SRC_EXT),$(SHARED_PLAT_STEMS) \
                                               $(BUILD_TOOL_PLAT_STEMS)) \
                       )
MSG_COMP_SOURCES := $(addsuffix $(SRC_EXT),$(SHARED_STEMS) $(MSG_COMP_STEMS)) \
                    $(addprefix $(PLAT_DIR)/,$(addsuffix $(SRC_EXT),$(SHARED_PLAT_STEMS)))
# We use the sort function here because it provides the side-effect
# of eliminating duplicate entries.
SOURCES := $(sort $(BUILD_TOOL_SOURCES) $(MSG_COMP_SOURCES))

#
# Dependency file name
#
DEP_FILE := $(PLAT_DIR)/CIDBuild_Linux.Dep

#
# This make file name
#
MAKE_FILE := $(PLAT_DIR)/CIDBuild_Linux.Mak

#
# Build tool file name
#
BUILD_TOOL := $(OUT_DIR)/CIDBuild

#
# Message compiler file name
#
MSG_COMP := $(OUT_DIR)/CIDMsgs

#
# Compiler and linker flags
#
ifeq ($(CID_COMPILER),KCC)
  ifeq ($(CID_BUILDMODE),Dev)
    CPPFLAGS := +K0 -DBLDMODE_DEV
    LFLAGS := +K0
  else
    CPPFLAGS := +K3 -DBLDMODE_PROD
    LFLAGS := +K3
  endif
  CPPFLAGS += -DNO_PACK_WARNING --diag_suppress 608,68
else
  ifeq ($(CID_BUILDMODE),Dev)
    CPPFLAGS := -g -DBLDMODE_DEV
    LFLAGS := -g
  else
    CPPFLAGS := -O3 -DBLDMODE_PROD
    LFLAGS := -O3
  endif
endif
CPPFLAGS += -DPLATFORM_$(CID_PLATFORM) -DCHARMODE_$(CID_CHARMODE)
DEP_GEN_FLAGS := -M

#
# Declare the phony targets
#
.PHONY: non-specific-target default-target dep clean real-clean dirs execs

#
# Now it's time for some rules
#
# This one is first to facilitate automatically generating dependencies if they
# don't already exist.
non-specific-target: default-target

#
# If the dependency file doesn't exist, then make the default
# target build it, otherwise include it and build the executables.
#
ifeq ($(DEP_FILE),$(wildcard $(DEP_FILE)))
include $(DEP_FILE)
default-target: execs
else
default-target:
	@$(MAKE) -f $(MAKE_FILE) $(CID_MAKEFLAGS) dep
	@$(MAKE) -f $(MAKE_FILE) $(CID_MAKEFLAGS)
endif

# This target generates a dependency file
dep:
	@echo
	@echo "# Generating the dependency file..."
	@echo "# Automatically generated CIDBuild dependency file" > $(DEP_FILE)
	@echo "# $(shell date)" >> $(DEP_FILE)
	@$(CID_COMPILER) $(DEP_GEN_FLAGS) $(SOURCES) >> $(DEP_FILE)
	@echo
	@echo "# The dependency file has been generated. You may now build the project"
	@echo "# by invoking 'make' with no target specified."
	@echo

# This removes the object files and executables, but leaves the directories
# and the dependency file
clean:
	@$(RM) $(OBJECTS) $(BUILD_TOOL) $(MSG_COMP)

# This removes everything
real-clean: clean
	@$(RMTREE) $(PROJ_OUT_DIR)
	@$(RM) $(DEP_FILE)

# This creates the directories that we'll need
dirs:
	@$(MKTREE) $(PROJ_OUT_DIR)

# This generates the objects and executables
execs: $(BUILD_TOOL) $(MSG_COMP)

# Here is the build tool target
$(BUILD_TOOL): dirs $(BUILD_TOOL_OBJECTS)
	$(CID_COMPILER) $(LFLAGS) -o $(@) $(BUILD_TOOL_OBJECTS)

# Here is the message compiler target
$(MSG_COMP): dirs $(MSG_COMP_OBJECTS)
	$(CID_COMPILER) $(LFLAGS) -o $(@) $(MSG_COMP_OBJECTS)

# These are pattern rules for generating object files
$(PROJ_OUT_DIR)/%.o: %$(SRC_EXT)
	$(CID_COMPILER) -c $(CPPFLAGS) -o $(@) $(<)

$(PROJ_OUT_DIR)/%.o: $(PLAT_DIR)/%$(SRC_EXT)
	$(CID_COMPILER) -c $(CPPFLAGS) -o $(@) $(<)

