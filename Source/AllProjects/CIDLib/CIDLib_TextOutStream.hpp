//
// FILE NAME: CIDLib_TextOutStream.hpp
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
//  This is the header for the CIDLib_TextOutStream.Cpp file, which implements
//  the TTextOutStream.
//
//  We implement an 'indent janitor' that will set the stream indentation on a scoped
//  basis, which is very useful, and put it back to the original when it destructs.
//  And we implement one that saves the current general output format and restores it.
//
//  And we also add some stuff to the kCIDLib namespace, related to special values
//  that can be sent to a text output stream, like end line, new line, etc...
//
//  Streams are NOT thread safe so synchronize all operations if a single stream is
//  being used by multiple threads.
//
// CAVEATS/GOTCHAS:
//
//  1)  If no converter is provided, then a TUTFConverter object is created
//      internally for use by the stream. It uses the default encoding that
//      is set by that class' default ctor.
//
//  2)  Though the underlying binary stream is cached, this class is cached
//      as well, in order to keep performance up.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TTextOutStream;


// ---------------------------------------------------------------------------
//   CLASS: TStreamJanitor
//  PREFIX: jan
// ---------------------------------------------------------------------------
class CIDLIBEXP TStreamJanitor
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TStreamJanitor() = delete;

        CIDLib_Suppress(26429) // It can legally be null
        TStreamJanitor
        (
                    TTextOutStream* const   pstrmToSanitize
        );

        TStreamJanitor(const TStreamJanitor&) = delete;
        TStreamJanitor(TStreamJanitor&&) = delete;

        ~TStreamJanitor();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TStreamJanitor& operator=(const TStreamJanitor&) = delete;
        TStreamJanitor& operator=(TStreamJanitor&&) = delete;
        tCIDLib::TVoid* operator new(size_t) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TStreamFmt& strmfSaved() const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pstrmToSanitize
        //      This is the pointer to the text out stream we are providing
        //      janitorial services for.
        //
        //  m_strmfSave
        //      This is the saved stream format state to be restored.
        // -------------------------------------------------------------------
        TTextOutStream* m_pstrmToSanitize;
        TStreamFmt      m_strmfSave;
};


// ---------------------------------------------------------------------------
//   CLASS: TStreamIndentJan
//  PREFIX: jan
// ---------------------------------------------------------------------------
class CIDLIBEXP TStreamIndentJan
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TStreamIndentJan() = delete;

        TStreamIndentJan
        (
                    TTextOutStream* const   pstrmToSanitize
            , const tCIDLib::TCard4         c4Adjust
        );

        TStreamIndentJan(const TStreamIndentJan&) = delete;
        TStreamIndentJan(TStreamIndentJan&&) = delete;

        ~TStreamIndentJan();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TStreamIndentJan& operator=(const TStreamIndentJan&) = delete;
        TStreamIndentJan& operator=(TStreamIndentJan&&) = delete;
        tCIDLib::TVoid* operator new(size_t) = delete;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4OldIndent
        //      This is the saved indent that we will put back
        //
        //  m_pstrmToSanitize
        //      This is the pointer to the text out stream we are providing
        //      janitorial services for.
        // -------------------------------------------------------------------
        tCIDLib::TCard4 m_c4OldIndent;
        TTextOutStream* m_pstrmToSanitize;
};


// ---------------------------------------------------------------------------
//   CLASS: TTextOutStream
//  PREFIX: strm
// ---------------------------------------------------------------------------
class CIDLIBEXP TTextOutStream : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static data
        //
        //  strmfDefaults
        //      This object contains the default stream format. So you can
        //      reset the defaults by dumping it to the text stream.
        //
        //  strmfXXX
        //      Some very commonly used stream formats so that folks don't
        //      have to constantly create them from scratch.
        // -------------------------------------------------------------------
        static const TStreamFmt& strmfDefault();
        static const TStreamFmt& strmfHex();
        static const TStreamFmt& strmfHex2();
        static const TStreamFmt& strmfHex4();


        // -------------------------------------------------------------------
        //  Public types
        //
        // -------------------------------------------------------------------
        class Spaces
        {
            public :
                explicit constexpr Spaces(const tCIDLib::TCard4 c4Count) : c4Count(c4Count)
                {
                }
                tCIDLib::TCard4 c4Count;
        };

        class Fill
        {
            public :
                explicit constexpr Fill(const tCIDLib::TCh chNewFill) : chFill(chNewFill)
                {
                }
                tCIDLib::TCh chFill;
        };

        class Justify
        {
            public :
                explicit constexpr Justify(const tCIDLib::EHJustify  eNewJustification)
                    : eJustification(eNewJustification)
                {
                }
                tCIDLib::EHJustify eJustification;
        };

        class Precision
        {
            public :
                explicit constexpr Precision(const tCIDLib::TCard4 c4NewPrecision)

                    : c4Precision(c4NewPrecision)
                {
                }
                tCIDLib::TCard4 c4Precision;
        };

        class RepChars
        {
            public :
                constexpr RepChars( const   tCIDLib::TCh    chOutChar
                                    , const tCIDLib::TCard4 c4OutCount)

                    : chOut(chOutChar), c4Count(c4OutCount)
                {
                }
                tCIDLib::TCh chOut;
                tCIDLib::TCard4 c4Count;
        };

        class Radix
        {
            public :
                explicit constexpr Radix(const tCIDLib::ERadices eNewRadix) : eRadix(eNewRadix)
                {
                }
                tCIDLib::ERadices eRadix;
        };

        class Width
        {
            public :
                explicit constexpr Width(const tCIDLib::TCard4 c4NewWidth) : c4Width(c4NewWidth)
                {
                }
                tCIDLib::TCard4 c4Width;
        };



        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TTextOutStream
        (
                    TBinOutStream* const    pstrmToAdopt
            ,       TTextConverter* const   ptcvtToAdopt = nullptr
        );

        TTextOutStream
        (
                    TBinOutStream* const    pstrmToAdopt
            , const TStreamFmt&             strmfToUse
            ,       TTextConverter* const   ptcvtToAdopt = nullptr
        );

        TTextOutStream(const TTextOutStream&) = delete;
        TTextOutStream(TTextOutStream&&) = delete;

        ~TTextOutStream();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TTextOutStream& operator=(const TTextOutStream&) = delete;
        TTextOutStream& operator=(TTextOutStream&&) = delete;

        TTextOutStream& operator<<
        (
            const   tCIDLib::TBoolean       bToWrite
        );

        TTextOutStream& operator<<
        (
            const   tCIDLib::TCh            chToWrite
        );

        TTextOutStream& operator<<
        (
            const   tCIDLib::TCard1         c1ToWrite
        );

        TTextOutStream& operator<<
        (
            const   tCIDLib::TCard2         c2ToWrite
        );

        TTextOutStream& operator<<
        (
            const   tCIDLib::TCard4         c4ToWrite
        );

        TTextOutStream& operator<<
        (
            const   tCIDLib::TCard8&        c8ToWrite
        );

        TTextOutStream& operator<<
        (
            const   tCIDLib::TFloat4        f4ToWrite
        );

        TTextOutStream& operator<<
        (
            const   tCIDLib::TFloat8&       f8ToWrite
        );

        TTextOutStream& operator<<
        (
            const   tCIDLib::TInt1          i1ToWrite
        );

        TTextOutStream& operator<<
        (
            const   tCIDLib::TInt2          i2ToWrite
        );

        TTextOutStream& operator<<
        (
            const   tCIDLib::TInt4          i4ToWrite
        );

        TTextOutStream& operator<<
        (
            const   tCIDLib::TInt8&         i8ToWrite
        );

        TTextOutStream& operator<<
        (
            const   tCIDLib::TUInt          uToWrite
        );

        TTextOutStream& operator<<
        (
            const   tCIDLib::TSInt          iToWrite
        );

        TTextOutStream& operator<<
        (
            const   TStringView&            strvToWrite
        );

        TTextOutStream& operator<<
        (
            const   TString&                strToWrite
        );

        TTextOutStream& operator<<
        (
            const   tCIDLib::TCh* const     pszToWrite
        );

        TTextOutStream& operator<<
        (
            const   Width                   NewWidth
        );

        TTextOutStream& operator<<
        (
            const   Precision               NewPrecision
        );

        TTextOutStream& operator<<
        (
            const   Spaces                  SpacesToWrite
        );

        TTextOutStream& operator<<
        (
            const   tCIDLib::ETStrmSpecial  eSpecial
        );

        TTextOutStream& operator<<
        (
            const   tCIDLib::EStrmSpecial   eSpecial
        );

        TTextOutStream& operator<<
        (
            const   Justify                 NewJustification
        );

        TTextOutStream& operator<<
        (
            const   Fill                    NewFillChar
        );

        TTextOutStream& operator<<
        (
            const   Radix                   NewRadix
        );

        TTextOutStream& operator<<
        (
            const   RepChars&               RepChars
        );

        TTextOutStream& operator<<
        (
            const   TStreamFmt&             strmfNewFmt
        );



        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bSuppressIndent() const;

        tCIDLib::TBoolean bSuppressIndent
        (
            const   tCIDLib::TBoolean       bToSet
        );

        tCIDLib::TCh chFill() const;

        tCIDLib::TCh chFill
        (
            const   tCIDLib::TCh            chToSet
        );

        tCIDLib::TCard4 c4Indent() const;

        tCIDLib::TCard4 c4Indent
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        tCIDLib::TCard4 c4Precision() const;

        tCIDLib::TCard4 c4Precision
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        tCIDLib::TCard4 c4TrailingSp() const;

        tCIDLib::TCard4 c4TrailingSp
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        tCIDLib::TCard4 c4Width() const;

        tCIDLib::TCard4 c4Width
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        tCIDLib::EHJustify eJustification() const;

        tCIDLib::EHJustify eJustification
        (
            const   tCIDLib::EHJustify      eToSet
        );

        tCIDLib::ENewLineTypes eNewLineType() const;

        tCIDLib::ENewLineTypes eNewLineType
        (
            const   tCIDLib::ENewLineTypes  eToSet
        );

        tCIDLib::ERadices eRadix() const;

        tCIDLib::ERadices eRadix
        (
            const   tCIDLib::ERadices       eToSet
        );

        tCIDLib::TVoid Flush();

        //
        //  Just call the recursive helper with the start of the format buffer. We
        //  have another that flushes at the end.
        //
        template <typename... TArgs>
        tCIDLib::TVoid Format(const TString& strFmt, const TArgs... Args)
        {
            // Save any stream format stuff then call our recursive helper
            TStreamJanitor janFormat(this);

            // If we get back the buffer, then no parms, just write out fmt text
            if (pszFmtHelper(strFmt.pszBuffer(), Args...) == strFmt.pszBuffer())
                PutLine(strFmt);
        }

        template <typename... TArgs>
        tCIDLib::TVoid FormatF(const TString& strFmt, const TArgs... Args)
        {
            // Save any stream format stuff then call our recursive helper
            TStreamJanitor janFormat(this);

            // If we get back the buffer, then no parms, just write out fmt text
            if (pszFmtHelper(strFmt.pszBuffer(), Args...) == strFmt.pszBuffer())
                PutLine(strFmt);
            Flush();
        }

        tCIDLib::TVoid PutLine
        (
            const   TStringView&            strvToWrite
        );

        tCIDLib::TVoid Reset();

        tCIDLib::TVoid SetDefaultFormat();

        tCIDLib::TVoid SetFormat
        (
            const   TStreamFmt&             strmfToSet
        );

        const TString& strEncodingName() const;

        tCIDLib::TVoid SeekToEnd();

        const TTextConverter& tcvtThis() const;

        tCIDLib::TVoid WriteChars
        (
            const   tCIDLib::TCh* const     pszToWrite
            , const tCIDLib::TCard4         c4Len = 0
        );

        tCIDLib::TVoid WriteTextBody
        (
            const   TStringView&            strvSrc
        );


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TStreamFmt;


        // -------------------------------------------------------------------
        //  Hidden Constructors
        // -------------------------------------------------------------------
        TTextOutStream
        (
            const   TStreamFmt&             strmfToUse
            ,       TTextConverter* const   ptcvtToAdopt = nullptr
        );

        TTextOutStream
        (
                    TTextConverter* const   ptcvtToAdopt
        );


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AdoptStream
        (
                    TBinOutStream* const    pstrmToAdopt
        );

        TBinOutStream& strmOut();

        const TBinOutStream& strmOut() const;


    private :
        // -------------------------------------------------------------------
        //  Private class constants
        // -------------------------------------------------------------------
        static const tCIDLib::TCard4 c4CacheBufSize = 2048;


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------

        //
        //  Helpers for Format() which needs to process a list of variadic
        //  parameters.
        //
        template <typename TOne, typename... TOthers>
        const tCIDLib::TCh* pszFmtHelper(const  tCIDLib::TCh* const pszFmt
                                        , const TOne                tOne
                                        , const TOthers...          tOthers)
        {
            //
            //  We have to loop until we get a token, since we could have text bits
            //  which we just copy straight out as text.
            //
            const tCIDLib::TCh* pszStart = pszFmt;
            const tCIDLib::TCh* pszEnd = nullptr;
            while (kCIDLib::True)
            {
                tCIDLib::TCh chToken;
                const TString::ETokenFind eFindRes = TString::eFindToken
                (
                    pszStart, chToken, m_eJustification, m_c4Width, m_chFill, m_c4Precision, pszEnd
                );

                // If it fails, then give up and stop recursing
                if ((eFindRes == TString::ETokenFind::BadFormat)
                ||  (eFindRes == TString::ETokenFind::End))
                {
                    return nullptr;
                }
                 else if (eFindRes == TString::ETokenFind::Token)
                {
                    // OK, we got one, so format it out and we need to recurse
                    *this << tOne;
                    pszEnd = pszFmtHelper(pszEnd, tOthers...);
                }
                 else if (eFindRes == TString::ETokenFind::TextRun)
                {
                    // It's a text run, so just write those character out
                    WriteChars(pszStart, (pszEnd - pszStart));
                }

                // Move forward
                pszStart = pszEnd;
            }
            return pszEnd;
        }

        const tCIDLib::TCh* pszFmtHelper(const tCIDLib::TCh* pszFmt)
        {
            return pszFmt;
        }


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_achCache
        //      This is a cache buffer that is used to spool out char data. It
        //      supports much better performance by buffering up multiple
        //      text output operations and then converting and writing them to
        //      the underlying binary stream all at once.
        //
        //  m_bIndentNext
        //      Every time we do a new line, we set this. On the next non-
        //      new line, we do an indent if m_c4Indent is non-zero and then
        //      clear this flag.
        //
        //  m_bSawOD
        //      According to the m_eNewLineType we have to map any new line
        //      sequence to a particular new line sequence. But, since we
        //      have to deal with either CR/LF sequences, it's a little
        //      more complicated. We don't know if an LF is coming next or
        //      not, so we have to remember that we saw a CR. If the next
        //      thing we see is an LF, then we have to eat it because it was
        //      part of the CR/LF sequence.
        //
        //  m_bSuppressIndent
        //      This flag allows the indentation feature to be supressed, so
        //      that code that is written to create nice formatted text does
        //      not have to be changed to create a compacted version of it.
        //      This defaults to false until set otherwise.
        //
        //  m_chFill
        //      The fill character to use when text formatting into a field
        //      larger than the text being formatted.
        //
        //  m_c4Indent
        //      We allow an indent level to be set, which will cause us to
        //      put out this many spaces after any newline. So that each line
        //      is indented over. It defaults to zero.
        //
        //  m_c4Index
        //      Our current index into the cache. When its full, or the
        //      operation would overflow it, we have to flush it out. Its
        //      full when this value equals c4CacheBufSize.
        //
        //  m_c4Precision
        //      The number of decimal digits to use when formatting a floating
        //      point value.
        //
        //  m_c4TrailingSp
        //      After formatting something to a field width (set in m_c4Width), we
        //      put out this many trailing spaces, defaults to zero. This is only used
        //      if m_c4Width is non-zero, meaning we are formatting into a sized field.
        //
        //  m_c4Width
        //      The field width to format into.
        //
        //  m_eJustification
        //      They justification to use when formatting a value that is
        //      smaller than the current field width.
        //
        //  m_eNewLineType
        //      Indicates how we translate CR, LF, or CR/LF sequences that
        //      we see in the outgoing text. All of them will be converted
        //      to this format. Defaults to CR/LF.
        //
        //  m_eRadix
        //      The radix to use when formatting raw numeric values formatted
        //      to the stream. Any particular object of course can control
        //      its own formatting so this is just for raw numbers.
        //
        //  m_pstrmOut
        //      This is a pointer to our underlying binary ourput stream. We
        //      own it and clean it up at dtor time.
        //
        //  m_ptcvtThis
        //      A pointer to the text encoding converter for this stream. We
        //      own it and clean it up at dtor time.
        // -------------------------------------------------------------------
        tCIDLib::TCh            m_achCache[c4CacheBufSize];
        tCIDLib::TBoolean       m_bIndentNext;
        tCIDLib::TBoolean       m_bSawOD;
        tCIDLib::TBoolean       m_bSuppressIndent;
        tCIDLib::TCh            m_chFill;
        tCIDLib::TCard4         m_c4Indent;
        tCIDLib::TCard4         m_c4Index;
        tCIDLib::TCard4         m_c4Precision;
        tCIDLib::TCard4         m_c4TrailingSp;
        tCIDLib::TCard4         m_c4Width;
        tCIDLib::EHJustify      m_eJustification;
        tCIDLib::ENewLineTypes  m_eNewLineType;
        tCIDLib::ERadices       m_eRadix;
        TBinOutStream*          m_pstrmOut;
        TTextConverter*         m_ptcvtThis;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTextOutStream,TObject)
};




#pragma CIDLIB_POPPACK



// ---------------------------------------------------------------------------
//  Create some short cut values for the special stream enum values. These
//  are often used so having a short cut is very convenient.
// ---------------------------------------------------------------------------
namespace kCIDLib
{
    const tCIDLib::ETStrmSpecial DNewLn     = tCIDLib::ETStrmSpecial::DblNewLine;
    const tCIDLib::ETStrmSpecial EndLn      = tCIDLib::ETStrmSpecial::EndLine;
    const tCIDLib::ETStrmSpecial NewLn      = tCIDLib::ETStrmSpecial::NewLine;
    const tCIDLib::ETStrmSpecial NewEndLn   = tCIDLib::ETStrmSpecial::NewEndLine;

    const TTextOutStream::Radix  sfmtDec(tCIDLib::ERadices::Dec);
    const TTextOutStream::Radix  sfmtHex(tCIDLib::ERadices::Hex);
}

