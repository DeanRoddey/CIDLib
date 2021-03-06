#!/bin/bash
# A simple helper script to go to specific CIDLib directories quickly
# It takes one or two parameters
#
#  ccd target [subdir]
#
function setdir() {
    tardir=$1
    if [ -z $tardir ] || [ "$tardir" == "" ]; then
        echo You must provide a target directory
        return 1
    fi

    chdir=""
    case $tardir in
        "cidbuild") chdir=${CID_SRCTREE}/Source/AllProjects/CIDBuild
        ;;

        "cidlib") chdir=${CID_SRCTREE}/Source/AllProjects/CIDLib
        ;;

        "cidkernel") chdir=${CID_SRCTREE}/Source/AllProjects/CIDKernel
        ;;                

        "cmd") chdir=${CID_SRCTREE}/Source/Cmd/Linux
        ;;

        "src") chdir=${CID_SRCTREE}/Source
        ;;

        "output") chdir=${CID_SRCTREE}/Output
        ;;

        "res") chdir=${CID_RESDIR}
        ;;

        "root") chdir=${CID_SRCTREE}
        ;;

        "tests") chdir=${CID_SRCTREE}/Source/AllProjects/Tests
        ;;

        "tests2") chdir=${CID_SRCTREE}/Source/AllProjects/Tests2
        ;;

        "top") chdir=${CID_DEVTREE}
        ;;

    esac

    # echo Dir=$chdir
    if [ "$chdir" != "" ]; then
            cd $chdir
    fi
}

# Make the shell just run the incoming parms
"$@"

