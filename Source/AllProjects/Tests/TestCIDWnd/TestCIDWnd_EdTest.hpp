//
// FILE NAME: TestCIDWnd_EdTest.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/25/2014
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
//  This is the header for a simple dialog to test our line editor window.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  CLASS: TEdTestDlg
// PREFIX: dlg
// ---------------------------------------------------------------------------
class TEdTestDlg : public TDlgBox
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TEdTestDlg();

        ~TEdTestDlg();



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

        tCIDLib::TVoid Destroyed();

        tCIDLib::TVoid Paint
        (
                    TGraphDrawDev&          gdevToUse
            , const TArea&                  areaUpdate
        );

        tCIDLib::TVoid SetChildFocus();


    private :
        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        TEdTestDlg(const TEdTestDlg&);
        tCIDLib::TVoid operator=(const TEdTestDlg&);



        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_areaDisplay
        //      The area we have available to display our window
        //
        //  m_wndEd
        //      The test editor.
        // -------------------------------------------------------------------
        TArea           m_areaDisplay;
        TLineEditorWnd  m_wndEd;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TEdTestDlg,TDlgBox)
};


