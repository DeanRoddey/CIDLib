//
// FILE NAME: CIDLib_BinMBufStream.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/01/1997
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
//  This is the header for the CIDLib_BinaryMBufStream.Cpp file. This
//  file provide very simple derivatives of TBinInStream and TBinOutStream. It
//  just handles the management of the stream implementation object, which in
//  this case manages a memory buffer object. This stuff can all be done
//  manually, but this class just makes it safer and easier.
//
//  Since these buffers can tend to be large, it uses a system memory buffer
//  which can be committed over time without having to allocate another buffer
//  and copying the old contents in when the buffer needs to be expanded.
//
// CAVEATS/GOTCHAS:
//
//  1)  In order to have an in and out stream by synchronized in terms of
//      using the same buffer and agreeing on where the logical end of stream
//      is, you can construct an input stream from an output stream.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TBinMBufOutStream;


// ---------------------------------------------------------------------------
//   CLASS: TBinMBufInStream
//  PREFIX: strm
// ---------------------------------------------------------------------------
class CIDLIBEXP TBinMBufInStream : public TBinInStream
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TBinMBufInStream() = delete;

        TBinMBufInStream
        (
            const   tCIDLib::TCard4         c4InitSize
            , const tCIDLib::TCard4         c4MaxSize = kCIDLib::c4DefMaxBufferSz
        );

        TBinMBufInStream
        (
            const   TMemBuf* const          pmbufToUse
            , const tCIDLib::TCard4         c4InitLogicalEnd = 0
            , const tCIDLib::EAdoptOpts     eAdopt = tCIDLib::EAdoptOpts::NoAdopt
        );

        TBinMBufInStream
        (
            const   tCIDLib::TCard1* const  pc1InitData
            , const tCIDLib::TCard4         c4Count
        );

        TBinMBufInStream
        (
            const   TBinMBufOutStream&      strmSyncTo
        );

        TBinMBufInStream(const TBinMBufInStream&) = delete;

        ~TBinMBufInStream();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TBinMBufInStream& operator=(const TBinMBufInStream&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4CurSize() const;

        const TMemBuf& mbufData() const;

        const TMemInStreamImpl& strmiThis() const;

        TMemInStreamImpl& strmiThis();

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
        TMemInStreamImpl*   m_pstrmiMem;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TBinMBufInStream,TBinInStream)
};



// ---------------------------------------------------------------------------
//   CLASS: TBinMBufOutStream
//  PREFIX: strm
// ---------------------------------------------------------------------------
class CIDLIBEXP TBinMBufOutStream : public TBinOutStream
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TBinMBufOutStream() = delete;

        TBinMBufOutStream
        (
            const   tCIDLib::TCard4         c4InitSize
            , const tCIDLib::TCard4         c4MaxSize = kCIDLib::c4DefMaxBufferSz
        );

        TBinMBufOutStream
        (
                    TMemBuf* const          pmbufToUse
            , const tCIDLib::EAdoptOpts     eAdopt = tCIDLib::EAdoptOpts::NoAdopt
        );

        TBinMBufOutStream(const TBinMBufOutStream&) = delete;

        ~TBinMBufOutStream();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TBinMBufOutStream& operator=(const TBinMBufOutStream&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4CurSize() const;

        const TMemBuf& mbufData() const;

        const TMemOutStreamImpl& strmiThis() const;

        TMemOutStreamImpl& strmiThis();

        tCIDLib::TVoid TruncateAt
        (
            const   tCIDLib::TCard4         c4At
        );


    protected :
        // -------------------------------------------------------------------
        //  Make the in-stream our friend
        // -------------------------------------------------------------------
        friend class TBinMBufInStream;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pstrmiMem
        //      We store a copy of our impl object, in addition to giving it
        //      to our praent class to own. This prevents lots of casting.
        // -------------------------------------------------------------------
        TMemOutStreamImpl*  m_pstrmiMem;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TBinOutMBufStream,TBinOutStream)
};

#pragma CIDLIB_POPPACK



