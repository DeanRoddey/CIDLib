//
// FILE NAME: CIDSIP_Transact.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/28/2013
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
//  This file implements the TSIPTransact class.
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
#include    "CIDSIP_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TSIPTransact, TObject)



// ---------------------------------------------------------------------------
//   CLASS: SIPTransact
//  PREFIX: sipdlg
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TSIPTransact: Constructors and Destructor
// ---------------------------------------------------------------------------
TSIPTransact::TSIPTransact()
{
    Reset();
}

TSIPTransact::~TSIPTransact()
{
}


// ---------------------------------------------------------------------------
//  TSIPTransact: Constructors and Destructor
// ---------------------------------------------------------------------------

tCIDLib::TVoid TSIPTransact::Reset()
{
    // Generate a branch id
    m_strBranchID = L"z9hG4bK";
    m_strBranchID.Append(TUniqueId::strMakeId());
}


