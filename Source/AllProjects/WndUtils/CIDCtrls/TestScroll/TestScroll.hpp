//
// FILE NAME: TestCIDCtrls2.Hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/28/97
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the main header of the program. It brings in any needed
//  underlying headers, and any program headers.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
//  Includes
// ----------------------------------------------------------------------------
#include    "CIDCtrls.hpp"


// ----------------------------------------------------------------------------
//  Global data from TestCIDCtrls2.Cpp
// ----------------------------------------------------------------------------
extern TGUIFacility facTestCIDCtrls2;



// ----------------------------------------------------------------------------
//   CLASS: TTestFrameWnd
//  PREFIX: wnd
// ----------------------------------------------------------------------------
class TTestFrameWnd : public TFrameWnd
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TTestFrameWnd();

        TTestFrameWnd(const TTestFrameWnd&) = delete;

        ~TTestFrameWnd();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TTestFrameWnd& operator=(const TTestFrameWnd&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CreateMain();


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

        tCIDLib::TBoolean bClick
        (
            const   tCIDCtrls::EMouseButts  eButton
            , const tCIDCtrls::EMouseClicks eClickType
            , const TPoint&                 pntAt
            , const tCIDLib::TBoolean       bCtrlShift
            , const tCIDLib::TBoolean       bShift
        )   override;

        tCIDLib::TBoolean bCreated() override;

        tCIDLib::TBoolean bPaint
        (
                    TGraphDrawDev&          gdevToUse
            , const TArea&                  areaUpdate
        )   override;

        tCIDLib::TVoid Destroyed() override;

        tCIDLib::TVoid Timer
        (
            const   tCIDCtrls::TTimerId     tmidSrc
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virutal methods
        // -------------------------------------------------------------------


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pwndTest
        // -------------------------------------------------------------------
        TArea               m_areaBmp;
        tCIDLib::TBoolean   m_bPaused;
        TBitmap             m_bmpTest;
        tCIDLib::TInt4      m_i4XOfs;
        tCIDLib::TInt4      m_i4YOfs;
        tCIDCtrls::TTimerId m_tmidMove;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTestFrameWnd, TFrameWnd)
};

