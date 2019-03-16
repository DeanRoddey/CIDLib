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
