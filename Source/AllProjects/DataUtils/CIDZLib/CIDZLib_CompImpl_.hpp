//
// FILE NAME: CIDZLib_CompImpl_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/20/2004
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header file for the CIDZLib_CompImpl.Cpp file. This file
//  implements the actual zlib compression and decompression algorithms, and
//  keeps all the associated state for that.
//
//  This is not exposed, instead we expose a private class that just works
//  in terms of this class, so that we can hide the raft of constants and
//  types from downstream code.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)


// ---------------------------------------------------------------------------
//  CLASS: TZLibCompImpl
// PREFIX: zimpl
// ---------------------------------------------------------------------------
class TZLibCompImpl : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TZLibCompImpl
        (
            const   tCIDZLib::ECompLevels   eLevel
            , const tCIDZLib_::EStrategies  eStrategy
        );

        TZLibCompImpl(const TZLibCompImpl&) = delete;

        ~TZLibCompImpl();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TZLibCompImpl& operator==(const TZLibCompImpl&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4Compress
        (
                    TBinInStream&           strmInput
            ,       TBinOutStream&          strmOutput
            , const tCIDLib::TCard4         c4MaxInput = kCIDLib::c4MaxCard
        );

        tCIDLib::TCard4 c4Decompress
        (
                    TBinInStream&           strmInput
            ,       TBinOutStream&          strmOutput
            , const tCIDLib::TCard4         c4MaxInput = kCIDLib::c4MaxCard
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bFillWindow();

        tCIDLib::TCard4 c4BuildBLTree
        (
                    tCIDLib::TCard4&        c4OptLen
            ,       tCIDLib::TCard4&        c4StaticLen
        );

        tCIDLib::TCard2 c2GetInflBits
        (
            const   tCIDLib::TCard4         c4ToGet
        );

        tCIDLib::TCard4 c4GetInflBits
        (
            const   tCIDLib::TCard4         c4ToGet
        );

        tCIDLib::TCard4 c4PeekInflBits
        (
            const   tCIDLib::TCard4         c4ToGet
        );

        tCIDLib::TCard4 c4PeekInflBitsNC
        (
            const   tCIDLib::TCard4         c4ToGet
        );

        tCIDLib::TVoid DropInflBits
        (
            const   tCIDLib::TCard4         c4ToGet
        );

        tCIDLib::TCard4 c4LongestMatch
        (
            const   tCIDLib::TCard2         c2Start
            , const tCIDLib::TCard4         c4PrevMatch
            , const tCIDLib::TCard4         c4PrevLen
            ,       tCIDLib::TCard4&        c4MatchOfs
        );

        tCIDLib::TCard4 c4ReadBuf
        (
                    tCIDLib::TCard1* const  pc1ToFill
            , const tCIDLib::TCard4         c4ReadUpTo
        );

        tCIDLib::TVoid CompressBlock
        (
            const   TZTree&                 treeLits
            , const TZTree&                 treeDist
            , const tCIDLib::TCard4         c4LastLit
        );

        tCIDLib::TVoid CopyInflBlock
        (
            const   tCIDLib::TCard4         c4Length
        );

        tCIDLib::TVoid CopyInflBytes
        (
            const   tCIDLib::TCard4         c4FromOfs
            , const tCIDLib::TCard4         c4Count
        );

        tCIDLib::TVoid Deflate
        (
            const   tCIDZLib_::ECompMethods eMethod
        );

        tCIDLib::TVoid DeflateFast();

        tCIDLib::TVoid DeflateSlow();

        tCIDLib::TVoid DeflateStore();

        tCIDZLib_::EDataTypes eCheckDataType();

        tCIDLib::TVoid FlushBitBuf();

        tCIDLib::TVoid FlushBlock
        (
            const   tCIDLib::TInt4          i4LastBlock
            , const tCIDLib::TCard4         c4LastLit
            , const tCIDLib::TBoolean       bEOF
        );

        tCIDLib::TVoid Inflate
        (
            const   tCIDLib::TBoolean       bHaveDict
        );

        tCIDLib::TVoid InflateTable
        (
            const   tCIDZLib_::EInflTbls    eTable
            ,       tCIDLib::TCard2* const  pc2Lens
            ,       tCIDZLib_::TCode*&      pcdTable
            , const tCIDLib::TCard4         c4TblSz
            ,       tCIDLib::TCard4&        c4Bits
        );

        tCIDLib::TVoid PerBlockReset();

        tCIDLib::TVoid PullCompByte();

        tCIDLib::TVoid PutByte
        (
            const   tCIDLib::TCard1         c1ToPut
        );

        tCIDLib::TVoid PutShortLSB
        (
            const   tCIDLib::TCard2         c2ToPut
        );

        tCIDLib::TVoid PutShortMSB
        (
            const   tCIDLib::TCard2         c2ToPut
        );

        tCIDLib::TVoid PutWordLSB
        (
            const   tCIDLib::TCard4         c4ToPut
        );

        tCIDLib::TVoid ReserveInflBits
        (
            const   tCIDLib::TCard4         c4ToRes
        );

        tCIDLib::TVoid PutInflByte
        (
            const   tCIDLib::TCard1         c1ToPut
        );

        tCIDLib::TVoid PutInflBytes
        (
            const   tCIDLib::TCard1* const  pc1From
            , const tCIDLib::TCard4         c4Count
        );

        tCIDLib::TVoid Reset();

        tCIDLib::TVoid ResetInflBitBuf();

        tCIDLib::TVoid ScanTree
        (
                    TZTreeDescr&            tdToScan
        );

        tCIDLib::TVoid SendAllTrees
        (
            const   tCIDLib::TCard4         c4MaxBLInd
        );

        tCIDLib::TVoid SendBits
        (
            const   tCIDLib::TCard4         c4ToSend
            , const tCIDLib::TCard4         c4Bits
        );

        tCIDLib::TVoid SendCode
        (
            const   tCIDLib::TCard2         c2Code
            , const TZTree&                 treeData
        );

        tCIDLib::TVoid SendTree
        (
            const   TZTreeDescr&            tdToSend
        );

        tCIDLib::TVoid SyncInflStream();

        tCIDLib::TVoid UpdateDecompBuf();


        // -------------------------------------------------------------------
        //  Private, static data members
        // -------------------------------------------------------------------
        static const TStaticZTreeDescr s_stdBitLen;
        static const TStaticZTreeDescr s_stdDist;
        static const TStaticZTreeDescr s_stdLens;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bEndOfInput
        //      When we have read all the data we are going to get from the
        //      input stream, we set this so that we know that no more data
        //      is coming and that we have to just finish off what we've got.
        //
        //  m_bInitialized
        //      This is used to lazy init the data that depends on the
        //      aggreesiveness of compresion. If they don't explicitly init
        //      us, we'll force it with defaults upon first compress/decompress
        //      action.
        //
        //  m_c2BitBuf
        //  m_c4BitCount
        //      A lot of the format isn't byte aligned, so the SendBits() method
        //      dumps bits into here until it's full then it's flushed and
        //      starts over again.
        //
        //  m_c2LastEOBLen
        //      The last end of block length we wrote out.
        //
        //  m_c4Adler
        //      A running hash of the uncompressed data.
        //
        //  m_c4BytesAvail
        //  m_c4CurOfs
        //      The index into the sliding window of the current string we
        //      are working on, and the bytes available in the window (the
        //      amount of lookahead we can do.) For decompression, the bytes
        //      avail is used to keep up with the bytes in the sliding window.
        //
        //  m_c4InflBuf
        //      Used to read in compressed data for deflation, and we pull
        //      out bits from it as we go.
        //
        //  m_c4InputBytes
        //      Indicates how many bytes to pull from the input stream and
        //      compress/decompress. If they give us kCIDLib::c4MaxCard, we
        //      read to the end of the input stream.
        //
        //  m_c4GoodLen
        //  m_c4MaxChainLen
        //  m_c4MaxLazyLen
        //  m_c4NiceLen
        //      These are tuning parameters set by using the compression level
        //      to look up the optimal values for that compression level in a
        //      table.
        //
        //  m_c4TotalIn
        //  m_c4TotalOut
        //      We track the total bytes in and out during the compression
        //      process. The total in is also used to know when we've read
        //      all of the input data, if m_c4InputBytes indicates that we
        //      are not just reading to the end of the input stream. For
        //      decompresion total in is used to keep up with the input stream
        //      bytes read and output is the number of bytes written out.
        //
        //  m_eCompLevel
        //      The compression level provided in the init call. These are used
        //      to set a number of tuning parameters.
        //
        //  m_eDataType
        //      We make a best guess at whether the data is text or data.
        //
        //  m_eMode
        //      The overall comp/decomp mode that we are in.
        //
        //  m_eStrategy
        //      The compression strategy to use
        //
        //  m_pc1WndBuf
        //      Our actual working buffer, which is twice the size of the
        //      32K sliding buffer so that we can deal with the issue of
        //      back references to matched strings, which can be up to 32K
        //      behind us. For compression it is used to read in chunks of
        //      input, and for decompression it is used to hold chunks of
        //      output long enough to handle any back refs.
        //
        //  m_pc2DistAccum
        //  m_pc1LLAccum
        //      The buffers for counting up instances of distances and code
        //      lengths/literals. At the end these will be used to build a
        //      tree for them.
        //
        //  m_pc2HashPrev
        //  m_pc2HashTbl
        //      We have a hash table with a slot for each possible hash value.
        //      The values stored are indices into the sliding buffer. We also
        //      keep up with up to 1 previous string with the same hash.
        //
        //  m_pstrmIn
        //  m_pstrmOut
        //      Pointers to the in and output streams that provide the
        //      uncompressed data and take the compressed data. m_c4InputBytes
        //      indicates how many bytes to read from the input stream. If it
        //      is kCIDLib::c4MaxCard, we read to the end of stream.
        //
        //  m_tdDynBitLen
        //  m_tdDynDist
        //  m_tdDynLen
        //      The dymamic trees for bit lengths, distances, and code lengths.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bEndOfInput;
        tCIDLib::TBoolean       m_bInitialized;
        tCIDLib::TCard2         m_c2BitBuf;
        tCIDLib::TCard4         m_c4BitCount;
        tCIDLib::TCard4         m_c2LastEOBLen;
        tCIDLib::TCard4         m_c4Adler;
        tCIDLib::TCard4         m_c4BytesAvail;
        tCIDLib::TCard4         m_c4CurOfs;
        tCIDLib::TCard4         m_c4GoodLen;
        tCIDLib::TCard4         m_c4InflBuf;
        tCIDLib::TCard4         m_c4InputBytes;
        tCIDLib::TCard4         m_c4MaxChainLen;
        tCIDLib::TCard4         m_c4MaxLazyLen;
        tCIDLib::TCard4         m_c4NiceLen;
        tCIDLib::TCard4         m_c4TotalIn;
        tCIDLib::TCard4         m_c4TotalOut;
        tCIDZLib::ECompLevels   m_eCompLevel;
        tCIDZLib_::EDataTypes   m_eDataType;
        tCIDZLib_::EModes       m_eMode;
        tCIDZLib_::EStrategies  m_eStrategy;
        tCIDLib::TCard1*        m_pc1LLAccum;
        tCIDLib::TCard1*        m_pc1WndBuf;
        tCIDLib::TCard2*        m_pc2DistAccum;
        tCIDLib::TCard2*        m_pc2HashPrev;
        tCIDLib::TCard2*        m_pc2HashTbl;
        TBinInStream*           m_pstrmIn;
        TBinOutStream*          m_pstrmOut;
        TZTreeDescr             m_tdDynBitLen;
        TZTreeDescr             m_tdDynDist;
        TZTreeDescr             m_tdDynLens;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TZLibCompImpl,TObject)
};

#pragma CIDLIB_POPPACK


