//
// FILE NAME: CIDLib_ExpByteBuf.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/04/2002
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
//  This file implements the TExpByteBuf class.
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
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TExpByteBuf,TObject)



// ---------------------------------------------------------------------------
//   CLASS: TExpByteBuf
//  PREFIX: expb
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TExpByteBuf: Constuctors and Destructor
// ---------------------------------------------------------------------------
TExpByteBuf::TExpByteBuf(const tCIDLib::TCard4 c4InitAlloc) :

    m_c4CurSize(c4InitAlloc)
    , m_c4CurOfs(0)
    , m_pc1Buffer(nullptr)
{
    // If the init size was zero, then set it to 8
    if (!m_c4CurSize)
        m_c4CurSize = 8;

    m_pc1Buffer = new tCIDLib::TCard1[m_c4CurSize];
}

TExpByteBuf::TExpByteBuf(const TExpByteBuf& expbSrc) :

    m_c4CurSize(0)
    , m_c4CurOfs(0)
    , m_pc1Buffer(nullptr)
{
    tCIDLib::TCard1* pszNew = new tCIDLib::TCard1[expbSrc.m_c4CurSize];
    TArrayJanitor<tCIDLib::TCard1> janBuffer(pszNew);
    TRawMem::CopyMemBuf(pszNew, expbSrc.m_pc1Buffer, expbSrc.m_c4CurSize);

    // It appears to have worked, so store the info away
    m_c4CurSize = expbSrc.m_c4CurSize;
    m_c4CurOfs = expbSrc.m_c4CurOfs;
    m_pc1Buffer = janBuffer.paOrphan();
}

TExpByteBuf::~TExpByteBuf()
{
    delete [] m_pc1Buffer;
    m_pc1Buffer = nullptr;
}


// ---------------------------------------------------------------------------
//  TExpByteBuf: Public operators
// ---------------------------------------------------------------------------
TExpByteBuf& TExpByteBuf::operator=(const TExpByteBuf& expbSrc)
{
    if (this != &expbSrc)
    {
        // Reallocate our buffer if not the same size
        if (m_c4CurSize != expbSrc.m_c4CurSize)
        {
            // Make sure we can allocate the new buffer before we change anything
            tCIDLib::TCard1* pc1New = new tCIDLib::TCard1[expbSrc.m_c4CurSize];

            TArrayJanitor<tCIDLib::TCard1> janOld(m_pc1Buffer);
            m_pc1Buffer = pc1New;
            m_c4CurSize = expbSrc.m_c4CurSize;
        }

        m_c4CurOfs = expbSrc.m_c4CurOfs;
        TRawMem::CopyMemBuf(m_pc1Buffer, expbSrc.m_pc1Buffer, m_c4CurSize);
    }
    return *this;
}

TExpByteBuf& TExpByteBuf::operator=(TExpByteBuf&& expbSrc)
{
    if (this != &expbSrc)
    {
        tCIDLib::Swap(m_c4CurOfs, expbSrc.m_c4CurOfs);
        tCIDLib::Swap(m_c4CurSize, expbSrc.m_c4CurSize);
        tCIDLib::Swap(m_pc1Buffer, expbSrc.m_pc1Buffer);
    }
    return *this;
}


tCIDLib::TCard1 TExpByteBuf::operator[](const tCIDLib::TCard4 c4At) const
{
    if (c4At >= m_c4CurOfs)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_IndexError
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
            , TCardinal(c4At)
            , clsThis()
            , TCardinal(m_c4CurOfs)
        );
    }
    return m_pc1Buffer[c4At];
}


// ---------------------------------------------------------------------------
//  TExpByteBuf: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TExpByteBuf::Append(const tCIDLib::TCard1 c1ToAppend)
{
    // Expand it if we have to
    if (m_c4CurOfs >= m_c4CurSize)
    {
        // Create a new size 1.5 times larger
        const tCIDLib::TCard4 c4NewSize = tCIDLib::TCard4(m_c4CurSize * 1.5);

        // Allocate a temp, making sure to add one for the null
        tCIDLib::TCard1* pc1Tmp = new tCIDLib::TCard1[c4NewSize + 1];

        // Now copy over the old content and delete the old buffer
        TRawMem::CopyMemBuf(pc1Tmp, m_pc1Buffer, m_c4CurSize);
        delete [] m_pc1Buffer;

        // Store the new pointer and size
        m_pc1Buffer = pc1Tmp;
        m_c4CurSize = c4NewSize;
    }

    // Store the char and bump the current offset
    m_pc1Buffer[m_c4CurOfs++] = c1ToAppend;
}

tCIDLib::TVoid TExpByteBuf::Append( const   tCIDLib::TCard1* const  pc1ToAppend
                                    , const tCIDLib::TCard4         c4Count)
{
    // See if we can hold the new data. If not, then expand
    if (m_c4CurOfs + c4Count >= m_c4CurSize)
    {
        //
        //  Calc the new size. Figure out what we need to hold the new data,
        //  then up by 1.5.
        //
        const tCIDLib::TCard4 c4NewSize = tCIDLib::TCard4
        (
            (m_c4CurOfs + c4Count) * 1.5
        );

        // Allocate a temp, making sure to add one for the null
        tCIDLib::TCard1* pc1Tmp = new tCIDLib::TCard1[c4NewSize + 1];

        // Now copy over the old content and delete the old buffer
        TRawMem::CopyMemBuf(pc1Tmp, m_pc1Buffer, m_c4CurSize);
        delete [] m_pc1Buffer;

        // Store the new pointer and size
        m_pc1Buffer = pc1Tmp;
        m_c4CurSize = c4NewSize;
    }

    // Now copy in the new bytes and bump the offset
    TRawMem::CopyMemBuf(&m_pc1Buffer[m_c4CurOfs], pc1ToAppend, c4Count);
    m_c4CurOfs += c4Count;
}


tCIDLib::TCard1 TExpByteBuf::c1First() const
{
    if (!m_c4CurOfs)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcExpb_NoData
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
        );
    }
    return m_pc1Buffer[0];
}


tCIDLib::TCard1 TExpByteBuf::c1Last() const
{
    if (!m_c4CurOfs)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcExpb_NoData
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
        );
    }
    return m_pc1Buffer[m_c4CurOfs - 1];
}


tCIDLib::TCard4 TExpByteBuf::c4CheckSum() const
{
    tCIDLib::TCard4 c4Sum = 0;
    const tCIDLib::TCard1* pc1Buf = m_pc1Buffer;
    for (tCIDLib::TCard4 c4Tmp = 0; c4Tmp < m_c4CurOfs; c4Tmp++)
    {
        c4Sum += *pc1Buf;
        pc1Buf++;
    }
    return c4Sum;
}


tCIDLib::TCard4 TExpByteBuf::c4Bytes() const
{
    return m_c4CurOfs;
}


const tCIDLib::TCard1* TExpByteBuf::pc1Buffer() const
{
    return m_pc1Buffer;
}


tCIDLib::TCard1* TExpByteBuf::pszReplicateBuffer()
{
    // Allocate a return buffer and copy the current contents to it
    tCIDLib::TCard1* pc1Ret = new tCIDLib::TCard1[m_c4CurOfs];
    TRawMem::CopyMemBuf(pc1Ret, m_pc1Buffer, m_c4CurOfs);
    return pc1Ret;
}


tCIDLib::TVoid TExpByteBuf::Reset()
{
    m_c4CurOfs = 0;
}

