//
// FILE NAME: CIDMetaExtr_MetaExtractor.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/24/2007
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
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
RTTIDecls(TCIDMetaExtractor,TObject)



// ---------------------------------------------------------------------------
//  CLASS: TCIDMetaExtractor
// PREFIX: fac
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDMetaExtractor: Constructors and Destructor
// ---------------------------------------------------------------------------
TCIDMetaExtractor::TCIDMetaExtractor()
{
}

TCIDMetaExtractor::~TCIDMetaExtractor()
{
}


// ---------------------------------------------------------------------------
//  TCIDMetaExtractor: Protected, non-virtual methods
// ---------------------------------------------------------------------------

// Extracts value from the passed stream, flipping them if required
tCIDLib::TCard2 TCIDMetaExtractor::c2Extract(TBinInStream& strmSrc)
{
    tCIDLib::TCard2 c2Ret;
    strmSrc >> c2Ret;

    #if defined(CIDLIB_LITTLEENDIAN)
    c2Ret = TRawBits::c2SwapBytes(c2Ret);
    #endif

    return c2Ret;
}


tCIDLib::TCard4 TCIDMetaExtractor::c4Extract(TBinInStream& strmSrc)
{
    tCIDLib::TCard4 c4Ret;
    strmSrc >> c4Ret;

    #if defined(CIDLIB_LITTLEENDIAN)
    c4Ret = TRawBits::c4SwapBytes(c4Ret);
    #endif

    return c4Ret;
}


tCIDLib::TCard8 TCIDMetaExtractor::c8Extract(TBinInStream& strmSrc)
{
    tCIDLib::TCard8 c8Ret;
    strmSrc >> c8Ret;

    #if defined(CIDLIB_LITTLEENDIAN)
    c8Ret = TRawBits::c8SwapBytes(c8Ret);
    #endif

    return c8Ret;
}


// These throw some common errors that derived types will all need to throw
tCIDLib::TVoid TCIDMetaExtractor::ThrowNotOfType()
{
    facCIDMetaExtr().ThrowErr
    (
        CID_FILE
        , CID_LINE
        , kMExtrErrs::errcFmt_NotOfType
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::Format
        , strTypeName()
    );
}

