//
// FILE NAME: CIDLib_Duplicable.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/06/1992
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file just implements a couple statics of the duplicable class.
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
//   CLASS: MDuplicable
//  PREFIX: mdup
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  MDuplicable: Public, static methods
// ---------------------------------------------------------------------------
MDuplicable& MDuplicable::Nul_MDuplicable()
{
    //
    //  We need some object to represent this and it's not a concrete class
    //  so we just use a point, which implements this interface.
    //
    static TPoint* pmdupNull = nullptr;
    if (!pmdupNull)
    {
        TBaseLock lockInit;
        if (!pmdupNull)
            TRawMem::pExchangePtr(pmdupNull, new TPoint);
    }
    return *pmdupNull;
}


tCIDLib::TVoid
MDuplicable::TestCanDupTo(  const   MDuplicable* const  pmdupToCast
                            , const TClass&             clsToCastTo)
{
    // The source cannot be a null pointer
    if (!pmdupToCast)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcDup_NulSrc
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , clsToCastTo
        );
    }

    if (!pmdupToCast->bIsDescendantOf(clsToCastTo))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcRTTI_BadCast
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , pmdupToCast->clsIsA()
            , clsToCastTo
        );
    }
}

tCIDLib::TVoid
MDuplicable::TestCanDupTo(  const   MDuplicable&    mdupToCast
                            , const TClass&         clsToCastTo)
{
    // Just call the other version that takes a pointer
    TestCanDupTo(&mdupToCast, clsToCastTo);
}

tCIDLib::TVoid
MDuplicable::TestCanDupTo(  const   MDuplicable&        mdupToCast
                            , const tCIDLib::TCh* const pszClassName)
{
    // Find the class object for this name first
    TClass clsToCastTo = TClass::clsForClass(pszClassName);

    // Just call the other version
    TestCanDupTo(&mdupToCast, clsToCastTo);
}

tCIDLib::TVoid
MDuplicable::TestCanDupTo(  const   MDuplicable* const  pmdupToCast
                            , const tCIDLib::TCh* const pszClassName)
{
    // Find the class object for this name first
    TClass clsToCastTo = TClass::clsForClass(pszClassName);

    // And call the other version
    TestCanDupTo(pmdupToCast, clsToCastTo);
}

