//
// FILE NAME: CIDAdvGraph_Window.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/19/2015
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the main graphical output window class for this facility.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
//
//  We have to bring in our internal helpers for some COM stuff and the D3D
//  headers to get the functionality we need.
// ---------------------------------------------------------------------------
#include    "CIDAdvGraph_.hpp"
#include    "CIDKernel_InternalHelpers_.hpp"
#include    <d2d1.h>
// #include    <dxgi.h>


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TCIDAdvGraphWnd,TWindow)



// ---------------------------------------------------------------------------
//  This is a structure that we use internally to hold all of the data we need to
//  keep up with.
// ---------------------------------------------------------------------------
struct TAdvGrWndData
{
    ID2D1Factory*           pDirect2dFactory;
    ID2D1HwndRenderTarget*  pRenderTarget;
};



// ---------------------------------------------------------------------------
//  CLASS: TCIDAdvGraphWnd
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDAdvGraphWnd: Constructors and Destructor
// ---------------------------------------------------------------------------
TCIDAdvGraphWnd::TCIDAdvGraphWnd() :

    TWindow(kCIDLib::False)
    , m_pData(nullptr)
{
    // Set up our internal data
    m_pData = new TAdvGrWndData{0};
}


TCIDAdvGraphWnd::~TCIDAdvGraphWnd()
{
    //
    //  Delete our internal data structure if created. The stuff it holds will already
    //  be cleaned up during the destruction of this window.
    //
    if (m_pData)
        delete m_pData;
}


// ---------------------------------------------------------------------------
//  TCIDAdvGraphWnd: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Create our window. Based on the provided configuration info, once created, we
//  will set up our graphics info.
//
tCIDLib::TVoid
TCIDAdvGraphWnd::Create(const   TWindow&                wndParent
                        , const TArea&                  areaInit
                        , const tCIDCtrls::EWndStyles   eStyles
                        , const tCIDCtrls::EExWndStyles eExStyles
                        , const tCIDCtrls::TWndId       widThis)
{
    CreateWnd
    (
        wndParent.hwndThis()
        , kCIDCtrls::pszCustClassHV
        , L""
        , areaInit
        , eStyles
        , eExStyles
        , widThis
    );

    //
    //  And now set up the resources we need. We won't store them in our
    //  data structure until we are sure it's all going to work. WE put COM
    //  janitors on all of them to insure they clean up if we fail.
    //
    HRESULT hRes = 0;
    ID2D1Factory* pDirect2dFactory = nullptr;
    ID2D1HwndRenderTarget* pRenderTarget = nullptr;


    if (FAILED(hRes))
    {
        TKrnlError::SetLastHostError(hRes);
        facCIDAdvGraph().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kAdvGrErrs::errcRes_Create
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TString(L"buffer/swap chain")
        );
    }


}




// ---------------------------------------------------------------------------
//  TCIDAdvGraphWnd: Protected, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TBoolean TCIDAdvGraphWnd::bCreated()
{
    TParent::bCreated();


    // Allow creation to continue
    return kCIDLib::True;
}


tCIDLib::TVoid TCIDAdvGraphWnd::Destroyed()
{
    // Cleanup any of our resources if they are allocated
    if (m_pData)
    {
        if (m_pData->pRenderTarget)
        {
            m_pData->pRenderTarget->Release();
            m_pData->pRenderTarget = nullptr;
        }


        if (m_pData->pDirect2dFactory)
        {
            m_pData->pDirect2dFactory->Release();
            m_pData->pDirect2dFactory = nullptr;
        }
    }

    TParent::Destroyed();
}


