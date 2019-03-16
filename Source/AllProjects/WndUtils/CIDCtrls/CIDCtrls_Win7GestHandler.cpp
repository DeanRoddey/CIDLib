//
// FILE NAME: CIDCtrls_Win7GestureEng.cpp
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
//  This file implements the gesture engine derivative for systems that support real
//  multi-touch gestures.
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
#include    "CIDCtrls_Win7GestHandler_.hpp"



// ---------------------------------------------------------------------------
//   CLASS: Win7GestHandler
//  PREFIX: gesth
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TWin7GGestHandler: Constructors and Destructor
// ---------------------------------------------------------------------------
TWin7GestHandler::TWin7GestHandler(         MCIDGestTarget* const   pmgesttToUse
                                    , const tCIDCtrls::TWndHandle   hwndTar) :

    TCIDWndGestHandler(pmgesttToUse)
    , m_eCurState(EW7States::Idle)
    , m_hwndTar(hwndTar)
{
}


//
//  The code that uses us passes us mouse and touch window messages to process. We turn
//  those into gesture output by making the appropriate calls down into the base gesture
//  handler class.
//
//  In our case we handle real gesture messages, however we also still process some mouse
//  oriented messages, since press/release messages are still sent as mouse clicks. They
//  are only reported as gestures if movement occurs.
//
tCIDLib::TBoolean
TWin7GestHandler::bHandleMsg(const  tCIDCtrls::TWndMsg      wmsgMsg
                            , const tCIDCtrls::TWParam      wParam
                            , const tCIDCtrls::TLParam      lParam
                            ,       tCIDCtrls::TMsgResult&  mresRet
                            ,       tCIDLib::TBoolean&      bFlag)
{
    // Unless explicitly otherwise, let the message pass on to the window
    tCIDLib::TBoolean bRet = kCIDLib::True;

    if (wmsgMsg == WM_GESTURE)
    {
         // Create a structure to populate and retrieve the extra message info.
        GESTUREINFO GestInfo;
        ZeroMemory(&GestInfo, sizeof(GESTUREINFO));
        GestInfo.cbSize = sizeof(GESTUREINFO);
        ::GetGestureInfo((HGESTUREINFO)lParam, &GestInfo);

        if (GestInfo.dwID == GID_PAN)
        {
            // Get the new position.
            POINT pntCvt = {GestInfo.ptsLocation.x, GestInfo.ptsLocation.y};
            ::MapWindowPoints(0, m_hwndTar, &pntCvt, 1);
            m_pntTmp.Set(pntCvt.x, pntCvt.y);

            // Call the appropriate method in our parent class
            if (GestInfo.dwFlags & GF_BEGIN)
            {
                //
                //  Note that the base class returns a go/no-go indicator
                //  but we don't really do anything with it. The underlying
                //  gesture is going to happen one way or another. It'll just
                //  end up getting ignored by the base class since he will
                //  remain in an idle state and not react. We don't bother
                //  trying to supress them.
                //
                bStartGesture(m_pntTmp);
            }
             else if (GestInfo.dwFlags & GF_END)
            {
                EndGesture(m_pntTmp);
            }
             else
            {
                Move
                (
                    m_pntTmp, tCIDLib::TCard4(GestInfo.ullArguments & 0xFFFFFFFF) != 0
                );
            }
        }
    }
     else if (wmsgMsg == WM_LBUTTONDOWN)
    {
        //
        //  If not shifted in any way, then we we ask the base class if it
        //  it would be ok with a buton click. If so, we capture the mouse
        //  and go into pressed state. Else we do nothing.
        //
        //  The base class may say no if there's outstanding inertial scrolling
        //  going on, and he wants to use the press to stop that instead of
        //  starting a new gesture.
        //
        if ((wParam & (MK_CONTROL | MK_SHIFT)) == 0)
        {
            m_pntPress.Set
            (
                TRawBits::i2Low16From32(lParam)
                , TRawBits::i2High16From32(lParam)
            );

            if (bAllowClickStart())
            {
                // Set up the breakout area
                m_areaBreakout.Set(m_pntPress, TSize(18, 18));
                m_areaBreakout.AdjustOrg(-9, -9);

                // Remember we got a press and are waiting to see what happens
                m_eCurState = EW7States::GotPress;

                // Capture the mouse
                ::SetCapture(m_hwndTar);
            }

            // Don't let the window see this msg
            bRet = kCIDLib::False;
        }
    }
     else if (wmsgMsg == WM_MOUSEMOVE)
    {
        //
        //  If we got a press, see if we move outside the breakout area. If
        //  so we cancel our tracking and go back to idle state.
        //
        if (m_eCurState == EW7States::GotPress)
        {
            m_pntTmp.Set
            (
                TRawBits::i2Low16From32(lParam)
                , TRawBits::i2High16From32(lParam)
            );
            if (!m_areaBreakout.bContainsPoint(m_pntTmp))
            {
                // Cancel our mouse tracking and go back to idle
                ::ReleaseCapture();
                m_eCurState = EW7States::Idle;
            }
        }
    }
     else if (wmsgMsg == WM_LBUTTONUP)
    {
        m_pntTmp.Set
        (
            TRawBits::i2Low16From32(lParam)
            , TRawBits::i2High16From32(lParam)
        );

        // If not in idle state, then release the mouse
        if (m_eCurState != EW7States::Idle)
            ::ReleaseCapture();

        //
        //  Go back to idle state now. But remember the current state
        //  for below. This insures we get back to idle.
        //
        EW7States eState(m_eCurState);
        m_eCurState = EW7States::Idle;

        //
        //  Hide this because it's going to be an unmatched button up or
        //  one that we are turning into a click.
        //
        bRet = kCIDLib::False;

        //
        //  If we never moved out of the breakout area, then tell our parent
        //  area we were clicked.
        //
        if (eState == EW7States::GotPress)
            Clicked(m_pntTmp);
    }

    return bRet;
}


//
//  For init, we set up the gesture config stuff so that we only get the gestures we care
//  about. Unless the target window overrides the pre-gesture setup callback, these stay
//  this way for the life of the window.
//
tCIDLib::TVoid TWin7GestHandler::Initialize()
{
    // Call our parent first
    TCIDWndGestHandler::Initialize();

    // We want to disable flicks permanently
    #define TABLET_DISABLE_FLICKS 0x00010000
    const DWORD_PTR dwHwndTabletProperty
    (
        TABLET_DISABLE_FLICKS
    );

    SetProp
    (
        m_hwndTar
        , L"MicrosoftTabletPenServiceProperty"
        , reinterpret_cast<HANDLE>(dwHwndTabletProperty)
    );

    //
    //  We always use the same options. We generate our own inertia
    //  in the higher level code (our base class), and deal with h/v
    //  limiting and all that. We leave on gutter since it doesn't
    //  hurt anything. And we enable both directions since we will
    //  decide which direction to react to ourself.
    //
    DWORD dwPanEnable
    (
        GC_PAN_WITH_GUTTER
        | GC_PAN_WITH_SINGLE_FINGER_HORIZONTALLY
        | GC_PAN_WITH_SINGLE_FINGER_VERTICALLY
    );
    DWORD dwPanDisable = GC_PAN_WITH_INERTIA;

    // And set up our one enable type in the list
    GESTURECONFIG aGCfg[] =
    {
        {
            GID_PAN, dwPanEnable, dwPanDisable
        }
    };

    const UINT uiGcsCnt = sizeof(aGCfg) / sizeof(aGCfg[0]);
    BOOL bResult = ::SetGestureConfig
    (
        m_hwndTar, 0, uiGcsCnt, aGCfg, sizeof(GESTURECONFIG)
    );
}


// ---------------------------------------------------------------------------
//  TWin7GestHandler: Protected, inherited methods
// ---------------------------------------------------------------------------

// For now a no-op but call our parent always
tCIDLib::TVoid TWin7GestHandler::Reset(const tCIDCtrls::EGestReset eType)
{
    TCIDWndGestHandler::Reset(eType);
}

