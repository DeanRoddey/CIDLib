//
// FILE NAME: CIDMacroEng_ParserSrc_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/27/2003
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header file for the CIDMacroEng_ParserSrc.cpp file, which
//  implements a helper class used by the parser. The parse is recursive because
//  classes import other classes, which must be parsed before the main class
//  can be parsed. So the parser uses a stack of 'parser source' objects to
//  keep track of the source currently being parsed. When it hits an import,
//  it can push the current one and create a new source object and make it
//  the current source, and invoke itself recursively to parse it.
//
//  TTokenInfo is used to support token look ahead. We have to remember
//  the token type, its text, and its starting line/col position. These
//  objects can be pushed onto a token pushback stack. eGetNextToken()
//  will look there first before parsing more source text.
//
//  These are purely internal classes, so they doesn't need to derive from
//  TObject or be exported.
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
//   CLASS: TParserSrc
//  PREFIX: psrc
// ---------------------------------------------------------------------------
class TParserSrc
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TParserSrc
        (
                    TTextInStream* const    pstrmToAdopt
            , const TString&                strClassPath
            , const tCIDLib::TBoolean       bDebugMode
        );

        ~TParserSrc();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsNameChar
        (
            const   tCIDLib::TCh            chToCheck
        )   const;

        tCIDLib::TBoolean bIsSpecialChar
        (
            const   tCIDLib::TCh            chToCheck
        )   const;

        tCIDLib::TBoolean bIfPeeked
        (
            const   tCIDMacroEng::ETokens      eToCheck
        );

        tCIDLib::TCard4 c4Col() const;

        tCIDLib::TCard4 c4Line() const;

        tCIDLib::TCh chEscape
        (
            const   TString&                strToEscape
        );

        tCIDLib::TCh chGetNextChar();

        tCIDLib::TCh chPeekNextChar();

        tCIDMacroEng::ETokens eGetNextToken
        (
                    TString&                strToFill
            , const tCIDLib::TBoolean       bEOFOk = kCIDLib::False
        );

        tCIDMacroEng::ETokens eGetNextToken
        (
            const   tCIDLib::TBoolean       bEOFOk = kCIDLib::False
        );

        tCIDMacroEng::ETokens eMapToken
        (
            const   tCIDLib::TCh* const     pszToMap
        )   const;

        tCIDMacroEng::ETokens ePeekNextToken();

        tCIDMacroEng::ETokens ePeekNextToken
        (
                    TString&                strText
        );

        tCIDLib::TVoid EatLineRemainder();

        tCIDLib::TVoid EatWhitespace();

        tCIDLib::TVoid EscapeStr
        (
                    TString&                strToEscape
        );

        tCIDLib::TVoid GetLineRemainder
        (
                    TString&                strToFill
        );

        tCIDLib::TVoid PushBack
        (
            const   tCIDLib::TCh            chToPush
        );

        tCIDLib::TVoid PushBack
        (
            const   tCIDMacroEng::ETokens      eTypeToPush
            ,       TString&                strToPush
        );

        tCIDLib::TVoid PushBack
        (
            const   TString&                strToPush
        );

        const TString& strPath() const;

        TString strTokenText
        (
            const   tCIDMacroEng::ETokens      eToken
        );

        tCIDLib::TVoid SavePos();


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCh chSpoolAChar();

        tCIDMacroEng::ETokens eExtractToken
        (
                    TString& strText
            , const tCIDLib::TBoolean       bEOFOk
        );

        tCIDMacroEng::ETokens eProcessBeginDebug
        (
                    TString&                strToFill
            , const tCIDLib::TBoolean       bEOFOk
        );


        // -------------------------------------------------------------------
        //  Privaet data members
        //
        //  m_bDebugMode
        //      We are given this at ctor time, and it tells us if we are
        //      to include debug-only conditional blocks or not.
        //
        //  m_c4CondCount
        //      Used to count nesting levels on conditional debug blocks.
        //      If we hit the EOF and it's non-zero, that's an error.
        //
        //  m_c4CurCol
        //  m_c4CurLine
        //      The actual current line/col info. This is not what is given
        //      to the outside world. They get the Save version, which
        //      represent where we started parsing thing that caused the
        //      problem. This represents the actual parsing position past
        //      what was just parsed.
        //
        //  m_c4LastLen
        //      We have to occasionally push back the end of line, because we
        //      are pulling out a token at the end, hit the end and need to
        //      unget it. This requires putting the column back to the right
        //      place at the end of the previous line, so we save the last line
        //      length. After ungetting it, we zero it, so trying to unget
        //      past two lines would cause an error. The big reason for this
        //      is that if the token at the end was itself ungotten, the
        //      ungetting has to start back up at the end of the previous line.
        //
        //  m_c4SaveCol
        //  m_c4SaveLine
        //      The SavePos() method will put the current line/col into these
        //      variables. This lets the parsing code remember the place where
        //      it started parsing, so that it can give that as the error pos.
        //
        //  m_fcolCharStack
        //      We use a fundamental stack collection as a char pushback stack,
        //      so that we can look ahead one or more chars in the stream when
        //      needed.
        //
        //  m_pstrmSrc
        //      A pointer to the stream that we are streaming our class source
        //      from.
        //
        //  m_strClassPath
        //      The class path of the class we are parsing, for error
        //      reporting purposes mainly.
        //
        //  m_strTmp
        //      For when they ask us to peek and they don't want any text,
        //      just the token. We have to have a place to build up the token.
        //
        //  m_strTmpExt
        //      A temp just for the use of the eExtractToken method, which
        //      cannot use m_strTmp since the higher level stuff might have
        //      that in use when then call the token extraction method.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean           m_bDebugMode;
        tCIDLib::TCard4             m_c4CondCount;
        tCIDLib::TCard4             m_c4CurCol;
        tCIDLib::TCard4             m_c4CurLine;
        tCIDLib::TCard4             m_c4LastLen;
        tCIDLib::TCard4             m_c4SaveCol;
        tCIDLib::TCard4             m_c4SaveLine;
        TFundStack<tCIDLib::TCh>    m_fcolCharStack;
        TTextInStream*              m_pstrmSrc;
        TString                     m_strClassPath;
        TString                     m_strTmp;
        TString                     m_strTmpExt;
};

#pragma CIDLIB_POPPACK


