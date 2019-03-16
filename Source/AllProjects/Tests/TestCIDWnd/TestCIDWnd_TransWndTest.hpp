//
// FILE NAME: TestCIDWnd_TransWndTest.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/17/2005
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
//  This is the header for the TestCIDWnd_TransWndTest.Cpp file. This guy
//  implements a trivial derivative of the layered window class, for testing
//  layered window functionality.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  CLASS: TTransWindow
// PREFIX: wnd
// ---------------------------------------------------------------------------
class TTransWindow : public TLayeredWindow
{
    public  :
        // -------------------------------------------------------------------
        // Constructors and destructor
        // -------------------------------------------------------------------
        TTransWindow();

        ~TTransWindow();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Create
        (
            const   TFrameWindow&           wndOwner
            , const TBitmap&                bmpShow1
            , const TBitmap&                bmpShow2
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid DrawLayered
        (
                    TGraphDrawDev&          gdevToUse
        );

        tCIDLib::TVoid Timer
        (
            const   tCIDWnd::ETimers        eTimer
        );


    private :
        // -------------------------------------------------------------------
        //
        // -------------------------------------------------------------------
        tCIDLib::TCard4 m_c4Frame;
        TBitmap         m_bmpShow1;
        TBitmap         m_bmpShow2;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMyWindow,TLayeredWindow)
};


