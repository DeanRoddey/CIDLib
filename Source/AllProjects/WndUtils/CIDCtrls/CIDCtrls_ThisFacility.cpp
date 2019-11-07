//
// FILE NAME: CIDCtrls_ThisFacility.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/27/1997
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
//  This file implements the facility class for this facility. It is a
//  derivative (as all facility objects are) of TFacility.
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
#include    "CIDKernel_InternalHelpers_.hpp"
#include    <Shlobj.h>



// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TFacCIDCtrls, TGUIFacility)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDCtrls_ThisFacility
{
    // -----------------------------------------------------------------------
    //  This is used to fault in the special text file when/if it is required.
    // -----------------------------------------------------------------------
    TAtomicFlag atomInitDone;


    // -----------------------------------------------------------------------
    //  These are used in the window enumeration callback, so that we can pass
    //  info through.
    // -----------------------------------------------------------------------
    struct TFindWndText
    {
                tCIDLib::TBoolean       bStartsWith;
        const   tCIDLib::TCh*           pszText;
        const   tCIDLib::TCh*           pszClass;
                tCIDLib::TCard4         c4TextLen;
                tCIDCtrls::TWndHandle   hwndFound;
                tCIDLib::TProcessId     pidTarget;
                tCIDLib::TCh*           pchTmpBuf;
                tCIDLib::TCard4         c4TmpBufSz;
    };

    struct TFindWndOwnedPopup
    {
        tCIDCtrls::TWndHandle   hwndFound;
        tCIDCtrls::TWndHandle   hwndRootOwner;
        tCIDLib::TBoolean       bModalOnly;
    };

    struct TFindWndFiltered
    {
                tCIDCtrls::TWndHandle   hwndFound;
                tCIDLib::TProcessId     pidTarget;
                tCIDLib::TBoolean       bOwned;
                tCIDCtrls::TWndHandle   hwndOwner;
        const   tCIDLib::TCh*           pszClass;
                tCIDLib::TCard4         c4DontWantStyles;
                tCIDLib::TCard4         c4WantStyles;
                tCIDLib::TCard4         c4DontWantExStyles;
                tCIDLib::TCard4         c4WantExStyles;

                tCIDLib::TCard4         c4FoundStyles;
                tCIDLib::TCard4         c4FoundExStyles;
    };
}


// ---------------------------------------------------------------------------
//  Local helpers
// ---------------------------------------------------------------------------
// Checks to see if the passed window is of the indicated window class.
static tCIDLib::TBoolean
bCheckClass(HWND hwndTarget, const tCIDLib::TCh* const pszToCheck)
{
    const tCIDLib::TCard4 c4MaxChars = 1024;
    tCIDLib::TCh achBuf[c4MaxChars + 1];

    ::GetClassName(hwndTarget, achBuf, c4MaxChars);
    return TRawStr::bCompareStr(achBuf, pszToCheck);
}


static tCIDLib::TBoolean bDevErrToIgnore()
{
    tCIDLib::TCard4 c4LastErr = ::GetLastError();
    if (!c4LastErr
    ||  (c4LastErr == ERROR_ACCESS_DENIED)
    ||  (c4LastErr == ERROR_INVALID_HANDLE)
    ||  (c4LastErr == ERROR_INVALID_WINDOW_HANDLE))
    {
        return kCIDLib::True;
    }
    TKrnlError::SetLastHostError(c4LastErr);
    return kCIDLib::False;
}


//
//  Callbacks used in some monitor enumeration calls below.
//
struct TMonEnumInfo
{
    const tCIDLib::TCh* pszDevName;
    HMONITOR            hmonFound;

};

//
//  Given a monitor handle, this guy will query the area, and optionally adjust
//  for any docked desktop tools, returning the available area.
//
static tCIDLib::TVoid
QueryMonitorArea(HMONITOR hMon, TArea& areaToFill, const tCIDLib::TBoolean bAvailOnly)
{
    // Get the monitor info about this monitor
    MONITORINFOEX MonInfo;
    MonInfo.cbSize = sizeof(MONITORINFOEX);
    if (::GetMonitorInfo(hMon, &MonInfo))
    {
        if (bAvailOnly)
            areaToFill.FromRectl(*reinterpret_cast<tCIDLib::THostRectl*>(&MonInfo.rcWork));
        else
            areaToFill.FromRectl(*reinterpret_cast<tCIDLib::THostRectl*>(&MonInfo.rcMonitor));
    }
     else
    {
        // We need to return something, so do the desktop area
        areaToFill = TWindow::wndDesktop().areaWndSize();
    }
}

static BOOL CALLBACK MonitorEnumCB(HMONITOR hMon, HDC, RECT* pRect, LPARAM pData)
{
    // Query the monitor info for this monitor
    MONITORINFOEX MonInfo;
    MonInfo.cbSize = sizeof(MONITORINFOEX);

    if (::GetMonitorInfo(hMon, &MonInfo))
    {
        TMonEnumInfo* pInfo = reinterpret_cast<TMonEnumInfo*>(pData);

        if (TRawStr::bCompareStrI(pInfo->pszDevName, MonInfo.szDevice))
        {
            // Store the monitor and stop looking
            pInfo->hmonFound = hMon;
            return FALSE;
        }
    }

    // Keep looking
    return TRUE;
}

static BOOL CALLBACK MonitorEnumCB2(HMONITOR hMon, HDC, RECT* pRect, LPARAM pData)
{
    // Query the monitor info for this monitor
    MONITORINFOEX MonInfo;
    MonInfo.cbSize = sizeof(MONITORINFOEX);

    if (::GetMonitorInfo(hMon, &MonInfo))
    {
        // Get the physical monitor size, and the available area
        TArea areaPhys;
        ::QueryMonitorArea(hMon, areaPhys, kCIDLib::False);

        TArea areaAvail;
        ::QueryMonitorArea(hMon, areaAvail, kCIDLib::True);

        reinterpret_cast<TCollection<TGUIMonInfo>*>(pData)->objAdd
        (
            TGUIMonInfo
            (
                TString(MonInfo.szDevice)
                , areaPhys
                , areaAvail
                , MonInfo.dwFlags & MONITORINFOF_PRIMARY
            )
        );
    }

    // Keep looking
    return TRUE;
}


//
//  Various callbacks for the window enumeration functions of Windows, to
//  help us find various types of windows.
//
static BOOL CALLBACK bEnumWndText(HWND hwndTarget, LPARAM lParam)
{
    // Cast the data parm to its correct type
    CIDCtrls_ThisFacility::TFindWndText* pFindData
                            = (CIDCtrls_ThisFacility::TFindWndText*)lParam;

    // Get the text of this window
    ::GetWindowText(hwndTarget, pFindData->pchTmpBuf, pFindData->c4TmpBufSz);

    //
    //  If this window's text matches our target, then store the handle and
    //  stop. If the target PID is set, then make sure that the window
    //  belongs to the indicated process before we accept it.
    //
    if (pFindData->bStartsWith)
    {
        if (TRawStr::bCompareStrN(  pFindData->pszText
                                    , pFindData->pchTmpBuf
                                    , pFindData->c4TextLen))
        {
            if (pFindData->pidTarget != kCIDLib::pidInvalid)
            {
                tCIDLib::TCard4 c4Id;
                ::GetWindowThreadProcessId(hwndTarget, &c4Id);

                if (c4Id == pFindData->pidTarget)
                {
                    if (!pFindData->pszClass || bCheckClass(hwndTarget, pFindData->pszClass))
                        pFindData->hwndFound = hwndTarget;
                    return 0;
                }
            }
             else
            {
                if (!pFindData->pszClass || bCheckClass(hwndTarget, pFindData->pszClass))
                    pFindData->hwndFound = hwndTarget;
                return 0;
            }
        }
    }
     else
    {
        if (TRawStr::bCompareStr(pFindData->pszText, pFindData->pchTmpBuf))
        {
            if (pFindData->pidTarget != kCIDLib::pidInvalid)
            {
                tCIDLib::TCard4 c4Id;
                ::GetWindowThreadProcessId(hwndTarget, &c4Id);

                if (c4Id == pFindData->pidTarget)
                {
                    if (!pFindData->pszClass || bCheckClass(hwndTarget, pFindData->pszClass))
                        pFindData->hwndFound = hwndTarget;
                    return 0;
                }
            }
             else
            {
                if (!pFindData->pszClass || bCheckClass(hwndTarget, pFindData->pszClass))
                    pFindData->hwndFound = hwndTarget;
                return 0;
            }
        }
    }
    return 1;
}


static BOOL CALLBACK bEnumWndOwnedPopups(HWND hwndTarget, LPARAM lParam)
{
    // Cast the data parm to its correct type
    CIDCtrls_ThisFacility::TFindWndOwnedPopup* pFindData
                            = (CIDCtrls_ThisFacility::TFindWndOwnedPopup*)lParam;

    // Get the root owner of the target and see if it's the one we care about
    tCIDCtrls::TWndHandle hwndOwner = ::GetAncestor(hwndTarget, GA_ROOTOWNER);
    if (!hwndOwner || (hwndOwner != pFindData->hwndRootOwner))
        return 1;

    //
    //  If they want only windows that are in a modal loop, then we
    //  have to get into the CIDLib level info.
    //
    if (pFindData->bModalOnly)
    {
        TWindow* pwndRet = TWindow::pwndGetWndLinkPtr(hwndTarget);
        if (!pwndRet || !pwndRet->bIsInModalLoop())
            return 1;
    }

    // We got a hit
    pFindData->hwndFound = hwndTarget;
    return 0;
}


static BOOL CALLBACK bEnumWndFiltered(HWND hwndTarget, LPARAM lParam)
{
    // Cast the data parm to its correct type
    CIDCtrls_ThisFacility::TFindWndFiltered* pFindData
                            = (CIDCtrls_ThisFacility::TFindWndFiltered*)lParam;

    //
    //  If they are filtering for pid, then get the process id of this window
    //  and check it.
    //
    if (pFindData->pidTarget)
    {
        tCIDLib::TCard4 c4Id;
        ::GetWindowThreadProcessId(hwndTarget, &c4Id);
        if (c4Id != pFindData->pidTarget)
            return 1;
    }

    // Make sure it matches the owned/unowned flag
    tCIDCtrls::TWndHandle hwndOwner = ::GetWindow(hwndTarget, GW_OWNER);

    if ((pFindData->bOwned && !hwndOwner)
    ||  (!pFindData->bOwned && hwndOwner))
    {
        return 1;
    }

    // If the only want windows owned by a specific window, check that
    if (pFindData->hwndOwner)
    {
        if (!hwndOwner || (hwndOwner != pFindData->hwndOwner))
            return 1;
    }

    // Test the styles
    const tCIDLib::TCard4 c4Styles = ::GetWindowLong(hwndTarget, GWL_STYLE);
    const tCIDLib::TCard4 c4ExStyles = ::GetWindowLong(hwndTarget, GWL_EXSTYLE);

    if (((c4Styles & pFindData->c4WantStyles) == pFindData->c4WantStyles)
    &&  ((c4Styles & pFindData->c4DontWantStyles) == 0)
    &&  ((c4ExStyles & pFindData->c4WantExStyles) == pFindData->c4WantExStyles)
    &&  ((c4ExStyles & pFindData->c4DontWantExStyles) == 0))
    {
        pFindData->c4FoundExStyles = c4ExStyles;
        pFindData->c4FoundStyles = c4Styles;
        pFindData->hwndFound = hwndTarget;

        // Check the class if they asked us to
        if (!pFindData->pszClass || bCheckClass(hwndTarget, pFindData->pszClass))
            return 0;
    }
    return 1;
}


// Translate one of our extended key enum values to a VK_ constant
static tCIDLib::TCard2 c2XlatExtKey(const tCIDCtrls::EExtKeys eKey)
{
    switch(eKey)
    {
        case tCIDCtrls::EExtKeys::Button1 :
            return VK_LBUTTON;

        case tCIDCtrls::EExtKeys::Button2 :
            return VK_MBUTTON;

        case tCIDCtrls::EExtKeys::Button3 :
            return VK_RBUTTON;

        case tCIDCtrls::EExtKeys::BackSpace :
            return VK_BACK;

        case tCIDCtrls::EExtKeys::Tab :
            return VK_TAB;

        case tCIDCtrls::EExtKeys::Shift :
            return VK_SHIFT;

        case tCIDCtrls::EExtKeys::Ctrl :
            return VK_CONTROL;

        case tCIDCtrls::EExtKeys::Alt :
            return VK_MENU;

        case tCIDCtrls::EExtKeys::Pause :
            return VK_PAUSE;

        case tCIDCtrls::EExtKeys::CapsLock :
            return VK_CAPITAL;

        case tCIDCtrls::EExtKeys::Esc :
            return VK_ESCAPE;

        case tCIDCtrls::EExtKeys::PageUp :
            return VK_PRIOR;

        case tCIDCtrls::EExtKeys::PageDown :
            return VK_NEXT;

        case tCIDCtrls::EExtKeys::End :
            return VK_END;

        case tCIDCtrls::EExtKeys::Home :
            return VK_HOME;

        case tCIDCtrls::EExtKeys::Left :
            return VK_LEFT;

        case tCIDCtrls::EExtKeys::Up :
            return VK_UP;

        case tCIDCtrls::EExtKeys::Right :
            return VK_RIGHT;

        case tCIDCtrls::EExtKeys::Down :
            return VK_DOWN;

        case tCIDCtrls::EExtKeys::PrintScreen :
            return VK_PRINT;

        case tCIDCtrls::EExtKeys::Insert :
            return VK_INSERT;

        case tCIDCtrls::EExtKeys::Delete :
            return VK_DELETE;

        case tCIDCtrls::EExtKeys::ScrollLock :
            return VK_SCROLL;

        case tCIDCtrls::EExtKeys::NumLock :
            return VK_NUMLOCK;

        case tCIDCtrls::EExtKeys::Enter :
            return VK_RETURN;

        case tCIDCtrls::EExtKeys::Space :
            return VK_SPACE;

        case tCIDCtrls::EExtKeys::NumPad_Div :
            return VK_DIVIDE;

        case tCIDCtrls::EExtKeys::NumPad_Mul :
            return VK_MULTIPLY;

        case tCIDCtrls::EExtKeys::NumPad_Sub :
            return VK_SUBTRACT;

        case tCIDCtrls::EExtKeys::NumPad_Plus :
            return VK_ADD;

        case tCIDCtrls::EExtKeys::NumPad_Decimal :
            return VK_DECIMAL;

        case tCIDCtrls::EExtKeys::Browser_Back :
            return VK_BROWSER_BACK;

        case tCIDCtrls::EExtKeys::Browser_Forward :
            return VK_BROWSER_FORWARD;

        case tCIDCtrls::EExtKeys::Browser_Stop :
            return VK_BROWSER_STOP;

        case tCIDCtrls::EExtKeys::Browser_Refresh :
            return VK_BROWSER_REFRESH;

        case tCIDCtrls::EExtKeys::Browser_Search :
            return VK_BROWSER_SEARCH;

        case tCIDCtrls::EExtKeys::Browser_Favorites :
            return VK_BROWSER_FAVORITES;

        case tCIDCtrls::EExtKeys::Browser_Home :
            return VK_BROWSER_HOME;

        case tCIDCtrls::EExtKeys::Volume_Mute :
            return VK_VOLUME_MUTE;

        case tCIDCtrls::EExtKeys::Volume_Down :
            return VK_VOLUME_DOWN;

        case tCIDCtrls::EExtKeys::Volume_Up :
            return VK_VOLUME_UP;

        case tCIDCtrls::EExtKeys::Media_PlayPause :
            return VK_MEDIA_PLAY_PAUSE;

        case tCIDCtrls::EExtKeys::Media_Stop :
            return VK_MEDIA_STOP;

        case tCIDCtrls::EExtKeys::Media_PrevTrack :
            return VK_MEDIA_PREV_TRACK;

        case tCIDCtrls::EExtKeys::Media_NextTrack :
            return VK_MEDIA_NEXT_TRACK;

        case tCIDCtrls::EExtKeys::F1 :
            return VK_F1;

        case tCIDCtrls::EExtKeys::F2 :
            return VK_F2;

        case tCIDCtrls::EExtKeys::F3 :
            return VK_F3;

        case tCIDCtrls::EExtKeys::F4 :
            return VK_F4;

        case tCIDCtrls::EExtKeys::F5 :
            return VK_F5;

        case tCIDCtrls::EExtKeys::F6 :
            return VK_F6;

        case tCIDCtrls::EExtKeys::F7 :
            return VK_F7;

        case tCIDCtrls::EExtKeys::F8 :
            return VK_F8;

        case tCIDCtrls::EExtKeys::F9 :
            return VK_F9;

        case tCIDCtrls::EExtKeys::F10 :
            return VK_F10;

        case tCIDCtrls::EExtKeys::F11 :
            return VK_F11;

        case tCIDCtrls::EExtKeys::F12 :
            return VK_F12;

        case tCIDCtrls::EExtKeys::F13 :
            return VK_F13;

        case tCIDCtrls::EExtKeys::F14 :
            return VK_F14;

        case tCIDCtrls::EExtKeys::F15 :
            return VK_F15;

        case tCIDCtrls::EExtKeys::F16 :
            return VK_F16;

        case tCIDCtrls::EExtKeys::F17 :
            return VK_F17;

        case tCIDCtrls::EExtKeys::F18 :
            return VK_F18;

        case tCIDCtrls::EExtKeys::F19 :
            return VK_F19;

        case tCIDCtrls::EExtKeys::F20 :
            return VK_F20;

        case tCIDCtrls::EExtKeys::F21 :
            return VK_F21;

        case tCIDCtrls::EExtKeys::F22 :
            return VK_F22;

        case tCIDCtrls::EExtKeys::F23 :
            return VK_F23;

        case tCIDCtrls::EExtKeys::F24 :
            return VK_F24;

        default :
            break;
    };
    return 0;
}


// ---------------------------------------------------------------------------
//  CLASS: TGUIMonInfo
// PREFIX: moni
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//  TGUIMonInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TGUIMonInfo::TGUIMonInfo() :

    m_bIsPrimary(kCIDLib::False)
{
}

TGUIMonInfo::TGUIMonInfo(const  TString&            strDevName
                        , const TArea&              areaPhys
                        , const TArea&              areaAvail
                        , const tCIDLib::TBoolean   bIsPrimary) :

    m_areaAvail(areaAvail)
    , m_areaPhys(areaPhys)
    , m_bIsPrimary(bIsPrimary)
    , m_strDevName(strDevName)
{
}

TGUIMonInfo::~TGUIMonInfo()
{
}


// ---------------------------------------------------------------------------
//  TGUIMonInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TGUIMonInfo::bIsPrimary() const
{
    return m_bIsPrimary;
}

const TArea& TGUIMonInfo::areaAvailable() const
{
    return m_areaAvail;
}

const TArea& TGUIMonInfo::areaPhysical() const
{
    return m_areaPhys;
}

const TString& TGUIMonInfo::strDevName() const
{
    return m_strDevName;
}




// ---------------------------------------------------------------------------
//  CLASS: TWndPtrJanitor
// PREFIX: jan
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TWndPtrJanitor: Constructors and Destructor
// ---------------------------------------------------------------------------
TWndPtrJanitor::TWndPtrJanitor(const tCIDCtrls::ESysPtrs eToSet) :

    m_pOldInfo(nullptr)
{
    // Save the old cursor, and set the new one
    m_pOldInfo = ::GetCursor();
    facCIDCtrls().SetPointer(eToSet);
}

TWndPtrJanitor::~TWndPtrJanitor()
{
    // Put the old one back
    ::SetCursor((HCURSOR)m_pOldInfo);
}




// ---------------------------------------------------------------------------
//   CLASS: TFacCIDCtrls
//  PREFIX: fac
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFacCIDCtrls: Constructors and operators
// ---------------------------------------------------------------------------
TFacCIDCtrls::TFacCIDCtrls() :

    TGUIFacility
    (
        L"CIDCtrls"
        , tCIDLib::EModTypes::Dll
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::HasMsgsAndRes
    )
    , m_areaDefWnd
      (
        kCIDCtrls::wcrDefault
        , kCIDCtrls::wcrDefault
        , kCIDCtrls::wszDefault
        , kCIDCtrls::wszDefault
      )
    , m_iInitShow(SW_SHOWNORMAL)
    , m_pmiSpecialMsgs(nullptr)
    , m_pwndTopWnd(nullptr)
{
    //
    //  Set up the list of dialog item class types. This is used to map from the names
    //  used in the CIDRC files, to the actual classes.
    //
    m_colDlgClasses.objAdd(TKeyValuePair(L"2COLSECTLIST", L"T2ColSectList"));
    m_colDlgClasses.objAdd(TKeyValuePair(L"ACTIVITYBAR", L"TActivityBar"));
    m_colDlgClasses.objAdd(TKeyValuePair(L"ATTREDIT", L"TAttrEditWnd"));
    m_colDlgClasses.objAdd(TKeyValuePair(L"CALENDAR", L"TCalendar"));
    m_colDlgClasses.objAdd(TKeyValuePair(L"CHECKBOX", L"TCheckBox"));
    m_colDlgClasses.objAdd(TKeyValuePair(L"CHECKEDMCLIST", L"TCheckedMCListWnd"));
    m_colDlgClasses.objAdd(TKeyValuePair(L"COLOREDLIST", L"TColoredList"));
    m_colDlgClasses.objAdd(TKeyValuePair(L"GENERICWND", L"TGenericWnd"));
    m_colDlgClasses.objAdd(TKeyValuePair(L"CLRPALETTE", L"T24BitPalette"));
    m_colDlgClasses.objAdd(TKeyValuePair(L"COMBOBOX", L"TComboBox"));
    m_colDlgClasses.objAdd(TKeyValuePair(L"ENTRYFLD", L"TEntryField"));
    m_colDlgClasses.objAdd(TKeyValuePair(L"ENUMSPINBOX", L"TEnumSpinBox"));
    m_colDlgClasses.objAdd(TKeyValuePair(L"IMGPREVIEW", L"TImgPreviewWnd"));
    m_colDlgClasses.objAdd(TKeyValuePair(L"LISTBOX", L"TListBox"));
    m_colDlgClasses.objAdd(TKeyValuePair(L"MCLISTBOX", L"TMultiColListBox"));
    m_colDlgClasses.objAdd(TKeyValuePair(L"OBJVIEW", L"TObjView"));
    m_colDlgClasses.objAdd(TKeyValuePair(L"MLEDIT", L"TMultiEdit"));
    m_colDlgClasses.objAdd(TKeyValuePair(L"MLSTATICTEXT", L"TStaticMultiText"));
    m_colDlgClasses.objAdd(TKeyValuePair(L"NUMSPINBOX", L"TNumSpinBox"));
    m_colDlgClasses.objAdd(TKeyValuePair(L"PROGRESSBAR", L"TProgressBar"));
    m_colDlgClasses.objAdd(TKeyValuePair(L"PUSHBUTTON", L"TPushButton"));
    m_colDlgClasses.objAdd(TKeyValuePair(L"RADIOBUTTON", L"TRadioButton"));
    m_colDlgClasses.objAdd(TKeyValuePair(L"SCROLLAREA", L"TScrollArea"));
    m_colDlgClasses.objAdd(TKeyValuePair(L"SECTIONALLIST", L"TSectionalList"));
    m_colDlgClasses.objAdd(TKeyValuePair(L"SEPARATOR", L"TSeparatorWnd"));
    m_colDlgClasses.objAdd(TKeyValuePair(L"SLIDER", L"TSlider"));
    m_colDlgClasses.objAdd(TKeyValuePair(L"STATICCLR", L"TStaticColor"));
    m_colDlgClasses.objAdd(TKeyValuePair(L"STATICIMG", L"TStaticImg"));
    m_colDlgClasses.objAdd(TKeyValuePair(L"STATICTEXT", L"TStaticText"));
    m_colDlgClasses.objAdd(TKeyValuePair(L"TABBEDWND", L"TTabbedWnd"));
    m_colDlgClasses.objAdd(TKeyValuePair(L"TEXTOUTWND", L"TTextOutWnd"));
    m_colDlgClasses.objAdd(TKeyValuePair(L"TEXTOUTWND2", L"TTextOutWnd2"));
    m_colDlgClasses.objAdd(TKeyValuePair(L"TREEVIEW", L"TTreeView"));
    m_colDlgClasses.objAdd(TKeyValuePair(L"VSEPARATOR", L"TVSeparatorWnd"));
}

TFacCIDCtrls::~TFacCIDCtrls()
{
}


// ---------------------------------------------------------------------------
//  TFacCIDCtrls: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Return the 'default window area'. If you pass this to a window creation method
//  it will size/position the window to a system chosen default.
//
const TArea& TFacCIDCtrls::areaDefWnd() const
{
    return m_areaDefWnd;
}


// Try to find a previous app with the passed title text and active it
tCIDLib::TBoolean
TFacCIDCtrls::bActivatePreviousInst(const   TString&            strWndText
                                    , const tCIDLib::TBoolean   bStartsWith)
{
    // Enumerate top level windows and look for one with the passed text
    CIDCtrls_ThisFacility::TFindWndText FindData = {0};
    FindData.pszText = strWndText.pszBuffer();
    FindData.bStartsWith = bStartsWith;
    FindData.hwndFound = kCIDCtrls::hwndInvalid;
    FindData.pidTarget = kCIDLib::pidInvalid;
    FindData.c4TextLen = strWndText.c4Length();

    // Set up a system buffer for the callback to use to query window text
    TSysBuf mbufTmp(8192, 8192);
    FindData.pchTmpBuf = (tCIDLib::TCh*)mbufTmp.pc1Data();
    FindData.c4TmpBufSz = 8191;

    // There is no meaningful error return, so just see if we found anything
    ::EnumWindows(&bEnumWndText, (LPARAM)&FindData);
    if (FindData.hwndFound == kCIDCtrls::hwndInvalid)
        return kCIDLib::False;

    //
    //  If the other guy is minimized, then restore him (which will implicitly
    //  bring him forward. If not minimized, then just set him to the foreground.
    //
    WINDOWPLACEMENT WndPlace = {0};
    WndPlace.length = sizeof(WINDOWPLACEMENT);
    ::GetWindowPlacement(FindData.hwndFound, &WndPlace);
    if (WndPlace.showCmd== SW_SHOWMINIMIZED)
    {
        WndPlace.showCmd = SW_RESTORE;
        ::SetWindowPlacement(FindData.hwndFound, &WndPlace);
    }
    ::SetForegroundWindow(FindData.hwndFound);
    return kCIDLib::True;
}


tCIDLib::TVoid TFacCIDCtrls::Activate(const tCIDCtrls::TWndHandle hwndTar)
{
    ::SetActiveWindow(hwndTar);
}


// Indicate that we want to allow this program to force the foreground app
tCIDLib::TVoid TFacCIDCtrls::AllowSetForeground()
{
    ::AllowSetForegroundWindow(ASFW_ANY);
}


tCIDLib::TBoolean TFacCIDCtrls::bAltShifted() const
{
    if (::GetKeyState(VK_MENU) & 0x80)
        return kCIDLib::True;
    return kCIDLib::False;
}


tCIDLib::TBoolean TFacCIDCtrls::bAnyShifts() const
{
    if ((::GetKeyState(VK_MENU) & 0x80)
    ||  (::GetKeyState(VK_CONTROL) & 0x80)
    ||  (::GetKeyState(VK_SHIFT) & 0x80))
    {
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


tCIDLib::TBoolean TFacCIDCtrls::bCapsLockState() const
{
    if (::GetKeyState(VK_CAPITAL) & 0x1)
        return kCIDLib::True;
    return kCIDLib::False;
}


tCIDLib::TBoolean TFacCIDCtrls::bCtrlShifted() const
{
    if (::GetKeyState(VK_CONTROL) & 0x80)
        return kCIDLib::True;
    return kCIDLib::False;
}


//
//  Returns true if the window to check is owned by the passed owner window (directly or
//  indirectly.) If bImmediate is true, we get the owner window directly from the window
//  to check. Otherwise we get the root owner window.
//
tCIDLib::TBoolean
TFacCIDCtrls::bIsOwnedBy(const  tCIDCtrls::TWndHandle   hwndToCheck
                        , const tCIDCtrls::TWndHandle   hwndOwner
                        , const tCIDLib::TBoolean       bImmediate) const
{
    if (bImmediate)
        return (::GetWindow(hwndToCheck, GW_OWNER) == hwndOwner);

    return (::GetAncestor(hwndToCheck, GA_ROOTOWNER) == hwndOwner);
}


tCIDLib::TBoolean TFacCIDCtrls::bIsPointerVisible() const
{
    CURSORINFO Info;
    Info.cbSize = sizeof(CURSORINFO);
    ::GetCursorInfo(&Info);
    return ((Info.flags & CURSOR_SHOWING) != 0);
}


tCIDLib::TBoolean
TFacCIDCtrls::bIsWindowValid(const tCIDCtrls::TWndHandle hwndToCheck) const
{
    if (::IsWindow(hwndToCheck))
        return kCIDLib::True;
    return kCIDLib::False;
}


tCIDLib::TBoolean
TFacCIDCtrls::bIsWindowVisible(const tCIDCtrls::TWndHandle hwndSrc) const
{
    if (::IsWindowVisible(hwndSrc))
        return kCIDLib::True;
    return kCIDLib::False;
}


//
//  Returns whether one of the mouse buttons is pressed or not. We have to deal with
//  possible swapping of the buttons by system config.
//
tCIDLib::TBoolean
TFacCIDCtrls::bMouseButtonDown(const tCIDCtrls::EMouseButts eToCheck) const
{
    const tCIDLib::TBoolean bSwapped = ::GetSystemMetrics(SM_SWAPBUTTON) != 0;

    int iKey;
    if (eToCheck == tCIDCtrls::EMouseButts::Middle)
    {
        iKey = VK_MBUTTON;
    }
     else if (eToCheck == tCIDCtrls::EMouseButts::Left)
    {
        if (bSwapped)
            iKey = VK_RBUTTON;
        else
            iKey = VK_LBUTTON;
    }
     else if (eToCheck == tCIDCtrls::EMouseButts::Right)
    {
        if (bSwapped)
            iKey = VK_LBUTTON;
        else
            iKey = VK_RBUTTON;
    }
     else
    {
        CIDAssert2(L"Unknown mouse button");
    }

    SHORT sState = ::GetAsyncKeyState(iKey);
    return (sState & 0x8000) != 0;
}


//
//  Invokes the standard file open dialog. If a selection is made it returns true, else
//  false. If true, it returns the full paths to the selected items.
//
tCIDLib::TBoolean
TFacCIDCtrls::bOpenFileDlg( const   TWindow&                wndOwner
                            , const TString&                strTitle
                            , const TString&                strInitPath
                            ,       tCIDLib::TStrCollect&   colSelected
                            , const tCIDLib::TKVPList&      colFileTypes
                            , const tCIDCtrls::EFOpenOpts   eOptions)
{
    tCIDLib::TBoolean   bRes = kCIDLib::False;
    IFileOpenDialog*    pDlg = NULL;
    HRESULT             hRes;

    try
    {
        hRes = ::CoCreateInstance
        (
            CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pDlg)
        );
        if (!SUCCEEDED(hRes))
            throw hRes;

        TCOMJanitor<IFileOpenDialog> janDlg(&pDlg);

        // Set the provided title
        pDlg->SetTitle(strTitle.pszBuffer());

        // Translate our options enum to their options
        FILEOPENDIALOGOPTIONS Opts = 0;
        hRes = pDlg->GetOptions(&Opts);
        if (!SUCCEEDED(hRes))
            throw hRes;

        if (tCIDLib::bAllBitsOn(eOptions, tCIDCtrls::EFOpenOpts::ConfirmPrompt))
            Opts |= FOS_OVERWRITEPROMPT | FOS_CREATEPROMPT;
        else
            Opts &= ~(FOS_OVERWRITEPROMPT | FOS_CREATEPROMPT);

        if (tCIDLib::bAllBitsOn(eOptions, tCIDCtrls::EFOpenOpts::FileMustExist))
            Opts |= FOS_FILEMUSTEXIST;
        else
            Opts &= ~FOS_FILEMUSTEXIST;

        if (tCIDLib::bAllBitsOn(eOptions, tCIDCtrls::EFOpenOpts::FileSystemOnly))
            Opts |= FOS_FORCEFILESYSTEM;
        else
            Opts &= ~FOS_FORCEFILESYSTEM;

        if (tCIDLib::bAllBitsOn(eOptions, tCIDCtrls::EFOpenOpts::MultiSelect))
            Opts |= FOS_ALLOWMULTISELECT;
        else
            Opts &= ~FOS_ALLOWMULTISELECT;

        if (tCIDLib::bAllBitsOn(eOptions, tCIDCtrls::EFOpenOpts::NoReadOnly))
            Opts |= FOS_NOREADONLYRETURN;
        else
            Opts &= ~FOS_NOREADONLYRETURN;

        if (tCIDLib::bAllBitsOn(eOptions, tCIDCtrls::EFOpenOpts::PathMustExist))
            Opts |= FOS_PATHMUSTEXIST;
        else
            Opts &= ~FOS_PATHMUSTEXIST;

        if (tCIDLib::bAllBitsOn(eOptions, tCIDCtrls::EFOpenOpts::SelectFolders))
            Opts |= FOS_PICKFOLDERS;
        else
            Opts &= ~FOS_PICKFOLDERS;

        if (tCIDLib::bAllBitsOn(eOptions, tCIDCtrls::EFOpenOpts::ShowSpecialFiles))
            Opts |= FOS_FORCESHOWHIDDEN;
        else
            Opts &= ~FOS_FORCESHOWHIDDEN;

        if (tCIDLib::bAllBitsOn(eOptions, tCIDCtrls::EFOpenOpts::StrictTypes))
            Opts |= FOS_STRICTFILETYPES;
        else
            Opts &= ~FOS_STRICTFILETYPES;

        pDlg->SetOptions(Opts);

        // Set the initial path, if they provided one
        if (!strInitPath.bIsEmpty())
        {
            ITEMIDLIST* pItemList = ::SHSimpleIDListFromPath(strInitPath.pszBuffer());
            if (pItemList)
            {
                IShellItem* pShellItem = 0;
                hRes = ::SHCreateShellItem
                (
                    NULL, NULL, pItemList, &pShellItem
                );

                if (SUCCEEDED(hRes))
                {
                    pDlg->SetDefaultFolder(pShellItem);
                    pShellItem->Release();
                }

                ::CoTaskMemFree(pItemList);
            }

            // See if there's a file name part. If so, set it
            TPathStr pathInit(strInitPath);
            TString strName;
            if (pathInit.bQueryNameExt(strName))
                pDlg->SetFileName(strName.pszBuffer());
        }

        // Set the file types if we got any
        if (!colFileTypes.bIsEmpty())
        {
            const tCIDLib::TCard4 c4TypeCnt = colFileTypes.c4ElemCount();

            COMDLG_FILTERSPEC* pFilters = new COMDLG_FILTERSPEC[c4TypeCnt];
            TArrayJanitor<COMDLG_FILTERSPEC> janTypes(pFilters);

            for (tCIDLib::TCard4 c4Index = 0; c4Index < c4TypeCnt; c4Index++)
            {
                pFilters[c4Index].pszName = colFileTypes[c4Index].strKey().pszBuffer();
                pFilters[c4Index].pszSpec = colFileTypes[c4Index].strValue().pszBuffer();
            }
            pDlg->SetFileTypes(c4TypeCnt, pFilters);
        }

        // Now invoke the dialog
        hRes = pDlg->Show(wndOwner.hwndSafe());

        // If it's just a user cancellation, then return false
        if (hRes == HRESULT_FROM_WIN32(ERROR_CANCELLED))
        {
            colSelected.RemoveAll();
            return kCIDLib::False;
        }

        // Else check for any other error
        if (!SUCCEEDED(hRes))
            throw hRes;

        // Get the results
        IShellItemArray* psiaResults = NULL;
        pDlg->GetResults(&psiaResults);
        if (!SUCCEEDED(hRes))
            throw hRes;

        // Get the count of results
        DWORD dwFolderCount;
        hRes = psiaResults->GetCount(&dwFolderCount);
        if (!SUCCEEDED(hRes))
            throw hRes;

        colSelected.RemoveAll();
        for (DWORD dwIndex = 0; dwIndex < dwFolderCount; dwIndex++)
        {
            IShellItem* psiResult = 0;
            if (!SUCCEEDED(psiaResults->GetItemAt(dwIndex, &psiResult)))
                throw hRes;
            TCOMJanitor<IShellItem> janItem(&psiResult);

            PWSTR pszPath = NULL;
            if (!SUCCEEDED(psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pszPath)))
                throw hRes;

            colSelected.objAdd(pszPath);
            ::CoTaskMemFree(pszPath);
        }
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToCatch);

        TErrBox msgbErr(L"File Open Dialog Error", errToCatch.strErrText());
        msgbErr.ShowIt(wndOwner);
        return kCIDLib::False;

    }

    catch(HRESULT)
    {
        TString strErr;
        TModule::bLoadOSMsg(hRes, strErr);

        TErrBox msgbErr(L"File Open Dialog Error", strErr);
        msgbErr.ShowIt(wndOwner);
        return kCIDLib::False;
    }

    // If we got any selections return true
    return !colSelected.bIsEmpty();
}


//
//  We see if the passed window owns any popup windows. This is used
//  to prevent certain things from being done if a dialog box is up generally.
//
tCIDLib::TBoolean
TFacCIDCtrls::bOwnsActivePopups(const   tCIDCtrls::TWndHandle   hwndToCheck
                                , const tCIDLib::TBoolean       bModalOnly) const
{
    // Get the thread id of the passed window
    tCIDLib::TProcessId pidWnd;
    tCIDLib::TThreadId tidWnd = ::GetWindowThreadProcessId(hwndToCheck, &pidWnd);

    CIDCtrls_ThisFacility::TFindWndOwnedPopup FindData = {0};
    FindData.hwndFound = kCIDCtrls::hwndInvalid;
    FindData.bModalOnly = bModalOnly;
    FindData.hwndRootOwner = ::GetAncestor(hwndToCheck, GA_ROOTOWNER);
    ::EnumThreadWindows(tidWnd, &bEnumWndOwnedPopups, (LPARAM)&FindData);
    return (FindData.hwndFound != 0);
}


//
//  This gives the calling app a feeling as to whether it would be ok now
//  to do an unsolicited popup. We check for various things that would make
//  that a bad idea.
//
tCIDLib::TBoolean TFacCIDCtrls::bPopupOK() const
{
    // First, see if the mouse is captured. If so, definitely not
    if (::GetCapture() != 0)
        return kCIDLib::False;

    // If any mouse button is pressed, that's bad
    if ((::GetKeyState(VK_LBUTTON) & 0x80)
    ||  (::GetKeyState(VK_RBUTTON) & 0x80)
    ||  (::GetKeyState(VK_MBUTTON) & 0x80))
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean TFacCIDCtrls::bScrollLockState() const
{
    if (::GetKeyState(VK_SCROLL) & 0x1)
        return kCIDLib::True;
    return kCIDLib::False;
}


tCIDLib::TBoolean TFacCIDCtrls::bShifted() const
{
    if (::GetKeyState(VK_SHIFT) & 0x80)
        return kCIDLib::True;
    return kCIDLib::False;
}


//
//  Looks up the named monitor and returns it's area, optinally all of it or just
//  the part that's not covered by standard desktop accessories.
//
tCIDLib::TBoolean
TFacCIDCtrls::bQueryNamedMonArea(const  TString&            strDevName
                                ,       TArea&              areaToFill
                                , const tCIDLib::TBoolean   bAvailOnly)
{
    //
    //  Te we set the found monitor handle to zero to use as a 'found it or not'
    //  type indicator.
    //
    TMonEnumInfo Info = {0};
    Info.pszDevName = strDevName.pszBuffer();
    ::EnumDisplayMonitors(0, 0, &MonitorEnumCB, (LPARAM)&Info);

    // If we never found it, return failure
    if (!Info.hmonFound)
        return kCIDLib::False;

    // Calla helper to get the info
    QueryMonitorArea(Info.hmonFound, areaToFill, bAvailOnly);
    return kCIDLib::True;
}


//
//  Invokes the standard file save dialog. If a selection is made it returns true,
//  else false. If true, it returns the full path to the selected thing.
//
tCIDLib::TBoolean
TFacCIDCtrls::bSaveFileDlg( const   TWindow&                wndOwner
                            , const TString&                strTitle
                            , const TString&                strInitPath
                            , const TString&                strInitName
                            ,       TString&                strSelected
                            , const tCIDLib::TKVPList&      colFileTypes
                            , const tCIDCtrls::EFSaveOpts   eOptions)
{
    tCIDLib::TBoolean   bRes = kCIDLib::False;
    IFileSaveDialog*    pDlg = NULL;
    HRESULT             hRes;

    try
    {
        hRes = ::CoCreateInstance
        (
            CLSID_FileSaveDialog
            , NULL
            , CLSCTX_INPROC_SERVER
            , IID_PPV_ARGS(&pDlg)
        );
        if (!SUCCEEDED(hRes))
            throw hRes;

        TCOMJanitor<IFileSaveDialog> janDlg(&pDlg);

        // Set the provided title
        pDlg->SetTitle(strTitle.pszBuffer());

        // Translate our options enum to their options
        if (eOptions != tCIDCtrls::EFSaveOpts::None)
        {
            FILEOPENDIALOGOPTIONS Opts = 0;

            if (tCIDLib::bAllBitsOn(eOptions, tCIDCtrls::EFSaveOpts::FileMustExist))
                Opts |= FOS_FILEMUSTEXIST;

            if (tCIDLib::bAllBitsOn(eOptions, tCIDCtrls::EFSaveOpts::FilesOnly))
                Opts |= FOS_FORCEFILESYSTEM;

            if (tCIDLib::bAllBitsOn(eOptions, tCIDCtrls::EFSaveOpts::PathMustExist))
                Opts |= FOS_PATHMUSTEXIST;

            if (tCIDLib::bAllBitsOn(eOptions, tCIDCtrls::EFSaveOpts::ConfirmPrompt))
                Opts |= FOS_OVERWRITEPROMPT | FOS_CREATEPROMPT;

            if (tCIDLib::bAllBitsOn(eOptions, tCIDCtrls::EFSaveOpts::ConfirmPrompt))
                Opts |= FOS_OVERWRITEPROMPT | FOS_CREATEPROMPT;

            if (tCIDLib::bAllBitsOn(eOptions, tCIDCtrls::EFSaveOpts::StrictTypes))
                Opts |= FOS_STRICTFILETYPES;

            pDlg->SetOptions(Opts);
        }

        // Set the file types if we got any
        if (!colFileTypes.bIsEmpty())
        {
            const tCIDLib::TCard4 c4TypeCnt = colFileTypes.c4ElemCount();

            COMDLG_FILTERSPEC* pFilters = new COMDLG_FILTERSPEC[c4TypeCnt];
            TArrayJanitor<COMDLG_FILTERSPEC> janTypes(pFilters);

            for (tCIDLib::TCard4 c4Index = 0; c4Index < c4TypeCnt; c4Index++)
            {
                pFilters[c4Index].pszName = colFileTypes[c4Index].strKey().pszBuffer();
                pFilters[c4Index].pszSpec = colFileTypes[c4Index].strValue().pszBuffer();
            }
            pDlg->SetFileTypes(c4TypeCnt, pFilters);
        }

        // Set the initial path, if they provided one
        if (!strInitPath.bIsEmpty())
        {
            if (TFileSys::bExists(strInitPath))
            {
                ITEMIDLIST* pItemList = ::SHSimpleIDListFromPath(strInitPath.pszBuffer());
                if (pItemList)
                {
                    IShellItem* pShellItem = 0;
                    hRes = ::SHCreateShellItem
                    (
                        NULL, NULL, pItemList, &pShellItem
                    );

                    if (SUCCEEDED(hRes))
                    {
                        pDlg->SetSaveAsItem(pShellItem);
                        pShellItem->Release();
                    }

                    ::CoTaskMemFree(pItemList);
                }
            }
        }

        // Set the initial name if they provided one
        if (!strInitName.bIsEmpty())
            pDlg->SetFileName(strInitName.pszBuffer());

        // Now invoke the dialog
        hRes = pDlg->Show(wndOwner.hwndSafe());

        // If it's just a user cancellation, then return false
        if (hRes == HRESULT_FROM_WIN32(ERROR_CANCELLED))
            return kCIDLib::False;

        // Else check for other errors
        if (!SUCCEEDED(hRes))
            throw hRes;

        // Get the results
        IShellItem* psiResult = 0;
        pDlg->GetResult(&psiResult);
        if (!SUCCEEDED(hRes))
            throw hRes;

        TCOMJanitor<IShellItem> janItem(&psiResult);

        PWSTR pszPath = NULL;
        if (!SUCCEEDED(psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pszPath)))
            throw hRes;

        strSelected = pszPath;
        ::CoTaskMemFree(pszPath);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToCatch);

        TErrBox msgbErr(L"File Save Dialog Error", errToCatch.strErrText());
        msgbErr.ShowIt(wndOwner);
        return kCIDLib::False;

    }

    catch(HRESULT)
    {
        TString strErr;
        TModule::bLoadOSMsg(hRes, strErr);

        TErrBox msgbErr(L"File Save Dialog Error", strErr);
        msgbErr.ShowIt(wndOwner);
        return kCIDLib::False;
    }

    return kCIDLib::True;
}



//
//  This is called to load a special text symbol by id. We just search our
//  list for this id.
//
tCIDLib::TBoolean
TFacCIDCtrls::bLoadSpecialText(const tCIDLib::TMsgId midToLoad, TString& strToFill)
{
    // If we've not loaded the special text yet, then do that
    if (!CIDCtrls_ThisFacility::atomInitDone)
    {
        TBaseLock lockInit;
        if (!CIDCtrls_ThisFacility::atomInitDone)
        {
            TKrnlModule::bLoadMessages
            (
                facCIDCtrls().strPath().pszBuffer()
                , L"CIDCommonText"
                , m_pmiSpecialMsgs
            );
            CIDCtrls_ThisFacility::atomInitDone.Set();
        }
    }

    if (!m_pmiSpecialMsgs)
        return kCIDLib::False;

    tCIDLib::TBoolean bLoaded;
    const tCIDLib::TCh* pszText = pszLoadCIDMsg
    (
        *m_pmiSpecialMsgs, midToLoad, bLoaded
    );

    //
    //  We still set the string since we get back a default message even
    //  if it fails.
    //
    strToFill = pszText;
    return bLoaded;
}


//
//  Low level method to get raw window styles from a raw window handle
//
tCIDLib::TCard4
TFacCIDCtrls::c4QueryWndStyles(const tCIDCtrls::TWndHandle hwndSrc) const
{
    return tCIDLib::TCard4(::GetWindowLong(hwndSrc, GWL_STYLE));
}


//
//  Low level method to get window text from a raw window handle.
//
tCIDLib::TCard4
TFacCIDCtrls::c4QueryWndText(const  tCIDCtrls::TWndHandle   hwndSrc
                            ,       TString&                strToFill)
{
    // Find out how long the text is
    const tCIDLib::TCard4 c4Len = ::SendMessageW(hwndSrc, WM_GETTEXTLENGTH, 0, 0);

    // If none, we can do the cheap scenario
    if (!c4Len)
    {
        strToFill.Clear();
        return 0;
    }

    //
    //  There's something to get, so make sure the string is big enough. If
    //  we reallocate, tell it not to retain any existing content.
    //
    if (strToFill.c4BufChars() < c4Len)
        strToFill.Reallocate(c4Len, kCIDLib::False);

    return ::SendMessageW
    (
        hwndSrc
        , WM_GETTEXT
        , c4Len + 1
        , tCIDLib::TInt4(strToFill.pszBuffer())
    );
}


// Return the height or width of the standard scroll bars
tCIDLib::TCard4 TFacCIDCtrls::c4ScrollBarHeight() const
{
    return tCIDLib::TCard4(::GetSystemMetrics(SM_CYHSCROLL));
}

tCIDLib::TCard4 TFacCIDCtrls::c4ScrollBarWidth() const
{
    return tCIDLib::TCard4(::GetSystemMetrics(SM_CXVSCROLL));
}


//
//  Provide access to our maapping of dialog item types to C++ classes. The
//  resource editor will use this to load up a list of available item types.
//
const tCIDLib::TKVPList& TFacCIDCtrls::colDlgClasses() const
{
    return m_colDlgClasses;
}


// Limit the movement of the pointer to the indicated area
tCIDLib::TVoid TFacCIDCtrls::ConfinePointer(const TArea& areaOnScreen)
{
    // Set the cursor confinement to the confine area
    tCIDLib::THostRectl rectlConfine;
    areaOnScreen.ToRectl(rectlConfine);
    ::ClipCursor((RECT*)&rectlConfine);
}


//
//  For custom windows that need to support sizing in the standard sort of way, we
//  provide this generic hit test helper that they can call in response to a hit
//  test query from the system. It tests for side/corner proximity and returns
//  the appropriate window area indicator.
//
tCIDCtrls::EWndAreas
TFacCIDCtrls::eGenericHitCalc(  const   TPoint&         pntToCheck
                                , const TArea&          areaToCheck
                                , const tCIDLib::TCard4 c4BorderWidth)
{
    // If the point isn't inside the area, that's bad
    if (!areaToCheck.bContainsPoint(pntToCheck))
        return tCIDCtrls::EWndAreas::Nowhere;

    TArea areaBorder(areaToCheck);
    areaBorder.Deflate(c4BorderWidth);
    const tCIDLib::TInt4 i4Width = tCIDLib::TInt4(c4BorderWidth);
    const tCIDLib::TInt4 i4SenseArea = tCIDLib::TInt4(c4BorderWidth << 1);
    tCIDLib::TCard1 c1Pos = 0;
    if (pntToCheck.i4X() < areaToCheck.i4X() + tCIDLib::TInt4(i4SenseArea))
        c1Pos = 1;
    else if (pntToCheck.i4X() > areaBorder.i4Right() - i4Width)
        c1Pos = 2;

    if (pntToCheck.i4Y() < areaToCheck.i4Y() + tCIDLib::TInt4(i4SenseArea))
        c1Pos |= 4;
    else if (pntToCheck.i4Y() > areaBorder.i4Bottom() - i4Width)
        c1Pos |= 8;

    //
    //  A lookup table which will translate the bitmapped value we created
    //  above into the correct enumerated value.
    //
    static const tCIDCtrls::EWndAreas m_aeTable[11] =
    {                                     // B T R L
        tCIDCtrls::EWndAreas::Client        // 0 0 0 0
        , tCIDCtrls::EWndAreas::Left        // 0 0 0 1
        , tCIDCtrls::EWndAreas::Right       // 0 0 1 0
        , tCIDCtrls::EWndAreas::Nowhere     // 0 0 1 1
        , tCIDCtrls::EWndAreas::Top         // 0 1 0 0
        , tCIDCtrls::EWndAreas::TopLeft     // 0 1 0 1
        , tCIDCtrls::EWndAreas::TopRight    // 0 1 1 0
        , tCIDCtrls::EWndAreas::Nowhere     // 0 1 1 1
        , tCIDCtrls::EWndAreas::Bottom      // 1 0 0 0
        , tCIDCtrls::EWndAreas::BottomLeft  // 1 0 0 1
        , tCIDCtrls::EWndAreas::BottomRight // 1 0 1 0
    };
    return m_aeTable[c1Pos];
}


//
// Exit the current message loop
//
tCIDLib::TVoid TFacCIDCtrls::ExitLoop(const tCIDLib::TCard4 c4Code)
{
    ::PostQuitMessage(c4Code);
}

tCIDLib::TVoid TFacCIDCtrls::ExitLoop(const tCIDLib::EExitCodes eCode)
{
    ::PostQuitMessage(tCIDLib::c4EnumOrd(eCode));
}


//
//  Used internally to flash the window, which is generally for saying, sorry
//  you are locked out of that right now.
//
tCIDLib::TVoid TFacCIDCtrls::FlashWindow(const tCIDCtrls::TWndHandle hwndTarget)
{
    FLASHWINFO FlashInfo;
    FlashInfo.cbSize = sizeof(FLASHWINFO);
    FlashInfo.hwnd = hwndTarget;
    FlashInfo.dwFlags = FLASHW_CAPTION;
    FlashInfo.uCount = 4;
    FlashInfo.dwTimeout = 50;
    ::FlashWindowEx(&FlashInfo);
}


// HIdes the pointer (cursor in Windows terms)
tCIDLib::TVoid TFacCIDCtrls::HidePointer()
{
    ::ShowCursor(0);
}


// Return the handle of the child of the indicated parent with the indicated id
tCIDCtrls::TWndHandle
TFacCIDCtrls::hwndChildFromId(  const   tCIDCtrls::TWndHandle   hwndParent
                                , const tCIDCtrls::TWndId       widChildId)
{
    return ::GetDlgItem(hwndParent, widChildId);
}


// Gets the window handle for the indicated child window
tCIDCtrls::TWndHandle
TFacCIDCtrls::hwndChildFromText(const   tCIDCtrls::TWndHandle   hwndParent
                                ,       tCIDCtrls::TWndId&      widChildId
                                , const TString&                strText
                                , const tCIDLib::TBoolean       bPrefix
                                , const tCIDLib::TBoolean       bDirectChild)
{
    const tCIDLib::TCard4 c4BufSz = 1024;
    tCIDLib::TCh achBuf[c4BufSz + 1];

    tCIDCtrls::TWndHandle hwndRet = kCIDCtrls::hwndInvalid;
    if (bDirectChild)
    {
        hwndRet = ::GetWindow(hwndParent, GW_CHILD);
        while (hwndRet)
        {
            // See if this window has the expected text
            ::GetWindowText(hwndRet, achBuf, c4BufSz);

            if (bPrefix)
            {
                if (strText.bStartsWith(achBuf))
                    break;
            }
             else
            {
                if (strText == achBuf)
                    break;
            }

            // Try the next window at this level
            hwndRet = ::GetWindow(hwndRet, GW_HWNDNEXT);
        }
    }
     else
    {
        hwndRet = hwndFindDescendant(hwndParent, strText, achBuf, c4BufSz, bPrefix);
    }

    if (hwndRet == kCIDCtrls::hwndInvalid)
        widChildId = 0;
    else
        widChildId = ::GetWindowLong(hwndRet, GWL_ID);
    return hwndRet;
}



// Gets the descendant under the passed window that has the focus, if any
tCIDCtrls::TWndHandle
TFacCIDCtrls::hwndChildWithFocus(const tCIDCtrls::TWndHandle hwndParent)
{
    HWND hwndFocus = ::GetFocus();

    // If nobody got da focus, return zero
    if (!hwndFocus)
        return 0;

    // If it's not even a descendant of any type, then return zero
    if (!::IsChild(hwndParent, hwndFocus))
        return 0;

    // Looks ok, so return it
    return hwndFocus;
}


tCIDCtrls::TWndHandle
TFacCIDCtrls::hwndFindAppWnd( const TExternalProcess&   extpTarget
                            , const TString&            strWndText
                            , const TString&            strClass
                            , const tCIDLib::TBoolean   bThrowIfNot)
{
    //
    //  Iterate the top level windows and look for one with the given
    //  window text and which belongs ot the indicated target process,
    //  and optionally with the indicated window class.
    //
    CIDCtrls_ThisFacility::TFindWndText FindData = {0};
    FindData.bStartsWith = kCIDLib::True;
    FindData.c4TextLen = strWndText.c4Length();
    FindData.hwndFound = kCIDCtrls::hwndInvalid;
    FindData.pidTarget = extpTarget.pidThis();
    FindData.pszText = strWndText.pszBuffer();

    if (!strClass.bIsEmpty())
        FindData.pszClass = strClass.pszBuffer();

    // Set up a system buffer for the callback to use to query window text
    TSysBuf mbufTmp(8192, 8192);
    FindData.pchTmpBuf = (tCIDLib::TCh*)mbufTmp.pc1Data();
    FindData.c4TmpBufSz = 8191;

    ::EnumWindows(&bEnumWndText, (LPARAM)&FindData);
    if (FindData.hwndFound == kCIDCtrls::hwndInvalid)
    {
        if (bThrowIfNot)
        {
            TKrnlError::SetLastHostError(::GetLastError());
            facCIDCtrls().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCtrlsErrs::errcWnd_EnumWindows
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
        return kCIDCtrls::hwndInvalid;
    }
    return FindData.hwndFound;
}


// Finds a visible, top level, frame window for the passed process
tCIDCtrls::TWndHandle
TFacCIDCtrls::hwndFindTopAppWnd(const   TExternalProcess&   extpTarget
                                , const TString&            strClass
                                , const tCIDLib::TBoolean   bThrowIfNot)
{
    //
    //  Iterate the top level windows and look for one that belows to the
    //  passed proces, and which optionally has the indicated window class.
    //
    CIDCtrls_ThisFacility::TFindWndFiltered FindData = {0};
    FindData.bOwned = kCIDLib::False;
    FindData.c4DontWantStyles = WS_CHILD;
    FindData.c4DontWantExStyles = WS_EX_NOACTIVATE;
    FindData.c4WantStyles = WS_VISIBLE | WS_CAPTION;
    FindData.hwndFound = kCIDCtrls::hwndInvalid;
    FindData.pidTarget = extpTarget.pidThis();

    if (!strClass.bIsEmpty())
        FindData.pszClass = strClass.pszBuffer();

    ::EnumWindows(&bEnumWndFiltered, (LPARAM)&FindData);
    if (FindData.hwndFound == kCIDCtrls::hwndInvalid)
    {
        // It failed, so try a less picky search
        FindData.c4WantStyles = WS_VISIBLE;
        ::EnumWindows(&bEnumWndFiltered, (LPARAM)&FindData);
        if (FindData.hwndFound == kCIDCtrls::hwndInvalid)
        {
            // Still failed so give up
            if (bThrowIfNot)
            {
                TKrnlError::SetLastHostError(::GetLastError());
                facCIDCtrls().ThrowKrnlErr
                (
                    CID_FILE
                    , CID_LINE
                    , kCtrlsErrs::errcWnd_EnumWindows
                    , TKrnlError::kerrLast()
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::CantDo
                );
            }
            return kCIDCtrls::hwndInvalid;
        }
    }
    return FindData.hwndFound;
}


//
//  Find the child of the passed parent, with the indicated id. It can be
//  limited to direct children or a recursive search.
//
tCIDCtrls::TWndHandle
TFacCIDCtrls::hwndFindChild(const   tCIDCtrls::TWndHandle   hwndParent
                            , const tCIDCtrls::TWndId       widChildId
                            , const tCIDLib::TBoolean       bDirectChild)
{
    if (bDirectChild)
        return ::GetDlgItem(hwndParent, widChildId);

    //
    //  They want any child below the passed window which has the indicated
    //  id, so we have to do a recursive iteration.
    //
    return hwndFindDescendant(hwndParent, widChildId);
}


//
//  Find the Nth child of the parent. If the bFirst flag is set, then we go
//  from the first child forward, else form the last child backwards to get
//  to the c4Ofs'th child.
//
tCIDCtrls::TWndHandle
TFacCIDCtrls::hwndNthChildFrom( const   tCIDCtrls::TWndHandle   hwndParent
                                , const tCIDLib::TBoolean       bFirst
                                , const tCIDLib::TCard4         c4Ofs)
{
    tCIDCtrls::TWndHandle hwndRet = 0;
    tCIDLib::TCard4 c4Count = 0;
    if (bFirst)
    {
        hwndRet = ::GetWindow(hwndParent, GW_CHILD);
        while (hwndRet && (c4Count < c4Ofs))
        {
            hwndRet = ::GetWindow(hwndRet, GW_HWNDNEXT);
            c4Count++;
        }
    }
     else
    {
        hwndRet = ::GetWindow(hwndParent, GW_CHILD);
        if (hwndRet)
        {
            hwndRet = ::GetWindow(hwndRet, GW_HWNDLAST);
            while (hwndRet && (c4Count < c4Ofs))
            {
                hwndRet = ::GetWindow(hwndRet, GW_HWNDPREV);
                c4Count++;
            }
        }
    }
    return hwndRet;
}


//
//  This will find the root parent of the starting window, and then get the owner of that
//  window. This is used typically when dealing with secondary frames and we need to see
//  if it has the same ultimate owner as it's own immediate owner window. If we have no
//  parent, then we return an invalid handle
//
tCIDCtrls::TWndHandle TFacCIDCtrls::hwndRootOwner(const tCIDCtrls::TWndHandle hwndStart)
{
    HWND hwndRootOwner = ::GetAncestor(hwndStart, GA_ROOT);
    if (hwndRootOwner)
        hwndRootOwner = ::GetWindow(hwndRootOwner, GW_OWNER);

    return hwndRootOwner;
}


// Find the top-most containing parent window of the passed window
tCIDCtrls::TWndHandle TFacCIDCtrls::hwndTopParent(const tCIDCtrls::TWndHandle hwndStart)
{
    return ::GetAncestor(hwndStart, GA_ROOT);
}

//
//  Sends an async message to the target window.
//
tCIDCtrls::TMsgResult
TFacCIDCtrls::mresSendAsyncMsg(         tCIDCtrls::TWndHandle   hwndTar
                                , const tCIDCtrls::TWndMsg      wmsgToSend
                                , const tCIDCtrls::TWParam      wParam
                                , const tCIDCtrls::TLParam      lParam)
{
    if (!::PostMessageW(hwndTar, wmsgToSend, wParam, lParam))
        return kCIDLib::False;
    return kCIDLib::True;
}


// Sends a synchronous message ot the target window
tCIDCtrls::TMsgResult
TFacCIDCtrls::mresSendSyncMsg(          tCIDCtrls::TWndHandle   hwndTar
                                , const tCIDCtrls::TWndMsg      wmsgToSend
                                , const tCIDCtrls::TWParam      wParam
                                , const tCIDCtrls::TLParam      lParam)
{
    if (!::SendMessageW(hwndTar, wmsgToSend, wParam, lParam))
        return kCIDLib::False;
    return kCIDLib::True;
}


// Moves the point to the indicated position relative to the indicated window
tCIDLib::TVoid
TFacCIDCtrls::MovePointer(const TPoint& pntNewPos, const TWindow& wndRelativeTo)
{
    // Make the point relative to the screen
    POINT ptActual;
    ptActual.x = pntNewPos.i4X();
    ptActual.y = pntNewPos.i4Y();
    ::ClientToScreen(wndRelativeTo.hwndThis(), &ptActual);
    if (!::SetCursorPos(ptActual.x, ptActual.y))
    {
        // If this does not work, something horrible is happening, so its fatal
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDCtrls().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcFac_SetPointerPos
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::Internal
        );
    }
}


//
//  Eat up to 64 messages or the timeout period, as long as there are
//  messages in the queue to process.
//
tCIDLib::TVoid TFacCIDCtrls::MsgYield(const tCIDLib::TCard4 c4WaitFor)
{
    MSG msgCur;
    if (!c4WaitFor)
    {
        //
        //  It's purely driven by message count in this case, so fall
        //  out as soon as we don't have a message or we max out.
        //
        tCIDLib::TCard4 c4Counter = 0;
        while (::PeekMessage(&msgCur, 0, 0, 0, PM_REMOVE))
        {
            tCIDLib::TBoolean bDispatch = kCIDLib::True;
            if (m_pwndTopWnd)
            {
                if ((msgCur.message == WM_KEYDOWN)
                ||  (msgCur.message == WM_SYSKEYDOWN))
                {
                    // It's possibly an accellerator, so check it
                    bDispatch = !m_pwndTopWnd->bProcessAccel(&msgCur);
                }
            }

            // If not accellerated, then dispatch it
            if (bDispatch)
            {
                ::TranslateMessage(&msgCur);
                ::DispatchMessage(&msgCur);
            }

            c4Counter++;
            if (c4Counter > 64)
                break;
        }
    }
     else
    {
        // It's time based in this case
        tCIDLib::TEncodedTime enctCur = TTime::enctNow();
        const tCIDLib::TEncodedTime enctEnd
        (
            enctCur + (c4WaitFor * kCIDLib::enctOneMilliSec)
        );

        while (enctCur < enctEnd)
        {
            if (::PeekMessage(&msgCur, 0, 0, 0, PM_REMOVE))
            {
                tCIDLib::TBoolean bDispatch = kCIDLib::True;
                if (m_pwndTopWnd)
                {
                    if ((msgCur.message == WM_KEYDOWN)
                    ||  (msgCur.message == WM_SYSKEYDOWN))
                    {
                        // It's possibly an accellerator, so check it
                        bDispatch = !m_pwndTopWnd->bProcessAccel(&msgCur);
                    }
                }

                if (bDispatch)
                {
                    ::TranslateMessage(&msgCur);
                    ::DispatchMessage(&msgCur);
                }
            }
             else
            {
                TThread::Sleep(2);
            }
            enctCur = TTime::enctNow();
        }
    }
}


//
//  Just adjusts the pointer position from wherever it is now, so it is
//  independent of any window.
//
tCIDLib::TVoid TFacCIDCtrls::OffsetPointer(const TPoint& pntAmount)
{
    //
    //  This can fail if the window station isn't valid, so do nothing in
    //  that case.
    //
    POINT ptTmp;
    if (::GetCursorPos(&ptTmp))
    {
        ptTmp.x += pntAmount.i4X();
        ptTmp.y += pntAmount.i4Y();

        if (!::SetCursorPos(ptTmp.x, ptTmp.y))
        {
            TKrnlError::SetLastHostError(::GetLastError());
            facCIDCtrls().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCtrlsErrs::errcFac_SetPointerPos
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::ProcFatal
                , tCIDLib::EErrClasses::Internal
            );
        }
    }
}


const TWindow* TFacCIDCtrls::pwndActiveTopWnd() const
{
    return m_pwndTopWnd;
}


// Return the pointer position, relative to the passed window's coordinates
TPoint TFacCIDCtrls::pntPointerPos(const TWindow& wndRelativeTo) const
{
    //
    //  This can sometimes fail when the blanker or logon window is up, so we
    //  have to check for it, but cannot throw, because it's not a real error.
    //  It will sometimes return bogus errors.
    //
    POINT ptTmp;
    if (!::GetCursorPos(&ptTmp))
        return TPoint(0);

    // Convert to window coordinates
    RECT rectCur;
    if (!::GetWindowRect(wndRelativeTo.hwndThis(), &rectCur))
    {
        if (!bDevErrToIgnore())
        {
            facCIDCtrls().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCtrlsErrs::errcWnd_QueryArea
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , TCardinal(wndRelativeTo.widThis())
            );
        }
        return TPoint(0);
    }

    return TPoint
    (
        ptTmp.x - rectCur.left
        , ptTmp.y - rectCur.top
    );
}


//
//  This guy is a special tool to get the focus on a window in another
//  application, which has special considerations.
//
tCIDLib::TVoid
TFacCIDCtrls::PrepForeignFocus( const   tCIDCtrls::TWndHandle  hwndCaller
                                , const tCIDCtrls::TWndHandle  hwndTarget)
{
    // If the target window doesn't have the focus, we have to do something
    if (hwndTarget != ::GetFocus())
    {
        //
        //  First the caller has to come forward in order to be able to
        //  have the right to get the foreign target forward.
        //
        SetForeground(hwndCaller, kCIDLib::True);

        //
        //  And now set the target as the foreground. This will really
        //  put it's top level parent as the active window and put the
        //  focus on the target (if the target itself isn't a top level
        //  frame.)
        //
        SetForeground(hwndTarget, kCIDLib::True);
    }
}



//
//  Queries the monitor area for the monitor that contains the bulk of the
//  passed window. It can return the whole area, or the area that doesn't
//  include any docked toolbars.
//
tCIDLib::TVoid
TFacCIDCtrls::QueryMonArea( const   TWindow&            wndSrc
                            ,       TArea&              areaToFill
                            , const tCIDLib::TBoolean   bAvailOnly)
{
    // Get the monitor that contains the most of the parent window
    HMONITOR hMon = ::MonitorFromWindow(wndSrc.hwndThis(), MONITOR_DEFAULTTONEAREST);

    // Call a local helper to do the work
    QueryMonitorArea(hMon, areaToFill, bAvailOnly);
}



//
//  Query some basic info about all of the monitors installed in this system
//
tCIDLib::TVoid TFacCIDCtrls::QueryMonitors(TCollection<TGUIMonInfo>& colToFill)
{
    // Make sure it's initially empty
    colToFill.RemoveAll();
    ::EnumDisplayMonitors(0, 0, &MonitorEnumCB2, (LPARAM)&colToFill);
}


//
//  Get one of the standard system colors
//
//  THIS MUST be kept int sync with the system colors enum!
//
TRGBClr TFacCIDCtrls::rgbSysClr(const tCIDCtrls::ESysColors eClr) const
{
    static const int aiColorMap[] =
    {
        -1
        , COLOR_3DHILIGHT
        , COLOR_APPWORKSPACE
        , COLOR_HIGHLIGHT
        , COLOR_BTNFACE
        , COLOR_HIGHLIGHTTEXT
        , COLOR_BTNTEXT
        , COLOR_WINDOW
        , COLOR_WINDOWTEXT
    };

    TRGBClr rgbRet;
    switch(eClr)
    {
        case tCIDCtrls::ESysColors::Highlight3D :
        case tCIDCtrls::ESysColors::AppWorkspace :
        case tCIDCtrls::ESysColors::HighlightBgn :
        case tCIDCtrls::ESysColors::HighlightBgnNF :
        case tCIDCtrls::ESysColors::HighlightText :
        case tCIDCtrls::ESysColors::HighlightTextNF :
        case tCIDCtrls::ESysColors::Window :
        case tCIDCtrls::ESysColors::WindowText :
        {
            DWORD clrVal = ::GetSysColor(aiColorMap[tCIDLib::c4EnumOrd(eClr)]);
            rgbRet.Set(GetRValue(clrVal), GetGValue(clrVal), GetBValue(clrVal));
            break;
        }

        // These are ones we create ourself
        case tCIDCtrls::ESysColors::DialogBgn :
            rgbRet.Set(0xF0, 0xF0, 0xF0);
            break;

        case tCIDCtrls::ESysColors::DlgCtrlFill :
            rgbRet.Set(0xFF, 0xFF, 0xFF);
            break;

        case tCIDCtrls::ESysColors::EditWndBgn :
            rgbRet.Set(0x80, 0x80, 0x80);
            break;

        case tCIDCtrls::ESysColors::None :
        default :
            rgbRet.ToBlack();
            break;
    };
    return rgbRet;
}


// Release a previous mouse confinement call
tCIDLib::TVoid TFacCIDCtrls::ReleasePointer()
{
    ::ClipCursor(0);
}


//
//  Given the dialog item type from the resource file, for a dialog item, return
//  the C++ class type.
//
const TString& TFacCIDCtrls::strXlatDlgClass(const TString& strItemType) const
{
    const tCIDLib::TCard4 c4Count = m_colDlgClasses.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        if (strItemType == m_colDlgClasses[c4Index].strKey())
            return m_colDlgClasses[c4Index].strValue();
    }

    facCIDCtrls().ThrowErr
    (
        CID_FILE
        , CID_LINE
        , kCtrlsErrs::errcDlgD_ItemType
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::NotFound
        , strItemType
    );

    // Make the compiler happy
    return TString::strEmpty();
}


//
//  Force regular or extended keys into the containing program's input queue.
//
tCIDLib::TVoid
TFacCIDCtrls::SendExtKey(const  tCIDCtrls::EExtKeys eKeyToSend
                        , const tCIDLib::TBoolean   bAltShifted
                        , const tCIDLib::TBoolean   bCtrlShifted
                        , const tCIDLib::TBoolean   bShifted)
{
    //
    //  Fill in an input event, which lets us fake input events to programs
    //  without having to play a lot of tricks. We have to do up to 8
    //  events, according to the shift states he wants to do.
    //
    tCIDLib::TCard4 c4Count = 0;
    INPUT aInputEvs[8];

    if (bAltShifted)
    {
        aInputEvs[c4Count].type = INPUT_KEYBOARD;
        aInputEvs[c4Count].ki.wVk = VK_MENU;
        aInputEvs[c4Count].ki.wScan = 0;
        aInputEvs[c4Count].ki.dwFlags = 0;
        aInputEvs[c4Count].ki.time = 0;
        aInputEvs[c4Count].ki.dwExtraInfo = 0;
        c4Count++;
    }

    if (bCtrlShifted)
    {
        aInputEvs[c4Count].type = INPUT_KEYBOARD;
        aInputEvs[c4Count].ki.wVk = VK_CONTROL;
        aInputEvs[c4Count].ki.wScan = 0;
        aInputEvs[c4Count].ki.dwFlags = 0;
        aInputEvs[c4Count].ki.time = 0;
        aInputEvs[c4Count].ki.dwExtraInfo = 0;
        c4Count++;
    }

    if (bShifted)
    {
        aInputEvs[c4Count].type = INPUT_KEYBOARD;
        aInputEvs[c4Count].ki.wVk = VK_SHIFT;
        aInputEvs[c4Count].ki.wScan = 0;
        aInputEvs[c4Count].ki.dwFlags = 0;
        aInputEvs[c4Count].ki.time = 0;
        aInputEvs[c4Count].ki.dwExtraInfo = 0;
        c4Count++;
    }

    aInputEvs[c4Count].type = INPUT_KEYBOARD;
    aInputEvs[c4Count].ki.wVk = c2XlatExtKey(eKeyToSend);
    aInputEvs[c4Count].ki.wScan = 0;
    aInputEvs[c4Count].ki.dwFlags = 0;
    aInputEvs[c4Count].ki.time = 0;
    aInputEvs[c4Count].ki.dwExtraInfo = 0;
    c4Count++;

    aInputEvs[c4Count].type = INPUT_KEYBOARD;
    aInputEvs[c4Count].ki.wVk = c2XlatExtKey(eKeyToSend);
    aInputEvs[c4Count].ki.wScan = 0;
    aInputEvs[c4Count].ki.dwFlags = KEYEVENTF_KEYUP;
    aInputEvs[c4Count].ki.time = 0;
    aInputEvs[c4Count].ki.dwExtraInfo = 0;
    c4Count++;

    if (bShifted)
    {
        aInputEvs[c4Count].type = INPUT_KEYBOARD;
        aInputEvs[c4Count].ki.wVk = VK_SHIFT;
        aInputEvs[c4Count].ki.wScan = 0;
        aInputEvs[c4Count].ki.dwFlags = KEYEVENTF_KEYUP;
        aInputEvs[c4Count].ki.time = 0;
        aInputEvs[c4Count].ki.dwExtraInfo = 0;
        c4Count++;
    }

    if (bCtrlShifted)
    {
        aInputEvs[c4Count].type = INPUT_KEYBOARD;
        aInputEvs[c4Count].ki.wVk = VK_CONTROL;
        aInputEvs[c4Count].ki.wScan = 0;
        aInputEvs[c4Count].ki.dwFlags = KEYEVENTF_KEYUP;
        aInputEvs[c4Count].ki.time = 0;
        aInputEvs[c4Count].ki.dwExtraInfo = 0;
        c4Count++;
    }

    if (bAltShifted)
    {
        aInputEvs[c4Count].type = INPUT_KEYBOARD;
        aInputEvs[c4Count].ki.wVk = VK_MENU;
        aInputEvs[c4Count].ki.wScan = 0;
        aInputEvs[c4Count].ki.dwFlags = KEYEVENTF_KEYUP;
        aInputEvs[c4Count].ki.time = 0;
        aInputEvs[c4Count].ki.dwExtraInfo = 0;
        c4Count++;
    }

    if (!::SendInput(c4Count, aInputEvs, sizeof(INPUT)))
    {
        facCIDCtrls().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcWnd_CantSendInput
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
}

tCIDLib::TVoid
TFacCIDCtrls::SendKey(const tCIDLib::TCh        chToSend
                    , const tCIDLib::TBoolean   bAltShifted
                    , const tCIDLib::TBoolean   bCtrlShifted
                    , const tCIDLib::TBoolean   bShifted)
{
    //
    //  Fill in an input event, which lets us fake input events to programs
    //  without having to play a lot of tricks. We have to send up to 6
    //  events according to the shifts they want to use.
    //
    tCIDLib::TCard4 c4Count = 0;
    INPUT aInputEvs[8];

    if (bAltShifted)
    {
        aInputEvs[c4Count].type = INPUT_KEYBOARD;
        aInputEvs[c4Count].ki.wVk = VK_MENU;
        aInputEvs[c4Count].ki.wScan = 0;
        aInputEvs[c4Count].ki.dwFlags = 0;
        aInputEvs[c4Count].ki.time = 0;
        aInputEvs[c4Count].ki.dwExtraInfo = 0;
        c4Count++;
    }

    if (bCtrlShifted)
    {
        aInputEvs[c4Count].type = INPUT_KEYBOARD;
        aInputEvs[c4Count].ki.wVk = VK_CONTROL;
        aInputEvs[c4Count].ki.wScan = 0;
        aInputEvs[c4Count].ki.dwFlags = 0;
        aInputEvs[c4Count].ki.time = 0;
        aInputEvs[c4Count].ki.dwExtraInfo = 0;
        c4Count++;
    }

    if (bShifted)
    {
        aInputEvs[c4Count].type = INPUT_KEYBOARD;
        aInputEvs[c4Count].ki.wVk = VK_SHIFT;
        aInputEvs[c4Count].ki.wScan = 0;
        aInputEvs[c4Count].ki.dwFlags = 0;
        aInputEvs[c4Count].ki.time = 0;
        aInputEvs[c4Count].ki.dwExtraInfo = 0;
        c4Count++;
    }

    aInputEvs[c4Count].type = INPUT_KEYBOARD;
    aInputEvs[c4Count].ki.wVk = (::VkKeyScan(chToSend) & 0xFF);
    aInputEvs[c4Count].ki.wScan = 0;
    aInputEvs[c4Count].ki.dwFlags = 0;
    aInputEvs[c4Count].ki.time = 0;
    aInputEvs[c4Count].ki.dwExtraInfo = 0;
    c4Count++;

    aInputEvs[c4Count].type = INPUT_KEYBOARD;
    aInputEvs[c4Count].ki.wVk = (::VkKeyScan(chToSend) & 0xFF);
    aInputEvs[c4Count].ki.wScan = 0;
    aInputEvs[c4Count].ki.dwFlags = KEYEVENTF_KEYUP;
    aInputEvs[c4Count].ki.time = 0;
    aInputEvs[c4Count].ki.dwExtraInfo = 0;
    c4Count++;

    if (bShifted)
    {
        aInputEvs[c4Count].type = INPUT_KEYBOARD;
        aInputEvs[c4Count].ki.wVk = VK_SHIFT;
        aInputEvs[c4Count].ki.wScan = 0;
        aInputEvs[c4Count].ki.dwFlags = KEYEVENTF_KEYUP;
        aInputEvs[c4Count].ki.time = 0;
        aInputEvs[c4Count].ki.dwExtraInfo = 0;
        c4Count++;
    }

    if (bCtrlShifted)
    {
        aInputEvs[c4Count].type = INPUT_KEYBOARD;
        aInputEvs[c4Count].ki.wVk = VK_CONTROL;
        aInputEvs[c4Count].ki.wScan = 0;
        aInputEvs[c4Count].ki.dwFlags = KEYEVENTF_KEYUP;
        aInputEvs[c4Count].ki.time = 0;
        aInputEvs[c4Count].ki.dwExtraInfo = 0;
        c4Count++;
    }

    if (bAltShifted)
    {
        aInputEvs[c4Count].type = INPUT_KEYBOARD;
        aInputEvs[c4Count].ki.wVk = VK_MENU;
        aInputEvs[c4Count].ki.wScan = 0;
        aInputEvs[c4Count].ki.dwFlags = KEYEVENTF_KEYUP;
        aInputEvs[c4Count].ki.time = 0;
        aInputEvs[c4Count].ki.dwExtraInfo = 0;
        c4Count++;
    }

    if (!::SendInput(c4Count, aInputEvs, sizeof(INPUT)))
    {
        facCIDCtrls().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcWnd_CantSendInput
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
}


// Sends a close message to the target window
tCIDLib::TVoid TFacCIDCtrls::SendShutdown(const tCIDCtrls::TWndHandle hwndTar)
{
    mresSendAsyncMsg(hwndTar, WM_CLOSE, 0, 0);
}


// Sets a new active top level window to get msgs
tCIDLib::TVoid TFacCIDCtrls::SetActiveTopWnd(const TWindow* const pwndToSet)
{
    m_pwndTopWnd = pwndToSet;
}


//
//  Used to push in an owner usually when this program wants to make an other process'
//  window its owner, so it will stay in front of that other window.
//
tCIDLib::TVoid
TFacCIDCtrls::SetExtOwner(  const   tCIDCtrls::TWndHandle hwndOwned
                            , const tCIDCtrls::TWndHandle hwndOwner)
{
    ::SetWindowLong(hwndOwned, GWL_HWNDPARENT, (LONG)hwndOwner);
}



// Set the passed window as the focus window, optionally forcing it
tCIDLib::TVoid
TFacCIDCtrls::SetFocus( const   tCIDCtrls::TWndHandle   hwndTar
                        , const tCIDLib::TBoolean       bForce)
{
    // Get the focus window and if it's our target, nothing to do
    if (::GetFocus() == hwndTar)
        return;

    // We have to do it
    if (bForce)
    {
        //
        //  We need to get the foreground window, but there can be short
        //  periods when there isn't one, so we do a little looping and
        //  yielding to try to get one.
        //
        tCIDLib::TCard4 c4Count = 0;
        tCIDCtrls::TWndHandle hwndFgn = ::GetForegroundWindow();
        while (!hwndFgn && (c4Count < 5))
        {
            MsgYield(50);
            hwndFgn = ::GetForegroundWindow();
            c4Count++;
        }

        if (hwndFgn)
        {
            //
            //  Get the thread id of the foreground window and of the calling
            //  thread, and attach the two threads together
            //
            const tCIDLib::TThreadId tidFgn = ::GetWindowThreadProcessId(hwndFgn, 0);
            const tCIDLib::TThreadId tidThis = ::GetCurrentThreadId();
            ::AttachThreadInput(tidThis, tidFgn, 1);

            //
            //  Find the top level frame that contains this target window.
            //  We need to bring it forward and restore it if necessary,
            //  before giving the focus to any of it's children.
            //
            tCIDCtrls::TWndHandle hwndPar = ::GetAncestor(hwndTar, GA_ROOT);
            if (hwndPar != ::GetDesktopWindow())
            {
                WINDOWPLACEMENT WndPlace = {0};
                WndPlace.length = sizeof(WINDOWPLACEMENT);
                ::GetWindowPlacement(hwndPar, &WndPlace);
                WndPlace.flags = WPF_ASYNCWINDOWPLACEMENT;

                // If minimized, then show normal, else just show in current state
                if (WndPlace.showCmd== SW_SHOWMINIMIZED)
                    WndPlace.showCmd = SW_RESTORE;
                else
                    WndPlace.showCmd = SW_SHOW;
                ::SetWindowPlacement(hwndPar, &WndPlace);
            }

            // Now set the focus, and detach
            ::SetFocus(hwndTar);
            ::AttachThreadInput(tidThis, tidFgn, 0);
        }
         else
        {
            // Can't get one, so we just have to do the standard thing
            ::SetFocus(hwndTar);
        }
    }
     else
    {
        ::SetFocus(hwndTar);
    }
}


//
//  Brings the indicated window to the foreground, optinally forcing it literally
//  forward, or just letting the OS indicate it wants to come forward.
//
tCIDLib::TVoid
TFacCIDCtrls::SetForeground(const   tCIDCtrls::TWndHandle   hwndTar
                            , const tCIDLib::TBoolean       bForce)
{
    if (bForce)
    {
        //
        //  We need to get the foreground window, but there can be short
        //  periods when there isn't one, so we do a little looping and
        //  yielding to try to get one.
        //
        tCIDLib::TCard4 c4Count = 0;
        tCIDCtrls::TWndHandle hwndFgn = ::GetForegroundWindow();
        while (!hwndFgn && (c4Count < 5))
        {
            MsgYield(50);
            hwndFgn = ::GetForegroundWindow();
            c4Count++;
        }

        if (hwndFgn)
        {
            //
            //  Get the thread id of the foreground window and of the calling
            //  thread.
            //
            const tCIDLib::TThreadId tidFgn = ::GetWindowThreadProcessId(hwndFgn, 0);
            const tCIDLib::TThreadId tidThis = ::GetCurrentThreadId();

            // And attach the two threads together
            ::AttachThreadInput(tidThis, tidFgn, 1);

            //
            //  Get the window placement info and if the window is minimized,
            //  then restore it first.
            //
            WINDOWPLACEMENT WndPlace = {0};
            WndPlace.length = sizeof(WINDOWPLACEMENT);
            ::GetWindowPlacement(hwndTar, &WndPlace);
            WndPlace.flags = WPF_ASYNCWINDOWPLACEMENT;

            // If minimized, then show normal, else just show in current state
            if (WndPlace.showCmd == SW_SHOWMINIMIZED)
            {
                WndPlace.showCmd = SW_RESTORE;
                ::SetWindowPlacement(hwndTar, &WndPlace);
            }

            // Now bring the window forward and then detach
            ::SetForegroundWindow(hwndTar);
            ::AttachThreadInput(tidThis, tidFgn, 0);
        }
         else
        {
            // Can't get one, so we just have to do the standard thing
            ::SetForegroundWindow(hwndTar);
        }
    }
     else
    {
        ::SetForegroundWindow(hwndTar);
    }
}


//
//  Force the monitor power state off or on. This is a bit tricky we need to do it
//  in a way that will work in all situations.
//
//  For power on, we force ourselves forward and fake a mouse move event into the
//  program's input queue. This seems to be the only way to reliably make it work.
//
//  For power off we use the SC_MONITORPOWER system command msg.
//
tCIDLib::TVoid
TFacCIDCtrls::SetMonitorPowerState(         TWindow&                wndCaller
                                    , const tCIDCtrls::EMonPowerSt  eToSet)
{
    if (eToSet == tCIDCtrls::EMonPowerSt::On)
    {
        // We have to force ourself forward for this to work
        SetForeground(wndCaller.hwndThis(), kCIDLib::True);

        INPUT aInputEvs[1];
        TRawMem::SetMemBuf(aInputEvs, tCIDLib::TCard1(0), sizeof(INPUT));

        // Move down/right one pixel, sleep a bit, then more back again
        aInputEvs[0].type = INPUT_MOUSE;
        aInputEvs[0].mi.dx = 1;
        aInputEvs[0].mi.dy = 1;
        ::SendInput(1, aInputEvs, sizeof(INPUT));

        TThread::Sleep(50);

        aInputEvs[0].mi.dx = -1;
        aInputEvs[0].mi.dy = -1;
        ::SendInput(1, aInputEvs, sizeof(INPUT));
    }
     else
    {
        // Only off and standby should be seen here, else do nothing
        LPARAM PState;
        if (eToSet == tCIDCtrls::EMonPowerSt::Off)
            PState = 2;
        else if (eToSet == tCIDCtrls::EMonPowerSt::Standby)
            PState = 1;
        else
            return;

        ::SendMessage(HWND_TOPMOST, WM_SYSCOMMAND, SC_MONITORPOWER, PState);
    }
}


// Set the pointer to one of the predefined system pointers
tCIDLib::TVoid TFacCIDCtrls::SetPointer(const tCIDCtrls::ESysPtrs eToSet)
{
    tCIDLib::TCh* pszRes = 0;
    switch(eToSet)
    {
        case tCIDCtrls::ESysPtrs::AppStarting :
            pszRes = IDC_APPSTARTING;
            break;

        case tCIDCtrls::ESysPtrs::Arrow :
            pszRes = IDC_ARROW;
            break;

        case tCIDCtrls::ESysPtrs::Cross :
            pszRes = IDC_CROSS;
            break;

        case tCIDCtrls::ESysPtrs::Hand :
            pszRes = IDC_HAND;
            break;

        case tCIDCtrls::ESysPtrs::Help :
            pszRes = IDC_HELP;
            break;

        case tCIDCtrls::ESysPtrs::IBeam :
            pszRes = IDC_IBEAM;
            break;

        case tCIDCtrls::ESysPtrs::No :
            pszRes = IDC_NO;
            break;

        case tCIDCtrls::ESysPtrs::SizeAll :
            pszRes = IDC_SIZEALL;
            break;

        case tCIDCtrls::ESysPtrs::SizeNESW :
            pszRes = IDC_SIZENESW;
            break;

        case tCIDCtrls::ESysPtrs::SizeNS :
            pszRes = IDC_SIZENS;
            break;

        case tCIDCtrls::ESysPtrs::SizeNWSE :
            pszRes = IDC_SIZENWSE;
            break;

        case tCIDCtrls::ESysPtrs::SizeWE :
            pszRes = IDC_SIZEWE;
            break;

        case tCIDCtrls::ESysPtrs::UpArrow :
            pszRes = IDC_UPARROW;
            break;

        case tCIDCtrls::ESysPtrs::Wait :
            pszRes = IDC_WAIT;
            break;

        default :
            pszRes = IDC_ARROW;
            break;
    }

    // Try to load this cursor
    HCURSOR hcurNew = ::LoadCursor(0, pszRes);
    if (!hcurNew)
    {
        if (!bDevErrToIgnore())
        {
            facCIDCtrls().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCtrlsErrs::errcFac_LoadSysPointer
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Internal
            );
        }
        return;
    }
    ::SetCursor(hcurNew);
}


//
//  Set the position and/or state state of the indicated window
//
tCIDLib::TVoid
TFacCIDCtrls::SetPosState(  const   tCIDCtrls::TWndHandle   hwndTar
                            , const tCIDCtrls::EPosStates   eToSet)
{
    // Get the current window placement info
    WINDOWPLACEMENT WndPlace = {0};
    WndPlace.length = sizeof(WINDOWPLACEMENT);
    ::GetWindowPlacement(hwndTar, &WndPlace);

    const tCIDLib::TCard4 c4Org = WndPlace.showCmd;
    switch(eToSet)
    {
        case tCIDCtrls::EPosStates::Maximized :
            if (WndPlace.showCmd != SW_SHOWMAXIMIZED)
                WndPlace.showCmd = SW_MAXIMIZE;
            break;

        case tCIDCtrls::EPosStates::Minimized :
            if (WndPlace.showCmd != SW_SHOWMINIMIZED)
                WndPlace.showCmd = SW_MINIMIZE;
            break;

        case tCIDCtrls::EPosStates::Restored :
            if (WndPlace.showCmd != SW_SHOWNORMAL)
                WndPlace.showCmd = SW_RESTORE;
            break;

        default :
            break;
    };

    if (c4Org != WndPlace.showCmd)
        ::SetWindowPlacement(hwndTar, &WndPlace);
}


//
//  The magic GUI app kick macro will call this to pass us the initial display
//  state. GUI programs should call ShowGUI() after init is complete, and we will
//  set this state.
//
tCIDLib::TVoid TFacCIDCtrls::SetInitShowState(const tCIDLib::TSInt iToSet)
{
    m_iInitShow = iToSet;
}


// Change a window's position in various ways
tCIDLib::TVoid
TFacCIDCtrls::SetWndPos(const  tCIDCtrls::TWndHandle    hwndTar
                        , const tCIDLib::TInt4          i4X
                        , const tCIDLib::TInt4          i4Y)
{
    ::SetWindowPos
    (
        hwndTar
        , 0
        , i4X
        , i4Y
        , 0
        , 0
        , SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER
    );
}

tCIDLib::TVoid
TFacCIDCtrls::SetWndSize(const  tCIDCtrls::TWndHandle   hwndTar
                        , const tCIDLib::TCard4         c4Width
                        , const tCIDLib::TCard4         c4Height)
{
    ::SetWindowPos
    (
        hwndTar
        , 0
        , 0
        , 0
        , c4Width
        , c4Height
        , SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER
    );
}

tCIDLib::TVoid
TFacCIDCtrls::SetWndSize(const tCIDCtrls::TWndHandle hwndTar, const TSize& szToSet)
{
    ::SetWindowPos
    (
        hwndTar
        , 0
        , 0
        , 0
        , szToSet.c4Width()
        , szToSet.c4Height()
        , SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER
    );
}


// Set the text of the indicated window
tCIDLib::TVoid
TFacCIDCtrls::SetWndText(const  tCIDCtrls::TWndHandle   hwndTar
                        , const TString&                strToSet)
{
    ::SendMessage(hwndTar, WM_SETTEXT, 0, (LPARAM)strToSet.pszBuffer());
}


//
//  This should be called by GUI apps after init is complete, to show the main
//  window.
//
tCIDLib::TVoid TFacCIDCtrls::ShowGUI(TWindow& wndMain)
{
    ::ShowWindow(wndMain.hwndSafe(), m_iInitShow);
}


// Snow the cursor
tCIDLib::TVoid TFacCIDCtrls::ShowPointer()
{
    ::ShowCursor(1);
}


// Show or hide the passed window
tCIDLib::TVoid
TFacCIDCtrls::ShowWindow(const  tCIDCtrls::TWndHandle   hwndTar
                        , const tCIDLib::TBoolean       bState)
{
    ::ShowWindow(hwndTar, bState ? SW_SHOW : SW_HIDE);
}


// Move this guy to the bottom of the Z-Order
tCIDLib::TVoid TFacCIDCtrls::ToBottom(const tCIDCtrls::TWndHandle hwndTar)
{
    ::SetWindowPos
    (
        hwndTar
        , HWND_BOTTOM
        , 0
        , 0
        , 0
        , 0
        , SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE
    );
}


//
//  The standard message loop handler. We make sure no C++ exceptions propogate out of
//  the message handler callbacks.
//
//  The caller gives us the initial main window. But, because of the stupid way Windows'
//  dispatching works, if there's more than one top level window, our loop has to change
//  which window it is dispatching for.
//
//  So this class provides a m_pwndTopWnd member, which top level windows can set when
//  they get activated, to make sure they are the one being dispatched to.
//
tCIDLib::TUInt TFacCIDCtrls::uMainMsgLoop(const TWindow& wndMain)
{
    tCIDLib::TBoolean bDone = kCIDLib::False;
    MSG               msgCur = {0};

    //
    //  Mark the calling thread as a message loop processing GUI thread.
    //  This will cause certain things to happen differently down in the
    //  threading code.
    //
    TThread::pthrCaller()->MarkAsGUIThread();

    // Set the incoming guy as the active top level window
    m_pwndTopWnd = &wndMain;

    // Set up an exception to log if we get an exception
    const TError errExcept
    (
        facCIDLib().strName()
        , CID_FILE
        , CID_LINE
        , kCtrlsErrs::errcGen_LoopExcept
        , facCIDCtrls().strMsg(kCtrlsErrs::errcGen_LoopExcept)
        , tCIDLib::ESeverities::Status
        , tCIDLib::EErrClasses::AppStatus
    );

    //
    //  We use a double loop here so that we don't have to set and remove an exception
    //  handler on every message. The outer loop lets us recover and get back into
    //  the message loop. We use a log limiter in case we get some sort of recurring
    //  exceptions quickly.
    //
    TLogLimiter loglimLogger;
    while (!bDone)
    {
        try
        {
            while (::GetMessage(&msgCur, 0, 0, 0) > 0)
            {
                //
                //  Give the main window a chance to processit as an accelerator.
                //  But reduce overhead by making sure it's a key message. Else
                //  we run it for every single message.
                //
                if (m_pwndTopWnd)
                {
                    tCIDLib::TBoolean bDispatch = kCIDLib::True;
                    if ((msgCur.message == WM_KEYDOWN)
                    ||  (msgCur.message == WM_SYSKEYDOWN))
                    {
                        // It's possibly an accellerator, so check it
                        bDispatch = !m_pwndTopWnd->bProcessAccel(&msgCur);
                    }

                    // If not accellerated, then dispatch it
                    if (bDispatch)
                    {
                        if (!::IsDialogMessage(m_pwndTopWnd->hwndThis(), &msgCur))
                        {
                            // Not eaten by the focus management, so send along
                            ::TranslateMessage(&msgCur);
                            ::DispatchMessage(&msgCur);
                        }
                    }
                }
                 else
                {
                    // Not much we can do but just dispatch it
                    ::TranslateMessage(&msgCur);
                    ::DispatchMessage(&msgCur);
                }
            }

            bDone = kCIDLib::True;
        }

        catch(TError& errToCatch)
        {
            TAudio::Beep(880, 200);
            if (loglimLogger.bLogErr(errToCatch, CID_FILE, CID_LINE))
            {
                TModule::LogEventObj(errExcept);

                TErrBox msgbErr
                (
                    L"UI Error", L"Unhandled exception in main msg loop. See the logs."
                );
                msgbErr.ShowIt(wndMain);
            }
        }

        catch(...)
        {
            TAudio::Beep(880, 200);
            if (loglimLogger.bLogErr(errExcept))
            {
                TErrBox msgbErr(L"UI Error", L"Unhandled system exception in main msg loop");
                msgbErr.ShowIt(wndMain);
            }
        }
    }
    return msgCur.wParam;
}



//
//  Given a stored window frame window area or origin, validate that it is
//  still a reasonable one, given our current monitor situation. In either case,
//  we get the virtual screen area and make sure it's within that.
//
tCIDLib::TVoid TFacCIDCtrls::ValidatePrevWindowArea(TArea& areaToVal)
{
    // If it's equal to the default window area, then don't mangle it
    if (areaToVal != areaDefWnd())
    {
        TArea areaVS = TGUIFacility::areaVirtualScreen();
        areaToVal.ForceWithin(areaVS, kCIDLib::True);
    }
}

tCIDLib::TVoid TFacCIDCtrls::ValidatePrevWindowPos(TPoint& pntToVal)
{
    // If it's equal to the default window pos, then don't mangle it
    if (pntToVal != areaDefWnd().pntOrg())
    {
        TArea areaVS = TGUIFacility::areaVirtualScreen();

        //
        //  If the point is not within this area, then set it to the origin of
        //  the virtual screen area.
        //
        if (!areaVS.bContainsPoint(pntToVal))
            pntToVal = areaVS.pntOrg();
    }
}



// ---------------------------------------------------------------------------
//  TFacCIDCtrls: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDCtrls::TWndHandle
TFacCIDCtrls::hwndFindDescendant( const   tCIDCtrls::TWndHandle hwndParent
                                , const tCIDCtrls::TWndId       widToFind)
{
    tCIDCtrls::TWndHandle hwndCur = ::GetWindow(hwndParent, GW_CHILD);
    while (hwndCur)
    {
        if (tCIDCtrls::TWndId(::GetWindowLong(hwndCur, GWL_ID)) == widToFind)
            break;

        // This window isn't it, so let's recurse. If that finds it, break out
        tCIDCtrls::TWndHandle hwndNew = hwndFindDescendant(hwndCur, widToFind);
        if (hwndNew)
        {
            hwndCur = hwndNew;
            break;
        }

        // Try the next window at this level
        hwndCur = ::GetWindow(hwndCur, GW_HWNDNEXT);
    }
    return hwndCur;
}

tCIDCtrls::TWndHandle
TFacCIDCtrls::hwndFindDescendant(const  tCIDCtrls::TWndHandle   hwndParent
                                , const TString&                strToFind
                                ,       tCIDLib::TCh* const     pszBuf
                                , const tCIDLib::TCard4         c4BufSz
                                , const tCIDLib::TBoolean       bPrefix)
{
    tCIDCtrls::TWndHandle hwndCur = ::GetWindow(hwndParent, GW_CHILD);
    while (hwndCur)
    {
        // See if this window has the expected text
        ::GetWindowText(hwndCur, pszBuf, c4BufSz);

        if (bPrefix)
        {
            if (strToFind.bStartsWith(pszBuf))
                break;
        }
         else
        {
            if (strToFind == pszBuf)
                break;
        }

        // This window isn't it, so let's recurse. If that finds it, break out
        tCIDCtrls::TWndHandle hwndNew = hwndFindDescendant
        (
            hwndCur
            , strToFind
            , pszBuf
            , c4BufSz
            , bPrefix
        );

        // If we found it, then break out
        if (hwndNew)
        {
            hwndCur = hwndNew;
            break;
        }

        // Try the next window at this level
        hwndCur = ::GetWindow(hwndCur, GW_HWNDNEXT);
    }
    return hwndCur;
}



