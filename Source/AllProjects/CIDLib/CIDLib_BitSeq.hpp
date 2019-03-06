//
// FILE NAME: CIDLib_BitSeq.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/26/2001
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the file CIDLib_BitSeq.cpp file, which implements
//  the TBitSeq class. This class is used in those cases where you must build
//  up a sequence of bit patterns into a buffer, in a way that is not byte
//  oriented. This happens, for instance, in X-10 extended data messages, in
//  that arbitrarily sized chunks of bits must be appended, or parsed out of,
//  a buffer.
//
//  The buffer will expand as required, but if you know you are always going
//  to use a particular number of bytes, allocate that up front by telling
//  the constructor what to pre-allocate.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TBitSeq
//  PREFIX: bsq
// ---------------------------------------------------------------------------
class CIDLIBEXP TBitSeq : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TBitSeq
        (
            const   tCIDLib::TCard4         c4InitAlloc = 64UL
        );

        TBitSeq
        (
            const   tCIDLib::TCard1* const  pc1InitData
            , const tCIDLib::TCard4         c4Bytes
        );

        TBitSeq
        (
            const   TBitSeq&                bsqSrc
        );

        ~TBitSeq();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TBitSeq& operator=
        (
            const   TBitSeq&                bsqSrc
        );

        tCIDLib::TBoolean operator[]
        (
            const   tCIDLib::TCard4         c4BitIndex
        )   const;

        tCIDLib::TBoolean operator==
        (
            const   TBitSeq&                bsqSrc
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TBitSeq&                bsqSrc
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AppendBits
        (
            const   tCIDLib::TCard4         c4Val
            , const tCIDLib::TCard4         c4BitCount
            , const tCIDLib::EBitSeqs       eMode = tCIDLib::EBitSeqs::Normal
        );

        tCIDLib::TVoid AppendBits
        (
            const   TBitSeq&                bsqToAppend
            , const tCIDLib::TCard4         c4FromBit = 0
        );

        tCIDLib::TBoolean bAt
        (
            const   tCIDLib::TCard4         c4BitIndex
        )   const;

        tCIDLib::TBoolean bSeqIsEqual
        (
            const   TBitSeq&                bsqSrc
            , const tCIDLib::TCard4         c4ByteCount = kCIDLib::c4MaxCard
        );

        tCIDLib::TCard1 c1At
        (
            const   tCIDLib::TCard4         c4ByteIndex
        )   const;

        tCIDLib::TCard4 c4BytesWritten() const;

        tCIDLib::TCard4 c4ExtractBits
        (
            const   tCIDLib::TCard4         c4BitCount
            , const tCIDLib::EBitSeqs       eMode = tCIDLib::EBitSeqs::Normal
        );

        tCIDLib::TCard4 c4ReadOfs() const;

        tCIDLib::TCard4 c4WriteOfs() const;

        tCIDLib::TVoid CapAt
        (
            const   tCIDLib::TCard4         c4BitCount
        );

        const tCIDLib::TCard1* pc1Buffer() const;

        tCIDLib::TVoid ReadReset
        (
            const   tCIDLib::TCard4         c4AtBit = 0
        );

        tCIDLib::TVoid Reset();

        tCIDLib::TVoid SetAt
        (
            const   tCIDLib::TCard4         c4ByteIndex
            , const tCIDLib::TCard1         c1Value
        );

        tCIDLib::TVoid WriteReset();


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid DoAppend
        (
            const   tCIDLib::TCard4         c4Val
            , const tCIDLib::TCard4         c4BitCount
            , const tCIDLib::EBitSeqs       eMode
        );

        tCIDLib::TCard4 c4DoExtract
        (
            const   tCIDLib::TCard4         c4BitCount
            , const tCIDLib::EBitSeqs       eMode
        );

        tCIDLib::TVoid CheckRAvail
        (
            const   tCIDLib::TCard4         c4BitCount
            , const tCIDLib::EBitSeqs       eMode
        );

        tCIDLib::TVoid CheckWAvail
        (
            const   tCIDLib::TCard4         c4BitCount
            , const tCIDLib::EBitSeqs       eMode
        );

        tCIDLib::TVoid Expand();


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        //
        //  m_c4CurAlloc
        //      The current size of the buffer. When m_c4CurOfs hits this
        //      value, its time to reallocate.
        //
        //  m_c4CurRBit
        //  m_c4CurRByte
        //      The current bit and byte offset for reading. I.e. this is where
        //      we read bits out of the sequence. It cannot go beyond the
        //      current write offset.
        //
        //  m_c4CurWBit
        //  m_c4CurWByte
        //      The current bit and byte offset for writing. I.e. this is where
        //      we apppend new bits onto the sequence.
        //
        //  m_pc1Buffer
        //      The buffer we are building into.
        // -------------------------------------------------------------------
        tCIDLib::TCard4     m_c4CurAlloc;
        tCIDLib::TCard4     m_c4CurRBit;
        tCIDLib::TCard4     m_c4CurRByte;
        tCIDLib::TCard4     m_c4CurWBit;
        tCIDLib::TCard4     m_c4CurWByte;
        tCIDLib::TCard1*    m_pc1Buffer;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TBitSeq,TObject)
};

#pragma CIDLIB_POPPACK


