//
// FILE NAME: CIDRegX_RegExEngine.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/28/1998
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
//  This is the header for the CIDLib_RegEx.cpp file, which implements
//  the TRegEx class. This calss is a regular expression engine in the vein
//  of a 'grep' like tool. It uses a simple, runtime interpreted, stack based
//  NFA for its work (which is common for this type of regular expression
//  engine.)
//
//  This class provides its own parser for the regular expression, so it has
//  a fixed syntax similar to that of most grep programs. So you cannot give
//  it an arbitarily built binary regular expression data structure. You can
//  only give it pattern strings which it will parse and use to build that
//  data structure itself internally.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)


// ----------------------------------------------------------------------------
//   CLASS: TRegEx
//  PREFIX: regx
// ----------------------------------------------------------------------------
class CIDREGXEXP TRegEx : public TObject, public MFormattable
{
    public :
        // --------------------------------------------------------------------
        //  Constructors and Destructor
        // --------------------------------------------------------------------
        TRegEx();

        TRegEx
        (
            const   TString&                strExpression
        );

        TRegEx
        (
            const   tCIDLib::TCh* const     pszExpression
        );

        TRegEx(const TRegEx&) = delete;
        TRegEx(TRegEx&&) = delete;

        ~TRegEx();


        // --------------------------------------------------------------------
        //  Public operators
        // --------------------------------------------------------------------
        TRegEx& operator=(const TRegEx&) = delete;
        TRegEx& operator=(TRegEx&&) = delete;


        // --------------------------------------------------------------------
        //  Public, non-virtual methods
        // --------------------------------------------------------------------
        tCIDLib::TBoolean bFindMatch
        (
            const   TString&                strFindIn
            ,       tCIDLib::TCard4&        c4Ofs
            ,       tCIDLib::TCard4&        c4Len
            , const tCIDLib::TBoolean       bOnlyAtStart = kCIDLib::True
            , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::False
        )   const;

        tCIDLib::TBoolean bFindMatch
        (
            const   tCIDLib::TCh* const     pszFindIn
            ,       tCIDLib::TCard4&        c4Ofs
            ,       tCIDLib::TCard4&        c4Len
            , const tCIDLib::TBoolean       bOnlyAtStart = kCIDLib::True
            , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::False
        )   const;

        tCIDLib::TBoolean bFindMatchAt
        (
            const   TString&                strFindIn
            ,       tCIDLib::TCard4&        c4Ofs
            ,       tCIDLib::TCard4&        c4Len
            , const tCIDLib::TBoolean       bOnlyAtStart = kCIDLib::True
            , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::False
        )   const;

        tCIDLib::TBoolean bFindMatchAt
        (
            const   tCIDLib::TCh* const     pszFindIn
            ,       tCIDLib::TCard4&        c4Ofs
            ,       tCIDLib::TCard4&        c4Len
            , const tCIDLib::TBoolean       bOnlyAtStart = kCIDLib::True
            , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::False
        )   const;

        tCIDLib::TBoolean bFullyMatches
        (
            const   TString&                strToTest
            , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::False
        )   const;

        tCIDLib::TBoolean bFullyMatches
        (
            const   tCIDLib::TCh* const     pszToTest
            , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::False
        )   const;

        tCIDLib::TBoolean bIsNullable() const;

        tCIDLib::TBoolean bReplaceAll
        (
                    TString&                strFindIn
            , const TString&                strRepWith
            , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::False
        )   const;

        TString strExpression() const;

        tCIDLib::TVoid SetExpression
        (
            const   TString&                strExpression
        );

        tCIDLib::TVoid SetExpression
        (
            const   tCIDLib::TCh* const     pszExpression
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatTo
        (
                    TTextOutStream&         strmDest
        )   const final;


    private :
        // --------------------------------------------------------------------
        //  Private, non-virtual methods
        // --------------------------------------------------------------------
        tCIDLib::TBoolean bEndOfPattern() const;

        tCIDLib::TCard4 c4ParseExpr();

        tCIDLib::TCard4 c4ParseFactor();

        tCIDLib::TCard4 c4ParseTerm();

        tCIDLib::TCh chPeekNext();

        tCIDLib::TCh chNext();

        TRXMatcher* pmatchParseCharRange();


        // --------------------------------------------------------------------
        //  Private data members
        //
        //  m_pszPattern
        //      A copy of the pattern string that is currently set up as our
        //      NFA. If it has not been set yet, then its still a null pointer.
        //
        //  m_prxnfaPattern
        //      The NFA that contains the compiled pattern built up from parsing the
        //      expression that gets set on us.
        // --------------------------------------------------------------------
        tCIDLib::TCh*           m_pszPattern;
        TRegExNFA*              m_prxnfaPattern;

        // Only used during parsing of pattern
        tCIDLib::TBoolean       m_bEscaped;
        tCIDLib::TBoolean       m_bLetter;
        tCIDLib::TCard4         m_c4CurInd;
        tCIDLib::TCard4         m_c4CurState;
        tCIDLib::TCard4         m_c4PatLen;


        // --------------------------------------------------------------------
        //  Magic macros
        // --------------------------------------------------------------------
        RTTIDefs(TRegEx, TObject)
};

#pragma CIDLIB_POPPACK

