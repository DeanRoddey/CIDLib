//
// CID_FILE NAME: TestCIDWnd.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/28/1997
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the main module of the program. This program tests the
//  services of the CIDWnd.Dll facility.
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
#include    "TestCIDWnd.hpp"



// ----------------------------------------------------------------------------
//  Local, const data
//
//  pszTitle1
//  pszTitle2
//      Titles used in emergency popups
// ----------------------------------------------------------------------------
const tCIDLib::TCh* const   pszTitle1 = L"TestCIDWnd";
const tCIDLib::TCh* const   pszTitle2 = L"CIDLib Windowing Tester";


// ----------------------------------------------------------------------------
//  Forward references
// ----------------------------------------------------------------------------
static tCIDLib::EExitCodes eMainThreadFunc
(
        TThread&            thrThis
        , tCIDLib::TVoid*   pData
);


// ----------------------------------------------------------------------------
//  Global data
//
//  facTestCIDWnd
//      The facility object for this program. It is exported to the rest of
//      the program via the main header. We have no special needs so we just
//      create an instance of the base GUI facility class.
// ----------------------------------------------------------------------------
TGUIFacility facTestCIDWnd
(
    L"TestCIDWnd"
    , tCIDLib::EModTypes::Exe
    , kCIDLib::c4MajVersion
    , kCIDLib::c4MinVersion
    , kCIDLib::c4Revision
    , tCIDLib::EModFlags::HasMsgsAndRes
);


// ----------------------------------------------------------------------------
//  Magic main module code
// ----------------------------------------------------------------------------
CIDLib_MainGUIModule(TThread(L"MainThread", eMainThreadFunc))



// ----------------------------------------------------------------------------
//  Local functions
// ----------------------------------------------------------------------------
static tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();


    if (facCIDWnd().bActivatePreviousInst(L"This is the text"))
        return tCIDLib::EExitCodes::Normal;

    TArea areaTmp;
    TGUIInfo::bQueryNamedMonArea(L"\\\\.\\DISPLAY1", areaTmp, kCIDLib::True);

    tCIDLib::TUInt uReturn = 0;
    TMyWindow wndMainFrame;
    try
    {
        // Create a main frame window object
        wndMainFrame.Create
        (
            TWindow::wndDesktop()
            , L"This is the text"
            , TArea(220, 220, 650, 510)
        );
    }

    //
    //  Since this is a test program just catch all exceptions so that
    //  we will know that we died during testing since we can put break
    //  points here.
    //
    //  We use ESev_Status for all of the logging here so that they don't
    //  cause more throws.
    //
    catch(TError& errToCatch)
    {
        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        TPopUp::PopUpErr
        (
            errToCatch.strFileName()
            , errToCatch.c4LineNum()
            , pszTitle1
            , pszTitle2
            , errToCatch.errcId()
            , errToCatch.errcKrnlId()
            , errToCatch.errcHostId()
            , errToCatch.strErrText()
            , errToCatch.strAuxText()
        );
        return tCIDLib::EExitCodes::InitFailed;
    }

    // Make sure that kernel errors get logged
    catch(const TKrnlError& kerrToCatch)
    {
        facTestCIDWnd.LogKrnlErr
        (
            CID_FILE
            , CID_LINE
            , 0
            , kerrToCatch
            , tCIDLib::ESeverities::Status
            , tCIDLib::EErrClasses::AppStatus
        );

        TPopUp::PopUpErr
        (
            CID_FILE
            , CID_LINE
            , pszTitle1
            , pszTitle2
            , 0
            , kerrToCatch.errcId()
            , kerrToCatch.errcHostId()
            , L"A kernel exception occured during init"
            , kCIDLib::pszEmptyZStr
        );
        return tCIDLib::EExitCodes::InitFailed;
    }

    catch(...)
    {
        facTestCIDWnd.LogMsg
        (
            CID_FILE
            , CID_LINE
            , L"An unknown exception occured during init"
            , tCIDLib::ESeverities::Status
            , tCIDLib::EErrClasses::AppStatus
        );

        TPopUp::PopUpMsg
        (
            CID_FILE
            , CID_LINE
            , pszTitle1
            , pszTitle2
            , L"An unknown exception occured during init"
            , kCIDLib::pszEmptyZStr
        );
        return tCIDLib::EExitCodes::InitFailed;
    }

    //
    //  Do the message loop. We use the standard error handler enabled
    //  frame window for our main frame, so the exception handler in the
    //  main loop will pass them to him and he'll handle them. Any unhandled
    //  exception will cause the loop to exit after logging it and telling
    //  the use what happened. No exceptions will propogate out of here
    //
    uReturn = facCIDWnd().uMessageLoop(wndMainFrame);

    try
    {
        wndMainFrame.Destroy();
    }

    catch(TError& errToCatch)
    {
        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        TPopUp::PopUpErr
        (
            errToCatch.strFileName()
            , errToCatch.c4LineNum()
            , pszTitle1
            , pszTitle2
            , errToCatch.errcId()
            , errToCatch.errcKrnlId()
            , errToCatch.errcHostId()
            , errToCatch.strErrText()
            , errToCatch.strAuxText()
        );
    }

    // Make sure that kernel errors get logged
    catch(const TKrnlError& kerrToCatch)
    {
        facTestCIDWnd.LogKrnlErr
        (
            CID_FILE
            , CID_LINE
            , 0
            , kerrToCatch
            , tCIDLib::ESeverities::Status
            , tCIDLib::EErrClasses::AppStatus
        );

        TPopUp::PopUpErr
        (
            CID_FILE
            , CID_LINE
            , pszTitle1
            , pszTitle2
            , 0
            , kerrToCatch.errcId()
            , kerrToCatch.errcHostId()
            , L"A kernel exception occured during cleanup"
            , kCIDLib::pszEmptyZStr
        );
    }

    catch(...)
    {
        facTestCIDWnd.LogMsg
        (
            CID_FILE
            , CID_LINE
            , L"An unknown exception occured during cleanup"
            , tCIDLib::ESeverities::Status
            , tCIDLib::EErrClasses::AppStatus
        );

        TPopUp::PopUpMsg
        (
            CID_FILE
            , CID_LINE
            , pszTitle1
            , pszTitle2
            , L"An unknown exception occured during cleanup"
            , kCIDLib::pszEmptyZStr
        );
    }

    return tCIDLib::EExitCodes(uReturn);
}

