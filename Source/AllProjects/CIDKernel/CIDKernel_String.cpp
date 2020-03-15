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

TKrnlString::TKrnlString(   const   tCIDLib::TCh* const pszValue1
                            , const tCIDLib::TCh* const pszValue2) :

    m_pszValue(nullptr)
{
    const tCIDLib::TCard4 c4Len = TRawStr::c4StrLen(pszValue1)
                                  + TRawStr::c4StrLen(pszValue2);
    m_pszValue = new tCIDLib::TCh[c4Len + 1];

    // We know it's long enough so just copy over ourself for maximum efficiency
    tCIDLib::TCh* pszTar = m_pszValue;
    const tCIDLib::TCh* pszSrc = pszValue1;
    while (*pszSrc)
        *pszTar++ = *pszSrc++;

    pszSrc = pszValue2;
    while (*pszSrc)
        *pszTar++ = *pszSrc++;

    *pszTar = kCIDLib::chNull;
}

TKrnlString::TKrnlString(   const   tCIDLib::TCh* const pszValue1
                            , const tCIDLib::TCh* const pszValue2
                            , const tCIDLib::TCh* const pszValue3) :

    m_pszValue(nullptr)
{
    const tCIDLib::TCard4 c4Len = TRawStr::c4StrLen(pszValue1)
                                  + TRawStr::c4StrLen(pszValue2)
                                  + TRawStr::c4StrLen(pszValue3);
    m_pszValue = new tCIDLib::TCh[c4Len + 1];

    // We know it's long enough so just copy over ourself for maximum efficiency
    tCIDLib::TCh* pszTar = m_pszValue;
    const tCIDLib::TCh* pszSrc = pszValue1;
    while (*pszSrc)
        *pszTar++ = *pszSrc++;

    pszSrc = pszValue2;
    while (*pszSrc)
        *pszTar++ = *pszSrc++;

    pszSrc = pszValue3;
    while (*pszSrc)
        *pszTar++ = *pszSrc++;

    *pszTar = kCIDLib::chNull;
}

TKrnlString::TKrnlString(   const   tCIDLib::TCh* const pszValue1
                            , const tCIDLib::TCh* const pszValue2
                            , const tCIDLib::TCh* const pszValue3
                            , const tCIDLib::TCh* const pszValue4) :

    m_pszValue(nullptr)
{
    const tCIDLib::TCard4 c4Len = TRawStr::c4StrLen(pszValue1)
                                  + TRawStr::c4StrLen(pszValue2)
                                  + TRawStr::c4StrLen(pszValue3)
                                  + TRawStr::c4StrLen(pszValue4);

    m_pszValue = new tCIDLib::TCh[c4Len + 1];

    // We know it's long enough so just copy over ourself for maximum efficiency
    tCIDLib::TCh* pszTar = m_pszValue;
    const tCIDLib::TCh* pszSrc = pszValue1;
    while (*pszSrc)
        *pszTar++ = *pszSrc++;

    pszSrc = pszValue2;
    while (*pszSrc)
        *pszTar++ = *pszSrc++;

    pszSrc = pszValue3;
    while (*pszSrc)
        *pszTar++ = *pszSrc++;

    pszSrc = pszValue4;
    while (*pszSrc)
        *pszTar++ = *pszSrc++;

    *pszTar = kCIDLib::chNull;
}

TKrnlString::TKrnlString(const TKrnlString& kstrSrc) :

    m_pszValue(TRawStr::pszReplicate(kstrSrc.m_pszValue))
{
}

TKrnlString::TKrnlString(TKrnlString&& kstrSrc) :

    m_pszValue(new tCIDLib::TCh[1])
{
    *m_pszValue = kCIDLib::chNull;
    tCIDLib::Swap(m_pszValue, kstrSrc.m_pszValue);
}

TKrnlString::~TKrnlString()
{
    delete [] m_pszValue;
}


// ---------------------------------------------------------------------------
//  TKrnlString: Public operators
// ---------------------------------------------------------------------------

tCIDLib::TBoolean TKrnlString::operator==(const TKrnlString& kstrSrc) const
{
    return TRawStr::bCompareStr(m_pszValue, kstrSrc.m_pszValue);
}


TKrnlString& TKrnlString::operator=(const TKrnlString& kstrSrc)
{
    if (this != &kstrSrc)
    {
        delete [] m_pszValue;
        m_pszValue = nullptr;
        m_pszValue = TRawStr::pszReplicate(kstrSrc.m_pszValue);
    }
    return *this;
}

TKrnlString& TKrnlString::operator=(TKrnlString&& kstrSrc)
{
    if (this != &kstrSrc)
    {
        tCIDLib::Swap(m_pszValue, kstrSrc.m_pszValue);
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
tCIDLib::TVoid TKrnlString::Append(const tCIDLib::TCh* const pszSrc)
{
    const tCIDLib::TCard4 c4Len = TRawStr::c4StrLen(m_pszValue) + TRawStr::c4StrLen(pszSrc);
    tCIDLib::TCh* pszNew = new tCIDLib::TCh[c4Len + 1];
    TArrayJanitor<tCIDLib::TCh> janNew(pszNew);
    TRawStr::CopyCatStr(pszNew, c4Len, m_pszValue, pszSrc);

    delete [] m_pszValue;
    m_pszValue = janNew.paOrphan();
}


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


tCIDLib::TVoid
TKrnlString::Combine(const tCIDLib::TCh* const pszSrc, const tCIDLib::TCh chSepChar)
{
    // If no source value, then nothing to do
    if (!pszSrc)
        return;
    const tCIDLib::TCard4 c4SrcLen = TRawStr::c4StrLen(pszSrc);
    if (!c4SrcLen)
        return;

    // We have to create a new string
    const tCIDLib::TCard4 c4CurLen = TRawStr::c4StrLen(m_pszValue);

    // Allocate an extra in case we need to add a sep char
    tCIDLib::TCard4 c4NewLen =  c4SrcLen + c4CurLen + 1;
    tCIDLib::TCh* pszNew = new tCIDLib::TCh[c4NewLen + 1];
    TArrayJanitor<tCIDLib::TCh> janNew(pszNew);

    // Copy over the current value
    TRawStr::CopyStr(pszNew, m_pszValue, c4NewLen);

    // Make sure our new one ends with the sep char
    tCIDLib::TCh* pszTar = pszNew + c4CurLen;
    if (!c4CurLen || (m_pszValue[c4CurLen - 1] != chSepChar))
        *pszTar++ = chSepChar;

    // And cat the incoming onto it, skipping any sep char
    const tCIDLib::TCh* pszRealSrc = pszSrc;
    if (c4SrcLen && (*pszRealSrc == chSepChar))
        pszRealSrc++;
    TRawStr::CopyStr(pszTar, pszSrc, c4NewLen);

    // Now we can trash the old one and store the new one
    delete [] m_pszValue;
    m_pszValue = janNew.paOrphan();
}


const tCIDLib::TCh* TKrnlString::pszValue() const
{
    return m_pszValue;
}


tCIDLib::TVoid TKrnlString::Reallocate(const tCIDLib::TCard4 c4MinSize)
{
    delete [] m_pszValue;
    m_pszValue = nullptr;
    m_pszValue = new tCIDLib::TCh[c4MinSize];
    m_pszValue[0] = kCIDLib::chNull;
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

