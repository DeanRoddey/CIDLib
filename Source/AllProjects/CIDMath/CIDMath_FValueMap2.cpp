//
// FILE NAME: CIDMath_FValueMap2.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/08/1996
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
//  This file implements a couple of simple derivatives of the abstract
//  TFValueMap class.
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
#include    "CIDMath_.hpp"


// ---------------------------------------------------------------------------
//  Do our standard members macros
// ---------------------------------------------------------------------------
RTTIDecls(TFLogMap,TFValueMap)



// ---------------------------------------------------------------------------
//  CLASS: TFLogMap
// PREFIX: vmap
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFLogMap: Constructors and Destructors
// ---------------------------------------------------------------------------
TFLogMap::TFLogMap(const tCIDMath::ELogTypes eLogType) :

    m_eLogType(eLogType)
{
    #if CID_DEBUG_ON
    if (!bIsValidEnum(eLogType))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadEnumValue
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(tCIDLib::TCard4(eLogType))
            , TString(L"ELogTypes")
        );
    }
    #endif
}

TFLogMap::~TFLogMap()
{
}


// ---------------------------------------------------------------------------
//  TFLogMap: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TFloat8 TFLogMap::f8MapValue(const tCIDLib::TFloat8 f8ToMap) const
{
    tCIDLib::TFloat8 f8Ret = 0;
    if (m_eLogType == tCIDMath::ELogTypes::Natural)
        f8Ret = TMathLib::f8Log(f8ToMap);
    else if (m_eLogType == tCIDMath::ELogTypes::Base10)
        f8Ret = TMathLib::f8Log10(f8ToMap);

    return f8Ret;
}

