//
// FILE NAME: CIDCtrls_WndState.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/07/2015
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
//  This file implements the window state classes
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
//  CLASS: TWndState
// PREFIX: wstate
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  Constructors and Destructor
// ---------------------------------------------------------------------------
TWndState::TWndState(const TString& strName) :

    m_bDefButtIfNeeded(kCIDLib::False)
    , m_bFocusIfNeeded(kCIDLib::False)
    , m_strName(strName)
    , m_widFocus(kCIDCtrls::widInvalid)
    , m_widDefButt(kCIDCtrls::widInvalid)
{
}

TWndState::TWndState(const TWndState& wstateSrc) :

    m_bDefButtIfNeeded(wstateSrc.m_bDefButtIfNeeded)
    , m_bFocusIfNeeded(wstateSrc.m_bFocusIfNeeded)
    , m_fcolMenuDisable(wstateSrc.m_fcolMenuDisable)
    , m_fcolMenuEnable(wstateSrc.m_fcolMenuEnable)
    , m_fcolWndDisable(wstateSrc.m_fcolWndDisable)
    , m_fcolWndEnable(wstateSrc.m_fcolWndEnable)
    , m_strName(wstateSrc.m_strName)
    , m_widFocus(wstateSrc.m_widFocus)
    , m_widDefButt(wstateSrc.m_widDefButt)
{
}

TWndState::~TWndState()
{
}


// ---------------------------------------------------------------------------
//  Public operators
// ---------------------------------------------------------------------------
TWndState& TWndState::operator=(const TWndState& wstateSrc)
{
    if (&wstateSrc != this)
    {
        m_bDefButtIfNeeded  = wstateSrc.m_bDefButtIfNeeded;
        m_bFocusIfNeeded    = wstateSrc.m_bFocusIfNeeded;

        m_fcolMenuDisable   = wstateSrc.m_fcolMenuDisable;
        m_fcolMenuEnable    = wstateSrc.m_fcolMenuEnable;
        m_fcolWndDisable    = wstateSrc.m_fcolWndDisable;
        m_fcolWndEnable     = wstateSrc.m_fcolWndEnable;
        m_strName           = wstateSrc.m_strName;
        m_widFocus          = wstateSrc.m_widFocus;
        m_widDefButt        = wstateSrc.m_widDefButt;
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  Public, non-virtual methods
// ---------------------------------------------------------------------------

// Add ids to the enable/disable lists
tCIDLib::TVoid TWndState::AddMenuDisable(const tCIDLib::TResId ridToAdd)
{
    m_fcolMenuDisable.c4AddElement(ridToAdd);
}


tCIDLib::TVoid TWndState::AddMenuEnable(const tCIDLib::TResId ridToAdd)
{
    m_fcolMenuEnable.c4AddElement(ridToAdd);
}


tCIDLib::TVoid TWndState::AddWndDisable(const tCIDLib::TResId ridToAdd)
{
    m_fcolWndDisable.c4AddElement(ridToAdd);
}


tCIDLib::TVoid TWndState::AddWndEnable(const tCIDLib::TResId ridToAdd)
{
    m_fcolWndEnable.c4AddElement(ridToAdd);
}


//
//  This is where the work is done. A window that wants to apply this state
//  calls this method and we do all of the operations configured.
//
tCIDLib::TVoid TWndState::ApplyState(TWindow& wndTar, TMenu* const pmenuTar)
{
    tCIDLib::TCard4 c4Count;
    tCIDCtrls::TWndHandle hwndTar = wndTar.hwndSafe();

    // Do menu ops if we got one
    if (pmenuTar)
    {
        c4Count = m_fcolMenuDisable.c4ElemCount();
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
            pmenuTar->SetItemEnable(m_fcolMenuDisable[c4Index], kCIDLib::False);

        c4Count = m_fcolMenuEnable.c4ElemCount();
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
            pmenuTar->SetItemEnable(m_fcolMenuEnable[c4Index], kCIDLib::True);
    }

    // Do any window disables
    tCIDLib::TBoolean bFocusDisabled = kCIDLib::False;
    tCIDCtrls::TWndHandle hwndFocus = ::GetFocus();
    tCIDCtrls::TWndHandle hwndCur;
    c4Count = m_fcolWndDisable.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        hwndCur = ::GetDlgItem(hwndTar, m_fcolWndDisable[c4Index]);
        if (hwndCur)
            ::EnableWindow(hwndCur, FALSE);

        if (!bFocusDisabled)
            bFocusDisabled = (hwndCur == hwndFocus);
    }

    // Do any window enables
    c4Count = m_fcolWndEnable.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        hwndCur = ::GetDlgItem(hwndTar, m_fcolWndEnable[c4Index]);
        if (hwndCur)
            ::EnableWindow(hwndCur, TRUE);
    }

    //
    //  If we got a focus item, then we may have to move the focus. They can ask
    //  that we only move the focus if the current focus is disabled.
    //
    if (m_widFocus != kCIDCtrls::widInvalid)
    {
        if (!m_bFocusIfNeeded || (m_bFocusIfNeeded && bFocusDisabled))
        {
            hwndCur = ::GetDlgItem(hwndTar, m_widFocus);
            if (hwndCur)
                ::SetFocus(hwndCur);
        }
    }

    //
    //  If we got a default button, then we may have to move that. They can
    //  ask that we only do this if the current default button got disabled.
    //
    if (m_widDefButt != kCIDCtrls::widInvalid)
    {
        hwndCur = kCIDCtrls::hwndInvalid;
        tCIDCtrls::TMsgResult mresDef = ::SendMessage(hwndTar, DM_GETDEFID, 0, 0);
        if ((mresDef >> 16) == DC_HASDEFID)
            hwndCur = ::GetDlgItem(hwndTar, mresDef & 0xFFFF);

        //
        //  If we are doing it unconditionally, or conditionally and we have a current
        //  defaul and it's not enabled.
        //
        if (!m_bDefButtIfNeeded
        ||  (m_bDefButtIfNeeded && hwndCur && !::IsWindowEnabled(hwndCur)))
        {
            ::SendMessage(hwndTar, DM_SETDEFID, m_widDefButt, 0);
        }
    }

    // Force a menu redraw on the target window if we got a menu
    if (pmenuTar)
        ::DrawMenuBar(hwndTar);
}


// We just flip our enable/disable lists
tCIDLib::TVoid TWndState::Invert()
{
    tCIDLib::TCardList fcolTmp;

    fcolTmp = m_fcolMenuDisable;
    m_fcolMenuDisable = m_fcolMenuEnable;
    m_fcolMenuEnable = fcolTmp;

    fcolTmp = m_fcolWndDisable;
    m_fcolWndDisable = m_fcolWndEnable;
    m_fcolWndEnable = fcolTmp;
}


// Remove existing enable/disable entries
tCIDLib::TVoid TWndState::RemoveMenuDisable(const tCIDLib::TResId ridToRemove)
{
    m_fcolMenuDisable.bDeleteIfPresent(ridToRemove);
}

tCIDLib::TVoid TWndState::RemoveMenuEnable(const tCIDLib::TResId ridToRemove)
{
    m_fcolMenuEnable.bDeleteIfPresent(ridToRemove);
}

tCIDLib::TVoid TWndState::RemoveWndDisable(const tCIDLib::TResId  ridToRemove)
{
    m_fcolWndDisable.bDeleteIfPresent(ridToRemove);
}

tCIDLib::TVoid TWndState::RemoveWndEnable(const tCIDLib::TResId  ridToRemove)
{
    m_fcolWndEnable.bDeleteIfPresent(ridToRemove);
}


// Put us back to default state
tCIDLib::TVoid TWndState::Reset(const TString& strName)
{
    m_bDefButtIfNeeded  = kCIDLib::False;
    m_bFocusIfNeeded    = kCIDLib::False;
    m_widFocus          = kCIDCtrls::widInvalid;
    m_widDefButt        = kCIDCtrls::widInvalid;

    m_fcolMenuDisable.RemoveAll();
    m_fcolMenuEnable.RemoveAll();
    m_fcolWndDisable.RemoveAll();
    m_fcolWndEnable.RemoveAll();

    m_strName = strName;
}


// Frames/dlgs manage states by name, so we have to expose that
const TString& TWndState::strName() const
{
    return m_strName;
}


// Set the control to put the focus one
tCIDLib::TVoid
TWndState::SetFocus(const   tCIDLib::TResId     ridToSet
                    , const tCIDLib::TBoolean   bIfNeeded)
{
    m_bFocusIfNeeded = bIfNeeded;
    m_widFocus = ridToSet;
}


// Set the id of the default button
tCIDLib::TVoid
TWndState::SetDefButt(const tCIDLib::TResId     ridToSet
                    , const tCIDLib::TBoolean   bIfNeeded)
{
    m_bDefButtIfNeeded = bIfNeeded;
    m_widDefButt = ridToSet;
}

