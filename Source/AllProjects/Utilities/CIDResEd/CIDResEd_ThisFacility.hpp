//
// FILE NAME: CIDResEd_ThisFacility.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/10/2000
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
//  This is the header for the CIDResEd_ThisFacility.cpp file, which implements
//  the facility class for this program. Since this is a GUI based program, it
//  derives from the TGUIFacility class, instead of the base TFacility class.
//
//  There are a lot of windows involved in this program, and many of them have to
//  know about each other in some way or another. To avoid a rat's nest of pointers
//  to things, this facility class acts as hookup central. Each of the important
//  windows is stored here and other windows call here to get access to them.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//



// ---------------------------------------------------------------------------
//   CLASS: TFacCIDResEd
//  PREFIX: fac
// ---------------------------------------------------------------------------
class TFacCIDResEd : public TGUIFacility
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFacCIDResEd();

        ~TFacCIDResEd();


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        tCIDLib::EExitCodes eMainThread
        (
                    TThread&                thrThis
            ,       tCIDLib::TVoid*         pData
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4NextEditId();

        tCIDResEd::EMsgTypes eParseTextSym
        (
            const   TString&                strText
            ,       TString&                strRealSym
        )   const;

        const TString& strBoolVal
        (
            const   tCIDLib::TBoolean       bVal
        )   const;

        const TString& strWndStyleOn
        (
            const   tCIDCtrls::EWndStyles   eItemStyles
            , const tCIDCtrls::EWndStyles   eToCheck
        )   const;

        TResEdMainFrame& wndMain();


    private :
        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        TFacCIDResEd(const TFacCIDResEd&);
        tCIDLib::TVoid operator=(const TFacCIDResEd&);


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4NextEdId
        //      Because it's possible to delete dialogs while one is being edited
        //      or change the resource id of a dialog, we need some way to link
        //      back from the thing edited to where it goes in the list, so that
        //      we can save the changes back. So each dailog object is assigned
        //      a running id that keeps going up. This can be used to search for
        //      the correct index in the list, even if the list has changed.
        //
        //  m_wndMainFrame
        //      Our main frame window, which kind of runs the show. Its a
        //      derivative of the basic frame window class.
        // -------------------------------------------------------------------
        tCIDLib::TCard4     m_c4NextEdId;
        TString             m_strVal_False;
        TString             m_strVal_True;
        TResEdMainFrame     m_wndMainFrame;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TFacCIDResEd,TGUIFacility)
};


