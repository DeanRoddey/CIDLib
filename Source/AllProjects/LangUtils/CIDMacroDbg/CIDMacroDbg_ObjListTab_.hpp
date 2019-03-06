//
// FILE NAME: CIDMacroDbg_ObjListTab.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/23/2015
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for a common tab type which is used to dipslay object
//  information, for globals, locals and method parameters. It's just a three
//  column list, where the first column is the name of the object, the second
//  is the type and the third is the value. The third supports in place editing
//  to modify the value where that's supported.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)


// ---------------------------------------------------------------------------
//   CLASS: TObjListTab
//  PREFIX: wnd
// ---------------------------------------------------------------------------
class TObjListTab : public TTabWindow, public MIPEIntf
{
    public :
        // -------------------------------------------------------------------
        //  Public types
        //
        //  EModes
        //      We have to support members, locals, and parameters, and this
        //      is set in the ctor to tell us what we are dealing with. We will
        //      use this to insure that the correct data updating methods are
        //      called on us.
        // -------------------------------------------------------------------
        enum class EModes
        {
            Members
            , Locals
            , Parms
        };


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TObjListTab
        (
            const   TString&                strName
            , const EModes                  eMode
        );

        ~TObjListTab();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid IPEValChanged
        (
            const   tCIDCtrls::TWndId       widSrc
            , const TString&                strSrc
            , const TAttrData&              adatNew
            , const TAttrData&              adatOld
            , const tCIDLib::TCard8         c8UserId
        )   override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CreateObjListTab
        (
                    TTabbedWnd&             wndParent
            , const TString&                strTabText
            ,       TCIDMacroEngine* const  pmeToUse
        );

        tCIDLib::TVoid Reset();

        tCIDLib::TVoid SetEditOK
        (
            const   tCIDLib::TBoolean       bToSet
        );

        tCIDLib::TVoid SetRadix
        (
            const   tCIDLib::ERadices       eToSet
        );

        tCIDLib::TVoid Update
        (
                    TMEngClassVal* const    pmecvInstance
        );

        tCIDLib::TVoid Update
        (
            const   TMEngMethodImpl* const  pmethSrc
        );

        tCIDLib::TVoid Update();


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


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtaul methods
        // -------------------------------------------------------------------
        tCIDCtrls::EEvResponses eLBHandler
        (
                    TListChangeInfo&        wnotEvent
        );

        tCIDLib::TVoid ExploreValue
        (
            const   tCIDLib::TCard4         c4RowInd
            , const TMEngClassVal&          mecvVal
        );

        tCIDLib::TVoid TObjListTab::InlineEdit
        (
            const   tCIDLib::TCard4         c4RowInd
            , const TMEngClassVal&          mecvVal
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bEditOK
        //      The main window tells us when it's ok to allow for in place edit
        //      of values.
        //
        //  m_c4Col_xxx
        //      The indices for our list window columns.
        //
        //  m_colVals
        //      A temp vector to load up the three column values with. We pre
        //      set it up with three members.
        //
        //  m_eMode
        //      The mode this instance of the window is set up for.
        //
        //  m_eRadix
        //      The radix that we are to use to display cardinal or integral
        //      values (or characters if radix.)
        //
        //  m_pmeOwner
        //      A pointer to the owning macro engine. We don't own it, we just
        //      reference it.
        //
        //  m_pmecvSrc
        //      If we are in members mode then this is the value object for the
        //      main class.
        //
        //  m_pmethSrc
        //      If doing locals this is the method impl object that the main
        //      window passed to us. We can't easily get it ourselves, but he
        //      gets it from the engine. We'll also use the m_pmethiSrc object
        //      as well in this case.
        //
        //  m_pmethiSrc
        //      If doing parameters or locals, this is the method info object
        //      that got from the top of the stack, which tells us about the
        //      parameters so that we can trace up the stack and find them and
        //      it gives us the type info for the locals.
        //
        //  m_pwndList
        //      The multi-column list box that shows the information.
        //
        //  m_strmFmt
        //      A temp string stream for formatting values.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bEditOK;
        const tCIDLib::TCard4   m_c4Col_Name;
        const tCIDLib::TCard4   m_c4Col_Type;
        const tCIDLib::TCard4   m_c4Col_ValExplore;
        const tCIDLib::TCard4   m_c4Col_Value;
        tCIDLib::TStrList       m_colVals;
        EModes                  m_eMode;
        tCIDLib::ERadices       m_eRadix;
        TMEngClassVal*          m_pmecvSrc;
        TCIDMacroEngine*        m_pmeOwner;
        const TMEngMethodImpl*  m_pmethSrc;
        const TMEngMethodInfo*  m_pmethiSrc;
        TMultiColListBox*       m_pwndObjList;
        TTextStringOutStream    m_strmFmt;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TObjListTab, TTabWindow)
};


#pragma CIDLIB_POPPACK


