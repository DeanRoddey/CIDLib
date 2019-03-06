//
// FILE NAME: CIDCtrls_TrayAppWnd.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/03/2012
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDWnd_TrayAppWnd.cpp module, which implements
//  the TTrayAppWindow class. This class provides support for the standard
//  Windows types tray app window. We are normally invisible but ask to have
//  an icon registered in the Windows system tray. When clicked on we will
//  display our window. We also support setting of some info in the mouse over
//  popup bubble display.
//
//  To implement an actual tray app, you would derive from this class and
//  handle the virtual methods to initialize and destroy the window, and to
//  show the menu. This class will translate the notifications that map to
//  keys and pass them to the underlying window class so they will show up
//  as regular key stroke virtual callbacks that you can override as well.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)


struct _NOTIFYICONDATAW;

// ---------------------------------------------------------------------------
//  CLASS: TTrayAppWnd
// PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TTrayAppWnd : public TWindow
{
    public  :
        // -------------------------------------------------------------------
        // Constructors and Destructor
        // -------------------------------------------------------------------
        ~TTrayAppWnd();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CreateTrayApp
        (
            const   TArea&                  areaInit
            , const TIcon&                  iconTray
            , const TString&                strText
        );

        tCIDLib::TVoid ShowBalloonText
        (
            const   TString&                strTitle
            , const TString&                strText
            , const tCIDLib::TBoolean       bError = kCIDLib::False
            , const tCIDLib::TCard4         c4TimeoutMSs = 5000
        );

        tCIDLib::TVoid SetTipText
        (
            const   TString&                strToSet
        );


    protected :
        // -------------------------------------------------------------------
        //  Hidden Constructors
        // -------------------------------------------------------------------
        TTrayAppWnd();


        // -------------------------------------------------------------------
        //  Protected, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bShowTrayMenu
        (
            const   TPoint&                 pntAt
        ) = 0;

        virtual tCIDLib::TVoid CleanupTrayApp() = 0;

        virtual tCIDLib::TVoid InitTrayApp() = 0;

        virtual tCIDLib::TVoid Invoked() = 0;


        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCreated() override;

        tCIDLib::TBoolean bPaint
        (
                    TGraphDrawDev&          gdevToUse
            , const TArea&                  areaUpdate
        )   override;

        tCIDLib::TVoid Destroyed() override;

        tCIDLib::TVoid UserMsg
        (
            const   tCIDCtrls::TWParam      wParam
            , const tCIDCtrls::TLParam      lParam
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Unimplemented
        // -------------------------------------------------------------------
        TTrayAppWnd(const TTrayAppWnd&);
        tCIDLib::TVoid operator=(const TTrayAppWnd&);


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid DoInfoSetup
        (
                    _NOTIFYICONDATAW&       ToFill
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_hicoTray
        //      The handle of the icon we want to show in the tray.
        //
        //  m_strTipText
        //      If the derived class wants us to display tip text during
        //      mouse over, it can set this value. We'll make sure it's set
        //      on the tray whenever we get the call to set it and any time
        //      we update the tray for other reasons.
        // -------------------------------------------------------------------
        tCIDGraphDev::TIconHandle   m_hicoTray;
        TString                     m_strTipText;


        // -------------------------------------------------------------------
        //  Magic macors
        // -------------------------------------------------------------------
        RTTIDefs(TTrayAppWnd,TWindow)
};

#pragma CIDLIB_POPPACK



