//
// FILE NAME: CIDLib_TextInStream.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/05/1999
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
//  This is the header for the CIDLib_TextInStream.Cpp file, which implements
//  the TTextInStream.
//
// CAVEATS/GOTCHAS:
//
//  1)  If no converter is supplied then a TUTFConverter is created internally
//      to use for converting the incoming text, and it tries to autosense
//      the encoding (within the ASCII - UTF-8/16 family at least.)
//
//  2)  Though the underlying binary stream also caches, we cache separately
//      at this level to maintain performance for transcoding and single char
//      internal spooling (which happens a lot inside.)
//
//  3)  The cache for an input text stream is considerably larger than the
//      one for an output stream, since there is no danger of losing anything
//      if the process or workstation goes down, as there is when caching
//      outgoing text.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)


// ---------------------------------------------------------------------------
//   CLASS: TTextInStream
//  PREFIX: strm
// ---------------------------------------------------------------------------
class CIDLIBEXP TTextInStream : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TTextInStream
        (
                    TBinInStream* const     pstrmToAdopt
            ,       TTextConverter* const   ptcvtToAdopt = nullptr
        );

        TTextInStream(const TTextInStream&) = delete;
        TTextInStream(TTextInStream&&) = delete;

        ~TTextInStream();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TTextInStream& operator=(const TTextInStream&) = delete;
        TTextInStream& operator=(TTextInStream&&) = delete;

        TTextInStream& operator>>
        (
                    tCIDLib::TBoolean&      bToFill
        );

        TTextInStream& operator>>
        (
                    tCIDLib::TCard1&        c1ToFill
        );

        TTextInStream& operator>>
        (
                    tCIDLib::TCard2&        c2ToFill
        );

        TTextInStream& operator>>
        (
                    tCIDLib::TCard4&        c4ToFill
        );

        TTextInStream& operator>>
        (
                    tCIDLib::TCard8&        c8ToFill
        );

        TTextInStream& operator>>
        (
                    tCIDLib::TFloat4&       f4ToFill
        );

        TTextInStream& operator>>
        (
                    tCIDLib::TFloat8&       f8ToFill
        );

        TTextInStream& operator>>
        (
                    tCIDLib::TInt1&         i1ToFill
        );

        TTextInStream& operator>>
        (
                    tCIDLib::TInt2&         i2ToFill
        );

        TTextInStream& operator>>
        (
                    tCIDLib::TInt4&         i4ToFill
        );

        TTextInStream& operator>>
        (
                    tCIDLib::TInt8&         i8ToFill
        );

        TTextInStream& operator>>
        (
                    tCIDLib::TUInt&         uToFill
        );

        TTextInStream& operator>>
        (
                    tCIDLib::TSInt&         iToFill
        );

        TTextInStream& operator>>
        (
                    tCIDLib::TCh&           chToFill
        );

        TTextInStream& operator>>
        (
                    TString&                strToFill
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bDataInCache() const;

        tCIDLib::TBoolean bEndOfStream() const;

        tCIDLib::TBoolean bRead();

        tCIDLib::TCard4 c4GetLine
        (
                    TString&                strToFill
            , const tCIDLib::TCard4         c4MaxChars = 0
            , const tCIDLib::TBoolean       bStripWhitespace = kCIDLib::False
        );

        tCIDLib::TCard4 c4GetToken
        (
                    TString&                strToFill
            , const TString&                strSepChars
        );

        tCIDLib::TCh chPeek
        (
            const   tCIDLib::TBoolean       bThrowIfEOS = kCIDLib::False
        );

        tCIDLib::TCh chRead();

        tCIDLib::TCard1 c1Read
        (
            const   tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
        );

        tCIDLib::TCard2 c2Read
        (
            const   tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
        );

        tCIDLib::TCard4 c4Read
        (
            const   tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
        );

        tCIDLib::TCard4 c4ReadChars
        (
                    tCIDLib::TCh* const     pszToFill
            , const tCIDLib::TCard4         c4MaxChars
        );

        tCIDLib::TCard8 c8Read
        (
            const   tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
        );

        tCIDLib::ENewLineTypes eNewLineType() const;

        tCIDLib::ENewLineTypes eNewLineType
        (
            const   tCIDLib::ENewLineTypes  eToSet
        );

        tCIDLib::TFloat4 f4Read();

        tCIDLib::TFloat8 f8Read();

        tCIDLib::TInt1 i1Read
        (
            const   tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
        );

        tCIDLib::TInt2 i2Read
        (
            const   tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
        );

        tCIDLib::TInt4 i4Read
        (
            const   tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
        );

        tCIDLib::TInt8 i8Read
        (
            const   tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
        );

        tCIDLib::TVoid Reset();

        const TString& strEncodingName() const;

        TTextConverter& tcvtThis();

        const TTextConverter& tcvtThis() const;


    protected :
        // -------------------------------------------------------------------
        //  Hidden Constructors
        // -------------------------------------------------------------------
        TTextInStream
        (
                    TTextConverter* const   ptcvtToAdopt
        );


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AdoptStream
        (
                    TBinInStream* const     pstrmToAdopt
        );


    private :
        // -------------------------------------------------------------------
        //  Private class constants
        // -------------------------------------------------------------------
        static constexpr tCIDLib::TCard4 c4CacheBufChars = 4096;
        static constexpr tCIDLib::TCard4 c4SpareByteCnt  = 16;


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bReloadCache() const;

        tCIDLib::TCard4 c4GetRawBuf
        (
                    tCIDLib::TCh* const     pszToFill
            , const tCIDLib::TCard4         c4MaxChars
            ,       tCIDLib::TBoolean&      bGotEnd
        );

        tCIDLib::TCh chNextChar
        (
            const   tCIDLib::TBoolean       bThrowIfEnd
        );

        tCIDLib::TVoid EatLineRemainder();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  NOTE:   All of the cache related members must be mutable because
        //          they might have to be faulted in within some ostensibly
        //          read-only methods, such as bEndOfStream(). So we make
        //          the bReloadCache() method const as well.
        //
        //          The text converter must be mutable because the cache
        //          refresh method, if a converter isn't provided, will try
        //          to auto-sense and create one.
        //
        //  m_ac1SpareBytes
        //      On each load of the buffer, there is a possibility that we
        //      will have a few bytes left over that don't represent a whole
        //      character in the current encoding. This buffer is used to save
        //      them until the next time. It can hold 16 bytes which should
        //      be wayyyy more than required to hold a partial last encoded
        //      character.
        //
        //  m_achCache
        //      This is a cache buffer that is used to spool in char data. It
        //      supports much better performance by asking the underlying
        //      binary stream for buffers of text at a time, and then spooling
        //      it out as needed.
        //
        //  m_c4CacheOfs
        //      Our current offset into the cache. When its equal to the
        //      current cache size, then we have to reload.
        //
        //  m_c4CacheSize
        //      The number of chars currently in the cache. Its often not
        //      possible to fully fill the cache because the bytes read in
        //      a reload will often create fewer chars when the encoding is
        //      multiple byte.
        //
        //  m_c4SpareBytes
        //      Indicates how many bytes are in the m_ac1SpareBytes array.
        //      Before reloading the cache, these are copied to the bottom
        //      of the cache.
        //
        //  m_chUnGet
        //      We need a single character unget mechanism internally. This
        //      char provides that. If its non-zero, then its an ungotten
        //      char.
        //
        //  m_eNewLineType
        //      Indicates what type of newlines to expect. By default it
        //      will be CR/LF. But, in some cases, the app knows it won't
        //      be that, and can set it to CR or LF. This can make a
        //      difference, because if it is really CR, but this is set
        //      to CR/LF, this class will look for a LF following a CR
        //      when a line read is done. If there isn't one there,  this
        //      can cause a timeout on the implementation class if the
        //      source of data is something like a socket. By setting it
        //      to CR, we know that the end of line has been reached and
        //      can stop the line read immediately.
        //
        //  m_pstrmIn
        //      This is a pointer to our underlying binary input stream.
        //
        //  m_ptcvtThis
        //      A pointer to the text encoding converter for this stream.
        // -------------------------------------------------------------------
        mutable tCIDLib::TCard1 m_ac1SpareBytes[c4SpareByteCnt];
        mutable tCIDLib::TCh    m_achCache[c4CacheBufChars];
        mutable tCIDLib::TCard4 m_c4CacheOfs;
        mutable tCIDLib::TCard4 m_c4CacheSize;
        mutable tCIDLib::TCard4 m_c4SpareBytes;
        mutable tCIDLib::TCh    m_chUnGet;
        tCIDLib::ENewLineTypes  m_eNewLineType;
        TBinInStream*           m_pstrmIn;
        mutable TTextConverter* m_ptcvtThis;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTextInStream,TObject)
};

#pragma CIDLIB_POPPACK

