//
// FILE NAME: CIDLib_ChunkedMemStream.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/12/2014
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
//  This is the header for the CIDLib_ChunkedMemStreamImpl.Cpp file. This file
//  implements specialized stream in/output streams.
//
//  The problem is that we often don't know how much memory will be needed to
//  stream in our out an object, or a set of objects. This puts us in the situation
//  where we have to use a memory buffer implementation object, which has to either
//  continually expand a heap buffer (or allocate some worst case buffer size in
//  a system buffer and eat up lots of virtual memory space.)
//
//  THis one allows the data to be chunked, so it just allocates new chunks of
//  a given size as required and adds them to the list. At the end, it can be
//  copied out to a single buffer once the actual size requirements are known if
//  you need to see it as a single contiguous memory buffer. Or the input stream
//  can be used to stream back in from the chunked data for maximum efficiency.
//
//  Since this operates within a stream, the fact that it's chunked doesn't
//  matter.
//
//  We create a simple info class that manages the buffers and current offsets
//  within it, and which is used to share the data between linked input and output
//  streams. Then the in and out stream implementation objects. Then the in and out
//  text and binary streams.
//
//  Only the actual stream classes need to be public. The implementation classes are
//  internal.
//
//
// CAVEATS/GOTCHAS:
//
//  1)  TruncateAt() cannot truncate past the current end of data, or an exception
//      will be thrown. The current output position is set back to zero when a
//      truncation is done.
//
//  2)  SetEndIndex() cannot set the index beyond the current end of data, since
//      that makes no sense. No data can be written to our buffers behind our back,
//      so the only way the data can change is to be written in by a linked output
//      stream, which will update the end of data position accordingly.
//
//  3)  SkipForwardBy cannot move the read position past the end of cotent, so it
//      will throw if you try.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// These are internal, we only need to store a pointer publically
class TChunkedInStreamImpl;
class TChunkedOutStreamImpl;

// And we have to forward ref these so we can support linking
class TChunkedBinOutStream;
class TChunkedTextOutStream;


// ---------------------------------------------------------------------------
//   CLASS: TChunkedBinInStream
//  PREFIX: strm
// ---------------------------------------------------------------------------
class CIDLIBEXP TChunkedBinInStream : public TBinInStream
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TChunkedBinInStream() = delete;

        TChunkedBinInStream
        (
            const   TChunkedBinOutStream&   strmSyncTo
        );

        TChunkedBinInStream(const TChunkedBinInStream&) = delete;

        ~TChunkedBinInStream();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TChunkedBinInStream& operator=(const TChunkedBinInStream&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4CurSize() const;

        const TChunkedInStreamImpl& strmiThis() const;

        tCIDLib::TVoid SetEndIndex
        (
            const   tCIDLib::TCard4         c4NewIndex
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pstrmiMem
        //      We store a copy of our impl object, in addition to giving it
        //      to our praent class to own. This prevents lots of casting.
        // -------------------------------------------------------------------
        TChunkedInStreamImpl*   m_pstrmiIn;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TChunkedBinInStream,TBinInStream)
};



// ---------------------------------------------------------------------------
//   CLASS: TChunkedBinOutStream
//  PREFIX: strm
// ---------------------------------------------------------------------------
class CIDLIBEXP TChunkedBinOutStream : public TBinOutStream
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TChunkedBinOutStream() = delete;

        TChunkedBinOutStream
        (
            const   tCIDLib::TCard4         c4MaxSize
        );

        TChunkedBinOutStream(const TChunkedBinOutStream&) = delete;

        ~TChunkedBinOutStream();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TChunkedBinOutStream& operator=(const TChunkedBinOutStream&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4CurSize() const;

        tCIDLib::TCard4 c4MaxSize() const;

        tCIDLib::TCard4 c4CopyOutTo
        (
                    TMemBuf&                mbufTar
            , const tCIDLib::TCard4         c4StartAt
        )   const;

        tCIDLib::TCard4 c4CopyOutToStream
        (
                    TBinOutStream&          strmTar
            , const tCIDLib::TCard4         c4Count = kCIDLib::c4MaxCard
        )   const;

        const TChunkedOutStreamImpl& strmiThis() const;

        tCIDLib::TVoid TruncateAt
        (
            const   tCIDLib::TCard4         c4At
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pstrmiMem
        //      We store a copy of our impl object, in addition to giving it
        //      to our parent class to own. This prevents lots of casting.
        // -------------------------------------------------------------------
        TChunkedOutStreamImpl*  m_pstrmiOut;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TChunkedBinOutStream,TBinOutStream)
};



// ---------------------------------------------------------------------------
//   CLASS: TChunkedTextInStream
//  PREFIX: strm
// ---------------------------------------------------------------------------
class CIDLIBEXP TChunkedTextInStream : public TTextInStream
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TChunkedTextInStream() = delete;

        TChunkedTextInStream
        (
            const   TChunkedTextOutStream&  strmToSyncWith
            ,       TTextConverter* const   ptcvtToAdopt = nullptr
        );

        TChunkedTextInStream(const TChunkedTextInStream&) = delete;

        ~TChunkedTextInStream();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TChunkedTextInStream& operator=(const TChunkedTextInStream&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4CurSize() const;

        const TChunkedBinInStream& strmIn() const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pstrmMem
        //      We store a typed copy of the binary input stream that we use
        //      to read the data. This way we can avoid casts any time we need
        //      to access it.
        // -------------------------------------------------------------------
        TChunkedBinInStream*    m_pstrmIn;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TChunkedTextInStream, TTextInStream)
};


// ---------------------------------------------------------------------------
//   CLASS: TChunkedTextOutStream
//  PREFIX: strm
// ---------------------------------------------------------------------------
class CIDLIBEXP TChunkedTextOutStream : public TTextOutStream
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TChunkedTextOutStream() = delete;

        TChunkedTextOutStream
        (
            const   tCIDLib::TCard4         c4MaxSize
            ,       TTextConverter* const   ptcvtToAdopt = nullptr
        );

        TChunkedTextOutStream(const TChunkedTextOutStream&) = delete;

        ~TChunkedTextOutStream();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TChunkedTextOutStream& operator=(const TChunkedTextOutStream&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4CurSize() const;

        tCIDLib::TCard4 c4MaxSize() const;

        tCIDLib::TCard4 c4CopyOutTo
        (
                    TMemBuf&                mbufTar
            , const tCIDLib::TCard4         c4StartAt
        )   const;

        tCIDLib::TCard4 c4CopyOutToStream
        (
                    TBinOutStream&          strmTar
            , const tCIDLib::TCard4         c4Count = kCIDLib::c4MaxCard
        )   const;

        const TChunkedBinOutStream& strmOut() const;


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TChunkedTextInStream;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pstrmMem
        //      We store a typed copy of the binary output stream that we use
        //      to store the data. This way we can avoid casts any time we need
        //      to access it.
        // -------------------------------------------------------------------
        TChunkedBinOutStream*   m_pstrmOut;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TBinOutMBufStream,TTextOutStream)
};


#pragma CIDLIB_POPPACK


