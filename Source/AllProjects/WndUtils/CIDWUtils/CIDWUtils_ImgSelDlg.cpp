//
// FILE NAME: CIDWUtils_ImgSelDlg.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/23/2006
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
//  This file implements a dialog that let's the user choose an image from
//  a small set of provided images.
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
#include    "CIDWUtils_.hpp"
#include    "CIDWUtils_ImgSelDlg_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TImgSelDlg,TDlgBox)



// ---------------------------------------------------------------------------
//   CLASS: TImgSelDlg
//  PREFIX: dlg
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TImgSelDlg: Constructors and Destructor
// ---------------------------------------------------------------------------
TImgSelDlg::TImgSelDlg() :

    m_c4SelIndex(0)
    , m_pcolList(nullptr)
    , m_pwndCancel(nullptr)
    , m_pwndLegend(nullptr)
    , m_pwndCurImage(nullptr)
    , m_pwndNext(nullptr)
    , m_pwndPrev(nullptr)
    , m_pwndSelect(nullptr)
{
}

TImgSelDlg::~TImgSelDlg()
{
}


// ---------------------------------------------------------------------------
//  TImgSelDlg: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TImgSelDlg::bRun(const  TWindow&                wndOwner
                , const TRefVector<TCIDImage>&  colList
                , const TString&                strTitle
                ,       tCIDLib::TCard4&        c4SelIndex)
{
    // If the list is empty, return false now
    if (colList.bIsEmpty())
        return kCIDLib::False;

    // Save incoming stuff for later user
    m_pcolList = &colList;
    m_strTitle = strTitle;

    const tCIDLib::TCard4 c4Res = c4RunDlg
    (
        wndOwner, facCIDWUtils(), kCIDWUtils::ridDlg_ImgSel
    );

    if (c4Res == kCIDWUtils::ridDlg_ImgSel_Select)
    {
        c4SelIndex = m_c4SelIndex;
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


// ---------------------------------------------------------------------------
//  TImgSelDlg: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TImgSelDlg::bCreated()
{
    // Call our parent first
    const tCIDLib::TBoolean bRet = TParent::bCreated();

    // Set our title text as provided by the caller
    strWndText(m_strTitle);

    // First get typed pointers to the widget we always keep
    CastChildWnd(*this, kCIDWUtils::ridDlg_ImgSel_Cancel, m_pwndCancel);
    CastChildWnd(*this, kCIDWUtils::ridDlg_ImgSel_Legend, m_pwndLegend);
    CastChildWnd(*this, kCIDWUtils::ridDlg_ImgSel_CurImg, m_pwndCurImage);
    CastChildWnd(*this, kCIDWUtils::ridDlg_ImgSel_Next, m_pwndNext);
    CastChildWnd(*this, kCIDWUtils::ridDlg_ImgSel_Prev, m_pwndPrev);
    CastChildWnd(*this, kCIDWUtils::ridDlg_ImgSel_Select, m_pwndSelect);

    // Set the art to keep the AR while fitting
    m_pwndCurImage->ePlacement(tCIDGraphDev::EPlacement::FitAR);

    // Register our handlers
    m_pwndCancel->pnothRegisterHandler(this, &TImgSelDlg::eClickHandler);
    m_pwndNext->pnothRegisterHandler(this, &TImgSelDlg::eClickHandler);
    m_pwndPrev->pnothRegisterHandler(this, &TImgSelDlg::eClickHandler);
    m_pwndSelect->pnothRegisterHandler(this, &TImgSelDlg::eClickHandler);

    // Load the first image
    TGraphWndDev gdevCompat(*this);
    m_pwndCurImage->SetImage(gdevCompat, *m_pcolList->pobjAt(m_c4SelIndex));
    UpdateLegend();

    return bRet;
}


// ---------------------------------------------------------------------------
//  TImgSelDlg: Private, non-virutal methods
// ---------------------------------------------------------------------------
tCIDCtrls::EEvResponses TImgSelDlg::eClickHandler(TButtClickInfo& wnotEvent)
{
    if (wnotEvent.widSource() == kCIDWUtils::ridDlg_ImgSel_Cancel)
    {
        EndDlg(kCIDWUtils::ridDlg_ImgSel_Cancel);
    }
     else if (wnotEvent.widSource() == kCIDWUtils::ridDlg_ImgSel_Next)
    {
        const tCIDLib::TCard4 c4ImgCnt = m_pcolList->c4ElemCount();
        if (c4ImgCnt > 1)
        {
            m_c4SelIndex++;
            if (m_c4SelIndex >= c4ImgCnt)
                m_c4SelIndex = 0;

            TGraphWndDev gdevCompat(*this);
            m_pwndCurImage->SetImage(gdevCompat, *m_pcolList->pobjAt(m_c4SelIndex));
            UpdateLegend();
        }
    }
     else if (wnotEvent.widSource() == kCIDWUtils::ridDlg_ImgSel_Prev)
    {
        const tCIDLib::TCard4 c4ImgCnt = m_pcolList->c4ElemCount();
        if (c4ImgCnt > 1)
        {
            if (m_c4SelIndex)
                m_c4SelIndex--;
            else
                m_c4SelIndex = c4ImgCnt - 1;

            TGraphWndDev gdevCompat(*this);
            m_pwndCurImage->SetImage(gdevCompat, *m_pcolList->pobjAt(m_c4SelIndex));
            UpdateLegend();
        }
    }
     else if (wnotEvent.widSource() == kCIDWUtils::ridDlg_ImgSel_Select)
    {
        EndDlg(kCIDWUtils::ridDlg_ImgSel_Select);
    }
    return tCIDCtrls::EEvResponses::Handled;
}


tCIDLib::TVoid TImgSelDlg::UpdateLegend()
{
    TString strLegend;
    strLegend.AppendFormatted(m_c4SelIndex + 1);
    strLegend.Append(L'/');
    strLegend.AppendFormatted(m_pcolList->c4ElemCount());

    const TCIDImage* pimgCur = m_pcolList->pobjAt(m_c4SelIndex);
    strLegend.Append(L"  Size=");
    strLegend.AppendFormatted(pimgCur->c4Width());
    strLegend.Append(L'x');
    strLegend.AppendFormatted(pimgCur->c4Height());

    strLegend.Append(L"  (Bytes=");
    strLegend.AppendFormatted(pimgCur->c4ImageSz());
    strLegend.Append(L")");


    m_pwndLegend->strWndText(strLegend);
}

