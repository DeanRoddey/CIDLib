//
// FILE NAME: TestGesture_MainFrame.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/19/2015
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
//  This is the header for the main window of this test program. We define a simple
//  main frame and a touch window which we set as it's client window.
//
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  CLASS: TTestGest
// PREFIX: wnd
// ---------------------------------------------------------------------------
class TTestGest : public TTouchWnd
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TTestGest();

        TTestGest(const TTestGest&) = delete;

        ~TTestGest();


    protected :
        // -------------------------------------------------------------------
        //  Protected, inheritd methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bPaint
        (
                    TGraphDrawDev&          gdevToUse
            , const TArea&                  areaUpdate
        )   override;

        tCIDLib::TBoolean bProcessGestEv
        (
            const   tCIDCtrls::EGestEvs     eEv
            , const TPoint&                 pntStart
            , const TPoint&                 pntAt
            , const TPoint&                 pntDelta
            , const tCIDLib::TBoolean       bTwoFingers
        )   override;

        tCIDLib::TVoid GestClick
        (
            const   TPoint&                 pntAt
        )   override;

        tCIDLib::TVoid GestReset
        (
            const   tCIDCtrls::EGestReset   eType
        )   override;

        tCIDLib::TVoid PerGestOpts
        (
            const   TPoint&                 pntAt
            , const tCIDLib::EDirs          eDir
            , const tCIDLib::TBoolean       bTwoFingers
            ,       tCIDCtrls::EGestOpts&   eOpts
            ,       tCIDLib::TFloat4&       f4VScale
        )   override;

        tCIDLib::TVoid GestFlick
        (
            const   tCIDLib::EDirs          eDirection
            , const TPoint&                 pntStartPos
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AdjustIndicator
        (
                    TGraphDrawDev&          gdevTar
            , const TPoint&                 pntDelta
        );

        tCIDLib::TVoid MoveIndicator
        (
                    TGraphDrawDev&          gdevTmp
            , const TPoint&                 pntNew
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_gbrFill
        //      Used to fill our little tracking circle.
        //
        //  m_pntLast
        //      The last position we drew the circle at
        // -------------------------------------------------------------------
        TSolidBrush     m_gbrFill;
        TPoint          m_pntLast;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTestGest, TTouchWnd);
};



// ---------------------------------------------------------------------------
//  CLASS: TGestMainWnd
// PREFIX: wnd
// ---------------------------------------------------------------------------
class TGestMainWnd : public TFrameWnd
{
    public  :
        // -------------------------------------------------------------------
        // Constructors and destructor
        // -------------------------------------------------------------------
        TGestMainWnd();

        ~TGestMainWnd();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CreateFrame();


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

        tCIDLib::TVoid Destroyed() override;

        tCIDLib::TVoid MenuCommand
        (
            const   tCIDLib::TResId         ridItem
            , const tCIDLib::TBoolean       bChecked
            , const tCIDLib::TBoolean       bEnabled
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pwndClient
        // -------------------------------------------------------------------
        TTestGest*  m_pwndClient;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TGestMainWnd,TFrameWnd)
};


