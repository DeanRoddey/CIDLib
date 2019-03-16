//
// FILE NAME: CIDLib_Keyops.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/26/2001
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
//  This file implements the out of line parts of the standard key ops clases.
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
//  Do our standard RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TStringKeyOps,TObject)



// ---------------------------------------------------------------------------
//   CLASS: TStringKeyOps
//  PREFIX: kops
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TStringKeyOps: Constructors and destructor
// ---------------------------------------------------------------------------
TStringKeyOps::TStringKeyOps() :

    m_bCase(kCIDLib::True)
{
}

TStringKeyOps::TStringKeyOps(const tCIDLib::TBoolean bCase) :

    m_bCase(bCase)
{
}


TStringKeyOps::TStringKeyOps(const TStringKeyOps& kopsSrc) :

    m_bCase(kopsSrc.m_bCase)
{
}

TStringKeyOps::~TStringKeyOps()
{
}


// ---------------------------------------------------------------------------
//  TStringKeyOps: Public operators
// ---------------------------------------------------------------------------
TStringKeyOps& TStringKeyOps::operator=(const TStringKeyOps& kopsSrc)
{
    if (this != &kopsSrc)
        m_bCase = kopsSrc.m_bCase;
    return *this;
}


// ---------------------------------------------------------------------------
//  TStringKeyOps: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TStringKeyOps::bCompKeys(const TString& str1, const TString& str2) const
{
    if (m_bCase)
        return (str1.eCompare(str2) == tCIDLib::ESortComps::Equal);

    return (str1.eCompareI(str2) == tCIDLib::ESortComps::Equal);
}

tCIDLib::THashVal
TStringKeyOps::hshKey(  const   TString&        strToHash
                        , const tCIDLib::TCard4 c4Modulus) const
{
    if (m_bCase)
        return strToHash.hshCalcHash(c4Modulus);

    // Create a temp and lower case it and hash that
    m_strTmp = strToHash;
    m_strTmp.ToLower();
    return m_strTmp.hshCalcHash(c4Modulus);
}

