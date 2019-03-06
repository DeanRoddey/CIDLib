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

#include    "CIDCtrls_ColHeader.hpp"
#include    "CIDCtrls_MColListBox.hpp"

#include    "TestCIDCtrls_ResourceIds.hpp"



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

        tCIDLib::TBoolean bCreated() override;

        tCIDLib::TVoid Destroyed() override;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virutal methods
        // -------------------------------------------------------------------
        tCIDCtrls::EEvResponses eHdrHandler
        (
                    TColHdrChangeInfo&      wnotEvent
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pwndTest
        // -------------------------------------------------------------------
        TStaticText*        m_pwndOutput;
        TMColListBox*       m_pwndTest;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTestFrameWnd, TFrameWnd)
};

