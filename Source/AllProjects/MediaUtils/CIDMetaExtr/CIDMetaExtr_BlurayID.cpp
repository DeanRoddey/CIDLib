//
// FILE NAME: CIDMetaExtr_BlurayID.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/29/2011
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
//  This file implements the base meta extractor interface.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDMetaExtr_.hpp"


// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TCIDBlurayID,TObject)


// ---------------------------------------------------------------------------
//  Local types and constants
// ---------------------------------------------------------------------------
namespace CIDMetaExtr_BlurayID
{
}



// ---------------------------------------------------------------------------
//  CLASS: TCIDBlurayID
// PREFIX: fac
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDBlurayID: Constructors and Destructor
// ---------------------------------------------------------------------------
TCIDBlurayID::TCIDBlurayID()
{
}

TCIDBlurayID::~TCIDBlurayID()
{
}


// ---------------------------------------------------------------------------
//  TCIDBlurayID: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TCIDBlurayID::GenBlurayID(  const   TString&            strBlurayPath
                            ,       tCIDLib::TCard8&    c8ID
                            ,       TString&            strFmtID)
{
    c8ID = 0;
    strFmtID.Clear();

    //
    //  Read in the index and movie object files if they exist and hash
    //  then.
    //
}

