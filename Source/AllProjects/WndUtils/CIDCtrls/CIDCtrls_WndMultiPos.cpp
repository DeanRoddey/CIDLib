//
// FILE NAME: CIDCtrls_WndMultiPos.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/19/2015
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
//  This file implements the multi-window size/pos mechaniam.
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
//  CLASS: TWndMultiPos
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  Constructors and Destructor
// ---------------------------------------------------------------------------
TWndMultiPos::TWndMultiPos(const tCIDLib::TCard4 c4Count) :

    m_pData(nullptr)
{
    // Create the handle
    m_pData = ::BeginDeferWindowPos(c4Count);
}

TWndMultiPos::~TWndMultiPos()
{
    // If the handle isn't null, we have to call complete
    if (m_pData)
    {
        try
        {
            Complete();
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
    }
}


// ---------------------------------------------------------------------------
//  Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TWndMultiPos::AddWindow(const TWindow& wndToAdd, const TArea& areaNew)
{
    // Add this one to the deferred list
    tCIDLib::TCard4 c4Flags = SWP_NOZORDER
                              | SWP_NOACTIVATE
                              | SWP_NOOWNERZORDER;
    m_pData = ::DeferWindowPos
    (
        (HDWP)m_pData
        , wndToAdd.hwndSafe()
        , 0
        , areaNew.i4X()
        , areaNew.i4Y()
        , areaNew.c4Width()
        , areaNew.c4Height()
        , c4Flags
    );

    // If it failed, the we have to give up since we lost the data
    if (!m_pData)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDCtrls().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcWnd_MultiWndRepos
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
}

tCIDLib::TVoid
TWndMultiPos::AddWindow(const TWindow& wndToAdd, const TPoint& pntNew)
{
    // Add this one to the deferred list
    tCIDLib::TCard4 c4Flags = SWP_NOZORDER
                              | SWP_NOACTIVATE
                              | SWP_NOOWNERZORDER
                              | SWP_NOSIZE;

    m_pData = ::DeferWindowPos
    (
        (HDWP)m_pData
        , wndToAdd.hwndSafe()
        , 0
        , pntNew.i4X()
        , pntNew.i4Y()
        , 0
        , 0
        , c4Flags
    );

    // If it failed, the we have to give up since we lost the data
    if (!m_pData)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDCtrls().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcWnd_MultiWndRepos
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
}

tCIDLib::TVoid
TWndMultiPos::AddWindow(const TWindow& wndToAdd, const TSize& szNew)
{
    // Add this one to the deferred list
    tCIDLib::TCard4 c4Flags = SWP_NOZORDER
                              | SWP_NOACTIVATE
                              | SWP_NOOWNERZORDER
                              | SWP_NOMOVE;

    m_pData = ::DeferWindowPos
    (
        (HDWP)m_pData
        , wndToAdd.hwndSafe()
        , 0
        , 0
        , 0
        , szNew.c4Width()
        , szNew.c4Height()
        , c4Flags
    );

    // If it failed, the we have to give up since we lost the data
    if (!m_pData)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDCtrls().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcWnd_MultiWndRepos
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
}


tCIDLib::TVoid TWndMultiPos::Complete()
{
    try
    {
        ::EndDeferWindowPos((HDWP)m_pData);
    }

    catch(TError& errToCatch)
    {
        m_pData = nullptr;
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }

    catch(...)
    {
        m_pData = nullptr;
        throw;
    }

    m_pData = nullptr;
}

