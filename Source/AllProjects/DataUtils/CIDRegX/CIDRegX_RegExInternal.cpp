//
// FILE NAME: CIDRegX_RegExInternal.cpp
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
//  This file implements a number of derivatives of the TRXMatcher class.
//  These implement the match operations supported by the regular expression
//  engine. The outside world only needs to see the abstract base class so
//  these are provided internally only.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include "CIDRegX_.hpp"
#include "CIDRegX_RegExInternal_.hpp"



// ---------------------------------------------------------------------------
//  RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TRXCharMatcher,TRXMatcher)
RTTIDecls(TRXAnyCharMatcher,TRXMatcher)
RTTIDecls(TRXRangeMatcher,TRXMatcher)
RTTIDecls(TRXPosMatcher,TRXMatcher)




// ----------------------------------------------------------------------------
//   CLASS: TRXCharMatcher
//  PREFIX: match
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//  TRXCharMatcher: Constructors and Destructor
// ----------------------------------------------------------------------------
TRXCharMatcher::TRXCharMatcher( const   tCIDLib::TCh        chToMatch
                                , const tCIDLib::TBoolean   bNot) :
    m_bNot(bNot)
    , m_chToMatch(chToMatch)
{
}

TRXCharMatcher::~TRXCharMatcher()
{
}


// ----------------------------------------------------------------------------
//  TRXCharMatcher: Public, inherited methods
// ----------------------------------------------------------------------------
tCIDLib::TBoolean
TRXCharMatcher::bMatches(const  tCIDLib::TCh        chToMatch
                        , const tCIDLib::TCard4
                        , const tCIDLib::TCard4
                        , const tCIDLib::TBoolean   bCaseSensitive) const
{
    //
    //  If it matches, return the negation of the not flag. Else
    //  return the not flag.
    //
    if (bCaseSensitive)
    {
        if (chToMatch == m_chToMatch)
            return !m_bNot;
    }
     else
    {
        if (TRawStr::chUpper(chToMatch) == TRawStr::chUpper(m_chToMatch))
            return !m_bNot;
    }
    return m_bNot;
}

tCIDLib::TVoid TRXCharMatcher::SetNot()
{
    m_bNot = kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TRXCharMatcher: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TRXCharMatcher::FormatTo(TTextOutStream& strmDest) const
{
    strmDest << TString(m_chToMatch);
    if (m_bNot)
        strmDest << facCIDRegX().strMsg(kRegXMsgs::midRegEx_NOT);
}



// ----------------------------------------------------------------------------
//   CLASS: TRXAnyCharMatcher
//  PREFIX: match
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//  TRXAnyCharMatcher: Constructors and Destructor
// ----------------------------------------------------------------------------
TRXAnyCharMatcher::TRXAnyCharMatcher()
{
}

TRXAnyCharMatcher::~TRXAnyCharMatcher()
{
}

// ----------------------------------------------------------------------------
//  TRXAnyCharMatcher: Public, inherited methods
// ----------------------------------------------------------------------------
tCIDLib::TBoolean
TRXAnyCharMatcher::bMatches(const   tCIDLib::TCh        chToMatch
                            , const tCIDLib::TCard4
                            , const tCIDLib::TCard4
                            , const tCIDLib::TBoolean   ) const
{
    // Don't match an end of string marker
    if (!chToMatch)
        return kCIDLib::False;

    return kCIDLib::True;
}


// ----------------------------------------------------------------------------
//  TRXAnyCharMatcher: Protected, inherited methods
// ----------------------------------------------------------------------------
tCIDLib::TVoid TRXAnyCharMatcher::FormatTo(TTextOutStream& strmDest) const
{
    strmDest << facCIDRegX().strMsg(kRegXMsgs::midRegEx_AnyChar);
}



// ----------------------------------------------------------------------------
//   CLASS: TRXRangeMatcher
//  PREFIX: match
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//  TRXRangeMatcher: Constructors and Destructor
// ----------------------------------------------------------------------------
TRXRangeMatcher::TRXRangeMatcher(const tCIDLib::TBoolean bNot) :
    m_bNot(bNot)
    , m_pnodeHead(0)
{
}

TRXRangeMatcher::~TRXRangeMatcher()
{
    TCharRngNode* pnodeCur = m_pnodeHead;
    while (pnodeCur)
    {
        TCharRngNode* pnodeOld = pnodeCur;
        pnodeCur = pnodeCur->pnodeNext;
        delete pnodeOld;
    }
}


// ----------------------------------------------------------------------------
//  TRXRangeMatcher: Public, inherited methods
// ----------------------------------------------------------------------------
tCIDLib::TBoolean
TRXRangeMatcher::bMatches(  const   tCIDLib::TCh        chToMatch
                            , const tCIDLib::TCard4
                            , const tCIDLib::TCard4
                            , const tCIDLib::TBoolean   ) const
{
    //
    //  Go through our list of ranges and see if the character
    //  falls into any of them.
    //
    TCharRngNode* pnodeCur = m_pnodeHead;
    while (pnodeCur)
    {
        if ((chToMatch >= pnodeCur->chFirst)
        &&  (chToMatch <= pnodeCur->chSecond))
        {
            //
            //  It matched, so return the negation of the not
            //  flag. This way, if the not flag is on, we return
            //  false. Else we return true.
            //
            return !m_bNot;
        }
        pnodeCur = pnodeCur->pnodeNext;
    }

    //
    //  If we got here, then we didn't find the character in any
    //  of our ranges. If we are a 'not' mode, then return True
    //  here, else return False. This basically means we can just
    //  return the 'not' flag.
    //
    return m_bNot;
}


// ----------------------------------------------------------------------------
//  TRXRangeMatcher: Public, non-virtual methods
// ----------------------------------------------------------------------------
tCIDLib::TVoid
TRXRangeMatcher::AddRange(  const   tCIDLib::TCh    chFirst
                            , const tCIDLib::TCh    chSecond)
{
    // Create a new range structure for this range
    TCharRngNode* pnodeNew = new TCharRngNode;
    pnodeNew->chFirst = chFirst;
    pnodeNew->chSecond = chSecond;

    // Patch the new node in at the head
    pnodeNew->pnodeNext = m_pnodeHead;
    m_pnodeHead = pnodeNew;
}

tCIDLib::TVoid TRXRangeMatcher::SetNot()
{
    m_bNot = kCIDLib::True;
}


// ----------------------------------------------------------------------------
//  TRXRangeMatcher: Protected, inherited methods
// ----------------------------------------------------------------------------
tCIDLib::TVoid TRXRangeMatcher::FormatTo(TTextOutStream& strmDest) const
{
    TCharRngNode* pnodeCur = m_pnodeHead;
    while (pnodeCur)
    {
        if (pnodeCur->chSecond)
        {
            strmDest << TString(pnodeCur->chFirst)
                     << L"-" << TString(pnodeCur->chSecond);
        }
         else
        {
            strmDest << TString(pnodeCur->chFirst);
        }
        pnodeCur = pnodeCur->pnodeNext;

        if (pnodeCur)
            strmDest << L" ";
    }

    if (m_bNot)
        strmDest << L" " << facCIDRegX().strMsg(kRegXMsgs::midRegEx_NOT);
}



// ----------------------------------------------------------------------------
//   CLASS: TRXPosMatcher
//  PREFIX: match
// ----------------------------------------------------------------------------

// --------------------------------------------------------------------
//  TRXPosMatcher: Constructors and Destructor
// --------------------------------------------------------------------
TRXPosMatcher::TRXPosMatcher(const tCIDLib::TBoolean bAtStart) :

    m_bAtStart(bAtStart)
{
}

TRXPosMatcher::~TRXPosMatcher()
{
}


// --------------------------------------------------------------------
//  TRXPosMatcher: Public, inherited methods
// --------------------------------------------------------------------
tCIDLib::TBoolean
TRXPosMatcher::bMatches(const   tCIDLib::TCh
                        , const tCIDLib::TCard4     c4CurOfs
                        , const tCIDLib::TCard4     c4SearchLen
                        , const tCIDLib::TBoolean   ) const
{
    if (m_bAtStart)
        return (c4CurOfs == 0);
    return (c4CurOfs == c4SearchLen);
}


// -------------------------------------------------------------------
//  TRXPosMatcher: Protected, inherited methods
// -------------------------------------------------------------------
tCIDLib::TVoid TRXPosMatcher::FormatTo(TTextOutStream& strmDest) const
{
    if (m_bAtStart)
        strmDest << facCIDRegX().strMsg(kRegXMsgs::midRegEx_AtStart);
    else
        strmDest << facCIDRegX().strMsg(kRegXMsgs::midRegEx_AtEnd);
}
