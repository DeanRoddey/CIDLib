//
// FILE NAME: CIDLib_TypedMBuf.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/16/2007
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
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

