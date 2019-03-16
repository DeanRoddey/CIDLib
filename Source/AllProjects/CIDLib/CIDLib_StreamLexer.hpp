//
// FILE NAME: CIDLib_StreamLexer.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/28/1993
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
//  This file provides the TStreamLexer class. This class provides lexical
//  analysis services for text streams. Basically it provides a lot of the
//  grunt work that is always associated with breaking text files into
//  tokens. A simple derivative of this class can create a powerful parser.
//
//  This guy does a relatively straightforward lexical analysis that is
//  appropriate for simpler, more ad hoc lexing. It does not implement a
//  Lexx/Yacc type of regular expression based scheme, as would be used by
//  say a language compiler. Such a scheme would be a bit much for the
//  average user just pulling simple tabular text out of a file.
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
//   CLASS: TStreamLexer
//  PREFIX: lexr
// ---------------------------------------------------------------------------
class CIDLIBEXP TStreamLexer : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  An enum for the eTokenMatch() method.
        // -------------------------------------------------------------------
        enum class ETokenMatches
        {
            None
            , First
            , Full
        };


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TStreamLexer() = delete;

        TStreamLexer
        (
                    TTextInStream* const    pstrmToUse
            , const tCIDLib::EAdoptOpts     eAdopt
        );

        TStreamLexer(const TStreamLexer&) = delete;

        ~TStreamLexer();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TStreamLexer& operator=(const TStreamLexer&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddSyntaxToken
        (
            const   TString&                strBuf
        );

        tCIDLib::TCard4 c4CurLine() const;

        tCIDLib::TCh chGetNextChar();

        tCIDLib::TVoid FlushLine();

        tCIDLib::TVoid GetNextToken
        (
            const   TString&                strSeparators
            ,       TString&                strTarget
            , const TString&                strNoSyntax = TString::Nul_TString()
        );

        tCIDLib::TVoid GetLineRemainder
        (
                    TString&                strTarget
        );

        tCIDLib::TVoid UnGetChar
        (
            const   tCIDLib::TCh            chPush
        );

        tCIDLib::TVoid UnGetToken
        (
            const   TString&                strToUnGet
        );

        tCIDLib::TVoid UnGetToken
        (
            const   tCIDLib::TCh* const     pszToUnGet
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCh chGetNext();

        ETokenMatches eTokenMatch
        (
            const   tCIDLib::TCh            chFirst
            , const tCIDLib::TCh            chSecond    = 0
            , const TString&                strNoSyntax = TString::Nul_TString()
        );

        tCIDLib::TVoid UnGet
        (
            const   tCIDLib::TCh            chPush
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4CurLine
        //      This is updated to reflect the current line of the input
        //      file. It is bumped up each time that a newline character is
        //      passed.
        //
        //  m_c4TokenIndex
        //      This is the current count of items in the m_ac2Tokens[]
        //      array. AddSyntaxToken() bumps up this index as new tokens
        //      are added.
        //
        //  m_eAdopt
        //      This indicates whether we adopted the stream that we are
        //      parsing. If so, we delete it when we destruct.
        //
        //  m_i4StackIndex
        //      This is the index of the current pushback stack top. It is
        //      just an index into m_pchCharStack. If the stack is empty it
        //      set to -1.
        //
        //  m_pac4Tokens
        //      This is a list of the tokens. Since they are all 1 or 2 chars,
        //      we encoded them in a TCard4 value. This makes for fast
        //      comparisons, i.e. fast compiles. m_c4TokenIndex is the current
        //      index into this array. AddSyntaxToken() is used to load
        //      the list.
        //
        //  m_pchCharStack
        //      This is a stack for pushing characters back. It is allocated
        //      as required, though its limited in size to a few K.
        //
        //  m_pstrmSrc
        //      This is the stream input source. It can be adopted or just
        //      referenced.
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4CurLine;
        tCIDLib::TCard4         m_c4TokenIndex;
        tCIDLib::EAdoptOpts     m_eAdopt;
        tCIDLib::TInt4          m_i4StackIndex;
        tCIDLib::TCard4*        m_pac4Tokens;
        tCIDLib::TCh*           m_pchCharStack;
        TTextInStream*          m_pstrmSrc;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TStreamLexer,TObject)
};

#pragma CIDLIB_POPPACK

