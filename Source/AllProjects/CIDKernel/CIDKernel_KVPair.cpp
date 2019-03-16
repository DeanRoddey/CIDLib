//
// FILE NAME: CIDKernel_KVPair.Cpp
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
//  This file implements a simple key/value pair class, for returning info
//  in this form to the CIDLib layer.
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
//   CLASS: TKrnlKVPair
//  PREFIX: kkvp
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlKVPair: Constuctors and Destructor
// ---------------------------------------------------------------------------
TKrnlKVPair::TKrnlKVPair() :

    m_pszKey(TRawStr::pszReplicate(L""))
    , m_pszValue(TRawStr::pszReplicate(L""))
    , m_pszValue2(TRawStr::pszReplicate(L""))
{
}

TKrnlKVPair::TKrnlKVPair(const  tCIDLib::TCh* const pszKey
                        , const tCIDLib::TCh* const pszValue) :

    m_pszKey(TRawStr::pszReplicate(pszKey))
    , m_pszValue(TRawStr::pszReplicate(pszValue))
    , m_pszValue2(TRawStr::pszReplicate(L""))
{
}

TKrnlKVPair::TKrnlKVPair(const  tCIDLib::TCh* const pszKey
                        , const tCIDLib::TCh* const pszValue
                        , const tCIDLib::TCh* const pszValue2) :

    m_pszKey(TRawStr::pszReplicate(pszKey))
    , m_pszValue(TRawStr::pszReplicate(pszValue))
    , m_pszValue2(TRawStr::pszReplicate(pszValue2))
{
}

TKrnlKVPair::TKrnlKVPair(const TKrnlKVPair& kkvpToCopy) :

    m_pszKey(TRawStr::pszReplicate(kkvpToCopy.m_pszKey))
    , m_pszValue(TRawStr::pszReplicate(kkvpToCopy.m_pszValue))
    , m_pszValue2(TRawStr::pszReplicate(kkvpToCopy.m_pszValue2))
{
}

TKrnlKVPair::~TKrnlKVPair()
{
    delete [] m_pszKey;
    delete [] m_pszValue;
    delete [] m_pszValue2;
}


// ---------------------------------------------------------------------------
//  TKrnlKVPair: Public operators
// ---------------------------------------------------------------------------
TKrnlKVPair& TKrnlKVPair::operator=(const TKrnlKVPair& kkvpToAssign)
{
    if (&kkvpToAssign != this)
    {
        delete [] m_pszKey;
        delete [] m_pszValue;
        delete [] m_pszValue2;

        m_pszKey = TRawStr::pszReplicate(kkvpToAssign.m_pszKey);
        m_pszValue = TRawStr::pszReplicate(kkvpToAssign.m_pszValue);
        m_pszValue2 = TRawStr::pszReplicate(kkvpToAssign.m_pszValue2);
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TKrnlKVPair: Public, non-virtual methods
// ---------------------------------------------------------------------------
const tCIDLib::TCh* TKrnlKVPair::pszKey() const
{
    return m_pszKey;
}


const tCIDLib::TCh* TKrnlKVPair::pszValue() const
{
    return m_pszValue;
}


const tCIDLib::TCh* TKrnlKVPair::pszValue2() const
{
    return m_pszValue2;
}


tCIDLib::TVoid TKrnlKVPair::Set(const   tCIDLib::TCh* const pszKey
                                , const tCIDLib::TCh* const pszValue)
{
    delete [] m_pszKey;
    delete [] m_pszValue;

    m_pszKey = TRawStr::pszReplicate(pszKey);
    m_pszValue = TRawStr::pszReplicate(pszValue);
}

tCIDLib::TVoid TKrnlKVPair::Set(const   tCIDLib::TCh* const pszKey
                                , const tCIDLib::TCh* const pszValue
                                , const tCIDLib::TCh* const pszValue2)
{
    delete [] m_pszKey;
    delete [] m_pszValue;
    delete [] m_pszValue2;

    m_pszKey = TRawStr::pszReplicate(pszKey);
    m_pszValue = TRawStr::pszReplicate(pszValue);
    m_pszValue2 = TRawStr::pszReplicate(pszValue2);
}


tCIDLib::TVoid TKrnlKVPair::SetKey(const tCIDLib::TCh* const pszKey)
{
    delete [] m_pszKey;
    m_pszKey = TRawStr::pszReplicate(pszKey);
}


tCIDLib::TVoid TKrnlKVPair::SetValue(const tCIDLib::TCh* const pszValue)
{
    delete [] m_pszValue;
    m_pszValue = TRawStr::pszReplicate(pszValue);
}

tCIDLib::TVoid TKrnlKVPair::SetValue2(const tCIDLib::TCh* const pszValue)
{
    delete [] m_pszValue2;
    m_pszValue2 = TRawStr::pszReplicate(pszValue);
}

