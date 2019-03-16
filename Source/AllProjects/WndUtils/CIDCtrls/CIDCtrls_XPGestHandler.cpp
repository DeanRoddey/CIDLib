//
// FILE NAME: CIDCtrls_XPGestureEng.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/26/2013
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This file implements the gesture engine derivative for mouse based faux gesture
//  processing. This is used if the local system doesn't support multi-touch or the user
//  forces non-multi-touch based gesture handling.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDCtrls_.hpp"
#include    "CIDCtrls_XPGestHandler_.hpp"



// ---------------------------------------------------------------------------
//   CLASS: TXPGestHandler
//  PREFIX: gesth
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TXPGestHandler: Constructors and Destructor
// ---------------------------------------------------------------------------
TXPGestHandler::TXPGestHandler(         MCIDGestTarget* const   pmgesttToUse
                                , const tCIDCtrls::TWndHandle   hwndTar) :

    TCIDWndGestHandler(pmgesttToUse)
    , m_eCurState(EXPStates::Idle)
    , m_hwndTar(hwndTar)
{
}

TXPGestHandler::~TXPGestHandler()
{
}


// ---------------------------------------------------------------------------
//  TXPGestHandler: Public, virtual methods
// ---------------------------------------------------------------------------

//
//  The code that uses us passes us mouse and touch window messages to process. We turn
//  those into gesture output by making the appropriate calls down into the base gesture
//  handler class. In our case we only deal with the mouse messages.
//
tCIDLib::TBoolean
TXPGestHandler::bHandleMsg( const   tCIDCtrls::TWndMsg      wmsgMsg
                            , const tCIDCtrls::TWParam      wParam
                            , const tCIDCtrls::TLParam      lParam
                            ,       tCIDCtrls::TMsgResult&  mresRet
                            ,       tCIDLib::TBoolean&      bFlag)
{
    // Unless explicitly otherwise, let the message pass on to the window
    tCIDLib::TBoolean bRet = kCIDLib::True;

    if (wmsgMsg == WM_MOUSEMOVE)
    {
        m_pntTmp.Set
        (
            TRawBits::i2Low16From32(lParam)
            , TRawBits::i2High16From32(lParam)
        );

        //
        //  If we don't do either of these two scenarios, then we just let
        //  the mouse move go to the window. Otherwise we supress it.
        //
        if (m_eCurState == EXPStates::GotPress)
        {
            //
            //  If we've moved out of the breakout area, start a gesture
            //  and report it to the window.
            //
            if (!m_areaBreakout.bContainsPoint(m_pntTmp))
            {
                //
                //  Call our parent with a start and the original start
                //  point. If the derived class says it's ok, then we will
                //  have set the gesture flag and we then move into gesture
                //  state and send the first move.
                //
                //  If the derived class says no, we go back to idle.
                //
                //  For the most part we will always get a true here, since
                //  in our case we also start gestures via button presses,
                //  and the initial button press has to be approved as well
                //  and wouldn't have if the caller didn't want to start a
                //  new gesture.
                //
                bFlag = bStartGesture(m_pntStart);
                if (bFlag)
                {
                    m_eCurState = EXPStates::InGesture;

                    // Do the first move
                    Move(m_pntTmp, kCIDLib::False);
                }
                 else
                {
                    m_eCurState = EXPStates::Idle;
                }
            }

            // Don't let the target window see these
            bRet = kCIDLib::False;
        }
         else if (m_eCurState == EXPStates::InGesture)
        {
            //
            //  If we are in a gesture, then we report these as gesture
            //  movements.
            //
            if (bFlag)
            {
                // Get the coordinates out
                m_pntTmp.Set
                (
                    TRawBits::i2Low16From32(lParam)
                    , TRawBits::i2High16From32(lParam)
                );

                // Call our parent. We are never two fingered, so pass false
                Move(m_pntTmp, kCIDLib::False);
            }

            // Don't let the target window see these
            bRet = kCIDLib::False;
        }
    }
     else if (wmsgMsg == WM_LBUTTONDOWN)
    {
        //
        //  If shifted, then we just let it be treated like a mouse button.
        //  We'll only used unshifted buttons as gestures.
        //
        if ((wParam & (MK_CONTROL | MK_SHIFT)) == 0)
        {
            //
            //  If we are idle, and the base class says it's ok to accept
            //  this button click start, then remember this as a potential
            //  gseture.
            //
            //  The base class may say no if there's outstanding inertial
            //  scrolling going on, and he wants to use the press to stop
            //  that instead of starting a new gesture or click.
            //
            m_pntStart.Set
            (
                TRawBits::i2Low16From32(lParam)
                , TRawBits::i2High16From32(lParam)
            );

            if (bAllowClickStart())
            {
                // Set up the breakout area
                m_areaBreakout.Set(m_pntStart, TSize(12, 12));
                m_areaBreakout.AdjustOrg(-6, -6);

                // Remember we got a press and are waiting to see what happens
                m_eCurState = EXPStates::GotPress;

                // Capture the mouse
                ::SetCapture(m_hwndTar);
            }

            // Don't let the window see this msg
            bRet = kCIDLib::False;
        }
    }
     else if (wmsgMsg == WM_LBUTTONUP)
    {
        // Get the coordinates out
        m_pntTmp.Set
        (
            TRawBits::i2Low16From32(lParam)
            , TRawBits::i2High16From32(lParam)
        );

        //
        //  Go back to idle state now. But remember the current state
        //  for below. This insures we get back to idle.
        //
        EXPStates eState(m_eCurState);
        m_eCurState = EXPStates::Idle;

        // If not in idle state, then release the mouse
        if (eState != EXPStates::Idle)
            ::ReleaseCapture();

        //
        //  If we were waiting for berakout and never got it, then it's a
        //  now a click. Else, if we are in a gesture, then we send the
        //  end gesture and clean up.
        //
        if (eState == EXPStates::GotPress)
        {
            // Tell our parent we were clicked
            Clicked(m_pntTmp);
            bRet = kCIDLib::False;
        }
         else if (eState == EXPStates::InGesture)
        {
            // Clear the gesture flag on the target window
            bFlag = kCIDLib::False;

            // Call our parent to tell him its over
            EndGesture(m_pntTmp);

            // Don't let the window see this msg
            bRet = kCIDLib::False;
        }
         else
        {
            //
            //  Hide this because it's going to be an unmatched button up.
            //  If we were in a button down we'd be in got press state. So
            //  we must have gotten the button down but got cancelled.
            //
            bRet = kCIDLib::False;
        }
    }
    return bRet;
}


// ---------------------------------------------------------------------------
//  TXPGestHandler: Protected, inherited methods
// ---------------------------------------------------------------------------

//
//  We don't do anything for starting. For cancelled or ending we clean our
//  state up.
//
tCIDLib::TVoid TXPGestHandler::Reset(const tCIDCtrls::EGestReset eType)
{
    // Call our parent always
    TCIDGestHandler::Reset(eType);

    if ((eType == tCIDCtrls::EGestReset::Ended)
    ||  (eType == tCIDCtrls::EGestReset::Cancelled))
    {
        // If not idle, then clean up
        if (m_eCurState != EXPStates::Idle)
        {
            m_eCurState = EXPStates::Idle;

            // And release the mouse capture, which we should have in this case
            ::ReleaseCapture();
        }
    }
}


