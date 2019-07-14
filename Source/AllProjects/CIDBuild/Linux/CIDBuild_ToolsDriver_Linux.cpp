//
// FILE NAME: CIDBuild_ToolsDriver_Linux.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/24/1999
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
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
//  TToolsDriver: Protected, non-virtual methods
// ---------------------------------------------------------------------------

// Nothing to do yet for Linux
tCIDLib::TVoid
TToolsDriver::PostLink( const   TProjectInfo&   projiTarget
                        , const TBldStr&        strTarget)
{
}


// ---------------------------------------------------------------------------
//  TToolsDriver: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TToolsDriver::BuildLibName( const   TBldStr&    strTargetProject
                            ,       TBldStr&    strToFill)
{
    strToFill = L"lib";
    strToFill.Append(strTargetProject);
    strToFill.Append(kCIDBuild::pszDllExt);
}

