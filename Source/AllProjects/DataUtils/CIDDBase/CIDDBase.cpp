//
// FILE NAME: CIDDBase.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/21/2003
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and
//  the author (Dean Roddey.) It is licensed under the MIT Open Source
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This is the main file of the facility. It provides the lazy faulting in
//  method for our facility object.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//

// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDDBase_.hpp"


// ---------------------------------------------------------------------------
//  Global functions
// ---------------------------------------------------------------------------

//
//  The lazy evaluation method for the facility object. This is the only
//  access to the facility object, insuring that it gets allocated when
//  its required.
//
TFacCIDDBase& facCIDDBase()
{
    static TFacCIDDBase facCIDDBase;
    return facCIDDBase;
}

