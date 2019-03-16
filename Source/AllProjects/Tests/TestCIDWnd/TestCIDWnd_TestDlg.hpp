//
// FILE NAME: TestCIDWnd_TestDlg.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/06/2001
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
//  This is the header for a test dialog class
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  CLASS: TTestDlg
// PREFIX: dlg
// ---------------------------------------------------------------------------
class TTestDlg : public TDlgBox
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TTestDlg();

        ~TTestDlg();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bProcessGestEv
        (
            const   tCIDWnd::EGestEvs       eEv
            , const TPoint&                 pntStart
            , const TPoint&                 pntAt
            , const TPoint&                 pntDelta
            , const tCIDLib::TBoolean       bTwoFingers
        );

        tCIDLib::TVoid Clicked
        (
            const   TPoint&                 pntAt
        );

        tCIDLib::TVoid GestInertiaIdle
        (
            const   tCIDLib::TEncodedTime   enctEnd
            , const tCIDLib::TCard4         c4Millis
            , const tCIDLib::EDirs          eDir
        );

        tCIDLib::TVoid GestReset
        (
            const   tCIDWnd::EGestReset     eType
        );

        tCIDLib::TVoid PerGestOpts
        (
            const   TPoint&                 pntAt
            , const tCIDLib::EDirs          eDirection
            , const tCIDLib::TBoolean       bTwoFingers
            ,       tCIDWnd::EGestOpts&     eGestOpts
            ,       tCIDLib::TFloat4&       f4VScale
        );

        tCIDLib::TVoid ProcessFlick
        (
            const   tCIDLib::EDirs          eDirection
            , const TPoint&                 pntStartPos
        );



        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bRunDlg
        (
                    TFrameWindow&           wndOwner
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bPreLoop();

        tCIDLib::TVoid Paint
        (
                    TGraphDrawDev&          gdevToUse
            , const TArea&                  areaUpdate
        );


    private :
        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        TTestDlg(const TTestDlg&);
        tCIDLib::TVoid operator=(const TTestDlg&);


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AdjustCircle
        (
                    TGraphDrawDev&          gdevToUse
            , const TPoint&                 pntDelta
        );

        tCIDWnd::EEventResponses eClickHandler
        (
                    TButtClickInfo&         wnotEvent
        );

        tCIDLib::TVoid MoveCircle
        (
                    TGraphDrawDev&          gdevToUse
            , const TPoint&                 pntNew
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_areaDisplay
        //      The area we have available to display our bitmap
        //
        // -------------------------------------------------------------------
        TArea           m_areaDisplay;
        TPoint          m_pntLast;
        TTextOutWnd2*   m_pwndList;
        TPushButton*    m_pwndClose;
        TString         m_strTmpMsg;

        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TTestDlg,TDlgBox)
};

