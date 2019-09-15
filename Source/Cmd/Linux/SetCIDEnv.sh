#
# This file does the environmental setup for CIDLib on Linux
# 
# All we expect here is:
#
#   CID_DEVTREE     - Set to the top of the dev tree, which is the
#                     CID_Dev directory.
#   CID_BUILDMODE   - Set to Prod or Dev
# 

# Make sure our expected incoming values are present and valid
if [ -z "$CID_DEVTREE" ]; then
    echo "You must set the CID_DEVTREE variable"
    return
fi

if [ -z "$CID_BUILDMODE" ]; then
    echo "You must set the CID_BUILDMODE variable"
    return
fi

if [ "$CID_BUILDMODE" = "Dev" ] || [ "$CID_BUILDMODE" = "Prod"] ; then
    echo "Setting up for $CID_BUILDMODE build mode"
else
    echo "CID_BUILDMODE must be set to Dev or Prod (case sensitive)"
    return
fi

# 
# Set the CIDLib version to apply when building, and create a version
# with underlines instead of dots.
#
export CID_VERSION=5.4.903
export CID_VERSIONUL=${CID_VERSION//./_}

# Set the platform, which for now is fixed at Linux_32
export CID_PLATFORM=Linux_32

# Set some key directories relative to the incoming dev tree
export CID_SRCTREE=${CID_DEVTREE}/CID_Dev
export CID_RESDIR=${CID_DEVTREE}/CID_Dev/Output/${CID_VERSIONUL}/${CID_BUILDMODE}Result

#
# This one is always pointed at the CIDLib stuff, so we can't use CID_SRCTREE, even
# though it's the same on CIDLib itself.
#
export CIDLIB_SRCDIR=${CID_SRCTREE}

# We need our result directory to be in the path, and our command directory
PATH=$PATH:${CID_RESDIR}:${CID_SRCTREE}/Source/Cmd/Linux

# Set our terminal title to reflect what we are set up for
echo -ne "\033]0;CIDLib[${CID_BUILDMODE}, ${CID_VERSION}]\007"

alias="cidccd.sh setdir"

