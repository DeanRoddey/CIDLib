//
// FILE NAME: CIDResEd_ThisFacility.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/10/2000
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
#include "CIDResEd.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TFacCIDResEd,TGUIFacility)


// ---------------------------------------------------------------------------
//   CLASS: TFacCIDResEd
//  PREFIX: fac
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFacCIDResEd: Constructors and Destructor
// ---------------------------------------------------------------------------
TFacCIDResEd::TFacCIDResEd() :

    TGUIFacility
    (
        L"CIDResEd"
        , tCIDLib::EModTypes::Exe
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::HasMsgsAndRes
    )
    , m_c4NextEdId(1)
    , m_strVal_False(L"False")
    , m_strVal_True(L"True")
{
}

TFacCIDResEd::~TFacCIDResEd()
{
}


// ---------------------------------------------------------------------------
//  TFacCIDDlg: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TCard4 TFacCIDResEd::c4NextEditId()
{
    return m_c4NextEdId++;
}


// The main entry point for the program
tCIDLib::EExitCodes TFacCIDResEd::eMainThread(TThread& thrThis, tCIDLib::TVoid*)
{
    // Let our caller go
    thrThis.Sync();

    // Create our main window
    try
    {
        m_wndMainFrame.CreateMain();
    }

    catch(const TError& errToCatch)
    {
        // If doing verbose logging, and this hasn't been logged already...
        if (facCIDResEd.bShouldLog(errToCatch))
            TModule::LogEventObj(errToCatch);

        // Use the standard exception display dialog to show it
        TExceptDlg dlgShowErr
        (
            m_wndMainFrame
            , strMsg(kResEdMsgs::midGen_Title1)
            , strMsg(kResEdMsgs::midGen_UnhandledException)
            , errToCatch
        );
        return tCIDLib::EExitCodes::InitFailed;
    }

    // Make us visible, after a short msg pumping delay
//    facCIDCtrls().MsgYield(100);

    //
    //  Do the message loop. We use the standard error handler enabled
    //  frame window for our main frame, so the exception handler in the
    //  main loop will pass them to him and he'll handle them. Any unhandled
    //  exception will cause the loop to exit after logging it and telling
    //  the use what happened. No exceptions will propogate out of here
    //
    facCIDCtrls().uMainMsgLoop(m_wndMainFrame);

    //
    //  Call the window processing loop. We won't come back until the
    //  program exits.
    //
    try
    {
        m_wndMainFrame.Destroy();
    }

    catch(const TError& errToCatch)
    {
        // If doing verbose logging, and this hasn't been logged already...
        if (facCIDResEd.bShouldLog(errToCatch))
            TModule::LogEventObj(errToCatch);

        // Use the standard exception display dialog to show it
        TExceptDlg dlgShowErr
        (
            TWindow::wndDesktop()
            , strMsg(kResEdMsgs::midGen_Title1)
            , strMsg(kResEdMsgs::midGen_UnhandledException)
            , errToCatch
        );
    }
    return tCIDLib::EExitCodes::Normal;
}


//
//  A helper that deals with the the very common task of parsing a text symbol
//  name. When parsing them from text, it's because we are reading them from a
//  .CIDRC file, i.e. they are referenced from a dialog. In that form, they are
//  either message or common text types. The latter are recognized by being
//  contained in [ ] brackets. The common ones also are not the real symbol, but
//  just the basic name part. So we remove the brackets and add a prefix.
//
//  So we parse it out and crete the
//
tCIDResEd::EMsgTypes
TFacCIDResEd::eParseTextSym(const TString& strText, TString& strRealSym) const
{
    if (strText.bIsEmpty())
        return tCIDResEd::EMsgTypes::Count;

    tCIDResEd::EMsgTypes eType;
    if (strText.chFirst() == L'[')
    {
        // Last char has to be the closing bracket
        if (strText.chLast() != L']')
            return tCIDResEd::EMsgTypes::Count;

        strRealSym = strText;
        strRealSym.Cut(0, 1);
        strRealSym.DeleteLast();
        strRealSym.Insert(L"midCTxt_", 0);
        eType = tCIDResEd::EMsgTypes::Common;
    }
     else
    {
        eType = tCIDResEd::EMsgTypes::Message;
        strRealSym = strText;
    }
    return eType;
}


const TString&
TFacCIDResEd::strBoolVal(const tCIDLib::TBoolean bVal) const
{
    if (bVal)
        return m_strVal_True;
    return m_strVal_False;
}


//
//  A helper to return true or false if a style is one or not. This saves a lot
//  of overhead when setting up the key/value data to pass to the attribute editor
//  window.
//
const TString&
TFacCIDResEd::strWndStyleOn(const   tCIDCtrls::EWndStyles   eItemStyles
                            , const tCIDCtrls::EWndStyles   eToCheck) const
{
    if (tCIDLib::bAllBitsOn(eItemStyles, eToCheck))
        return m_strVal_True;
    return m_strVal_False;
}


//
//  Make our main window available. This in turn makes all the other stuff that
//  the main window provides avaliable to other windows.
//
TResEdMainFrame& TFacCIDResEd::wndMain()
{
    return m_wndMainFrame;
}

