//
// FILE NAME: CIDLib_UndoCore.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/02/2004
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
//  This file implements the core undo framework classes.
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
#include    "CIDLib_.hpp"


// ---------------------------------------------------------------------------
//  Do our standard RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TUndoCmd, TObject)
RTTIDecls(TUndoCmdSep, TUndoCmd)
RTTIDecls(TUndoHandler, TObject)



// ---------------------------------------------------------------------------
//   CLASS: TUndoCmd
//  PREFIX: ui
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TUndoCmd: Constructors and Destructor
// ---------------------------------------------------------------------------
TUndoCmd::~TUndoCmd()
{
}


// ---------------------------------------------------------------------------
//  TUndoCmd: Public operators
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TUndoCmd::bIsSame(const TUndoCmd&) const
{
    // Nothing to control at this level
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TUndoCmd: Hidden constructors
// ---------------------------------------------------------------------------
TUndoCmd::TUndoCmd()
{
}



// ---------------------------------------------------------------------------
//   CLASS: TUndoCmdSep
//  PREFIX: ui
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TUndoCmdSep: Constructors and Destructor
// ---------------------------------------------------------------------------
TUndoCmdSep::TUndoCmdSep()
{
}

TUndoCmdSep::~TUndoCmdSep()
{
}


// ---------------------------------------------------------------------------
//  TUndoCmdSep: Public operators
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TUndoCmdSep::bIsSame(const TUndoCmdSep&) const
{
    return kCIDLib::True;
}





// ---------------------------------------------------------------------------
//   CLASS: TUndoHandler
//  PREFIX: uh
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  Constructors and Destructor
// ---------------------------------------------------------------------------

// Don't allow a max undo of less than 64
TUndoHandler::TUndoHandler(const tCIDLib::TCard4 c4MaxUndo) :

    m_c4LastPost(0)
    , m_c4MaxUndo(c4MaxUndo >= 64 ? c4MaxUndo : 64)
    , m_colUndoStack(tCIDLib::EAdoptOpts::Adopt)
{
}

TUndoHandler::~TUndoHandler()
{
}


// ---------------------------------------------------------------------------
//  Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TUndoHandler::bIsEmpty() const
{
    return m_colUndoStack.bIsEmpty();
}


tCIDLib::TBoolean
TUndoHandler::bLastWithin(const tCIDLib::TCard4 c4Millis) const
{
    // If empty, then obviously not
    if (m_colUndoStack.bIsEmpty())
        return kCIDLib::False;

    const tCIDLib::TCard4 c4Cur = TTime::c4Millis();

    // Less than shouldn't happen, but check for it
    if (c4Cur < m_c4LastPost)
        return kCIDLib::False;

    //
    //  If the interval between the last post and now is within the passed
    //  threshold, then return true.
    //
    return ((c4Cur - m_c4LastPost) <= c4Millis);
}


tCIDLib::TCard4 TUndoHandler::c4Count() const
{
    return m_colUndoStack.c4ElemCount();
}


tCIDLib::TCard4 TUndoHandler::c4SinceLast() const
{
    return TTime::c4Millis() - m_c4LastPost;
}


// Just pop and discard the top of stack
tCIDLib::TVoid TUndoHandler::DiscardTop()
{
    if (!m_colUndoStack.bIsEmpty())
    {
        // Reset the last post time, since the top item has been removed
        m_c4LastPost = 0;

        // Else pop an item off and delete it
        delete m_colUndoStack.pobjPop();
    }
}


//
//  Discard top of stack until we get to the next separator, which would have been pushed
//  as the first thing in a sequence that needs to be treated as a whole.
//
//  To deal with the situation where a separator gets pushed but no commands get pushed
//  after it, we will continue until there's not a separator on the top of stack once we
//  hit the first one.
//
tCIDLib::TVoid TUndoHandler::DiscardToSep()
{
    tCIDLib::TBoolean bGotSep = kCIDLib::False;
    while (!m_colUndoStack.bIsEmpty() && !bGotSep)
    {
        TUndoCmd* pucNext = m_colUndoStack.pobjPop();
        bGotSep = pucNext->bIsA(TUndoCmdSep::clsThis());
        delete pucNext;
    }

    // If we got one, get rid of any sequential ones
    if (bGotSep)
    {
        while (!m_colUndoStack.bIsEmpty())
        {
            TUndoCmd* pucNext = m_colUndoStack.pobjPeek();

            // Break out on the first non-separator
            if (!pucNext->bIsA(TUndoCmdSep::clsThis()))
                break;

            delete m_colUndoStack.pobjPop();
        }
    }

    m_c4LastPost = 0;
}


// Just let them look at the top item
TUndoCmd* TUndoHandler::pucTop()
{
    // If empty, return null
    if (m_colUndoStack.bIsEmpty())
        return nullptr;

    return m_colUndoStack.pobjPeek();
}


//
//  Push a new undo item onto the stack. We can optionally also push a separator for
//  those situations where it's not worth using an undo janitor because the caller
//  knows only one operation is going to be pushed. IT defaults to false.
//
tCIDLib::TVoid
TUndoHandler::Push(TUndoCmd* const pucToAdopt, const tCIDLib::TBoolean bPushSep)
{
    //
    //  Make sure we aren't full. If so, we have to toss some old items. We toss them
    //  in 16 item chunks.
    //
    if (m_colUndoStack.c4ElemCount() + 4 > m_c4MaxUndo)
        m_colUndoStack.ForceDown(16);

    //
    //  Compare this one to to the current top of stack. If the same, then we just
    //  delete it since it's not needed. This lets client code avoid a lot of potentially
    //  messy checks to avoid redundant undos.
    //
    if (!m_colUndoStack.bIsEmpty())
    {
        TUndoCmd* pucTop = m_colUndoStack.pobjPeek();
        if (pucTop->bIsA(pucToAdopt->clsIsA()) && pucToAdopt->bIsSame(*pucTop))
        {
            delete pucToAdopt;
            return;
        }
    }

    // If they asked, push a separator
    if (bPushSep)
        PushSep();

    // And now we can add this guy. Update the last post time
    m_c4LastPost = TTime::c4Millis();
    m_colUndoStack.Push(pucToAdopt);
}


tCIDLib::TVoid TUndoHandler::PushSep()
{
    // If the current top of stack is alreayd a separator, don't push one
    if (!m_colUndoStack.bIsEmpty())
    {
        if (m_colUndoStack.pobjPeek()->bIsA(TUndoCmdSep::clsThis()))
            return;
    }
    Push(new TUndoCmdSep());
}


TUndoCmd* TUndoHandler::pucPop()
{
    // If empty, then return a null pointer
    if (m_colUndoStack.bIsEmpty())
        return nullptr;

    // Reset the last post time, since the top item has been removed
    m_c4LastPost = 0;

    // Else pop an item off and return it
    return m_colUndoStack.pobjPop();
}


tCIDLib::TVoid TUndoHandler::Reset()
{
    m_c4LastPost = 0;
    m_colUndoStack.RemoveAll();
}


//
//  Sometimes, they have to push things in order as they process, but want
//  the one they just pushed to happen before the previous one, so we let
//  them swap the stack top with the next one down.
//
tCIDLib::TVoid TUndoHandler::SwapTop()
{
    m_colUndoStack.SwapTop();
}



// ---------------------------------------------------------------------------
//   CLASS: TUndoJanitor
//  PREFIX: jan
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TUndoJanitor: Constructors and Destructor
// ---------------------------------------------------------------------------
TUndoJanitor::TUndoJanitor(TUndoHandler* const puhTar) :

    m_puhTar(puhTar)
{
    // Push a command separator (if the top of stack is not already one)
    m_puhTar->PushSep();
}


TUndoJanitor::~TUndoJanitor()
{
    // If we still have the handler, clean it down to the next separator
    if (m_puhTar)
    {
        try
        {
            m_puhTar->DiscardToSep();
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
    }
}


// ---------------------------------------------------------------------------
//  Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TUndoJanitor::Orphan()
{
    m_puhTar = nullptr;
}

