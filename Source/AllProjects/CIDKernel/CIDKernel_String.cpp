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
#include    "CIDKernel_PlatformStrOps.hpp"


// ---------------------------------------------------------------------------
//   CLASS: TKrnlString
//  PREFIX: kkvp
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlString: Constuctors and Destructor
// ---------------------------------------------------------------------------
TKrnlString::TKrnlString(const tCIDLib::TCard4 c4BufSz) :

    m_c4BufSz(c4BufSz)
    , m_c4Length(0)
    , m_pszValue(nullptr)
{
    if (!m_c4BufSz)
        m_c4BufSz = 8;

    m_pszValue = new tCIDLib::TCh[m_c4BufSz];
    m_pszValue[0] = kCIDLib::chNull;
}

TKrnlString::TKrnlString(const  tCIDLib::TCh* const pszValue) :

    m_c4BufSz(0)
    , m_c4Length(0)
    , m_pszValue(nullptr)
{
    if (pszValue)
    {
        m_c4Length = TRawStr::c4StrLen(pszValue);
        m_c4BufSz = m_c4Length;
        m_pszValue = new tCIDLib::TCh[m_c4BufSz + 1];
        TRawStr::CopyStr(m_pszValue, pszValue, m_c4BufSz);
    }
    else
    {
        m_c4BufSz = 8;
        m_pszValue = new tCIDLib::TCh[m_c4BufSz + 1];
        m_pszValue[0] = kCIDLib::chNull;
    }
}


TKrnlString::TKrnlString(const  tCIDLib::TSCh* const pszValue) :

    m_c4BufSz(0)
    , m_c4Length(0)
    , m_pszValue(nullptr)
{
    if (pszValue)
    {
        m_pszValue = TRawStr::pszConvert(pszValue);
        m_c4Length = TRawStr::c4StrLen(m_pszValue);
        m_c4BufSz = m_c4Length;
    }
    else
    {
        m_c4BufSz = 32;
        m_pszValue = new tCIDLib::TCh[m_c4BufSz + 1];
        m_pszValue[0] = kCIDLib::chNull;
    }
}

TKrnlString::TKrnlString(const  tCIDLib::TCh* const pszSrc1
                        , const tCIDLib::TCh* const pszSrc2
                        , const tCIDLib::TCh* const pszSrc3
                        , const tCIDLib::TCh* const pszSrc4
                        , const tCIDLib::TCh* const pszSrc5) :
    m_c4BufSz(0)
    , m_c4Length(0)
    , m_pszValue(nullptr)
{
    tCIDLib::TCard4 c4SrcLen1 = 0;
    tCIDLib::TCard4 c4SrcLen2 = 0;
    tCIDLib::TCard4 c4SrcLen3 = 0;
    tCIDLib::TCard4 c4SrcLen4 = 0;
    tCIDLib::TCard4 c4SrcLen5 = 0;
    if (pszSrc1)
        c4SrcLen1 = TRawStr::c4StrLen(pszSrc1);
    if (pszSrc2)
        c4SrcLen2 = TRawStr::c4StrLen(pszSrc2);
    if (pszSrc3)
        c4SrcLen3 = TRawStr::c4StrLen(pszSrc3);
    if (pszSrc4)
        c4SrcLen4 = TRawStr::c4StrLen(pszSrc4);
    if (pszSrc5)
        c4SrcLen5 = TRawStr::c4StrLen(pszSrc5);

    const tCIDLib::TCard4 c4NewChars = c4SrcLen1 + c4SrcLen2 + c4SrcLen3 + c4SrcLen4 + c4SrcLen5;

    if (c4NewChars)
    {
        m_pszValue = new tCIDLib::TCh[c4NewChars + 1];
        m_c4BufSz = c4NewChars;
        m_c4Length = c4NewChars;

        // And now we can copy each one onto the end as we got
        tCIDLib::TCard4 c4At = 0;
        if (pszSrc1)
        {
            TRawMem::CopyMemBuf(&m_pszValue[c4At], pszSrc1, c4SrcLen1 * kCIDLib::c4CharBytes);
            c4At += c4SrcLen1;
        }
        if (pszSrc2)
        {
            TRawMem::CopyMemBuf(&m_pszValue[c4At], pszSrc2, c4SrcLen2 * kCIDLib::c4CharBytes);
            c4At += c4SrcLen2;
        }
        if (pszSrc3)
        {
            TRawMem::CopyMemBuf(&m_pszValue[c4At], pszSrc3, c4SrcLen3 * kCIDLib::c4CharBytes);
            c4At += c4SrcLen3;
        }
        if (pszSrc4)
        {
            TRawMem::CopyMemBuf(&m_pszValue[c4At], pszSrc4, c4SrcLen4 * kCIDLib::c4CharBytes);
            c4At += c4SrcLen4;
        }
        if (pszSrc5)
        {
            TRawMem::CopyMemBuf(&m_pszValue[c4At], pszSrc5, c4SrcLen5 * kCIDLib::c4CharBytes);
            c4At += c4SrcLen5;
        }

        m_pszValue[c4At] = kCIDLib::chNull;
    }
     else
    {
        // They are all null or empty, so just create an empty string
        m_c4BufSz = 8;
        m_pszValue = new tCIDLib::TCh[m_c4BufSz + 1];
        m_c4Length = 0;
        m_pszValue[0] = kCIDLib::chNull;

    }
}

TKrnlString::TKrnlString(const TKrnlString& kstrSrc) :

    m_c4BufSz(kstrSrc.m_c4BufSz)
    , m_c4Length(kstrSrc.m_c4Length)
    , m_pszValue(nullptr)
{
    m_pszValue = new tCIDLib::TCh[m_c4BufSz + 1];

    // Copy over any existing content, plus null
    TRawMem::CopyMemBuf
    (
        m_pszValue, kstrSrc.m_pszValue, (kstrSrc.m_c4Length  + 1) * kCIDLib::c4CharBytes
    );
}

TKrnlString::TKrnlString(TKrnlString&& kstrSrc) :

    m_c4BufSz(0)
    , m_c4Length(0)
    , m_pszValue(new tCIDLib::TCh[1])
{
    m_pszValue[0] = kCIDLib::chNull;
    *this = tCIDLib::ForceMove(kstrSrc);
}

TKrnlString::~TKrnlString()
{
    delete [] m_pszValue;
    m_pszValue = nullptr;
}


// ---------------------------------------------------------------------------
//  TKrnlString: Public operators
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKrnlString::operator==(const TKrnlString& kstrSrc) const
{
    // If different lengths, can't be equal
    if (m_c4Length != kstrSrc.m_c4Length)
        return kCIDLib::False;

    return TRawStr::bCompareStrN(m_pszValue, kstrSrc.m_pszValue, m_c4Length);
}


TKrnlString& TKrnlString::operator=(const TKrnlString& kstrSrc)
{
    if (this != &kstrSrc)
        SetNewBuffer(kstrSrc.m_pszValue, kstrSrc.m_c4Length, 0);
    return *this;
}

TKrnlString& TKrnlString::operator=(TKrnlString&& kstrSrc)
{
    if (this != &kstrSrc)
    {
        tCIDLib::Swap(m_c4BufSz, kstrSrc.m_c4BufSz);
        tCIDLib::Swap(m_c4Length, kstrSrc.m_c4Length);
        tCIDLib::Swap(m_pszValue, kstrSrc.m_pszValue);
    }
    return *this;
}


TKrnlString& TKrnlString::operator=(const tCIDLib::TCh* const pszRawStr)
{
    const tCIDLib::TCard4 c4SrcLen = TRawStr::c4StrLen(pszRawStr);
    SetNewBuffer(pszRawStr,c4SrcLen, 0);
    return *this;
}


TKrnlString& TKrnlString::operator=(const tCIDLib::TSCh* const pszRawSStr)
{
    if (!bSetNewSBuffer(pszRawSStr, 0))
    {
        // <TBD> We need to be able to do something with this
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TKrnlString: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TKrnlString::AdoptBuffer(           tCIDLib::TCh*&  pszSrc
                            , const tCIDLib::TCard4 c4BufSz)
{
    delete [] m_pszValue;
    m_c4BufSz = c4BufSz;
    m_c4Length = TRawStr::c4StrLen(pszSrc);
    m_pszValue = pszSrc;
    pszSrc = nullptr;
}

tCIDLib::TVoid TKrnlString::Append(const tCIDLib::TCh* const pszSrc)
{
    const tCIDLib::TCard4 c4SrcLen = TRawStr::c4StrLen(pszSrc);
    ExpandBuf(m_c4Length + c4SrcLen, kCIDLib::True);

    // Copy the new stuff onto the end, plus 1 to get the null
    TRawMem::CopyMemBuf(&m_pszValue[m_c4Length], pszSrc, (c4SrcLen + 1) * kCIDLib::c4CharBytes);

    // And increase our length value by what we added
    m_c4Length += c4SrcLen;
}


tCIDLib::TVoid TKrnlString::Append(const TKrnlString& kstrSrc)
{
    ExpandBuf(m_c4Length + kstrSrc.m_c4Length, kCIDLib::True);

    // Copy the new stuff onto the end, plus 1 to get the null
    TRawMem::CopyMemBuf
    (
        &m_pszValue[m_c4Length]
        , kstrSrc.m_pszValue
        , (kstrSrc.m_c4Length + 1) * kCIDLib::c4CharBytes
    );

    // And increase our length value by what we added
    m_c4Length += kstrSrc.m_c4Length;
}


tCIDLib::TVoid
TKrnlString::Append(const   tCIDLib::TCh* const pszSrc1
                    , const tCIDLib::TCh* const pszSrc2
                    , const tCIDLib::TCh* const pszSrc3
                    , const tCIDLib::TCh* const pszSrc4)
{
    tCIDLib::TCard4 c4SrcLen1 = 0;
    tCIDLib::TCard4 c4SrcLen2 = 0;
    tCIDLib::TCard4 c4SrcLen3 = 0;
    tCIDLib::TCard4 c4SrcLen4 = 0;
    if (pszSrc1)
        c4SrcLen1 = TRawStr::c4StrLen(pszSrc1);
    if (pszSrc2)
        c4SrcLen2 = TRawStr::c4StrLen(pszSrc2);
    if (pszSrc3)
        c4SrcLen3 = TRawStr::c4StrLen(pszSrc3);
    if (pszSrc4)
        c4SrcLen4 = TRawStr::c4StrLen(pszSrc4);

    const tCIDLib::TCard4 c4NewChars = c4SrcLen1 + c4SrcLen2 + c4SrcLen3 + c4SrcLen4;

    // If nothing to add, we are done
    if (!c4NewChars)
        return;

    // Expand if needed
    ExpandBuf(m_c4Length + c4NewChars, kCIDLib::True);

    // And now we can copy each one onto the end as we got
    tCIDLib::TCard4 c4At = m_c4Length;
    if (pszSrc1)
    {
        TRawMem::CopyMemBuf(&m_pszValue[c4At], pszSrc1, c4SrcLen1 * kCIDLib::c4CharBytes);
        c4At += c4SrcLen1;
    }
    if (pszSrc2)
    {
        TRawMem::CopyMemBuf(&m_pszValue[c4At], pszSrc2, c4SrcLen2 * kCIDLib::c4CharBytes);
        c4At += c4SrcLen2;
    }
    if (pszSrc3)
    {
        TRawMem::CopyMemBuf(&m_pszValue[c4At], pszSrc3, c4SrcLen3 * kCIDLib::c4CharBytes);
        c4At += c4SrcLen3;
    }
    if (pszSrc4)
    {
        TRawMem::CopyMemBuf(&m_pszValue[c4At], pszSrc4, c4SrcLen4 * kCIDLib::c4CharBytes);
        c4At += c4SrcLen4;
    }

    // And add the null finally
    if (c4At != m_c4Length + c4NewChars)
    {
        // <TBD> We need to do something
    }

    m_pszValue[c4At] = kCIDLib::chNull;
    m_c4Length += c4NewChars;
}


// If the passed char is not the last char, then append it
tCIDLib::TVoid TKrnlString::AppendIfNotAlready(const tCIDLib::TCh chSrc)
{
    if (!m_c4Length || (m_pszValue[m_c4Length - 1] != chSrc))
    {
        ExpandBuf(m_c4Length + 1, kCIDLib::True);
        m_pszValue[m_c4Length] = chSrc;
        m_c4Length++;
        m_pszValue[m_c4Length] = kCIDLib::chNull;
    }
}


tCIDLib::TBoolean TKrnlString::bCompareI(const tCIDLib::TCh* const pszSrc) const
{
    return TRawStr::bCompareStrI(m_pszValue, pszSrc);
}

tCIDLib::TBoolean TKrnlString::bCompareI(const TKrnlString& kstrSrc) const
{
    if (m_c4Length != kstrSrc.m_c4Length)
        return kCIDLib::False;

    return TRawStr::bCompareStrNI(m_pszValue, kstrSrc.m_pszValue, m_c4Length);
}


tCIDLib::TBoolean
TKrnlString::bCompareNI(const   tCIDLib::TCh* const pszSrc
                        , const tCIDLib::TCard4     c4Count) const
{
    return TRawStr::bCompareStrNI(m_pszValue, pszSrc, c4Count);
}

tCIDLib::TBoolean
TKrnlString::bCompareNI(const   TKrnlString&    kstrSrc
                        , const tCIDLib::TCard4 c4Count) const
{
    return TRawStr::bCompareStrNI(m_pszValue, kstrSrc.m_pszValue, c4Count);
}


tCIDLib::TVoid TKrnlString::Clear()
{
    m_c4Length = 0;
    m_pszValue[0] = kCIDLib::chNull;
}


//
//  If the source is null or empty, we don't add a sep char to the end of this
//  string. We only add one if actually combining something!
//
tCIDLib::TVoid
TKrnlString::Combine(const tCIDLib::TCh* const pszSrc, const tCIDLib::TCh chSepChar)
{
    // If no source value, then nothing to do
    if (!pszSrc)
        return;
    const tCIDLib::TCard4 c4SrcLen = TRawStr::c4StrLen(pszSrc);
    if (!c4SrcLen)
        return;

    // Expand the buffer, including one extra in case we need to add the sep
    ExpandBuf(m_c4Length + c4SrcLen + 1, kCIDLib::True);

    // Add a sep char if not there already
    AppendIfNotAlready(chSepChar);

    // And cat the incoming onto it, skipping any sep char
    const tCIDLib::TCh* pszRealSrc = pszSrc;
    if (c4SrcLen && (*pszRealSrc == chSepChar))
        pszRealSrc++;

    Append(pszRealSrc);
}


// Replicates our buffer and returns a pointer that the caller is responsible for
tCIDLib::TCh* TKrnlString::pszReplicate() const
{
    // WE copy length + 1 to get the null!
    tCIDLib::TCh* pszRet = new tCIDLib::TCh[m_c4Length + 1];
    TRawMem::CopyMemBuf(pszRet, m_pszValue, (m_c4Length + 1) * kCIDLib::c4CharBytes);
    return pszRet;
}


// Same as above, but at a specific position
tCIDLib::TCh* TKrnlString::pszReplicateAt(const tCIDLib::TCard4 c4At) const
{
    if (c4At >= m_c4Length)
        return nullptr;
    return TRawStr::pszReplicate(&m_pszValue[c4At]);
}


// Orphans out our buffer to the caller, leaving us with an empty string
tCIDLib::TCh*
TKrnlString::pszOrphanBuffer(tCIDLib::TCard4& c4Size, tCIDLib::TCard4& c4Length)
{
    // Allocate us a new one just big enough for an empty string
    tCIDLib::TCh* pszNew = new tCIDLib::TCh[1];
    pszNew[0] = kCIDLib::chNull;

    c4Size = m_c4BufSz;
    c4Length = m_c4Length;
    tCIDLib::TCh* pszRet = m_pszValue;
    m_pszValue = pszNew;
    m_c4Length = 0;
    m_c4BufSz = 0;

    return pszRet;
}


const tCIDLib::TCh* TKrnlString::pszValue() const
{
    return m_pszValue;
}


const tCIDLib::TCh* TKrnlString::pszValueAt(const tCIDLib::TCard4 c4At) const
{
    if (c4At >= m_c4Length)
        return nullptr;

    return &m_pszValue[c4At];
}


tCIDLib::TVoid TKrnlString::Reallocate(const tCIDLib::TCard4 c4MinSize)
{
    ExpandBuf(c4MinSize, kCIDLib::False);
}


tCIDLib::TVoid
TKrnlString::SetWithExtra(  const   tCIDLib::TCh* const pszToSet
                            , const tCIDLib::TCard4     c4Extra)
{
    SetNewBuffer(pszToSet, TRawStr::c4StrLen(pszToSet), c4Extra);
}


tCIDLib::TVoid TKrnlString::SetFromShort(const tCIDLib::TSCh* const pszToSet)
{
    if (!bSetNewSBuffer(pszToSet, 0))
    {
        // <TBD> We need to do something with this
    }
}


//
//  In this case it's not null terminated or they don't want all of it. We have
//  to trust that they are giving us a valid count.
//
tCIDLib::TVoid
TKrnlString::SetUnterminated(const  tCIDLib::TCh* const pszToSet
                            , const tCIDLib::TCard4     c4Count)
{
     ExpandBuf(c4Count, kCIDLib::False);

    TRawMem::CopyMemBuf(m_pszValue, pszToSet, c4Count * kCIDLib::c4CharBytes);
    m_pszValue[c4Count] = kCIDLib::chNull;
    m_c4Length = c4Count;
}




// ---------------------------------------------------------------------------
//  TKrnlString: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  A helper for when we need to add more text, and may need to expand the buffer
//  if there's not enough room. We can keep the old contents or discard it. We
//  will expand by a minimum of 32 chars, to avoid excessive reallocations for
//  small appends.
//
tCIDLib::TVoid
TKrnlString::ExpandBuf( const   tCIDLib::TCard4     c4Needed
                        , const tCIDLib::TBoolean   bKeepOld)
{
    if (c4Needed > m_c4BufSz)
    {
        // Set a minimium expansion
        tCIDLib::TCard4 c4RealNeed = c4Needed;
        if ((c4Needed - m_c4BufSz) < 32)
        {
            c4RealNeed = c4Needed + 32;
        }

        tCIDLib::TCh* pszNew = new tCIDLib::TCh[c4RealNeed + 1];
        TArrayJanitor<tCIDLib::TCh> janNew(pszNew);
        if (bKeepOld)
        {
            // Length +1 to get the null!
            TRawMem::CopyMemBuf(pszNew, m_pszValue, (m_c4Length + 1) * kCIDLib::c4CharBytes);
        }
        else
        {
            m_c4Length = 0;
            pszNew[0] = kCIDLib::chNull;
        }

        // Now we can dump the old one and swap the new one in
        m_c4BufSz = c4RealNeed;
        delete [] m_pszValue;
        m_pszValue = janNew.paOrphan();
    }
     else if (!bKeepOld)
    {
        // If not keeping the old, just clear what we already have
        m_c4Length = 0;
        m_pszValue[0] = kCIDLib::chNull;
    }
}


//
//  A reusable helper for the various places where we get a new buffer to set.
//  If it fits in what we have, we just copy it. Else we have to reallocate.
//  They can ask us to reserve more. And we have another for setting from a short
//  character buffer.
//
tCIDLib::TVoid
TKrnlString::SetNewBuffer(  const   tCIDLib::TCh* const pszValue
                            , const tCIDLib::TCard4     c4SrcLen
                            , const tCIDLib::TCard4     c4Extra)
{
    const tCIDLib::TCard4 c4FullNewLen = c4SrcLen + c4Extra;
    if (m_c4BufSz < c4FullNewLen)
    {
        delete [] m_pszValue;
        m_pszValue = nullptr;

        m_c4BufSz = c4FullNewLen;
        m_c4Length = c4SrcLen;
        m_pszValue = new tCIDLib::TCh[m_c4BufSz + 1];
    }

    // +1 to get the null terminator
    TRawMem::CopyMemBuf(m_pszValue, pszValue, (c4SrcLen + 1) * kCIDLib::c4CharBytes);
    m_c4Length = c4SrcLen;
}

tCIDLib::TBoolean
TKrnlString::bSetNewSBuffer(const   tCIDLib::TSCh* const    pszValue
                            , const tCIDLib::TCard4         c4Extra)
{
    // Calculate how many wide characters this creates
    tCIDLib::TCard4 c4SrcLen;
    if (!CIDStrOp_CalcWCSize(pszValue, c4SrcLen))
        return kCIDLib::False;

    // Then we can calculate the new full length
    const tCIDLib::TCard4 c4FullNewLen = c4SrcLen + c4Extra;
    if (m_c4BufSz < c4FullNewLen)
    {
        delete [] m_pszValue;
        m_pszValue = nullptr;
        m_c4Length = 0;

        m_c4BufSz = c4FullNewLen;
        m_pszValue = new tCIDLib::TCh[m_c4BufSz + 1];
    }

    return CIDStrOp_MBToWC(m_pszValue, m_c4BufSz, pszValue, m_c4Length);
}
