//
// FILE NAME: CIDCtrl_ExceptDlgs.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/13/2015
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This class implements some standard dialogs for displaying errors when there is
//  a caught TError exception involved.
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
#include    "CIDCtrls_.hpp"




// ---------------------------------------------------------------------------
//  CLASS: TExceptDlg
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TExceptDlg: Constructors and Destructor
// ---------------------------------------------------------------------------
TExceptDlg::TExceptDlg( const   TWindow&    wndOwner
                        , const TString&    strTitleText
                        , const TString&    strGenMsg
                        , const TError&     errToShow) :

    m_errToShow(errToShow)
    , m_strGenMsg(strGenMsg)
    , m_strTitleText(strTitleText)
{
    c4RunDlg(wndOwner, facCIDCtrls(), kCIDCtrls::ridDlg_ExceptDlg);

    // If the error hasn't been logged yet, then do so.
    if (!m_errToShow.bLogged())
    {
        m_errToShow.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(m_errToShow);
    }
}

TExceptDlg::TExceptDlg( const   TWindow&    wndOwner
                        , const TString&    strGenMsg
                        , const TError&     errToShow) :

    m_errToShow(errToShow)
    , m_strGenMsg(strGenMsg)
{
    // Get the title text of the owner as our title
    m_strTitleText = wndOwner.strWndText();

    // And run the dialog
    c4RunDlg(wndOwner, facCIDCtrls(), kCIDCtrls::ridDlg_ExceptDlg);

    // If the error hasn't been logged yet, then do so.
    if (!m_errToShow.bLogged())
    {
        m_errToShow.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(m_errToShow);
    }
}

TExceptDlg::~TExceptDlg()
{
}


// ---------------------------------------------------------------------------
//  TExceptDlg: Protected, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TBoolean TExceptDlg::bCreated()
{
    // We return our parent's car, which indicates if it set the focus
    const tCIDLib::TBoolean bRet = TDlgBox::bCreated();

    TPushButton*        pwndClose;
    TPushButton*        pwndCopy;
    TPushButton*        pwndDetails;
    TStaticImg*         pwndImg;
    TMultiEdit*         pwndMsg;

    CastChildWnd(*this, kCIDCtrls::ridDlg_ExceptDlg_Close, pwndClose);
    CastChildWnd(*this, kCIDCtrls::ridDlg_ExceptDlg_Copy, pwndCopy);
    CastChildWnd(*this, kCIDCtrls::ridDlg_ExceptDlg_Details, pwndDetails);
    CastChildWnd(*this, kCIDCtrls::ridDlg_ExceptDlg_Img, pwndImg);
    CastChildWnd(*this, kCIDCtrls::ridDlg_ExceptDlg_GenMsg, pwndMsg);

    // Set up the buttons with a click handler
    pwndClose->pnothRegisterHandler(this, &TExceptDlg::eClickHandler);
    pwndCopy->pnothRegisterHandler(this, &TExceptDlg::eClickHandler);
    pwndDetails->pnothRegisterHandler(this, &TExceptDlg::eClickHandler);

    // Set the title text into our caption bar
    strWndText(m_strTitleText);

    // Load the image
    pwndImg->SetAppImage(L"X Mark 2", tCIDCtrls::EAppImgSz::Large);

    //
    //  Size the multi-static text to fit the text, and then center it in the
    //  available area. The available area is the original size, which is set
    //  to the max.
    //
    TGraphWndDev gdevTmp(*pwndMsg);
    TArea areaOrg = pwndMsg->areaWnd();
    TArea areaText = gdevTmp.areaMLText(m_strGenMsg, areaOrg.c4Width());
    areaText.JustifyIn(areaOrg, tCIDLib::EHJustify::Left, tCIDLib::EVJustify::Center);
    pwndMsg->SetSizePos(areaText, kCIDLib::False);

    // Fill in the editor with the general message that we were given in the ctor.
    pwndMsg->strWndText(m_strGenMsg);

    return bRet;
}



// -------------------------------------------------------------------
//  Private, non-virtual methods
// -------------------------------------------------------------------
tCIDCtrls::EEvResponses TExceptDlg::eClickHandler(TButtClickInfo& wnotEvent)
{
    if (wnotEvent.widSource() == kCIDCtrls::ridDlg_ExceptDlg_Close)
    {
        EndDlg(wnotEvent.widSource());
    }
     else if (wnotEvent.widSource() == kCIDCtrls::ridDlg_ExceptDlg_Copy)
    {
        // Format it to text and put that on the clipboard
        TTextStringOutStream strmTar(2048UL);
        strmTar.eNewLineType(tCIDLib::ENewLineTypes::CRLF);

        TTime tmFmt;
        tmFmt.strDefaultFormat(L"%(M,2,0)/%(D,2,0) %(H,2,0):%(u,2,0):%(s,2,0)");
        m_errToShow.AdvFormat(strmTar, tmFmt);
        strmTar.Flush();

        TGUIClipboard gclipFmt(*this);
        gclipFmt.Clear();
        gclipFmt.StoreText(strmTar.strData());
    }
     else if (wnotEvent.widSource() == kCIDCtrls::ridDlg_ExceptDlg_Details)
    {
        TErrorDlg dlgDetails(*this, m_errToShow);
    }
    return tCIDCtrls::EEvResponses::Handled;
}





// ---------------------------------------------------------------------------
//  CLASS: TErrorDlg
// PREFIX: dlgb
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TErrorDlg: Constructors and Destructors
// ---------------------------------------------------------------------------
TErrorDlg::TErrorDlg(const  TWindow&    wndOwner
                    , const TError&     errToShow) :

    m_errToShow(errToShow)
{
    c4RunDlg(wndOwner, facCIDCtrls(), kCIDCtrls::ridDlg_TError);
}

TErrorDlg::~TErrorDlg()
{
}


// ---------------------------------------------------------------------------
//  TErrorDlg: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TErrorDlg::bCreated()
{
    // We return our parent's return, which indicates if it set focus
    const tCIDLib::TBoolean bRet = TDlgBox::bCreated();

    // A local text output stream to do a little formatting work
    TTextStringOutStream strmTmp(256);

    //
    //  Load up the information from the exception object into the widgets
    //  that display them.
    //
    pwndChildById(kCIDCtrls::ridDlg_TError_FacName)->strWndText(m_errToShow.strFacName());

    // Format the severity and class and set them
    strmTmp << m_errToShow.eSeverity() << kCIDLib::FlushIt;
    pwndChildById(kCIDCtrls::ridDlg_TError_Severity)->strWndText(strmTmp.strData());

    strmTmp.Reset();
    strmTmp << m_errToShow.eClass() << kCIDLib::FlushIt;
    pwndChildById(kCIDCtrls::ridDlg_TError_Class)->strWndText(strmTmp.strData());

    // Do the error numbres
    strmTmp.Reset();
    strmTmp << m_errToShow.errcId() << L"/" << m_errToShow.errcKrnlId()
            << L"/" << m_errToShow.errcHostId() << kCIDLib::FlushIt;
    pwndChildById(kCIDCtrls::ridDlg_TError_ErrNums)->strWndText(strmTmp.strData());

    // And do the line/column
    strmTmp.Reset();
    strmTmp << m_errToShow.strFileName() << L"/"
            << m_errToShow.c4LineNum() << kCIDLib::FlushIt;
    pwndChildById(kCIDCtrls::ridDlg_TError_LineCol)->strWndText(strmTmp.strData());

    strmTmp.Reset();
    strmTmp << m_errToShow.strErrText();
    if (m_errToShow.bHasAuxText())
        strmTmp << L"\n\n" << m_errToShow.strAuxText();
    strmTmp.Flush();
    pwndChildById(kCIDCtrls::ridDlg_TError_ErrText)->strWndText(strmTmp.strData());

    // Set our title text
    TString strTitle
    (
        kCtrlsMsgs::midDlg_TError_Title, facCIDCtrls(), m_errToShow.strProcess()
    );
    strWndText(strTitle);

    //
    //  Point the click events of our buttons at our own private event
    //  handler.
    //
    pwndChildAs<TPushButton>(kCIDCtrls::ridDlg_Close)->pnothRegisterHandler
    (
        this, &TErrorDlg::eClickHandler
    );
    pwndChildAs<TPushButton>(kCIDCtrls::ridDlg_TError_Copy)->pnothRegisterHandler
    (
        this, &TErrorDlg::eClickHandler
    );

    return bRet;
}


// ---------------------------------------------------------------------------
//  TErrorDlg: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDCtrls::EEvResponses TErrorDlg::eClickHandler(TButtClickInfo& wnotEvent)
{
    // Just exit on the close button
    if (wnotEvent.widSource() == kCIDCtrls::ridDlg_Close)
    {
        EndDlg(kCIDCtrls::widCancel);
    }
     else if (wnotEvent.widSource() == kCIDCtrls::ridDlg_TError_Copy)
    {
        // Format it to text and put that on the clipboard
        TTextStringOutStream strmTar(4096UL);
        strmTar.eNewLineType(tCIDLib::ENewLineTypes::CRLF);

        TTime tmFmt;
        tmFmt.strDefaultFormat(L"%(M,2,0)/%(D,2,0) %(H,2,0):%(u,2,0):%(s,2,0)");
        m_errToShow.AdvFormat(strmTar, tmFmt);
        strmTar.Flush();

        TGUIClipboard gclipFmt(*this);
        gclipFmt.Clear();
        gclipFmt.StoreText(strmTar.strData());
    }

    return tCIDCtrls::EEvResponses::Handled;
}


