//
// FILE NAME: CIDLib_RegExNFA.cpp
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
//  This file implements the TRegExNFA class.
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


// ---------------------------------------------------------------------------
//  RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TRXMatcher,TObject)
RTTIDecls(TRegExNFA,TObject)



// ----------------------------------------------------------------------------
//   CLASS: TRegExNFA
//  PREFIX: rxnfa
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//  TRegExNFA: Constructors and Destructor
// ----------------------------------------------------------------------------
TRegExNFA::TRegExNFA(const tCIDLib::TCard4 c4InitMaxStates) :

    m_bNullable(kCIDLib::False)
    , m_bReady(kCIDLib::False)
    , m_c4Entries(0)
    , m_c4StateCount(0)
    , m_pc4State1(nullptr)
    , m_pc4State2(nullptr)
    , m_pmatchTrans(nullptr)
{
    c4Reset(c4InitMaxStates);
}

TRegExNFA::~TRegExNFA()
{
    delete [] m_pc4State1;
    m_pc4State1 = nullptr;
    delete [] m_pc4State2;
    m_pc4State2 = nullptr;

    CleanupMatches();
    delete [] m_pmatchTrans;
    m_pmatchTrans = nullptr;
}


// ----------------------------------------------------------------------------
//  TRegExNFA: Public, non-virtual methods
// ----------------------------------------------------------------------------
tCIDLib::TBoolean
TRegExNFA::bIsEpsilonState(const tCIDLib::TCard4 c4At) const
{
    TestIndex(c4At, CID_LINE, CID_FILE);
    return (m_pmatchTrans[c4At] == 0);
}


tCIDLib::TCard4 TRegExNFA::c4AddState()
{
    if (m_c4StateCount >= m_c4Entries)
    {
        facCIDRegX().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kRegXErrs::errcRegEx_StateOverflow
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TCardinal(m_c4Entries)
        );
    }
    m_c4StateCount++;
    return m_c4StateCount - 1;
}

tCIDLib::TCard4 TRegExNFA::c4AddState(          TRXMatcher* const   pmatchNew
                                        , const tCIDLib::TCard4     c4Trans1
                                        , const tCIDLib::TCard4     c4Trans2)
{
    if (m_c4StateCount >= m_c4Entries)
    {
        facCIDRegX().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kRegXErrs::errcRegEx_StateOverflow
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TCardinal(m_c4Entries)
        );
    }

    // Add the new stuff where the state count points now
    m_pc4State1[m_c4StateCount] = c4Trans1;
    m_pc4State2[m_c4StateCount] = c4Trans2;
    m_pmatchTrans[m_c4StateCount] = pmatchNew;

    //
    //  And now bump up the counter, so now it points to the next available
    //  state.
    //
    m_c4StateCount++;
    return m_c4StateCount - 1;
}


tCIDLib::TCard4 TRegExNFA::c4AddStatePointedPastEnd(TRXMatcher* const pmatchNew)
{
    if (m_c4StateCount >= m_c4Entries)
    {
        facCIDRegX().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kRegXErrs::errcRegEx_StateOverflow
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TCardinal(m_c4Entries)
        );
    }

    // Create a new state pointing past itself
    m_pc4State1[m_c4StateCount] = m_c4StateCount + 1;
    m_pc4State2[m_c4StateCount] = m_c4StateCount + 1;
    m_pmatchTrans[m_c4StateCount] = pmatchNew;

    //
    //  And now bump up the counter, so now it points to the next available
    //  state.
    //
    m_c4StateCount++;
    return m_c4StateCount - 1;
}


tCIDLib::TCard4 TRegExNFA::c4LastState() const
{
    if (!m_c4StateCount)
    {
        facCIDRegX().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kRegXErrs::errcRegEx_NoStatesYet
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    return m_c4StateCount - 1;
}


tCIDLib::TCard4 TRegExNFA::c4MaxStates() const
{
    return m_c4Entries;
}


//
//  This is calle to reset us to get us ready for setting up a new incmoing expression.
//
tCIDLib::TCard4 TRegExNFA::c4Reset(const tCIDLib::TCard4 c4MaxStates)
{
    // Make sure its not zero
    if (!c4MaxStates)
    {
        facCIDRegX().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kRegXErrs::errcRegEx_ZeroMaxElems
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
        );
    }

    // Reset our bookkeeping stuff
    m_bReady = kCIDLib::False;
    m_bNullable = kCIDLib::False;
    m_c4StateCount = 0;

    //
    //  If the new expression cannot fit in the existing arrays, then we
    //  kill them and zero out the entry count. If the entry count is already
    //  zero, then don't bother.
    //
    if (m_c4Entries && (c4MaxStates > m_c4Entries))
    {
        delete [] m_pc4State1;
        m_pc4State1 = nullptr;
        delete [] m_pc4State2;
        m_pc4State2 = nullptr;

        CleanupMatches();
        delete [] m_pmatchTrans;
        m_c4Entries = 0;
    }

    //
    //  If there are more new entries than existing ones, then we have to
    //  allocate our new buffers. This means that either its our first
    //  visit (in which case m_c4Entries is zero), or we just deleted the
    //  old buffers above.
    //
    if (c4MaxStates > m_c4Entries)
    {
        m_c4Entries = c4MaxStates;

        m_pc4State1 = new tCIDLib::TCard4[m_c4Entries];
        m_pc4State2 = new tCIDLib::TCard4[m_c4Entries];
        m_pmatchTrans = new TRXMatcher*[m_c4Entries];
    }

    // Clean out the arrays
    TRawMem::SetMemBuf(m_pc4State1, tCIDLib::TCard4(0), m_c4Entries);
    TRawMem::SetMemBuf(m_pc4State2, tCIDLib::TCard4(0), m_c4Entries);
    TRawMem::SetMemBuf(m_pmatchTrans, tCIDLib::TCard4(0), m_c4Entries);

    return m_c4Entries;
}


tCIDLib::TCard4 TRegExNFA::c4StateCount() const
{
    return m_c4StateCount;
}

tCIDLib::TCard4 TRegExNFA::c4State1At(const tCIDLib::TCard4 c4At) const
{
    TestIndex(c4At, CID_LINE, CID_FILE);
    return m_pc4State1[c4At];
}

tCIDLib::TCard4 TRegExNFA::c4State2At(const tCIDLib::TCard4 c4At) const
{
    TestIndex(c4At, CID_LINE, CID_FILE);
    return m_pc4State2[c4At];
}


//
//  This has to be called after all of the states are updated. We will do some final
//  checking and updating and set the ready flag to indicate we have a good NFA.
//
tCIDLib::TVoid TRegExNFA::Complete()
{
    // If already marked ready, they didn't reset us
    if (m_bReady)
    {
        facCIDRegX().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kRegXErrs::errcRegEx_MustReset
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // Make sure something was set up
    if (!m_c4StateCount)
    {
        facCIDRegX().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kRegXErrs::errcRegEx_NoPattern
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // Mark us ready
    m_bReady = kCIDLib::True;

    //
    //  And see if we are nullable. If there is a path through the NFA that is
    //  all epsilon nodes, then it's nullable. We have to use a stack to do this
    //  since there are multiple possible such paths. Worst case we have to go
    //  through both sides of every state, so 2*count is the max.
    //
    tCIDLib::TCardStack fcolStack(m_c4StateCount * 2);

    // Start us off with the first state on the stack and nullable defaulted to false
    m_bNullable = kCIDLib::False;
    fcolStack.Push(m_pc4State1[0]);
    while (!fcolStack.bIsEmpty())
    {
        // Get the next state off the stack
        const tCIDLib::TCard4 c4CurState = fcolStack.tPop();

        //
        //  If this is the last state, then we made it all the way through
        //  so break out with nullable set.
        //
        if (c4CurState == m_c4StateCount - 1)
        {
            m_bNullable = kCIDLib::True;
            break;
        }

        if (bIsEpsilonState(c4CurState))
        {
            // It's an epsilon state. Push the unique out states from here
            fcolStack.Push(m_pc4State1[c4CurState]);
            if (m_pc4State1[c4CurState] != m_pc4State2[c4CurState])
                fcolStack.Push(m_pc4State2[c4CurState]);
        }
         else
        {
            // We hit a non-epsilon state, if the stack is empty we failed
            if (fcolStack.bIsEmpty())
                break;
        }
    }
}


const TRXMatcher& TRegExNFA::matchAt(const tCIDLib::TCard4 c4At) const
{
    TestIndex(c4At, CID_LINE, CID_FILE);

    if (!m_pmatchTrans[c4At])
    {
        facCIDRegX().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kRegXErrs::errcRegEx_NoMatcher
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    return *m_pmatchTrans[c4At];
}

TRXMatcher& TRegExNFA::matchAt(const tCIDLib::TCard4 c4At)
{
    TestIndex(c4At, CID_LINE, CID_FILE);

    if (!m_pmatchTrans[c4At])
    {
        facCIDRegX().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kRegXErrs::errcRegEx_NoMatcher
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    return *m_pmatchTrans[c4At];
}


TRXMatcher* TRegExNFA::pmatchAt(const tCIDLib::TCard4 c4At)
{
    TestIndex(c4At, CID_LINE, CID_FILE);
    return m_pmatchTrans[c4At];
}


tCIDLib::TVoid TRegExNFA::PointPastEnd(const tCIDLib::TCard4 c4At)
{
    TestIndex(c4At, CID_LINE, CID_FILE);

    m_pc4State1[c4At] = m_c4StateCount;
    m_pc4State2[c4At] = m_c4StateCount;
}


tCIDLib::TVoid
TRegExNFA::UpdateLast(TRXMatcher* const pmatchNew, const tCIDLib::TCard4 c4Trans)
{
    if (!m_c4StateCount)
    {
        facCIDRegX().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kRegXErrs::errcRegEx_NoStatesYet
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    UpdateStateAt(m_c4StateCount-1, pmatchNew, c4Trans);
}

tCIDLib::TVoid TRegExNFA::UpdateLast(const  tCIDLib::TCard4 c4Trans1
                                    , const tCIDLib::TCard4 c4Trans2)
{
    if (!m_c4StateCount)
    {
        facCIDRegX().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kRegXErrs::errcRegEx_NoStatesYet
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    UpdateStateAt(m_c4StateCount-1, c4Trans1, c4Trans2);
}


tCIDLib::TVoid
TRegExNFA::UpdateMatchAt(const  tCIDLib::TCard4     c4At
                        ,       TRXMatcher* const   pmatchNew)
{
    TestIndex(c4At, CID_LINE, CID_FILE);

    // If there is a match here, then delete it first
    if (m_pmatchTrans[c4At])
    {
        delete m_pmatchTrans[c4At];
        m_pmatchTrans[c4At] = 0;
    }
    m_pmatchTrans[c4At] = pmatchNew;
}


tCIDLib::TVoid
TRegExNFA::UpdateStateAt(const  tCIDLib::TCard4     c4At
                        ,       TRXMatcher* const   pmatchNew
                        , const tCIDLib::TCard4     c4Trans)
{
    TestIndex(c4At, CID_LINE, CID_FILE);

    // If there is a match here, then delete it first
    if (m_pmatchTrans[c4At])
    {
        delete m_pmatchTrans[c4At];
        m_pmatchTrans[c4At] = 0;
    }

    m_pmatchTrans[c4At] = pmatchNew;
    m_pc4State1[c4At] = c4Trans;
    m_pc4State2[c4At] = c4Trans;
}


tCIDLib::TVoid
TRegExNFA::UpdateStateAt(const  tCIDLib::TCard4         c4At
                        , const tCIDLib::TCard4         c4Trans1
                        , const tCIDLib::TCard4         c4Trans2)
{
    TestIndex(c4At, CID_LINE, CID_FILE);

    // If there is a match here, then delete it
    if (m_pmatchTrans[c4At])
    {
        delete m_pmatchTrans[c4At];
        m_pmatchTrans[c4At] = 0;
    }

    m_pc4State1[c4At] = c4Trans1;
    m_pc4State2[c4At] = c4Trans2;
}


tCIDLib::TVoid
TRegExNFA::UpdateState1At(  const   tCIDLib::TCard4 c4At
                            , const tCIDLib::TCard4 c4Trans1)
{
    TestIndex(c4At, CID_LINE, CID_FILE);

    m_pc4State1[c4At] = c4Trans1;
}


tCIDLib::TVoid
TRegExNFA::UpdateState2At(  const   tCIDLib::TCard4 c4At
                            , const tCIDLib::TCard4 c4Trans2)
{
    TestIndex(c4At, CID_LINE, CID_FILE);

    m_pc4State2[c4At] = c4Trans2;
}


// ----------------------------------------------------------------------------
//  TRegExNFA: Protected, inherited methods
// ----------------------------------------------------------------------------
tCIDLib::TVoid TRegExNFA::FormatTo(TTextOutStream& strmDest) const
{
    //
    //  Set up a stream format for columnar output. We want to display
    //  the data like this:
    //
    //  0   fs  ss      chars
    //  1   fs  ss      chars
    //
    //  And so on. The left column is the state number. fs is the first
    //  state out of that state, ss is the second state out of that state.
    //  Chars is the set of chars matched to get out of that state.
    //
    TStreamJanitor janFormat(&strmDest);
    TStreamFmt strmfNums(4, 0, tCIDLib::EHJustify::Right, kCIDLib::chSpace);
    TStreamFmt strmfChars(0, 0, tCIDLib::EHJustify::Left, kCIDLib::chSpace);

    for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4StateCount; c4Index++)
    {
        // Do the state number and transition states
        strmDest << strmfNums << c4Index
                 << m_pc4State1[c4Index] << m_pc4State2[c4Index];

        //
        //  And then get the match object to display its characters, or
        //  "N/A" if its null for this state.
        //
        strmDest << strmfChars << L"  ";
        if (m_pmatchTrans[c4Index])
            strmDest << *m_pmatchTrans[c4Index] << kCIDLib::NewLn;
         else
            strmDest << L"N/A" << kCIDLib::NewLn;
    }
}


// ----------------------------------------------------------------------------
//  TRegExNFA: Private, non-virtual methods
// ----------------------------------------------------------------------------
tCIDLib::TVoid TRegExNFA::CleanupMatches()
{
    for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4Entries; c4Index++)
    {
        delete m_pmatchTrans[c4Index];
        m_pmatchTrans[c4Index] = nullptr;
    }
}


tCIDLib::TVoid TRegExNFA::TestIndex(const   tCIDLib::TCard4     c4At
                                    , const tCIDLib::TCard4     c4Line
                                    , const tCIDLib::TCh* const pszFile) const
{
    if (c4At < m_c4StateCount)
        return;

    // NOTE: We are throwing a CIDLib error here, not one of ours!
    facCIDLib().ThrowErr
    (
        pszFile
        , c4Line
        , kCIDErrs::errcGen_IndexError
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::BadParms
        , TCardinal(c4At)
        , clsThis()
        , TCardinal(m_c4StateCount)
    );
}
