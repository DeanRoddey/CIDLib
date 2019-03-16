//
// FILE NAME: CIDWUtils_GetTextDlg.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/26/2015
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
//  Implements some simple dialogs to get a line of text from the user.
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
#include    "CIDWUtils_.hpp"
#include    "CIDWUtils_GetTextDlg_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TGetTextDlg, TDlgBox)
RTTIDecls(TGetTextDlg2, TDlgBox)



// ---------------------------------------------------------------------------
//   CLASS: TGetTextDlg
//  PREFIX: dlg
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TGetTextDlg: Constructors and Destructor
// ---------------------------------------------------------------------------
TGetTextDlg::TGetTextDlg()
{
}

TGetTextDlg::~TGetTextDlg()
{
}


// ---------------------------------------------------------------------------
//  TGetTextDlg: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TGetTextDlg::bRun(  const   TWindow&            wndOwner
                    , const TString&            strTitle
                    , const TString&            strOrgText
                    ,       TString&            strNewText)
{
    // Store the incoming info until we can set it
    m_strTitle  = strTitle;
    m_strText   = strOrgText;

    const tCIDLib::TCard4 c4Res = c4RunDlg
    (
        wndOwner, facCIDWUtils(), kCIDWUtils::ridDlg_GetText
    );

    if (c4Res == kCIDWUtils::ridDlg_GetText_Save)
    {
        strNewText = m_strText;
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


// ---------------------------------------------------------------------------
//  TGetTextDlg: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TGetTextDlg::bCreated()
{
    // Now call our parent first and return his return
    const tCIDLib::TBoolean bRes = TParent::bCreated();

    // Load up the text values we got
    strWndText(m_strTitle);
    pwndChildById(kCIDWUtils::ridDlg_GetText_Text)->strWndText(m_strText);

    // Register a handler for the buttons
    pwndChildAs<TPushButton>(kCIDWUtils::ridDlg_GetText_Save)->pnothRegisterHandler
    (
        this, &TGetTextDlg::eClickHandler
    );
    pwndChildAs<TPushButton>(kCIDWUtils::ridDlg_GetText_Cancel)->pnothRegisterHandler
    (
        this, &TGetTextDlg::eClickHandler
    );
    return bRes;
}


// ---------------------------------------------------------------------------
//  TGetTextDlg: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDCtrls::EEvResponses TGetTextDlg::eClickHandler(TButtClickInfo& wnotEvent)
{
    // If saving, store the text away before we exit
    if (wnotEvent.widSource() == kCIDWUtils::ridDlg_GetText_Save)
        m_strText = pwndChildById(kCIDWUtils::ridDlg_GetText_Text)->strWndText();

    EndDlg(wnotEvent.widSource());
    return tCIDCtrls::EEvResponses::Handled;
}




// ---------------------------------------------------------------------------
//   CLASS: TGetTextDlg2
//  PREFIX: dlg
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TGetTextDlg2: Constructors and Destructor
// ---------------------------------------------------------------------------
TGetTextDlg2::TGetTextDlg2() :

    m_bSecret(kCIDLib::False)
    , m_pregxFmt(nullptr)
{
}

TGetTextDlg2::~TGetTextDlg2()
{
}


// ---------------------------------------------------------------------------
//  TGetTextDlg2: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TGetTextDlg2::bRun( const   TWindow&            wndOwner
                    , const TString&            strTitle
                    , const TString&            strInstruct
                    , const TString&            strOrgText
                    ,       TString&            strNewText
                    , const TString&            strRegEx
                    , const tCIDLib::TBoolean   bSecret)
{
    // Store the incoming text until we can set it
    m_bSecret = bSecret;
    m_strInstruct = strInstruct;
    m_strTitle  = strTitle;
    m_strText   = strOrgText;

    if (!strRegEx.bIsEmpty())
        m_pregxFmt = new TRegEx(strRegEx);

    const tCIDLib::TCard4 c4Res = c4RunDlg
    (
        wndOwner, facCIDWUtils(), kCIDWUtils::ridDlg_GetText2
    );

    if (c4Res == kCIDWUtils::ridDlg_GetText2_Save)
    {
        strNewText = m_strText;
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


// ---------------------------------------------------------------------------
//  TGetTextDlg2: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TGetTextDlg2::bCreated()
{
    // Now call our parent first and return his return
    const tCIDLib::TBoolean bRes = TParent::bCreated();

    // Load up the text values we got
    strWndText(m_strTitle);
    pwndChildById(kCIDWUtils::ridDlg_GetText2_Instruct)->strWndText(m_strInstruct);
    pwndChildById(kCIDWUtils::ridDlg_GetText2_Text)->strWndText(m_strText);

    // Register a handler for the buttons
    pwndChildAs<TPushButton>(kCIDWUtils::ridDlg_GetText2_Save)->pnothRegisterHandler
    (
        this, &TGetTextDlg2::eClickHandler
    );
    pwndChildAs<TPushButton>(kCIDWUtils::ridDlg_GetText2_Cancel)->pnothRegisterHandler
    (
        this, &TGetTextDlg2::eClickHandler
    );
    return bRes;
}


// Watch for the entry field if we were asked to secret mode, and update him
tCIDLib::TVoid TGetTextDlg2::ModDlgItem(TDlgItem& dlgiToMod)
{
    if (m_bSecret)
    {
        if (dlgiToMod.ridChild() == kCIDWUtils::ridDlg_GetText2_Text)
            dlgiToMod.AddHint(kCIDCtrls::strHint_Password);
    }
}


// ---------------------------------------------------------------------------
//  TGetTextDlg2: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDCtrls::EEvResponses TGetTextDlg2::eClickHandler(TButtClickInfo& wnotEvent)
{
    //
    //  If saving, store the text away before we exit. If we have a regular expression
    //  then validate it first.
    //
    if (wnotEvent.widSource() == kCIDWUtils::ridDlg_GetText2_Save)
    {
        m_strText = pwndChildById(kCIDWUtils::ridDlg_GetText2_Text)->strWndText();

        if (m_pregxFmt)
        {
            if (!m_pregxFmt->bFullyMatches(m_strText, kCIDLib::False))
            {
                TErrBox msgbErr(L"The value is not in the required format");
                msgbErr.ShowIt(*this);
                return tCIDCtrls::EEvResponses::Handled;
            }
        }
        EndDlg(kCIDWUtils::ridDlg_GetText2_Save);
    }
     else if (wnotEvent.widSource() == kCIDWUtils::ridDlg_GetText2_Cancel)
    {
        EndDlg(kCIDWUtils::ridDlg_GetText2_Cancel);
    }
    return tCIDCtrls::EEvResponses::Handled;
}

