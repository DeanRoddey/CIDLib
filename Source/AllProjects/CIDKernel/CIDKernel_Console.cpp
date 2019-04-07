//
// FILE NAME: CIDKernel_Console.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/10/1996
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
//  This file provides the platform independent parts of the console classes.
//  Its mostly implemented in terms of the public and private methods that are
//  provided by each per-platform driver.
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
#include    "CIDKernel_.hpp"



// ---------------------------------------------------------------------------
//   CLASS: TKrnlConIn
//  PREFIX: kcon
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlConIn: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlConIn::TKrnlConIn( const   tCIDLib::TBoolean   bInsertMode
                        , const tCIDLib::TCard4     c4MaxRecall) :

    m_bBreakSeen(kCIDLib::False)
    , m_bInsertMode(bInsertMode)
    , m_pPlatform(0)
{
    m_pPlatform = pInitPlatform(c4MaxRecall);
}

TKrnlConIn::~TKrnlConIn()
{
    //
    //  First do a close, to shut down the console handles, then call the
    //  platform term to clean up the per-platform data.
    //
    Close();
    TermPlatform();
}


// ---------------------------------------------------------------------------
//  TKrnlConIn: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKrnlConIn::bHandleSignal(const tCIDLib::ESignals)
{
    //
    //  Just consider any of the signals reason enough to break out of the
    //  input routine.
    //
    m_bBreakSeen = kCIDLib::True;

    // Return false to let others see it as well
    return kCIDLib::False;
}


// ---------------------------------------------------------------------------
//  TKrnlConIn: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKrnlConIn::bIsOpen() const
{
    return m_hconThis.bIsValid();
}


tCIDLib::TBoolean TKrnlConIn::bOpen()
{
    // Make sure its not already opened
    if (m_hconThis.bIsValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyOpen);
        return kCIDLib::False;
    }

    // Ask the platform support to try to open the console
    if (!bOpenCon())
        return kCIDLib::False;

    // Force the initial insert mode
    bSetInsertMode(m_bInsertMode);

    return kCIDLib::True;
}


tCIDLib::TVoid TKrnlConIn::ResetRecallBuf()
{
    // Pass it to the per-platform implementation
    ResetRecall();
}






// ---------------------------------------------------------------------------
//   CLASS: TKrnlConOut
//  PREFIX: kcon
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlConOut: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlConOut::TKrnlConOut() :

    m_pPlatform(0)
{
    // Let the platform create its per-platform storage
    m_pPlatform = pInitPlatform();
}

TKrnlConOut::~TKrnlConOut()
{
    // Do per-platform cleanup
    Close();
    TermPlatform();
}


// ---------------------------------------------------------------------------
//  TKrnlConOut: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKrnlConOut::bIsOpen() const
{
    return m_hconThis.bIsValid();
}


tCIDLib::TBoolean TKrnlConOut::bOpen()
{
    // Make sure its not already opened
    if (m_hconThis.bIsValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyOpen);
        return kCIDLib::False;
    }

    // Its not, so invoke the per-platform open
    return bOpenCon();
}

