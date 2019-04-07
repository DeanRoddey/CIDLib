//
// FILE NAME: CIDCtrls_ListWnd.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/14/2015
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
//  This file implements the base class for simple list type controls.
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
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TListChangeInfo,TCtrlNotify)
RTTIDecls(TListWnd, TStdCtrlWnd)




// ---------------------------------------------------------------------------
//   CLASS: TListChangeInfo
//  PREFIX: wnot
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TListChangeInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TListChangeInfo::TListChangeInfo(const  tCIDCtrls::EListEvents  eEvent
                                , const tCIDLib::TCard4         c4Index
                                , const tCIDLib::TCard4         c4Id
                                , const tCIDLib::TCard4         c4ColIndex
                                , const TWindow&                wndSrc) :
    TCtrlNotify(wndSrc)
    , m_c4ColInd(c4ColIndex)
    , m_c4Index(c4Index)
    , m_c4Id(c4Id)
    , m_eEvent(eEvent)
{
}

TListChangeInfo::TListChangeInfo(const TListChangeInfo& wnotSrc) :

    TCtrlNotify(wnotSrc)
    , m_c4ColInd(wnotSrc.m_c4ColInd)
    , m_c4Index(wnotSrc.m_c4Index)
    , m_c4Id(wnotSrc.m_c4Id)
    , m_eEvent(wnotSrc.m_eEvent)
{
}

TListChangeInfo::~TListChangeInfo()
{
}


// ---------------------------------------------------------------------------
//  TListChangeInfo: Public operators
// ---------------------------------------------------------------------------
TListChangeInfo& TListChangeInfo::operator=(const TListChangeInfo& wnotSrc)
{
    if (this != &wnotSrc)
    {
        TParent::operator=(wnotSrc);

        m_c4ColInd = wnotSrc.m_c4ColInd;
        m_c4Index = wnotSrc.m_c4Index;
        m_c4Id = wnotSrc.m_c4Id;
        m_eEvent = wnotSrc.m_eEvent;
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TListChangeInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4 TListChangeInfo::c4ColIndex() const
{
    return m_c4ColInd;
}


tCIDLib::TCard4 TListChangeInfo::c4Index() const
{
    return m_c4Index;
}


tCIDLib::TCard4 TListChangeInfo::c4Id() const
{
    return m_c4Id;
}


tCIDCtrls::EListEvents TListChangeInfo::eEvent() const
{
    return m_eEvent;
}





// ---------------------------------------------------------------------------
//  CLASS: TListWnd
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// TListWnd: Destructor
// ---------------------------------------------------------------------------
TListWnd::~TListWnd()
{
}


// ---------------------------------------------------------------------------
//  TListWnd: Public, virtual methods
// ---------------------------------------------------------------------------

// If not overridden, say not
tCIDLib::TBoolean TListWnd::bIsMultiSel() const
{
    return kCIDLib::False;
}


// ---------------------------------------------------------------------------
//  TListWnd: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Return the per-item id for the currently selected item, if any. If no selection
//  it will throw because the returned is max card. This we can do in terms of other
//  virtuals.
//
tCIDLib::TCard4 TListWnd::c4CurItemId() const
{
    const tCIDLib::TCard4 c4CurInd = c4CurItem();
    if (c4CurInd == kCIDLib::c4MaxCard)
        return c4CurInd;
    return c4IndexToId(c4CurInd);
}


//
//  We can do this in terms of virtual methods, so it will work for all derivatives. We
//  call the virtual (fault tolerant) version of this. If it doesn't work we throw.
//
tCIDLib::TCard4
TListWnd::c4SelectByText(const  TString&            strSel
                        , const tCIDLib::TBoolean   bForceEvent)
{
    tCIDLib::TCard4 c4Ret;
    if (!bSelectByText(strSel, c4Ret, bForceEvent))
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcList_TextNotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
            , strSel
            , TCardinal(widThis())
        );
    }

    // It worked, so return the new index
    return c4Ret;
}


//
//  Remove the currently selected item, if any. We can do this in terms of available virtual
//  methods on behalf of all derivatives.
//
tCIDLib::TVoid TListWnd::RemoveCurrent(const tCIDLib::TBoolean bSelectAnother)
{
    const tCIDLib::TCard4 c4Index = c4CurItem();
    if (c4Index != kCIDLib::c4MaxCard)
        RemoveAt(c4Index, bSelectAnother);
}


// ---------------------------------------------------------------------------
// TListWnd: Hidden constructors
// ---------------------------------------------------------------------------
TListWnd::TListWnd(const TString& strNotificationId) :

    TStdCtrlWnd()
    , m_nidChangeIdToUse(strNotificationId)
{
}


// ---------------------------------------------------------------------------
//  TListWnd: Protected, non-virtual methods
// ---------------------------------------------------------------------------

//
//  A helper for derived classes to call when they want to send out a list event. These
//  are all done synchronously.
//
tCIDLib::TVoid
TListWnd::SendListEvent(const   tCIDCtrls::EListEvents  eEvent
                        , const tCIDLib::TCard4         c4Index
                        , const tCIDLib::TCard4         c4Id
                        , const tCIDLib::TCard4         c4ColIndex
                        , const tCIDLib::TBoolean       bSync)
{

    if (bSync)
    {
        TListChangeInfo wnotToSend(eEvent, c4Index, c4Id, c4ColIndex, *this);
        SendSyncNotify(wnotToSend, m_nidChangeIdToUse);
    }
     else
    {
        TListChangeInfo* pwnotToSend = new TListChangeInfo
        (
            eEvent, c4Index, c4Id, c4ColIndex, *this
        );
        SendAsyncNotify(pwnotToSend, m_nidChangeIdToUse);
    }
}
