//
// FILE NAME: CIDRegX_RegExInternal.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/28/1998
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This internal header provides a number of derivatives of the TRXCharMatcher
//  class from CIDRegX_RegExNFA.hpp. These are only needed internally so there
//  is no need to pollute the outside world with them.
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
//   CLASS: TRXCharMatcher
//  PREFIX: match
// ----------------------------------------------------------------------------
class TRXCharMatcher : public TRXMatcher
{
    public :
        // --------------------------------------------------------------------
        //  Constructors and Destructor
        // --------------------------------------------------------------------
        TRXCharMatcher
        (
            const   tCIDLib::TCh            chToMatch
            , const tCIDLib::TBoolean       bNot = kCIDLib::False
        );

        ~TRXCharMatcher();


        // --------------------------------------------------------------------
        //  Public, inherited methods
        // --------------------------------------------------------------------
        tCIDLib::TBoolean bMatches
        (
            const   tCIDLib::TCh            chToMatch
            , const tCIDLib::TCard4         c4CurOfs
            , const tCIDLib::TCard4         c4SearchLen
            , const tCIDLib::TBoolean       bCaseSensitive
        )   const;


        // --------------------------------------------------------------------
        //  Public, non-virtual methods
        // --------------------------------------------------------------------
        tCIDLib::TVoid SetNot();


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatTo(TTextOutStream& strmDest) const;


    private :
        // --------------------------------------------------------------------
        //  Private data members
        //
        //  m_bNot
        //      This flag is used to flip the meaning, so that this matcher
        //      matches anything but the character.
        //
        //  m_chToMatch
        //      This is the character we are to match
        // --------------------------------------------------------------------
        tCIDLib::TBoolean   m_bNot;
        tCIDLib::TCh        m_chToMatch;


        // --------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // --------------------------------------------------------------------
        TRXCharMatcher(const TRXCharMatcher&);
        tCIDLib::TVoid operator=(const TRXCharMatcher&);

        // --------------------------------------------------------------------
        //  Magic macros
        // --------------------------------------------------------------------
        RTTIDefs(TRXCharMatcher,TRXMatcher)
};


// ----------------------------------------------------------------------------
//   CLASS: TRXAnyCharMatcher
//  PREFIX: match
// ----------------------------------------------------------------------------
class TRXAnyCharMatcher : public TRXMatcher
{
    public :
        // --------------------------------------------------------------------
        //  Constructors and Destructor
        // --------------------------------------------------------------------
        TRXAnyCharMatcher();

        ~TRXAnyCharMatcher();


        // --------------------------------------------------------------------
        //  Public, inherited methods
        // --------------------------------------------------------------------
        tCIDLib::TBoolean bMatches
        (
            const   tCIDLib::TCh            chToMatch
            , const tCIDLib::TCard4         c4CurOfs
            , const tCIDLib::TCard4         c4SearchLen
            , const tCIDLib::TBoolean       bCaseSensitive
        )   const;


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatTo(TTextOutStream& strmDest) const;


    private :
        // --------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // --------------------------------------------------------------------
        TRXAnyCharMatcher(const TRXAnyCharMatcher&);
        tCIDLib::TVoid operator=(const TRXAnyCharMatcher&);


        // --------------------------------------------------------------------
        //  Magic macros
        // --------------------------------------------------------------------
        RTTIDefs(TRXAnyCharMatcher,TRXMatcher)
};


// ----------------------------------------------------------------------------
//   CLASS: TRXRangeMatcher
//  PREFIX: match
// ----------------------------------------------------------------------------
class TRXRangeMatcher : public TRXMatcher
{
    public :
        // --------------------------------------------------------------------
        //  Constructors and Destructor
        // --------------------------------------------------------------------
        TRXRangeMatcher(const tCIDLib::TBoolean bNot = kCIDLib::False);

        ~TRXRangeMatcher();


        // --------------------------------------------------------------------
        //  Public, inherited methods
        // --------------------------------------------------------------------
        tCIDLib::TBoolean bMatches
        (
            const   tCIDLib::TCh            chToMatch
            , const tCIDLib::TCard4         c4CurOfs
            , const tCIDLib::TCard4         c4SearchLen
            , const tCIDLib::TBoolean       bCaseSensitive
        )   const;


        // --------------------------------------------------------------------
        //  Public, non-virtual methods
        // --------------------------------------------------------------------
        tCIDLib::TVoid AddRange
        (
            const   tCIDLib::TCh            chFirst
            , const tCIDLib::TCh            chSecond
        );

        tCIDLib::TVoid SetNot();


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatTo(TTextOutStream& strmDest) const;


    private :
        // --------------------------------------------------------------------
        //  Nested types
        // --------------------------------------------------------------------
        struct TCharRngNode
        {
            tCIDLib::TCh    chFirst;
            tCIDLib::TCh    chSecond;
            TCharRngNode*   pnodeNext;
        };


        // --------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // --------------------------------------------------------------------
        TRXRangeMatcher(const TRXRangeMatcher&);
        tCIDLib::TVoid operator=(const TRXRangeMatcher&);


        // --------------------------------------------------------------------
        //  Private, non-virtual methods
        //
        //  m_bNot
        //      Indicates whether this range is a 'not' range. If so, then
        //      the char matches only if not in any range.
        //
        //  m_pnodeHead
        //      The head of a linked list of character ranges. Each node is
        //      of the TCharRngNode structure (a nested type of this class.)
        // --------------------------------------------------------------------
        tCIDLib::TBoolean   m_bNot;
        TCharRngNode*       m_pnodeHead;


        // --------------------------------------------------------------------
        //  Magic macros
        // --------------------------------------------------------------------
        RTTIDefs(TRXRangeMatcher,TRXMatcher)
};


// ----------------------------------------------------------------------------
//   CLASS: TRXPosMatcher
//  PREFIX: match
// ----------------------------------------------------------------------------
class TRXPosMatcher : public TRXMatcher
{
    public :
        // --------------------------------------------------------------------
        //  Constructors and Destructor
        // --------------------------------------------------------------------
        TRXPosMatcher(const tCIDLib::TBoolean bAtStart);

        ~TRXPosMatcher();


        // --------------------------------------------------------------------
        //  Public, inherited methods
        // --------------------------------------------------------------------
        tCIDLib::TBoolean bMatches
        (
            const   tCIDLib::TCh            chToMatch
            , const tCIDLib::TCard4         c4CurOfs
            , const tCIDLib::TCard4         c4SearchLen
            , const tCIDLib::TBoolean       bCaseSensitive
        )   const;


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatTo(TTextOutStream& strmDest) const;


    private :
        // --------------------------------------------------------------------
        //  Private data members
        //
        //  m_bAtStart
        //      This flag is used to indicate whether we match at the start
        //      or at the end.
        // --------------------------------------------------------------------
        tCIDLib::TBoolean   m_bAtStart;


        // --------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // --------------------------------------------------------------------
        TRXPosMatcher(const TRXPosMatcher&);
        tCIDLib::TVoid operator=(const TRXPosMatcher&);


        // --------------------------------------------------------------------
        //  Magic macros
        // --------------------------------------------------------------------
        RTTIDefs(TRXPosMatcher,TRXMatcher)
};


#pragma CIDLIB_POPPACK


