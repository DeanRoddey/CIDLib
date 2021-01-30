//
// FILE NAME: CIDLib_Bitset.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/25/1999
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
//  This file implements the TBitset class, a class for managing sets
//  of 1 bit fields.
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
RTTIDecls(TBitsetBase,TObject)
RTTIDecls(TBitset,TBitsetBase)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDLib_BitSet
{
    namespace
    {
        //
        //  Format version for the base class
        //
        //      Version 2 - Get rid of the big endian flag which we never used
        //
        constexpr tCIDLib::TCard2   c2FmtVersion = 2;
    }
}



// ---------------------------------------------------------------------------
//   CLASS: TBitsetBase
//  PREFIX: bts
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TBitsetBase: Destructor
// ---------------------------------------------------------------------------
TBitsetBase::~TBitsetBase()
{
    try
    {
        // Clean up the bit array if it got allocated
        delete [] m_pc1Bits;
    }

    catch(...)
    {
        // <TBD> log something
    }
}


// ---------------------------------------------------------------------------
//  TBitsetBase: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TBitsetBase::ANDRawArray(const   tCIDLib::TCard1* const  pc1Src
                                        , const tCIDLib::TCard4         c4ArraySize)
{
    //
    //  Make sure its been set up, which also guarantees we don't have to deal with
    //  a zero sized set below.
    //
    CheckReady();

    // Make sure that they are the same byte size
    if (m_c4Bytes != c4ArraySize)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcBitset_DifferentSizes
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    //  The are the same so AND in all the source bits.
    for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4Bytes; c4Index++)
        m_pc1Bits[c4Index] &= pc1Src[c4Index];

    // Mask off the final byte to insure no stray bits
    if (m_c1TrailingMask)
        m_pc1Bits[m_c4Bytes - 1] &= m_c1TrailingMask;
}


tCIDLib::TBoolean TBitsetBase::bAllCleared() const
{
    // Make sure its even been set up yet
    CheckReady();

    // Just loop through the bytes and check each byte for zero
    tCIDLib::TCard1* pc1End = m_pc1Bits + m_c4Bytes;
    tCIDLib::TCard1* pc1Cur = m_pc1Bits;
    while (pc1Cur < pc1End)
    {
        if (*pc1Cur)
            return kCIDLib::False;
        pc1Cur++;
    }
    return kCIDLib::True;
}


tCIDLib::TCard4 TBitsetBase::c4BitCount() const
{
    return m_c4Bits;
}


tCIDLib::TVoid TBitsetBase::Clear()
{
    // Make sure its even been set up yet
    CheckReady();

    // Zero out all of the bytes of the data array
    TRawMem::SetMemBuf(m_pc1Bits, kCIDLib::c1MinCard, m_c4Bytes);
}


tCIDLib::TVoid
TBitsetBase::FromRawArray(  const   tCIDLib::TCard1* const  pc1Src
                            , const tCIDLib::TCard4         c4ArraySize) const
{
    // Make sure its even been set up yet
    CheckReady();

    // Make sure that the source array is the right size
    if (c4ArraySize != m_c4Bytes)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcBitset_SrcBufSize
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(c4ArraySize)
            , TCardinal(m_c4Bytes)
        );
    }

    // Copy over the data
    TRawMem::CopyMemBuf(m_pc1Bits, pc1Src, c4ArraySize);

    // Make sure no trailing bits
    if (m_c1TrailingMask)
        m_pc1Bits[m_c4Bytes - 1] &= m_c1TrailingMask;
}


tCIDLib::TVoid TBitsetBase::ORRawArray( const   tCIDLib::TCard1* const  pc1Src
                                        , const tCIDLib::TCard4         c4ArraySize)
{
    // Make sure its even been set up yet
    CheckReady();

    // Make sure that they are the same size
    if (m_c4Bytes != c4ArraySize)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcBitset_DifferentSizes
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // The are the same so OR in all the source bits
    for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4Bytes; c4Index++)
        m_pc1Bits[c4Index] |= pc1Src[c4Index];

    // Make sure no trailing bits
    if (m_c1TrailingMask)
        m_pc1Bits[m_c4Bytes - 1] &= m_c1TrailingMask;
}


tCIDLib::TVoid TBitsetBase::SetAllBits()
{
    // Make sure its even been set up yet
    CheckReady();

    // Set all of the bytes of the data array
    TRawMem::SetMemBuf(m_pc1Bits, tCIDLib::TCard1(0xFF), m_c4Bytes);

    // But make sure no trailing bits are set
    if (m_c1TrailingMask)
        m_pc1Bits[m_c4Bytes - 1] &= m_c1TrailingMask;
}


tCIDLib::TVoid
TBitsetBase::ToRawArray(        tCIDLib::TCard1* const  pc1ToFill
                        , const tCIDLib::TCard4         c4ArraySize) const
{
    CheckReady();

    if (c4ArraySize < m_c4Bytes)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcBitset_TargetBufSize
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::OutResource
            , TCardinal(c4ArraySize)
            , TCardinal(m_c4Bytes)
        );
    }
    TRawMem::CopyMemBuf(pc1ToFill, m_pc1Bits, m_c4Bytes);
}



// ---------------------------------------------------------------------------
//  TBitsetBase: Hidden Constructors
// ---------------------------------------------------------------------------
TBitsetBase::TBitsetBase() :

    m_c1TrailingMask(0)
    , m_c4Bits(0)
    , m_c4Bytes(0)
    , m_pc1Bits(nullptr)
{
}

TBitsetBase::TBitsetBase(const tCIDLib::TCard4 c4SetSize) :

    m_c1TrailingMask(0)
    , m_c4Bits(c4SetSize)
    , m_c4Bytes(0)
    , m_pc1Bits(nullptr)
{
    if (!c4SetSize)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcBitset_ZeroSize
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // Calculate the bytes required
    m_c4Bytes = m_c4Bits / 8;
    if (m_c4Bits % 8)
        m_c4Bytes++;

    // Calculate our traiing byte mask
    CalcTrailingMask();

    // And allocate and zero the storage array
    m_pc1Bits = new tCIDLib::TCard1[m_c4Bytes];
    TRawMem::SetMemBuf(m_pc1Bits, kCIDLib::c1MinCard, m_c4Bytes);
}

TBitsetBase::TBitsetBase(const  tCIDLib::TCard4         c4SetSize
                        , const tCIDLib::TCard1* const  pc1InitVals) :

    m_c1TrailingMask(0)
    , m_c4Bits(c4SetSize)
    , m_c4Bytes(0)
    , m_pc1Bits(nullptr)
{
    if (!c4SetSize)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcBitset_ZeroSize
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // Calculate the bytes required
    m_c4Bytes = m_c4Bits / 8;
    if (m_c4Bits % 8)
        m_c4Bytes++;

    // Allocate the storage we need
    m_pc1Bits = new tCIDLib::TCard1[m_c4Bytes];

    // And copy over the initial data
    TRawMem::CopyMemBuf(m_pc1Bits, pc1InitVals, m_c4Bytes);

    // Calculate the trailing byte mask and apply it
    CalcTrailingMask();
    if (m_c1TrailingMask)
        m_pc1Bits[m_c4Bytes - 1] &= m_c1TrailingMask;
}


TBitsetBase::TBitsetBase(const TBitsetBase& btsSrc) :

    m_c1TrailingMask(btsSrc.m_c1TrailingMask)
    , m_c4Bits(btsSrc.m_c4Bits)
    , m_c4Bytes(btsSrc.m_c4Bytes)
    , m_pc1Bits(nullptr)
{
    // And allocate and copy over the source, if any. Might not be set up yet
    if (m_c4Bits)
    {
        m_pc1Bits = new tCIDLib::TCard1[m_c4Bytes];
        TRawMem::CopyMemBuf(m_pc1Bits, btsSrc.m_pc1Bits, m_c4Bytes);

        // Shouldn't be necessary but just in case mask the final byte
        if (m_c1TrailingMask)
            m_pc1Bits[m_c4Bytes - 1] &= m_c1TrailingMask;
    }
}

TBitsetBase::TBitsetBase(TBitsetBase&& btsSrc) :

    TBitsetBase()
{
    *this = tCIDLib::ForceMove(btsSrc);
}

TBitsetBase& TBitsetBase::operator=(const TBitsetBase& btsSrc)
{
    if (this != &btsSrc)
    {
        m_c1TrailingMask = btsSrc.m_c1TrailingMask;

        // If not the same size, then allocate a new buffer
        if (m_c4Bits != btsSrc.m_c4Bits)
        {
            TArrayJanitor<tCIDLib::TCard1> janOld(m_pc1Bits);

            m_c4Bits = btsSrc.m_c4Bits;
            m_c4Bytes = btsSrc.m_c4Bytes;
            m_pc1Bits = new tCIDLib::TCard1[m_c4Bytes];
        }

        // Either way, copy over the source bytes
        TRawMem::CopyMemBuf(m_pc1Bits, btsSrc.m_pc1Bits, m_c4Bytes);
    }
    return *this;
}

TBitsetBase& TBitsetBase::operator=(TBitsetBase&& btsSrc)
{
    if (this != &btsSrc)
    {
        tCIDLib::Swap(m_c1TrailingMask, btsSrc.m_c1TrailingMask);
        tCIDLib::Swap(m_c4Bits, btsSrc.m_c4Bits);
        tCIDLib::Swap(m_c4Bytes, btsSrc.m_c4Bytes);
        tCIDLib::Swap(m_pc1Bits, btsSrc.m_pc1Bits);
    }
    return *this;
}



// ---------------------------------------------------------------------------
//  TBitsetBase: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TBitsetBase::FormatTo(TTextOutStream& strmDest) const
{
    // Make sure its even been set up yet
    CheckReady();

    // Stream out the bits from the 0th one to the end.
    for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4Bits; c4Index++)
    {
        // Calculate the byte/bit masks
        const tCIDLib::TCard1 c1Mask = tCIDLib::TCard1(0x1 << (c4Index % 8));
        const tCIDLib::TCard4 c4ByteIndex = c4Index >> 3;

        if (m_pc1Bits[c4ByteIndex] & c1Mask)
            strmDest << L"+";
        else
            strmDest << L"o";
    }
}


tCIDLib::TVoid TBitsetBase::StreamFrom(TBinInStream& strmToReadFrom)
{
    // We should get a start object marker
    strmToReadFrom.CheckForStartMarker(CID_FILE, CID_LINE);

    // Check the format version
    tCIDLib::TCard2 c2FmtVersion;
    strmToReadFrom  >> c2FmtVersion;
    if (!c2FmtVersion || (c2FmtVersion > CIDLib_BitSet::c2FmtVersion))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_UnknownFmtVersion
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(c2FmtVersion)
            , clsThis()
        );
    }

    //
    //  Stream in the new bit size. We never used the big ending thing, and just eat
    //  it now.
    //
    tCIDLib::TCard4 c4NewSize;
    strmToReadFrom  >> c4NewSize;

    // If version 1, read in and discard the old big endian flag
    if (c2FmtVersion == 1)
    {
        tCIDLib::TBoolean bDiscard;
        strmToReadFrom >> bDiscard;
    }

    // If the sizes are different, get rid of the current array
    if (m_c4Bits != c4NewSize)
    {
        delete [] m_pc1Bits;
        m_pc1Bits = nullptr;
    }

    // Store the new size and calc the bytes required
    m_c4Bits = c4NewSize;
    m_c4Bytes = m_c4Bits / 8;
    if (m_c4Bits % 8)
        m_c4Bytes++;

    // If no array, allocate one big enough
    if (!m_pc1Bits)
        m_pc1Bits = new tCIDLib::TCard1[m_c4Bytes];

    // And read in the data values to it
    strmToReadFrom.ReadArray(m_pc1Bits, m_c4Bytes);

    //
    //  Recalc the trailing byte mask and just to be safe, apply it. Should not
    //  have to check for having any bits, but do it just in case.
    //
    CalcTrailingMask();
    if (m_c1TrailingMask)
        m_pc1Bits[m_c4Bytes - 1] &= m_c1TrailingMask;

    // We should get an end object marker
    strmToReadFrom.CheckForEndMarker(CID_FILE, CID_LINE);
}


tCIDLib::TVoid TBitsetBase::StreamTo(TBinOutStream& strmToWriteTo) const
{
    // Make sure its even been set up yet
    CheckReady();

    //
    //  Start with a marker and a format version so that we can auto update
    //  this data later. Then stream out the bit count, then the bit values.
    //
    strmToWriteTo   << tCIDLib::EStreamMarkers::StartObject
                    << CIDLib_BitSet::c2FmtVersion
                    << m_c4Bits;

    strmToWriteTo.WriteArray(m_pc1Bits, m_c4Bytes);

    // And end up with and end object marker
    strmToWriteTo << tCIDLib::EStreamMarkers::EndObject;
}


// ---------------------------------------------------------------------------
//  TBitsetBase: Protected, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TBitsetBase::ANDWith(const TBitsetBase& btsSrc)
{
    // Make sure its been set up
    CheckReady();

    // Make sure that they are the same size
    if (m_c4Bits != btsSrc.m_c4Bits)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcBitset_DifferentSizes
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // The are the same so AND in all the source bits
    for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4Bytes; c4Index++)
        m_pc1Bits[c4Index] &= btsSrc.m_pc1Bits[c4Index];

    // Just to be safe
    if (m_c1TrailingMask)
        m_pc1Bits[m_c4Bytes - 1] &= m_c1TrailingMask;
}


//
//  We are depending here on the fact that both of us have been correctly applying
//  the trailing byte mask and so we won't fail due to any extraneous bits.
//
tCIDLib::TBoolean TBitsetBase::bCompareTo(const TBitsetBase& btsSrc) const
{
    if (&btsSrc == this)
        return kCIDLib::True;

    // If the bit sizes are different, definitely not equal
    if (m_c4Bits != btsSrc.m_c4Bits)
        return kCIDLib::False;

    // Compare the bits
    for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4Bytes; c4Index++)
    {
        if (m_pc1Bits[c4Index] != btsSrc.m_pc1Bits[c4Index])
            return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean TBitsetBase::bSetABit(const   tCIDLib::TCard4     c4BitIndex
                                        , const tCIDLib::TBoolean   bNewState) const
{
    // Do the standard checks
    CheckIndex(c4BitIndex);

    //
    //  Calculate the mask for the target byte and the byte index. The
    //  mask is different based on endianness
    //
    const tCIDLib::TCard1 c1Mask = tCIDLib::TCard1(0x1 << (c4BitIndex % 8));
    const tCIDLib::TCard4 c4ByteIndex = c4BitIndex >> 3;

    //
    //  Get the right byte and weave the mask in. We turn it off first,
    //  regardless of the new state. Then we just turn it back on if the new
    //  value is on. We also need to remember if the state changed for return.
    //
    tCIDLib::TCard1* pc1Byte = &m_pc1Bits[c4ByteIndex];
    const tCIDLib::TBoolean bPrevState = (*pc1Byte & c1Mask) != 0;
    *pc1Byte &= ~c1Mask;
    if (bNewState)
        *pc1Byte |= c1Mask;

    return (bPrevState != bNewState);
}


tCIDLib::TBoolean TBitsetBase::bTestABit(const tCIDLib::TCard4 c4BitIndex) const
{
    // Do the standard checks
    CheckIndex(c4BitIndex);

    // Get the right byte and see if the mask is non-zero
    const tCIDLib::TCard1 c1Mask = tCIDLib::TCard1(0x1 << (c4BitIndex % 8));
    return (m_pc1Bits[c4BitIndex >> 3] & c1Mask) != 0;
}


// Validate the passed value against our bit set size
tCIDLib::TVoid TBitsetBase::CheckIndex(const tCIDLib::TCard4 c4ToCheck) const
{
    // Make sure its even been set up yet
    CheckReady();

    // We have some bits, so do the index check
    if (c4ToCheck >= m_c4Bits)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_IndexError
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4ToCheck)
            , clsThis()
            , TCardinal(m_c4Bits)
        );
    }
}


// Throws if the bit array is not set, which can happen if the set is zero sized
tCIDLib::TVoid TBitsetBase::CheckReady() const
{
    if (!m_pc1Bits)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcBitset_NotReady
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
        );
    }
}


tCIDLib::TVoid TBitsetBase::CopyFrom(const TBitsetBase& btsSrc)
{
    // Don't bother if this is us
    if (this == &btsSrc)
        return;

    // If the sizes are different, get rid of the current array
    if (m_c4Bits != btsSrc.m_c4Bits)
    {
        delete [] m_pc1Bits;
        m_pc1Bits = nullptr;
    }

    // Store the bits and bytes members
    m_c4Bits    = btsSrc.m_c4Bits;
    m_c4Bytes   = btsSrc.m_c4Bytes;

    // If no array, allocate one big enough, if any source bits
    if (!m_pc1Bits && (m_c4Bits != 0))
        m_pc1Bits = new tCIDLib::TCard1[m_c4Bytes];

    // Copy over the source, if any. Shouldn't have to check but to be sure
    if (m_c4Bits)
        TRawMem::CopyMemBuf(m_pc1Bits, btsSrc.m_pc1Bits, m_c4Bytes);

    // To be safe re-calc the trailing mask and apply it
    CalcTrailingMask();
    if (m_c1TrailingMask)
        m_pc1Bits[m_c4Bytes - 1] &= m_c1TrailingMask;
}


//
//  We are depending here on the fact that both of us have been correctly applying
//  the trailing byte mask and so we won't fail due to any extraneous bits.
//
tCIDLib::TVoid TBitsetBase::ORWith(const TBitsetBase& btsSrc)
{
    // Make sure its been set up
    CheckReady();

    // Make sure that they are the same size
    if (m_c4Bits != btsSrc.m_c4Bits)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcBitset_DifferentSizes
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // The are the same so AND in all the source bits
    for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4Bytes; c4Index++)
        m_pc1Bits[c4Index] |= btsSrc.m_pc1Bits[c4Index];

    // Shouldn't need it, but do it anyway
    if (m_c1TrailingMask)
        m_pc1Bits[m_c4Bytes - 1] &= m_c1TrailingMask;
}


tCIDLib::TVoid TBitsetBase::ResetBitCount(const tCIDLib::TCard4 c4NewSize)
{
    // Can't be zero sized
    if (!c4NewSize)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcBitset_ZeroSize
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // If the sizes are different, get rid of the current array
    if (m_c4Bits != c4NewSize)
    {
        delete [] m_pc1Bits;
        m_pc1Bits = nullptr;
    }

    // Store the new size and calc the bytes required
    m_c4Bits = c4NewSize;
    m_c4Bytes = m_c4Bits / 8;
    if (m_c4Bits % 8)
        m_c4Bytes++;

    // If no array, allocate one big enough
    if (!m_pc1Bits)
        m_pc1Bits = new tCIDLib::TCard1[m_c4Bytes];

    // Zero out the bit buffer
    TRawMem::SetMemBuf(m_pc1Bits, kCIDLib::c1MinCard, m_c4Bytes);

    // Recalc the trailing byte mask
    CalcTrailingMask();
}


// ---------------------------------------------------------------------------
//  TBitsetBase: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Calculates a bitmask that we can use ensure that our last byte never gets
//  bits into it beyond our bit set mask.
//
tCIDLib::TVoid TBitsetBase::CalcTrailingMask()
{
    // Take the bits mod 8
    const tCIDLib::TCard4 c4TrailBits = m_c4Bits % 8;

    if (c4TrailBits)
    {
        //
        //  Set a bit for each of these trailing bits. We shift a 1 up by that many
        //  bits then subtract one which will set all of the bits covered by these
        //  trailing bits, leaving the reset zero.
        //
        //  Do this in a larger value so that it doesn't truncate before we sub
        //
        tCIDLib::TCard4 c4Mask = 0x1UL;
        c4Mask <<= c4TrailBits;
        c4Mask -= 1;

        // And take the low byte
        m_c1TrailingMask = tCIDLib::TCard1(c4Mask);
    }
     else
    {
        // There aren't any so just zero it
        m_c1TrailingMask = 0;
    }
}





// ---------------------------------------------------------------------------
//   CLASS: TBitset
//  PREFIX: bts
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TBitset: Constructors and Destructor
// ---------------------------------------------------------------------------
TBitset::TBitset()
{
}

TBitset::TBitset(const tCIDLib::TCard4 c4BitCount) :

    TBitsetBase(c4BitCount)
{
}

TBitset::TBitset(const  tCIDLib::TCard4         c4BitCount
                , const tCIDLib::TCard1* const  pc1InitVals) :

    TBitsetBase(c4BitCount, pc1InitVals)
{
}

TBitset::TBitset(const TBitset& btsSrc) :

    TBitsetBase(btsSrc)
{
}

TBitset::TBitset(TBitset&& btsSrc) :

    TBitset()
{
    *this = tCIDLib::ForceMove(btsSrc);
}

TBitset::~TBitset()
{
}


// ---------------------------------------------------------------------------
//  TBitset: Public operators
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TBitset::operator[](const tCIDLib::TCard4 c4Index) const
{
    return bTestABit(c4Index);
}


TBitset& TBitset::operator=(const TBitset& btsSrc)
{
    if (this != &btsSrc)
        TParent::operator=(btsSrc);
    return *this;
}

TBitset& TBitset::operator=(TBitset&& btsSrc)
{
    if (this != &btsSrc)
        TParent::operator=(tCIDLib::ForceMove(btsSrc));
    return *this;
}

tCIDLib::TVoid TBitset::operator&=(const TBitset& btsSrc)
{
    if (this != &btsSrc)
        ANDWith(btsSrc);
}


tCIDLib::TVoid TBitset::operator|=(const TBitset& btsSrc)
{
    if (this != &btsSrc)
        ORWith(btsSrc);
}


tCIDLib::TBoolean TBitset::operator==(const TBitset& btsSrc) const
{
    if (&btsSrc == this)
        return kCIDLib::True;

    return bCompareTo(btsSrc);
}

tCIDLib::TBoolean TBitset::operator!=(const TBitset& btsSrc) const
{
    return !operator==(btsSrc);
}


// ---------------------------------------------------------------------------
//  TBitset: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Gets the value of the indicated bit
tCIDLib::TBoolean TBitset::bBitState(const tCIDLib::TCard4 c4BitIndex) const
{
    return bTestABit(c4BitIndex);
}


tCIDLib::TBoolean TBitset::bSetBitState(const   tCIDLib::TCard4     c4BitIndex
                                        , const tCIDLib::TBoolean   bNewState) const
{
    return bSetABit(c4BitIndex, bNewState);
}


tCIDLib::TVoid TBitset::ChangeBitCount(const tCIDLib::TCard4 c4NewBitCount)
{
    ResetBitCount(c4NewBitCount);
}

