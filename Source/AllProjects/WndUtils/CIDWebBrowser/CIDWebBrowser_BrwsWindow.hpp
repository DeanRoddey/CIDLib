//
// FILE NAME: CIDWebBrowser_BrwsWindow.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/10/2005
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
//  This is the header file for the CIDWebBrowser_BrwsWindow.cpp file. This
//  file implements the browser window class. This window will contain a
//  browser that completely covers it.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

struct TBrwsWndData;

// ---------------------------------------------------------------------------
//  CLASS: TWebBrowserWnd
// PREFIX: brwsw
// ---------------------------------------------------------------------------
class CIDWBRWSEXP TWebBrowserWnd : public TWindow
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TWebBrowserWnd();

        ~TWebBrowserWnd();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bFlag() const;

        tCIDLib::TBoolean bFlag
        (
            const   tCIDLib::TBoolean       bToSet
        );

        tCIDLib::TBoolean bQueryOpt
        (
            const   tWebBrowser::EBoolOpts  eOpt
        )   const;

        tCIDLib::TBoolean bReadyForCleanup
        (
            const   tCIDLib::TEncodedTime   enctTest
        )   const;

        tCIDLib::TVoid CreateBrwsWindow
        (
            const   TWindow&                wndParent
            , const tCIDCtrls::EWndStyles   eWndStyles
            , const TArea&                  areaInit = TArea::Nul_TArea()
            , const tCIDCtrls::TWndId       widToUse = 0
        );

        tCIDLib::TVoid DoNavOp
        (
            const   tWebBrowser::ENavOps    eOp
        );

        tCIDLib::TVoid SetCleanupStamp
        (
            const   tCIDLib::TCard4         c4Secs
        );

        tCIDLib::TVoid SetOpt
        (
            const   tWebBrowser::EBoolOpts  eOpt
            , const tCIDLib::TBoolean       bToSet
        );

        tCIDLib::TVoid ShowHTML
        (
            const   TString&                strShow
        );

        tCIDLib::TVoid SetURL
        (
            const   TString&                strToSet
        );

        tCIDLib::TVoid Terminate();


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AreaChanged
        (
            const   TArea&                  areaPrev
            , const TArea&                  areaNew
            , const tCIDCtrls::EPosStates   ePosState
            , const tCIDLib::TBoolean       bOrgChanged
            , const tCIDLib::TBoolean       bSizeChanged
            , const tCIDLib::TBoolean       bStateChanged
        )   override;

        tCIDLib::TBoolean bCreated() override;

        tCIDLib::TBoolean bEraseBgn
        (
                    TGraphDrawDev&          gdevToUse
        )   override;

        tCIDLib::TBoolean bPaint
        (
                    TGraphDrawDev&          gdevToUse
            , const TArea&                  areaUpdate
        )   override;

        tCIDLib::TVoid Destroyed() override;


    private :
        // -------------------------------------------------------------------
        //  Private data
        //
        //  m_bFlag
        //      A flag for use by the application.
        //
        //  m_enctCleanup
        //      Sometimes applications want to clean these up async, and to wait a while
        //      before doing so. Otherwise, sometimes it clearly wrecks somethign in the
        //      browser engine. So we provide this stamp and a couple methods to set and
        //      test it.
        //
        //  m_pEmbedInfo
        //      When we embed, we get back a pointer to some OLE site data which we just
        //      store opaquely and pass back in to any subsequent calls. When we unembed,
        //      this will be nulled back out. We don't own the data, the embedding code
        //      will clean it up when we unembed, which we must do before being destroyed,
        //      if not already done manually by client code.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bFlag;
        tCIDLib::TEncodedTime   m_enctCleanup;
        tCIDLib::TVoid*         m_pEmbedInfo;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TWebBrowserWnd,TWindow)
};

#pragma CIDLIB_POPPACK


