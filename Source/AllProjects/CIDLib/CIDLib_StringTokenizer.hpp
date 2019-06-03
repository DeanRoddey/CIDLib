//
// FILE NAME: CIDLib_StringTokenizer.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/23/1998
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
//  This is the header for the CIDLib_StringTokenizer.Cpp file. This file
//  implements the TStringTokenizer class, which is a mechanism for pulling
//  out consequtive 'tokens' from a string, which token means just any string
//  of characters separated from any other string of characters by one or more
//  characters from a list of 'whitespace' characters.
//
// CAVEATS/GOTCHAS:
//
//  1)  Each string tokenizer maintains a pointer to a string object that is
//      tokenizing. It does not adopt it or own it in any way, so destroying
//      the tokenizer does not affect the string in any way. However, the
//      string cannot be modified while the tokenizer is working on it,
//      otherwise the results will be undefined.
//
//  2)  The tokenizer keeps a const pointer to the string being tokenized,
//      so it does not modify it in any way.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TStringTokenizer
//  PREFIX: stok
// ---------------------------------------------------------------------------
class CIDLIBEXP TStringTokenizer : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TBoolean bParseQuotedCommaList
        (
            const   TString&                strText
            ,       TCollection<TString>&   colToFill
            ,       tCIDLib::TCard4&        c4ErrIndex
        );

        static tCIDLib::TBoolean bParseCSVLine
        (
            const   TString&                strText
            ,       TCollection<TString>&   colToFill
            ,       tCIDLib::TCard4&        c4ErrIndex
        );

        static tCIDLib::TBoolean bParseSpacedTokens
        (
            const   TString&                strText
            ,       TCollection<TString>&   colToFill
            , const tCIDLib::TBoolean       bAppend = kCIDLib::False
        );

        static tCIDLib::TVoid BuildQuotedCommaList
        (
            const   TString&                strToAdd
            ,       TString&                strAccum
        );

        static tCIDLib::TVoid BuildQuotedCommaList
        (
            const   TCollection<TString>&   colValues
            ,       TString&                strAccum
        );


        // -------------------------------------------------------------------
        //  Constuctors and Destructor
        // -------------------------------------------------------------------
        TStringTokenizer();

        TStringTokenizer
        (
            const   TString* const          pstrToTokenize
            , const TString&                strWhitespace
        );

        TStringTokenizer
        (
            const   TString* const          pstrToTokenize
            , const tCIDLib::TCh* const     pszWhitespace
        );

        TStringTokenizer(const TStringTokenizer&) = delete;

        ~TStringTokenizer();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TStringTokenizer& operator=(const TStringTokenizer&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bEatNextToken();

        tCIDLib::TBoolean bGetNextToken
        (
                    TString&                strToFill
        );

        tCIDLib::TBoolean bGetRestOfLine
        (
                    TString&                strToFill
        );

        tCIDLib::TBoolean bMoreTokens() const;

        tCIDLib::TBoolean bPeekNextToken
        (
                    TString&                strToFill
        )   const;

        tCIDLib::TBoolean bPeekRestOfLine
        (
                    TString&                strToFill
        )   const;

        tCIDLib::TCard4 c4BreakApart
        (
                    TCollection<TString>&   colToFill
        );

        tCIDLib::TCard4 c4CurToken() const;

        tCIDLib::TVoid Reset();

        tCIDLib::TVoid Reset
        (
            const   TString* const          pstrToTokenize
            , const TString&                strWhitespace
        );

        tCIDLib::TVoid Reset
        (
            const   TString* const          pstrToTokenize
            , const tCIDLib::TCh* const     pszWhitespace
        );

        const TString& strSrcText() const;

        const TString& strWhitespace() const;

        const TString& strWhitespace
        (
            const   TString&                strToSet
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4CurTokenLen() const;

        tCIDLib::TVoid Init();

        tCIDLib::TVoid FindNext();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4CurOffset
        //      The offset into the string of the token that will be
        //      returned on the next call to strNextToken().
        //
        //  m_c4CurToken
        //      The number of the token that will be returned on the next
        //      call to strNextToken(). Zero is the first token.
        //
        //  m_c4SourceLen
        //      The length of the source string upon construction. Since the
        //      string cannot be modified while tokenizing, this should
        //      remain correct. Debug build will confirm this.
        //
        //  m_pstrSrc
        //      This is the string that we are tokenizing. It is a const
        //      pointer since we only read the string.
        //
        //  m_strWhitespace
        //      The characters that make up the whitespace that divides the
        //      tokens.
        // -------------------------------------------------------------------
        tCIDLib::TCard4 m_c4CurOffset;
        tCIDLib::TCard4 m_c4CurToken;
        tCIDLib::TCard4 m_c4SourceLen;
        const TString*  m_pstrSrc;
        TString         m_strWhitespace;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TStringTokenizer,TObject)
};

#pragma CIDLIB_POPPACK

