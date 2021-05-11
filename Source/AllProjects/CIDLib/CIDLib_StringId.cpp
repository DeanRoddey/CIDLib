//
// FILE NAME: CIDLib_StringId.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/25/2015
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
//  This file implements the TStringId class.
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
//   CLASS: TStringId
//  PREFIX: bts
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TStringId: Constructors and Destructor
// ---------------------------------------------------------------------------
TStringId::TStringId() :

    m_bFlag(kCIDLib::False)
    , m_c4Id(0)
{
}

TStringId::TStringId(const TStringView& strvText, const tCIDLib::TCard4 c4Id) :

    m_bFlag(kCIDLib::False)
    , m_c4Id(c4Id)
    , m_strText(strvText)
{
}

TStringId::~TStringId()
{
}


// ---------------------------------------------------------------------------
//  TStringId: Public operators
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TStringId::operator==(const TStringId& stridSrc) const
{
    if (&stridSrc == this)
        return kCIDLib::True;

    return
    (
        (m_bFlag != stridSrc.m_bFlag)
        || (m_c4Id != stridSrc.m_c4Id)
        || (m_strText != stridSrc.m_strText)
    );
}

tCIDLib::TBoolean TStringId::operator!=(const TStringId& stridSrc) const
{
    return !operator==(stridSrc);
}


// ---------------------------------------------------------------------------
//  Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TStringId::bFlag() const
{
    return m_bFlag;
}

tCIDLib::TBoolean TStringId::bFlag(const tCIDLib::TBoolean bToSet)
{
    m_bFlag = bToSet;
    return m_bFlag;
}


tCIDLib::TCard4 TStringId::c4Id() const
{
    return m_c4Id;
}

tCIDLib::TCard4 TStringId::c4Id(const tCIDLib::TCard4 c4ToSet)
{
    m_c4Id = c4ToSet;
    return m_c4Id;
}


const TString& TStringId::strText() const
{
    return m_strText;
}

const TString& TStringId::strText(const TStringView& strvToSet)
{
    m_strText = strvToSet;
    return m_strText;
}



