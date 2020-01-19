//
// FILE NAME: CIDLib_RegExEngine.cpp
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
//  This file implements the TRegEx class.
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
RTTIDecls(TRegEx,TObject)


namespace CIDRegX_Engine
{
    namespace
    {
        // -----------------------------------------------------------------------
        //  Local, const data
        //
        //  c4Scan
        //      This is a special value that is put into the deque while running
        //      the DFA. It separates the states for the current character and
        //      those for the next character.
        // -----------------------------------------------------------------------
        const tCIDLib::TCard4    c4Scan = kCIDLib::c4MaxCard;
    }
}



// ---------------------------------------------------------------------------
//   CLASS: TRegEx
//  PREFIX: regx
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TRegEx: Constructors and Destructor
// ---------------------------------------------------------------------------
TRegEx::TRegEx() :

    m_bEscaped(kCIDLib::False)
    , m_bLetter(kCIDLib::False)
    , m_c4CurInd(0)
    , m_c4CurState(0)
    , m_c4PatLen(0)
    , m_prxnfaPattern(0)
    , m_pszPattern(0)
{
}

TRegEx::TRegEx(const TString& strExpression) :

    m_bEscaped(kCIDLib::False)
    , m_bLetter(kCIDLib::False)
    , m_c4CurInd(0)
    , m_c4CurState(0)
    , m_c4PatLen(0)
    , m_prxnfaPattern(0)
    , m_pszPattern(0)
{
    SetExpression(strExpression.pszBuffer());
}

TRegEx::TRegEx(const tCIDLib::TCh* const pszExpression) :

    m_bEscaped(kCIDLib::False)
    , m_bLetter(kCIDLib::False)
    , m_c4CurInd(0)
    , m_c4CurState(0)
    , m_c4PatLen(0)
    , m_prxnfaPattern(0)
    , m_pszPattern(0)
{
    SetExpression(pszExpression);
}

TRegEx::~TRegEx()
{
    delete [] m_pszPattern;
    delete m_prxnfaPattern;
}


// ---------------------------------------------------------------------------
//  TRegEx: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TRegEx::bFindMatch( const   TString&            strFindIn
                    ,       tCIDLib::TCard4&    c4Ofs
                    ,       tCIDLib::TCard4&    c4Len
                    , const tCIDLib::TBoolean   bOnlyAtStart
                    , const tCIDLib::TBoolean   bCaseSensitive) const
{
    // Just call the other version with the raw string and start of 0
    c4Ofs  = 0;
    return bFindMatchAt
    (
        strFindIn.pszBuffer()
        , c4Ofs
        , c4Len
        , bOnlyAtStart
        , bCaseSensitive
    );
}

tCIDLib::TBoolean
TRegEx::bFindMatch( const   tCIDLib::TCh* const pszFindIn
                    ,       tCIDLib::TCard4&    c4Ofs
                    ,       tCIDLib::TCard4&    c4Len
                    , const tCIDLib::TBoolean   bOnlyAtStart
                    , const tCIDLib::TBoolean   bCaseSensitive) const
{
    // Just call the other version with a start of 0
    c4Ofs = 0;
    return bFindMatchAt(pszFindIn, c4Ofs, c4Len, bOnlyAtStart, bCaseSensitive);
}

tCIDLib::TBoolean
TRegEx::bFindMatchAt(const  TString&            strFindIn
                    ,       tCIDLib::TCard4&    c4Ofs
                    ,       tCIDLib::TCard4&    c4Len
                    , const tCIDLib::TBoolean   bOnlyAtStart
                    , const tCIDLib::TBoolean   bCaseSensitive) const
{
    // Just call the other version with the raw string
    return bFindMatchAt
    (
        strFindIn.pszBuffer()
        , c4Ofs
        , c4Len
        , bOnlyAtStart
        , bCaseSensitive
    );
}

tCIDLib::TBoolean
TRegEx::bFindMatchAt(const  tCIDLib::TCh* const pszFindIn
                    ,       tCIDLib::TCard4&    c4Ofs
                    ,       tCIDLib::TCard4&    c4Len
                    , const tCIDLib::TBoolean   bOnlyAtStart
                    , const tCIDLib::TBoolean   bCaseSensitive) const
{
    // Check for not having set up the expression yet
    if (!m_prxnfaPattern)
    {
        facCIDRegX().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kRegXErrs::errcRegEx_NoPattern
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
        );
    }

    // Get the length of the string to search, and check for pathological case
    const tCIDLib::TCard4 c4SearchLen = TRawStr::c4StrLen(pszFindIn);
    if (!c4SearchLen)
        return kCIDLib::False;

    //
    //  Get the start at index, so that we don't have to worry about using
    //  it after overwriting it.
    //
    const tCIDLib::TCard4 c4StartInd = c4Ofs;

    // Make sure the start index is not longer than the string
    if (c4StartInd >= c4SearchLen)
    {
        facCIDRegX().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kRegXErrs::errcRegEx_BadStartAt
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4StartInd)
            , TCardinal(c4SearchLen)
        );
    }

    //
    //  Create a fundamental deque with enough states to hold the worst
    //  case scenario for our pattern.
    //
    TFundDeque<tCIDLib::TCard4> fcolStates
    (
        m_prxnfaPattern->c4StateCount() * c4SearchLen
    );

    //
    //  Each time we find a path to the end of the pattern, we save the
    //  current position here and keep going to see if we can find a longer
    //  match. We set it to the max as a 'no match' flag.
    //
    tCIDLib::TCard4 c4LastMatch = kCIDLib::c4MaxCard;

    //
    //  Lets set up the outer loop first. This guy handles moving up through
    //  the search string when we fail to get matches. It will keep going
    //  until it hits the end of the string.
    //
    //  If the bOnlyAtStart flag is set, then it will limit the iterations
    //  to one, which forces it to check for a match at the starting position.
    //
    const tCIDLib::TCard4 c4Iters = bOnlyAtStart ? 1 : c4SearchLen - c4StartInd;
    for (tCIDLib::TCard4 c4CurIter = 0; c4CurIter < c4Iters; c4CurIter++)
    {
        // Set up the deque for the next run
        fcolStates.RemoveAll();
        fcolStates.AddAtBack(CIDRegX_Engine::c4Scan);

        // Set the starting position for this round
        tCIDLib::TCard4 c4CurInd = c4StartInd + c4CurIter;

        // Get the initial state from the NFA
        tCIDLib::TCard4 c4CurState = m_prxnfaPattern->c4State1At(0);

        // And now enter the loop that runs the NFA
        while (c4CurState)
        {
            if (c4CurState == CIDRegX_Engine::c4Scan)
            {
                if (c4CurInd > c4SearchLen)
                    break;

                c4CurInd++;
                fcolStates.AddAtBack(CIDRegX_Engine::c4Scan);
            }
             else
            {
                const tCIDLib::TCard4 c41 = m_prxnfaPattern->c4State1At(c4CurState);
                const tCIDLib::TCard4 c42 = m_prxnfaPattern->c4State2At(c4CurState);

                if (m_prxnfaPattern->bIsEpsilonState(c4CurState))
                {
                    //
                    //  If either of them is zero, then remember where we
                    //  are now, but don't add the zero state.
                    //
                    fcolStates.bAddAtFrontIfNotAlready(c41);
                    if (c42 != c41)
                        fcolStates.bAddAtFrontIfNotAlready(c42);
                }
                 else
                {
                    #if CID_DEBUG_ON
                    if (c4CurInd > c4SearchLen)
                    {
                        facCIDRegX().ThrowErr
                        (
                            CID_FILE
                            , CID_LINE
                            , kRegXErrs::errcRegEx_PatternOverflow
                            , tCIDLib::ESeverities::ProcFatal
                            , tCIDLib::EErrClasses::Internal
                        );
                    }
                    #endif

                    if (m_prxnfaPattern->matchAt(c4CurState).bMatches
                    (
                        pszFindIn[c4CurInd]
                        , c4CurInd
                        , c4SearchLen
                        , bCaseSensitive))
                    {
                        fcolStates.bAddAtBackIfNotAlready(c41);
                        if (c41 != c42)
                            fcolStates.bAddAtBackIfNotAlready(c42);
                    }
                }
            }

            // Get the next state to work on
            c4CurState = fcolStates.tGetFromFront();

            //
            //  If its zero, then remember our current index as the last match
            //  and get another one off. This lets us keep going to try to
            //  find a longer match, but remember that we did get a full match.
            //
            //  However, if we are already past the end of the string, then
            //  stop now.
            //
            if (!c4CurState)
            {
                // Save current pos as the last match
                c4LastMatch = c4CurInd;

                // If we are at the end of the string, don't bother continuing
                if (c4CurInd >= c4SearchLen)
                    break;

                // Otherwise, get another state and go on
                c4CurState = fcolStates.tGetFromFront();
            }

            //
            //  If the stack is empty, then we have completed the whole
            //  NFA, so break out.
            //
            if (fcolStates.bIsEmpty())
                break;
        }

        //
        //  If we got some match along the way, then we made it through
        //  the whole pattern via some path. So we consider that a success.
        //
        if (c4LastMatch != kCIDLib::c4MaxCard)
        {
            //
            //  Note that the search len could go beyond the lenght of the
            //  search path if the $ (match at end) is used, so adjust it
            //  back before doing calculations.
            //
            if (c4LastMatch > c4SearchLen)
                c4LastMatch = c4SearchLen;

            c4Ofs = c4StartInd + c4CurIter;
            c4Len = c4LastMatch - c4Ofs;

            // Watch for a zero length, which can happen
            if (c4Len)
                return kCIDLib::True;
        }
    }

    // We did not find a match
    return kCIDLib::False;
}


tCIDLib::TBoolean
TRegEx::bFullyMatches(  const   TString&            strToTest
                        , const tCIDLib::TBoolean   bCaseSensitive) const
{
    // Just call the other version with the raw string
    return bFullyMatches(strToTest.pszBuffer(), bCaseSensitive);
}


tCIDLib::TBoolean
TRegEx::bFullyMatches(  const   tCIDLib::TCh* const pszToTest
                        , const tCIDLib::TBoolean   bCaseSensitive) const
{
    // Check for not having set up the expression yet
    if (!m_prxnfaPattern)
    {
        facCIDRegX().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kRegXErrs::errcRegEx_NoPattern
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
        );

        // Won't happen, but makes analyzer happy
        return kCIDLib::False;
    }

    // Get the length of the string to search
    const tCIDLib::TCard4 c4SearchLen = TRawStr::c4StrLen(pszToTest);

    //
    //  If the input is zero length, see if the expression is nullable. If not, then
    //  we know it failed and this simplifies things below.
    //
    if (!c4SearchLen && !m_prxnfaPattern->bIsNullable())
        return kCIDLib::False;

    //
    //  Create a fundamental deque with enough states to hold the worst
    //  case scenario for our pattern.
    //
    TFundDeque<tCIDLib::TCard4> fcolStates
    (
        m_prxnfaPattern->c4StateCount() * (c4SearchLen + 1)
    );
    fcolStates.AddAtBack(CIDRegX_Engine::c4Scan);

    tCIDLib::TBoolean   bSuccess = kCIDLib::False;
    tCIDLib::TCard4     c4CurInd = 0;
    tCIDLib::TCard4     c4CurState = m_prxnfaPattern->c4State1At(0);
    while (c4CurState)
    {
        if (c4CurState == CIDRegX_Engine::c4Scan)
        {
            // We passed the end of the string before completing the pattern
            if (c4CurInd > c4SearchLen)
                return kCIDLib::False;

            c4CurInd++;
            fcolStates.AddAtBack(CIDRegX_Engine::c4Scan);
        }
         else
        {
            const tCIDLib::TCard4 c41 = m_prxnfaPattern->c4State1At(c4CurState);
            const tCIDLib::TCard4 c42 = m_prxnfaPattern->c4State2At(c4CurState);

            if (m_prxnfaPattern->bIsEpsilonState(c4CurState))
            {
                fcolStates.bAddAtFrontIfNotAlready(c41);
                if (c42 != c41)
                    fcolStates.bAddAtFrontIfNotAlready(c42);
            }
             else
            {
                #if CID_DEBUG_ON
                if (c4CurInd > c4SearchLen)
                {
                    facCIDRegX().ThrowErr
                    (
                        CID_FILE
                        , CID_LINE
                        , kRegXErrs::errcRegEx_PatternOverflow
                        , tCIDLib::ESeverities::ProcFatal
                        , tCIDLib::EErrClasses::Internal
                    );
                }
                #endif

                if (m_prxnfaPattern->matchAt(c4CurState).bMatches
                (
                    pszToTest[c4CurInd]
                    , c4CurInd
                    , c4SearchLen
                    , bCaseSensitive))
                {
                    fcolStates.bAddAtBackIfNotAlready(c41);
                }
            }
        }

        // If no more states to process break out
        if (fcolStates.bIsEmpty())
            break;

        // Get the next state from the deque to process
        c4CurState = fcolStates.tGetFromFront();

        //
        //  If its zero, then we have gone through the whole pattern via
        //  some path. If we are at the end of the pattern, then break out
        //  now. Otherwise, try to keep going to see if we can make it
        //  to the end.
        //
        if (!c4CurState)
        {
            if (c4CurInd >= c4SearchLen)
            {
                bSuccess = kCIDLib::True;
                break;
            }
            c4CurState = fcolStates.tGetFromFront();
        }
    }
    return bSuccess;
}


// Just a convenience to replace all sub-strings that match
tCIDLib::TBoolean
TRegEx::bReplaceAll(        TString&            strFindIn
                    , const TString&            strRepWith
                    , const tCIDLib::TBoolean   bCaseSensitive) const
{
    tCIDLib::TBoolean bChanges = kCIDLib::False;

    tCIDLib::TCard4 c4At = 0;
    tCIDLib::TCard4 c4Len;
    while (c4At < strFindIn.c4Length())
    {
        const tCIDLib::TBoolean bMatch = bFindMatchAt
        (
            strFindIn
            , c4At
            , c4Len
            , kCIDLib::False
            , bCaseSensitive
        );

        //
        //  If we found one, replace that part and then move
        //  forward past that.
        //
        if (bMatch)
        {
            bChanges = kCIDLib::True;
            strFindIn.Cut(c4At, c4Len);
            strFindIn.Insert(strRepWith, c4At);
            c4At += strRepWith.c4Length();
        }
         else
        {
            break;
        }
    }
    return bChanges;
}


TString TRegEx::strExpression() const
{
    if (m_pszPattern)
        return TString(m_pszPattern);
    return TString::strEmpty();
}


// Set the expression to use in matching
tCIDLib::TVoid TRegEx::SetExpression(const TString& strExpression)
{
    // Just call the other version with the raw string
    SetExpression(strExpression.pszBuffer());
}

tCIDLib::TVoid TRegEx::SetExpression(const tCIDLib::TCh* const pszExpression)
{
    //
    //  Get the maximum number of entries that this new expression could
    //  possibly require. The worst case is that every char is a transition
    //  character (i.e. something like "abc", and we need two states per
    //  transition.) And we need an extra 0th state to kick start with and
    //  an extra last state.
    //
    const tCIDLib::TCard4 c4NewLen = TRawStr::c4StrLen(pszExpression);
    const tCIDLib::TCard4 c4NewEntries = (c4NewLen * 2) + 2;

    // Make sure the new expression is not empty
    if (!c4NewLen)
    {
        facCIDRegX().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kRegXErrs::errcRegEx_EmptyPattern
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
        );
    }

    // Get our private copy of the raw pattern and its length
    m_c4PatLen = c4NewLen;
    m_pszPattern = TRawStr::pszReplicate(pszExpression);

    //
    //  If we have not created our NFA object yet, then create it. If
    //  we have, then reset its maximum entries.
    //
    if (!m_prxnfaPattern)
        m_prxnfaPattern = new TRegExNFA(c4NewEntries);
     else
        m_prxnfaPattern->c4Reset(c4NewEntries);

    //
    //  Zero the current index into the pattern. Do a peek in order to
    //  initialize the input character state flags for the first character.
    //
    m_c4CurInd = 0;
    chPeekNext();

    //
    //  And now lets parse the new expression and create the state info
    //  for it. We kick off the recursive parsing mechanism here, which
    //  fills in the state and match arrays for us. We add one state first
    //  to leave room for the start state.
    //
    m_prxnfaPattern->c4AddState();
    const tCIDLib::TCard4 c4Start = c4ParseExpr();

    if (m_c4CurInd != m_c4PatLen)
    {
        facCIDRegX().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kRegXErrs::errcRegEx_ExpectedExpr
            , TString(m_pszPattern)
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TCardinal(m_c4CurInd)
        );
    }

    // Set the 0th state to point to the start state
    m_prxnfaPattern->UpdateStateAt(0, c4Start, c4Start);

    // And add a last state which is the end state
    m_prxnfaPattern->c4AddState(0, 0, 0);

    // And call complete on it
    m_prxnfaPattern->Complete();
}


// ---------------------------------------------------------------------------
//  TRegEx: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TRegEx::FormatTo(TTextOutStream& strmDest) const
{
    // If we don't have a pattern, then just say so
    if (!m_prxnfaPattern)
    {
        strmDest << facCIDRegX().strMsg(kRegXMsgs::midRegEx_NoPattern);
        return;
    }

    // Display the pattern and the NFA we created from it
    strmDest << facCIDRegX().strMsg(kRegXMsgs::midGen_Pattern) << L": "
             << m_pszPattern << kCIDLib::NewLn
             << facCIDRegX().strMsg(kRegXMsgs::midRegEx_NFA) << kCIDLib::NewLn
             << *m_prxnfaPattern;
}


// ---------------------------------------------------------------------------
//  TRegEx: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TRegEx::bEndOfPattern() const
{
    return (m_c4CurInd >= m_c4PatLen);
}


tCIDLib::TCard4 TRegEx::c4ParseExpr()
{
    //
    //  Parse the opening term, which starts any expression. We get back
    //  the index of its first state. We provisionally set it as the
    //  return value.
    //
    const tCIDLib::TCard4 c4State1 = c4ParseTerm();
    tCIDLib::TCard4 c4Ret = c4State1;

    //
    //  If the next char is an OR symbol then lets deal with the trailing
    //  expression.
    //
    if (!m_bEscaped && (chPeekNext() == kCIDLib::chVerticalBar))
    {
        // Eat the OR symbol
        chNext();

        //
        //  Make room for the next two states, but we can't fill them in
        //  until we've parsed the other expression. The second of these
        //  new states becomes our return value.
        //
        m_prxnfaPattern->c4AddState();
        c4Ret = m_prxnfaPattern->c4AddState();

        //
        //  Update the second state to do the 'forward or back' state for
        //  the OR of the previous term and the newly parsed one.
        //
        m_prxnfaPattern->UpdateStateAt(c4Ret, c4ParseExpr(), c4State1);

        //
        //  And set the first of the states to point past the OR state
        //  we just set.
        //
        m_prxnfaPattern->PointPastEnd(c4Ret-1);
    }
    return c4Ret;
}


tCIDLib::TCard4 TRegEx::c4ParseFactor()
{
    tCIDLib::TCard4 c4State1, c4State2;
    tCIDLib::TCard4 c4Ret(0);

    // Remember the next available state as our first state
    c4State1 = m_prxnfaPattern->c4StateCount();

    //
    //  First, check for whether we have a letter. This will handle any
    //  possibly escaped special characters up front. If its not a letter,
    //  then we can check for all of the other special characters without
    //  worrying whether its an escaped special character.
    //
    const tCIDLib::TCh chTrigger = chPeekNext();
    if (m_bLetter)
    {
        // Its a single char so update the states and eat the char
        chNext();
        c4State2 = m_prxnfaPattern->c4AddStatePointedPastEnd
        (
            new TRXCharMatcher(chTrigger)
        );
    }
     else if (chTrigger == kCIDLib::chOpenParen)
    {
        // Remember the current offset and eat the paren
        const tCIDLib::TCard4 c4OldOfs = m_c4CurInd;
        chNext();

        //
        //  Make room for an intermediate state we'll fill in below, and it becomes
        //  our new state1 since this is what we want updated if there's any conditiona
        //  at the end.
        //
        const tCIDLib::TCard4 c4ExprStart = m_prxnfaPattern->c4AddState();

        // Our start state is now moved forward by one
        c4State1++;

        // Parse the expression
        c4State2 = c4ParseExpr();

        // And make sure we get the close paren
        if ((chPeekNext() != kCIDLib::chCloseParen) || m_bEscaped)
        {
            facCIDRegX().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kRegXErrs::errcRegEx_CloseParen
                , TString(m_pszPattern)
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::AppError
                , TCardinal(m_c4CurInd)
                , TCardinal(c4OldOfs)
            );
        }

        //
        //  Now we need to point the extra state we made room for at the
        //  entry state for this parenthesized expression.
        //
        m_prxnfaPattern->UpdateStateAt(c4ExprStart, c4State2, c4State2);

        // Eat the close paren
        chNext();
    }
     else if (chTrigger == kCIDLib::chOpenBracket)
    {
        chNext();
        c4State2 = m_prxnfaPattern->c4AddStatePointedPastEnd
        (
            pmatchParseCharRange()
        );
    }
     else if (chTrigger == kCIDLib::chDollarSign)
    {
        //
        //  This means a match at end. So we just put in a position matcher
        //  object, with a 'not at start' flag, which means 'at the end'.
        //
        chNext();
        c4State2 = m_prxnfaPattern->c4AddStatePointedPastEnd
        (
            new TRXPosMatcher(kCIDLib::False)
        );
    }
     else if (chTrigger == kCIDLib::chPeriod)
    {
        //
        //  This matches any single character, so eat the character and
        //  create a single char match object.
        //
        chNext();
        c4State2 = m_prxnfaPattern->c4AddStatePointedPastEnd
        (
            new TRXAnyCharMatcher()
        );
    }
     else
    {
        // Make compiler happy
        c4State2 = 0;

        facCIDRegX().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kRegXErrs::errcRegEx_ExpectedFactor
            , TString(m_pszPattern)
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TCardinal(m_c4CurInd)
        );
    }

    const tCIDLib::TCh chClosure = chPeekNext();
    if (((chClosure == kCIDLib::chAsterisk)
    ||   (chClosure == kCIDLib::chQuestionMark)
    ||   (chClosure == kCIDLib::chPlusSign))
    &&  !m_bEscaped)
    {
        //
        //  Its a closure symbol, so eat it and update the states for
        //  closure on the previous character or expression.
        //
        chNext();

        if (chClosure == kCIDLib::chAsterisk)
        {
            //
            //  Add a new state after the expression, which will be the
            //  epsilon node that lets us either go back through the pattern
            //  or move forward. This is the one we return as the starting
            //  state for this factor.
            //
            c4Ret = m_prxnfaPattern->c4AddStatePointedPastEnd();

            //
            //  Set one state of our epsilon node to point back to the
            //  start of the previous expression. The other points forward
            //  for the 'no match' state.
            //
            m_prxnfaPattern->UpdateState2At(c4Ret, c4State2);

            //
            //  Update the states of the state before our expression so that
            //  it points into our starting state.
            //
            m_prxnfaPattern->UpdateState1At(c4State1 - 1, c4Ret);
            m_prxnfaPattern->UpdateState2At(c4State1 - 1, c4Ret);
        }
         else if (chClosure == kCIDLib::chPlusSign)
        {
            //
            //  In this case, we want to return the second state as the
            //  target state, so that the pattern will have to be passed
            //  through at least once.
            //
            c4Ret = c4State2;

            //
            //  Now add a new epsilon state that let's us either go back
            //  through the pattern again, or to move forward to the next
            //  state. One state will point forward, and we'll set it's
            //  first state to point back to the state of the previous
            //  expression. Use state1 as a temp here.
            //
            c4State1 = m_prxnfaPattern->c4AddStatePointedPastEnd();
            m_prxnfaPattern->UpdateState1At(c4State1, c4State2);
        }
         else if (chClosure == kCIDLib::chQuestionMark)
        {
            //
            //  We need to add two new epsilon nodes at the end. The first
            //  one jumps over the second one. The second point either points
            //  forward or back to the start of the previous expression, but
            //  when we get to the end of the expression (if it's taken), we
            //  need to be sure that we don't loop back again, so we want to
            //  jump forward over the second epsilon node.
            //
            //  We return the second node as the entry to this expression.
            //
            const tCIDLib::TCard4 c4E1 = m_prxnfaPattern->c4AddStatePointedPastEnd();
            c4Ret = m_prxnfaPattern->c4AddStatePointedPastEnd();

            // Point both of the first to after the second
            m_prxnfaPattern->UpdateState1At(c4E1, c4Ret + 1);
            m_prxnfaPattern->UpdateState2At(c4E1, c4Ret + 1);

            // And point the second state of the entry state back to the start
            m_prxnfaPattern->UpdateState2At(c4Ret, c4State2);

            //
            //  Update the states of the state before our expression so that
            //  it points into our starting state.
            //
            m_prxnfaPattern->UpdateState1At(c4State1 - 1, c4Ret);
            m_prxnfaPattern->UpdateState2At(c4State1 - 1, c4Ret);
        }
    }
     else
    {
        c4Ret = c4State2;
    }
    return c4Ret;
}


tCIDLib::TCard4 TRegEx::c4ParseTerm()
{
    // Parse the factor, which starts any term
    const tCIDLib::TCard4 c4Ret = c4ParseFactor();

    const tCIDLib::TCh chTrigger = chPeekNext();

    //
    //  If its a letter, or an open paren or open bracket, parse the other
    //  term.
    //
    if (m_bLetter)
    {
        c4ParseTerm();
    }
     else if ((chTrigger == kCIDLib::chOpenParen)
          ||  (chTrigger == kCIDLib::chOpenBracket)
          ||  (chTrigger == kCIDLib::chPeriod)
          ||  (chTrigger == kCIDLib::chDollarSign))
    {
        c4ParseTerm();
    }
    return c4Ret;
}


tCIDLib::TCh TRegEx::chPeekNext()
{
    // Get the char at the current position
    tCIDLib::TCh chRet = m_pszPattern[m_c4CurInd];

    // Check special case of end of pattern
    if (chRet == kCIDLib::chNull)
    {
        m_bLetter = kCIDLib::False;
        m_bEscaped = kCIDLib::False;
        return chRet;
    }

    // Assume it will be a letter
    m_bLetter = kCIDLib::True;

    //
    //  Check for an escape sequence. This starts with a backslash and
    //  means that the next char is to be taken literally, not as part of
    //  any special meaning. The escape char is eaten and the next character
    //  is made the current character.
    //
    if (chRet == kCIDLib::chBackSlash)
    {
        //
        //  So we skip this character and get the next one, and set the
        //  m_bEscaped flag. Check first that it is not the last character
        //  in the pattern, which could not be legal since it could not be
        //  escaping anything.
        //
        if (m_c4CurInd+1 == m_c4PatLen)
        {
            facCIDRegX().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kRegXErrs::errcRegEx_EscapeAtEOP
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::AppError
            );
        }
        chRet = m_pszPattern[m_c4CurInd+1];
        m_bEscaped = kCIDLib::True;
    }
     else
    {
        m_bEscaped = kCIDLib::False;

        if ((chRet == kCIDLib::chAsterisk)
        ||  (chRet == kCIDLib::chBackSlash)
        ||  (chRet == kCIDLib::chCircumflex)
        ||  (chRet == kCIDLib::chCloseBracket)
        ||  (chRet == kCIDLib::chCloseParen)
        ||  (chRet == kCIDLib::chOpenBracket)
        ||  (chRet == kCIDLib::chOpenParen)
        ||  (chRet == kCIDLib::chPeriod)
        ||  (chRet == kCIDLib::chVerticalBar))
        {
            m_bLetter = kCIDLib::False;
        }

        //
        //  If its a $ and this is the last character, then it means
        //  something special (match at end), so we treat it like a non
        //  character in that one case.
        //
        if ((chRet == kCIDLib::chDollarSign) && (m_c4CurInd == m_c4PatLen-1))
        {
            m_bLetter = kCIDLib::False;
        }
    }
    return chRet;
}


tCIDLib::TCh TRegEx::chNext()
{
    // Can't do this if at the end of the pattern
    if (bEndOfPattern())
    {
        facCIDRegX().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kRegXErrs::errcRegEx_UnexpectedEOP
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::OutResource
        );
    }

    //
    //  Get out the current character for return. If its escaped we have to
    //  get the second character and bump the index by 2. Otherwise get the
    //  first character and bump the index by one.
    //
    tCIDLib::TCh chRet;
    if (m_bEscaped)
    {
        chRet =  m_pszPattern[m_c4CurInd+1];
        m_c4CurInd += 2;
    }
     else
    {
        chRet = m_pszPattern[m_c4CurInd];
        m_c4CurInd++;
    }

    //
    //  Call chPeekNext() in order to update the state flags to represent
    //  the state of the next character next character.
    //
    chPeekNext();

    return chRet;
}


//
//  This method is called to parse ranges, which are treated as atomic
//  states. They are in the form "[a-c]", which means anything between the
//  first and second char. Single char ranges can be used, e.g. "[a-cde]"
//  which means all chars between 'a' and 'c', plus 'd' and 'e'. The sequence
//  "[ ]" is treated the same as ".', i.e. a match of any char.
//
TRXMatcher* TRegEx::pmatchParseCharRange()
{
    //
    //  We use a little state machine here to parse the range. It can contain
    //  multiple characters and multiple ranges and it can be "not'ed" as
    //  well.
    //
    enum class EStates
    {
        FirstChar
        , Dash
        , SecondChar
        , End
    };

    //
    //  To simplify the logic below, check for an empty range, which is not
    //  legal.
    //
    if ((chPeekNext() == kCIDLib::chCloseBracket) && !m_bEscaped)
    {
        facCIDRegX().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kRegXErrs::errcRegEx_EmptyRange
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
        );
    }

    //
    //  Create a range matcher and put a janitor on it. Note that, if we
    //  find the special case "[ ]", then this one will be thrown away and
    //  a TRXAnyCharMatcher object will be returned instead.
    //
    TRXRangeMatcher* pmatchRet = new TRXRangeMatcher;
    TJanitor<TRXRangeMatcher> janMatcher(pmatchRet);

    //
    //  In order to make things simpler, lets deal with the potential 'Not'
    //  operation first. If its a circumflex and its not escaped, then the
    //  range object represents a character NOT in any of the ranges. It has
    //  to be first, so its simpler to do it here.
    //
    if ((chPeekNext() == kCIDLib::chCircumflex) && !m_bEscaped)
    {
        // Set the object to the not state
        pmatchRet->SetNot();

        // And eat the char
        chNext();
    }

    // We start in the base state. We have to end in base or char state
    tCIDLib::TCh    chFirst = kCIDLib::chNull;
    EStates         eCurState = EStates::FirstChar;
    tCIDLib::TCard4 c4RangeCount = 0;
    while (eCurState != EStates::End)
    {
        //
        //  Get the status flags and then the character. This will throw
        //  and unexpected end of pattern if we hit the end while in here.
        //
        const tCIDLib::TBoolean bLetter = m_bLetter;
        const tCIDLib::TCh chCur = chNext();

        //
        //  The only non-escaped special char that we allow inside a
        //  range is the close bracket that ends the range. So check for
        //  that here and throw the exception in one place.
        //
        if (!bLetter && (chCur != kCIDLib::chCloseBracket))
        {
            facCIDRegX().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kRegXErrs::errcRegEx_EscapeRequired
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::AppError
                , TString(chCur)
            );
        }

        if (eCurState == EStates::FirstChar)
        {
            if (bLetter)
            {
                //
                //  Save the current letter as the possible first letter
                //  of a range, and move to the dash range since we
                //  anticipate a dash (though it might be another char.)
                //
                eCurState = EStates::Dash;
                chFirst = chCur;
            }
             else if (chCur == kCIDLib::chCloseBracket)
            {
                eCurState = EStates::End;
            }
        }
         else if (eCurState == EStates::Dash)
        {
            if (bLetter)
            {
                if (chCur == kCIDLib::chHyphenMinus)
                {
                    // Just move on to get the potential second character
                    eCurState = EStates::SecondChar;
                }
                 else
                {
                    //
                    //  We did not get the dash. So we enter the old first
                    //  character as a single character range, make the
                    //  current character the new first character, and
                    //  stay in the dash state.
                    //
                    pmatchRet->AddRange(chFirst, chFirst);
                    chFirst = chCur;
                    c4RangeCount++;
                }
            }
             else if (chCur == kCIDLib::chCloseBracket)
            {
                //
                //  If the first char was a space and this is the only range
                //  we've got, then we treat that as a 'match any' special
                //  case. Otherwise, we add a single char range to the range
                //  object.
                //
                if ((chFirst == kCIDLib::chSpace) && !c4RangeCount)
                {
                    //
                    //  Create and return an any char matcher. The range
                    //  object will be cleaned up since the janitor is still
                    //  on it.
                    //
                    return new TRXAnyCharMatcher;
                }

                pmatchRet->AddRange(chFirst, chFirst);
                c4RangeCount++;

                // And move to the end state
                eCurState = EStates::End;
            }
        }
         else if (eCurState == EStates::SecondChar)
        {
            if (bLetter)
            {
                //
                //  Check that the second character is higher in the
                //  character set than the first. If they are equal, then
                //  optimize by doing a single char range.
                //
                if (chFirst == chCur)
                {
                    pmatchRet->AddRange(chFirst, chFirst);
                }
                 else if (chFirst > chCur)
                {
                    facCIDRegX().ThrowErr
                    (
                        CID_FILE
                        , CID_LINE
                        , kRegXErrs::errcRegEx_InvalidRange
                        , tCIDLib::ESeverities::Failed
                        , tCIDLib::EErrClasses::AppError
                        , TString(chFirst)
                        , TString(chCur)
                    );
                }
                 else
                {
                    pmatchRet->AddRange(chFirst, chCur);
                    c4RangeCount++;
                }

                // And go back to the first char state
                eCurState = EStates::FirstChar;
            }
             else if (chCur == kCIDLib::chCloseBracket)
            {
                //
                //  Put in a single character range for both the
                //  first char and the dash.
                //
                pmatchRet->AddRange(chFirst, chFirst);
                pmatchRet->AddRange(kCIDLib::chHyphenMinus, kCIDLib::chHyphenMinus);
                c4RangeCount += 2;

                // Move to the end state
                eCurState = EStates::End;
            }
        }
        #if CID_DEBUG_ON
        else
        {
            facCIDRegX().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kRegXErrs::errcRegEx_BadRangeState
                , tCIDLib::ESeverities::ProcFatal
                , tCIDLib::EErrClasses::Internal
            );
        }
        #endif
    }

    // Let the match object go from the janitor and return it
    janMatcher.Orphan();
    return pmatchRet;
}


