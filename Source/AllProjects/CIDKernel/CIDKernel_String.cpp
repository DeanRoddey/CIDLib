//
// FILE NAME: CIDKernel_String.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/10/2011
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
//  This file implements a simple class, to allow us to safely return lists
//  of string values to the CIDLib layer (and a couple others that talk
//  directly to us.)
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
#include    "CIDKernel_.hpp"


// ---------------------------------------------------------------------------
//   CLASS: TKrnlString
//  PREFIX: kkvp
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlString: Constuctors and Destructor
// ---------------------------------------------------------------------------
TKrnlString::TKrnlString() :

    m_pszValue(TRawStr::pszReplicate(L""))
{
}

TKrnlString::TKrnlString(const  tCIDLib::TCh* const pszValue) :

    m_pszValue(TRawStr::pszReplicate(pszValue))
{
}

TKrnlString::TKrnlString(const TKrnlString& kstrToCopy) :

    m_pszValue(TRawStr::pszReplicate(kstrToCopy.m_pszValue))
{
}

TKrnlString::~TKrnlString()
{
    delete [] m_pszValue;
}


// ---------------------------------------------------------------------------
//  TKrnlString: Public operators
// ---------------------------------------------------------------------------
TKrnlString& TKrnlString::operator=(const TKrnlString& kstrToAssign)
{
    if (&kstrToAssign != this)
    {
        delete [] m_pszValue;
        m_pszValue = nullptr;
        m_pszValue = TRawStr::pszReplicate(kstrToAssign.m_pszValue);
    }
    return *this;
}


TKrnlString& TKrnlString::operator=(const tCIDLib::TCh* const pszRawStr)
{
    delete [] m_pszValue;
    m_pszValue = TRawStr::pszReplicate(pszRawStr);
    return *this;
}


TKrnlString& TKrnlString::operator=(const tCIDLib::TSCh* const pszRawSStr)
{
    delete [] m_pszValue;
    m_pszValue = TRawStr::pszConvert(pszRawSStr);
    return *this;
}


// ---------------------------------------------------------------------------
//  TKrnlString: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TBoolean TKrnlString::bIsEmpty() const
{
    return !(*m_pszValue);
}


tCIDLib::TCard4 TKrnlString::c4Length() const
{
    return TRawStr::c4StrLen(m_pszValue);
}


tCIDLib::TVoid TKrnlString::Clear()
{
    m_pszValue[0] = kCIDLib::chNull;
}


const tCIDLib::TCh* TKrnlString::pszValue() const
{
    return m_pszValue;
}


tCIDLib::TVoid TKrnlString::Set(const tCIDLib::TCh* const pszToSet)
{
    delete [] m_pszValue;
    m_pszValue = nullptr;
    m_pszValue = TRawStr::pszReplicate(pszToSet);
}


// In this case it's not null terminated or they don't want all of it
tCIDLib::TVoid
TKrnlString::Set(const tCIDLib::TCh* const pszToSet, const tCIDLib::TCard4 c4Count)
{
    delete [] m_pszValue;
    m_pszValue = nullptr;
    m_pszValue = new tCIDLib::TCh[c4Count + 1];

    tCIDLib::TCh* pszTar = m_pszValue;
    const tCIDLib::TCh* pszSrc = pszToSet;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        *pszTar++ = *pszSrc++;

    // Add the null
    *pszTar = kCIDLib::chNull;
}


tCIDLib::TVoid TKrnlString::SetFromShort(const tCIDLib::TSCh* const pszToSet)
{
    delete [] m_pszValue;
    m_pszValue = nullptr;
    m_pszValue = TRawStr::pszConvert(pszToSet);
}

