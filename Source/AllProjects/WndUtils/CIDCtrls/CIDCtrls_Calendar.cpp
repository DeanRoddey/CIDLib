//
// FILE NAME: CIDCtrls_Calendar.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/16/2015
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
//  This file implements the calendar control.
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
AdvRTTIDecls(TCalendar, TStdCtrlWnd)




// ---------------------------------------------------------------------------
//  CLASS: TCalendar
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// TCalendar: Constructors and Destructor
// ---------------------------------------------------------------------------
TCalendar::TCalendar() :

    TStdCtrlWnd()
    , m_eCalStyle(tCIDCtrls::ECalStyles::None)
{
}

TCalendar::~TCalendar()
{
}


// ---------------------------------------------------------------------------
// TCalendar: Public, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TCalendar::InitFromDesc(const   TWindow&                wndParent
                        , const TDlgItem&               dlgiSrc
                        , const tCIDCtrls::EWndThemes   eTheme)
{
    tCIDCtrls::ECalStyles eCalStyles = tCIDCtrls::ECalStyles::None;
    tCIDCtrls::EWndStyles eStyles = tCIDCtrls::EWndStyles
    (
        tCIDCtrls::EWndStyles::VisTabChild
    );

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_Group))
        eStyles |= tCIDCtrls::EWndStyles::Group;

    Create
    (
        wndParent
        , dlgiSrc.ridChild()
        , dlgiSrc.areaPos()
        , eStyles
        , eCalStyles
    );
}


tCIDLib::TVoid TCalendar::QueryHints(tCIDLib::TStrCollect& colToFill) const
{
    TParent::QueryHints(colToFill);
    colToFill.objAdd(kCIDCtrls::strHint_Group);
}


// ---------------------------------------------------------------------------
// TCalendar: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Just set up the styles and call our parent to create the control
tCIDLib::TVoid
TCalendar::Create(  const   TWindow&                wndParent
                    , const tCIDCtrls::TWndId       widThis
                    , const TArea&                  areaInit
                    , const tCIDCtrls::EWndStyles   eStyles
                    , const tCIDCtrls::ECalStyles   eCalStyle)
{
    // Store our calendar specific styles
    m_eCalStyle = eCalStyle;

    // Create the calendar control, forcing on any type specific styles we need
    CreateWnd
    (
        wndParent.hwndThis()
        , MONTHCAL_CLASS
        , kCIDLib::pszEmptyZStr
        , areaInit
        , eStyles | tCIDCtrls::EWndStyles(MCS_NOTODAY)
        , tCIDCtrls::EExWndStyles::None
        , widThis
    );

    // If asked to size to the data, then do that
    if (tCIDLib::bAllBitsOn(eCalStyle, tCIDCtrls::ECalStyles::SizeTo))
    {
        RECTL rectSize;
        ::SendMessage(hwndThis(), MCM_GETMINREQRECT, 0, tCIDCtrls::TLParam(&rectSize));

        // Resize the control now that the size values have been obtained.
        ::SetWindowPos
        (
            hwndThis()
            , NULL
            , 0
            , 0
            , rectSize.right - rectSize.left
            , rectSize.bottom - rectSize.top
            , SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER
        );

        // Set the calendar to the annual view.
        ::SendMessage(hwndThis(), MCM_SETCURRENTVIEW, 0, MCMV_MONTH);
    }

    // All standard controls must be subclassed
    SetSubclass();
}


//
//  Get the currently selected day and convert it to a time stamp in our format
//  and set up the caller's time object with this timeout.
//
tCIDLib::TVoid TCalendar::GetSelection(TTime& tmToFill)
{
    SYSTEMTIME SysTime;
    if (!::SendMessage(hwndSafe(), MCM_GETCURSEL, 0, tCIDCtrls::TLParam(&SysTime)))
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcCal_GetCurDate
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(widThis())
        );
    }

    // We need to convert it to a local time
    FILETIME FlTime;
    if (!::SystemTimeToFileTime(&SysTime, &FlTime))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDCtrls().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcCal_GetCurDate
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(widThis())
        );
    }

    // Convert that to a 64 bit int
    const tCIDLib::TCard8 c8Tmp = TRawBits::c8From32
    (
        FlTime.dwLowDateTime
        , FlTime.dwHighDateTime
    );

    // Adjusted it for our local time base and set it on the caller's time object
    tmToFill = (c8Tmp  - TKrnlTimeStamp::enctBaseOfs);
}


//
//  Set the calendar to select the indicated date. Any time component doesn't
//  matter.
//
tCIDLib::TVoid TCalendar::SetSelection(const TTime& tmToSet)
{
    // Get the incoming stamp into a file time, adjusted back to Windows units
    FILETIME FlTime;

    tCIDLib::TCard8 c8Tmp = tmToSet.enctTime() + TKrnlTimeStamp::enctBaseOfs;
    FlTime.dwLowDateTime = TRawBits::c4Low32From64(c8Tmp);
    FlTime.dwHighDateTime = TRawBits::c4High32From64(c8Tmp);

    // Convert it to a system time
    SYSTEMTIME  SysTime;
    if (!::FileTimeToSystemTime(&FlTime, &SysTime))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDCtrls().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcCal_SetCurDate
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(widThis())
        );
    }

    // And now we can pass this to the calendar
    ::SendMessage(hwndSafe(), MCM_SETCURSEL, 0, tCIDCtrls::TLParam(&SysTime));
}


