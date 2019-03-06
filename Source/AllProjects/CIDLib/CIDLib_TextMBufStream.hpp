//
// FILE NAME: CIDLib_TextMBufStream.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/01/1997
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDLib_TextMBufStream.Cpp file, which provides
//  simple derivatives of the TTextInStream and TTextOutStream classes. These
//  just handle the grunt work of creating the underlying binary stream and
//  its implementation object.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TTextMBufOutStream;


// ---------------------------------------------------------------------------
//   CLASS: TTextMBufInStream
//  PREFIX: strm
// ---------------------------------------------------------------------------
class CIDLIBEXP TTextMBufInStream : public TTextInStream
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TTextMBufInStream() = delete;

        TTextMBufInStream
        (
            const   tCIDLib::TCard4         c4InitSize
            , const tCIDLib::TCard4         c4MaxSize = kCIDLib::c4DefMaxBufferSz
            ,       TTextConverter* const   ptcvtToAdopt = nullptr
        );

        TTextMBufInStream
        (
            const   TMemBuf* const          pmbufToUse
            , const tCIDLib::TCard4         c4InitLogicalEnd = 0
            , const tCIDLib::EAdoptOpts     eAdopt = tCIDLib::EAdoptOpts::NoAdopt
            ,       TTextConverter* const   ptcvtToAdopt = nullptr
        );

        TTextMBufInStream
        (
            const   TTextMBufOutStream&     strmToSyncWith
        );

        TTextMBufInStream(const TTextMBufInStream&) = delete;

        ~TTextMBufInStream();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TTextMBufInStream& operator=(const TTextMBufInStream&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid SetEndIndex
        (
            const   tCIDLib::TCard4         c4NewIndex
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pstrmiBuf
        //      We don't own this. It is owned by the underlying text stream class,
        //      after being wrapped in a binary input stream. But, in order to support
        //      resetting the end of stream (which is very useful for efficiency), we
        //      need to keep this pointer around.
        // -------------------------------------------------------------------
        TMemInStreamImpl*   m_pstrmiBuf;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTextMBufInStream, TTextInStream)
};


// ---------------------------------------------------------------------------
//   CLASS: TTextMBufOutStream
//  PREFIX: strm
// ---------------------------------------------------------------------------
class CIDLIBEXP TTextMBufOutStream : public TTextOutStream
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TTextMBufOutStream() = delete;

        TTextMBufOutStream
        (
            const   tCIDLib::TCard4         c4InitSize
            , const tCIDLib::TCard4         c4MaxSize = kCIDLib::c4MaxCard
            ,       TTextConverter* const   ptcvtToAdopt = nullptr
        );

        TTextMBufOutStream
        (
                    TMemBuf* const          pmbufToUse
            , const tCIDLib::EAdoptOpts     eAdopt = tCIDLib::EAdoptOpts::NoAdopt
            ,       TTextConverter* const   ptcvtToAdopt = nullptr
        );

        TTextMBufOutStream(const TTextMBufOutStream&) = delete;

        ~TTextMBufOutStream();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TTextMBufOutStream& operator=(const TTextMBufOutStream&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4CurSize() const;

        const TMemBuf& mbufData() const;


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TTextMBufInStream;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pstrmiThis
        //      In order to support the creation of input streams that are
        //      synchronized with output streams, we save a copy of the stream
        //      impl object.
        // -------------------------------------------------------------------
        TMemOutStreamImpl*  m_pstrmiThis;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTextMBufOutStream, TTextOutStream)
};

#pragma CIDLIB_POPPACK


