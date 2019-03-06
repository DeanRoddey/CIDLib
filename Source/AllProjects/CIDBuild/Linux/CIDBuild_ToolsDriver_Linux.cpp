//
// FILE NAME: CIDBuild_ToolsDriver_Linux.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/24/1999
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file provides the Linux specific parts of the TToolsDriver.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "../CIDBuild.hpp"



// ---------------------------------------------------------------------------
//  TToolsDriver: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDBuild::TVoid
TToolsDriver::__BuildLibName(const  TBldStr&    strTargetProject
                            ,       TBldStr&    strToFill)
{
    strToFill = L"lib";
    strToFill.Append(strTargetProject);
    strToFill.Append(kCIDBuild::pszDllExt);
}
