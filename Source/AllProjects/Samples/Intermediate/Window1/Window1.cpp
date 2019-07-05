//
// CID_FILE NAME: Window1.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/21/1019
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
//  This sample program is about the simplest UI based program possible, basically
//  a UI version of Hello, World. It creates the simplest possible frame window
//  derivative, which it just shows.
//
//  As with many of the simplest samples we don't create a facility object we just
//  start a main thread on a local function, though in this case we use a different
//  main module macro than with all of the console samples.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
//  Includes
// ----------------------------------------------------------------------------
#include  "CIDCtrls.hpp"


// ----------------------------------------------------------------------------
//  Magic main module code. Note this one is different from the console based
//  samples!
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread&, tCIDLib::TVoid*);
CIDLib_CtrlsModule(TThread(L"MainThread", eMainThreadFunc))


// ----------------------------------------------------------------------------
//   CLASS: TTestFrameWnd
//  PREFIX: wnd
// ----------------------------------------------------------------------------
class TTestFrameWnd : public TFrameWnd
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TTestFrameWnd() = default;

        TTestFrameWnd(const TTestFrameWnd&) = delete;

        ~TTestFrameWnd() = default;


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTestFrameWnd, TFrameWnd)
};
RTTIDecls(TTestFrameWnd, TFrameWnd)



// ----------------------------------------------------------------------------
//  We start up the main thread here
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    try
    {
        // Create a frame window, let the system decide on placement
        TTestFrameWnd wndMain;
        wndMain.CreateFrame
        (
            nullptr
            , facCIDCtrls().areaDefWnd()
            , L"Window 1 Sample"
            , tCIDCtrls::EWndStyles::StdFrame
            , tCIDCtrls::EExWndStyles::None
            , tCIDCtrls::EFrameStyles::StdFrame
            , kCIDLib::False
        );

        // Now just process msgs until the user closes us
        facCIDCtrls().uMainMsgLoop(wndMain);
    }

    catch(TError& errToCatch)
    {
        // Use the desktop as owner, since our frame window may not have worked
        TErrBox msgbErr(L"Window 1 Sample", errToCatch.strErrText());
        msgbErr.ShowIt(TWindow::wndDesktop());
    }

    return tCIDLib::EExitCodes::Normal;
}
