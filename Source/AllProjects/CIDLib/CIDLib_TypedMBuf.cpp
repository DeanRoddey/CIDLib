//
// FILE NAME: CIDLib_TypedMBuf.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/16/2007
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
//  This file implements the base typed buffer class that we use jut to
//  keep some common code out of the templatized stuff.
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
#include    "CIDLib_.hpp"


// ---------------------------------------------------------------------------
//  Do our standard members macros
// ---------------------------------------------------------------------------
RTTIDecls(TTypedMBufBase,TObject)



// ---------------------------------------------------------------------------
//   CLASS: TTypedMBufBase
//  PREFIX: tmbuf
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTypedMBufBase: Destructor
// ---------------------------------------------------------------------------
TTypedMBufBase::~TTypedMBufBase()
{
}


// ---------------------------------------------------------------------------
//  TTypedMBufBase: Hidden Constructors
// ---------------------------------------------------------------------------
TTypedMBufBase::TTypedMBufBase()
{
}


// ---------------------------------------------------------------------------
//  TTypedMBufBase: Protected, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TTypedMBufBase::ThrowNotReady(const tCIDLib::TCard4 c4Line) const
{
    facCIDLib().ThrowErr
    (
        CID_FILE
        , c4Line
        , kCIDErrs::errcMBuf_BufferNotReady
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::NotReady
    );
}

